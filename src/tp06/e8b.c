#include <omp.h>
#include <stdlib.h>

int main()
{
    int i, j;
    omp_set_num_threads(omp_get_max_threads());
    #pragma omp parallel for schedule(dynamic) private(j)
    for (i = 0; i < 10000000; i++) {
        if (rand() % 2 == 0)
            for (j = 0; j < 100; j++);
        else
            for (j = 0; j < 1000; j++);
    }

    return 0;
}
