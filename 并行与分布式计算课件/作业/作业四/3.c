#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define DIMENSION 1000

int **matrix;
int *vec;
int *result;
int thread = 4;

void *mul(void *r);

int main()
{
    // 初始化矩阵和向量
    long i, j;
    matrix = (int **)calloc(DIMENSION, sizeof(int *));
    for(i = 0; i < DIMENSION; ++i)
        matrix[i] = (int *)calloc(DIMENSION, sizeof(int));
    vec = (int *)calloc(DIMENSION, sizeof(int));
    result = (int *)calloc(DIMENSION, sizeof(int));
    for(i = 0; i < DIMENSION; ++i)
    {
        matrix[i][i] = i;
        vec[i] = i;
    } 

    printf("Before mul vec:\n");
    for (i = 0; i < DIMENSION; ++i)
        printf("%d ", vec[i]);
    printf("\n");

    pthread_t *handles = (pthread_t *)malloc(thread * sizeof(pthread_t));
    for(i = 0; i < thread; ++i)
        pthread_create(&handles[i], NULL, mul,\
            (void *)i);

    for(i = 0; i < thread; ++i)
        pthread_join(handles[i], NULL);

    printf("After mul vec:\n");
    for(i = 0; i < DIMENSION; ++i)
        printf("%d ", result[i]);
    printf("\n");

    free(handles);
    free(vec);
    free(result);
    for(i = 0; i < DIMENSION; ++i)
        free(matrix[i]);
    free(matrix);

    return 0;
}

void *mul(void *r)
{
    long rank = (long)r;
    int len = DIMENSION / thread;
    int fir_dim = rank * len;
    int las_dim = (rank + 1) * len;

    int i, j;
    for(i = fir_dim; i < las_dim; ++i)
        for(j = 0; j < DIMENSION; ++j)
            result[i] += vec[j] * matrix[i][j];
    
    return NULL;
}

