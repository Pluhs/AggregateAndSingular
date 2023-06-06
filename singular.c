#include "singular.h"
#include <stdio.h>
static int prec = 0;
int setprec(int precision)
{
    prec = precision;
    return prec;
}
void print(double a[], size_t size)
{
    if (prec == -1)
    {
        for (size_t i = 0; i < size; i++)
        {
            printf("%f ", a[i]);
        }
    }
    else
        for (size_t i = 0; i < size; i++)
        {
            printf("%.*f ", prec, a[i]);
        }
}
void shift(double a[], size_t size, double by)
{
    for (size_t i = 0; i < size; i++)
    {
        a[i] += by;
    }
}
size_t filter(double a[], size_t count, enum filter_type t, double threshold)
{
    size_t filteredCount = 0;
    for (size_t i = 0; i < count; i++)
    {
        double number = a[i];
        switch (t)
        {
        case EQ:
            if (number == threshold)
            {
                a[filteredCount++] = a[i];
            }
            break;
        case NEQ:
            if (number != threshold)
            {
                a[filteredCount++] = a[i];
            }
            break;
        case GEQ:
            if (number >= threshold)
            {
                a[filteredCount++] = a[i];
            }
            break;
        case LEQ:
            if (number <= threshold)
            {
                a[filteredCount++] = a[i];
            }
            break;
        case LESS:
            if (number < threshold)
            {
                a[filteredCount++] = a[i];
            }
            break;
        case GREATER:
            if (number > threshold)
            {
                a[filteredCount++] = a[i];
            }
            break;
        }
    }
    return filteredCount;
}
