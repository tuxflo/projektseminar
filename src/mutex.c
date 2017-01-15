#include <mpi.h>
#include "mpi_error.h"

int mutex_create(MPI_Comm comm, int array_dim, int world_size, MPI_Win *lock_win) {
    int i, *mutex_val = 0;
    _MPI_CHECK_(MPI_Alloc_mem((MPI_Aint)array_dim * sizeof(int), MPI_INFO_NULL, &mutex_val));
    for (i = 0; i < array_dim; i++) {
        mutex_val[i] = 0;
    }
    MPI_Win_create(mutex_val, array_dim, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, lock_win);
    return 0;
}

int mutex_acquire(MPI_Win lock_win, int rank, int idx) {
    int minus_one = -1, one = 1, old_value;
    
    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, lock_win));
    do {
        _MPI_CHECK_(MPI_Fetch_and_op(&one, &old_value, MPI_INT, rank, idx, MPI_SUM, lock_win));
        _MPI_CHECK_(MPI_Win_flush(rank, lock_win));
        if (old_value == 0) {
            break;
        }
        _MPI_CHECK_(MPI_Accumulate(&minus_one, 1, MPI_INT, rank, idx, 1, MPI_INT, MPI_SUM, lock_win));
        _MPI_CHECK_(MPI_Win_flush(rank, lock_win));
    } while (1);
    _MPI_CHECK_(MPI_Win_unlock(rank, lock_win));
    return 0;
}

int mutex_release(MPI_Win lock_win, int rank, int idx) {
    int minus_one = -1;

    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, lock_win));
    _MPI_CHECK_(MPI_Accumulate(&minus_one, 1, MPI_INT, rank, idx, 1, MPI_INT, MPI_SUM, lock_win));
    _MPI_CHECK_(MPI_Win_flush(rank, lock_win));
    _MPI_CHECK_(MPI_Win_unlock(rank, lock_win));

    return 0;
}
