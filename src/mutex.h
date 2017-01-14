#ifndef MUTEX_H
#define MUTEX_H
#include <mpi.h>

int mutex_create(MPI_Comm comm, int array_dim, int world_size, MPI_Win *lock_win);
int mutex_acquire(MPI_Win lock_win, int rank, int idx);
int mutex_release(MPI_Win lock_win, int rank, int idx);

#endif
