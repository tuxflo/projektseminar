#ifndef TABLE_H
#define TABLE_H

#include "entry.h"

typedef struct Table {
    uint32_t count;
    uint32_t *localAddressBook;
    struct Entry **entries;
    struct Entry *pool;
    struct Entry *firstFree;
} Table;

void initializeTable(Table *table, int entries, int elements) {
    table->count = 0;
    table->localAddressBook = (uint32_t*) malloc(elements * sizeof(uint32_t*));
    table->entries = (Entry**) calloc(entries, sizeof(Entry*));
    table->pool = (Entry*) malloc(elements * sizeof( Entry ));
    table->firstFree = table->pool;
}

void freeTable(Table *table) {
    free(table->entries);
    free(table->pool);
}

void addToTable(Table *table, uint32_t key, uint32_t keyHash, void *value) {
    Entry *entry = table->firstFree++;
    entry->key = keyHash;
    strncpy(entry->value, value, sizeof(entry->value) - 1);
    entry->value[sizeof(entry->value) - 1] = '\0';
    table->entries[key] = entry;
    table->localAddressBook[table->count] = key;
    table->count++;
}

void verifyTable(const Table *table, uint8_t i) {
    printf("\n\nVerifying table %u:\n\n", i);
    printf("Entry count: %d\n", table->count);
    for (uint32_t i = 0; i < table->count; i++) {
        Entry *current = table->entries[table->localAddressBook[i]];
        if (current != NULL) {
            printf("Entry %u:\nKey: %u\nValue:%s", i, current->key, current->value);
        } else {
            printf("ERROR with entry %u\n", i);
        }
    }
    printf("\n\n");
}
#endif
