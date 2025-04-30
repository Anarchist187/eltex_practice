#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#define RAND_MAX_CNT 10//+2
#define SHARED_MEM_SIZE 512

typedef struct{
    int vals[100];
    int nums_count;
    int min_val;
    int max_val;
    int generated;
    int processed;
} SharedMem;

volatile sig_atomic_t flag_running = 1;

void sigint(int sig){
    flag_running=0;
}

int main(){
    int shmid,sets_amount;
    key_t key;
    pid_t pid;
    SharedMem *shared_mem;
    key=ftok("mymem",187);
    shmid = shmget(key,SHARED_MEM_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (shmget==(void*)-1){
        perror ("Error shmget");
        exit(EXIT_FAILURE);
    }
    shared_mem=(SharedMem*)shmat(shmid,NULL,0);
    if (shared_mem==(void*)-1){
        perror("Error shmat");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT,sigint);
    srand(time(NULL));
    sets_amount=0;
    shared_mem->generated=0;
    shared_mem->processed=0;
    while(flag_running){
        shared_mem->nums_count=2+rand()% RAND_MAX_CNT;
        printf("Amount of generated numbers: %d\n",shared_mem->nums_count);
        for (int i=0;i<shared_mem->nums_count;i++){
            shared_mem->vals[i]=rand()%1000;
            printf("%d: %d\n",i,shared_mem->vals[i]);
        }
        shared_mem->generated=1;
        switch (pid=fork()){
            case -1:
                perror("Error fork");
                    if (shmdt(shared_mem)==-1){
                        perror("Error shmdt");
                        exit(EXIT_FAILURE);
                    }
                exit(EXIT_FAILURE);
            case 0:
                if (shared_mem->generated){
                    shared_mem->min_val=shared_mem->max_val=shared_mem->vals[0];
                    for (int i=1;i<shared_mem->nums_count;i++){
                        if (shared_mem->min_val>shared_mem->vals[i]){
                            shared_mem->min_val=shared_mem->vals[i];
                        }
                        if (shared_mem->max_val<shared_mem->vals[i]){
                            shared_mem->max_val=shared_mem->vals[i];
                        }
                    }
                    shared_mem->processed=1;
                    exit(EXIT_SUCCESS);
                }
                default:
                    if (flag_running && !shared_mem->processed){
                        usleep(1000);
                    }
                    if (flag_running){
                        printf("Parent reply: min = %d, max = %d\n",shared_mem->min_val, shared_mem->max_val);
                        sets_amount++;
                        shared_mem->processed=0;
                        shared_mem->generated=0;
                    }
        }
    }
    printf("Sets processed: %d\n",sets_amount);
    if (shmdt(shared_mem)==-1){
        perror("Error shmdt");
        exit(EXIT_FAILURE);
    }
    

}