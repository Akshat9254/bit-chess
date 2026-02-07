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
    assert(moves_list.moves[0].flags == MOVE_QUIET);

    assert(moves_list.moves[1].from == B1);
    assert(moves_list.moves[1].to == C3);
    assert(moves_list.moves[1].piece == WHITE_KNIGHT);
    assert(moves_list.moves[1].captured_piece == NO_PIECE);
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

int main(void) {
    test_generate_knight_moves();
    printf("✅ All tests in movegen_test.c passed.\n");
    return 0;
}
