#include <math.h>
#include "calc.h"

float divide(float numbers[], int numArgs)
{
    float result = numbers[0];
    for (int i = 1; i < numArgs; i++)
    {
        if (numbers[i] != 0)
            result /= numbers[i];
        else
            return NAN;
    }
    return result;
}