#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[]){
    char sum[50];
    //printf("%s\n",argv[0]);
    for (int i=0; argv[i]!=NULL;i++){
        strcat(sum,argv[i]);
    }
    printf("String: %s\n",sum);
    return 0;
}