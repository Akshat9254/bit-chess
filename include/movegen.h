#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "move.h"

void generate_all_legal_moves(const Board *board, MoveList *legal_moves);
void generate_all_pawn_legal_moves(const Board *board, MoveList *pseudo_legal_moves);
void generate_all_knight_legal_moves(const Board *board, MoveList *pseudo_legal_moves);
void generate_all_bishop_legal_moves(const Board *board, MoveList *pseudo_legal_moves);
void generate_all_rook_legal_moves(const Board *board, MoveList *pseudo_legal_moves);
void generate_all_queen_legal_moves(const Board *board, MoveList *pseudo_legal_moves);
void generate_all_king_legal_moves(const Board *board, MoveList *pseudo_legal_moves);

#endif
