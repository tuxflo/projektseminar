#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>

#define TABLE_COUNT 5
#define ELEMENT_COUNT 10000

typedef struct Entry {
    unsigned int key;
    char value[256];
} Entry;

typedef struct Table {
    unsigned int count;
    unsigned int localAddressBook[ELEMENT_COUNT];
    struct Entry **entries;
    struct Entry *pool;
    struct Entry *firstFree;
} Table;

unsigned int hash(unsigned int key, unsigned int tableCount) {
    return key % tableCount;
}

void initializeTable(Table *table, int entries, int elements) {
    table->count = 0;
    table->entries = (Entry**)calloc( entries, sizeof(Entry*) );
    table->pool = (Entry*)malloc( elements * sizeof( Entry ) );
    table->firstFree = table->pool;
}

void freeTable(Table *table) {
    free(table->entries);
    free(table->pool);
}

void addToTable(Table *table, unsigned int key, void *value) {
    Entry *entry = table->firstFree++;
    entry->key = key;
    strncpy(entry->value, value, sizeof(entry->value) - 1);
    entry->value[sizeof(entry->value) - 1] = '\0';
    table->entries[key] = entry;
    table->localAddressBook[table->count] = key;
    table->count++;
}

void verifyTable(const Table *table, size_t i) {
    printf("Verifying table %zu:\n\n", i);
    printf("Entry count: %d\n", table->count);
    for (size_t i = 0; i < table->count; i++) {
        Entry *current = table->entries[table->localAddressBook[i]];
        if (current != NULL) {
            printf("Entry %zu:\nKey: %d\nValue:%s", i, current->key, current->value);
        } else {
            printf("ERROR with entry %zu\n", i);
        }
    }
    printf("\n\n");
}

int main(void) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Table tables[TABLE_COUNT];
    for (int i = 0; i < TABLE_COUNT; i++) {
        initializeTable(&tables[i],ELEMENT_COUNT, ELEMENT_COUNT);
    }

    fp = fopen("../names.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    int insertCount = 0;
    int tableKey;
    unsigned int key;
    char *token1, *token2;

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("%s", line);
        if ((token1 = strsep(&line, ",")) == NULL) {
            printf("ERROR token1:%s\n", token1);
            continue;
        }
        if ((token2 = strsep(&line, ",")) == NULL) {
            printf("ERROR token2:%s\n", token2);
            continue;
        }
        if ((key = (unsigned int)strtol(token1, (char**)NULL, 10)) == 0) {
            printf("ERROR strtol\n");
            continue;
        }

        tableKey = hash(key, TABLE_COUNT);
        addToTable(&tables[tableKey], key, token2);

        insertCount++;
        if (insertCount > 10) {
            break;
        }
    }

    for (size_t i = 0; i < TABLE_COUNT; i++) {
        verifyTable(&tables[i], i);
    }

    fclose(fp);

    if (line) {
        free(line);
    }

    for (int i = 0; i < TABLE_COUNT; i++) {
        freeTable(&tables[i]);
    }

    exit(EXIT_SUCCESS);
}
