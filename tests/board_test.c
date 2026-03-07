#include "board.h"

#include "fen_parser.h"
#include "move.h"
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
    ASSERT_INT_EQ(SQ_NONE, board.kings_sq[COLOR_WHITE], white_king_sq_empty_init);
    ASSERT_INT_EQ(SQ_NONE, board.kings_sq[COLOR_BLACK], black_king_sq_empty_init);

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

static int test_is_sq_attacked_leapers(void) {
    Board board;
    clear_board(&board);

    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_F3);

    ASSERT_INT_EQ(true, is_sq_attacked(&board, SQ_D4, COLOR_WHITE), "Knight on f3 should attack d4");
    ASSERT_INT_EQ(false, is_sq_attacked(&board, SQ_D4, COLOR_BLACK), "Black should not be attacking d4");

    clear_board(&board);
    place_piece_on_sq(&board, BLACK_PAWN, SQ_E5);
    ASSERT_INT_EQ(true, is_sq_attacked(&board, SQ_D4, COLOR_BLACK), "Black pawn on e5 should attack d4");

    clear_board(&board);
    place_piece_on_sq(&board, WHITE_KING, SQ_E3);
    ASSERT_INT_EQ(true, is_sq_attacked(&board, SQ_D4, COLOR_WHITE), "White king on e3 should attack d4");

    return 0;
}

static int test_is_sq_attacked_sliders(void) {
    Board board;

    clear_board(&board);
    place_piece_on_sq(&board, WHITE_QUEEN, SQ_A4);
    ASSERT_INT_EQ(true, is_sq_attacked(&board, SQ_D4, COLOR_WHITE), "White Queen on a4 should attack d4");

    place_piece_on_sq(&board, BLACK_PAWN, SQ_C4);
    ASSERT_INT_EQ(false, is_sq_attacked(&board, SQ_D4, COLOR_WHITE), "White Queen attack on d4 should be blocked by c4");

    clear_board(&board);
    place_piece_on_sq(&board, BLACK_BISHOP, SQ_G7);
    ASSERT_INT_EQ(true, is_sq_attacked(&board, SQ_D4, COLOR_BLACK), "Black Bishop on g7 should attack d4");

    return 0;
}

static int test_is_sq_attacked_complex(void) {
    Board board;
    clear_board(&board);

    place_piece_on_sq(&board, WHITE_ROOK, SQ_E1);
    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_E2);

    ASSERT_INT_EQ(false, is_sq_attacked(&board, SQ_E4, COLOR_WHITE), "Rook on e1 should be blocked by knight on e2");

    clear_board(&board);
    place_piece_on_sq(&board, WHITE_ROOK, SQ_D1);
    place_piece_on_sq(&board, BLACK_QUEEN, SQ_D5);
    ASSERT_INT_EQ(true, is_sq_attacked(&board, SQ_D4, COLOR_WHITE), "Rook on d1 attacks d4 despite blocker on d5");

    return 0;
}

static int test_make_unmake_cycle() {
    Board board;
    StateInfo state;
    FenError err;
    assert(fen_init_board(&board, FEN_START_POS, &err));

    const Bitboard original_occ = board.occupied[COLOR_BOTH];
    const U8 original_rights = board.castling_rights;
    const Square original_ep = board.enpassant_sq;

    // Move: White Pawn e2 -> e4 (Pawn Double Push)
    const Move e2e4 = create_move(SQ_E2, SQ_E4, MOVE_PAWN_DOUBLE_PUSH);

    // --- TEST MAKE ---
    make_move(&board, e2e4, &state);

    ASSERT_INT_EQ(PIECE_NONE, board.mailbox[SQ_E2], test_make_e2e4_clear_source);
    ASSERT_INT_EQ(WHITE_PAWN, board.mailbox[SQ_E4], test_make_e2e4_place_dest);
    ASSERT_INT_EQ(SQ_E3, board.enpassant_sq, test_make_e2e4_ep_set);
    ASSERT_INT_EQ(COLOR_BLACK, board.side_to_move, test_make_e2e4_side_flip);

    // --- TEST UNMAKE ---
    unmake_move(&board, &state);

    ASSERT_UINT64_EQ(original_occ, board.occupied[COLOR_BOTH], test_unmake_restores_bitboards);
    ASSERT_INT_EQ(WHITE_PAWN, board.mailbox[SQ_E2], test_unmake_restores_mailbox);
    ASSERT_INT_EQ(original_rights, board.castling_rights, test_unmake_restores_rights);
    ASSERT_INT_EQ(original_ep, board.enpassant_sq, test_unmake_restores_ep);
    ASSERT_INT_EQ(COLOR_WHITE, board.side_to_move, test_unmake_restores_side);

    return 0;
}

static int test_make_unmake_capture_and_clock_reset() {
    Board board;
    StateInfo state;
    FenError err;
    // Position where White can capture a piece: White Knight on f3 captures Black Pawn on e5
    assert(fen_init_board(&board, "rnbqkbnr/pppp1ppp/8/4p3/5N2/8/PPPPPPPP/RNBQKB1R w KQkq - 5 3", &err));

    // Move: Knight f4 -> e5 (Capture)
    const Move n_cap_e5 = create_move(SQ_F4, SQ_E5, MOVE_CAPTURE);

    ASSERT_INT_EQ(5, board.half_move_clock, initial_clock_check);

    make_move(&board, n_cap_e5, &state);

    ASSERT_INT_EQ(0, board.half_move_clock, clock_reset_on_capture);
    ASSERT_INT_EQ(BLACK_PAWN, state.captured_piece, state_info_saved_captured_piece);
    ASSERT_INT_EQ(WHITE_KNIGHT, board.mailbox[SQ_E5], piece_replaced_on_dest);

    unmake_move(&board, &state);

    ASSERT_INT_EQ(5, board.half_move_clock, clock_restored_after_unmake);
    ASSERT_INT_EQ(BLACK_PAWN, board.mailbox[SQ_E5], captured_piece_restored);

    return 0;
}

static int test_make_unmake_castling_integrity() {
    Board board;
    StateInfo state;
    FenError err;
    // FEN: KingSide castling ready for White
    assert(fen_init_board(&board, "rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1", &err));

    const Move castle_ks = create_move(SQ_E1, SQ_G1, MOVE_KING_CASTLE);

    make_move(&board, castle_ks, &state);

    // Check King
    ASSERT_INT_EQ(WHITE_KING, board.mailbox[SQ_G1], king_placed_on_g1);
    ASSERT_INT_EQ(SQ_G1, board.kings_sq[COLOR_WHITE], king_sq_updated);
    // Check Rook
    ASSERT_INT_EQ(WHITE_ROOK, board.mailbox[SQ_F1], rook_placed_on_f1);
    ASSERT_INT_EQ(PIECE_NONE, board.mailbox[SQ_H1], rook_cleared_from_h1);
    // Check Rights (Both sides for White should be gone)
    ASSERT_BOOL_EQ(false, (board.castling_rights & 0x3), castling_rights_removed);

    unmake_move(&board, &state);

    ASSERT_INT_EQ(WHITE_KING, board.mailbox[SQ_E1], king_back_to_e1);
    ASSERT_INT_EQ(WHITE_ROOK, board.mailbox[SQ_H1], rook_back_to_h1);
    ASSERT_INT_EQ(SQ_E1, board.kings_sq[COLOR_WHITE], king_sq_restored);
    ASSERT_INT_EQ(0xF, board.castling_rights, rights_restored);

    return 0;
}

int main(void) {
    if (test_clear_board() != 0) return 1;
    if (test_place_piece_on_sq() != 0) return 1;
    if (test_clear_sq() != 0) return 1;
    if (test_is_sq_attacked_leapers() != 0) return 1;
    if (test_is_sq_attacked_sliders() != 0) return 1;
    if (test_is_sq_attacked_complex() != 0) return 1;

    if (test_make_unmake_cycle() != 0) return 1;
    if (test_make_unmake_capture_and_clock_reset() != 0) return 1;
    if (test_make_unmake_castling_integrity() != 0) return 1;

    printf("✅ All tests in board_test.c passed.\n");
    return 0;
}
