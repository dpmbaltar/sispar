#include <stdio.h>
#include <unistd.h>
#include <omp.h>

int main(int argc, char **argv)
{
    int id;
    int num_threads = 0;

    #pragma omp parallel num_threads(10) private(id)
    {
        #pragma omp critical
        {
            if (!num_threads)
                num_threads = omp_get_num_threads();
        }
        id = omp_get_thread_num();
        usleep(500);
        printf("Soy el hilo %d de %d hilos...\n", id, num_threads);
    }

    printf("%d\n", num_threads);

    return 0;
}
