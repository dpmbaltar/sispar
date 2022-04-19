#include <stdio.h>

/**
 * Extracción de subexpresiones comunes
 */
int main()
{
    int i, j, a;

    //real    0m3,344s
    //user    0m3,340s
    //sys     0m0,000s
    /*for (i = 0; i < 1000; i++)
        for (j = 0; j < 1000000; j++)
            a = i + j;*/

    //real    0m2,256s
    //user    0m2,252s
    //sys     0m0,000s
    /*for (i = 0; i < 1000; i++)
        for (j = 0; j < 1000000; j = j+2) {
            a = i + j;
            a = i + (j+1);
        }*/

    //real    0m1,998s
    //user    0m1,993s
    //sys     0m0,003s
    /*for (i = 0; i < 1000; i = i+2) {
        for (j = 0; j < 1000000; j = j+2) {
            a = i + j;
            a = i + (j+1);
        }
        for (j = 0; j < 1000000; j = j+2) {
            a = (i+1) + j;
            a = (i+1) + (j+1);
        }
    }*/

    for (i = 0; i < 1000; i = i+2) {
        for (j = 0; j < 1000000; j = j+2) {
            a = i + j;
            a = i + (j+1);
        }
        //a = (i+1);
        for (j = 0; j < 1000000; j = j+2) {
            a = (i+1) + j;
            a = (i+1) + (j+1);
        }
    }

    return 0;
}
