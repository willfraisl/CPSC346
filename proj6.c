/*
Class: CPSC 346-02
Team Member 1: Will Fraisl
Team Member 2: N/A 
GU Username of project lead: wfraisl
Pgm Name: proj6.c
Pgm Desc: semaphore example 
Usage: ./a.out
Due Date: 10/16/18
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_MUTEX 1 
#define INIT_EMPTY 100 
#define INIT_FULL  0 
typedef struct sembuf sem_buf;

void set_sem_buf(sem_buf[],int);
int create_semaphore(int);
void set_sem_values(int,int,int);
void print_sem_values(int,int,int);
void producer(sem_buf[],sem_buf[],int,int,int);
void consumer(sem_buf[],sem_buf[],int,int,int);
void critical_section(char);

int main(int argc, char* argv[]){
    sem_buf s_wait[1], s_signal[1]; //create semaphore structs
    int mutex, empty, full;   //will hold semaphore identifiers
    int value;

    //set wait/signal structs used in semop 
    set_sem_buf(s_wait,-1);
    set_sem_buf(s_signal,1);

    //create semaphore sets using arbitrary int unique to the semaphore set. 
    mutex = create_semaphore(0);
    empty = create_semaphore(1);
    full = create_semaphore(2);

    //set semaphores to initial values
    set_sem_values(mutex, empty, full);

    printf("Initial semaphore values:\n");
    print_sem_values(mutex, empty, full);

    //fork off a process
    if ((value = fork()) < 0){
        perror("fork");
    }
    else if(value == 0){  //child
        consumer(s_wait, s_signal, mutex, empty, full);
        printf("Final semaphore values:\n");
        print_sem_values(mutex, empty, full);
    }
    else{   //parent
        
        producer(s_wait, s_signal, mutex, empty, full);
        wait(NULL); //wait for child to exit
    }

    
}

//set struct values for down/up ops to be used in semop
//By design, we will always refer to the 0th semaphore in the set
void set_sem_buf(sem_buf sem[], int op_val){
    sem[0].sem_num = 0;
    sem[0].sem_op = op_val;
    sem[0].sem_flg = SEM_UNDO;
}

//create a semaphore set of 1 semaphore specified by key 
int create_semaphore(int key){
    int new_sem;
    if ((new_sem = semget(key, 1, 0777 | IPC_CREAT)) == -1){
        perror("semget failed");
        exit(1);
    }
    return new_sem;
}

//set mutex, empty, full to initial values
//these semaphore sets consist of a single semaphore, the 0th 
void set_sem_values(int mutex, int empty, int full)
{
    semctl(mutex, 0, SETVAL, INIT_MUTEX);
    semctl(empty, 0, SETVAL, INIT_EMPTY);
    semctl(full, 0, SETVAL, INIT_FULL);
}

//retrieve value held by the 0th semaphore, which is the second arg.
//0 in the final argument indicates that it is not used 
void print_sem_values(int mutex, int empty, int full)
{
    int m, e, f; 
    m = semctl(mutex, 0, GETVAL, 0);
    e = semctl(empty, 0, GETVAL, 0);
    f = semctl(full, 0, GETVAL, 0);
    printf("mutex: %d empty: %d full: %d\n", m,e,f);
}

void producer(sem_buf s_wait[0] ,sem_buf s_signal[0] ,int mutex, int empty, int full){
    int i;
    for (i = 0; i < 5; i++){
        semop(empty, s_wait, 1);
        semop(mutex, s_wait, 1);
        critical_section('P');
        semop(mutex, s_signal, 1);
        semop(full, s_signal, 1); //experiment with putting delays after this line
        usleep(10000);
    }
}

void consumer(sem_buf s_wait[0] ,sem_buf s_signal[0] ,int mutex, int empty, int full){
    int i;
    for (i = 0; i < 5; i++){
        semop(full, s_wait, 1);
        semop(mutex, s_wait, 1);
        critical_section('C');
        semop(mutex, s_signal, 1);
        semop(empty, s_signal, 1); //experiment with putting delays after this line
        usleep(100000);
    }    
}

void critical_section(char who){
    if (who == 'P'){
        printf("Producer making an item\n");
    }
    else{
        printf("Consumer consuming an item\n");
    }
}