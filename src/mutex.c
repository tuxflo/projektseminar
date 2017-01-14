#include <mpi.h>
#include "mpi_error.h"

int MUTEX_KEYVAL = MPI_KEYVAL_INVALID;

int mutex_create(MPI_Comm comm, int array_dim, int world_size, MPI_Win *lock_win) {
    int i, *mutex_val = 0;
    MPI_CALL_AND_CHECK(MPI_Alloc_mem((MPI_Aint)array_dim, MPI_INFO_NULL, &mutex_val));
    for (i = 0; i < array_dim; i++) {
        mutex_val[i] = 0;
    }

    if (MUTEX_KEYVAL == MPI_KEYVAL_INVALID) {
        MPI_CALL_AND_CHECK(MPI_Win_create_keyval(MPI_WIN_NULL_COPY_FN, MPI_WIN_NULL_DELETE_FN, &MUTEX_KEYVAL, (void*)0));
    }

    // Set windows attribute to the world size.
    MPI_CALL_AND_CHECK(MPI_Win_set_attr(*lock_win, MUTEX_KEYVAL, (void*)(MPI_Aint)(world_size)));
    return 0;
}

int mutex_acquire(MPI_Win lock_win, int rank, int idx) {
    int minus_one = -1, one = 1, old_value;
    int flag, *attrval;

    /* Compute the location of the counter */
    MPI_CALL_AND_CHECK(MPI_Win_get_attr(lock_win, MUTEX_KEYVAL, &attrval, &flag));
    if (flag != 0) {
        return -1; /* Error: mutex windows not setup */
    }

    MPI_CALL_AND_CHECK(MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, lock_win));
    do {
        MPI_CALL_AND_CHECK(MPI_Fetch_and_op(&one, &old_value, MPI_INT, rank, idx, MPI_SUM, lock_win));
        MPI_CALL_AND_CHECK(MPI_Win_flush(rank, lock_win));
        if (old_value == 0) {
            break;
        }
        MPI_CALL_AND_CHECK(MPI_Accumulate(&minus_one, 1, MPI_INT, rank, idx, 1, MPI_INT, MPI_SUM, lock_win));
        MPI_CALL_AND_CHECK(MPI_Win_flush(rank, lock_win));
    } while (1);
    MPI_CALL_AND_CHECK(MPI_Win_unlock(rank, lock_win));
    return 0;
}

int mutex_release(MPI_Win lock_win, int rank, int idx) {
    int minus_one = -1;
    int flag, *attrval;

    MPI_CALL_AND_CHECK(MPI_Win_get_attr(lock_win, MUTEX_KEYVAL, &attrval, &flag));
    if (flag != 0) {
        return -1; /* Error: mutex windows not setup */
    }

    MPI_CALL_AND_CHECK(MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, lock_win));
    MPI_CALL_AND_CHECK(MPI_Accumulate(&minus_one, 1, MPI_INT, rank, idx, 1, MPI_INT, MPI_SUM, lock_win));
    MPI_CALL_AND_CHECK(MPI_Win_flush(rank, lock_win));
    return 0;
}
