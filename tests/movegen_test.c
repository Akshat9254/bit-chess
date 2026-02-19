#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "movegen.h"
#include "attacks.h"

static void test_generate_knight_moves(void) {
    Board board;
    init_board(&board);

    MoveList moves_list;
    moves_list.count = 0;

    generate_moves_from_sq(&board, b1, &moves_list);

    assert(moves_list.count == 2);

    assert(moves_list.moves[0].from == b1);
    assert(moves_list.moves[0].to == a3);
    assert(moves_list.moves[0].piece == WHITE_KNIGHT);
    assert(moves_list.moves[0].captured_piece == NO_PIECE);
    assert(moves_list.moves[0].promotion == NO_PIECE);
    assert(moves_list.moves[0].flags == MOVE_QUIET);

    assert(moves_list.moves[1].from == b1);
    assert(moves_list.moves[1].to == c3);
    assert(moves_list.moves[1].piece == WHITE_KNIGHT);
    assert(moves_list.moves[1].captured_piece == NO_PIECE);
    assert(moves_list.moves[1].promotion == NO_PIECE);
    assert(moves_list.moves[1].flags == MOVE_QUIET);

    clear_board(&board);
    place_piece_on_sq(&board, BLACK_KNIGHT, c3);
    place_piece_on_sq(&board, WHITE_PAWN, a2);
    place_piece_on_sq(&board, BLACK_PAWN, a4);
    board.side_to_move = BLACK;

    moves_list.count = 0;
    generate_moves_from_sq(&board, c3, &moves_list);
    
    assert(moves_list.count == 7);
    assert(moves_list.moves[2].captured_piece != NO_PIECE);
    assert(moves_list.moves[2].flags & MOVE_CAPTURE);

    clear_board(&board);
    place_piece_on_sq(&board, WHITE_KNIGHT, a2);

    board.side_to_move = WHITE;
    moves_list.count = 0;
    generate_moves_from_sq(&board, a2, &moves_list);

    assert(moves_list.count == 3);
}

static void test_generate_pawn_regular_moves(void) {
    Board board;
    init_board(&board);

    MoveList moves_list;
    moves_list.count = 0;

    generate_moves_from_sq(&board, d2, &moves_list);

    assert(moves_list.count == 2);

    assert(moves_list.moves[0].from == d2);
    assert(moves_list.moves[0].to == d3);
    assert(moves_list.moves[0].piece == WHITE_PAWN);
    assert(moves_list.moves[0].captured_piece == NO_PIECE);
    assert(moves_list.moves[0].promotion == NO_PIECE);
    assert(moves_list.moves[0].flags == MOVE_QUIET);

    assert(moves_list.moves[1].from == d2);
    assert(moves_list.moves[1].to == d4);
    assert(moves_list.moves[1].piece == WHITE_PAWN);
    assert(moves_list.moves[1].captured_piece == NO_PIECE);
    assert(moves_list.moves[1].promotion == NO_PIECE);
    assert(moves_list.moves[1].flags == (MOVE_QUIET | MOVE_DOUBLE_PAWN));

    /* test pawn promotion */
    clear_board(&board);

    place_piece_on_sq(&board, BLACK_PAWN, e2);
    board.side_to_move = BLACK;
    moves_list.count = 0;

    generate_moves_from_sq(&board, e2, &moves_list);

    assert(moves_list.count == 4);

    assert(moves_list.moves[0].from == e2);
    assert(moves_list.moves[0].to == e1);
    assert(moves_list.moves[0].piece == BLACK_PAWN);
    assert(moves_list.moves[0].captured_piece == NO_PIECE);
    assert(moves_list.moves[0].promotion == BLACK_KNIGHT);
    assert(moves_list.moves[0].flags == (MOVE_QUIET | MOVE_PAWN_PROMOTION));

    assert(moves_list.moves[1].from == e2);
    assert(moves_list.moves[1].to == e1);
    assert(moves_list.moves[1].piece == BLACK_PAWN);
    assert(moves_list.moves[1].captured_piece == NO_PIECE);
    assert(moves_list.moves[1].promotion == BLACK_BISHOP);
    assert(moves_list.moves[1].flags == (MOVE_QUIET | MOVE_PAWN_PROMOTION));

    assert(moves_list.moves[2].from == e2);
    assert(moves_list.moves[2].to == e1);
    assert(moves_list.moves[2].piece == BLACK_PAWN);
    assert(moves_list.moves[2].captured_piece == NO_PIECE);
    assert(moves_list.moves[2].promotion == BLACK_ROOK);
    assert(moves_list.moves[2].flags == (MOVE_QUIET | MOVE_PAWN_PROMOTION));

    assert(moves_list.moves[3].from == e2);
    assert(moves_list.moves[3].to == e1);
    assert(moves_list.moves[3].piece == BLACK_PAWN);
    assert(moves_list.moves[3].captured_piece == NO_PIECE);
    assert(moves_list.moves[3].promotion == BLACK_QUEEN);
    assert(moves_list.moves[3].flags == (MOVE_QUIET | MOVE_PAWN_PROMOTION));


    /* test en passant move */
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_PAWN, d5);
    place_piece_on_sq(&board, BLACK_PAWN, e5);
    board.enpassant_sq = e6;
    board.side_to_move = WHITE;
    moves_list.count = 0;

    generate_moves_from_sq(&board, d5, &moves_list);

    assert(moves_list.count == 2);

    assert(moves_list.moves[0].from == d5);
    assert(moves_list.moves[0].to == d6);
    assert(moves_list.moves[0].piece == WHITE_PAWN);
    assert(moves_list.moves[0].captured_piece == NO_PIECE);
    assert(moves_list.moves[0].promotion == NO_PIECE);
    assert(moves_list.moves[0].flags == MOVE_QUIET);

    assert(moves_list.moves[1].from == d5);
    assert(moves_list.moves[1].to == e6);
    assert(moves_list.moves[1].piece == WHITE_PAWN);
    assert(moves_list.moves[1].captured_piece == BLACK_PAWN);
    assert(moves_list.moves[1].promotion == NO_PIECE);
    assert(moves_list.moves[1].flags == (MOVE_CAPTURE | MOVE_ENPASSANT));

    /*test left capture */
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_PAWN, a4);
    place_piece_on_sq(&board, BLACK_PAWN, h4);

    moves_list.count = 0;
    board.side_to_move = WHITE;

    generate_moves_from_sq(&board, a4, &moves_list);

    assert(moves_list.count == 1);
    
    /* test right capture move */
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_PAWN, h4);
    place_piece_on_sq(&board, BLACK_PAWN, a6);

    moves_list.count = 0;
    board.side_to_move = WHITE;

    generate_moves_from_sq(&board, h4, &moves_list);

    assert(moves_list.count == 1);
}

static void test_generate_bishop_moves(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_BISHOP, c5);
    place_piece_on_sq(&board, WHITE_PAWN, b6);
    place_piece_on_sq(&board, BLACK_KNIGHT, e3);

    MoveList move_list;
    move_list.count = 0;
    board.side_to_move = BLACK;

    generate_moves_from_sq(&board, c5, &move_list);

    assert(move_list.count == 7);
}

static void test_generate_rook_moves(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_ROOK, g6);
    place_piece_on_sq(&board, WHITE_PAWN, c6);
    place_piece_on_sq(&board, BLACK_KNIGHT, h6);
    place_piece_on_sq(&board, BLACK_PAWN, g4);

    MoveList move_list;
    move_list.count = 0;
    board.side_to_move = WHITE;

    generate_moves_from_sq(&board, g6, &move_list);
    
    assert(move_list.count == 8);
}

static void test_generate_queen_moves(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_QUEEN, d3);
    place_piece_on_sq(&board, BLACK_KNIGHT, b5);
    place_piece_on_sq(&board, WHITE_ROOK, g3);

    MoveList move_list;
    move_list.count = 0;
    board.side_to_move = WHITE;

    generate_moves_from_sq(&board, d3, &move_list);

    assert(move_list.count == 22);
}

static void test_generate_king_moves(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_KING, d3);
    place_piece_on_sq(&board, BLACK_KNIGHT, c2);

    MoveList move_list;
    move_list.count = 0;
    board.side_to_move = BLACK;

    generate_moves_from_sq(&board, d3, &move_list);

    assert(move_list.count == 7);
}

static void test_move_to_string(void) {
    char str[6];
    Move regular_move = {
        .from = b1,
        .to = c3,
        .piece = WHITE_KNIGHT,
        .captured_piece = NO_PIECE,
        .promotion = NO_PIECE,
        .flags = (MOVE_QUIET | MOVE_DOUBLE_PAWN)
    };

    move_to_string(&regular_move, str, 6);

    assert(strcmp(str, "b1c3") == 0);

    Move pawn_promotion_move = {
        .from = f2,
        .to = f1,
        .piece = BLACK_PAWN,
        .captured_piece = NO_PIECE,
        .promotion = BLACK_QUEEN,
        .flags = (MOVE_QUIET | MOVE_PAWN_PROMOTION)
    };

    move_to_string(&pawn_promotion_move, str, 6);

    assert(strcmp(str, "f2f1q") == 0);
}

int main(void) {
    init_attack_tables();
    test_generate_knight_moves();
    test_generate_pawn_regular_moves();
    test_generate_bishop_moves();
    test_generate_rook_moves();
    test_generate_queen_moves();
    test_generate_king_moves();
    test_move_to_string();
    printf("✅ All tests in movegen_test.c passed.\n");
    return 0;
}
