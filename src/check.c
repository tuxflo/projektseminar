#include "check.h"

Entry *init_check_buffer(int world_size) {
    int i;
    Entry *check_buffer = NULL;
    check_buffer = (Entry *) malloc(sizeof(Entry) * world_size * ELEMENT_COUNT);
    if(check_buffer == NULL)
        return -1;
    for(i = 0; i < world_size * ELEMENT_COUNT; i++)
        memset(check_buffer[i].key, '\0', sizeof(check_buffer[i].key));
    printf("now in init check buffer\n");
    return check_buffer;
}

int check_values(LA local_array, int world_size, Entry *check_buffer) {
  int x, y, i;
  int ret = 0;

  for(i = 0; i < ELEMENT_COUNT * world_size; i++) {
    char *tmp = la_get(local_array, check_buffer[i].key);
    if(strncmp(tmp, check_buffer[i].value, strlen(tmp)) != 0 && strcmp(tmp, "COLLISION") != 0) {
      printf("check buffer %d,  key: %s, value: %s, string_size: %d\n", i, check_buffer[i].key, check_buffer[i].value, strlen(tmp)); 
      ret = 1;
    }
  }
  return ret;
}

int check_init(LA local_array, int world_size) {
    printf("Checking that array was initialized empty...\n");
    int tmp_rank;
    int i;

    Entry *tmp = malloc(sizeof(Entry));

    for(tmp_rank = 0; tmp_rank < world_size; tmp_rank++) {
      for(i= 0; i < ELEMENT_COUNT; i++) {
        _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, tmp_rank, MPI_MODE_NOCHECK, local_array->la_win));
        MPI_Get(tmp, 1, local_array->mpi_datatype, tmp_rank, i, 1, local_array->mpi_datatype, local_array->la_win);
        if(strlen(tmp->key) != 0) {
          printf("Array not empty\n");
          MPI_Abort(MPI_COMM_WORLD, 1);
          return -1;
        }
        _MPI_CHECK_(MPI_Win_unlock(tmp_rank, local_array->la_win));
      }
    }
    printf("...done\n");
    return 0;
}
