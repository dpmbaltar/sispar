
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

#define N 80000000

int main()
{
    int num_floats, num_iters;
    float *vec;
    float *res;
    float sum = 0;
    size_t float_size;
    __m256 vec1, vec2;

    float_size = sizeof(float);
    num_floats = 256 / (float_size * 8); //__m256 es un vector de 8 floats (registro de 256 bits)
    num_iters = N / num_floats;

    if(posix_memalign((void**)&vec, 32, float_size * N) != 0)
        return EXIT_FAILURE;

    if(posix_memalign((void**)&res, 32, num_floats) != 0)
        return EXIT_FAILURE;

    // init
    for (int i = 0; i < N; i++)
        vec[i] = (float)i;

    vec1 = _mm256_load_ps((const float*)vec);

    // sum
    for (int i = 1; i < num_iters; i++) {
        vec2 = _mm256_load_ps((const float*)vec + (i * num_floats));
        vec1 = _mm256_add_ps(vec1, vec2);
    }

    _mm256_store_ps((float*)res, vec1);

    for (int i = 0; i < num_floats; i++)
        sum += res[i];

    printf("suma = %.2f\n", sum);

    free(vec);

    return EXIT_SUCCESS;
}
