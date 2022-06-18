#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 10000000

int main(int argc, char **argv)
{
    int size, rank, root_rank = 0;
    int i, len, local_sum = 0;
    int *vec, *local_vec;
    double t;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    t = MPI_Wtime();
    local_vec = (int*)malloc(sizeof(int) * N);

    // Repartir elementos por proceso
    if (rank == root_rank) {
        len = size * N;
        vec = (int*)malloc(sizeof(int) * len);

        for (i = 0; i < len; i++)
            vec[i] = i;

        MPI_Scatter(vec, N, MPI_INT,
                    local_vec, N, MPI_INT,
                    root_rank, MPI_COMM_WORLD);
    } else {
        MPI_Scatter(NULL, N, MPI_INT,
                    local_vec, N, MPI_INT,
                    root_rank, MPI_COMM_WORLD);
    }

    printf("Proccess %d received from %d to %d\n",
           rank, local_vec[0], local_vec[N-1]);

    // Sumar elementos
    for (i = 0; i < N; i++)
        local_sum += local_vec[i];

    // Recolectar sumas parciales
    if (rank == root_rank) {
        int total_sum = 0;
        int global_sum[size];

        MPI_Gather(&local_sum, 1, MPI_INT,
                   &global_sum, 1, MPI_INT,
                   root_rank, MPI_COMM_WORLD);

        for (i = 0; i < size; i++)
            total_sum += global_sum[i];

        printf("Sum: %d\n", total_sum);
    } else {
        MPI_Gather(&local_sum, 1, MPI_INT,
                   NULL, 0, MPI_INT,
                   root_rank, MPI_COMM_WORLD);
    }

    t = MPI_Wtime() - t;
    printf("Proccess %d time: %f\n", rank, t);

    MPI_Finalize();

    return 0;
}
