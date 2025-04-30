#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define FILE_NAME "file.txt"
#define LOG_NAME "log.txt"
#define SEM_NAME "/my_sem"
#define RW_SEM_NAME "/my_rw_sem"
#define WRITER_SEM_NAME "/my_writer_sem"

sem_t *sem;//2
sem_t *rw_sem;//1
sem_t *writer_sem;//0
int *sem_val;

void reader_lock(){
    sem_wait(rw_sem);
    sem_post(rw_sem);
    sem_wait(writer_sem);
    sem_getvalue(sem,sem_val);
    if (*sem_val==0){
        sem_wait(rw_sem);
    }
    sem_post(sem);
    sem_post(writer_sem);
}

void reader_unlock (){
    sem_wait(writer_sem);
    sem_wait(sem);
    sem_getvalue(sem,sem_val);
    if (*sem_val==0){
        sem_post(rw_sem);
    }
    sem_post(writer_sem);
}

int main (int argc, char *argv[]){
    int fd,fd_log, num,status,*reader_ind=0; 
    pid_t pid;
    int pipe_fd[2];
    char buf[300];
    char log_ms[300];

    fd_log = open(LOG_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_log==-1){
        perror("Cannot open log");
        exit(EXIT_FAILURE);
    }
    fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd==-1){
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }
    close(fd);

    if (argc<2){
        printf("Usage: %s <num_of_readers> <positive_number>",argv[0]);
        close(fd_log);
        exit(EXIT_FAILURE);
    }
    int readers_count=atoi(argv[1]);
    int num_count=atoi(argv[2]);
    if (num_count<=0 || readers_count<=0){
        printf("Parameters must be positive");
        close (fd_log);
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe_fd)==-1){
        perror("Error creating pipe");
        close (fd_log);
        exit(EXIT_FAILURE);
    }
    sem = sem_open(SEM_NAME, O_CREAT,0666,0);
    rw_sem=sem_open(RW_SEM_NAME, O_CREAT,0666,1);
    writer_sem=sem_open(WRITER_SEM_NAME,O_CREAT,0666,1);
    if (sem==SEM_FAILED || rw_sem==SEM_FAILED || writer_sem==SEM_FAILED){
        perror("Error sem_open");
        exit(EXIT_FAILURE);
    }
    for (int i=0;i<readers_count;i++){
        pid=fork();
        if (pid==-1){
            perror("Error fork");
            close(fd_log);
            exit(EXIT_FAILURE);
        } else if(pid==0){
            close(pipe_fd[0]);
            srand(time(NULL)^getpid());
            for (int j=0;j<3;j++){
                sleep (rand() % 3 + 1);
                printf("asd\n");
                reader_lock();
                snprintf(log_ms,sizeof(log_ms),"Child %d blocked access\n",i+1);
                write(fd_log,log_ms,strlen(log_ms));
                fd=open(FILE_NAME,O_RDONLY);
                if(fd==-1){
                    perror("Child: error opening file to read");
                    close(fd_log);
                    exit(EXIT_FAILURE);
                }
                ssize_t bytes_read=read(fd,buf,sizeof(buf)-1);
                close(fd);
                if (bytes_read>0){
                    buf[bytes_read]='\0';
                    snprintf(log_ms,sizeof(log_ms),"Child got: %s\n",buf);
                    write(fd_log,log_ms,strlen(log_ms));
                }
                reader_unlock();
                snprintf(log_ms,sizeof(log_ms),"Child unblocked access\n");
                write(fd_log,log_ms,strlen(log_ms));
                
                num=rand()%10000;
                write(pipe_fd[1],&num,sizeof(num));
                snprintf(log_ms,sizeof(log_ms),"Child sent: %d\n",num);
                write(fd_log,log_ms,strlen(log_ms));
            }
            close(pipe_fd[1]);
            close(fd_log);
            exit(EXIT_SUCCESS);
        }
    }
        if (pid>0){
            close (pipe_fd[1]);
            fd=open(FILE_NAME,O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd==-1){
                perror("Error opening file");
                close(fd_log);
                exit(EXIT_FAILURE);
            }
            for (int i=0;i<num_count;i++){
                read(pipe_fd[0],&num,sizeof(num));
                snprintf(log_ms,sizeof(log_ms),"Parent received: %d\n",num);
                write(fd_log,log_ms,strlen(log_ms));
                sem_wait(rw_sem);
                snprintf(log_ms,sizeof(log_ms),"Parent blocked file access\n");
                write(fd_log,log_ms,strlen(log_ms));
                int msg_len=snprintf(buf,sizeof(buf),"%d\n",num);
                write(fd,buf,msg_len);
                snprintf(log_ms,sizeof(log_ms),"Parent unblocked file access\n");
                write(fd_log,log_ms,strlen(log_ms));
                sem_post(rw_sem);
            }
        }
            close(pipe_fd[0]);
            close(fd);
            wait(&status);
            printf("Parent: return status for Child: %d\n",WEXITSTATUS(status));
}
