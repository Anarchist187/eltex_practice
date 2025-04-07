#include "calc.h"

float sub(float numbers[], int numArgs)
{
    float result = numbers[0];
    for (int i = 1; i < numArgs; i++)
        result -= numbers[i];
    return result;
}