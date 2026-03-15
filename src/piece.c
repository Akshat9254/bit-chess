#include "piece.h"

const char* piece_symbols = "PNBRQKpnbrqk  ";
const Color piece_colors[PIECE_NB] = {
    COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE,
    COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK,
};

Piece piece_from_symbol(const char symbol) {
    switch (symbol) {
        case 'P': return WHITE_PAWN;
        case 'N': return WHITE_KNIGHT;
        case 'B': return WHITE_BISHOP;
        case 'R': return WHITE_ROOK;
        case 'Q': return WHITE_QUEEN;
        case 'K': return WHITE_KING;
        case 'p': return BLACK_PAWN;
        case 'n': return BLACK_KNIGHT;
        case 'b': return BLACK_BISHOP;
        case 'r': return BLACK_ROOK;
        case 'q': return BLACK_QUEEN;
        case 'k': return BLACK_KING;
        default: return PIECE_NONE;
    }
}

const PieceType piece_types[PIECE_NB] = {
    [WHITE_PAWN] = PT_PAWN,     [BLACK_PAWN] = PT_PAWN,
    [WHITE_KNIGHT] = PT_KNIGHT,   [BLACK_KNIGHT] = PT_KNIGHT,
    [WHITE_BISHOP] = PT_BISHOP,   [BLACK_BISHOP] = PT_BISHOP,
    [WHITE_ROOK] = PT_ROOK,     [BLACK_ROOK] = PT_ROOK,
    [WHITE_QUEEN] = PT_QUEEN,    [BLACK_QUEEN] = PT_QUEEN,
    [WHITE_KING] = PT_KING,   [BLACK_KING] = PT_KING,
};
