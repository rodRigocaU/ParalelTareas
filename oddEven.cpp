#include <iostream>
#include <mpi.h>
#include <algorithm>
#include <fstream>
#include <cstring>

using namespace std;


void print(int *data, int rank, unsigned long data_size)
{
    cout << "rank " << rank << " : ";
    for(unsigned long int i=0; i<data_size; i++)
        cout << data[i] << " ";
    cout << endl;
}

int findPartner(int phase, int rank) {
    int partner;

    /* if it's an even phase */
    if (phase % 2 == 0) {
        /* if we are an even process */
        if (rank % 2 == 0) {
            partner = rank + 1;
        } else {
            partner = rank - 1;
        }
    } else {
        /* it's an odd phase - do the opposite */
        if (rank % 2 == 0) {
            partner = rank - 1;
        } else {
            partner = rank + 1;
        }
    }
    return partner;
}

int compare (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

void parallel_sort(int *data, int rank, int count_processes, unsigned long data_size)
{
    const unsigned long concat_data_size = data_size * 2;

    auto *other      = new int[data_size];
    auto *concatData = new int[concat_data_size];

    for (int i=0; i<count_processes; i++)
    {
        int partner = findPartner(i, rank);
        if (partner < 0 || partner >= count_processes)
          continue;

        if (rank % 2 == 0) {
            MPI_Send(data, (int) data_size, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(other, (int) data_size, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(other, (int) data_size, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(data, (int) data_size, MPI_INT, partner, 0, MPI_COMM_WORLD);
        }

        merge(data,  data  + data_size,
              other, other + data_size,
              concatData);
        qsort(concatData, concat_data_size, sizeof(int), compare);

        auto posHalfConcatData = concatData + data_size;
        if (rank < partner)
            copy(concatData, posHalfConcatData, data);
        else
            copy(posHalfConcatData, concatData + concat_data_size, data);
    }
}

/**
 * Compile:      mpic++ OddEvenSort.cpp -o OddEvenSort -std=gnu++0x
 * Example-Call: mpirun -np 4 ./OddEvenSort "<numbers_file.bin>" <y>
 * <y> output on console
 * */
int main(int argCount, char** argValues)
{
    int rank, count_processes;
    int n = 200;

    MPI_Init(&argCount, &argValues);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &count_processes);

    int *data;
    unsigned long data_size;
    int status = EXIT_SUCCESS;

    if(rank == 0)
    {
        data = (int*)malloc(sizeof(int)*n);
         for(i = 0; i < n; i++) {
            data[i] = rand()%100;
         }
    }

    if (status == EXIT_FAILURE) {
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    parallel_sort(data, rank, count_processes, data_size);

    if(argCount > 2 && strcmp(argValues[2], "y") == 0) {
        print(data, rank, data_size);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}