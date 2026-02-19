#include <stdlib.h>
#include <stdio.h>
#include "chess_types.h"
#include "square.h"
#include "piece.h"
#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "movegen.h"

static void generate_pawn_moves(const Board *board, Piece piece, Square from, MoveList *move_list);
static void generate_knight_moves(const Board *board, Piece piece, Square from, MoveList *move_list);
static void generate_bishop_moves(const Board *board, Piece piece, Square from, MoveList *move_list);
static void generate_rook_moves(const Board *board, Piece piece, Square from, MoveList *move_list);
static void generate_queen_moves(const Board *board, Piece piece, Square from, MoveList *move_list);
static void generate_king_moves(const Board *board, Piece piece, Square from, MoveList *move_list);
static void add_pawn_promotion_moves(const Board *board, Square from, Square to, Piece piece, uint8_t extra_flags, MoveList *move_list);
static void add_move(const Board *board, Square from, Square to, Piece piece, uint8_t extra_flags, MoveList *move_list);
static void add_enpassant_move(const Board *board, Square from, Piece captured_piece, MoveList *move_list);
static void add_sliding_moves(const Board *board, Square from, Piece piece, 
    const Offset move_offsets[], size_t offsets_size, MoveList *move_list);

void generate_moves_from_sq(const Board *board, Square sq, MoveList *move_list) {
    Piece piece = piece_on_sq(board, sq);
    if (piece == NO_PIECE || piece_color_of(piece) != board->side_to_move) {
        return;
    }

    switch (piece) {
        case WHITE_PAWN:
        case BLACK_PAWN:
            generate_pawn_moves(board, piece, sq, move_list);
            break;
        
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            generate_knight_moves(board, piece, sq, move_list);
            break;

        case WHITE_BISHOP:
        case BLACK_BISHOP:
            generate_bishop_moves(board, piece, sq, move_list);
            break;
        
        case WHITE_ROOK:
        case BLACK_ROOK:
            generate_rook_moves(board, piece, sq, move_list);
            break;

        case WHITE_QUEEN:
        case BLACK_QUEEN:
            generate_queen_moves(board, piece, sq, move_list);
            break;

        case WHITE_KING:
        case BLACK_KING:
            generate_king_moves(board, piece, sq, move_list);
            break;
        
        default:
            break;

    }
}

void move_to_string(Move *move, char *str, size_t size) {
    char from_file = 'a' + file_of_sq(move->from);
    char from_rank = '1' + rank_of_sq(move->from);
    char to_file = 'a' + file_of_sq(move->to);
    char to_rank = '1' + rank_of_sq(move->to);

    if (move->flags & MOVE_PAWN_PROMOTION) {
        Piece promotion = piece_symbol_of(move->promotion);
        snprintf(str, size, "%c%c%c%c%c", from_file, from_rank, to_file, to_rank, promotion);
    } else {
        snprintf(str, size, "%c%c%c%c", from_file, from_rank, to_file, to_rank);
    }
}

static void generate_pawn_moves(const Board *board, Piece piece, Square from, MoveList *move_list) {
    uint8_t rank = rank_of_sq(from);
    uint8_t file = file_of_sq(from);

    int8_t single_push_dir = board->side_to_move == WHITE ? 8 : -8;
    uint8_t start_rank = board->side_to_move == WHITE ? 1 : 6;
    uint8_t promotion_rank = board->side_to_move == WHITE ? 6 : 1;

    Square to = from + single_push_dir;
    Bitboard all_occ = board_occupancy(board);
    Bitboard enemy_occ = enemy_board_occupancy(board);

    if (!bb_test(all_occ, to)) {
        if (rank == promotion_rank) {
            add_pawn_promotion_moves(board, from, to, piece, MOVE_QUIET, move_list);
        } else {
            add_move(board, from, to, piece, MOVE_QUIET, move_list);

            Square to2 = from + single_push_dir * 2;
            if (rank == start_rank && !bb_test(all_occ, to2)) {
                add_move(board, from, to2, piece, MOVE_QUIET | MOVE_DOUBLE_PAWN, move_list);
            }
        }
    }

    // left capture
    if (file > 0) {
        Square to = from + single_push_dir - 1;
        if (bb_test(enemy_occ, to)) {
            if (rank == promotion_rank) {
                add_pawn_promotion_moves(board, from, to, piece, MOVE_CAPTURE, move_list);
            } else {
                add_move(board, from, to, piece, MOVE_CAPTURE, move_list);
            }
        }

        if (to == board->enpassant_sq) {
            add_enpassant_move(board, from, piece_on_sq(board, from - 1), move_list);
        }
    }

    // right capture
    if (file + 1 < FILE_NB) {
        Square to = from + single_push_dir + 1;
        if (bb_test(enemy_occ, to)) {
            if (rank == promotion_rank) {
                add_pawn_promotion_moves(board, from, to, piece, MOVE_CAPTURE, move_list);
            } else {
                add_move(board, from, to, piece, MOVE_CAPTURE, move_list);
            }
        }

        if (to == board->enpassant_sq) {
            add_enpassant_move(board, from, piece_on_sq(board, from + 1), move_list);
        }
    }
}

static void generate_knight_moves(const Board *board, Piece piece, Square from, MoveList *move_list) {
    Bitboard current_side_occ = current_side_occupancy(board);
    Bitboard enemy_occ = enemy_board_occupancy(board);

    Bitboard attacks = (knight_attacks[from] & ~current_side_occ);
    while (attacks) {
        Square to = pop_lssb(&attacks);
        uint8_t flags = bb_test(enemy_occ, to) ? MOVE_CAPTURE : MOVE_QUIET;
        add_move(board, from, to, piece, flags, move_list);
    }
}

static void generate_bishop_moves(const Board *board, Piece piece, Square from, MoveList *move_list) {
    add_sliding_moves(board, from, piece, diagonal_offsets, DIAGONAL_OFFSETS_SIZE, move_list);
}

static void generate_rook_moves(const Board *board, Piece piece, Square from, MoveList *move_list) {
    add_sliding_moves(board, from, piece, orthogonal_offsets, ORTHOGONAL_OFFSETS_SIZE, move_list);
}

static void generate_queen_moves(const Board *board, Piece piece, Square from, MoveList *move_list) {
    add_sliding_moves(board, from, piece, diagonal_offsets, DIAGONAL_OFFSETS_SIZE, move_list);
    add_sliding_moves(board, from, piece, orthogonal_offsets, ORTHOGONAL_OFFSETS_SIZE, move_list);
}

static void generate_king_moves(const Board *board, Piece piece, Square from, MoveList *move_list) {
    Bitboard current_side_occ = current_side_occupancy(board);
    Bitboard enemy_occ = enemy_board_occupancy(board);

    Bitboard attacks = (king_attacks[from] & ~current_side_occ);
    while (attacks) {
        Square to = pop_lssb(&attacks);
        uint8_t flags = bb_test(enemy_occ, to) ? MOVE_CAPTURE : MOVE_QUIET;
        add_move(board, from, to, piece, flags, move_list);
    }
}

static void add_pawn_promotion_moves(const Board *board, Square from, Square to, Piece piece, 
        uint8_t extra_flags, MoveList *move_list) {
    Piece promotion_option_start = WHITE_KNIGHT + (board->side_to_move * PIECES_PER_SIDE);
    Piece promotion_option_end = WHITE_QUEEN + (board->side_to_move * PIECES_PER_SIDE);

    for (Piece promotion = promotion_option_start; promotion <= promotion_option_end; promotion++) {
        Move move = {
            .from = from,
            .to = to,
            .piece = piece,
            .captured_piece = piece_on_sq(board, to),
            .promotion = promotion,
            .flags = (MOVE_PAWN_PROMOTION | extra_flags)
        };

        move_list->moves[move_list->count++] = move;
    }
}

static void add_move(const Board *board, Square from, Square to, Piece piece, uint8_t flags, MoveList *move_list) {
    Move move = {
        .from = from,
        .to = to,
        .piece = piece,
        .captured_piece = piece_on_sq(board, to),
        .promotion = NO_PIECE,
        .flags = flags
    };

    move_list->moves[move_list->count++] = move;
}

static void add_enpassant_move(const Board *board, Square from, Piece captured_piece, MoveList *move_list) {
    Move move = {
        .from = from,
        .to = board->enpassant_sq,
        .piece = piece_on_sq(board, from),
        .captured_piece = captured_piece,
        .promotion = NO_PIECE,
        .flags = (MOVE_CAPTURE | MOVE_ENPASSANT)
    };

    move_list->moves[move_list->count++] = move;
}

static void add_sliding_moves(const Board *board, Square from, Piece piece, 
    const Offset offsets[], size_t offsets_size, MoveList *move_list) {
    Bitboard all_occ = board_occupancy(board);
    Bitboard enemy_occ = enemy_board_occupancy(board);
    const uint8_t from_rank = rank_of_sq(from);
    const uint8_t from_file = file_of_sq(from);

    for (uint8_t i = 0; i < offsets_size; i++) {
        uint8_t to_rank = from_rank + offsets[i].delta_rank;
        uint8_t to_file = from_file + offsets[i].delta_file;

        while (is_rank_file_on_board(to_rank, to_file)) {
            Square to = to_sq(to_rank, to_file);
            if (!bb_test(all_occ, to)) {
                add_move(board, from, to, piece, MOVE_QUIET, move_list);
                to_rank += offsets[i].delta_rank;
                to_file += offsets[i].delta_file;
            } else {
                if (bb_test(enemy_occ, to)) {
                    add_move(board, from, to, piece, MOVE_CAPTURE, move_list);
                }

                break;
            }
        }
    }
}
