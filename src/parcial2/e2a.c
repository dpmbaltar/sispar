#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define N 1000
#define PASOS 500

double sample_time()
{
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec + ((double)tv.tv_nsec)/1000000000.0);
}
// 25.566354
int main()
{
    double t;
    double A[N][N], b[N], x[N], x2[N];
    double max=-1.0, q=0, e;
    int s;
    int i, j, k, n=N, pasos=PASOS;

    omp_set_dynamic(0);
    omp_set_num_threads(omp_get_max_threads());

    #pragma omp parallel
    {
        //inicialización
        #pragma omp for schedule(static,250) nowait
        for (i=0;i<n;i++)
            b[i] = 0.5;
        #pragma omp for schedule(static,250) nowait
        for (i=0;i<n;i++)
            x[i] = 0.5+(double)i/n;
        #pragma omp for schedule(static,250) nowait collapse(2) \
                        private(j)
        for (i=0;i<n;i++)
            for (j=0;j<n;j++)
                A[i][j] = 0.6*(sin(i+j)+1)/2;
        //fin inicialización
    }

    t = sample_time();

    for (k=0;k<pasos;k++) {
        q=1;
        for (i=0;i<n;i++) {
            s = b[i];
            #pragma omp parallel
            {
                #pragma omp for schedule(static,250) nowait \
                            reduction(+:s) private(e)
                for (j=0;j<n;j++) {
                    e = sin(A[i][j] * x[j]);
                    s+= (int) (e * e * e + 2 * e * e + 5 * e);
                }
            }
            x2[i] = x2[i-1] + s;
            q *= sin(s);
        }
        #pragma omp parallel
        {
            #pragma omp for schedule(static,250)
            for (i=0;i<n;i++)
                x[i] = x2[i];
        }
        #pragma omp parallel
        {
            #pragma omp for schedule(static) nowait
            for (i=0;i<n/2;i++)
                b[i] = b[i] + x[i] + 0.00002;
        }
        if (max<q) {
            max = q;
            printf("max: %.10e\n", max);
        }
    }

    printf("max final: %.10e\n", max);
    t = sample_time() - t;
    printf("%f\n", t);
    return max;
}
