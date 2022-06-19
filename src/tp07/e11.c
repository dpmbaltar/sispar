#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 10000000

int main(int argc, char **argv)
{
    int size, rank, root_rank = 0, token = 0;
    int i, len, local_sum = 0, total_sum = 0;
    int *vec = NULL;
    int *global_sum = NULL;
    int *local_vec;
    double t;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    t = MPI_Wtime();
    local_vec = (int*)malloc(sizeof(int) * N);

    // Inicializar variables del proceso 0
    if (rank == root_rank) {
        len = size * N;
        vec = (int*)malloc(sizeof(int) * len);
        global_sum = (int*)malloc(sizeof(int) * size);

        for (i = 0; i < len; i++)
            vec[i] = i;
    }

    // Repartir elementos por proceso (desde el proc. 0)
    MPI_Scatter(vec, N, MPI_INT,
                local_vec, N, MPI_INT,
                root_rank, MPI_COMM_WORLD);

    // Sumar elementos asignados
    for (i = 0; i < N; i++)
        local_sum += local_vec[i];

    // Recolectar sumas parciales de los procesos
    MPI_Gather(&local_sum, 1, MPI_INT,
               global_sum, 1, MPI_INT,
               root_rank, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    printf("Proccess %d received from %d to %d\n",
           rank, local_vec[0], local_vec[N-1]);

    // Realizar suma global
    if (rank == root_rank) {
        for (i = 0; i < size; i++)
            total_sum += global_sum[i];

        printf("Sum: %d\n", total_sum);
    }

    t = MPI_Wtime() - t;
    //printf("Proccess %d time: %f\n", rank, t);

    MPI_Finalize();

    return 0;
}
