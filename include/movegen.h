#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "move.h"

void generate_all_legal_moves(Board *board, MoveList *legal_moves);
void generate_all_legal_capture_moves(Board *board, MoveList *capture_moves);
void generate_all_pawn_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves);
void generate_all_pawn_pseudo_legal_capture_moves(const Board *board, MoveList *pseudo_legal_moves);
void generate_all_knight_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, Bitboard filter_mask);
void generate_all_bishop_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, Bitboard filter_mask);
void generate_all_rook_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, Bitboard filter_mask);
void generate_all_queen_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, Bitboard filter_mask);
void generate_all_king_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, Bitboard filter_mask);

#endif
