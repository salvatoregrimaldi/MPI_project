#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <limits.h>
#define N 50


int main(int argc, char ** argv){
    /*definizioni variabili*/
    int i;
    int dim;
    int rank;
    int n_ranks;
    int local_min = INT_MAX;
    int local_max = INT_MIN;
    int min;
    int max;
    int *full_vector;
    int *piece_of_vector;
    int *c_local;
    int lenC;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

    //printf("n_ranks=%d\n", n_ranks);

    if(rank == 0){
        //printf("Ciao sono proc. %d\n", rank);
        full_vector = (int *)malloc(N*sizeof(int)); 
        dim = N/n_ranks;
    }

    if(rank > 0 && rank < n_ranks-1){
        //printf("Ciao sono proc. %d\n", rank);
        dim = N/n_ranks;
    }

    if(rank == n_ranks-1){
        //printf("Ciao sono proc. %d\n", rank);
        dim = N/n_ranks + N % n_ranks;
    }

    piece_of_vector = (int *)malloc(dim*sizeof(int));
    for (i = 0; i < dim; i++){
            piece_of_vector[i] = rand() % 16;
            if(piece_of_vector[i] < local_min)
                local_min = piece_of_vector[i]; 
            else if(piece_of_vector[i] > local_max)
                local_max = piece_of_vector[i];
        }
        /*for(i=0; i<dim; i++){
            printf("%d ", piece_of_vector[i]);
        }
        printf("\n");
        printf("min = %d\n", local_min);
        printf("max = %d\n", local_max);
        printf("\n\n");*/

        MPI_Allreduce(&local_min, &min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        MPI_Allreduce(&local_max, &max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

        if(rank == 0){
        printf("min = %d\n", min);
        printf("max = %d\n", max);
        printf("\n\n");
        }

        lenC = max - min + 1;
        c_local = (int *)malloc(lenC*sizeof(int));
        for (i = 0; i < lenC_local; i++)
            c_local[i] = 0;
        
        /*provare algoritmo mainS su carta e completare la parallelizzazione*/


    MPI_Finalize();


}