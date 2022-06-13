#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void proc1(int source, int dest)
{
    int buf1 = 1234;
    char buf2[5];
    memset(buf2, '\0', sizeof(buf2));

    printf("Send message \"1234\" from %d to %d\n", source, dest);

    MPI_Send(&buf1, /* buffer que contiene los elementos a enviar */
             1, /* cantidad de elementos a enviar */
             MPI_INT, /*  tipo de dato de los elementos */
             dest, /* indica el rank del proceso, dentro del comunicador comm, a quien va dirigido el mensaje */
             0, /* es una etiqueta del mensaje que puede ser utilizada por el receptor para distinguir diferentes mensajes del mismo emisor */
             MPI_COMM_WORLD);

    printf("Recv message \"hola\" from %d\n", dest);
    MPI_Recv(buf2, 4, MPI_CHAR, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Recvd message: %s\n", buf2);
}

void proc2(int source, int dest)
{
    int buf1 = 0;
    char *buf2 = "hola";
    MPI_Status status;

    printf("Recv message \"1234\" from %d\n", dest);

    MPI_Recv(&buf1, /* buffer que contendrá los elementos a recibir */
             1, /* cantidad máxima de elementos a recibir */
             MPI_INT, /* tipo de dato de los elementos */
             dest, /* indica el rank del proceso, dentro del comunicador comm, de quien proviene el mensaje */
             0, /* es una etiqueta del mensaje que puede ser utilizada por el receptor para distinguir diferentes mensajes del mismo emisor */
             MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

    printf("Recvd message: %d\n", buf1);
    printf("Send message \"hola\" from %d to %d\n", source, dest);
    MPI_Send(buf2, strlen(buf2), MPI_CHAR, dest, 0, MPI_COMM_WORLD);
}

int main(int argc, char *argv[])
{
    int rank;
    int size;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &name_len);

    if (rank == 0)
        proc1(rank, 1);
    else if (rank == 1)
        proc2(rank, 0);

    MPI_Finalize();

    return 0;
}
