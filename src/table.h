#ifndef TABLE_H
#define TABLE_H

#include "entry.h"

bool addToTable(MPI_Win win, uint8_t nodeKey, uint32_t localKey, uint8_t *key, void *value) {
    MPI_INT localOverflowSize = 0;
    Entry entry;
    strcpy(entry.id, key);
    strcpy(send.value, value);
    MPI_Get(&localOverflowSize, 1, MPI_INT, (int)nodeKey, 0, 1, MPI_INT, win);
    MPI_PUT(&entry, 1, )
    if (localOverflowSize > 0) {
        return false;
    } else {
        return true;
    }
}
#endif
