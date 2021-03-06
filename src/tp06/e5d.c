#include <stdio.h>
#include <string.h>
#include <omp.h>

void main ()
{
    long num_steps = 800000000;
    double step;
    int i;
    double x, pi, sum = 0.0;
    int step_init;
    int thread_num_steps;
    int thread_num = 0;
    int nthreads = omp_get_max_threads();
    double thread_sum[nthreads][8];
    memset(thread_sum, 0, sizeof(double) * nthreads);

    //Establecer número de threads dinámicos
    omp_set_dynamic(1);
    omp_set_num_threads(nthreads);

    thread_num_steps = num_steps / nthreads;
    step = 1.0/(double) num_steps;
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

    for (i = 0; i < nthreads; i++)
        sum += thread_sum[i][0];

    pi = step * sum;
    printf("%f\n", pi);
}
