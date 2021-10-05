#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>


int rank, size;
double media = 0,f;

void mat_vec_mult(double local_A[], double local_x[], double local_y[], int local_n) {
  int local_i, local_j;
  double start, finish;
  start = MPI_Wtime();

  //CARGAMOS TODO LOS PROCESADORES CON EL INICIO DEL ARREGLO Y CUANDO TIENEN QUE 
  //ITERAR
  MPI_Bcast(local_x, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  for (local_i = 0; local_i < local_n/size; local_i++) {
    local_y[local_i] = 0.0;
    for (local_j = 0; local_j < local_n; local_j++)
    {
      //USAMOS LA FORMULA PROPORCIONADA POR EL LIBRO PARA PODER CALCULAR LA MULTIPLICACION DE LA
      //FILA Y LA COLUMNA 
      local_y[local_i] += local_A[local_i * local_n + local_j] * local_x[local_j];
    }
      
  }
  finish = MPI_Wtime();
  media += finish - start;
  printf("EL tiempo transcurrido es de = %f seconds\n", (finish - start));
}

int main(int argc, char* argv){

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  //GENEREMOS EL ARREGLO Y EL VECTOR INICIAL
  int n = 8192;
  double* A;
  double *x;
  double *y;
  double *local_y;
  double *local_A;
  x = malloc(n * sizeof(double));
  //PRE CALCULAMOS LA MATRIX Y EL VECTOR RESULTATE PARA LA OPERACION
  if(rank == 0){
    A = malloc(n * n * sizeof(double));
    y = malloc(n * sizeof(double));
    
    //LLENAMOS LOS ARREGLOS
    for(int i = 0; i < n * n; i++)
    {
      A[i] = rand() % 100;
    }
      
    for(int i = 0; i < n; i++)
    {
      x[i] = rand() % 100;
      y[i] = 0.0;
    }
  }
  double tam = (n * n) / size;
  local_A = malloc(tam * sizeof(double));

  local_y = malloc((n/size) * sizeof(double));

  

  MPI_Scatter(A, tam, MPI_DOUBLE, local_A, tam, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  mat_vec_mult(local_A, x, local_y, n);

  MPI_Gather(local_y, n/size, MPI_DOUBLE, y, n/size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  free(local_A);
  free(local_y);
  free(x);

  if(rank == 0){
    printf("MEDIA: %f seconds\n", media);
    free(A);
    free(y);
    
  }
  MPI_Finalize();
  return 0;
}