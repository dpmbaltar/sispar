#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

double sample_time() {
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec+((double)tv.tv_nsec)/1000000000.0);
}

void trabajo(int id)
{
    printf("%d: Trabajo muy duro...\n", id);
    sleep(1);
    printf("%d: como un esclavo!\n", id);
    sleep(1);
}

int main()
{
    int i;
    double t;
    omp_set_num_threads(omp_get_max_threads());
    t = sample_time();
    #pragma omp parallel
    {
        #pragma omp for
        for (i = 0; i < 8; i++)
            trabajo(i);
    }
    t = sample_time() - t;
    printf("%f\n", t);

    return 0;
}
