#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N 2500000//0

double sample_time()
{
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec + ((double)tv.tv_nsec)/1000000000.0);
}

void main(int argc, char *argv[])
{
    int n1,n2,limite,i,divisor_comun=0;
    int k, cant_coprimos=0;
    int* datos = malloc(N*sizeof(int));

    for(k=0;k<N;k++)
        datos[k]=rand()%10000;

    double t = sample_time();

    for(k=0;k<N;k=k+2){
        n1 = datos[k];
        n2 = datos[k+1];

        if(n1>n2)
            limite = n2/2;
        else
            limite = n1/2;
        i=2;
        do{
            divisor_comun = n1%i==0 && n2%i==0;
            i++;
        } while (!divisor_comun && i<=limite);

        if(!divisor_comun)
            cant_coprimos++;
    }
    printf("Cantidad de coprimos: %d\n",cant_coprimos);

    t = sample_time() - t;
    printf("Time: %f\n", t);
}
