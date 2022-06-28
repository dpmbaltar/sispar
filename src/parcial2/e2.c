#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

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

    //inicialización
    for (i=0;i<n;i++)
        b[i] = 0.5;
    for (i=0;i<n;i++)
        x[i] = 0.5+(double)i/n;
    for (i=0;i<n;i++)
        for (j=0;j<n;j++)
            A[i][j] = 0.6*(sin(i+j)+1)/2;
    //fin inicialización
    t = sample_time();
    for (k=0;k<pasos;k++) {
        q=1;
        for (i=0;i<n;i++) {
            s = b[i];
            for (j=0;j<n;j++) {
                e = sin(A[i][j] * x[j]);
                s+= (int) (e * e * e + 2 * e * e + 5 * e);
            }
            x2[i] = x2[i-1] + s;
            q *= sin(s);
        }
        for (i=0;i<n;i++)
            x[i] = x2[i];
        for (i=0;i<n/2;i++)
            b[i] = b[i] + x[i] + 0.00002;
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
