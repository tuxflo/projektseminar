#include "mpi.h"
#include <stddef.h> //for offsetoff usage
#include <stdlib.h>

#define BUFFER_SIZE 256
#define ELEMENT_COUNT 10000

/* We make GA a pointer to this structure so that users always
   have a pointer, never the actual structure */
typedef struct _GA {
    MPI_Win      ga_win;
    MPI_Win      lock_win;
    /* Datatype and size */
    MPI_Datatype dtype;
    int          dtype_size;
    /* sizes of the global array */
    int          dim1, dim2, chunk2;
} *GA;

typedef struct Entry {
        char id[BUFFER_SIZE];
        char name[BUFFER_SIZE];
} Entry;
int MPE_Mutex_release(MPI_Win mutex_win, int num);
int MPE_Mutex_acquire(MPI_Win mutex_win, int num);
int MPE_Mutex_create(MPI_Comm comm, int num, MPI_Win *mutex_win);
//int ga_put(GA ga, int ilo, int target, Entry *e);
int ga_put(GA ga, Entry *e);
char* ga_get(GA ga, char* key);
int ga_create(MPI_Comm comm, int dim1, int dim2, GA *ga);
