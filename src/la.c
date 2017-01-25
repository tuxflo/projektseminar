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

    /* Specify ordering of accumulate operations (this is the
       default behavior in MPI-3) */
    MPI_Info mpi_info;
    _MPI_CHECK_(MPI_Info_create(&mpi_info));
    _MPI_CHECK_(MPI_Info_set(mpi_info, "accumulate_ordering", "rar,raw,war,waw"));

    // Creation of derived MPI data type
    const int       nitems = 2;
    int             block_lengths[2] = { BUFFER_SIZE, BUFFER_SIZE };
    MPI_Datatype    dtypes[2] = { MPI_CHAR, MPI_CHAR };
    MPI_Datatype    mpi_entry_type;
    MPI_Aint        offsets[2];

    offsets[0] = offsetof(Entry, key);
    offsets[1] = offsetof(Entry, value);

    _MPI_CHECK_(MPI_Type_create_struct(nitems, block_lengths, offsets, dtypes, &mpi_entry_type));
    _MPI_CHECK_(MPI_Type_commit(&mpi_entry_type));
    new_la = (LA)malloc(sizeof(struct _LA));
    if (new_la == NULL) {
        printf("Error during memory allocation for new_la.\n");
    }

    // Calculate MPI window size
    int size_of_type;
    int i;
    MPI_Aint la_size;
    _MPI_CHECK_(MPI_Type_size(mpi_entry_type, &size_of_type));
    la_size = array_dim * size_of_type;

    /* Allocate memory and create window */
    void *la_win_ptr;
    _MPI_CHECK_(MPI_Win_allocate(la_size, size_of_type, MPI_INFO_NULL, comm, &la_win_ptr, &new_la->la_win));
    _MPI_CHECK_(MPI_Info_free(&mpi_info));

    /* Memory init*/
    /* Create critical section window */
    int ret;
    ret = mutex_create(comm, array_dim, w_size, &new_la->lock_win);
    if (ret != 0) {
        return 1;
    }

    ret = insert_window_create(comm, array_dim, &new_la->insert_win);
    if (ret != 0) {
        return 1;
    }

    /* Save other data and return */
    new_la->mpi_datatype        = mpi_entry_type;
    new_la->datatype_size       = sizeof(Entry);
    new_la->local_array_size    = array_dim;
    new_la->world_size          = w_size;
    *la  = new_la;
    return 0;
}

int la_put(LA la, Entry *e) {
    int ret, check;
    uint32_t node, idx;
    calculate_hash_values(la->world_size, e->key, &node, &idx);

    Entry *tmp = malloc(sizeof(Entry));

    // Accquireing mutex
    check = mutex_acquire(la->lock_win, node, idx);
    if (check != 0) {
        printf("Error during mutex_accquire for idx %d on node %d.\n", idx, node);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }

    // Collision check
    int occupation = 0;
    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, node, 0, la->insert_win));
    _MPI_CHECK_(MPI_Get(&occupation, 1, MPI_INT, node, idx, 1, MPI_INT, la->insert_win));
    _MPI_CHECK_(MPI_Win_unlock(node, la->insert_win));

    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, node, 0, la->la_win));
    printf("Occupation: %d\n", occupation);
    if(occupation > 0) {
        _MPI_CHECK_(MPI_Get(tmp, 1, la->mpi_datatype, node, idx, 1, la->mpi_datatype, la->la_win));
        printf("node: %u, idx: %u\n", node, idx);
        printf("tmp.key: %s tmp.value: %s, e.key: %s, e.value: %s\n", tmp->key, tmp->value, e->key, e->value);
        if (strcmp(tmp->key, e->key) == 0) {
            printf("Update! on node %u on index %u.\n\t Latest entry: key = %s,  value = %s\n\t Update entry: key = %s, value = %s\n", node, idx, tmp->key, tmp->value, e->key, e->value);

            ret = 2;
        } else {
            ret = 1;
            //printf("Collision! on node %u on index %u.\n", node, idx);
            insert_window_increment(la->insert_win, node, idx);
        }

    } else {
        ret = 0;
    }

    if (ret != 1) {
        _MPI_CHECK_(MPI_Put(e, 1, la->mpi_datatype, node, idx, 1, la->mpi_datatype, la->la_win));
        insert_window_increment(la->insert_win, node, idx);
        printf("Saving on node %d on index %d. key = %s,  value = %s\n", node, idx, e->key, e->value);
    }

    _MPI_CHECK_(MPI_Win_unlock(node, la->la_win));

    check = mutex_release(la->lock_win, node, idx);
    if (check != 0) {
        printf("Error during mutex_release for idx %u on node %u.\n", idx, node);
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
        printf("Error during mutex_accquire for idx %u on node %u.\n", idx, node);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }

    int occupation = 0;
    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, node, 0, la->insert_win));
    _MPI_CHECK_(MPI_Get(&occupation, 1, MPI_INT, node, idx, 1, MPI_INT, la->insert_win));
    _MPI_CHECK_(MPI_Win_unlock(node, la->insert_win));

    // Using lock_shared allows get accesses to proceed
    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, node, 0, la->la_win));
    _MPI_CHECK_(MPI_Get(tmp, 1, la->mpi_datatype, node, idx, 1, la->mpi_datatype, la->la_win));
    _MPI_CHECK_(MPI_Win_unlock(node, la->la_win));

    ret = mutex_release(la->lock_win, node, idx);
    if (ret != 0) {
        printf("Error during mutex_release for idx %u on node %u.\n", idx, node);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }

    if(occupation == 0) {
        return "NULL";
    } else if(occupation == 1) {
        if (strcmp(tmp->key, key) == 0) {
            //printf("Got entry: key = %s, value = %s\n", tmp->key, tmp->value);
            return tmp->value;
        } else {
          return (char*)COLLISION;
        }
    } else {
        return (char*)COLLISION;
    }
}

int la_init_mem(LA la) {
    int world_rank;
    int i;
    _MPI_CHECK_(MPI_Comm_rank(MPI_COMM_WORLD, &world_rank));
    Entry *tmp =  malloc(sizeof(Entry));
    memset(tmp->key, '\0', sizeof(tmp->key));
    memset(tmp->value, '\0', sizeof(tmp->value));
    MPI_Win_fence(0, la->la_win);
    _MPI_CHECK_(MPI_Win_lock(MPI_LOCK_SHARED, world_rank, 0, la->la_win));
    for (i = 0; i < ELEMENT_COUNT; i++) {
        _MPI_CHECK_(MPI_Put(tmp, 1, la->mpi_datatype, world_rank, i, 1, la->mpi_datatype, la->la_win));
    }
    _MPI_CHECK_(MPI_Win_unlock(world_rank, la->la_win));
    MPI_Win_sync(la->la_win);
    MPI_Win_fence(0, la->la_win);
    free(tmp);
    return 0;
}

void calculate_hash_values(int world_size, char *key, uint32_t *node, uint32_t *idx) {
    uint8_t     node_part;
    uint32_t    local_part, key_hash;
    uint32_t    node_count;
    key_hash = jenkins_hash((uint8_t*)key, strlen(key));
    separate(key_hash, &local_part, &node_part);
    //*node = jenkins_hash(node_part, world_size);

    *node = node_hash(key_hash, world_size);
    *idx = local_hash(key_hash, ELEMENT_COUNT);
}
