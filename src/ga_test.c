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

  ga_create(MPI_COMM_WORLD, ELEMENT_COUNT, TABLE_COUNT, &global_array);
  Entry *received;

  MPI_Status status;
  char message[30];
  Entry e;
  strcpy(e.name, "deadbeef");
  strcpy(e.id, "10001");
  Entry e2;
  strcpy(e2.name, "test");
  strcpy(e2.id, "555");
  if(world_rank == 0) {
    //MPI_Recv(message, 30, MPI_CHAR, 1, 99, MPI_COMM_WORLD, &status);
    //printf("received: %s\n", message);
    ga_put(global_array, &e); 
    ga_put(global_array, &e2); 
  }
  else {
    char **buf;
    ga_get(global_array, e.id, buf);
    printf("Get value: %s from key: %s\n", *buf, e.id);
    ga_get(global_array, "10001", buf);
    printf("Get value: %s from key: %s\n", *buf, e.id);
    free(*buf);
    //MPI_Send("Hello getter", 20, MPI_CHAR, 0, 99, MPI_COMM_WORLD);
  }

  // Finalize the MPI environment.
  MPI_Finalize();
  return 0;
}
