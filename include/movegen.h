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
    uint8_t flags;
} Move;

typedef struct {
    Move moves[MAX_MOVES];
    int count;
} MoveList;

enum {
    MOVE_QUIET   = 0,
    MOVE_CAPTURE = 1 << 0
};

void generate_moves_from_sq(const Board *board, Square sq, MoveList *move_list);
// static void generate_pawn_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
// static void generate_knight_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
// static void generate_bishop_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
// static void generate_rook_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
// static void generate_queen_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
// static void generate_king_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);

#endif
