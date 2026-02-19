#include <stdio.h>
#include <assert.h>
#include "board.h"

static Bitboard rank_mask(int rank) {
    Bitboard m = 0;
    for (int file = 0; file < FILE_NB; ++file) {
        int sq = rank * FILE_NB + file;
        m |= (1ULL << sq);
    }
    return m;
}

static void test_init_board_white_pawns(void) {
    Board b;
    init_board(&b);
    assert(b.pieces[WHITE_PAWN] == RANK_2_MASK);
}

static void test_init_board_black_pawns(void) {
    Board b;
    init_board(&b);
    assert(b.pieces[BLACK_PAWN] == RANK_7_MASK);
}

static void test_init_board_white_pieces(void) {
    Board b;
    init_board(&b);

    Bitboard knights = (1ULL << b1) | (1ULL << g1);
    Bitboard bishops = (1ULL << c1) | (1ULL << f1);
    Bitboard rooks   = (1ULL << a1) | (1ULL << h1);
    Bitboard queen   = (1ULL << d1);
    Bitboard king    = (1ULL << e1);

    assert(b.pieces[WHITE_KNIGHT] == knights);
    assert(b.pieces[WHITE_BISHOP] == bishops);
    assert(b.pieces[WHITE_ROOK]   == rooks);
    assert(b.pieces[WHITE_QUEEN]  == queen);
    assert(b.pieces[WHITE_KING]   == king);

    Bitboard expected_occ = rank_mask(0) | RANK_2_MASK;
    assert(b.occupied[WHITE] == expected_occ);
}

static void test_init_board_black_pieces(void) {
    Board b;
    init_board(&b);

    Bitboard knights = (1ULL << b8) | (1ULL << g8);
    Bitboard bishops = (1ULL << c8) | (1ULL << f8);
    Bitboard rooks   = (1ULL << a8) | (1ULL << h8);
    Bitboard queen   = (1ULL << d8);
    Bitboard king    = (1ULL << e8);

    assert(b.pieces[BLACK_KNIGHT] == knights);
    assert(b.pieces[BLACK_BISHOP] == bishops);
    assert(b.pieces[BLACK_ROOK]   == rooks);
    assert(b.pieces[BLACK_QUEEN]  == queen);
    assert(b.pieces[BLACK_KING]   == king);

    Bitboard expected_occ = rank_mask(7) | RANK_7_MASK;
    assert(b.occupied[BLACK] == expected_occ);
}

static void test_init_board_all_bitboard(void) {
    Board b;
    init_board(&b);

    Bitboard expected_all = rank_mask(0) | RANK_2_MASK | RANK_7_MASK | rank_mask(7);
    Bitboard actual_all = board_occupancy(&b);

    assert(actual_all == expected_all);
}

int main(void) {
    test_init_board_white_pawns();
    test_init_board_black_pawns();
    test_init_board_white_pieces();
    test_init_board_black_pieces();
    test_init_board_all_bitboard();
    printf("✅ All tests in board_test.c passed.\n");
    return 0;
}

