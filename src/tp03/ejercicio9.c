
#include <stdlib.h>
#include <stdio.h>

#define N 80000000

int main()
{
    float *v;
    float sum = 0;

    v = (float*) malloc(sizeof(float) * N);
    if (v == NULL)
        return EXIT_FAILURE;

    // init
    for (int i = 0; i < N; i++)
        v[i] = (float)i;

    // sum
    for (int i = 0; i < N; i++)
        sum+= v[i];

    printf("suma = %.2f\n", sum);

    free(v);

    return EXIT_SUCCESS;
}
