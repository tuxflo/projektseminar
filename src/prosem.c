#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include "hash.h"
#include "entry.h"
#include "table.h"
#include "separate.h"

#define TABLE_COUNT 5
#define INSERT_COUNT 100
#define ELEMENT_COUNT 10000

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
    uint32_t keyHash;
    uint8_t nodePart;
    uint32_t localPart;
    char *key, *value;

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
        addToTable(&tables[nodeKey], localKey, keyHash, value);

        insertCount++;
        if (insertCount > INSERT_COUNT) {
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
