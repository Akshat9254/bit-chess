#ifndef SQUARE_H
#define SQUARE_H

#include <assert.h>
#include <stdio.h>

#include "types.h"

static inline bool is_sq_on_board(const Square sq) {
	return SQ_A1 <= sq && sq <= SQ_H8;
}

static inline bool is_rank_on_board(const Rank rank) {
	return RANK_1 <= rank && rank <= RANK_8;
}

static inline bool is_file_on_board(const File file) {
	return FILE_A <= file && file <= FILE_H;
}

static inline bool is_rank_file_on_board(const Rank rank, const File file) {
	return is_rank_on_board(rank) && is_file_on_board(file);
}

static inline Rank rank_of_sq(const Square sq) {
    assert(is_sq_on_board(sq));
    const Rank rank = (Rank) (sq >> FILE_NB_LOG);
    assert(is_rank_on_board(rank));
	return rank;
}

static inline File file_of_sq(const Square sq) {
    assert(is_sq_on_board(sq));
    const File file = (File) (sq & FILE_NB_REM_MASK);
    assert(is_file_on_board(file));
	return file;
}

static inline Square rank_file_to_sq(const Rank rank, const File file) {
    if (!is_rank_file_on_board(rank, file)) {
        printf("invalid rank: %d, file: %d\n", rank, file);
    }
    assert(is_rank_file_on_board(rank, file));
    const Square sq = (Square) (rank * FILE_NB + file);
    assert(is_sq_on_board(sq));
    return sq;
}

#endif
