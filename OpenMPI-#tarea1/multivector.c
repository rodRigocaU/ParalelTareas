#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int thread_count;
long m, n;
long A[8][8000000];
long y[8];
long x[8000000];
double time_spent = 0.0;



void *Pth_mat_vect(void* rank)
{   
    
    clock_t begin = clock();
    long my_rank = (long) rank;
    int i, j;
    int local_m = m/thread_count;
    int my_first_row = my_rank*local_m;
    int my_last_row = (my_rank + 1) * local_m - 1;
    
    for(i = my_first_row; i <= my_last_row; i++)
    {
        y[i] = 0.0;
        for(j = 0; j < n; j++)
        {
            y[i] += A[i][j] * x[j];
        }
    }
    clock_t end = clock();
    time_spent  += (double)(end - begin) / CLOCKS_PER_SEC;
    return NULL;
}

int main(int argc, char * argv[]) 
{
    long thread;
    pthread_t* thread_handles;

    thread_count = strtol(argv[1], NULL, 10);

    thread_handles = malloc(thread_count*sizeof(pthread_t));

    m = 8;  
    n = 8000000;

    //PARA RELLENAR LOS ARREGLOS
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            A[i][j] = rand() % 50;
        }
    }
     for(int i = 0; i < n; i++)
    {
        x[i] = rand() % 50;
    }

     //clock_t begin = clock();
    for(thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread],NULL, Pth_mat_vect, (void*) thread);
    }

    for(thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }

    /*clock_t end = clock();
    time_spent  += (double)(end - begin) / CLOCKS_PER_SEC;
    free(thread_handles);*/
    time_spent = time_spent / thread_count;
    printf("El tiempo obtenido es equivalente a: %f segundos \n", time_spent);
    return 0;


}