#include <mpi.h>
#include "insert.h"
#include "mpi_error.h"

int insert_window_create(MPI_Comm comm, int array_dim, MPI_Win *insert_win) {
    int i, *insert_window_val = 0;
    _MPI_CHECK_(MPI_Alloc_mem((MPI_Aint)array_dim * sizeof(int), MPI_INFO_NULL, &insert_window_val));
    for (i = 0; i < array_dim; i++) {
        insert_window_val[i] = 0;
    }
    MPI_Win_create(insert_window_val, array_dim, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, insert_win);
    return 0;
}

int insert_window_increment(MPI_Win insert_win, int rank, int idx) {
    int one = 1;
    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, insert_win));
    _MPI_CHECK_(MPI_Accumulate(&one, 1, MPI_INT, rank, idx, 1, MPI_INT, MPI_SUM, insert_win));
    _MPI_CHECK_(MPI_Win_flush(rank, insert_win));
    _MPI_CHECK_(MPI_Win_unlock(rank, insert_win));
    return 0;
}

int insert_window_decrement(MPI_Win insert_win, int rank, int idx) {
    int minus_one = -1;
    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, insert_win));
    _MPI_CHECK_(MPI_Accumulate(&minus_one, 1, MPI_INT, rank, idx, 1, MPI_INT, MPI_SUM, insert_win));
    _MPI_CHECK_(MPI_Win_flush(rank, insert_win));
    _MPI_CHECK_(MPI_Win_unlock(rank, insert_win));
    return 0;
}
