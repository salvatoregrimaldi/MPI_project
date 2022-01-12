#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <limits.h>
#define N 50 // dimensione del vettore da ordinare

int main(int argc, char **argv)
{
    /*definizione variabili*/
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
    int *c;
    int *recvcounts;
    int *displ;
    int lenC;
    int quoz;

    // inizializzazione ambiente MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

    quoz = N / n_ranks;

    // L'inizializzazione randomica del vettore viene equamente distribuita tra i processi

    if (rank == 0)
    {
        // il proc. 0 alloca lo spazio necessario per l'intero vettore
        full_vector = (int *)malloc(N * sizeof(int));
        dim = quoz;
    }

    if (rank > 0 && rank < n_ranks - 1)
        dim = quoz;

    if (rank == n_ranks - 1)
        dim = quoz + N % n_ranks;

    recvcounts = (int *)malloc(n_ranks * sizeof(int));
    displ = (int *)malloc(n_ranks * sizeof(int));
    for (i = 0; i < n_ranks; i++)
    {
        displ[i] = i * quoz;
        // printf("displ[%d] = %d\n", i, displ[i]);
        if (i == n_ranks - 1)
            recvcounts[i] = quoz + N % n_ranks;
        else
            recvcounts[i] = quoz;
    }

    piece_of_vector = (int *)malloc(dim * sizeof(int));
    for (i = 0; i < dim; i++)
    {
        piece_of_vector[i] = rand() % 30;
        if (piece_of_vector[i] < local_min)
            local_min = piece_of_vector[i];
        else if (piece_of_vector[i] > local_max)
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

    /* if(rank == 0){
    printf("min = %d\n", min);
    printf("max = %d\n", max);
    printf("\n\n");
    } */

    lenC = max - min + 1;
    c_local = (int *)malloc(lenC * sizeof(int));
    for (i = 0; i < lenC; i++)
        c_local[i] = 0;

    for (i = 0; i < dim; i++)
        c_local[piece_of_vector[i] - min] += 1;

    for (i = 1; i < lenC; i++)
        c_local[i] += c_local[i - 1];

    if (rank == 0)
        c = (int *)malloc(lenC * sizeof(int));

    // // stampa di prova
    // for (i = 0; i < lenC; i++)
    //      printf("%d ", c_local[i]);
    // printf("        %d", lenC);
    // printf("\n");

    MPI_Reduce(c_local, c, lenC, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    free(c_local);

    // if (rank == 0)
    //  {
    //      for (i = 0; i < lenC; i++)
    //          printf("%d ", c[i]);
    //      printf("        %d", lenC);
    //      printf("\n");
    //  }

    MPI_Gatherv(piece_of_vector, dim, MPI_INT, full_vector, recvcounts, displ, MPI_INT, 0, MPI_COMM_WORLD);
    free(piece_of_vector);
    free(recvcounts);
    free(displ);

    if(rank == 0){
        for(i=0; i<N; i++){
            printf("%d ", full_vector[i]);
        }
        printf("\n\n");
    }


/*CAPIRE COME FUNZIONA SU CARTA IL SEGUENTE PEZZO DI ALGORITMO*/
/*-----------------------------------------------------------------------*/
    if (rank == 0)
    {
        int p;

    for (int j = 0; j < lenC; j++)
    {
        if (j == 0)
            p = 0;
        else
            p = c[j - 1];

        for (int x = p; x < c[j]; x++)
        {
            full_vector[x] = j + min;
        }
    }
/*------------------------------------------------------------------------*/

    for(i=0; i<N; i++){
        printf("%d ", full_vector[i]);
    }
    printf("\n\n");
    }

    if(rank == 0){
    free(c);
    free(full_vector);}

    MPI_Finalize();
}