#ifndef MOVE_H
#define MOVE_H

#include <stddef.h>

#include "piece.h"
#include "types.h"

#define MAX_MOVES 256

/*
 * Move (16 bits):
 * 0000: flags (4 bits)
 * 00000: to square (6 bits)
 * 00000: from square (6 bits)
 */

enum: U16 {
    MOVE_NONE = 0
};

enum: U8 {
    MOVE_QUIET              = 0,    // 0000
    MOVE_PAWN_DOUBLE_PUSH   = 1,    // 0001
    MOVE_KING_CASTLE        = 2,    // 0010
    MOVE_QUEEN_CASTLE       = 3,    // 0011
    MOVE_CAPTURE            = 4,    // 0100
    MOVE_ENPASSANT          = 5,    // 0101
    MOVE_PROMO_N            = 8,    // 1000
    MOVE_PROMO_B            = 9,    // 1001
    MOVE_PROMO_R            = 10,   // 1010
    MOVE_PROMO_Q            = 11,   // 1011
    MOVE_PROMO_N_CAP        = 12,   // 1100
    MOVE_PROMO_B_CAP        = 13,   // 1101
    MOVE_PROMO_R_CAP        = 14,   // 1110
    MOVE_PROMO_Q_CAP        = 15,   // 1111
} MoveFlags;

typedef struct {
    size_t count;
    Move moves[MAX_MOVES];
    U16 scores[MAX_MOVES];
} MoveList;

void move_to_string(Move move, Color color, char *str, size_t size);

static inline Move create_move(const Square from, const Square to, const U8 flags) {
    return (Move) (from | (to << 6) | (flags << 12));
}

static inline Square move_get_from(const Move move) {
    return (Square) (move & 0x3f);
}

static inline Square move_get_to(const Move move) {
    return (Square) ((move >> 6) & 0x3f);
}

static inline U8 move_get_flags(const Move move) {
    return ((move >> 12) & 0x0f);
}

static inline bool is_capture(const U8 flags) {
    return (flags & 0x4);
}

static inline bool is_promo(const U8 flags) {
    return (flags & 0x8);
}

static inline bool is_castle(const U8 flags) {
    return (flags & 0x0e) == 2;
}

static inline Piece move_get_promo_piece(const U8 flags, const Color color) {
    const U8 color_offset = color * PIECE_PER_SIDE;
    const U8 piece_offset = flags & 0x3;
    const Piece piece = WHITE_KNIGHT + color_offset + piece_offset;
    assert(is_valid_piece(piece));
    return piece;
}

static inline void add_move(MoveList *move_list, const Move move) {
    move_list->moves[move_list->count++] = move;
}

#endif
