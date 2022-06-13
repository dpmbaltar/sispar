#include <stdio.h>
#include <mpi.h>
#define MSGLEN 2048

int main(int argc, char *argv[])
{
    int ITAG_A = 100, ITAG_B = 200;
    int irank, i, idest, isrc, istag, iretag;
    float rmsg1[MSGLEN];
    float rmsg2[MSGLEN];
    MPI_Status recv_status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);

    for (i = 0; i < MSGLEN; i++) {
        rmsg1[i] = 100;
        rmsg2[i] = -100;
    }
    if (irank == 0) {
        idest = 1;
        isrc = 1;
        istag = ITAG_A;
        iretag = ITAG_B;
    }
    else if (irank == 1) {
        idest = 0;
        isrc = 0;
        istag = ITAG_B;
        iretag = ITAG_A;
    }

    printf("La tarea %d va a enviar un mensaje\n", irank);
    if (irank == 0)
        MPI_Ssend(rmsg1, MSGLEN, MPI_FLOAT, idest, istag, MPI_COMM_WORLD);

    MPI_Recv(rmsg2, MSGLEN, MPI_FLOAT, isrc, iretag, MPI_COMM_WORLD, &recv_status);

    if (irank == 1)
        MPI_Ssend(rmsg1, MSGLEN, MPI_FLOAT, idest, istag, MPI_COMM_WORLD);

    printf("La tarea %d ha recibido el mensaje\n", irank);
    MPI_Finalize();
}
