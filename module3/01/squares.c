#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void calculate_area(int *sides, int count){
    int area=0;
    for (int i=0;i<count;i++){
        area=sides[i]*sides[i];
        printf("Side length - %d, area - %d\n",sides[i],area);
    }
}

int main(int argc, char *argv[]){
    pid_t pid;
    int status;
    if (argc<2){
        printf("Arguments: %s <side_length1> <side_length2> ...\n",argv[0]);
        return 1;
    }
    int count=argc-1;
    int *sides=malloc(count*sizeof(int));
    for (int i=0;i<count;i++){
        sides[i]=atoi(argv[i+1]);
    }
    int start=count/2;
    switch(pid = fork()){
        case -1:
            perror("fork");
            free (sides);
            exit(EXIT_FAILURE);
        case 0:
            printf("Child: PID - %d\n", getpid());
            printf("Child: PPID - %d,\n",getppid());
            calculate_area(sides, start);
            exit(EXIT_SUCCESS);
        default:
            printf("Parent: PID - %d\n", getpid());
            printf("Parent: Child PID - %d,\n",pid);
            wait(&status);
            calculate_area(&sides[start],count-start);
            printf("Parent: return status for Child: %d\n",WEXITSTATUS(status));
    }
    free(sides);
    exit(EXIT_SUCCESS);
}