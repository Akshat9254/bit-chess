#ifndef SQUARE_H
#define SQUARE_H

#include <stdbool.h>
#include <assert.h>
#include "chess_types.h"

static inline bool is_sq_on_board(Square sq) {
	return 0 <= sq && sq < SQ_NB;
}

static inline bool is_rank_file_on_board(int8_t rank, int8_t file) {
	return 0 <= rank && rank < RANK_NB && 0 <= file && file < FILE_NB;
}

static inline uint8_t rank_of_sq(Square sq) {
    assert(is_sq_on_board(sq));
	return (sq >> FILE_NB_LOG);
}

static inline uint8_t file_of_sq(Square sq) {
    assert(is_sq_on_board(sq));
	return (sq & FILE_NB_REM_MASK);
}

static inline Square to_sq(uint8_t rank, uint8_t file) {
	Square sq = rank * FILE_NB + file;
	return is_sq_on_board(sq) ? sq : NO_SQ;
}

uint8_t rank_dist(Square sq1, Square sq2);
uint8_t file_dist(Square sq1, Square sq2);

#endif
