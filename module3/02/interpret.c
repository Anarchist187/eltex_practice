#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 10
#define MAX_LENGTH 20

int main() {
    char *args[MAX_ARGS];
    char program[MAX_LENGTH];
    int args_count;
    int status;
    pid_t pid;
    while (1){

    printf("Какую программу вы хотите запустить?\nОпции:\nsum - сумма аргументов\nmaxval - максимальный из аргументов\nunitestr-объединить строки\nexit - выход\n");
    scanf("%s",&program);
    if (strcmp(program,"exit")==0)
        break;

    printf("Введите количество аргументов:\n");
    scanf("%d",&args_count);

    printf("Введите аргументы для программы:\n");
    int i = 0;
    while (i < args_count) {
        char arg[MAX_LENGTH];
        scanf("%s",arg);
        args[i] = strdup(arg);
        i++;
    }
    args[i] = NULL; 
    switch(pid = fork()){
        case -1:
            perror("fork");
            for (int j = 0; j < i; j++) {
                free(args[j]);
            }
            exit(EXIT_FAILURE);
        case 0:
            printf("Child: PID - %d, PPID - %d", getpid(), getppid());
            if (execv(program, args)==-1)
                exit(EXIT_FAILURE);
            exit(EXIT_SUCCESS);
        default:
            printf("Parent: PID - %d, Child PID - %d", getpid(),pid);
            wait(&status);
            printf("Parent: return status for Child: %d\n",WEXITSTATUS(status));
            for (int j = 0; j < i; j++) {
                free(args[j]);
            }
    } 
} 
    return 1;
}