#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank;
    int size;
    int x = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        printf("x initial value: %d\n", x);
        MPI_Send(&x, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
    } else if (rank < size-1) {
        MPI_Recv(&x, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        x++;
        MPI_Send(&x, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&x, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("x final value: %d\n", x);
    }

    MPI_Finalize();

    return 0;
}
