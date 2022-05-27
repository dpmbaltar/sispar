#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

#define N 2000000000
#define BASE 1000000

double sample_time() {
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec+((double)tv.tv_nsec)/1000000000.0);
}

void vector_add(int *A, int *B, int *C, int n)
{
    int i;
    for (i = 0; i < n; i++)
        C[i] = A[i] + B[i];
}

void rec_vector_add(int *A, int *B, int *C, int n)
{
    /* si es el caso base, llamar a vector_add
     * sino, hacer la suma recursiva sobre la primera mitad de los
     * arreglos, y luego la suma recursiva sobre la segunda mitad de los
     * arreglos.
     */
    if (n <= BASE)
        vector_add(A, B, C, n);
    else {
        int h = n/2;
        int r = n%2;
        #pragma omp task
        rec_vector_add(A, B, C, h);
        #pragma omp task
        rec_vector_add(A+h, B+h, C+h, h+r);
    }
}

int main()
{
    double t;
    int i;
    int *a;
    int *b;
    int *c;

    a = (int*) malloc(sizeof(int) * N);
    b = (int*) malloc(sizeof(int) * N);
    c = (int*) malloc(sizeof(int) * N);
    for (i = 0; i < N; i++) {
        a[i] = i;
        b[i] = i;
        c[i] = 0;
    }

    t = sample_time();
    #pragma omp parallel num_threads(2)
    {
        #pragma omp single
        {
            rec_vector_add(a, b, c, N);
        }
    }
    t = sample_time() - t;
    printf("%f\n", t);

    /*for (i = 0; i < N; i++)
        printf("%d ", c[i]);*/

    return 0;
}
