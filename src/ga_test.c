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
  FILE *fp;
  char *line = NULL, *key, *value;
  size_t len = 0;
  ssize_t read;
  int insertCount = 0;

  //Do work!
  if(world_rank == 0) {
    //MPI_Recv(message, 30, MPI_CHAR, 1, 99, MPI_COMM_WORLD, &status);
    //printf("received: %s\n", message);
    fp = fopen("../names.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
      if(insertCount >= TABLE_COUNT * ELEMENT_COUNT)
        break;
      Entry e;
      printf("%s", line);
      if ((key = strsep(&line, ",")) == NULL) {
        printf("ERROR key:%s\n", key);
        continue;
      }
      if ((value = strsep(&line, ",")) == NULL) {
        printf("ERROR value:%s\n", value);
        continue;
      }
      strcpy(e.id, key);
      strcpy(e.name, value);
      ga_put(global_array, &e); 
      insertCount++;
    }
  }
  else {
    //char **buf;
    //ga_get(global_array, "1", buf);
    //printf("Get value: %s from key: %s\n", *buf, "1");
    //free(*buf);
    //MPI_Send("Hello getter", 20, MPI_CHAR, 0, 99, MPI_COMM_WORLD);
  }

  // Finalize the MPI environment.
  MPI_Finalize();
  return 0;
}
