#include <stdio.h>

int complex_func(int n)
{
    return 1000 - n/2;
}

int main1()
{
    int flag = 0;
    int a[1000];

    for (int i = 0; i < 1000; i++) {
        if (complex_func(a[i]) < 55) {
            flag = 1;
            break; // Cortar una vez que el flag es 1
        }
    }

    printf("¿Elemento encontrado? %d\n", flag);

    return 0;
}

/**
 * float:
 * real    0m5,827s
 * user    0m5,817s
 * sys     0m0,000s
 *
 * int:
 * real    0m3,282s
 * user    0m3,279s
 * sys     0m0,000s
 *
 */
int main() {
    //float i, j, a;
    int i, j, a; // Usar int en vez de float

    for (i = 0; i < 1000; i++)
        for (j = 0; j < 1000000; j++)
            a = i + j;

    return 0;
}
