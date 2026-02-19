#ifndef PIECE_H
#define PIECE_H

#include <stdbool.h>
#include <assert.h>
#include "chess_types.h"

extern const char* piece_symbols;

static inline bool is_valid_piece(Piece piece) {
    return 0 <= piece && piece < PIECE_NB;
}

static inline char piece_symbol_of(Piece piece) {
    assert(is_valid_piece(piece));
	return piece_symbols[piece];
}

static inline Color piece_color_of(Piece piece) {
    assert(is_valid_piece(piece));
	return piece <= WHITE_KING ? WHITE : BLACK;
}

#endif
