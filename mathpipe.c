#include "aggregate.h"
#include "singular.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

const char *aggregateFunctions[] = {"COUNT", "MIN", "MAX", "SUM", "AVG", "PAVG"};
const char *singularFunctions[] = {"FILTER", "SHIFT", "PRINT"};
#define MAX_SIZE 256
#define BUFFER_SIZE 2048
#define FUNCTION_SIZE 20
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: mathpipe aggregate_function at line %d\n", __LINE__);
        exit(1);
    }

    int precision = -1;
    int size = MAX_SIZE;
    char function_agg[FUNCTION_SIZE];
    char function_sing[FUNCTION_SIZE];
    double shift_number = 0.0;
    enum filter_type filter_;
    double threshold = 0.0;
    int agg = 0;
    int sing = 0;
    int sizeCheck = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "-prec=", 6) == 0)
        {
            char *arg = argv[i] + 6;
            precision = atoi(arg);

            if (precision < 1 || precision > 9)
            {
                fprintf(stderr, "Precision must be between 1 and 9 at line %d\n", __LINE__);
                exit(1);
            }
        }
        else if (strncmp(argv[i], "-size=", 6) == 0)
        {
            char *arg = argv[i] + 6;
            size = atoi(arg);
            sizeCheck++;
            if (size <= 0)
            {
                fprintf(stderr, "Invalid size \n");
                exit(1);
            }
        }
        else
        {
            char *function = argv[i];
            for (int j = 0; function[j]; j++)
            {
                function[j] = toupper(function[j]);
            }
            for (int j = 0; j < 6; j++)
            {
                if (strcmp(function, aggregateFunctions[j]) == 0)
                {
                    strcpy(function_agg, function);
                    agg++;
                }
            }

            for (int j = 0; j < 3; j++)
            {
                if (strcmp(function, singularFunctions[j]) == 0)
                {
                    strcpy(function_sing, function);
                    sing++;
                    if (strcmp(function_sing, "FILTER") == 0 && i + 2 < argc)
                    {
                        if (strcmp(argv[i + 1], "EQ") == 0)
                            filter_ = EQ;
                        if (strcmp(argv[i + 1], "NEQ") == 0)
                            filter_ = NEQ;
                        if (strcmp(argv[i + 1], "GEQ") == 0)
                            filter_ = GEQ;
                        if (strcmp(argv[i + 1], "LEQ") == 0)
                            filter_ = LEQ;
                        if (strcmp(argv[i + 1], "LESS") == 0)
                            filter_ = LESS;
                        if (strcmp(argv[i + 1], "GREATER") == 0)
                            filter_ = GREATER;
                        threshold = atof(argv[i + 2]);
                    }
                }
            }

            if (strcmp(function_sing, "SHIFT") == 0 && i + 1 < argc)
            {
                shift_number = atof(argv[i + 1]);
                i++;
            }
        }
    }

    if (agg == 0 && sing == 0)
    {
        fprintf(stderr, "Invalid function name at line %d\n", __LINE__);
        exit(1);
    }

    double* array = (double*)malloc(size * sizeof(double));
    if (array == NULL)
    {
        fprintf(stderr, "Memory allocation failed at line %d\n", __LINE__);
        exit(1);
    }

    int numElements = 0;
    int numFiltered = 0;
    char readBuff[BUFFER_SIZE];
    char *token = NULL;
    int countTotal = 0;
    int sizeTemp = 0;
    int notEmpty = 0;
    setprec(precision);
    while (fgets(readBuff, sizeof(readBuff), stdin) != NULL)
    {
        notEmpty=1;
        int count = 0;
        char countBuff[BUFFER_SIZE];
        strncpy(countBuff, readBuff, sizeof(readBuff));
        token = strtok(readBuff, " ");
        while (token != NULL)
        {
            count++;
            token = strtok(NULL, " ");
        }
        countTotal += count;
        if (!sizeCheck)
        {
            size = count;
            if (count >= 256)
            {
                size = MAX_SIZE;
            }
            array = (double *)realloc(array, size * sizeof(double));
            if (array == NULL)
            {
                fprintf(stderr, "Memory allocation failed at line %d\n", __LINE__);
                exit(1);
            }
        }
        if (sing)
        {
            token = strtok(countBuff, " ");
            while (token != NULL)
            {
                sscanf(token, "%lf", &array[numElements]);
                numElements++;
                sizeTemp++;
                token = strtok(NULL, " ");
                if (numElements >= size || token == NULL)
                {
                    if (strcmp(function_sing, "FILTER") == 0)
                    {
                        numFiltered = filter(array, numElements, filter_, threshold);
                        print(array, numFiltered);
                        if (numFiltered != 0)
                        {
                            if (!feof(stdin))
                            {
                                printf("\n");
                            }
                            else if (countTotal != sizeTemp)
                                printf("\n");
                        }
                    }
                    else if (strcmp(function_sing, "SHIFT") == 0)
                    {
                        shift(array, numElements, shift_number);
                        print(array, numElements);
                       if (!feof(stdin))
                        {
                            printf("\n");
                        }
                        else if (countTotal != sizeTemp)
                            printf("\n");
                    }
                    else if (strcmp(function_sing, "PRINT") == 0)
                    {
                        print(array, numElements);
                        if (!feof(stdin))
                        {
                            printf("\n");
                        }
                        else if (countTotal != sizeTemp)
                            printf("\n");
                    }
                    numElements = 0;
                }
            }
        }
        else if (agg)
        {
            token = strtok(countBuff, " ");
            while (token != NULL)
            {
                sscanf(token, "%lf", &array[numElements]);
                numElements++;
                token = strtok(NULL, " ");
                if (numElements >= size || token == NULL)
                {
                    double result = aggregate(function_agg, array, numElements);
                    if (precision > 0)
                    {
                        printf("%.*f ", precision, result);
                    }
                    else
                    {
                        printf("%f ", result);
                    }
                    numElements = 0;
                }
            }
        }
    }
    if ((strcmp(function_sing, "FILTER") == 0 && numFiltered == 0) || notEmpty != 1)
    {
    }
    else
        printf("\n");
    free(array);
    return 0;
}
