#include <assert.h>
#include <stdio.h>
#include "movegen.h"

static void test_generate_knight_moves(void) {
    Board board;
    init_board(&board);

    MoveList moves_list;
    moves_list.count = 0;

    generate_moves_from_sq(&board, B1, &moves_list);

    assert(moves_list.count == 2);

    assert(moves_list.moves[0].from == B1);
    assert(moves_list.moves[0].to == A3);
    assert(moves_list.moves[0].piece == WHITE_KNIGHT);
    assert(moves_list.moves[0].captured_piece == NO_PIECE);
    assert(moves_list.moves[0].promotion == NO_PIECE);
    assert(moves_list.moves[0].flags == MOVE_QUIET);

    assert(moves_list.moves[1].from == B1);
    assert(moves_list.moves[1].to == C3);
    assert(moves_list.moves[1].piece == WHITE_KNIGHT);
    assert(moves_list.moves[1].captured_piece == NO_PIECE);
    assert(moves_list.moves[1].promotion == NO_PIECE);
    assert(moves_list.moves[1].flags == MOVE_QUIET);

    clear_board(&board);
    place_piece_on_sq(&board, BLACK_KNIGHT, C3);
    place_piece_on_sq(&board, WHITE_PAWN, A2);
    place_piece_on_sq(&board, BLACK_PAWN, A4);
    board.side_to_move = BLACK;

    moves_list.count = 0;
    generate_moves_from_sq(&board, C3, &moves_list);
    
    assert(moves_list.count == 7);
    assert(moves_list.moves[2].captured_piece != NO_PIECE);
    assert(moves_list.moves[2].flags & MOVE_CAPTURE);
}

static void test_generate_pawn_regular_moves(void) {
    Board board;
    init_board(&board);

    MoveList moves_list;
    moves_list.count = 0;

    generate_moves_from_sq(&board, D2, &moves_list);

    assert(moves_list.count == 2);

    assert(moves_list.moves[0].from == D2);
    assert(moves_list.moves[0].to == D3);
    assert(moves_list.moves[0].piece == WHITE_PAWN);
    assert(moves_list.moves[0].captured_piece == NO_PIECE);
    assert(moves_list.moves[0].promotion == NO_PIECE);
    assert(moves_list.moves[0].flags == MOVE_QUIET);

    assert(moves_list.moves[1].from == D2);
    assert(moves_list.moves[1].to == D4);
    assert(moves_list.moves[1].piece == WHITE_PAWN);
    assert(moves_list.moves[1].captured_piece == NO_PIECE);
    assert(moves_list.moves[1].promotion == NO_PIECE);
    assert(moves_list.moves[1].flags == (MOVE_QUIET | MOVE_DOUBLE_PAWN));

    /* test pawn promotion */
    clear_board(&board);

    place_piece_on_sq(&board, BLACK_PAWN, E2);
    board.side_to_move = BLACK;
    moves_list.count = 0;

    generate_moves_from_sq(&board, E2, &moves_list);

    assert(moves_list.count == 4);

    assert(moves_list.moves[0].from == E2);
    assert(moves_list.moves[0].to == E1);
    assert(moves_list.moves[0].piece == BLACK_PAWN);
    assert(moves_list.moves[0].captured_piece == NO_PIECE);
    assert(moves_list.moves[0].promotion == BLACK_KNIGHT);
    assert(moves_list.moves[0].flags == (MOVE_QUIET | MOVE_PAWN_PROMOTION));

    assert(moves_list.moves[1].from == E2);
    assert(moves_list.moves[1].to == E1);
    assert(moves_list.moves[1].piece == BLACK_PAWN);
    assert(moves_list.moves[1].captured_piece == NO_PIECE);
    assert(moves_list.moves[1].promotion == BLACK_BISHOP);
    assert(moves_list.moves[1].flags == (MOVE_QUIET | MOVE_PAWN_PROMOTION));

    assert(moves_list.moves[2].from == E2);
    assert(moves_list.moves[2].to == E1);
    assert(moves_list.moves[2].piece == BLACK_PAWN);
    assert(moves_list.moves[2].captured_piece == NO_PIECE);
    assert(moves_list.moves[2].promotion == BLACK_ROOK);
    assert(moves_list.moves[2].flags == (MOVE_QUIET | MOVE_PAWN_PROMOTION));

    assert(moves_list.moves[3].from == E2);
    assert(moves_list.moves[3].to == E1);
    assert(moves_list.moves[3].piece == BLACK_PAWN);
    assert(moves_list.moves[3].captured_piece == NO_PIECE);
    assert(moves_list.moves[3].promotion == BLACK_QUEEN);
    assert(moves_list.moves[3].flags == (MOVE_QUIET | MOVE_PAWN_PROMOTION));


    /* test en passant move */
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_PAWN, D5);
    place_piece_on_sq(&board, BLACK_PAWN, E5);
    board.enpassant_sq = E6;
    board.side_to_move = WHITE;
    moves_list.count = 0;

    generate_moves_from_sq(&board, D5, &moves_list);

    assert(moves_list.count == 2);

    assert(moves_list.moves[0].from == D5);
    assert(moves_list.moves[0].to == D6);
    assert(moves_list.moves[0].piece == WHITE_PAWN);
    assert(moves_list.moves[0].captured_piece == NO_PIECE);
    assert(moves_list.moves[0].promotion == NO_PIECE);
    assert(moves_list.moves[0].flags == MOVE_QUIET);

    assert(moves_list.moves[1].from == D5);
    assert(moves_list.moves[1].to == E6);
    assert(moves_list.moves[1].piece == WHITE_PAWN);
    assert(moves_list.moves[1].captured_piece == BLACK_PAWN);
    assert(moves_list.moves[1].promotion == NO_PIECE);
    assert(moves_list.moves[1].flags == (MOVE_CAPTURE | MOVE_ENPASSANT));
}

int main(void) {
    test_generate_knight_moves();
    test_generate_pawn_regular_moves();
    printf("✅ All tests in movegen_test.c passed.\n");
    return 0;
}
