#include "ga.h"
#include <stdio.h>
int ga_create(MPI_Comm comm, int dim1, int dim2, GA *ga)
{
    GA       new_ga;
    int      size, chunk2, sizeoftype;
    MPI_Aint local_size;
    MPI_Info info;
    void     *ga_win_ptr;

    const int nitems=2;
    int          blocklengths[2] = {1,BUFFER_SIZE};
    MPI_Datatype types[2] = {MPI_INT, MPI_CHAR};
    MPI_Datatype mpi_Entry_type;
    MPI_Aint     offsets[2];

    offsets[0] = offsetof(Entry, id);
    offsets[1] = offsetof(Entry, name);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_Entry_type);
    MPI_Type_commit(&mpi_Entry_type);

    /* Get a new structure */
    new_ga = (GA)malloc(sizeof(struct _GA));
    if (!new_ga) return 0;
    /* Determine size of GA memory */
    MPI_Comm_size(comm, &size);
    chunk2 = dim2 / size;
    /* Require size to exactly divide dim2 */
    if ((dim2 % size) != 0) MPI_Abort(comm, 1);
    MPI_Type_size(mpi_Entry_type, &sizeoftype);
    local_size = dim1 * chunk2 * sizeoftype;

    /* Specify ordering of accumulate operations (this is the
       default behavior in MPI-3) */
    MPI_Info_create(&info);
    MPI_Info_set(info,"accumulate_ordering", "rar,raw,war,waw");

    /* Allocate memory and create window */
    MPI_Win_allocate(local_size, sizeoftype, info, comm,
		     &ga_win_ptr, &new_ga->ga_win);
    MPI_Info_free(&info);

    /* Create critical section window */
    MPE_Mutex_create(comm, size, &new_ga->lock_win);

    /* Save other data and return */
    new_ga->dtype      = mpi_Entry_type;
    new_ga->dtype_size = sizeof(Entry);
    new_ga->dim1       = dim1;
    new_ga->dim2       = dim2;
    new_ga->chunk2     = chunk2;
    *ga                = new_ga;
    return 1;
}
