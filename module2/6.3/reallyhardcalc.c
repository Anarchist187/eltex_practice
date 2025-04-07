#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "calc.h"
#include <dlfcn.h>

int main() 
{
    int argsCount=0;
    int action=0;
    void *handle;
    void (*menu)();
    float (*operations)(float*, int);
    cyclePoint:
    handle=dlopen("lib/libmenu.so",RTLD_LAZY);
    if(!handle){
        fputs(dlerror(),stderr);
        exit(1);
    }
    menu=dlsym(handle,"menu");
    menu();
    dlclose(handle);
    scanf("%d", &action);
    switch (action){
        case 1: {
            handle=dlopen("lib/libadd.so",RTLD_LAZY);
            operations=dlsym(handle,"add"); 
            break;
        }
            
        case 2: {
            handle=dlopen("lib/libsub.so",RTLD_LAZY);
            operations=dlsym(handle,"sub"); 
            break;
        }
        case 3: {
            handle=dlopen("lib/libmult.so",RTLD_LAZY);
            operations=dlsym(handle,"mult"); 
            break;
        }
        case 4: {
            handle=dlopen("lib/libdivide.so",RTLD_LAZY);
            operations=dlsym(handle,"divide"); 
            break;
        }
        case 0: return 0;
    }
            
    if(!handle){
            fputs(dlerror(),stderr);
            exit(1);
    }
    printf("Введите количество аргументов: \n");
    scanf("%d", &argsCount);
    float *ptr=(float *)malloc(sizeof(float)*argsCount);
    printf("Введите аргументы: \n");
    for (int i = 0; i < argsCount; i++)
        scanf("%f", &ptr[i]);
    printf("Результат: %.2f \n", operations(ptr, argsCount));
    free (ptr);
    dlclose(handle);
    goto cyclePoint;
}