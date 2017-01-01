#include "ga.h" 
#include <stdio.h>
#include "hash.h"
#include "separate.h"
#include <string.h>
char* ga_get( GA ga, char *key) 
{ 
  int rank; 
  int index = 2;
  int target = 1;
  uint32_t localPart;
  uint8_t nodePart;
  uint32_t keyHash = jenkinsHash((uint8_t*)key, strlen(key));
  Entry *tmp = malloc(sizeof(Entry));

  separate(keyHash, &localPart, &nodePart);
  int idx = localHash(localPart, ELEMENT_COUNT);
  int nodeIdx = nodeHash(nodePart, TABLE_COUNT);

  uint8_t nodeKey = nodeHash(nodePart, TABLE_COUNT);
  uint32_t localKey = localHash(localPart, ELEMENT_COUNT);
  MPI_Aint disp; 

  rank   = (target - 1) /ga->chunk2; 

  /* The locks on ga->lock_win are required only when  
     ga_read_inc is used */ 
  MPE_Mutex_acquire(ga->lock_win,nodeIdx);

  /* Using lock_shared allows other get accesses to proceed */ 
  MPI_Win_lock( MPI_LOCK_SHARED, nodeIdx, MPI_MODE_NOCHECK, ga->ga_win ); 

  //printf("nodeIdx: %d, idx: %d\n", nodeIdx, idx);
  MPI_Get(tmp, 1, ga->dtype, nodeIdx, idx, 1, ga->dtype, ga->ga_win);
  printf("Getting entry: key: %s value: %s on node: %d at index: %d len: %d\n", tmp->id, tmp->name, nodeIdx, idx, strlen(tmp->name));
  //*buf = malloc(sizeof(char) * strlen(tmp->name)+1);
  /*if(*buf == NULL)*/
    /*printf("Error with malloc!");*/
  //strncpy(*buf, tmp->name, strlen(*buf)-1);

  MPI_Win_unlock( nodeIdx, ga->ga_win ); 

  MPE_Mutex_release(ga->lock_win,nodeIdx);
  return tmp->name; 
} 
