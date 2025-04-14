#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_ARGS 10
#define MAX_LENGTH 20

int main() {
    char *args[MAX_ARGS];
    char program[MAX_LENGTH];
    int args_count;

    printf("Какую программу вы хотите запустить?\nОпции:\nsum - сумма аргументов\nmaxval - максимальный из аргументов\nunitestr-объединить строки\n");
    scanf("%s",&program);

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
    execv(program, args);
    perror("Ошибка при запуске программы");
    for (int j = 0; j < i; j++) {
        free(args[j]);
    }

    return 1;
}