#ifndef PIECE_H
#define PIECE_H

#include <assert.h>

#include "types.h"

extern const char* piece_symbols;

static inline bool is_valid_piece(const Piece piece) {
    return WHITE_PAWN <= piece && piece <= BLACK_KING;
}

static inline char piece_symbol_of(const Piece piece) {
    assert(is_valid_piece(piece));
    return piece_symbols[piece];
}

static inline Color piece_color_of(const Piece piece) {
    assert(is_valid_piece(piece));
    return piece < BLACK_PAWN ? COLOR_WHITE : COLOR_BLACK;
}

Piece piece_from_symbol(char symbol);

#endif
