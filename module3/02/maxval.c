#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){
    int maxval=0;
    for (int i=0; argv[i]!=NULL; i++){
        if (atoi(argv[i])>maxval)
            maxval=atoi(argv[i]);
    }
    printf("Max value: %d\n",maxval);
    return 0;
}