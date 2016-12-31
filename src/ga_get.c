#include "ga.h" 
#include <stdio.h>
int ga_get( GA ga, int ilo, int jlo, int jhigh, Entry *buf ) 
{ 
  int      jcur, jfirst, jlast, j, rank; 
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
  MPI_Aint disp; 
 
  jcur = jlo; 
    rank   = (jcur - 1) /ga->chunk2; 
    jfirst = rank * ga->chunk2 + 1; 
    jlast  = (rank + 1) * ga->chunk2; 
    if (jlast > jhigh) jlast = jhigh; 
 
    /* The locks on ga->lock_win are required only when  
       ga_read_inc is used */ 
    MPE_Mutex_acquire(ga->lock_win,rank);
 
    /* Using lock_shared allows other get accesses to proceed */ 
    MPI_Win_lock( MPI_LOCK_SHARED, rank, MPI_MODE_NOCHECK, ga->ga_win ); 
    //MPI_Get(buf, sizeof(Entry), mpi_Entry_type, rank, ilo, 1, mpi_Entry_type, ga->ga_win);
    MPI_Get(buf, (jlast-jcur+1)*(ilo-ilo +1), mpi_Entry_type, rank, (jcur-jfirst)*ga->dim1 + (ilo-1), 1, mpi_Entry_type, ga->ga_win);
    //e = (void *) ( ((char *)e) + (jlast-jcur+1)*(ihigh-ilo+1)*ga->dtype_size);
    //for (j=jcur; j<=jlast; j++) { 
    //  disp = (j - jfirst) * ga->dim1 + (ilo - 1); 
    //  MPI_Get( buf, ihigh - ilo + 1, ga->dtype,  
    //           rank, disp, ihigh - ilo + 1, ga->dtype, ga->ga_win ); 
    //  buf = (void *)( ((char *)buf) +  
    //                  (ihigh - ilo + 1) *  ga->dtype_size ); 
    //} 
    MPI_Win_unlock( rank, ga->ga_win ); 
 
    MPE_Mutex_release(ga->lock_win,rank);
    jcur = jlast + 1; 
  return 0; 
} 
