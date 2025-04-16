#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FILE_NAME "log.txt"

int main(int argc, char *argv[]){
    int fd;
    pid_t pid;
    if (argc != 2){
        printf("Usage: %s <number>",argv[0]);
        exit(EXIT_FAILURE);
    }
    fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd==-1){
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }
    if (argv[1]<=0){
        printf("Parameter <number> must be positive");
        close (fd);
        exit(EXIT_FAILURE);
    }
    switch(pid = fork()){
        case -1:
            perror("fork");
            close(fd);
            exit(EXIT_FAILURE);
        case 0:
            
        default:
            
    } 
}