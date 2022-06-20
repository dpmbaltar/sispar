#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
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

    //Se reserva memoria dinámica para la matriz de celdas, representada por el
    //arreglo de punteros "old"
    char **old;
    old = malloc(nrows * sizeof (char*));
    for (i = 0; i < nrows; i++) {
        old[i] = (char *) malloc((ncols) * sizeof (char));
    }

    //Inicializa elementos de la matriz "old" con 0 o 1 segun el patron de entrada
    i = 1;
    s = malloc(cols + 2);
    res = fgets(s, cols + 2, f);

    while (i <= rows && res != NULL) {
        for (j = 0; j < strlen(s) - 1; j++)
            old[i][j + 1] = (s[j] == '.') ? 0 : 1;
        for (j = strlen(s) - 1; j < cols+2; j++)
            old[i][j+1] = 0;
        res = fgets(s, cols + 2, f);
        i++;
    };

    //Para las últimas filas si no están "dibujadas" en el patrón
    for (j = i; j < nrows - 1; j++)
        memset(old[j], 0, cols+2);

    fclose(f); //Se cierra el archivo
    free(s); //Se libera la memoria utilizada para recorrer el archivo

    //Copias para hacer el mundo toroidal
    for (j = 1; j < ncols - 1; j++) { //Copia las columnas
        old[0][j] = old[nrows - 2][j];
        old[nrows - 1][j] = old[1][j];
    }

    for (i = 1; i < nrows - 1; i++) { //Copia las filas
        old[i][0] = old[i][ncols - 2];
        old[i][ncols - 1] = old[i][1];
    }

    //Copia las 4 esquinas
    old[0][0] = old[nrows - 2][ncols - 2];
    old[nrows - 1][0] = old[1][ncols - 2];
    old[0][ncols - 1] = old[nrows - 2][1];
    old[nrows - 1][ncols - 1] = old[1][1];

    //CALCULAR EL SIGUIENTE ESTADO DE MUNDO
    char **aux;
    char **nextStep; //Representa el estado siguiente del mundo
    nextStep = malloc(nrows * sizeof (char*));
    //aux = malloc(nrows * sizeof (char*)); // No es necesario?

    //Reserva memoria dinámica para el siguiente estado de mundo
    for (i = 0; i < nrows; i++)
        nextStep[i] = (char *) malloc((cols + 2) * sizeof (char));

    int c;
    int cant1;
    for (c = 0; c < steps; c++) {
        //Se recorre la matriz y se calcula el valor de su estado siguiente en
        //la matriz nextStep
        for (i = 1; i <= rows; i++) {
            for (j = 1; j <= cols; j++) {
                //Suma las celdas vecinas para saber cuantas están vivas
                cant1 = old[i - 1][j - 1] + old[i - 1][j] + old[i - 1][j + 1] + old[i][j - 1] + old[i][j + 1] + old[i + 1][j - 1] + old[i + 1][j] + old[i + 1][j + 1];

                if (old[i][j] == 1) //si tiene 2 o 3 vecinas vivas, sigue viva
                    nextStep[i][j] = ((cant1 == 2 || cant1 == 3)) ? 1 : 0;
                else //Si está muerta y tiene 3 vecinas vivas revive
                    nextStep[i][j] = (cant1 == 3) ? 1 : 0;
            }
        }

        //Copia los lados correspondientes en los lados auxiliares
        for (j = 1; j < ncols - 1; j++) { //Copia columnas
            nextStep[0][j] = nextStep[nrows - 2][j];
            nextStep[nrows - 1][j] = nextStep[1][j];
        }

        for (i = 1; i < nrows - 1; i++) { //Copia filas
            nextStep[i][0] = nextStep[i][ncols - 2];
            nextStep[i][ncols - 1] = nextStep[i][1];
        }

        //Copia las esquinas
        nextStep[0][0] = nextStep[nrows - 2][ncols - 2];
        nextStep[nrows - 1][0] = nextStep[1][ncols - 2];
        nextStep[0][ncols - 1] = nextStep[nrows - 2][1];
        nextStep[nrows - 1][ncols - 1] = nextStep[1][1];

        //Intercambio de punteros, para realizar el siguiente paso con los nuevos
        //estados calculados
        aux = old;
        old = nextStep;
        nextStep = aux;
    }

    //IMPRESIÓN DE LA CABECERA Y DEL ÚLTIMO ESTADO LUEGO DE EJECUTAR LOS STEPS
    printf("cols %d \nrows %d \nsteps %d \n", cols, rows, steps);
    for (i = 1; i <= rows; i++) {
        for (j = 1; j <= cols; j++) {
            printf("%c", old[i][j] == 1 ? 'O' : '.');
        }
        printf("\n");
    }

    return 0;
}
