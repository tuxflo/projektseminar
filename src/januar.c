#include <ctype.h>
#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi_error.h"
#include "constants.h"
#include "entry.h"
#include "la.h"
#include "hash.h"
#include "separate.h"
#include "check.h"

int job_zero(LA local_array, int world_rank);
int job_one(LA local_array, int world_rank);
int job_two(LA local_array, int world_rank);
int default_job(LA local_array, int world_rank);
int read_and_put(char *file, int *inserted, int *collisions, int *updated, LA local_array);
Entry *check_buffer = NULL;

int check(LA local_array, int world_size) {
    printf("Checking that array was initialized empty...\n");
    int tmp_rank, i;

    for(tmp_rank = 0; tmp_rank < world_size; tmp_rank++) {
      for(i= 0; i < ELEMENT_COUNT; i++) {
		Entry *tmp = malloc(sizeof(Entry));
        _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, tmp_rank, 0, local_array->la_win));
        MPI_Get(tmp, 1, local_array->mpi_datatype, tmp_rank, i, 1, local_array->mpi_datatype, local_array->la_win);
        _MPI_CHECK_(MPI_Win_unlock(tmp_rank, local_array->la_win));
        printf("Rank: %d, tmp.key: %s // tmp.value: %s //\n",tmp_rank, tmp->key, tmp->value);
        free(tmp);
      }
    }
    printf("...done\n");
    return 0;
}

int main(int argc, char **argv) {
	int ret;
    MPI_Init(NULL, NULL);
    // Configure MPI error handling
    _MPI_CHECK_(MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN));

    int world_size;
    int i;
    _MPI_CHECK_(MPI_Comm_size(MPI_COMM_WORLD, &world_size));

    int world_rank;
    _MPI_CHECK_(MPI_Comm_rank(MPI_COMM_WORLD, &world_rank));

    int name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    _MPI_CHECK_(MPI_Get_processor_name(processor_name, &name_len));

    LA local_array;
    ret = la_create(MPI_COMM_WORLD, ELEMENT_COUNT, world_size, &local_array);
    if (ret != 0) {
        printf("Erro during creation of local array on rank %d.\n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }
    
    _MPI_CHECK_(MPI_Barrier(MPI_COMM_WORLD));
    
    if (world_rank == 0) {
		ret = check(local_array, world_size);
	}
	
	_MPI_CHECK_(MPI_Barrier(MPI_COMM_WORLD));
	
	ret = la_init_mem(local_array);
    if (ret != 0) {
        printf("Erro during initialisation of local array on rank %d.\n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }
    
    _MPI_CHECK_(MPI_Barrier(MPI_COMM_WORLD));
    
    if (world_rank == 0) {
		ret = check(local_array, world_size);
	}
    
    _MPI_CHECK_(MPI_Barrier(MPI_COMM_WORLD));
    
    _MPI_CHECK_(MPI_Finalize());
    exit(EXIT_SUCCESS);
}
