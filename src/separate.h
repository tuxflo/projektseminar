#ifndef SEPARATE_H
#define SEPARATE_H
#include <stdint.h>

uint32_t create_mask(uint32_t a, uint32_t b);
void separate(uint32_t global_key, uint32_t* local_key, uint8_t* node_key);

#endif
