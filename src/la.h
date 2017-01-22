#ifndef LA_h
#define LA_h
#include "entry.h"
#include "constants.h"
#include <inttypes.h>
#include <errno.h>
#include <mpi.h>

static const char *COLLISION = "COLLISION";

// This is the structure of the local array of each process in our MPI world
typedef struct _LA {
    MPI_Win         la_win;
    MPI_Win         lock_win;
    MPI_Datatype    mpi_datatype;
    int datatype_size, local_array_size, world_size;
} * LA;

int     la_create(MPI_Comm comm, int la_size, int world_size, LA *la);
int     la_put(LA la, Entry *e);
char*   la_get(LA la, char *key);
int     la_init_mem(LA la);

#endif
