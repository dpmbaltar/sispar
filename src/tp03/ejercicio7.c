#include <stdlib.h>

#define X 1000000

/**
 * 7) Evitar saltos condicionales
 */
int main() {
    int i, j, **m;

    m = malloc(900 * sizeof(int*));
    for (i = 0; i < 900; i++)
        m[i] = malloc(X * sizeof(int));

    //real    0m5,436s
    //user    0m3,731s
    //sys     0m1,690s
    /*for(i = 0; i < 900; i++)
        for(j = 0; j < X; j++)
            switch (i) {
                case 0 ... 299: m[i][j] = 0; break;
                case 300 ... 599: m[i][j] = 1; break;
                default: m[i][j] = 2; break;
            }*/

    for(i = 0; i < 300; i++)
        for(j = 0; j < X; j++)
            m[i][j] = 0;
    for(i = 300; i < 600; i++)
        for(j = 0; j < X; j++)
            m[i][j] = 1;
    for(i = 600; i < 900; i++)
        for(j = 0; j < X; j++)
            m[i][j] = 2;

    return 0;
}
