#include "aggregate.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
static double COUNT(double* arr, int size);
static double MIN(double* arr, int size);
static double MAX(double* arr, int size);
static double SUM(double* arr, int size);
static double AVG(double* arr, int size);
static double PAVG(double* arr, int size);
const char* funcnames[] = { "COUNT", "MIN", "MAX", "SUM", "AVG", "PAVG" };
double (*farray[])(double*, int) = { &COUNT, &MIN, &MAX, &SUM, &AVG, &PAVG };
int num_functions = sizeof(funcnames) / sizeof(funcnames[0]);

static double COUNT(double* arr, int size) {
    return (double)size;
}

static double MIN(double* arr, int size) {
    double min = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;
}

static double MAX(double* arr, int size) {
    double max = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

static double SUM(double* arr, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

static double AVG(double* arr, int size) {
    double sum = SUM(arr, size);
    return sum / (double)size;
}

static double PAVG(double* arr, int size) {
    double min = MIN(arr, size);
    double max = MAX(arr, size);
    return (min + max) / 2.0;
}

double aggregate(const char* func, double* arr, int size) {
    for (int i = 0; i < num_functions; i++) {
        if (strcmp(func, funcnames[i]) == 0) {
            return farray[i](arr, size);
        }
    }

    fprintf(stderr, "Invalid function name: %s\n", func);
    exit(1);
}