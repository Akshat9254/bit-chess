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
    assert(b.pieces[WHITE_PAWN] == RANK_2);
}

static void test_init_board_black_pawns(void) {
    Board b;
    init_board(&b);
    assert(b.pieces[BLACK_PAWN] == RANK_7);
}

static void test_init_board_white_pieces(void) {
    Board b;
    init_board(&b);

    Bitboard knights = (1ULL << B1) | (1ULL << G1);
    Bitboard bishops = (1ULL << C1) | (1ULL << F1);
    Bitboard rooks   = (1ULL << A1) | (1ULL << H1);
    Bitboard queen   = (1ULL << D1);
    Bitboard king    = (1ULL << E1);

    assert(b.pieces[WHITE_KNIGHT] == knights);
    assert(b.pieces[WHITE_BISHOP] == bishops);
    assert(b.pieces[WHITE_ROOK]   == rooks);
    assert(b.pieces[WHITE_QUEEN]  == queen);
    assert(b.pieces[WHITE_KING]   == king);

    Bitboard expected_occ = rank_mask(0) | RANK_2;
    assert(b.occupied[WHITE] == expected_occ);
}

static void test_init_board_black_pieces(void) {
    Board b;
    init_board(&b);

    Bitboard knights = (1ULL << B8) | (1ULL << G8);
    Bitboard bishops = (1ULL << C8) | (1ULL << F8);
    Bitboard rooks   = (1ULL << A8) | (1ULL << H8);
    Bitboard queen   = (1ULL << D8);
    Bitboard king    = (1ULL << E8);

    assert(b.pieces[BLACK_KNIGHT] == knights);
    assert(b.pieces[BLACK_BISHOP] == bishops);
    assert(b.pieces[BLACK_ROOK]   == rooks);
    assert(b.pieces[BLACK_QUEEN]  == queen);
    assert(b.pieces[BLACK_KING]   == king);

    Bitboard expected_occ = rank_mask(7) | RANK_7;
    assert(b.occupied[BLACK] == expected_occ);
}

static void test_init_board_all_bitboard(void) {
    Board b;
    init_board(&b);

    Bitboard expected_all = rank_mask(0) | RANK_2 | RANK_7 | rank_mask(7);
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

