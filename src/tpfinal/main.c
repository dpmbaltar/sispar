/**
 * TP Final de Sistemas Paralelos 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

/**
 * Macro para calcular siguiente estado de un elemento.
 */
#define NEXT_STEP(i,j,neighbors,oldbuf,newbuf)                                 \
    if ((oldbuf)[(i)][(j)] == 1)                                               \
        (newbuf)[(i)][(j)] = (((neighbors) == 2 || (neighbors) == 3)) ? 1 : 0; \
    else                                                                       \
        (newbuf)[(i)][(j)] = ((neighbors) == 3) ? 1 : 0;

/**
 * Calcula la división mas optima de la grilla y retorna la partición
 * encontrada de la siguiente forma:
 * division[0] = cantidad de procesos por fila
 * division[1] = cantidad de procesos por columna
 *
 * El ultimo proceso de cada fila/columna procesará mas celdas
 */
void calcular_division(int rows, int cols, int cantProcesos, int division[])
{
  int resultado, i, menor = -1;

  for (i = 1; i <= cantProcesos; i++)
  {
    if (cantProcesos % i == 0)
    {
      resultado = fabs((rows / i) - (cols / (cantProcesos / i)));
      if (menor == -1)
      {
        menor = resultado;
      }
      if (resultado <= menor)
      {
        menor = resultado;
        division[0] = i;
        division[1] = cantProcesos / i;
      }
    }
  }
}

/**
 * Programa principal del enunciado.
 */
int main(int argc, char **argv)
{
    FILE *f;
    char *s, *res;
    int rows, cols, steps, n, i, j;
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

    //printf("Rows: %d cols: %d\n", rows, cols);

    //Se reserva memoria dinámica para la matriz "old"
    char(*old)[cols] = malloc(sizeof(char[rows][cols]));
    memset(old, 0, sizeof(char[rows][cols]));
    
    //Inicializa elementos de la matriz "old" con 0 o 1 segun el patron de entrada
    i = 0;
    s = (char*)malloc(cols + 2);
    res = fgets(s, cols + 2, f);

    while (i < rows && res != NULL) {
        for (j = 0; j < strlen(s) - 1; j++)
            old[i][j] = (s[j] == '.') ? 0 : 1;
        for (j = strlen(s) - 1; j < cols+2; j++)
            old[i][j] = 0;
        res = fgets(s, cols + 2, f);
        i++;
    };

    //Para las últimas filas si no están "dibujadas" en el patrón
    for (j = i; j < rows; j++)
        memset(&old[j], 0, cols+2);

    fclose(f); //Se cierra el archivo
    free(s); //Se libera la memoria utilizada para recorrer el archivo

    //Crear topología cartesiana de 2 dimensiones
    enum CartDimension {ROWS, COLS, N_DIMS};
    int dims[N_DIMS] = {0, 0};
    int periods[N_DIMS] = {true, true};
    int reorder = false;
    MPI_Comm new_comm;

    calcular_division(rows, cols, size, dims);
    //MPI_Dims_create(size, N_DIMS, dims);
    MPI_Cart_create(MPI_COMM_WORLD, N_DIMS, dims, periods, reorder, &new_comm);
    //printf("Dimensiones: %d, %d\n", dims[0], dims[1]);

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

    /*printf("[MPI process %d] Coords: (%d, %d).\n", rank, coords[0], coords[1]);
    printf("[MPI process %d] Neighbours:", rank);
    for (i = 0; i < 8; i++)
        printf(" %s: %d;", next_names[i], next_ranks[i]);
    printf("\n");*/

    //Establecer datos del proceso
    int chunk_rows = rows / dims[ROWS];
    int chunk_cols = cols / dims[COLS];
    int chunk_length = chunk_rows * chunk_cols;
    char(*old_buffer)[chunk_cols] = malloc(sizeof(char[chunk_rows][chunk_cols]));
    char(*new_buffer)[chunk_cols] = malloc(sizeof(char[chunk_rows][chunk_cols]));
    memset(old_buffer, 0, sizeof(char[chunk_rows][chunk_cols]));
    memset(new_buffer, 0, sizeof(char[chunk_rows][chunk_cols]));

    //printf("[MPI process %d] Chunk lengths: (%d, %d).\n", rank, chunk_rows, chunk_cols);

    MPI_Datatype chunk_type;
    MPI_Type_vector(chunk_rows, chunk_cols, cols, MPI_CHAR, &chunk_type);
    MPI_Type_commit(&chunk_type);

    if (rank == root) {
        //Mostrar datos cargados
        printf("Entrada:\n");
        printf("cols %d \nrows %d \nsteps %d \n", cols, rows, steps);
        for (i = 0; i < rows; i++) {
            for (j = 0; j < cols; j++) {
                printf("%c", old[i][j] == 1 ? 'O' : '.');
            }
            printf("\n");
        }

        int pos = 0;
        MPI_Pack(old, 1, chunk_type, old_buffer, chunk_length, &pos, new_comm);

        MPI_Status status[size-1];
        MPI_Request request[size-1];

        for (i = 1; i < size; i++) {
            int pcoords[2] = {0,0};
            MPI_Cart_coords(new_comm, i, N_DIMS, pcoords);
            int offset_rows = pcoords[0] * chunk_rows;
            int offset_cols = pcoords[1] * chunk_cols;
            MPI_Isend(&old[offset_rows][offset_cols], 1, chunk_type, i, 0, new_comm, &request[i-1]);
        }

        MPI_Waitall(size-1, request, status);
    } else { //Recibir datos
        MPI_Status status;
        MPI_Request request;

        MPI_Irecv(old_buffer, chunk_length, MPI_CHAR, root, 0, new_comm, &request);
        MPI_Wait(&request, &status);
    }

    //Mostrar trozo de datos
    /*for (i = 0; i < chunk_rows; i++) {
        for (j = 0; j < chunk_cols; j++)
            printf("%c", old_buffer[i][j] == 0 ? '.' : 'O');
        printf("\n");
    }*/

    /* Computar estados del mundo */
    int current_step;
    int live_neighbors;
    char corners[4] = {0, 0, 0, 0};
    char (*aux_buffer)[chunk_cols];
    char (*outer_rows)[chunk_cols] = malloc(sizeof(char[2][chunk_cols]));
    char (*outer_cols)[chunk_rows] = malloc(sizeof(char[2][chunk_rows]));
    memset(outer_rows, 0, sizeof(char[2][chunk_cols]));
    memset(outer_cols, 0, sizeof(char[2][chunk_rows]));

    MPI_Status send_status[8];
    MPI_Status recv_status[8];
    MPI_Request send_request[8];
    MPI_Request recv_request[8];

    //Tipo derivado para columna de la partición de datos
    MPI_Datatype chunk_col_type;
    MPI_Type_vector(chunk_rows, 1, chunk_cols, MPI_CHAR, &chunk_col_type);
    MPI_Type_commit(&chunk_col_type);

    for (current_step = 0; current_step < steps; current_step++) {

        //Enviar datos a los procesos vecinos
        MPI_Isend(&old_buffer[0][0], 1, MPI_CHAR, next_ranks[TOP_LEFT], TOP_LEFT, new_comm, &send_request[TOP_LEFT]);
        MPI_Isend(&old_buffer[0][chunk_cols-1], 1, MPI_CHAR, next_ranks[TOP_RIGHT], TOP_RIGHT, new_comm, &send_request[TOP_RIGHT]);
        MPI_Isend(&old_buffer[chunk_rows-1][0], 1, MPI_CHAR, next_ranks[BOTTOM_LEFT], BOTTOM_LEFT, new_comm, &send_request[BOTTOM_LEFT]);
        MPI_Isend(&old_buffer[chunk_rows-1][chunk_cols-1], 1, MPI_CHAR, next_ranks[BOTTOM_RIGHT], BOTTOM_RIGHT, new_comm, &send_request[BOTTOM_RIGHT]);
        MPI_Isend(&old_buffer[0][0], chunk_cols, MPI_CHAR, next_ranks[TOP], TOP, new_comm, &send_request[TOP]);
        MPI_Isend(&old_buffer[chunk_rows-1][0], chunk_cols, MPI_CHAR, next_ranks[BOTTOM], BOTTOM, new_comm, &send_request[BOTTOM]);
        MPI_Isend(&old_buffer[0][0], 1, chunk_col_type, next_ranks[LEFT], LEFT, new_comm, &send_request[LEFT]);
        MPI_Isend(&old_buffer[0][chunk_cols-1], 1, chunk_col_type, next_ranks[RIGHT], RIGHT, new_comm, &send_request[RIGHT]);

        //MPI_Waitall(8, send_request, send_status);

        //Calcular los estados internos
        for (i = 1; i < chunk_rows-1; i++) {
            for (j = 1; j < chunk_cols-1; j++) {
                //Suma las celdas vecinas para saber cuantas están vivas
                live_neighbors = old_buffer[i-1][j-1] + old_buffer[i-1][j] + old_buffer[i-1][j+1];
                live_neighbors+= old_buffer[i][j-1] + old_buffer[i][j+1];
                live_neighbors+= old_buffer[i+1][j-1] + old_buffer[i+1][j] + old_buffer[i+1][j+1];
                NEXT_STEP(i, j, live_neighbors, old_buffer, new_buffer);
            }
        }

        //Recibir datos de los procesos vecinos en forma no bloqueante
        MPI_Irecv(&corners[0], 1, MPI_CHAR, next_ranks[TOP_LEFT], BOTTOM_RIGHT, new_comm, &recv_request[TOP_LEFT]);
        MPI_Irecv(&corners[1], 1, MPI_CHAR, next_ranks[TOP_RIGHT], BOTTOM_LEFT, new_comm, &recv_request[TOP_RIGHT]);
        MPI_Irecv(&corners[2], 1, MPI_CHAR, next_ranks[BOTTOM_LEFT], TOP_RIGHT, new_comm, &recv_request[BOTTOM_LEFT]);
        MPI_Irecv(&corners[3], 1, MPI_CHAR, next_ranks[BOTTOM_RIGHT], TOP_LEFT, new_comm, &recv_request[BOTTOM_RIGHT]);
        MPI_Irecv(&outer_rows[0], chunk_cols, MPI_CHAR, next_ranks[TOP], BOTTOM, new_comm, &recv_request[TOP]);
        MPI_Irecv(&outer_rows[1], chunk_cols, MPI_CHAR, next_ranks[BOTTOM], TOP, new_comm, &recv_request[BOTTOM]);
        MPI_Irecv(&outer_cols[0], chunk_rows, MPI_CHAR, next_ranks[LEFT], RIGHT, new_comm, &recv_request[LEFT]);
        MPI_Irecv(&outer_cols[1], chunk_rows, MPI_CHAR, next_ranks[RIGHT], LEFT, new_comm, &recv_request[RIGHT]);

        //Recibir cambios de los vecinos y procesar los bordes necesarios
        MPI_Wait(&recv_request[TOP_LEFT], &recv_status[TOP_LEFT]);
        MPI_Wait(&recv_request[TOP_RIGHT], &recv_status[TOP_RIGHT]);
        MPI_Wait(&recv_request[TOP], &recv_status[TOP]);
        MPI_Wait(&recv_request[LEFT], &recv_status[LEFT]);
        MPI_Wait(&recv_request[RIGHT], &recv_status[RIGHT]);

        //Procesar esquina superior izquierda
        live_neighbors = corners[0];
        live_neighbors+= outer_rows[0][0] + outer_rows[0][1];
        live_neighbors+= outer_cols[0][0] + outer_cols[0][1];
        live_neighbors+= old_buffer[0][1] + old_buffer[1][0] + old_buffer[1][1];
        NEXT_STEP(0, 0, live_neighbors, old_buffer, new_buffer);

        //Procesar esquina superior derecha
        live_neighbors = corners[1];
        live_neighbors+= outer_rows[0][chunk_cols-2] + outer_rows[0][chunk_cols-1];
        live_neighbors+= outer_cols[1][0] + outer_cols[1][1];
        live_neighbors+= old_buffer[0][chunk_cols-2] + old_buffer[1][chunk_cols-2] + old_buffer[1][chunk_cols-1];
        NEXT_STEP(0, chunk_cols-1, live_neighbors, old_buffer, new_buffer);

        //Procesar interior de la fila superior
        for (i = 1; i < chunk_cols-1; i++) {
            live_neighbors = old_buffer[0][i-1] + old_buffer[0][i+1];
            live_neighbors+= old_buffer[1][i-1] + old_buffer[1][i] + old_buffer[1][i+1];
            live_neighbors+= outer_rows[0][i-1] + outer_rows[0][i] + outer_rows[0][i+1];
            NEXT_STEP(0, i, live_neighbors, old_buffer, new_buffer);
        }

        //Recibir cambios de los vecinos y procesar los bordes necesarios
        MPI_Wait(&recv_request[BOTTOM_LEFT], &recv_status[BOTTOM_LEFT]);
        MPI_Wait(&recv_request[BOTTOM_RIGHT], &recv_status[BOTTOM_RIGHT]);
        MPI_Wait(&recv_request[BOTTOM], &recv_status[BOTTOM]);

        //Procesar interior de la columna izquierda
        for (i = 1; i < chunk_rows-1; i++) {
            live_neighbors = old_buffer[i-1][0] + old_buffer[i+1][0];
            live_neighbors+= old_buffer[i-1][1] + old_buffer[i][1] + old_buffer[i+1][1];
            live_neighbors+= outer_cols[0][i-1] + outer_cols[0][i] + outer_cols[0][i+1];
            NEXT_STEP(i, 0, live_neighbors, old_buffer, new_buffer);
        }

        //Procesar interior de la columna derecha
        for (i = 1; i < chunk_rows-1; i++) {
            live_neighbors = old_buffer[i-1][chunk_cols-1] + old_buffer[i+1][chunk_cols-1];
            live_neighbors+= old_buffer[i-1][chunk_cols-2] + old_buffer[i][chunk_cols-2] + old_buffer[i+1][chunk_cols-2];
            live_neighbors+= outer_cols[1][i-1] + outer_cols[1][i] + outer_cols[1][i+1];
            NEXT_STEP(i, chunk_cols-1, live_neighbors, old_buffer, new_buffer);
        }

        //Procesar esquina inferior izquierda
        live_neighbors = corners[2];
        live_neighbors+= outer_rows[1][0] + outer_rows[1][1];
        live_neighbors+= outer_cols[0][chunk_rows-2] + outer_cols[0][chunk_rows-1];
        live_neighbors+= old_buffer[chunk_rows-1][1] + old_buffer[chunk_rows-2][0] + old_buffer[chunk_rows-2][1];
        NEXT_STEP(chunk_rows-1, 0, live_neighbors, old_buffer, new_buffer);

        //Procesar esquina inferior derecha
        live_neighbors = corners[3];
        live_neighbors+= outer_rows[1][chunk_cols-2] + outer_rows[1][chunk_cols-1];
        live_neighbors+= outer_cols[1][chunk_rows-2] + outer_cols[1][chunk_rows-1];
        live_neighbors+= old_buffer[chunk_rows-1][chunk_cols-2] + old_buffer[chunk_rows-2][chunk_cols-2] + old_buffer[chunk_rows-2][chunk_cols-1];
        NEXT_STEP(chunk_rows-1, chunk_cols-1, live_neighbors, old_buffer, new_buffer);

        //Procesar interior de la fila inferior
        for (i = 1; i < chunk_cols-1; i++) {
            live_neighbors = old_buffer[chunk_rows-1][i-1] + old_buffer[chunk_rows-1][i+1];
            live_neighbors+= old_buffer[chunk_rows-2][i-1] + old_buffer[chunk_rows-2][i] + old_buffer[chunk_rows-2][i+1];
            live_neighbors+= outer_rows[1][i-1] + outer_rows[1][i] + outer_rows[1][i+1];
            NEXT_STEP(chunk_rows-1, i, live_neighbors, old_buffer, new_buffer);
        }

        MPI_Waitall(8, send_request, send_status);

        //Intercambio de punteros, para realizar el siguiente paso con los
        //nuevos estados calculados
        aux_buffer = old_buffer;
        old_buffer = new_buffer;
        new_buffer = aux_buffer;
    }

    /*printf("Datos locales finales:\n");
    for (i = 0; i < chunk_rows; i++) {
        for (j = 0; j < chunk_cols; j++)
            printf("%c", old_buffer[i][j] == 0 ? '.' : 'O');
        printf("\n");
    }*/

    //Recibir datos finales y mostrarlos por pantalla
    if (rank == root) {
        int position = 0;
        MPI_Status end_recv_status;
        MPI_Request end_recv_request;

        MPI_Unpack(old_buffer, chunk_length, &position, &old[0][0], 1, chunk_type, new_comm);

        for (i = 1; i < size; i++) {
            int pcoords[2] = {0,0};
            MPI_Cart_coords(new_comm, i, N_DIMS, pcoords);
            int offset_rows = pcoords[0] * chunk_rows;
            int offset_cols = pcoords[1] * chunk_cols;
            MPI_Irecv(&old[offset_rows][offset_cols], 1, chunk_type, i, 0, new_comm, &end_recv_request);
            MPI_Wait(&end_recv_request, &end_recv_status);
        }

        printf("Salida:\n");
        printf("cols %d \nrows %d \nsteps %d \n", cols, rows, steps);
        for (i = 0; i < rows; i++) {
            for (j = 0; j < cols; j++) {
                printf("%c", old[i][j] == 1 ? 'O' : '.');
            }
            printf("\n");
        }

        //Si hay restos, la salida difiere del programa serie
        if (rows % dims[ROWS] != 0 || cols % dims[COLS] != 0)
            printf("Hay resto de datos para %d filas, %d columnas y %d procesos!\n",
                rows, cols, size);
    } else { //Enviar los datos finales al proceso raíz
        MPI_Request end_send_request;
        MPI_Isend(old_buffer, chunk_length, MPI_CHAR, root, 0, new_comm, &end_send_request);
        MPI_Wait(&end_send_request, MPI_STATUS_IGNORE);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
