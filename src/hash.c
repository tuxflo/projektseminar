#include "hash.h"

uint32_t jenkins_hash(const uint8_t* key, size_t length) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

uint8_t node_hash(const uint8_t node_key, const uint8_t node_count) {
    return node_key%node_count;
}

uint32_t local_hash(const uint32_t local_key, const uint32_t local_element_count) {
    return local_key%local_element_count;
}
