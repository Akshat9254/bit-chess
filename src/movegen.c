#include "movegen.h"
#include "board.h"

#define KNIGHT_MOVE_OFFSETS_SIZE 8
const int8_t knight_move_offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};

static void generate_pawn_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_knight_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_bishop_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_rook_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_queen_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);
static void generate_king_moves(const Board *board, Piece piece, Square sq, MoveList *move_list);

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
    
}

static void generate_knight_moves(const Board *board, Piece piece, Square sq, MoveList *move_list) {
    for (int index = 0; index < KNIGHT_MOVE_OFFSETS_SIZE; index++) {
        int nextSq = sq + knight_move_offsets[index];
        if (nextSq < 0 || nextSq >= SQ_NB) {
            continue;
        }

        Square to = nextSq;
        Piece piece_on_to = piece_on_sq(board, to);
        if (piece_on_to != NO_PIECE && piece_color_of(piece_on_to) == board->side_to_move) {
            continue;
        }

        uint8_t flags = piece_on_to == NO_PIECE ? MOVE_QUIET : MOVE_CAPTURE;

        Move move = {
            .from = sq,
            .to = to,
            .piece = piece_on_sq(board, sq),
            .captured_piece = piece_on_to,
            .flags = flags
        };

        move_list->moves[move_list->count++] = move;
    }
}

static void generate_bishop_moves(const Board *board, Piece piece, Square sq, MoveList *move_list) {

}

static void generate_rook_moves(const Board *board, Piece piece, Square sq, MoveList *move_list) {

}

static void generate_queen_moves(const Board *board, Piece pice, Square sq, MoveList *move_list) {

}

static void generate_king_moves(const Board *board, Piece pice, Square sq, MoveList *move_list) {

}
