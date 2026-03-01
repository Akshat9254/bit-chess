#include "fen_parser.h"
#include "test_util.h"

static int test_fen_startpos(void) {
    Board board;
    FenError err;
    const char* start_fen = FEN_START_POS;

    const bool success = fen_init_board(&board, start_fen, &err);

    if (!success) {
        fprintf(stdout, "error in fen parsing: %s\n", err.message);
    }

    ASSERT_INT_EQ(true, success, startpos_parse_status);
    ASSERT_INT_EQ(WHITE_ROOK, board.mailbox[SQ_A1], startpos_white_rook);
    ASSERT_INT_EQ(BLACK_KING, board.mailbox[SQ_E8], startpos_black_king);
    ASSERT_INT_EQ(COLOR_WHITE, board.side_to_move, startpos_side);
    ASSERT_INT_EQ(CASTLE_ANY, board.castling_rights, startpos_castling);
    ASSERT_INT_EQ(SQ_NONE, board.enpassant_sq, startpos_ep);

    char fen_output[256];
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(start_fen, fen_output, startpos_fen_serialization);

    return 0;
}

static int test_fen_midgame(void) {
    Board board;
    FenError err;
    const char* mid_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

    const bool success = fen_init_board(&board, mid_fen, &err);

    ASSERT_INT_EQ(true, success, midgame_parse_status);
    // SQ_A8 is r (Black Rook)
    ASSERT_INT_EQ(BLACK_ROOK, board.mailbox[SQ_A8], midgame_a8_rook);
    // 3 squares empty after r -> SQ_E8 is k (Black King)
    ASSERT_INT_EQ(BLACK_KING, board.mailbox[SQ_E8], midgame_e8_king);

    char fen_output[256];
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(mid_fen, fen_output, mid_fen_serialization);

    return 0;
}

static int test_fen_errors(void) {
    Board board;
    FenError err;

    // 1. Invalid piece character 'X'
    const bool res1 = fen_init_board(&board, "rnbqkbnr/ppppXppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &err);
    ASSERT_INT_EQ(false, res1, error_invalid_piece_status);
    ASSERT_INT_EQ(FEN_ERR_INVALID_PIECE, err.code, error_code_piece);

    // 2. Missing a rank (only 7 ranks provided)
    const bool res2 = fen_init_board(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP w KQkq - 0 1", &err);
    ASSERT_INT_EQ(false, res2, error_missing_rank_status);
    ASSERT_INT_EQ(FEN_ERR_INVALID_RANK_SEPARATOR, err.code, error_code_rank);

    // 3. Invalid color character
    const bool res3 = fen_init_board(&board, "8/8/8/8/8/8/8/8 x KQkq - 0 1", &err);
    ASSERT_INT_EQ(false, res3, error_invalid_color_status);
    ASSERT_INT_EQ(FEN_ERR_INVALID_SIDE, err.code, error_code_color);

    return 0;
}

static int test_fen_en_passant(void) {
    Board board;
    FenError err;
    // Standard Ruy Lopez opening: 1. e4 e5 2. Nf3 Nc6 3. Bb5 a6
    // After 1. e4, the FEN should have e3 as the EP square.
    const char* ep_fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";

    const bool success = fen_init_board(&board, ep_fen, &err);

    ASSERT_INT_EQ(true, success, ep_parse_status);
    ASSERT_INT_EQ(SQ_E3, board.enpassant_sq, ep_square_match);

    char fen_output[256];
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(ep_fen, fen_output, ep_fen_serialization);

    return 0;
}

static int test_fen_castling_rights(void) {
    Board board;
    FenError err;
    char fen_output[256];

    // 1. All rights in standard order
    const char *all_castling_rights_fen = "8/8/8/8/8/8/8/8 w KQkq - 0 1";
    ASSERT_INT_EQ(true, fen_init_board(&board, all_castling_rights_fen, &err), fen_castle_full_status);
    ASSERT_INT_EQ(15, board.castling_rights, castle_full_bits);
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(all_castling_rights_fen, fen_output, all_castling_rights_fen_serialization);

    // 2. Reordered and repeated symbols: "qKkQk"
    // Logic: Should still result in all bits set (15) and not toggle them off.
    const char *reordered_castling_rights_fen = "8/8/8/8/8/8/8/8 w qKkQk - 0 1";
    ASSERT_INT_EQ(true, fen_init_board(&board, reordered_castling_rights_fen, &err), fen_castle_repeat_status);
    ASSERT_INT_EQ(15, board.castling_rights, castle_repeat_bits);
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ("8/8/8/8/8/8/8/8 w KQkq - 0 1", fen_output, reordered_castling_rights_fen_serialization);

    // 3. Partial rights: "Kq" (White Kingside + Black Queenside)
    // Binary: 0001 | 1000 = 9
    const char *partial_castling_rights_fen = "8/8/8/8/8/8/8/8 w Kq - 0 1";
    ASSERT_INT_EQ(true, fen_init_board(&board, partial_castling_rights_fen, &err), fen_castle_partial_status);
    ASSERT_INT_EQ(9, board.castling_rights, castle_partial_bits);
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(partial_castling_rights_fen, fen_output, partial_castling_rights_fen_serialization);

    // 4. No rights: "-"
    const char *no_castling_rights_fen = "8/8/8/8/8/8/8/8 w - - 0 1";
    ASSERT_INT_EQ(true, fen_init_board(&board, no_castling_rights_fen, &err), fen_castle_none_status);
    ASSERT_INT_EQ(0, board.castling_rights, castle_none_bits);
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(no_castling_rights_fen, fen_output, no_castling_rights_fen_serialization);

    return 0;
}

static int test_fen_clocks(void) {
    Board board;
    FenError err;
    char fen_output[256];

    // 1. Standard start values
    const char *standard_clock_values_fen = "8/8/8/8/8/8/8/8 w - - 0 1";
    ASSERT_INT_EQ(true, fen_init_board(&board, standard_clock_values_fen, &err), fen_clock_start_status);
    ASSERT_INT_EQ(0, board.half_move_clock, half_move_start);
    ASSERT_INT_EQ(1, board.full_move_number, full_move_start);
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(standard_clock_values_fen, fen_output, standard_clock_values_fen_serialization);

    // 2. High values (testing multi-digit sscanf logic)
    // 49 half-moves (one before 50-move draw), 125 full moves
    const char *multidigit_clock_values_fen = "8/8/8/8/8/8/8/8 w - - 49 125";
    ASSERT_INT_EQ(true, fen_init_board(&board, multidigit_clock_values_fen, &err), fen_clock_high_status);
    ASSERT_INT_EQ(49, board.half_move_clock, half_move_high);
    ASSERT_INT_EQ(125, board.full_move_number, full_move_high);
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(multidigit_clock_values_fen, fen_output, multidigit_clock_values_fen_serialization);

    // 3. Maximum U8 value for half-move (255)
    const char *max_half_move_value_fen = "8/8/8/8/8/8/8/8 w - - 255 1000";
    ASSERT_INT_EQ(true, fen_init_board(&board, max_half_move_value_fen, &err), fen_clock_max_status);
    ASSERT_INT_EQ(255, board.half_move_clock, half_move_max);
    ASSERT_INT_EQ(1000, board.full_move_number, full_move_max);
    fen_serialize_board(&board, fen_output, sizeof(fen_output));
    ASSERT_STR_EQ(max_half_move_value_fen, fen_output, max_half_move_value_fen_serialization);

    return 0;
}

int main(void) {
    if (test_fen_startpos() != 0) return 1;
    if (test_fen_midgame() != 0) return 1;
    if (test_fen_errors() != 0) return 1;
    if (test_fen_en_passant() != 0) return 1;
    if (test_fen_castling_rights() != 0) return 1;
    if (test_fen_clocks() != 0) return 1;

    printf("✅ All tests in fen_parser_test.c passed.\n");
    return 0;
}
