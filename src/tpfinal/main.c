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
    printf("Rows: %d cols: %d\n", rows, cols);

    //MPI----------------------------------------------------------------------
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Permitir solo cantidad de procesos pares
    if (size < 2 || (size % 2) != 0) {
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
    printf("Dimensiones: %d, %d\n", dims[0], dims[1]);

    //Obtener datos del proceso
    int rank;
    int coords[2];
    int next_ranks[4];
    char* next_names[4] = {"up", "down", "left", "right"};
    enum DIRECTIONS {UP, DOWN, LEFT, RIGHT};

    MPI_Comm_rank(new_comm, &rank);
    MPI_Cart_coords(new_comm, rank, 2, coords);
    MPI_Cart_shift(new_comm, 0, 1, &next_ranks[UP], &next_ranks[DOWN]);
    MPI_Cart_shift(new_comm, 1, 1, &next_ranks[LEFT], &next_ranks[RIGHT]);
    printf("[MPI process %d] Coords: (%d, %d).\n", rank, coords[0], coords[1]);
    printf("[MPI process %d] Neighbours:", rank);
    for (i = 0; i < 4; i++)
        printf(" %s: %d;", next_names[i], next_ranks[i]);
    printf("\n");

    //Repartir datos
    int chunk_lengths[2];
    int chunk_remains[2];
    enum DIMENSIONS {ROWS, COLS};
    chunk_lengths[ROWS] = rows / dims[ROWS];
    chunk_lengths[COLS] = cols / dims[COLS];
    chunk_remains[ROWS] = rows % dims[ROWS];
    chunk_remains[COLS] = cols % dims[COLS];

    //Repartir resto a los primeros procesos de la topología
    if (coords[0] < chunk_remains[ROWS])
        chunk_lengths[ROWS]++;
    if (coords[1] < chunk_remains[COLS])
        chunk_lengths[COLS]++;

    //Crear tipo de datos para la porción de datos del proceso
    MPI_Datatype chunk_col_type;
    MPI_Type_vector(cols, chunk_lengths[COLS], cols, MPI_CHAR, &chunk_col_type);
    MPI_Type_commit(&chunk_col_type);

    printf("[MPI process %d] Chunk col length: (%d, %d).\n", rank, chunk_lengths[ROWS], chunk_lengths[COLS]);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
