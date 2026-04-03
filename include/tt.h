#ifndef TT_H
#define TT_H

#include <stdio.h>

#include "types.h"

#define TT_SIZE (1 << 20)

typedef enum: U8 {
    TT_EXACT = 0,
    TT_LOWER = 1,
    TT_UPPER = 2,
} TT_ScoreType;

typedef struct {
    ZobristHash hash;
    int score;
    Move move;
    U8 depth;
    TT_ScoreType score_type;
} TTEntry;

static inline size_t tt_index_from_hash(const ZobristHash hash) {
    return (hash & (TT_SIZE - 1));
}

extern TTEntry tt[TT_SIZE];

#endif
