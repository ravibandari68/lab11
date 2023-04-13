#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    double *x;
    double sum;
    int N;
    int size;
    long tid;
} ThreadArgs;

void *compute(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int begin, end, N2, i;

    
    double *x = args->x;
    double sum2 = 0.0;
    int N = args->N;
    int size = args->size;
    long tid = args->tid;

    
    N2 = N / size;
    begin = tid * N2;
    end = begin + N2;
    if (tid == (size - 1))
        end = N;

    
    for (i = begin; i < end; i++)
        sum2 += x[i];

    
    pthread_mutex_lock(&mutex);
    args->sum += sum2;
    pthread_mutex_unlock(&mutex);

    return (NULL);
}

int main(int argc, char **argv) {
    long i;
    pthread_t *tid;

    if (argc != 3) {
        printf("Usage: %s <# of elements> <# of threads>\n", argv[0]);
        exit(-1);
    }

    int N = atoi(argv[1]); 
    int size = atoi(argv[2]); 

    
    double *x = (double *)malloc(sizeof(double) * N);
    for (i = 0; i < N; i++)
        x[i] = (double)(i + 1);

    
    tid = (pthread_t *)malloc(sizeof(pthread_t) * size);
    ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs) * size);
    for (i = 0; i < size; i++) {
        args[i].x = x;
        args[i].sum = 0.0;
        args[i].N = N;
        args[i].size = size;
        args[i].tid = i;
        pthread_create(&tid[i], NULL, compute, (void *)&args[i]);
    }

    
    for (i = 0; i < size; i++)
        pthread_join(tid[i], NULL);

    double total = 0.0;
    for (i = 0; i < size; i++)
        total += args[i].sum;

    printf("The total is %g, it should be equal to %g\n",
           total, ((double)N * (N + 1)) / 2);

    
    free(x);
    free(tid);
    free(args);

    return 0;
}