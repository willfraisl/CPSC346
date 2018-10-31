/*
Will Fraisl
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4

void* thread_func(void*);
int is_prime(int);

struct params{
    void* tid;
    int num_to_create;
    int* pipe;
    char type;
};
typedef struct params param;

int main(int argc, char* argv[]){
    pthread_t threads[NUM_THREADS];
    int status, i;
    param p;
    int fd[2];

    //check for correct usage
    if(argc == 2){
        p.num_to_create = atoi(argv[1]);
    }
    else{
        printf("usage: ./a.out <num_to_create>\n");
        return -1;
    }

    //preparing random number
    srand(time(0));

    //pipe
    pipe(fd);
    p.pipe = fd;

    //creating write thread
    printf("In Main.  Creating read thread %d\n", 0);
    p.tid = (void*)0;
    p.type = 'w';
    status = pthread_create(&threads[0], NULL, thread_func, (void*)&p );
    sleep(1);

    //create read threads
    for (i = 1; i < NUM_THREADS; i++){
        printf("In Main.  Creating read thread %d\n", i);
        p.tid = (void*)i;
        p.type = 'r';
        status = pthread_create(&threads[i], NULL, thread_func, (void*)&p );
    }

    //join all threads
    for (i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i],NULL); //wait for thread[i] to complete
    }

    return 0; 
}

void* thread_func(void* param_in){
    param* p = (param*)param_in;
    printf("Hello from thread %c\n",p->type);
    if((char)p->type == 'w'){ //write thread
        printf("Hello from write thread %d\n",p->tid);
        int nums_generated = 0;
        int num_to_add;
        //open write end of pipe
        open(p->pipe[0]);
        printf("pipe open\n");
        while(nums_generated < (int)p->num_to_create){
            //generate random num
            num_to_add = rand();
            //write to pipe
            write(p->pipe[1], num_to_add, sizeof(int));
            //display generated number
            printf("Just added %d to pipe\n", num_to_add);
            nums_generated++;
        }
        //close pipe
    }
    else{   //read thread
        int num_read;
        while((read((int)p->pipe[0], num_read, sizeof(int))) > 0){
            if(is_prime(num_read)){
                //display that number is prime and tid
            }
            printf("Just read %d from pipe\n", num_read);
        }
    }
    pthread_exit(NULL);
}

int is_prime(int num){
    int i = 2;
    while (i < num){
        if (num % i == 0){
            return 0;
        }
        i++;
    }
    return 1;
}