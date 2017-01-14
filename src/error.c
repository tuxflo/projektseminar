#include <mpi.h>
#include <stdio.h>
#include "ga.h"
#include <ctype.h>
#include <string.h>
#include "separate.h"

int main(int argc, char **argv) {
  MPI_Init(NULL, NULL);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  const int nitems=2;

  // Print off a hello world message
  printf("Hello world from processor %s, rank %d"
      " out of %d processors\n",
      processor_name, world_rank, world_size);

  GA global_array;
  ga_create(MPI_COMM_WORLD, ELEMENT_COUNT, world_size, &global_array);
  int rank, size, *counterMem=0;
  int lnum, lleft, i;
  int num = 4;
  MPI_Aint counterSize = 0;

  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  lnum  = num / size;
  lleft = num % size;
  if (rank < lleft) lnum++;
  counterSize = lnum * sizeof(int);
  if (counterSize > 0) {
    MPI_Alloc_mem(counterSize, MPI_INFO_NULL, &counterMem);
    for (i=0; i<lnum; i++) counterMem[i] = 0;
  }
int MPE_MUTEX_KEYVAL;
  MPI_Win_create(counterMem, counterSize, sizeof(int),
		 MPI_INFO_NULL, MPI_COMM_WORLD, &global_array->lock_win);
    MPI_Win_create_keyval(MPI_WIN_NULL_COPY_FN, MPI_WIN_NULL_DELETE_FN,
			  &MPE_MUTEX_KEYVAL, (void*)0);
  MPI_Win_set_attr(&global_array->lock_win, MPE_MUTEX_KEYVAL, (void*)(MPI_Aint)(size));
  //MPE_Mutex_create(MPI_COMM_WORLD, world_size, &new_ga->lock_win);
  printf("after create\n");

  Entry *received;

  MPI_Status status;

  int insertCount = 0;
  int collisionCount = 0;
  int updatedCount = 0;
  char message[30];
  //Do work!
  if(world_rank == 0) {
    Entry e;
    strcpy(e.id, "3");
    strcpy(e.name, "test");
    int ret = ga_put(global_array, &e);
    printf("after put\n");
    MPI_Send("Work done!", 20, MPI_CHAR, 1, 99, MPI_COMM_WORLD);
  }
  if(world_rank == 1){
    char message[25];
    MPI_Recv(message, 30, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
    //printf("received: %s\n", message);
  }
  // Finalize the MPI environment.
  MPI_Finalize();
  return 0;
}
