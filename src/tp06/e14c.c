#include <omp.h>
#include <stdio.h>

int main()
{
    #pragma omp parallel num_threads (2)
    {
        int x;
        int id = omp_get_thread_num();
        int taskid;
        for (x = 0; x < 10; x++) {
            #pragma omp task
            {
                taskid = omp_get_thread_num();
                printf("hola %d tarea %d\n", id, taskid);
            }
        }
        #pragma omp taskwait
        printf("chau %d\n", id);
    }
}
