#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "entry.h"

void calculate_hash_values(int machinecount, int elementcount, char *key, uint32_t *node, uint32_t *idx);
void calculate_hash_values_separate(int machinecount, int elementcount, char *key, uint32_t *node, uint32_t *idx);
void print_results(int *array, int machinecount, int elementcount);
int read_and_hash(int *jenkins_hash, int *separate_hash, int machinecount, int elementcount);
int main(int argc, char **argv) {
    int machinecount, elementcount;
    int *separate_hash;
    int *jenkins_hash;
    int i, j;
    //memset (ret, 0, sizeof (int) * machinecount);
    if(argc < 3) {
        printf("usage: ./distribution machinecount elementcount\n");
        exit(EXIT_FAILURE);
    }
    machinecount = atoi(argv[1]);
    elementcount = atoi(argv[2]);
    jenkins_hash = (int *) calloc(machinecount, sizeof(int));
    separate_hash = (int *) calloc(machinecount, sizeof(int));
    printf("Machines: %d, Elements: %d\n", machinecount, elementcount);
    FILE *fp;
    char *file = "./test_files/names.txt";
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
        jenkins_hash[node] = jenkins_hash[node] + 1;

        calculate_hash_values_separate(machinecount, elementcount,key, &node, &index);
        separate_hash[node] = separate_hash[node] + 1;

        i++;
    }

    printf("Jenkins distribution:\n");
    print_results(jenkins_hash, machinecount, elementcount);
    printf("\n\n");
    printf("Separate distribution:\n");
    print_results(separate_hash, machinecount, elementcount);



    return 0;
}
int read_and_hash(int *jenkins_hash, int *separate_hash, int machinecount, int elementcount) {
}


void calculate_hash_values(int machinecount, int elementcount, char *key, uint32_t *node, uint32_t *idx) {
    uint8_t     node_part;
    uint32_t    local_part, key_hash;
    key_hash = jenkins_hash((uint8_t*)key, strlen(key));
    *node = key_hash % machinecount;
    *idx = better_local_hash(key_hash, elementcount);
}

void calculate_hash_values_separate(int machinecount, int elementcount, char *key, uint32_t *node, uint32_t *idx) {
    uint8_t     node_part;
    uint32_t    local_part, key_hash;
    key_hash = jenkins_hash((uint8_t*)key, strlen(key));
    separate(key_hash, &local_part, &node_part);
    *node = node_part % machinecount;
    *idx = local_part % elementcount;
}

void print_results(int *array, int machinecount, int elementcount) {
  int max = 1;
  int i, j;
  if((int) elementcount / machinecount > 100)
    max = 15;
  if((int)elementcount / machinecount > 1000)
    max = 50;
  if((int)elementcount / machinecount >= 30000)
    max = 350;
  if((int)elementcount / machinecount >= 100000)
    max = 100;

  printf("%27s", "Elements [");
  for(j=1; j < 13; j++) {
    for(i=0; i<10; i++)
      printf("-");
    printf("%d", (i*100)*j);
  }
  printf("]\n");

    for(i=0; i<machinecount; i++) {
        printf("Node: %3d, values: %5d  ", i, array[i]);
        for(j=0; j<array[i] / 73; j++) {
            printf("+");
        }
        printf("\n");
    }
}

