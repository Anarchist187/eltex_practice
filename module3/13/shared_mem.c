#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define RAND_MAX_CNT 10

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
    int shmfd,sets_amount,shmd,status;
    pid_t pid;
    const char *shmname="/my_shmem";
    shmfd=shm_open(shmname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (shmfd==-1){
        perror("Error shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shmfd,sizeof(SharedMem))==-1){
        perror ("Error truncate");
        exit(EXIT_FAILURE);
    }
    SharedMem *shared_mem=mmap(NULL,sizeof(SharedMem),PROT_READ | PROT_WRITE, MAP_SHARED,shmfd,0);
    if (shared_mem==MAP_FAILED){
        perror ("Error mmap");
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
                    flag_running=0;
                    break;
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
    if (munmap(shared_mem,sizeof(SharedMem))==-1){
        perror("Error munmap");
    }
    close(shmfd);
    if (shm_unlink(shmname)==-1){
        perror ("Error shm_unlink");
        exit(EXIT_FAILURE);
    }
    wait(&status);
    printf("Parent: return status for Child: %d\n",WEXITSTATUS(status));
}
    