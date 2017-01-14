#include "ga.h"
#include "hash.h"
#include <stddef.h> //for offsetoff usage
#include <string.h>
#include <stdio.h>
#include "separate.h"

int ga_put(GA ga, Entry *e)
{
  int rank;
  uint32_t localPart;
  uint8_t nodePart;
  uint32_t keyHash = jenkinsHash((uint8_t*)e->id, strlen(e->id));
  int ret = 0;

  separate(keyHash, &localPart, &nodePart);
  uint32_t idx = localHash(localPart, ELEMENT_COUNT);
  uint8_t nodeIdx = nodeHash(nodePart, ga->dim2);

  /* pass the size of the struct entries: 1 int, BUFFER_SIZE chars */
  MPI_Aint disp;

  Entry *tmp = malloc(sizeof(Entry));
  MPE_Mutex_acquire(ga->lock_win,nodeIdx);
  /* Using lock_shared allows get accesses to proceed */
  MPI_Win_lock(MPI_LOCK_SHARED, nodeIdx, MPI_MODE_NOCHECK, ga->ga_win);
  //collision check
  MPI_Get(tmp, 1, ga->dtype, nodeIdx, idx, 1, ga->dtype, ga->ga_win);

  //printf("Saving entry: key: %s value: %s on node: %d at index: %d\n", e->id, e->name, nodeIdx, idx);
  e->name[strlen(e->name)-1] = '\0';
  if(strlen(tmp->name) > 0)
  {
    //printf("collision! previous value: %s new value: %s\n", tmp->name, e->name);
    ret = 1;
  }

  if(strcmp(tmp->id, e->id) == 0)
  {
    printf("value with key %s already exists, updating to new value: %s\n", e->id, e->name);
    ret = 2;
  }
  if( ret == 0 || ret == 2 )
    MPI_Put(e, 1, ga->dtype, nodeIdx, idx, 1, ga->dtype, ga->ga_win);

  MPI_Win_unlock(nodeIdx, ga->ga_win);

  MPE_Mutex_release(ga->lock_win,nodeIdx);
  free(tmp);
  return ret;
}
