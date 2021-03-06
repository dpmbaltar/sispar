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

    { //región paralela
        var = inc(var);
        printf("var (in): %d\n", var);
    }

    printf("var (out): %d\n", var);
}
