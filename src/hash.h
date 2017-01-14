#ifndef HASH_H
#define HASH_H
#include <stdint.h>
#include <stdlib.h>

uint32_t jenkins_hash(const uint8_t* key, size_t length);

uint8_t node_hash(const uint8_t node_key, const uint8_t node_count);

uint32_t local_hash(const uint32_t local_key, const uint32_t local_element_ount);

#endif
