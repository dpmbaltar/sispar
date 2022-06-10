#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank;
    int procesos;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procesos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //MPI_Get_processor_name(processor_name, &name_len);

    int nGlobal = 40;
    int i, n = nGlobal / procesos;
    int exceso = nGlobal % procesos;
    int *datos;

    if (rank == procesos - 1)
        n += exceso;

    datos = malloc(sizeof(int) * n);
    for (i = 0; i < n; i++)
        datos[i] = i + rank * n;

    for (i = 0; i < n; i++)
        printf("%d: %d\n", rank, datos[i]);

    MPI_Finalize();

    return 0;
}
