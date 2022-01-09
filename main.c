#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 99

int main(int argc, char ** argv){
  int n_ranks;
  int rank;
  int *a;
  int *a_chunk;
  int *n_elements;
  int *receive;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

  n_elements = (int *)malloc(n_ranks*sizeof(int));

  if(rank==0){

    a = (int *)malloc(N*sizeof(int));
    //riempimento a
    for (int i = 0; i < N; i++)
    a[i] = rand() % 1001;

    receive = (int *)malloc(N*sizeof(int));
  }

  for(int i=0; i < n_ranks; i++){
    if(i == n_ranks-1)
    n_elements[i] = N/n_ranks + N%n_ranks;
    else
    n_elements[i] = N/n_ranks;
  }

  a_chunk = (int *)malloc(n_elements[rank]*sizeof(int));

  MPI_Scatter(a, n_elements[rank], MPI_INT, a_chunk, n_elements[rank], MPI_INT, 0, MPI_COMM_WORLD);

  //ordinamento
  int max = a_chunk[0];
  int min = a_chunk[0];
  int i = 1;
  for(int i=1; i<n_elements[rank]; i++){
    if(a_chunk[i]>max)
    max = a_chunk[i];
    if(a_chunk[i]<min)
    min=a_chunk[i];
  }
  //costruzione dell'array c (inizializzazione)
  int *c = (int *)malloc((max - min + 1) * sizeof(int));
  for (i = 0; i < max - min + 1; i++)
  c[i] = 0;

  for (i = 0; i < n_elements[rank]; i++)
  c[a_chunk[i] - min] += 1;
  int k = 0;
  for (i = 0; i < max - min + 1; i++)
  while (c[i] > 0)
  {
    a_chunk[k] = i + min;
    k = k + 1;
    c[i] = c[i] - 1;
  }
  free(c);

  for(int i=0;i<n_elements[rank];i++)
  printf("%d ", a_chunk[i]);
  printf("\n");
  printf("%d\n", rank);

  MPI_Gather(a_chunk, n_elements[rank], MPI_INT, receive, n_elements[rank], MPI_INT, 0, MPI_COMM_WORLD);




  MPI_Finalize();

}
