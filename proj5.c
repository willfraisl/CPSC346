/*
Class: CPSC 346-02
Team Member 1: Will Fraisl
Team Member 2: N/A 
GU Username of project lead: wfraisl
Pgm Name: proj5.c
Pgm Desc: forks off a process and protects two pretend critical sections using Peterson 
Usage: ./a.out time_parent time_child time_parent_non_cs time_child_non_cs
    where:
    time_parent is an integer value defining how long the parent is to sleep in its critical section
    time_child is an integer value defining how long the child is to sleep in its critical section
    time_parent_non_cs is an integer value defining how long the parent is to sleep in its non-critical section
    time_child_non_cs is an integer value defining how long the child is to sleep in its non-critical section
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void parent(int*, int*, int*, int, int);
void child(int*, int*, int*, int, int);
void cs(char, int);
void non_crit_sect(int);

int main(int argc, char *argv[]){
    pid_t pid;
    int shmid_turn;
    int shmid_pr_0;
    int shmid_pr_1;
    int* turn;
    int* pr_0;
    int* pr_1;

    int time_parent;
    int time_child;
    int time_parent_non_cs;
    int time_child_non_cs;

    //check for correct usage
    if(argc == 1){
        time_parent = 1;
        time_child = 1;
        time_parent_non_cs = 1;
        time_child_non_cs = 4;
    }
    else if(argc == 5){
        time_parent = atoi(argv[1]);
        time_child = atoi(argv[2]);
        time_parent_non_cs = atoi(argv[3]);
        time_child_non_cs = atoi(argv[4]);
    }
    else{
        printf("Usage: ./a.out <time_parent> <time_child> <time_parent_non_cs> <time_child_non_cs>");
        exit(1);
    }

    //create a shared memory segment
    shmid_turn = shmget(0,sizeof(int),0777 | IPC_CREAT);
    shmid_pr_0 = shmget(0,sizeof(int),0777 | IPC_CREAT);
    shmid_pr_1 = shmget(0,sizeof(int),0777 | IPC_CREAT);


    //attach it to the process
    turn = shmat(shmid_turn,0,0);
    pr_0 = shmat(shmid_pr_0,0,0);
    pr_1 = shmat(shmid_pr_1,0,0);

    //set initial values
    *turn = 0;
    *pr_0 = 0;
    *pr_1 = 0;

    //fork and run
    pid = fork();
    if(pid < 0){
        printf("Fork Error");
        exit(1);
    }
    else if(pid == 0){
        child(turn, pr_0, pr_1, time_child, time_child_non_cs);
        shmdt(turn);
        shmdt(pr_0);
        shmdt(pr_0);
    }
    else{
        parent(turn, pr_0, pr_1, time_parent, time_parent_non_cs);
        wait(NULL);
        shmdt(turn);
        shmdt(pr_0);
        shmdt(pr_1);
        shmctl(shmid_turn,IPC_RMID,0);
        shmctl(shmid_pr_0,IPC_RMID,0);
        shmctl(shmid_pr_1,IPC_RMID,0);
    }
}

void parent(int* turn, int* pr_0, int* pr_1, int time_crit_sect, int time_non_crit_sect){
    for (int i = 0; i < 10; i++){
        *pr_0 = 1;
        *turn = 1;
        while(*pr_1 && *turn);
        cs('p', time_crit_sect);
        *pr_0 = 0;
        non_crit_sect(time_non_crit_sect); 
    }
}

void child(int* turn, int* pr_0, int* pr_1, int time_crit_sect, int time_non_crit_sect){
    for (int i = 0; i < 10; i++){
        *pr_1 = 1;
        *turn = 0;
        while(*pr_0 && !*turn);
        cs('c', time_crit_sect);
        *pr_1 = 0;
        non_crit_sect(time_non_crit_sect); 
    }
}

void cs(char process, int time_crit_sect){
    if (process == 'p'){
        printf("parent in critical section\n");
        sleep(time_crit_sect);
        printf("parent leaving critical section\n");
    }
    else{
        printf("child in critical section\n");
        sleep(time_crit_sect);
        printf("child leaving critical section\n");
    }
}

void non_crit_sect(int time_non_crit_sect){
    sleep(time_non_crit_sect);
}