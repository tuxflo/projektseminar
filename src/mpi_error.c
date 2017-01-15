#include "mpi_error.h"
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

void mpi_error_handling(int mpi_error) {
    int     mpi_error_length;
    char    mpi_error_message[MPI_MAX_ERROR_STRING];

    MPI_Error_string(mpi_error, mpi_error_message, &mpi_error_length);
    printf("%.*s\n", mpi_error_length, mpi_error_message);
    MPI_Abort(MPI_COMM_WORLD, 1);
    exit(EXIT_SUCCESS);
}
