
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Ejercicio 5.3.b
 */
int main() {
#define ROWS 3
#define COLS 4
#define M(a,b) (((a)*ROWS)+(b))
	int *arr;
	int n = 1;

	printf("Tamaño de entero: %li\n", sizeof(int));
	printf("Tamaño de puntero: %li\n", sizeof(int*));
	printf("Tamaño de matriz: %li\n", sizeof(int) * ROWS * COLS);

	arr = (int*) malloc(sizeof(int) * ROWS * COLS);

	printf("Dirección del arreglo: %x\n", arr); // &arr[0]

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			arr[M(i, j)] = n++;
			printf("Elemento[%d][%d]: %d - Dirección: %x\n", i, j, arr[M(i, j)], &arr[M(i, j)]);
		}
	}

	// Liberar arreglo
	free(arr);
	arr = NULL;
#undef ROWS
#undef COLS
#undef M

	return 0;
}
