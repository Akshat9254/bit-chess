#ifndef BITBOARD_H
#define BITBOARD_H

#include "square.h"
#include "types.h"

static inline bool bb_test(const Bitboard bb, const Square sq) {
    assert(is_sq_on_board(sq));
	return (bb & (1ULL << sq));
}

static inline void bb_set(Bitboard *bb, const Square sq) {
    assert(is_sq_on_board(sq));
	*bb |= (1ULL << sq);
}

static inline void bb_clear(Bitboard *bb, const Square sq) {
    assert(is_sq_on_board(sq));
	*bb &= ~(1ULL << sq);
}

static inline int bb_popcount(const Bitboard bb) {
	return __builtin_popcountll(bb);
}

static inline int bb_find_lssb_index(const Bitboard bb) {
	return bb == 0 ? -1 : __builtin_ctzll(bb);
}

static inline void bb_pop_lssb(Bitboard *bb) {
	if (*bb == 0) {
        return;
    }

    *bb &= (*bb - 1);
}

void bb_print(Bitboard bb);

#endif
