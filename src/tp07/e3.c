#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int nGlobal = 40;
    int n = nGlobal;
    int i;
    int *datos;

    datos = malloc(sizeof(int) * n);
    for (i = 0; i < n; i++)
        datos[i] = i;

    for (i = 0; i < n; i++)
        printf("%d\n", datos[i]);

    return 0;
}
