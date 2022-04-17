#include <stdlib.h>

/**
 * real    0m0,271s
 * user    0m0,267s
 * sys     0m0,004s
 */
#define pordos(x) ((x)*2)

/**
 * real    0m1,601s
 * user    0m1,599s
 * sys     0m0,000s
 */
//inline static int pordos (int x) { return x * 2; }

/**
 * real    0m1,604s
 * user    0m1,602s
 * sys     0m0,000s
 */
//int pordos (int x) { return x * 2; }

int main() {
    register unsigned int i, res = 0;

    for (i = 0; i < 500000000; i++)
        res += pordos(i);

    return 0;
}
