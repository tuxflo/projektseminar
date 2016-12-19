#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <mpi.h>
#include <stddef.h>
#include "hash.h"
#include "entry.h"
//#include "table.h"
#include "separate.h"

#define INSERT_COUNT 100
#define ELEMENT_COUNT 10000
#define OVERFLOW_COUNT 10

boolean addToTable(MPI_Win win, MPI_Datatype mpi_Entry_type, uint8_t nodeKey, uint32_t localKey, uint8_t *key, void *value) {
    MPI_INT localOverflowSize = 0;
    Entry entry;
    strcpy(entry.key, key);
    strcpy(entry.value, value);
    MPI_Get(&localOverflowSize, 1, MPI_INT, (int)nodeKey, 0, 1, MPI_INT, win);
    MPI_PUT(&entry, 1, mpi_Entry_type, (int)nodeKey, localKey, 1, mpi_Entry_type, win);
    if (localOverflowSize > 0) {
        return false;
    } else {
        return true;
    }
}

void master() {
    FILE *fp;
    fp = fopen("../names.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    char *line = NULL, *key, *value;
    size_t len = 0;
    ssize_t read;
    int insertCount = 0;
    uint32_t keyHash, localPart;
    uint8_t nodePart;

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("%s", line);
        if ((key = strsep(&line, ",")) == NULL) {
            printf("ERROR key:%s\n", key);
            continue;
        }
        if ((value = strsep(&line, ",")) == NULL) {
            printf("ERROR value:%s\n", value);
            continue;
        }

        keyHash = jenkinsHash((uint8_t*)key, strlen(key));
        separate(keyHash, &localPart, &nodePart);

        uint8_t nodeKey = nodeHash(nodePart, TABLE_COUNT);
        uint32_t localKey = localHash(localPart, ELEMENT_COUNT);

        addToTable(nodeKey, localKey, key, value);

        insertCount++;
        if (insertCount > INSERT_COUNT) {
            break;
        }
    }

    fclose(fp);

    if (line) {
        free(line);
    }
}

void slave() {
    int i = 0;
    while (true) {
        i++;
        if (i == 1000) {
            i = 0;
            int size = 0;
            for (size_t j = 0; i < sizeof(overflowSize); j++) {
                size += overflowSize[j];
            }
            printf("Overflow-Size: %d\n", size);
        }
    }
}

int main(void) {

    int size, rank;
    MPI_Group group;
    MPI_Win mapWindow, overflowSizeWindow;
    // MPI_Win overflowArrayWindows;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    const int nitems=2;
    int blocklengths[2] = {1, BUFFER_SIZE};
    MPI_Datatype types[2] = {MPI_INT, MPI_CHAR};
    MPI_Datatype mpi_Entry_type;
    MPI_Aint offsets[2];
    offsets[0] = offsetof(Entry, id);
    offsets[1] = offsetof(Entry, name);
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_Entry_type);
    MPI_Type_commit(&mpi_Entry_type);

    Entry[ELEMENT_COUNT] map;
    uint32_t[ELEMENT_COUNT] overflowSize;
    // Entry[ELEMENT_COUNT][OVERFLOW_COUNT] overflowArray;

    MPI_Win_allocate(sizeof(map), sizeof(Entry), MPI_INFO_NULL, MPI_COMM_WORLD, &map, &mapWindow);
    MPI_Win_allocate(sizeof(overflowSize), sizeof(uint32_t), MPI_INFO_NULL, MPI_COMM_WORLD, &overflowSize, &overflowSizeWindow);
    // MPI_Win_allocate(sizeof(overflowArray), OVERFLOW_COUNT*sizeof(Entry), MPI_INFO_NULL, MPI_COMM_WORLD, &overflowArray, &overflowArrayWindows);

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if (rank == 0) {
        master();
    } else {
        slave();
    }

    MPI_Win_free(&mapWindow);
    MPI_Win_free(&overflowSizeWindow;);
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
