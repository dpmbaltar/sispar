#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int size, rank;
    int i;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 15) {
        printf("Must run with 15 processes!\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    int dims[2] = {3, 5};
    int periods[2] = {true, true};
    int reorder = false;

    MPI_Comm new_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &new_comm);

    MPI_Comm_rank(new_comm, &rank);

    int coords[2];
    MPI_Cart_coords(new_comm, rank, 2, coords);

    printf("[MPI process %d] Coords: (%d, %d).\n", rank, coords[0], coords[1]);

    enum DIRECTIONS {UP, DOWN, LEFT, RIGHT};
    char* nb_names[4] = {"up", "down", "left", "right"};
    int nb_ranks[4];

    MPI_Cart_shift(new_comm, 0, 1, &nb_ranks[UP], &nb_ranks[DOWN]);
    MPI_Cart_shift(new_comm, 1, 1, &nb_ranks[LEFT], &nb_ranks[RIGHT]);

    printf("[MPI process %d] Neighbours:", rank);
    for (i = 0; i < 4; i++)
        printf(" %s: %d;", nb_names[i], nb_ranks[i]);
    printf("\n");

    MPI_Finalize();

    return 0;
}
