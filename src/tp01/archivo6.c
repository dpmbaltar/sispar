
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	/*int *p;
	p = (int *) malloc(sizeof(int)); // Bloque de memoria para un entero
	*p = 50; // Asignar 50 al valor del entero que apunta p*/

	int *p;
	p = (int*) malloc(sizeof(int) * 5);
	p[0] = 1;
	p[1] = 2;
	p[2] = 3;
	p[3] = 4;
	p[4] = 5;

	for (int i = 0; i < 5; i++)
		printf("p[%d] = %x\n", i, p[i]);

	return 0;
}
