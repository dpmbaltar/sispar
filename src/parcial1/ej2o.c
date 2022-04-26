#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

int main()
{
    int i, elementos = 1000000;
    int offset, iters;
    int *v;
    int v_offset;

    __m256i v1, v2, v3;
    __m256i aux1, aux2;
    __m256i iaux1, iaux2;
    __m256i cmp1;

    offset = 256 / (8 * sizeof(int));
    iters = elementos / offset;

    if (posix_memalign((void**)&v, 32, elementos * sizeof(int)) != 0)
        return 1;

    // no vectorizar este for
    for (i = 0; i < elementos; i++)
        v[i] = rand() % 10000;

    printf("v[0]=%11d, ", v[0]);
    printf("v[11]=%11d, ", v[11]);
    printf("v[elementos-1]=%11d\n", v[elementos-1]);

    aux1 = _mm256_set1_epi32(2);
    aux2 = _mm256_set1_epi32(-2);
    iaux1 = _mm256_set_epi32(0, 10, 20, 30, 40, 50, 60, 70); //i*10 start
    iaux2 = _mm256_set1_epi32(80); //i*10 step

    for (i = 0; i < iters; i++) {
        v_offset = i * offset;

        // cargar vector
        v1 = _mm256_load_si256((__m256i const *)&v[v_offset]);

        // realizar comparación: v[i] < i*10
        cmp1 = _mm256_cmpgt_epi32(iaux1, v1);
        v3 = _mm256_and_si256(cmp1, aux1);
        v2 = _mm256_andnot_si256(cmp1, aux2);
        v2 = _mm256_or_si256(v2, v3);

        // computar resultado
        v1 = _mm256_mullo_epi32(v1, v2);

        // guardar resultado
        _mm256_store_si256((__m256i *)&v[v_offset], v1);

        iaux1 = _mm256_add_epi32(iaux1, iaux2);
    }

    printf("v[0]=%11d, ", v[0]);
    printf("v[11]=%11d, ", v[11]);
    printf("v[elementos-1]=%11d\n", v[elementos-1]);

    return 0;
}
