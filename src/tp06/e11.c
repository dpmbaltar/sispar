#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

double sample_time()
{
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec + ((double)tv.tv_nsec)/1000000000.0);
}

int primo(int n)
{
    int i,res,mitad;
    i = 2;
    res = 1;
    mitad = n / 2;
    while(res != 0 && i <= mitad) {
        res = n % i;
        i++;
    }
    return res;
}

void main()
{
    double t;
    int i;
    int from, to;
    int nprimes = 0;

    from = 2;
    to = 200000;

    #pragma omp parallel private(t) reduction(+:nprimes)
    {
        t = sample_time();
        #pragma omp for schedule(guided) nowait
        for (i = from; i <= to; i++) {
            nprimes += primo(i);
        }
        t = sample_time() - t;
        printf("%f\n", t);
    }

    printf("%d\n", nprimes);
}
