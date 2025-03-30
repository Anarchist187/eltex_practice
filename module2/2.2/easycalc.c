#include <stdio.h>
#include <stdlib.h>

void menu()
{
    printf("Выберите действие: \n");
    printf("1.Сложить\n");
    printf("2.Вычесть\n");
    printf("3.Умножить\n");
    printf("4.Поделить\n");
    printf("Выбранное действие: ");
}
float add(float a, float b)
{
    return a + b;
}
float sub(float a, float b)
{
    return a - b;
}
float mult(float a, float b)
{
    return a * b;
}
float divide(float a, float b)
{
    return (b==0 ? 0 : a/b);
}

int main()
{
   // setlocale(0, "");
    float arg1=0, arg2=0;
    int action=0;
    cyclePoint:
    menu();
    scanf("%d", &action);
    printf("Введите аргументы: ");
    scanf("%f%f", &arg1, &arg2);
    switch (action)
    {
        case 1:
        {
            printf("Результат сложения: %.2f\n", add(arg1, arg2));
            break;
        }
        case 2:
        {
            printf("Результат вычитания: %.2f\n", sub(arg1, arg2));
            break;
        }
        case 3:
        {
            printf("Результат умножения: %.2f\n", mult(arg1, arg2));
            break;
        }
        case 4:
        {
            printf("Результат деления: %.2f\n", divide(arg1, arg2));
            break;
        }
        default:
            return 0;
    }
    goto cyclePoint;
}