
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Ejercicio 5.3.a
 */
int main() {
#define ROWS 3
#define COLS 4
	int **arr;
	int n = 1;

	printf("Tamaño de entero: %li\n", sizeof(int));
	printf("Tamaño de puntero: %li\n", sizeof(int*));

	arr = (int**) malloc(sizeof(int*) * ROWS);
	for (int i = 0; i < ROWS; i++)
		arr[i] = (int*) malloc(sizeof(int) * COLS);

	printf("Dirección del arreglo: %x\n", arr); // &arr[0]

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			arr[i][j] = n++;
			printf("Elemento[%d][%d]: %d - Dirección: %x\n", i, j, arr[i][j], &arr[i][j]);
		}
	}

	// Liberar arreglo
	for (int i = 0; i < ROWS; i++) {
		free(arr[i]);
		arr[i] = NULL;
	}
	free(arr);
	arr = NULL;
#undef ROWS
#undef COLS

	return 0;
}
