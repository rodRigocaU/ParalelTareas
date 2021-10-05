/**
* @file odd-even_parallel.c
* @brief Dado um vetor de inteiros criado anteriormente em forma de binário, 
* 	este algoritmo o ordena fazendo uso da técnica odd-even explorando processamento paralelo. 
* @author Israel Medeiros Fontes
* @since 23/10/2020
* @date 27/10/2020
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#undef DEBUG

void odd_even_sort(int a[], int n){

	int phase, i, temp;

	for(phase = 0; phase < n; phase++){
		if(phase%2 == 0){
			for(i = 1; i<n; i+=2){
				if(a[i-1]>a[i]){
					temp = a[i];
					a[i] = a[i-1];
					a[i-1] = temp;
				}
			}
		}else{
			for (i = 1; i < n-1; i += 2){
				if(a[i]>a[i+1]){
					temp = a[i];
					a[i] = a[i+1];
					a[i+1] = temp;					
				}
			}
		}
	}
}

void read_vector_in_file(int vector[], int n){
	
        //printf("vector %d", n);
         for(int i = 0; i < n; i++) {
            vector[i] = n - i;
            //printf("%d",vector[i]);
         }
}

MPI_Status status;

int main(int argc, char **argv){
	clock_t t;
	
	t = clock();

	int comm_sz,my_rank;
	
    int n = 320000;
    //atoi(argv[1]);
	int *vector = malloc(sizeof(int)*n);

	read_vector_in_file(vector,n);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	
	int local_n = n / comm_sz;
	int *local_vector = malloc(local_n*sizeof(int));
    //printf("ENTREAQUI ");
	
#ifdef DEBUG 
    //printf("ENTRE");
	if(my_rank == 0){
		printf("VECTOR_MASTER: %d - ", my_rank);
		for(int i=0;i<n	;i++){
			printf("%d ",vector[i]);
		}
		printf("\n\n");	
		//Inicia contagem do tempo
		t = clock();
	}
#endif

	MPI_Scatter(vector, local_n, MPI_INT, local_vector, local_n, MPI_INT, 0, MPI_COMM_WORLD);
		
	odd_even_sort(local_vector, local_n);
	
	int *temp_vector = malloc(2*local_n*sizeof(int));

    for(int phase = 0; phase < comm_sz; phase++){
	    
	    if(phase%2 == 0){
	    	
	    	if(my_rank%2 != 0)
				MPI_Send(local_vector, local_n, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD);

	    	if(my_rank%2 == 0){
				//int *temp_vector = malloc(2*local_n*sizeof(int));
	    		
	    		MPI_Recv(temp_vector, local_n, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				int ii = 0;
	    		for(int i = local_n; i<local_n*2;i++)
	    			temp_vector[i] = local_vector[ii++];
	    		
	    		odd_even_sort(temp_vector,local_n*2);
	    		
	    		for(int i = 0; i<local_n;i++)
	    			local_vector[i] = temp_vector[i];	
	    		
#ifdef DEBUG
	    		printf("Phase: %d | Proc: %d - ", phase, my_rank);
				for(int i=0;i<local_n	;i++)
					printf("%d ",local_vector[i]);			
				printf("\n");
#endif
	    		MPI_Send(&temp_vector[local_n], local_n, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD);
	    		
	    		//free(temp_vector);

	    	}else{

	    		MPI_Recv(local_vector, local_n, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#ifdef DEBUG
	    		printf("Phase: %d | Proc: %d - ", phase, my_rank);
				for(int i=0;i<local_n	;i++)
					printf("%d ",local_vector[i]);
				printf("\n");
#endif	
	    	}
		}else{
			if(my_rank%2==0 && my_rank>0){

				MPI_Send(local_vector, local_n, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD);

	    		MPI_Recv(local_vector, local_n, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);				
#ifdef DEBUG
				printf("Phase: %d | Proc: %d - ", phase, my_rank);
				for(int i=0;i<local_n	;i++)
					printf("%d ",local_vector[i]);			
				printf("\n");	
#endif

	    	}else if(my_rank%2 != 0 && my_rank<comm_sz-1){
	    		
	    		//int *temp_vector = malloc(2*local_n*sizeof(int));

	    		MPI_Recv(temp_vector, local_n, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    		
	    		int ii = 0;
	    		for(int i = local_n; i<local_n*2;i++)
	    			temp_vector[i] = local_vector[ii++];
	    		
	    		odd_even_sort(temp_vector,local_n*2);
	   
	    		for(int i = 0; i<local_n;i++)
	    			local_vector[i] = temp_vector[i];
#ifdef DEBUG
	    		printf("Phase: %d | Proc: %d - ", phase, my_rank);
				for(int i=0;i<local_n	;i++)
					printf("%d ",local_vector[i]);
				printf("\n");	
#endif
	    		MPI_Send(&temp_vector[local_n], local_n, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD);

	    		//free(temp_vector);
			}
	    		
		}
		
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gather(local_vector,local_n,MPI_INT,vector,local_n,MPI_INT,0,MPI_COMM_WORLD);
    
    if(my_rank == 0){
#ifdef DEBUG
    	printf("Final - ");
		for(int i=0;i<n	;i++){
			printf("%d ",vector[i]);
		}
		printf("\n");
#endif
		t = clock() - t;
		printf("TIME: %f\n", ((double)t)/((CLOCKS_PER_SEC)));
    }
    free(temp_vector);
	MPI_Finalize();

	return 0;
}