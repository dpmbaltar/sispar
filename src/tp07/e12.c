#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 10

int main(int argc, char **argv)
{
    int size, rank;
    int i, j = 1;
    double t;
    enum rank_roles {SENDER, RECEIVER};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 2) {
        printf("Must run with 2 processes!\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    t = MPI_Wtime();

    switch (rank)
    {
        case SENDER:
        {
            // Crear matriz de N*N como arreglo;
            // no se usa pila para evitar overflow con N muy grande
            int *matrix = (int*)malloc(sizeof(int) * N * N);
            for (i = 0; i < N * N; i++)
                matrix[i] = i;

            // Crear tipo "mcol_type" (matrix column type) para matriz de N*N
            MPI_Datatype mcol_type;
            MPI_Type_vector(N, 1, N, MPI_INT, &mcol_type);
            MPI_Type_commit(&mcol_type);

            // Enviar la columna j
            MPI_Send(&matrix[j], 1, mcol_type, RECEIVER, 0, MPI_COMM_WORLD);
        }
        break;
        case RECEIVER:
        {
            int *col = (int*)malloc(sizeof(int) * N);
            MPI_Recv(col, N, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            printf("Valores de la columna %d:\n", j);
            for (i = 0; i < N; i++)
                printf("%d\n", col[i]);
        }
        break;
    }

    t = MPI_Wtime() - t;
    //printf("Proccess %d time: %f\n", rank, t);

    MPI_Finalize();

    return 0;
}
