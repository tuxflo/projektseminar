#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "constants.h"
#include "ga.h"
#include "separate.h"

#define EXPERIMENT_CYCLE 5000

int mpiError, mpiErrorLength;
char mpiErrorMessage[MPI_MAX_ERROR_STRING];

int prepareGlobalArray(char* file, int *inserted, int *collisions, int *updated,
  char addressBook[][BUFFER_SIZE], GA gloabal_array);

int main(int argc, char **argv) {
  // Configure MPI error handling
  mpiError = MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

  MPI_Init(NULL, NULL);
  if (mpiError != MPI_SUCCESS) {
      MPI_Error_string(mpiError, mpiErrorMessage, &mpiErrorLength);
      printf("%.*s\n", mpiErrorLength, mpiErrorMessage);
      MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  if (mpiError != MPI_SUCCESS) {
      MPI_Error_string(mpiError, mpiErrorMessage, &mpiErrorLength);
      printf("%.*s\n", mpiErrorLength, mpiErrorMessage);
      MPI_Abort(MPI_COMM_WORLD, 1);
  }

  if (ELEMENT_COUNT % world_size) {
    printf("Element count %d modulo world size %d must be zero.\n",
      ELEMENT_COUNT, world_size);
      MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  if (mpiError != MPI_SUCCESS) {
      MPI_Error_string(mpiError, mpiErrorMessage, &mpiErrorLength);
      printf("%.*s\n", mpiErrorLength, mpiErrorMessage);
      MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // Get the name of the current processor
  int name_len;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(processor_name, &name_len);
  if (mpiError != MPI_SUCCESS) {
      MPI_Error_string(mpiError, mpiErrorMessage, &mpiErrorLength);
      printf("%.*s\n", mpiErrorLength, mpiErrorMessage);
      MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // Print a hello world message
  printf("Hello world from processor %s, rank %d"
      " out of %d processors.\n",
      processor_name, world_rank, world_size);

  // Create flobal Array
  GA global_array;
  if (!ga_create(MPI_COMM_WORLD, ELEMENT_COUNT, world_size, &global_array)){
    printf("Error during global array creation on process %s, rank %d.\n",
        processor_name, world_rank);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  Entry *received;
  MPI_Status status;
  char addressBook[ELEMENT_COUNT][BUFFER_SIZE];

  int insertCount = 0;
  int collisionCount = 0;
  int updatedCount = 0;
  char message[30];

  clock_t start, end;
  double timeDiff;

  // Experiment preparation: Rank 0 inserts
  if(world_rank == 0) {
    start = clock();
    if (!prepareGlobalArray("names.txt", &insertCount, &collisionCount, &updatedCount, addressBook, global_array)) {
      printf("Error during global array initialization on process %s, rank %d.\n",
          processor_name, world_rank);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    end = clock();
    printf("inserted values: %d collisions: %d updated values %d\n", insertCount, collisionCount, updatedCount);

    timeDiff = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time to insert %d records into our global array: %f\nArray size: %d\nWorld size: %d",
      EXPERIMENT_CYCLE, timeDiff, ELEMENT_COUNT, world_size);
  }

  /*
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
    //readAndPut("names101.txt", &insertCount, &collisionCount, &updatedCount, global_array);
    printf("inserted values: %d collisions: %d updated values %d\n", insertCount, collisionCount, updatedCount);
  }
  */
  // Finalize the MPI environment.
  MPI_Finalize();
  return 0;
}

int prepareGlobalArray(char* file, int *inserted, int *collisions, int *updated, char globalAddressBook[][BUFFER_SIZE], GA global_array) {
  FILE *fp;
  char *line = NULL, *key, *value;
  size_t len = 0;
  size_t read;
  int ins = 0;
  int col = 0;
  int upd = 0;
  int idx = 0;
  fp = fopen(file, "r");
  if (fp == NULL) {
    printf("error opening file\n");
    return 0;
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    Entry e;
    if ((key = strsep(&line, ",")) == NULL) {
      printf("ERROR key: %s\n", key);
      continue;
    }
    if ((value = strsep(&line, ",")) == NULL) {
      printf("ERROR value: %s\n", value);
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

    strcpy(globalAddressBook[idx], key);
    idx++;

    if (idx >= EXPERIMENT_CYCLE) {
      break;
    }
  }

  *inserted = ins;
  *collisions = col;
  *updated = upd;

  return 1;
}
