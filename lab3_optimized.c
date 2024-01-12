#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define XDIM 10000
#define YDIM 10000

double **alloc_matrix(void)
{
    int i;
    double **array;

    array = malloc(XDIM * sizeof(double *));

    for (i = 0; i < XDIM; i++)
        array[i] = calloc(YDIM, sizeof(double));

    return array;
}

void fill(double **arr)
{
    int i, j;
    time_t t1;
    srand((unsigned)time(&t1));

    for (i = 0; i < XDIM; i++)
        for (j = 0; j < YDIM; j++)
            arr[i][j] = (double)(rand() % 100);
}

void compute(double **arr, int kern[3][3])
{
    int i, j;
    const double factor = 1.0 / 250;
    const double kern_factor[9] =
    {
        kern[0][0] * factor, kern[1][0] * factor, kern[2][0] * factor,
        kern[0][1] * factor, kern[1][1] * factor, kern[2][1] * factor,
        kern[0][2] * factor, kern[1][2] * factor, kern[2][2] * factor
    };

    for (i = 1; i < XDIM - 1; i++)
    {
        double *prev_row = arr[i - 1];
        double *curr_row = arr[i];
        double *next_row = arr[i + 1];

        for (j = 1; j < YDIM - 1; j++)
        {
            arr[i][j] = kern_factor[0] * prev_row[j - 1] +
                        kern_factor[1] * curr_row[j - 1] +
                        kern_factor[2] * next_row[j - 1] +
                        kern_factor[3] * prev_row[j] +
                        kern_factor[4] * curr_row[j] +
                        kern_factor[5] * next_row[j] +
                        kern_factor[6] * prev_row[j + 1] +
                        kern_factor[7] * curr_row[j + 1] +
                        kern_factor[8] * next_row[j + 1] + 9;
        }
    }
}

void print(double **arr)
{
    int i, j;

    for (i = 0; i < XDIM; i++)
        for (j = 0; j < YDIM; j++)
            printf("array[%d][%d] = %f\n", i, j, arr[i][j]);
}

int main(void)
{
    double **arr;
    int kern[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

    arr = alloc_matrix();
    fill(arr);
    compute(arr, kern);
    // print(arr);

    return 0;
}