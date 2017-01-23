#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "entry.h"

void calculate_hash_values(int machinecount, int elementcount, char *key, uint32_t *node, uint32_t *idx);
int main(int argc, char **argv) {
    int machinecount, elementcount;
    char *file = "./test_files/names.txt";
    int *ret;
    int i, j;
    //memset (ret, 0, sizeof (int) * machinecount);
    if(argc < 3) {
        printf("usage: ./distribution machinecount elementcount\n");
        exit(EXIT_FAILURE);
    }
    machinecount = atoi(argv[1]);
    elementcount = atoi(argv[2]);
    ret = (int *) calloc(machinecount, sizeof(int));
    printf("Machines: %d, Elements: %d\n", machinecount, elementcount);
    FILE *fp;
    char *key, *value, *line = NULL;
    size_t read, len = 0;

    fp = fopen(file, "r");
    if (fp == NULL) {
        printf("Error opening file: %s!\n", file);
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        Entry e;
        if(i == elementcount)
            break;
        if ((key = strsep(&line, ",")) == NULL) {
            printf("ERROR key:%s\n", key);
            continue;
        }
        if ((value = strsep(&line, ",")) == NULL) {
            printf("ERROR value:%s\n", value);
            continue;
        }

        strcpy(e.key, key);
        strcpy(e.value, value);
        int node;
        int index;

        calculate_hash_values(machinecount, elementcount,key, &node, &index);
        ret[node] = ret[node] + 1;
        i++;
    }

    for(i=0; i<machinecount; i++) {
        printf("Node: %03d, values: %05d  ", i, ret[i]);
        for(j=0; j<ret[i] / 50; j++) {
            printf("+");
        }
        printf("\n");
    }


    return 0;
}


void calculate_hash_values(int machinecount, int elementcount, char *key, uint32_t *node, uint32_t *idx) {
    uint8_t     node_part;
    uint32_t    local_part, key_hash;
    key_hash = jenkins_hash((uint8_t*)key, strlen(key));
    //printf("key_hash: %d\n", key_hash);
    separate(key_hash, &local_part, &node_part);
    //*node = jenkins_hash(node_part, world_size);
    *node = key_hash % machinecount;
    *idx = key_hash % elementcount;
}
