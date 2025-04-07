#include "calc.h"

float mult(float numbers[], int numArgs)
{
    float result = 1.0;
    for (int i = 0; i < numArgs; i++)
        result *= numbers[i];
    return result;
}