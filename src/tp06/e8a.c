#include <omp.h>

int main()
{
    int i, j;

    #pragma omp parallel for schedule(____) private(j)
    for (i = 0; i < 100000000; i++)
        for (j = 0; j < 100; j++)

    return 0;
}
