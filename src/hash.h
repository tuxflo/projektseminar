#ifndef HASH_H
#define HASH_H
#include <stdint.h>
#include <stdlib.h>

uint32_t    jenkins_hash(const uint8_t* key, size_t length);
uint32_t    local_hash(const uint32_t local_key, const uint32_t local_element_ount);
uint32_t    node_hash(const uint32_t node_key, const uint8_t node_count);
uint32_t 	murmur3_32(const uint8_t* key, size_t len, uint32_t seed);
uint32_t	better_node_hash(const uint32_t node_key, const uint32_t node_count);
uint32_t better_local_hash(const uint32_t l_key, const uint32_t local_element_count);

#endif
