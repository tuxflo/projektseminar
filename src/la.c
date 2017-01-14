#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "la.h"
#include "mutex.h"
#include "hash.h"
#include "separate.h"
#include "constants.h"
#include "mpi_error.h"

void calculate_hash_values(int world_size, char *key, uint32_t *node, uint32_t *idx);

int la_create(MPI_Comm comm, int array_dim, int w_size, LA *la) {
    LA new_la;
    new_la = (LA)malloc(sizeof(struct _LA));
    if (new_la == NULL) {
        printf("Error during memory allocation for new_la.\n");
    }

    /* Specify ordering of accumulate operations (this is the
       default behavior in MPI-3) */
    MPI_Info mpi_info;
    MPI_CALL_AND_CHECK(MPI_Info_create(&mpi_info));
    MPI_CALL_AND_CHECK(MPI_Info_set(mpi_info, "accumulate_ordering", "rar,raw,war,waw"));

    // Creation of derived MPI data type
    const int nitems = 2;
    int block_lengths[2] = { BUFFER_SIZE, BUFFER_SIZE };
    MPI_Datatype dtypes[2] = { MPI_CHAR, MPI_CHAR };
    MPI_Datatype mpi_entry_type;
    MPI_Aint offsets[2];

    offsets[0] = offsetof(Entry, key);
    offsets[1] = offsetof(Entry, value);

    MPI_CALL_AND_CHECK(MPI_Type_create_struct(nitems, block_lengths, offsets, dtypes, &mpi_entry_type));
    MPI_CALL_AND_CHECK(MPI_Type_commit(&mpi_entry_type));

    // Calculate MPI window size
    int size_of_type;
    MPI_Aint la_size;
    MPI_CALL_AND_CHECK(MPI_Type_size(mpi_entry_type, &size_of_type));
    la_size = array_dim * size_of_type;

    /* Allocate memory and create window */
    void *la_win_ptr;
    MPI_CALL_AND_CHECK(MPI_Win_allocate(la_size, size_of_type, mpi_info, comm, &la_win_ptr, &new_la->la_win));
    MPI_CALL_AND_CHECK(MPI_Info_free(&mpi_info));

    /* Create critical section window */
    int ret;
    ret = mutex_create(comm, array_dim, w_size, &new_la->lock_win);
    if (ret != 0) {
        return 1;
    }

    /* Save other data and return */
    new_la->mpi_datatype = mpi_entry_type;
    new_la->datatype_size = sizeof(Entry);
    new_la->local_array_size = array_dim;
    new_la->world_size = w_size;
    *la  = new_la;
    return 0;
}

int la_put(LA la, Entry *e) {
    int ret;
    uint32_t node, idx;
    calculate_hash_values(la->world_size, e->key, &node, &idx);

    Entry *tmp = malloc(sizeof(Entry));

    // Accquireing mutex
    ret = mutex_acquire(la->lock_win, node, idx);
    if (ret != 0) {
        printf("Error during mutex_accquire for idx %d on node %d.\n", idx, node);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }

    // Using lock_shared allows get accesses to proceed
    MPI_CALL_AND_CHECK(MPI_Win_lock(MPI_LOCK_SHARED, node, MPI_MODE_NOCHECK, la->la_win));

    // Collision check
    MPI_CALL_AND_CHECK(MPI_Get(tmp, 1, la->mpi_datatype, node, idx, 1, la->mpi_datatype, la->la_win));
    if (strlen(tmp->value) > 0) {
        /* printf("Collision! on node %d on index %d.\n\t
            Latest entry: key = %s,  value = %s\n\t
            Collision entry: key = %s, value = %s\n",
            node, idx, tmp->key, tmp->value, e->key, e->value);*/
        if (strcmp(tmp->key, e->key) == 0) {
            /* printf("Update! on node %d on index %d.\n\t
                Latest entry: key = %s,  value = %s\n\t
                Update entry: key = %s, value = %s\n",
                node, idx, tmp->key, tmp->value, e->key, e->value);*/
            ret = 2;
        } else {
            ret = 1;
        }
    }

    if (ret != 1) {
        e->value[strlen(e->value)-1] = '\0';
        MPI_CALL_AND_CHECK(MPI_Put(e, 1, la->mpi_datatype, node, idx, 1, la->mpi_datatype, la->la_win));
    }

    MPI_CALL_AND_CHECK(MPI_Win_unlock(node, la->la_win));

    ret = mutex_release(la->lock_win, node, idx);
    if (ret != 0) {
        printf("Error during mutex_release for idx %d on node %d.\n", idx, node);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }
    free(tmp);
    return ret;
}

char *la_get(LA la, char *key) {
    int ret;
    uint32_t node, idx;
    calculate_hash_values(la->world_size, key, &node, &idx);

    Entry *tmp = malloc(sizeof(Entry));

    // Accquireing mutex
    ret = mutex_acquire(la->lock_win, node, idx);
    if (ret != 0) {
        printf("Error during mutex_accquire for idx %d on node %d.\n", idx, node);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }

    // Using lock_shared allows get accesses to proceed
    MPI_CALL_AND_CHECK(MPI_Win_lock(MPI_LOCK_SHARED, node, MPI_MODE_NOCHECK, la->la_win));
    MPI_CALL_AND_CHECK(MPI_Get(tmp, 1, la->mpi_datatype, node, idx, 1, la->mpi_datatype, la->la_win));
    MPI_CALL_AND_CHECK(MPI_Win_unlock(node, la->la_win));

    ret = mutex_release(la->lock_win, node, idx);
    if (ret != 0) {
        printf("Error during mutex_release for idx %d on node %d.\n", idx, node);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }

    if (strcmp(tmp->key, key) == 0) {
        //printf("Got entry: key = %s, value = %s\n", tmp->key, tmp->value);
        return tmp->value;
    } else {
        //printf("Got collision on node %d on index %d with key %s.\n", node, idx, key);
        return (char*)COLLISION;
    }
}

void calculate_hash_values(int world_size, char *key, uint32_t *node, uint32_t *idx) {
    uint8_t node_part;
    uint32_t local_part, key_hash;
    key_hash = jenkins_hash((uint8_t*)key, strlen(key));
    separate(key_hash, &local_part, &node_part);
    *node = node_hash(node_part, world_size);
    *idx = local_hash(local_part, ELEMENT_COUNT);
}
