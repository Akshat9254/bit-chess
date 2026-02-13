#include "movegen.h"
#include "board.h"
#include "utils.h"

#define KNIGHT_MOVE_OFFSETS_SIZE 8
#define BISHOP_MOVE_OFFSETS_SIZE 4
#define ROOK_MOVE_OFFSETS_SIZE 4

const int8_t knight_move_offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};
const int8_t bishop_move_offsets[] = {-9, -7, 7, 9};
const int8_t rook_move_offsets[] = {-8, -1, 1, 8};

static void generate_pawn_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_knight_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_bishop_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_rook_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_queen_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_king_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void add_pawn_promotion_moves(const Board *board, Square from, Square to, uint8_t extra_flags, MoveList *move_list);
static void add_move(const Board *board, Square from, Square to, uint8_t extra_flags, MoveList *move_list);
static void add_enpassant_move(const Board *board, Square from, Piece captured_piece, MoveList *move_list);
static void add_sliding_moves(const Board *board, Square from, Piece piece, 
    const int8_t move_offsets[], int8_t move_offsets_size, MoveList *move_list);

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

static void generate_pawn_moves(const Board *board, Piece piece, Square sq, MoveList *move_list) {
    uint8_t rank = rank_of_sq(sq);
    char file = file_of_sq(sq);

    int8_t single_push_dir = board->side_to_move == WHITE ? 8 : -8;
    uint8_t start_rank = board->side_to_move == WHITE ? 2 : 7;
    uint8_t promotion_rank = board->side_to_move == WHITE ? 7 : 2;

    Square to = sq + single_push_dir;
    Bitboard all_occ = board_occupancy(board);
    Bitboard enemy_occ = enemy_board_occupancy(board);

    if (!bb_test(all_occ, to)) {
        if (rank == promotion_rank) {
            add_pawn_promotion_moves(board, sq, to, MOVE_QUIET, move_list);
        } else {
            add_move(board, sq, to, MOVE_QUIET, move_list);

            Square to2 = sq + single_push_dir * 2;
            if (rank == start_rank && !bb_test(all_occ, to2)) {
                add_move(board, sq, to2, MOVE_QUIET | MOVE_DOUBLE_PAWN, move_list);
            }
        }
    }

    // left capture
    if (file > 'A') {
        Square to = sq + single_push_dir - 1;
        if (bb_test(enemy_occ, to)) {
            if (rank == promotion_rank) {
                add_pawn_promotion_moves(board, sq, to, MOVE_CAPTURE, move_list);
            } else {
                add_move(board, sq, to, MOVE_CAPTURE, move_list);
            }
        }

        if (to == board->enpassant_sq) {
            add_enpassant_move(board, sq, piece_on_sq(board, sq - 1), move_list);
        }
    }

    // right capture
    if (file < 'H') {
        Square to = sq + single_push_dir + 1;
        if (bb_test(enemy_occ, to)) {
            if (rank == promotion_rank) {
                add_pawn_promotion_moves(board, sq, to, MOVE_CAPTURE, move_list);
            } else {
                add_move(board, sq, to, MOVE_CAPTURE, move_list);
            }
        }

        if (to == board->enpassant_sq) {
            add_enpassant_move(board, sq, piece_on_sq(board, sq + 1), move_list);
        }
    }
}

static void generate_knight_moves(const Board *board, Piece piece, Square sq, MoveList *move_list) {
    Bitboard current_side_occ = current_side_occupancy(board);
    for (int index = 0; index < KNIGHT_MOVE_OFFSETS_SIZE; index++) {
        Square to  = sq + knight_move_offsets[index];
        if (!is_valid_sq(to)) {
            continue;
        }

        if (bb_test(current_side_occ, to)) {
            continue;
        }
        
        uint8_t flags = piece_on_sq(board, to) == NO_PIECE ? MOVE_QUIET : MOVE_CAPTURE;
        add_move(board, sq, to, flags, move_list);
    }
}

static void generate_bishop_moves(const Board *board, Piece piece, Square sq, MoveList *move_list) {
    add_sliding_moves(board, sq, piece, bishop_move_offsets, BISHOP_MOVE_OFFSETS_SIZE, move_list);
}

static void generate_rook_moves(const Board *board, Piece piece, Square sq, MoveList *move_list) {
    add_sliding_moves(board, sq, piece, rook_move_offsets, ROOK_MOVE_OFFSETS_SIZE, move_list);
}

static void generate_queen_moves(const Board *board, Piece pice, Square sq, MoveList *move_list) {

}

static void generate_king_moves(const Board *board, Piece pice, Square sq, MoveList *move_list) {

}

static void add_pawn_promotion_moves(const Board *board, Square from, Square to, uint8_t extra_flags, MoveList *move_list) {
    Piece promotion_option_start = WHITE_KNIGHT + (board->side_to_move * PIECES_PER_SIDE);
    Piece promotion_option_end = WHITE_QUEEN + (board->side_to_move * PIECES_PER_SIDE);

    for (Piece promotion = promotion_option_start; promotion <= promotion_option_end; promotion++) {
        Move move = {
            .from = from,
            .to = to,
            .piece = piece_on_sq(board, from),
            .captured_piece = piece_on_sq(board, to),
            .promotion = promotion,
            .flags = (MOVE_PAWN_PROMOTION | extra_flags)
        };

        move_list->moves[move_list->count++] = move;
    }
}

static void add_move(const Board *board, Square from, Square to, uint8_t flags, MoveList *move_list) {
    Move move = {
        .from = from,
        .to = to,
        .piece = piece_on_sq(board, from),
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
    const int8_t move_offsets[], int8_t move_offsets_size, MoveList *move_list) {
    Bitboard all_occ = board_occupancy(board);
    Bitboard enemy_occ = enemy_board_occupancy(board);

    for (int8_t i = 0; i < move_offsets_size; i++) {
        Square prev = from;
        Square to = prev + move_offsets[i];
        while (is_valid_sq(to) && rank_dist(prev, to) <= 1 && file_dist(prev, to) <= 1) {
            if (!bb_test(all_occ, to)) {
                add_move(board, from, to, MOVE_QUIET, move_list);
                prev = to;
                to += move_offsets[i];
            } else {
                if (bb_test(enemy_occ, to)) {
                    add_move(board, from, to, MOVE_CAPTURE, move_list);
                }

                break;
            }
        }
    }
}
