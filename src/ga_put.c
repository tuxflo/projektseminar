#include "ga.h"
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

  separate(keyHash, &localPart, &nodePart);
  int idx = localHash(localPart, ELEMENT_COUNT);
  int nodeIdx = nodeHash(nodePart, TABLE_COUNT);

  uint8_t nodeKey = nodeHash(nodePart, TABLE_COUNT);
  uint32_t localKey = localHash(localPart, ELEMENT_COUNT);
  /* pass the size of the struct entries: 1 int, BUFFER_SIZE chars */
  MPI_Aint disp;

  //rank   = (target - 1) /ga->chunk2;

  MPE_Mutex_acquire(ga->lock_win,nodeIdx);

  /* Using lock_shared allows get accesses to proceed */
  MPI_Win_lock(MPI_LOCK_SHARED, nodeIdx, MPI_MODE_NOCHECK, ga->ga_win);
  //collision check
  Entry *tmp = malloc(sizeof(Entry));
  MPI_Get(tmp, 1, ga->dtype, nodeIdx, idx, 1, ga->dtype, ga->ga_win);
  if(strlen(tmp->name) > 0)
    printf("collision! previous value: %s\n", tmp->name);

  //printf("Saving entry: key: %s value: %s on node: %d at index: %d\n", e->id, e->name, nodeIdx, idx);
  MPI_Put(e, 1, ga->dtype, nodeIdx, idx, 1, ga->dtype, ga->ga_win);

  MPI_Win_unlock(nodeIdx, ga->ga_win);

  MPE_Mutex_release(ga->lock_win,nodeIdx);
  free(tmp);
  return 0;
}
