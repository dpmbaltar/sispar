#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

int main()
{
    int i, elementos = 1000000;
    int offset, iters;
    int *v;

    __m256i v1, v2, v3;
    __m256i iaux1, iaux2, iaux3;
    __m256i daux1, daux2;
    __m256i cmp1, cmp2, cmp3;

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

    daux1 = _mm256_set1_epi32(2);
    daux2 = _mm256_set1_epi32(-2);
    iaux2 = _mm256_set1_epi32(10);
    iaux3 = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7); //i offsets

    for (i = 0; i < iters; i++) {
        v1 = _mm256_load_si256((__m256i const *)(v+(i*offset)));

        iaux1 = _mm256_set1_epi32(i*offset);
        iaux1 = _mm256_add_epi32(iaux1, iaux3);
        iaux1 = _mm256_mullo_epi32(iaux1, iaux2);
        cmp1 = _mm256_cmpgt_epi32(iaux1, v1); // v[i] < i*10?
        cmp1 = _mm256_and_si256(cmp1, v1);
        v2 = _mm256_mullo_epi32(cmp1, daux1); // v2 = v[i] * 2

        cmp2 = _mm256_cmpgt_epi32(v1, iaux1); // v[i] > i*10?
        cmp3 = _mm256_cmpeq_epi32(v1, iaux1); // v[i] = i*10
        cmp2 = _mm256_or_si256(cmp2, cmp3); // v[i] >= i*10
        cmp2 = _mm256_and_si256(cmp2, v1);
        v3 = _mm256_mullo_epi32(cmp2, daux2); // v3 = v[i] * -2

        v1 = _mm256_add_epi32(v2, v3);

        _mm256_store_si256((__m256i *)(v+(i*offset)), v1);
    }

    printf("v[0]=%11d, ", v[0]);
    printf("v[11]=%11d, ", v[11]);
    printf("v[elementos-1]=%11d\n", v[elementos-1]);

    return 0;
}
