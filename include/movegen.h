#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdint.h>
#include "board.h"

#define MAX_MOVES 256

typedef struct {
    Square from;
    Square to;
    Piece piece;
    Piece captured_piece;
    Piece promotion;
    uint8_t flags;
} Move;

typedef struct {
    Move moves[MAX_MOVES];
    int count;
} MoveList;

enum {
    MOVE_QUIET   = 0,
    MOVE_CAPTURE = 1 << 0,
    MOVE_PAWN_PROMOTION = 1 << 1,
    MOVE_DOUBLE_PAWN = 1 << 2,
    MOVE_ENPASSANT = 1 << 3
};

void generate_moves_from_sq(const Board *board, Square sq, MoveList *move_list);
void move_to_string(Move *move, char *str, size_t size);

#endif
