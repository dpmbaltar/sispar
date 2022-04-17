#include <stdio.h>
#include <stdlib.h>

#define X 50000
#define Y 50000

int main_nopt()
{
    int i, j;
    int **m;

    m = malloc(X * sizeof(int*));
    for (i = 0; i < X; i++)
        m[i] = malloc(Y * sizeof(int));
    for(i = 0; i < X; i++)
        for(j = 0; j < Y; j++)
            m[i][j] = j % 2;

    return 0;
}

int main_opt()
{
    int i, j;
    char **m;

    m = malloc(X * sizeof(char*));
    for (i = 0; i < X; i++)
        m[i] = malloc(Y * sizeof(char));
    for(i = 0; i < X; i++)
        for(j = 0; j < Y; j++)
            m[i][j] = j % 2;

    return 0;
}

/**
 * main_nopt:
 * total          9803032K
 * main_opt:
 * total          2445120K
 */
int main()
{
    //return main_nopt();
    return main_opt();
}
