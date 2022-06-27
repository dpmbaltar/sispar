#include <stdio.h>
#include <unistd.h>
#include <omp.h>

int inc(int var)
{
    usleep(1000);
    return var + 1;
}

void main(int argc, char **argv)
{
    int var = 10;

    #pragma omp parallel num_threads(4) shared(var)
    {
        var = inc(var);
        printf("var (in): %d\n", var);
    }

    printf("var (out): %d\n", var);
}
