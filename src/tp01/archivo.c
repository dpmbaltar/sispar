
#include <stdio.h>

int main() {
	int x = 10;
	int *p;
	p = &x;
	printf("Dirección de x: %p \n", &x);
	printf("Dirección almacenada en p: %p \n", p);
	printf("Valor de *p: %d \n", *p);

	return 0;
}
