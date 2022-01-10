#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 102

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


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 20

int main(int argc, char ** argv){
  int n_ranks;
  int rank;
  int *a;
  int *a_chunk;
  int *displ;
  int *sendcnts;
  int *receive;
  int count;
  int max = -1;
  int min = -1;
  int i;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

  count = N/n_ranks;
  displ = (int *)malloc(n_ranks*sizeof(int));
  sendcnts = (int *)malloc(n_ranks*sizeof(int));

  for(i=0; i<n_ranks; i++){
    displ[i] = i*count;
    if(i == n_ranks-1)
      sendcnts[i] = count + N%n_ranks;
    else
      sendcnts[i] = count;
  }

  if(rank==0){
    a = (int *)malloc(N*sizeof(int));
    //riempimento a
    for (i = 0; i < N; i++)
      a[i] = rand() % 5;

    for(i=0; i<N; i++){
        printf("%d ", a[i]);
    }
    printf("\n");


    max = a[0];
    min = a[0];
    for(i=1; i<N; i++){
      if(a[i]>max)
        max = a[i];
      if(a[i]<min)
        min=a[i];
    }

  receive = (int *)malloc((max-min+1)*sizeof(int));
  }



  MPI_Bcast(&max, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&min, 1, MPI_INT, 0, MPI_COMM_WORLD);

  //printf("max = %d, min =%d by proc %d\n", max, min, rank);

  a_chunk = (int *)malloc(sendcnts[rank]*sizeof(int));

  MPI_Scatterv(a, sendcnts, displ, MPI_INT, a_chunk, sendcnts[rank], MPI_INT, 0, MPI_COMM_WORLD);

  /*for(int i=0;i<sendcnts[rank];i++)
    printf("%d ", a_chunk[i]);
  printf("\n");
  printf("%d\n", rank);
  printf("\n");*/

  int *c = (int *)malloc((max - min + 1) * sizeof(int));
  for (i = 0; i < max - min + 1; i++)
  c[i] = 0;

  for (i = 0; i < sendcnts[rank]; i++)
  c[a_chunk[i] - min] += 1;

  MPI_Reduce(c, receive, max-min+1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if(rank == 0){
    for(i=0; i<max-min+1; i++)
      printf("%d ", receive[i]);
  }
  /*
  int k = 0;
  for (i = 0; i < max - min + 1; i++)
  while (c[i] > 0)
  {
    a_chunk[k] = i + min;
    k = k + 1;
    c[i] = c[i] - 1;
  }*/
  free(c);

  /*for(int i=0;i<n_elements[rank];i++)
  printf("%d ", a_chunk[i]);
  printf("\n");
  printf("%d\n", rank);
  MPI_Gather(a_chunk, n_elements[rank], MPI_INT, receive, n_elements[rank], MPI_INT, 0, MPI_COMM_WORLD);*/




  MPI_Finalize();

}