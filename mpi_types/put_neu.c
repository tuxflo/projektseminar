#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stddef.h>


#include <mpi.h>
#define LEN 128
#define BUFFER_SIZE 256


typedef struct Entry {
        unsigned int id;
        char name[BUFFER_SIZE];
} Entry;

/* the following is the simplest one-sided communication call MPI_Put */

int main(int argc,char ** argv)
{
  int size;
  int rank;

  char buf[LEN];
  MPI_Group group;
  MPI_Win win;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  memset(buf,0,LEN);

  /* create a type for struct Entry */
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
  Entry send;
  send.id = 42;
  strcpy(send.name, "test");
  Entry send2;
  send2.id = 23;
  strcpy(send2.name, "abc");
  Entry *ptr = &send2;


  MPI_Comm_group(MPI_COMM_WORLD,&group);
  MPI_Win_create(ptr,*offsets,sizeof(char)*BUFFER_SIZE +sizeof(int),MPI_INFO_NULL,MPI_COMM_WORLD,&win);
  
  /* must put a MPI_Win_fence here otherwise MPI_Put will raise an error */ 
  MPI_Win_fence(0,win);
  if (rank == 0) {
    sprintf(buf,"hello world from rank %d\n",rank);
    MPI_Put(&send,LEN,MPI_CHAR,1,0,LEN,MPI_CHAR,win);
  } 
  MPI_Win_fence(0,win);
  printf("rank :%d id: %d value: %s \n",rank,ptr->id, ptr->name);
  
  MPI_Win_free(&win); 
  MPI_Finalize();
  return 0;
}
