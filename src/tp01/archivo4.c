
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
#define ROWS 3
#define COLS 4
	int arr[ROWS][COLS];
	int n = 1;

	printf("Tamaño de entero: %li\n", sizeof(int));
	printf("Tamaño de puntero: %li\n", sizeof(int*));
	printf("Dirección del arreglo: %x\n", arr); // &arr[0]

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			arr[i][j] = n++;
			printf("Elemento[%d][%d]: %d - Dirección: %x\n", i, j, arr[i][j], &arr[i][j]);
		}
	}

	// Distintas formas de acceder al elemento 1,1 usando punteros
	// (funciona solo asignado en la pila)
	printf("Elemento[1][1]: %d\n", *(*(arr)+5));
	printf("Elemento[1][1]: %d\n", *(*(arr+1)+1));
	printf("Elemento[1][1]: %d\n", *(*(arr+2)-3));
#undef ROWS
#undef COLS

	return 0;
}
