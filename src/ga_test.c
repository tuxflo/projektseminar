#include <mpi.h>
#include <stdio.h>
#include "ga.h"
#include <ctype.h>
#include <string.h>


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
  /* pass the size of the struct entries: 1 int, BUFFER_SIZE chars */
  int          blocklengths[2] = {1,BUFFER_SIZE};
  MPI_Datatype types[2] = {MPI_INT, MPI_CHAR};
  MPI_Datatype mpi_Entry_type;
  MPI_Aint     offsets[2];

  offsets[0] = offsetof(Entry, id);
  offsets[1] = offsetof(Entry, name);

  MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_Entry_type);
  MPI_Type_commit(&mpi_Entry_type);

  // Print off a hello world message
  printf("Hello world from processor %s, rank %d"
      " out of %d processors\n",
      processor_name, world_rank, world_size);

  GA global_array;

  ga_create(MPI_COMM_WORLD, 10, 4, MPI_CHAR, &global_array);
  Entry *received;

  MPI_Status status;
  char message[30];
  if(world_rank == 0) {
    MPI_Recv(message, 30, MPI_CHAR, 1, 99, MPI_COMM_WORLD, &status);
    printf("received: %s\n", message);
    //einzeles GA Objekt, speicher anfang, speicher Ende, Ziel rank1, Ziel rank2 ... bei uns gleich, da Daten nur auf einem einzelnen Rank gespeichert werden sollen
    ga_get(global_array, 1, 1, 1, received);
    printf("Buffer content: name: %s id: %d\n", received->name, received->id);

  }
  else {
    Entry e;
    e.id=42;
    strcpy(e.name, "deadbeef");
    ga_put(global_array, 1, 1, 1, &e); 
    e.id=23;
    strcpy(e.name, "foobar");
    ga_put(global_array, 2, 1, 1, &e); 
    MPI_Send("Hello getter", 20, MPI_CHAR, 0, 99, MPI_COMM_WORLD);
  }

  // Finalize the MPI environment.
  MPI_Finalize();
  return 0;
}
