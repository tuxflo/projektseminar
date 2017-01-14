#ifndef MPI_Error_h
#define MPI_Error_h
#include <mpi.h>

#define MPI_CALL_AND_CHECK(MPI_FUNC)do{\
int call_ret = MPI_FUNC;\
if (call_ret != MPI_SUCCESS)\
    {\
    mpi_error_handling(call_ret);\
    }}while(0);

void mpi_error_handling(int mpi_error);

#endif
