#include <mpi.h>
#include <stdio.h>
#include "ga.h"
#include <ctype.h>
#include <string.h>
#include "separate.h"

int readAndPut(char* file, int *inserted, int *collisions, int *updated, GA gloabal_array);
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
  Entry *received;

  MPI_Status status;

  int insertCount = 0;
  int collisionCount = 0;
  int updatedCount = 0;
  char message[30];
  //Do work!
  if(world_rank == 0) {
    readAndPut("names100.txt", &insertCount, &collisionCount, &updatedCount, global_array);
    printf("after create\n");
    //MPI_Send("Work done!", 20, MPI_CHAR, 1, 99, MPI_COMM_WORLD);
    printf("inserted values: %d collisions: %d updated values %d\n", insertCount, collisionCount, updatedCount);
  }
  if(world_rank == 1){
    //MPI_Recv(message, 30, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
    //printf("received: %s\n", message);
    int a, b;
    for(a=0; a<15; a++)
    {
      char key[10];
      sprintf(key, "%d", a);
      char *buf = ga_get(global_array, key);
      //if(strlen(buf) > 0)
        //printf("Get value %s from key: %s\n", buf, key);
    }
    //printf("Get value: %s from key: %s\n", *buf, "1");
    //free(*buf);
  }
  if(world_rank == 2) {
    readAndPut("names101.txt", &insertCount, &collisionCount, &updatedCount, global_array);
    printf("inserted values: %d collisions: %d updated values %d\n", insertCount, collisionCount, updatedCount);
  }

  // Finalize the MPI environment.
  MPI_Finalize();
  return 0;
}

int readAndPut(char* file, int *inserted, int *collisions, int *updated, GA global_array) {
  FILE *fp;
  char *line = NULL, *key, *value;
  size_t len = 0;
  size_t read;
  int ins = 0;
  int col = 0;
  int upd = 0;
  fp = fopen(file, "r");
  if (fp == NULL) {
    printf("error opening file\n");
    exit(EXIT_FAILURE);
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    Entry e;
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
    int ret = ga_put(global_array, &e);
    if(ret == 0)
      ins++;
    if(ret == 1)
      col++;
    if(ret == 2)
      upd++;
  }
    *inserted = ins;
    *collisions = col;
    *updated = upd;

  return 0;
}
