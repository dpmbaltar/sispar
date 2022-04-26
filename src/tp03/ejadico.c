#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

/**
 * TP3. Ejercicio Adicional.
 *
 * Consideraciones:
 * La condición (2 * v[i] < i * 10), se simplifica a (v[i] < i*5).
 * La operación ((v[i] - 2) * -2) se realiza como (v[i] * (-2) + 4).
 * En vez de computar i*5, se colocan los 8 primeros i*5 en un vector, y luego
 * se le suma la cantidad 8*5=40 a cada i, por cada iteración.
 *
 * Bucle "for" original:
 * for (i = 0; i < elementos; i++) {
 *     if(2 * v[i] < i * 10)
 *         v[i] = v[i] * 2;
 *     else
 *         v[i] = (v[i] - 2) * -2;
 * }
 *
 * Bucle "for" con los cambios mencionados:
 * for (i = 0; i < elementos; i++) {
 *     if(v[i] < i * 5)
 *         v[i] = v[i] * 2;
 *     else
 *         v[i] = v[i] * (-2) + 4;
 * }
 */
int main()
{
    int i, elementos = 1000005;
    int offset, iters;
    int *v;

    __m256i vec1, vec2, vec3, vec4;
    __m256i aux1, aux2, aux3;
    __m256i iaux1, iaux2;
    __m256i cmp1;

    offset = 256 / (8 * sizeof(int));
    iters = elementos / offset;

    // ¿se debe reservar los elementos restantes?
    // ¿puede producirse un acceso ilegal a la memoria?
    if ((elementos % offset) != 0)
        iters++;

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
    aux3 = _mm256_set1_epi32(4);
    iaux1 = _mm256_set_epi32(0, 5, 10, 15, 20, 25, 30, 35); //i*5 start
    iaux2 = _mm256_set1_epi32(40); //i*5 step

    for (i = 0; i < iters; i++) {
        // cargar vector
        vec1 = _mm256_load_si256((__m256i const *)v + i);

        // realizar comparación: v[i] < i*5
        cmp1 = _mm256_cmpgt_epi32(iaux1, vec1);
        vec2 = _mm256_and_si256(cmp1, aux1);
        vec3 = _mm256_andnot_si256(cmp1, aux2);
        vec4 = _mm256_or_si256(vec2, vec3);

        // computar resultado
        vec1 = _mm256_mullo_epi32(vec1, vec4);
        vec3 = _mm256_and_si256(vec3, aux3);
        vec1 = _mm256_add_epi32(vec1, vec3);

        // guardar resultado
        _mm256_store_si256((__m256i *)v + i, vec1);

        iaux1 = _mm256_add_epi32(iaux1, iaux2);
    }

    printf("v[0]=%11d, ", v[0]);
    printf("v[11]=%11d, ", v[11]);
    printf("v[elementos-1]=%11d\n", v[elementos-1]);

    return 0;
}
