#include <stdio.h>
#include <unistd.h>
#include <sched.h>

int main(int argc, char *argv[]) {
   char hostname[128];
   gethostname(hostname, sizeof(hostname));

   while (1) {
      printf("Ejecutando en nodo %s, core %d\n", hostname, sched_getcpu());
      sleep(1);
   }

   return 0;
}
