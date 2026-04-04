#ifndef SQUARE_H
#define SQUARE_H

#include <assert.h>
#include <string.h>

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

static inline Rank char_to_rank(const char rank) {
	return '1' <= rank && rank <= '8' ? rank - '1' : RANK_NONE;
}

static inline File char_to_file(const char file) {
	return 'a' <= file && file <= 'h' ? file - 'a' : FILE_NONE;
}

static inline Square rank_file_to_sq(const Rank rank, const File file) {
	assert(is_rank_file_on_board(rank, file));
    const Square sq = (Square) (rank * FILE_NB + file);
    assert(is_sq_on_board(sq));
    return sq;
}

static inline Square string_to_sq(const char *str) {
	assert(strlen(str) >= 2);
	const File file = char_to_file(str[0]);
	const Rank rank = char_to_rank(str[1]);
	return rank_file_to_sq(rank, file);
}

#endif
