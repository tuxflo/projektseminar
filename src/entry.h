#ifndef Entry_h
#define Entry_h

#define BUFFER_SIZE 256

typedef struct Entry {
        char key[BUFFER_SIZE];
        char value[BUFFER_SIZE];
} Entry;
#endif
