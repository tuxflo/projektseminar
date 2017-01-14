#ifndef Entry_h
#define Entry_h

#define BUFFER_SIZE 256

typedef struct Entry {
        uint32_t key;
        char value[BUFFER_SIZE];
} Entry;
#endif
