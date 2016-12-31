#include "mpi.h"
#include <stddef.h> //for offsetoff usage
#include <stdlib.h>
#define BUFFER_SIZE 256
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
        unsigned int id;
        char name[BUFFER_SIZE];
} Entry;
