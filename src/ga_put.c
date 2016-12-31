#include "ga.h"
#include <stddef.h> //for offsetoff usage
#include <string.h>
#include <stdio.h>
int ga_put(GA ga, int ilo, int target, Entry *e)
{
  int jlo = target;
  int jhigh = target;

  int      jcur, jfirst, jlast, j, rank;
  /* pass the size of the struct entries: 1 int, BUFFER_SIZE chars */
  MPI_Aint disp;

  jcur = jlo;
  rank   = (jcur - 1) /ga->chunk2;
  jfirst = rank * ga->chunk2 + 1;
  jlast  = (rank + 1) * ga->chunk2;
  if (jlast > jhigh) jlast = jhigh;

  MPE_Mutex_acquire(ga->lock_win,rank);

  /* Using lock_shared allows get accesses to proceed */
  MPI_Win_lock(MPI_LOCK_SHARED, rank, MPI_MODE_NOCHECK, ga->ga_win);
  Entry tmp;
  Entry *tmp_ptr = &tmp;
  MPI_Get(tmp_ptr, (jlast-jcur+1)*(ilo-ilo +1), ga->dtype, rank, (jcur-jfirst)*ga->dim1 + (ilo-1), 1, ga->dtype, ga->ga_win);
  if(strlen(tmp_ptr->name) > 0)
    printf("collision! previous value: %s\n", tmp_ptr->name);

  MPI_Put(e, (jlast-jcur+1)*(ilo-ilo +1), ga->dtype, rank, (jcur-jfirst)*ga->dim1 + (ilo-1), 1, ga->dtype, ga->ga_win);

  MPI_Win_unlock(rank, ga->ga_win);

  MPE_Mutex_release(ga->lock_win,rank);
  jcur = jlast + 1;
  return 0;
}
