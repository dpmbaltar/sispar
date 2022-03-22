
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	int a, b; // Se declaran 2 variables enteras a y b
	int *x, *y; // Se declaran 2 punteros a enteros x e y

	a = 10; // Se asigna el valor 10 a a
	b = 20; // Se asigna el valor 20 a b
	x = &a; // Se asigna la dirección de a a x
	y = &b; // Se asigna la dirección de b a y
	x = y; // Se asigna la dirección almacenada en y (de b) a x
	*x = 5; // Se asigna el valor 5 a la variable que apunta x (b)

	printf("Valor de a: %d\n", a);
	printf("Valor de b: %d\n", b);
	printf("Dirección de a: %x\n", &a);
	printf("Dirección de b: %x\n", &b);
	printf("Valor de x: %x\n", x);
	printf("Valor de y: %x\n", y);
	printf("Valor de *x: %d\n", *x);
	printf("Valor de *y: %d\n", *y);

	return 0;
}
