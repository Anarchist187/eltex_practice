#include "calc.h"

float add(float numbers[], int numArgs)
{
    float result=0.0;
    for (int i = 0; i < numArgs; i++)
        result += numbers[i];
    return result;
}