#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    FILE *f;
    char *s, *res;
    int rows, cols, steps, n, i, j, ncols, nrows;

    //Controla que se haya ingresado un argumento en la llamada (el archivo con
    //el patrón de entrada)
    if (argc != 2) {
        printf("Error: Debe indicar el nombre del archivo de entrada\n");
        return 1;
    }

    //Lectura del encabezado del archivo que contiene el patrón de celdas
    f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Error al intentar abrir el archivo.\n");
        return 1;
    }

    n = fscanf(f, "cols %d\nrows %d\nsteps %d\n", &cols, &rows, &steps);

    if (n != 3) {
        printf("Error: formato de archivo incorrecto\n");
        return 1;
    }

    //El tamaño total de la matriz es el tamaño indicado más dos filas y dos
    //columnas auxiliares
    nrows = rows + 2;
    ncols = cols + 2;

    //MPI----------------------------------------------------------------------
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Permitir solo cantidad de procesos pares
    if (size < 1 || (size % 2) != 0) {
        printf("Error: debe ejecutarse con un número par de procesos\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    //Crear topología cartesiana de 2 dimensiones
    int dims[2] = {0, 0};
    int periods[2] = {true, true};
    int reorder = false;
    MPI_Comm new_comm;
    MPI_Dims_create(size, 2, dims);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &new_comm);

    MPI_Comm_rank(new_comm, &rank);

    int coords[2];
    MPI_Cart_coords(new_comm, rank, 2, coords);

    printf("[MPI process %d] Coords: (%d, %d).\n", rank, coords[0], coords[1]);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
