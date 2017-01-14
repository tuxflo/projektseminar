#ifndef HASH_H
#define HASH_H
#include <stdint.h>
#include <stdlib.h>

uint32_t jenkinsHash(const uint8_t* key, size_t length);

uint8_t nodeHash(const uint8_t nodeKey, const uint8_t nodeCount);

uint32_t localHash(const uint32_t localKey, const uint32_t localElementCount);
#endif
