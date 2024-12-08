#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_THREADS  64
#define VECTOR_SIZE  10000000

pthread_t tid[MAX_THREADS];
pthread_mutex_t mutexsum;
int *a;
int sum = 0;
int threads_num;

void *runner(void *param) {
    int i, start, end, mysum = 0;
    long tid;
    tid = (long)param;
    start = tid * VECTOR_SIZE / threads_num;
    end = (tid + 1) * VECTOR_SIZE / threads_num;

    for (i = start; i < end; i++) {
        mysum += a[i];
    }

    pthread_mutex_lock(&mutexsum);
    sum += mysum;
    pthread_mutex_unlock(&mutexsum);
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    int i;
    struct timeval start, end;
    double elapsed_time;
    pthread_attr_t attr;
    a = (int *)malloc(sizeof(int) * VECTOR_SIZE);

    for (i = 0; i < VECTOR_SIZE; i++) {
        a[i] = rand() % 100;
    }

    if (argc != 2) {
        fprintf(stderr, "usage: pthreads <number of threads>\n");
        exit(1);
    }

    threads_num = atoi(argv[1]);

    if (threads_num > MAX_THREADS) {
        fprintf(stderr, "Maximum number of threads is %d\n", MAX_THREADS);
        exit(1);
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&mutexsum, NULL);
    gettimeofday(&start, NULL);

    for (i = 0; i < threads_num; i++) {
        pthread_create(&tid[i], &attr, runner, (void *)(intptr_t)i);
    }

    for (i = 0; i < threads_num; i++) {
        pthread_join(tid[i], NULL);
    }

    gettimeofday(&end, NULL);
    elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0;

    printf("sum = %d\n", sum);
    printf("Elapsed time: %f ms\n", elapsed_time);

    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutexsum);
    free(a);
    pthread_exit(NULL);
}

