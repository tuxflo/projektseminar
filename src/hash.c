#include <stdio.h>
#include <string.h>
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

uint32_t better_node_hash(const uint32_t node_key, const uint32_t node_count) {
	uint32_t local_key = node_key;
	local_key = (( local_key >> 16 ) ^ local_key ) * 0x45d9f3b;
	local_key = (( local_key >> 16 ) ^ local_key ) * 0x45d9f3b;
	local_key = ( local_key >> 16 ) ^ local_key;
    return local_key % node_count;
}

uint32_t node_hash(const uint32_t node_key, const uint8_t node_count) {
    return (uint32_t) node_key % node_count;
}

uint32_t better_local_hash(const uint32_t l_key, const uint32_t local_element_count) {
	uint32_t local_key = l_key;
	local_key = (( local_key >> 16 ) ^ local_key ) * 0x45d9f3b;
	local_key = (( local_key >> 16 ) ^ local_key ) * 0x45d9f3b;
	local_key = ( local_key >> 16 ) ^ local_key;
    return local_key % local_element_count;
}

uint32_t local_hash(const uint32_t l_key, const uint32_t local_element_count) {
    return l_key % local_element_count;
}

uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed) {
  uint32_t h = seed;
  if (len > 3) {
    const uint32_t* key_x4 = (const uint32_t*) key;
    size_t i = len >> 2;
    do {
      uint32_t k = *key_x4++;
      k *= 0xcc9e2d51;
      k = (k << 15) | (k >> 17);
      k *= 0x1b873593;
      h ^= k;
      h = (h << 13) | (h >> 19);
      h += (h << 2) + 0xe6546b64;
    } while (--i);
    key = (const uint8_t*) key_x4;
  }
  if (len & 3) {
    size_t i = len & 3;
    uint32_t k = 0;
    key = &key[i - 1];
    do {
      k <<= 8;
      k |= *key--;
    } while (--i);
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    h ^= k;
  }
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}
