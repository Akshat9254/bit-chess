#include "board.h"
#include "test_util.h"

static int test_clear_board(void) {
    Board board;
    clear_board(&board);

    ASSERT_INT_EQ(COLOR_WHITE, board.side_to_move, side_to_move_init);
    ASSERT_INT_EQ(SQ_NONE, board.enpassant_sq, enpassant_init);
    ASSERT_INT_EQ(CASTLE_ANY, board.castling_rights, castling_init);
    ASSERT_INT_EQ(1, board.full_move_number, move_number_init);

    ASSERT_INT_EQ(PIECE_NONE, board.mailbox[SQ_E4], mailbox_empty_init);
    ASSERT_UINT64_EQ(0ULL, board.occupied[COLOR_BOTH], occupancy_empty_init);

    return 0;
}

static int test_place_piece_on_sq(void) {
    Board board;
    clear_board(&board);

    const Square sq = SQ_D4;
    const Piece piece = WHITE_KNIGHT;
    const Color color = COLOR_WHITE;

    place_piece_on_sq(&board, piece, sq);

    ASSERT_INT_EQ(piece, board.mailbox[sq], placement_mailbox);
    ASSERT_INT_EQ(false, is_sq_empty(&board, sq), is_sq_empty_check);

    const Bitboard expected_mask = (1ULL << sq);
    ASSERT_UINT64_EQ(expected_mask, board.pieces[piece], placement_piece_bb);
    ASSERT_UINT64_EQ(expected_mask, board.occupied[color], placement_color_bb);
    ASSERT_UINT64_EQ(expected_mask, board.occupied[COLOR_BOTH], placement_both_bb);

    return 0;
}

static  int test_clear_sq(void) {
    Board board;
    clear_board(&board);

    const Square sq = SQ_A1;
    place_piece_on_sq(&board, BLACK_ROOK, sq);

    clear_sq(&board, sq);

    ASSERT_INT_EQ(PIECE_NONE, board.mailbox[sq], clear_mailbox_sync);
    ASSERT_INT_EQ(true, is_sq_empty(&board, sq), clear_is_empty_sync);
    ASSERT_UINT64_EQ(0ULL, board.pieces[BLACK_ROOK], clear_piece_bb);
    ASSERT_UINT64_EQ(0ULL, board.occupied[COLOR_BLACK], clear_color_bb);
    ASSERT_UINT64_EQ(0ULL, board.occupied[COLOR_BOTH], clear_both_bb);

    return 0;
}

int main(void) {
    if (test_clear_board() != 0) return 1;
    if (test_place_piece_on_sq() != 0) return 1;
    if (test_clear_sq() != 0) return 1;

    printf("✅ All tests in board_test.c passed.\n");
    return 0;
}
