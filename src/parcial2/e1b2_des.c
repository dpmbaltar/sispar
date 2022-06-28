#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 250000//00
#define B 100

int coprimos(int *datos, int n) {
    int n1,n2,limite,i,divisor_comun=0;
    int k, cant_coprimos=0;
    for(k=0;k<n;k=k+2){
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

    return cant_coprimos;
}

void coprimos_rec(int *v, int n, int *cant_coprimos) {
    if (n <= B) {
        *cant_coprimos+= coprimos(v, n);
    } else {
        int h = n/2;
        int h2 = h;
        if ((h%2) > 0) {
            h++;
            h2--;
        }
        #pragma omp task
        coprimos_rec(v, h, cant_coprimos);
        //#pragma omp taskwait
        #pragma omp task
        coprimos_rec(v+h,h2, cant_coprimos);
        //#pragma omp taskwait
    }
}

int main(int argc, char *argv[])
{
    int k, cant_coprimos=0;
    int* datos = malloc(N*sizeof(int));

    for(k=0;k<N;k++)
        datos[k]=rand()%10000;

    #pragma omp parallel reduction(+:cant_coprimos)
    {
        #pragma omp single
        {
            coprimos_rec(datos, N, &cant_coprimos);
        }
    }
    printf("Cantidad de coprimos: %d\n",cant_coprimos);
    return 0;
}
