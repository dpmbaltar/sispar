#include <stdio.h>
#include <mpi.h>
#define N 50000

int main(int argc, char *argv[])
{
    int ITAG_A = 100, ITAG_B = 200;
    int irank, i, idest, isrc, istag, iretag;
    float rmsg1[MSGLEN];
    float rmsg2[MSGLEN];
    MPI_Status send_status;
    MPI_Status recv_status;
    MPI_Request send_request;
    MPI_Request recv_request;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);

    //...

    MPI_Finalize();
}
