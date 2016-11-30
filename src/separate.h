#ifndef SEPARATE_H
#define SEPARATE_H

uint32_t createMask(uint32_t a, uint32_t b) {
   uint32_t mask = 0;
   for (uint32_t i=a; i<=b; i++) {
       mask |= 1 << i;
   }
   return mask;
}

void separate(uint32_t globalKey, uint32_t* localKey, uint8_t* nodeKey) {
    uint32_t mask;

    mask = createMask(0, 7);
    uint8_t node = (uint8_t) globalKey & mask;

    mask = createMask(8, 31);
    uint32_t local = globalKey & mask;

    *localKey = local;
    *nodeKey = node;
}
#endif
