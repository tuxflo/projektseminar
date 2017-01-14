#ifndef Entry_h
#define Entry_h
#include "constants.h"

// This is the structure of e single hashmap entry.
typedef struct Entry {
    char key[BUFFER_SIZE];
    char value[BUFFER_SIZE];
} Entry;

#endif
