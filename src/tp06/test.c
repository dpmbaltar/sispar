#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

#define N 1000000

double sample_time()
{
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec + ((double)tv.tv_nsec)/1000000000.0);
}

void main()
{
    #pragma omp parallel num_threads (2)
    {
        #pragma omp single
        {
            #pragma omp task
            {
                int thread_num = omp_get_thread_num();
                printf("hola %d\n", thread_num);
            }
            #pragma omp task
            {
                int thread_num = omp_get_thread_num();
                printf("mundo %d\n", thread_num);
            }
        }
        printf("fin\n");
    }



    /*int sum = 0;
    int sub = 0;
    int *a = (int*)malloc(sizeof(int) * N);

    for (int i = 0; i < N; i++)
        a[i] = i;

    omp_set_dynamic(0);
    double t = sample_time();

    #pragma omp parallel num_threads(2)
    {
        int thread_num = omp_get_thread_num();

        #pragma omp for schedule(guided,10000)
        for (int i = 0; i < N; i++) {
            sum+= a[i];
            sub-= a[i];
            //printf("[Thread %d] elem %d\n", thread_num, i);
        }
    }

    printf("Sum: %d Sub: %d\n", sum, sub);

    t = sample_time() - t;
    printf("Time: %f\n", t);*/



    /*float a[3] = {1, 2, 3};
    float x, y;

    x = 10;
    y = 0;

    omp_set_dynamic(0);

    #pragma omp parallel reduction(-:x) reduction(+:y) num_threads(3)
    {
        int t = omp_get_thread_num();
        printf("Thread %d tiene el dato %f\n", t, a[t]);
        x = x - a[t];
        y = y + a[t];
    }

    printf("Resultado: x=%f y=%f\n", x, y);*/




    /*#pragma omp parallel num_threads (2)
    {
        #pragma omp single
        {
            #pragma omp task
            printf("hola\n");
            #pragma omp taskwait
            #pragma omp task
            printf("mundo\n");
        }
        printf("fin\n");
    }*/
}
