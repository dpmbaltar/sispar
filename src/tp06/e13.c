#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

void tarea_uno() {
    sleep(2);
}

void tarea_dos() {
    sleep(3);
}

void tarea_tres() {
    sleep(2);
}

int main () {
    double tiempoIni, tiempoFin;

    tiempoIni = omp_get_wtime();
    printf("Ejecutando tarea 1\n");
    tarea_uno();
    printf("Ejecutando tarea 2\n");
    tarea_dos();
    printf("Ejecutando tarea 3\n");
    tarea_tres();
    tiempoFin = omp_get_wtime();
    printf("Tiempo: %f segundos\n", tiempoFin - tiempoIni);

    return 0;
}
