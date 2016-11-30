#ifndef HASH_H
#define HASH_H

uint32_t jenkinsHash(const uint8_t* key, size_t length) {
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

uint8_t nodeHash(const uint8_t nodeKey, const uint8_t nodeCount) {
    return nodeKey%nodeCount;
}

uint32_t localHash(const uint32_t localKey, const uint32_t localElementCount) {
    return localKey%localElementCount;
}
#endif
