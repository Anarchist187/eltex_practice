#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int arg_count=atoi(argv[1]);
    int pipefd[2];
    int fd;
    int num;
    if (pipe(pipefd)==-1){
        perror("Cannot create pipe");
        exit(EXIT_FAILURE);
    }
    pid_t pid;
    switch (pid=fork()){
        case -1:
            perror("Error fork");
            exit(EXIT_FAILURE);
        case 0:
            close(pipefd[0]);
            srand(time(NULL));
            for(int i =0;i<arg_count;i++){
                num=rand()%10000;
                write(pipefd[1],&num,sizeof(num));
            }
            close(pipefd[1]);
            exit(EXIT_SUCCESS);
            break;
        default:
            close(pipefd[1]);
            fd=open("nums.txt",O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd==-1){
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            for (int i=0;i<arg_count;i++){
                read(pipefd[0],&num,sizeof(num));
                char buf[16];
                int len = snprintf(buf, sizeof(buf),"%d ",num);
                write(STDOUT_FILENO,buf,len);
                write(fd,buf,len);
            }
            write(STDOUT_FILENO,"\n",1);
            write(fd,"\n",1);
            close(fd);
            close(pipefd[0]);
            wait(NULL);
            break;
    }
}