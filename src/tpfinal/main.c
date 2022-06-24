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

    int size, rank, root = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Permitir solo cantidad de procesos pares
    if (size < 2 || (size % 2) != 0) {
        printf("Error: debe ejecutarse con un número par de procesos\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

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

    //Se reserva memoria dinámica para la matriz de celdas, representada por el
    //arreglo de punteros "old"
    //char **old;
    //old = malloc(rows * sizeof (char*));
    /*for (i = 0; i < rows; i++) {
        old[i] = (char *) malloc((cols) * sizeof (char));
    }*/
    //char *old;
    //old = (char *)malloc(rows * cols * sizeof (char));
    //memset(old, 0, rows * cols * sizeof (char));
    char(*old)[cols] = malloc (sizeof(char[rows][cols]));
    memset(old, 0, sizeof(char[rows][cols]));
    
    //Inicializa elementos de la matriz "old" con 0 o 1 segun el patron de entrada
    i = 0;
    s = (char *)malloc(cols + 2);
    res = fgets(s, cols + 2, f);

    while (i < rows && res != NULL) {
        for (j = 0; j < strlen(s) - 1; j++)
            //old[i*cols+j] = (s[j] == '.') ? 0 : 1;
            old[i][j] = (s[j] == '.') ? 0 : 1;
        for (j = strlen(s) - 1; j < cols+2; j++)
            //old[i*cols+j] = 0;
            old[i][j] = 0;
        res = fgets(s, cols + 2, f);
        i++;
    };

    //Para las últimas filas si no están "dibujadas" en el patrón
    for (j = i; j < rows; j++)
        memset(&old[j], 0, cols+2);

    fclose(f); //Se cierra el archivo
    free(s); //Se libera la memoria utilizada para recorrer el archivo

    //MPI----------------------------------------------------------------------
    //Crear topología cartesiana de 2 dimensiones
    enum CartDimension {ROWS, COLS, N_DIMS};
    int dims[N_DIMS] = {0, 0};
    int periods[N_DIMS] = {true, true};
    int reorder = false;
    MPI_Comm new_comm;

    MPI_Dims_create(size, N_DIMS, dims);
    MPI_Cart_create(MPI_COMM_WORLD, N_DIMS, dims, periods, reorder, &new_comm);
    printf("Dimensiones: %d, %d\n", dims[0], dims[1]);

    //Obtener datos del proceso
    int coords[2];
    int next_ranks[8];
    char* next_names[8] = {"top", "bottom", "left", "right", "top_left", "top_right", "bottom_left", "bottom_right"};
    enum Direction {TOP, BOTTOM, LEFT, RIGHT, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};

    MPI_Comm_rank(new_comm, &rank);
    MPI_Cart_coords(new_comm, rank, 2, coords);
    MPI_Cart_shift(new_comm, 0, 1, &next_ranks[TOP], &next_ranks[BOTTOM]);
    MPI_Cart_shift(new_comm, 1, 1, &next_ranks[LEFT], &next_ranks[RIGHT]);

    int corner_coords[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
    for (i = 0; i < 4; i++) {
        corner_coords[i][0]+=coords[0];
        corner_coords[i][1]+=coords[1];
        MPI_Cart_rank(new_comm, corner_coords[i], &next_ranks[TOP_LEFT+i]);
    }

    printf("[MPI process %d] Coords: (%d, %d).\n", rank, coords[0], coords[1]);
    printf("[MPI process %d] Neighbours:", rank);
    for (i = 0; i < 8; i++)
        printf(" %s: %d;", next_names[i], next_ranks[i]);
    printf("\n");

    //Establecer datos del proceso
    //int chunk_lengths[N_DIMS]; //(cantidad de filas, cantidad de columnas)
    //int chunk_remains[N_DIMS]; //(resto de filas, resto de columnas)
    //chunk_lengths[ROWS] = rows / dims[ROWS];
    //chunk_lengths[COLS] = cols / dims[COLS];
    int chunk_rows = rows / dims[ROWS];
    int chunk_cols = cols / dims[COLS];
    int chunk_length = chunk_rows * chunk_cols;
    char(*old_buffer)[chunk_cols] = malloc(sizeof(char[chunk_rows][chunk_cols]));
    char(*new_buffer)[chunk_cols] = malloc(sizeof(char[chunk_rows][chunk_cols]));
    memset(old_buffer, 0, sizeof(sizeof(char[chunk_rows][chunk_cols])));
    memset(new_buffer, 0, sizeof(sizeof(char[chunk_rows][chunk_cols])));

    //chunk_remains[ROWS] = rows % dims[ROWS];
    //chunk_remains[COLS] = cols % dims[COLS];
    //Repartir resto a los primeros procesos de la topología
    /*if (coords[0] < chunk_remains[ROWS])
        chunk_lengths[ROWS]++;
    if (coords[1] < chunk_remains[COLS])
        chunk_lengths[COLS]++;*/

    printf("[MPI process %d] Chunk lengths: (%d, %d).\n", rank, chunk_rows, chunk_cols);

    MPI_Datatype chunk_type;
    MPI_Type_vector(chunk_rows, chunk_cols, cols, MPI_CHAR, &chunk_type);
    MPI_Type_commit(&chunk_type);

    if (rank == root) {
        /*int proc_rows = rows / dims[ROWS];
        int proc_cols = cols / dims[COLS];
        /int proc_rows_rem = rows % dims[ROWS];
        int proc_cols_rem = cols % dims[COLS];
        MPI_Status send_st;
        MPI_Request send_req;

        //Crear tipo de datos para la porción de datos del proceso:
        //Si la cantidad de datos por proceso no es igual para todos los procesos
        //entonces se necesitan 2 o 4 tipos derivados distintos:
        // TL | TR
        // ---+---
        // BL | BR
        enum ChunkType {BR, BL, TR, TL, N_TYPES};
        MPI_Datatype chunk_type[N_TYPES];
        MPI_Type_vector(proc_rows, proc_cols, cols, MPI_CHAR, &chunk_type[BR]);
        MPI_Type_commit(&chunk_type[BR]);

        if (proc_rows_rem > 0) {
            MPI_Type_vector(proc_rows+1, proc_cols, cols, MPI_CHAR, &chunk_type[BL]);
            MPI_Type_commit(&chunk_type[BL]);
        }
        if (proc_cols_rem > 0) {
            MPI_Type_vector(proc_rows, proc_cols+1, cols, MPI_CHAR, &chunk_type[TR]);
            MPI_Type_commit(&chunk_type[TR]);
        }
        if (proc_rows_rem > 0 && proc_cols_rem > 0) {
            MPI_Type_vector(proc_rows+1, proc_cols+1, cols, MPI_CHAR, &chunk_type[TL]);
            MPI_Type_commit(&chunk_type[TL]);
        }

        int group_sizes[4] = {0,0,0,0};
        int group_ranks[4][size];
        MPI_Group world_group;
        MPI_Group chunk_type_group[4];
        MPI_Comm chunk_type_comm[4];
        MPI_Comm_group(MPI_COMM_WORLD, &world_group);

        for (i = 0; i < dims[ROWS]; i++) {
            for (j = 0; i < dims[COLS]; i++) {
                if (i < proc_rows_rem && j < proc_cols_rem)
                    group_sizes[TL]++;
                    group_ranks[group_sizes[TL]] =
            }
        }*/
        
        //FALTA:
        //- Enviar correctamente los datos a todos los procesos
        //- Procesador los steps en cada proceso
        //- Comunicar los cambios entre procesos
        //MPI_Isend(&old[0][5], 1, chunk_type[TL], 1, 0, new_comm, &send_req);
        //MPI_Isend(&old[0][10], 1, chunk_type[TL], 2, 0, new_comm, &send_req);
        //MPI_Isend(&old[0][15], 1, chunk_type[TR], 3, 0, new_comm, &send_req);
        //MPI_Wait(&send_req, &send_st);

        //Mostrar datos cargados
        for (i = 0; i < rows; i++) {
            for (j = 0; j < cols; j++) {
                printf("%c", old[i][j] == 1 ? 'O' : '.');
            }
            printf("\n");
        }

        int pos = 0;
        MPI_Pack(old, 1, chunk_type, old_buffer, chunk_length, &pos, new_comm);

        MPI_Status status;
        MPI_Request request;

        for (i = 1; i < size; i++) {
            int pcoords[2] = {0,0};
            MPI_Cart_coords(new_comm, i, N_DIMS, pcoords);
            int offset_rows = pcoords[0] * chunk_rows;
            int offset_cols = pcoords[1] * chunk_cols;
            MPI_Isend(&old[offset_rows][offset_cols], 1, chunk_type, i, 0, new_comm, &request);
        }

        /*MPI_Scatter(old, 1, chunk_type,
                    old_buffer, chunk_length, MPI_CHAR,
                    root, new_comm);*/
    } else {
        /*MPI_Scatter(NULL, 1, chunk_type,
                    old_buffer, chunk_length, MPI_CHAR,
                    root, new_comm);*/
        MPI_Status status;
        MPI_Request request;

        MPI_Irecv(old_buffer, chunk_length, MPI_CHAR, root, 0, new_comm, &request);
        MPI_Wait(&request, &status);

    }

    //Recibir datos
    /*if (rank > 0 && rank < 4) { //if (rank > 0) {
        MPI_Status recv_st;
        MPI_Request recv_req;
        int ncells = chunk_lengths[ROWS] * chunk_lengths[COLS];
        char buf[chunk_lengths[ROWS]][chunk_lengths[COLS]];
        memset(buf, '\0', ncells);

        MPI_Irecv(buf, ncells, MPI_CHAR, 0, 0, new_comm, &recv_req);
        MPI_Wait(&recv_req, &recv_st);

        for (i = 0; i < chunk_lengths[ROWS]; i++) {
            for (j = 0; j < chunk_lengths[COLS]; j++)
                printf("%c", buf[i][j] == 0 ? '.' : 'O');
            printf("\n");
        }
        printf("\n");
    }*/

    for (i = 0; i < chunk_rows; i++) {
        for (j = 0; j < chunk_cols; j++)
            printf("%c", old_buffer[i][j] == 0 ? '.' : 'O');
        printf("\n");
    }

    /* Computar estados del mundo */
    int current_step;
    int live_neighbors;
    char corners[4] = {0, 0, 0, 0};
    char (*aux_buffer)[chunk_cols];
    char (*outer_rows)[chunk_cols];
    char (*outer_cols)[2];
    memset(outer_rows, 0, sizeof(sizeof(char[2][chunk_cols])));
    memset(outer_cols, 0, sizeof(sizeof(char[chunk_rows][2])));

    MPI_Status send_status[8];
    MPI_Status recv_status[8];
    MPI_Request send_request[8];
    MPI_Request recv_request[8];

    //Tipo derivado para columna de la partición de datos
    MPI_Datatype chunk_col_type;
    MPI_Type_vector(chunk_rows, 1, cols, MPI_CHAR, &chunk_col_type);
    MPI_Type_commit(&chunk_col_type);

    for (current_step = 0; current_step < steps; current_step++) {

        //Enviar datos a los procesos vecinos
        MPI_Isend(&old_buffer[0][0], 1, MPI_CHAR, next_ranks[TOP_LEFT], 0, new_comm, &send_request[TOP_LEFT]);
        MPI_Isend(&old_buffer[0][chunk_cols-1], 1, MPI_CHAR, next_ranks[TOP_RIGHT], 0, new_comm, &send_request[TOP_RIGHT]);
        MPI_Isend(&old_buffer[chunk_rows-1][0], 1, MPI_CHAR, next_ranks[BOTTOM_LEFT], 0, new_comm, &send_request[BOTTOM_LEFT]);
        MPI_Isend(&old_buffer[chunk_rows-1][chunk_cols-1], 1, MPI_CHAR, next_ranks[BOTTOM_RIGHT], 0, new_comm, &send_request[BOTTOM_RIGHT]);
        MPI_Isend(&old_buffer[0][0], chunk_cols, MPI_CHAR, next_ranks[TOP], 0, new_comm, &send_request[TOP]);
        MPI_Isend(&old_buffer[chunk_rows-1][0], chunk_cols, MPI_CHAR, next_ranks[BOTTOM], 0, new_comm, &send_request[BOTTOM]);
        MPI_Isend(&old_buffer[0][0], 1, chunk_col_type, next_ranks[LEFT], 0, new_comm, &send_request[LEFT]);
        MPI_Isend(&old_buffer[0][chunk_cols-1], 1, chunk_col_type, next_ranks[RIGHT], 0, new_com, &send_request[RIGHT]);

        //Recibir datos de los procesos vecinos en forma no bloqueante
        for (i = 0; i < 4; i++)
            MPI_Irecv(&corners[i], 1, MPI_CHAR, next_ranks[TOP_LEFT+i], 0, new_comm, &recv_request[TOP_LEFT+i]);
        MPI_Irecv(&outer_rows[0], chunk_cols, MPI_CHAR, next_ranks[TOP], 0, new_comm, &recv_request[TOP]);
        MPI_Irecv(&outer_rows[1], chunk_cols, MPI_CHAR, next_ranks[BOTTOM], 0, new_comm, &recv_request[BOTTOM]);
        MPI_Irecv(&outer_cols[0], chunk_rows, MPI_CHAR, next_ranks[LEFT], 0, new_comm, &recv_request[LEFT]);
        MPI_Irecv(&outer_cols[1], chunk_rows, MPI_CHAR, next_ranks[RIGHT], 0, new_comm, &recv_request[RIGHT]);

        //Calcular los estados internos
        for (i = 1; i < chunk_rows-1; i++) {
            for (j = 1; j < chunk_cols-1; j++) {
                //Suma las celdas vecinas para saber cuantas están vivas
                live_neighbors = old_buffer[i-1][j-1];
                live_neighbors+= old_buffer[i-1][j];
                live_neighbors+= old_buffer[i-1][j+1];
                live_neighbors+= old_buffer[i][j-1];
                live_neighbors+= old_buffer[i][j+1];
                live_neighbors+= old_buffer[i+1][j-1];
                live_neighbors+= old_buffer[i+1][j];
                live_neighbors+= old_buffer[i+1][j+1];

                if (old_buffer[i][j] == 1) //si tiene 2 o 3 vecinas vivas, sigue viva
                    new_buffer[i][j] = ((live_neighbors == 2 || live_neighbors == 3)) ? 1 : 0;
                else //Si está muerta y tiene 3 vecinas vivas revive
                    new_buffer[i][j] = (live_neighbors == 3) ? 1 : 0;
            }
        }

        //Recibir cambios de los vecinos y procesar los bordes


    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
