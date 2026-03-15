#ifndef PIECE_H
#define PIECE_H

#include <assert.h>

#include "types.h"

extern const char* piece_symbols;
extern const Color piece_colors[PIECE_NB];
extern const PieceType piece_types[PIECE_NB];

static inline bool is_valid_piece(const Piece piece) {
    return piece <= BLACK_KING;
}

static inline char piece_symbol_of(const Piece piece) {
    assert(is_valid_piece(piece));
    return piece_symbols[piece];
}

static inline Color piece_color_of(const Piece piece) {
    assert(is_valid_piece(piece));
    return piece_colors[piece];
}

static inline PieceType piece_type_of(const Piece piece) {
    assert(is_valid_piece(piece));
    return piece_types[piece];
}

Piece piece_from_symbol(char symbol);

#endif
