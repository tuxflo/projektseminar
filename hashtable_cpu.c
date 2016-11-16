#include <stdio.h>
#include <stdlib.h>
#define SIZE    (100*1024*1024)
#define ELEMENTS    (SIZE / sizeof(unsigned int))
#define HASH_ENTRIES     1024


typedef struct Entry{
    unsigned int    key;
    void            *value;
    struct Entry           *next;
} Entry;

typedef struct Table{
    size_t  count;
    struct Entry   **entries;
    struct Entry   *pool;
    struct Entry   *firstFree;
} Table;


size_t hash( unsigned int key, size_t count ) {
    return key % count;
}

void* big_random_block( int size ) {
    unsigned char *data = (unsigned char*)malloc( size );
    for (int i=0; i<size; i++)
        data[i] = rand();

    return data;
}

void initialize_table( Table *table, int entries,
                       int elements ) {
    table->count = entries;
    table->entries = (Entry**)calloc( entries, sizeof(Entry*) );
    table->pool = (Entry*)malloc( elements * sizeof( Entry ) );
    table->firstFree = table->pool;
}

void free_table( Table *table ) {
    free( table->entries );
    free( table->pool );
}

void add_to_table( Table *table, unsigned int key, void *value ) {
    size_t hashValue = hash( key, table->count );
    Entry *location = table->firstFree++;
    location->key = key;
    location->value = value;
    location->next = table->entries[hashValue];
    table->entries[hashValue] = location;
}

void verify_table( const Table *table ) {
    int count = 0;
    for (size_t i=0; i<table->count; i++) {
        Entry   *current = table->entries[i];
        while (current != NULL) {
            ++count;
            if (hash( current->key, table->count ) != i)
                printf( "%d hashed to %ld, but was located at %ld\n",
                        current->key,
                        hash( current->key, table->count ), i );
            current = current->next;
        }
    }
    if (count != ELEMENTS)
        printf( "%d elements found in hash table.  Should be %ld\n",
                count, ELEMENTS );
    else
        printf( "All %d elements found in hash table.\n", count);
}


int main( void ) {
    unsigned int *buffer =
                     (unsigned int*)big_random_block( SIZE );

    Table table;
    initialize_table( &table, HASH_ENTRIES, ELEMENTS );

    for (int i=0; i<ELEMENTS; i++) {
        add_to_table( &table, buffer[i], (void*)NULL );
    }


    verify_table( &table );

    free_table( &table );
    free( buffer );
    return 0;
}
