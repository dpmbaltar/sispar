#include <stdio.h>
#include <time.h>
#include <unistd.h>

double sampleTime() {
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
   return ((double)tv.tv_sec+((double)tv.tv_nsec)/1000000000.0);
}

int main(){
   double t;
   t = sampleTime();
   sleep(1); //aquí va la tarea que quiero medir
   t = sampleTime() - t;
   printf("tiempo transcurrido: %f segundos\n", t);
   return 0;
}
