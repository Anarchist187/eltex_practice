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
#define SEM_NAME "my_sem"

int main (int argc, char *argv[]){
    int fd,fd_log, num,status; 
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
        printf("Usage: %s <positive_number>",argv[0]);
        close(fd_log);
        exit(EXIT_FAILURE);
    }
    int num_count=atoi(argv[1]);
    if (num_count<=0){
        printf("Parameter <number> must be positive");
        close (fd_log);
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe_fd)==-1){
        perror("Error creating pipe");
        close (fd_log);
        exit(EXIT_FAILURE);
    }
    sem_t *sem = sem_open(SEM_NAME, O_CREAT,0666,1);
    if (sem==SEM_FAILED){
        perror("Error sem_open");
        exit(EXIT_FAILURE);
    }
    switch (pid=fork()){
        case -1:
            perror("Error fork");
            close(fd_log);
            exit(EXIT_FAILURE);
        case 0:
            close(pipe_fd[0]);
            srand(time(NULL));
            for (int i=0;i<num_count;i++){
                sem_wait(sem);
                snprintf(log_ms,sizeof(log_ms),"Child blocked access\n");
                write(fd_log,log_ms,strlen(log_ms));
                fd=open(FILE_NAME,O_RDONLY);
                if(fd==-1){
                    perror("Child: error opening file to read");
                    close(fd_log);
                    exit(EXIT_FAILURE);
                }
                ssize_t bytes_read=read(fd,buf,sizeof(buf)-1);
                close(fd);
                sem_post(sem);
                snprintf(log_ms,sizeof(log_ms),"Child unblocked access\n");
                write(fd_log,log_ms,strlen(log_ms));
                if (bytes_read>0){
                    buf[bytes_read]='\0';
                    snprintf(log_ms,sizeof(log_ms),"Child got: %s\n",buf);
                    write(fd_log,log_ms,strlen(log_ms));
                }
                num=rand()%10000;
                write(pipe_fd[1],&num,sizeof(num));
                snprintf(log_ms,sizeof(log_ms),"Child sent: %d\n",num);
                write(fd_log,log_ms,strlen(log_ms));
            }
            close(pipe_fd[1]);
            close(fd_log);
            exit(EXIT_SUCCESS);
        default:
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
                sem_wait(sem);
                snprintf(log_ms,sizeof(log_ms),"Parent blocked file access\n");
                write(fd_log,log_ms,strlen(log_ms));
                int msg_len=snprintf(buf,sizeof(buf),"%d\n",num);
                write(fd,buf,msg_len);
                sem_post(sem);
                snprintf(log_ms,sizeof(log_ms),"Parent unblocked file access\n");
                write(fd_log,log_ms,strlen(log_ms));
            }
            close(pipe_fd[0]);
            close(fd);
            wait(&status);
            printf("Parent: return status for Child: %d\n",WEXITSTATUS(status));
        }
}