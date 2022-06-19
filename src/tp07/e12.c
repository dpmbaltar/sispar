#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 3

int main(int argc, char **argv)
{
    int size, rank;
    int i, j;
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

    // Probar tipo
    switch (rank)
    {
        case SENDER:
        {
            /*int **matrix = (int**)malloc(sizeof(int*) * N);
            for (i = 0; i < N; i++) {
                matrix[i] = (int*)malloc(sizeof(int) * N);
                for (j = 0; j < N; j++)
                    matrix[i][j] = (N*i) + j;
            }*/
            int matrix[3][3] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

            // Crear tipo "mcol" (matrix column) para matriz de N*N
            MPI_Datatype mcol;
            MPI_Type_vector(N, N, N+1, MPI_INT, &mcol);
            MPI_Type_commit(&mcol);

            // Enviar la columna 1
            MPI_Send(&matrix[0][1], 1, mcol, RECEIVER, 0, MPI_COMM_WORLD);
        }
        break;
        case RECEIVER:
        {
            int *col = (int*)malloc(sizeof(int) * N);
            MPI_Recv(&col, N, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

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
