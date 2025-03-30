#include <stdlib.h>
#include <stdio.h>

void menu()
{
    printf("Выберите действие: \n");
    printf("1.Сложить\n");
    printf("2.Вычесть\n");
    printf("3.Умножить\n");
    printf("4.Поделить\n");
    printf("Выбранное действие: ");
}
float add(float numbers[], int numArgs)
{
    float result=0.0;
    for (int i = 0; i < numArgs; i++)
        result += numbers[i];
    return result;
}
float sub(float numbers[], int numArgs)
{
    float result = numbers[0];
    for (int i = 1; i < numArgs; i++)
        result -= numbers[i];
    return result;
}
float mult(float numbers[], int numArgs)
{
    float result = 1.0;
    for (int i = 0; i < numArgs; i++)
        result *= numbers[i];
    return result;
}
float divide(float numbers[], int numArgs)
{
    float result = numbers[0];
    for (int i = 1; i < numArgs; i++)
    {
        if (numbers[i] != 0)
            result /= numbers[i];
        else
            return 0;
    }
    return result;
}

int main() 
{
    int argsCount=0;
    int action=0;
    float (*operations[4])(float*, int) = {add, sub, mult, divide};
    cyclePoint:
    menu();
    scanf("%d", &action);
    printf("Введите количество аргументов: \n");
    scanf("%d", &argsCount);
    float *ptr=(float *)malloc(sizeof(float)*argsCount);
    //float* numbers{new float[argsCount]};
    printf("Введите аргументы: \n");
    for (int i = 0; i < argsCount; i++)
        scanf("%f", &ptr[i]);
    printf("Результат: %.2f \n", operations[action-1](ptr, argsCount));
    free (ptr);
    goto cyclePoint;
}