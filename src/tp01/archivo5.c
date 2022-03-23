
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	int num;
	int *p1;
	int **p2;

	num = 123;
	p1 = &num;
	p2 = &p1;
	*p1 = num - 23; // num = 123 - 23 = 100
	**p2 = *p1 * 2; // num = 100 * 2 = 200

	printf("Valor de num = %d\n", num);

	return 0;
}
