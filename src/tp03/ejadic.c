#include <stdlib.h>
#include <stdio.h>

int main() {

   int i, elementos = 1000005;
   int *v;

   v = malloc(elementos * sizeof(int));

   // no vectorizar este for
   for (i = 0; i < elementos; i++)
      v[i] = rand() % 10000;

   printf("v[0]=%11d, ", v[0]);
   printf("v[11]=%11d, ", v[11]);
   printf("v[elementos-1]=%11d\n", v[elementos-1]);

   for (i = 0; i < elementos; i++) {
      if(2 * v[i] < i * 10)
         v[i] = v[i] * 2;
      else
         v[i] = (v[i] - 2) * -2;
   }

   printf("v[0]=%11d, ", v[0]);
   printf("v[11]=%11d, ", v[11]);
   printf("v[elementos-1]=%11d\n", v[elementos-1]);

   return 0;
}
