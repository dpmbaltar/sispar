#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>


double sample_time()
{
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec + ((double)tv.tv_nsec)/1000000000.0);
}

void main ()
{
    double t;
    long num_steps = 800000000;  //Pasos totales
    double step;  //Paso actual
    int i;
    double x, pi, sum = 0.0;
    int step_init; //Paso inicial local del thread
    int thread_num_steps; //Cantidad de pasos por thread
    int thread_num = 0; //Número de thread
    int nthreads = omp_get_max_threads(); //Cantidad de threads
    double thread_sum[nthreads][8];
    memset(thread_sum, 0, sizeof(double) * nthreads * 8);

    //Establecer número de threads dinámicos
    omp_set_dynamic(1);
    omp_set_num_threads(nthreads);

    thread_num_steps = num_steps / nthreads;
    step = 1.0/(double)num_steps;
    t = sample_time();

    #pragma omp parallel \
                private(i, x, num_steps, step_init, thread_num) \
                firstprivate(thread_num_steps)
    {
        thread_num = omp_get_thread_num();
        step_init = thread_num * thread_num_steps;
        num_steps = step_init + thread_num_steps;

        for (i = step_init; i < num_steps; i++) {
            x = (i + 0.5) * step;
            thread_sum[thread_num][0] += 4.0 / (1.0 + x * x);
        }
    }
    t = sample_time() -t;

    for (i = 0; i < nthreads; i++)
        sum += thread_sum[i][0];

    pi = step * sum;
    printf("%f\n", pi);
    printf("%f\n", t);
}
