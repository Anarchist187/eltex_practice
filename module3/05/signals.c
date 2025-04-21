#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

//#define _XOPEN_SOURCE 700

#define FILE_NAME "file.txt"
#define LOG_NAME "log.txt"

volatile sig_atomic_t access_allowed = 1;

void sigusr1_def(){
    access_allowed=0;
}

void sigusr2_def(){
    access_allowed=1;
}

int main(int argc, char *argv[]){
    int fd,fd_log, num,status;
    pid_t pid;
    int pipe_fd[2];
    char buf[300];
    char log_ms[300];
    if (argc != 2){
        printf("Usage: %s <number>",argv[0]);
        exit(EXIT_FAILURE);
    }
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
    srand(time(NULL));
    switch(pid = fork()){
        case -1:
            perror("fork failed");
            close(fd_log);
            exit(EXIT_FAILURE);
        case 0:
            close(pipe_fd[0]);
            srand(time(NULL));
            //struct sigaction sa;
            //sigemptyset(&sa.sa_mask);
            //sa.sa_handler=sigusr1_def;
            //sa.sa_flags = 0;
            signal(SIGUSR1,sigusr1_def);
            //sa.sa_handler=sigusr2_def;
            signal(SIGUSR2,sigusr2_def);

            for(int i =0;i<num_count;i++){
                while (!access_allowed){
                    pause();
                }
                fd=open(FILE_NAME,O_RDONLY);
                if(fd==-1){
                    perror("Child: error opening file to read");
                    close(fd_log);
                    exit(EXIT_FAILURE);
                }
                ssize_t bytes_read=read(fd,buf,sizeof(buf)-1);
                close(fd);
                while (!access_allowed){
                    pause();
                }
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
        close(pipe_fd[1]);
        fd=open(FILE_NAME,O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(fd==-1){
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        for (int i=0;i<num_count;i++){
            read(pipe_fd[0],&num,sizeof(num));
            snprintf(log_ms,sizeof(log_ms),"Parent received: %d\n",num);
            write(fd_log,log_ms,strlen(log_ms));
            kill(pid,SIGUSR1);
            write(fd_log,"Parent blocked access\n",sizeof("Parent blocked access\n"));
            int msg_len=snprintf(buf,sizeof(buf),"%d\n",num);
            write(fd,buf,msg_len);
            kill(pid,SIGUSR2);
            write(fd_log,"Parent unblocked access\n",sizeof("Parent unblocked access\n"));
        }
        close(pipe_fd[0]);
        wait(&status);
        printf("Parent: return status for Child: %d\n",WEXITSTATUS(status));
    } 
}