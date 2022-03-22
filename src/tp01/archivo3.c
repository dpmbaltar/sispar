
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	float arr[5];
	*(arr+4) = 2.71;

	printf("Valor del quinto elemento: %f\n", arr[4]);

	return 0;
}
