#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){
    int sum=0;
    for (int i=0; argv[i]!=NULL;i++){
        sum+=atoi(argv[i]);
    }
    printf("Sum: %d\n",sum);
    return 0;
}