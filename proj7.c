/*
Class: CPSC 346-02
Team Member 1: Will Fraisl
Team Member 2: N/A 
GU Username of project lead: wfraisl
Pgm Name: proj7.c
Pgm Desc: using pthreads and a pipe to check if a number is prime 
Usage: ./a.out <num_to_create> -pthreads
Due Date: 11/2/18
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#define NUM_THREADS 4

void* thread_func(void*);
int is_prime(int);

struct params{
    int tid;
    int num_to_create;
    int pipe[2];
    char type;
};
typedef struct params param;

int main(int argc, char* argv[]){
    pthread_t threads[NUM_THREADS];
    int status, i;
    param p[4];
    int fd[2];

    //check for correct usage
    if(argc == 3){
        for(i=0; i<4; i++){
            p[i].num_to_create = atoi(argv[1]);
        }
    }
    else{
        printf("usage: ./a.out <num_to_create> -pthreads\n");
        return -1;
    }

    //preparing random number
    srand(time(0));

    //pipe
    pipe(fd);
    for(i=0; i<4; i++){
        p[i].pipe[0] = fd[0];
        p[i].pipe[1] = fd[1];
    }

    //creating write thread
    p[0].tid = 0;
    p[0].type = 'w';
    status = pthread_create(&threads[0], NULL, thread_func, (void*)&p );
    //sleep(1);

    //create read threads
    for (i = 1; i < NUM_THREADS; i++){
        p[i].tid = i;
        p[i].type = 'r';
        status = pthread_create(&threads[i], NULL, thread_func, (void*)&p[i] );
    }

    //join all threads
    for (i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i],NULL); //wait for thread[i] to complete
    }

    return 0; 
}

void* thread_func(void* param_in){
    param* p = (param*)param_in;
    if((char)p->type == 'w'){ //write thread
        int nums_generated = 0;
        int num_to_add;

        while(nums_generated < (int)p->num_to_create){
            //generate random num
            num_to_add = rand();
            //write to pipe
            write(p->pipe[1], &num_to_add, sizeof(int));
            //display generated number
            printf("Generated: %d\n", num_to_add);
            nums_generated++;
            sleep(1);
        }
        close(p->pipe[1]);
    }
    else{   //read thread
        int num_read;
        while((read((int)p->pipe[0], &num_read, sizeof(int))) > 0){
            usleep(10000);
            if(is_prime(num_read)){
                //display that number is prime and tid
                printf("Reader %d: %d is prime\n", (int)p->tid, num_read);
            }
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