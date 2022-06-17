#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 500000000

int main(int argc, char **argv)
{
    int size, rank, dest;
    int *vec_a, *vec_b;
    int i, x = 0;
    double t;

    enum PROCESSES {PROC_A, PROC_B};

    MPI_Status send_st;
    MPI_Status recv_st;
    MPI_Request send_req;
    MPI_Request recv_req;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    t = MPI_Wtime();
    vec_a = (int*)malloc(sizeof(int) * N);
    vec_b = (int*)malloc(sizeof(int) * N);

    for (i = 0; i < N; i++) {
        vec_a[i] = i + rank;
        x += vec_a[i];
    }

    switch (rank) {
        case PROC_A:
            dest = PROC_B;
            break;

        case PROC_B:
            dest = PROC_A;
            break;
    }

    MPI_Isend(vec_a, N, MPI_INT, dest, 0, MPI_COMM_WORLD, &send_req);
    MPI_Irecv(vec_b, N, MPI_INT, dest, 0, MPI_COMM_WORLD, &recv_req);
    MPI_Wait(&send_req, &send_st);
    MPI_Wait(&recv_req, &recv_st);

    for (i = 0; i < N; i++)
        vec_a[i] = x + vec_b[i];

    t = MPI_Wtime() - t;
    printf("Proccess %d time: %f\n", rank, t);

    MPI_Finalize();

    return 0;
}
