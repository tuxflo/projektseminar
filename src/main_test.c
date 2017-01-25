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
    
    ret = la_init_mem(local_array);
    if (ret != 0) {
        printf("Erro during initialisation of local array on rank %d.\n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }

    _MPI_CHECK_(MPI_Barrier(MPI_COMM_WORLD));

    // DO Work
    if (world_rank == 0) {

        clock_t start, end;
        double time_diff;

        start = clock();

        int world_size;
        _MPI_CHECK_(MPI_Comm_size(MPI_COMM_WORLD, &world_size));

        ret = job_zero(local_array, world_rank);
        if (ret != 0) {
            printf("Something went wrong during job zero!\n");
        }

        end = clock();
        time_diff = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("\tCPU-Time: %f seconds, global hashmap size: %d.\n", time_diff, ELEMENT_COUNT * world_size);
        printf("Compare check: %d\n", check_values(local_array, world_size, check_buffer));
    } else {
          ret = default_job(local_array, world_rank);
          if (ret != 0) {
              printf("Something went wrong during the default job!\n");
          }
      }
      
      _MPI_CHECK_(MPI_Barrier(MPI_COMM_WORLD));
      _MPI_CHECK_(MPI_Finalize());
      exit(EXIT_SUCCESS);
}


int job_zero(LA local_array, int world_rank) {
    int ret;
    int insert_count = 0;
    int collision_count = 0;
    int update_count = 0;
    char message[30];

    printf("I am rank %d and I'm on job zero.\n", world_rank);
    char buf[BUFFER_SIZE];
    int tmp_rank, i;
    memset(buf, '\0', sizeof(buf));
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    ret = read_and_put("./test_files/names_collision_test.txt", &insert_count, &collision_count, &update_count, local_array);
    if (ret != 0) {
        printf("Something went wrong during job zero on  rank %d.\n", world_rank);
        return ret;
    }
    printf("Rank %d finished its work!\n\tInserted values: %d, collisions: %d, updated values %d.\n", world_rank, insert_count, collision_count, update_count);
    strcpy(buf, la_get(local_array, "1"));
    printf("Buf: %s\n", buf);

    return 0;
}

int job_one(LA local_array, int world_rank) {
    int ret;
    int insert_count = 0;
    int collision_count = 0;
    int update_count = 0;
    char message[30];

    printf("I am rank %d and I'm on job one.\n", world_rank);
    ret = read_and_put("./test_files/names101.txt", &insert_count, &collision_count, &update_count, local_array);
    if (ret != 0) {
        printf("Something went wrong during job one on  rank %d.\n", world_rank);
        return ret;
    }
    printf("Rank %d finished its work!\n\tInserted values: %d, collisions: %d, updated values %d.\n", world_rank, insert_count, collision_count, update_count);

    return 0;
}
int job_two(LA local_array, int world_rank) {
    printf("I am rank %d and I'm on job two.\n", world_rank);
    return 0;
}
int default_job(LA local_array, int world_rank){
    printf("I am rank %d and I'm on the default job.\n", world_rank);
    return 0;
}

int read_and_put(char *file, int *inserted, int *collisions, int *updated, LA local_array) {
    FILE *fp;
    char *key, *value, *line = NULL;
    size_t read, len = 0;
    int ins = 0; int col = 0; int upd = 0;

    int world_size;
    int i, j;
    _MPI_CHECK_(MPI_Comm_size(MPI_COMM_WORLD, &world_size));
    check_buffer = init_check_buffer(world_size);
    if(check_buffer == NULL) {
        printf("Error initializing the check buffer\n");
        exit(EXIT_FAILURE);
    }

    fp = fopen(file, "r");
    if (fp == NULL) {
        printf("Error opening file: %s!\n", file);
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        Entry e;
        if ((key = strsep(&line, ",")) == NULL) {
            printf("ERROR key:%s\n", key);
            continue;
        }
        if ((value = strsep(&line, ",")) == NULL) {
            printf("ERROR value:%s\n", value);
            continue;
        }

        strcpy(e.key, key);
        strcpy(e.value, value);

        int ret = la_put(local_array, &e);
        int x, y;
        calculate_hash_values(world_size, key, &x, &y);

        strcpy(check_buffer[ELEMENT_COUNT * x + y].key, key);
        strcpy(check_buffer[ELEMENT_COUNT * x + y].value, value);

        if (ret == 0) {
            ins++;
        } else if (ret == 1) {
            col++;
        } else if (ret == 2) {
            upd++;
        }
    }
    *inserted = ins;
    *collisions = col;
    *updated = upd;

    return 0;
}
