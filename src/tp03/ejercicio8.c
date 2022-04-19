#include <stdlib.h>
#include <stdio.h>

#define X 50000
#define Y 10000

int main() {
    int i, j;
    int **m;

    m = malloc(X * sizeof(int*));
    for (i = 0; i < X; i++)
        m[i] = malloc(Y * sizeof(int));

    for(j = 0; j < Y; j++)
        for(i = 0; i < X; i++)
            m[i][j] = i + j;

    return 0;
}
