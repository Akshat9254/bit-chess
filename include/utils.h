#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include "types.h"

static inline U64 random_u64(void) {
    return ((U64) rand() & 0xFFFF) |
        ((U64) rand() & 0xFFFF) << 16 |
        ((U64) rand() & 0xFFFF) << 32 |
        ((U64) rand() & 0xFFFF) << 48;
}

#endif
