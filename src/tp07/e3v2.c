#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank;
    int procesos;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    int nGlobal = 8;
    int i, n, exceso, offset;
    int *datos;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procesos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //MPI_Get_processor_name(processor_name, &name_len);

    n = nGlobal / procesos;
    exceso = nGlobal % procesos;

    if (rank < exceso) {
        n++;
        offset = rank * n;
    } else
        offset = (rank * n) + exceso;

    datos = malloc(sizeof(int) * n);
    for (i = 0; i < n; i++)
        datos[i] = i + offset;

    for (i = 0; i < n; i++)
        printf("%d: %d\n", rank, datos[i]);

    MPI_Finalize();

    return 0;
}
