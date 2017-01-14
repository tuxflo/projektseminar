#include "separate.h"
#include <ctype.h>
#include <string.h>

uint32_t create_mask(uint32_t a, uint32_t b) {
   uint32_t mask = 0;
   for (uint32_t i=a; i<=b; i++) {
       mask |= 1 << i;
   }
   return mask;
}

void separate(uint32_t global_key, uint32_t* local_key, uint8_t* node_key) {
    uint32_t mask;

    mask = create_mask(0, 7);
    uint8_t node = (uint8_t) global_key & mask;

    mask = create_mask(8, 31);
    uint32_t local = global_key & mask;

    *local_key = local;
    *node_key = node;
}
