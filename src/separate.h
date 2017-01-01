#ifndef SEPARATE_H
#define SEPARATE_H
#include <stdint.h>

uint32_t createMask(uint32_t a, uint32_t b);

void separate(uint32_t globalKey, uint32_t* localKey, uint8_t* nodeKey);
#endif
