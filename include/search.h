#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"

#define MAX_DEPTH 64

typedef struct {
    U64 start_time;
    U64 soft_limit;
    U64 hard_limit;
    U64 nodes;
    volatile bool aborted;
} SearchInfo;

extern SearchInfo search_info;

Move search_for_best_move(Board *board);

#endif
