#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <limits.h>
#define N 10000 // dimensione del vettore da ordinare

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
    int *sendcounts;
    int *displ;
    int lenC;
    int quoz;

    // inizializzazione ambiente MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

    // Stampa del numero di ranks
    //printf("%d\n\n", n_ranks);

    quoz = N / n_ranks;

    if (rank == 0)
    {
        dim = quoz;
        // il proc. 0 alloca lo spazio necessario per l'intero vettore
        full_vector = (int *)malloc(N * sizeof(int));
        // riempimento del vettore
        for (i = 0; i < N; i++)
            full_vector[i] = rand() % 7000;
        for (i = 0; i < N; i++)
            printf("%d ", full_vector[i]);
        printf("\n\n");
    }

    if (rank > 0 && rank < n_ranks - 1)
        dim = quoz;

    if (rank == n_ranks - 1)
        dim = quoz + N % n_ranks;

    
    displ = (int *)malloc(n_ranks * sizeof(int));
    sendcounts = (int *)malloc(n_ranks * sizeof(int));

    for (i = 0; i < n_ranks; i++)
    {
        displ[i] = i * quoz;
        if (i == n_ranks - 1)
            sendcounts[i] = quoz + N % n_ranks;
        else
            sendcounts[i] = quoz;
    }

    piece_of_vector = (int *)malloc(sendcounts[rank] * sizeof(int));

    MPI_Scatterv(full_vector, sendcounts, displ, MPI_INT, piece_of_vector, sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    /*-------------------------------MIN E MAX-------------------------------------------------------------*/
    double start_counting_sort = MPI_Wtime();

    for (i = 0; i < dim; i++)
    {
        if (piece_of_vector[i] < local_min)
            local_min = piece_of_vector[i];
        else if (piece_of_vector[i] > local_max)
            local_max = piece_of_vector[i];
    }

    MPI_Allreduce(&local_min, &min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    MPI_Allreduce(&local_max, &max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    /*----------------------------------------------------------------------------------------------------*/

    /*--------------------------COMPUTE C------------------------------------------------------------------*/
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

    free(piece_of_vector);
    free(sendcounts);
    free(displ);

    MPI_Reduce(c_local, c, lenC, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    free(c_local);
    /*-----------------------------------------------------------------------------------------------------*/

    /*CAPIRE COME FUNZIONA SU CARTA IL SEGUENTE PEZZO DI ALGORITMO NON PARALLELIZZABILE*/
    /*----------------------------------------------------------------------------------------------------*/
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

        double end_counting_sort = MPI_Wtime(); // eventualmente da spostare sotto le free() quando si toglierà la stampa sottostante
        /*-----------------------------------------------------------------------------------------------------*/

        for (i = 0; i < N; i++)
        {
            printf("%d ", full_vector[i]);
        }
        printf("\n\n");
    }

    if (rank == 0)
    {
        free(c);
        free(full_vector);
    }


    MPI_Finalize();
}