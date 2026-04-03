#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <sys/time.h>

#include "types.h"

static inline U64 random_u64(void) {
    return ((U64) rand() & 0xFFFF) |
        ((U64) rand() & 0xFFFF) << 16 |
        ((U64) rand() & 0xFFFF) << 32 |
        ((U64) rand() & 0xFFFF) << 48;
}

static U64 get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (U64) tv.tv_sec * 1000 + (U64) tv.tv_usec / 1000;
}

#endif
