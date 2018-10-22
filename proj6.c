#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

const int PROD = 0;
const int CONS = 1;
typedef struct sembuf sem_buf;

int main(int argc, char* argv[]){
    sem_buf s_wait[1], s_signal[1]; //create semaphore structs
    semapore mutex(1), empty(100), full(0);
    printf("Initial semaphore values: mutex = %d, empty = %d, full = %d\n", mutex, empty, full);

    parbegin{
        prod();
        cons();
    }
    parend

        printf("Final semaphore values: mutex = %d, empty = %d, full = %d\n", mutex, empty, full);
}

producer(){
    int i;
    for (i = 0; i < 5; i++){
        empty.down();
        mutex.down();
        criticalSection(PROD);
        mutex.up();
        full.up(); //experiment with putting delays after this line
    }
}

consumer(){
    int i;
    for (i = 0; i < 5; i++){
        full.down();
        mutex.down();
        criticalSection(CONS);
        mutex.up();
        empty.up(); //experiment with putting delays after this line
    }
}

criticalSection(int who){
    if (who == PROD){
        printf("Producer making an item\n");
    }
    else{
        printf("Consumer consuming an item\n");
    }
}
