#ifndef BIT_SEARCH_H
#define BIT_SEARCH_H

#include "board.h"

#define MAX_DEPTH 64

typedef struct {
    U64 start_time;
    U64 nodes;
    U32 soft_limit;
    U32 hard_limit;
    U32 depth;
    volatile bool aborted;
} SearchInfo;

extern SearchInfo search_info;

Move search_for_best_move(Board *board);

#endif
