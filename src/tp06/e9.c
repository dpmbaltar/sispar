#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <omp.h>

double sample_time() {
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec+((double)tv.tv_nsec)/1000000000.0);
}

void main ()
{
    double t;
    long num_steps = 800000000;
    double step;
    int i;
    double x, pi, sum = 0.0;

    step = 1.0/(double) num_steps;
    t = sample_time();
    omp_set_num_threads(omp_get_max_threads());
    #pragma omp parallel for private(x) reduction(+:sum)
    for (i = 0; i < num_steps; i++) {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }
    t = sample_time() - t;
    pi = step * sum;
    printf("%f\n", pi);
    printf("%f\n", t);
}
