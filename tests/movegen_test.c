#include "movegen.h"
#include "test_util.h"

static bool has_move(const MoveList *list, const Square from, const Square to, const U8 flag) {
    const Move target = create_move(from, to, flag);
    for (size_t i = 0; i < list->count; i++) {
        if (list->moves[i] == target) return true;
    }
    return false;
}

static  int test_pawn_move_generation() {
    Board board;
    MoveList list;

    // --- CASE 1: White Start Position (Quiet + Double Pushes) ---
    reset_board(&board);
    list.count = 0;
    generate_all_pawn_pseudo_legal_moves(&board, &list);

    ASSERT_INT_EQ(16, list.count, "White Start Total Moves");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E2, SQ_E3, MOVE_QUIET), "e2e3 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E2, SQ_E4, MOVE_PAWN_DOUBLE_PUSH), "e2e4 exists");

    // --- CASE 2: Black Start Position (Mirror Check) ---
    board.side_to_move = COLOR_BLACK;
    list.count = 0;
    generate_all_pawn_pseudo_legal_moves(&board, &list);

    ASSERT_INT_EQ(16, list.count, "Black Start Total Moves");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E7, SQ_E6, MOVE_QUIET), "e7e6 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E7, SQ_E5, MOVE_PAWN_DOUBLE_PUSH), "e7e5 exists");

    // --- CASE 3: Blocked Pushes & Regular Captures ---
    // White pawn on d4. Enemy on e5 (capture). Friend on d5 (block).
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_PAWN, SQ_D4);
    place_piece_on_sq(&board, BLACK_PAWN, SQ_E5); // Enemy
    place_piece_on_sq(&board, WHITE_PAWN, SQ_D5); // Blocker

    list.count = 0;
    generate_all_pawn_pseudo_legal_moves(&board, &list);

    // Move 1: D4 captures E5 (Capture)
    // Move 2: D5 moves to D6 (Quiet)
    // Note: D4 moving to D5 is IMPOSSIBLE because D5 is occupied.
    ASSERT_INT_EQ(2, list.count, "Two white pawns on board: one capture, one push");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_E5, MOVE_CAPTURE), "d4xe5 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D5, SQ_D6, MOVE_QUIET), "d5d6 exists");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_D4, SQ_D5, MOVE_QUIET), "d4d5 is blocked by friend");

    // --- CASE 4: En Passant ---
    // White pawn on e5. Black pawn just did d7-d5. EP square is d6.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    board.enpassant_sq = SQ_D6;
    place_piece_on_sq(&board, WHITE_PAWN, SQ_E5);
    place_piece_on_sq(&board, WHITE_PAWN, SQ_C5);

    list.count = 0;
    generate_all_pawn_pseudo_legal_moves(&board, &list);

    // Expected Moves (4 Total):
    // 1. e5-e6 (Quiet)
    // 2. c5-c6 (Quiet)
    // 3. e5xd6 (En Passant)
    // 4. c5xd6 (En Passant)
    ASSERT_INT_EQ(4, list.count, "Two pawns attacking one EP square + two quiet pushes");

    // Verify Quiet Pushes
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E5, SQ_E6, MOVE_QUIET), "e5-e6 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_C5, SQ_C6, MOVE_QUIET), "c5-c6 exists");

    // Verify En Passant Captures
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E5, SQ_D6, MOVE_ENPASSANT), "e5xd6 (EP) exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_C5, SQ_D6, MOVE_ENPASSANT), "c5xd6 (EP) exists");

    // --- CASE 5: Promotions (Quiet & Capture) ---
    // White pawn on b7. Enemy on a8.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_PAWN, SQ_B7);
    place_piece_on_sq(&board, BLACK_ROOK, SQ_A8);

    list.count = 0;
    generate_all_pawn_pseudo_legal_moves(&board, &list);

    // 4 quiet promos (b7b8) + 4 capture promos (b7xa8) = 8 moves
    ASSERT_INT_EQ(8, list.count, "White Promotion Count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_B7, SQ_B8, MOVE_PROMO_Q), "b7-b8=Q");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_B7, SQ_A8, MOVE_PROMO_N_CAP), "b7xa8=N");

    // --- CASE 6: Black File-Wrap Check ---
    // Black pawn on a2. Should not capture "left" onto h1 (index 7).
    clear_board(&board);
    board.side_to_move = COLOR_BLACK;
    place_piece_on_sq(&board, BLACK_PAWN, SQ_A2);
    place_piece_on_sq(&board, WHITE_PAWN, SQ_H1); // Trap piece

    list.count = 0;
    generate_all_pawn_pseudo_legal_moves(&board, &list);

    // Should only have 4 quiet promos (a2a1). Capture on h1 is illegal wrap.
    ASSERT_INT_EQ(4, list.count, "Black File Wrap Check");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_A2, SQ_H1, MOVE_PROMO_Q_CAP), "No A to H wrap");

    return 0;
}

static  int test_knight_move_generation() {
    Board board;
    MoveList list;

    // --- CASE 1: Knight in the Center (e4) ---
    // A knight on e4 should have 8 quiet moves if the board is empty.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_E4);

    list.count = 0;
    generate_all_knight_legal_moves(&board, &list);

    ASSERT_INT_EQ(8, list.count, "Knight at e4 move count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E4, SQ_F6, MOVE_QUIET), "e4-f6 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E4, SQ_D2, MOVE_QUIET), "e4-d2 exists");

    // --- CASE 2: Knight in the Corner (a1) ---
    // A knight on a1 only has 2 possible moves: b3 and c2.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_A1);

    list.count = 0;
    generate_all_knight_legal_moves(&board, &list);

    ASSERT_INT_EQ(2, list.count, "Knight at a1 move count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_A1, SQ_B3, MOVE_QUIET), "a1-b3 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_A1, SQ_C2, MOVE_QUIET), "a1-c2 exists");

    // --- CASE 3: Friendly Blockage and Captures ---
    // White Knight on d4.
    // Friend on f5 (block). Enemy on e6 (capture).
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_D4);
    place_piece_on_sq(&board, WHITE_PAWN, SQ_F5);   // Blocker
    place_piece_on_sq(&board, BLACK_PAWN, SQ_E6);   // Target

    list.count = 0;
    generate_all_knight_legal_moves(&board, &list);

    // Total should be 7 (8 possible - 1 blocked + 0 for the pawn itself since we only call knight gen)
    ASSERT_INT_EQ(7, list.count, "Knight blockage test count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_E6, MOVE_CAPTURE), "d4xe6 capture exists");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_D4, SQ_F5, MOVE_QUIET), "d4-f5 blocked by friend");

    // --- CASE 4: Multiple Knights ---
    // White Knights on b1 and g1 (Starting position).
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_B1);
    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_G1);

    list.count = 0;
    generate_all_knight_legal_moves(&board, &list);

    // Each knight has 3 moves from the back rank (total 6)
    ASSERT_INT_EQ(6, list.count, "Two knights start rank count");

    return 0;
}

static int test_bishop_move_generation() {
    Board board;
    MoveList list;

    // --- CASE 1: Bishop in the Center (Empty Board) ---
    // A bishop on d4 has 13 moves on an empty board.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_BISHOP, SQ_D4);

    list.count = 0;
    generate_all_bishop_legal_moves(&board, &list);

    ASSERT_INT_EQ(13, list.count, "Bishop at d4 empty board count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_A1, MOVE_QUIET), "d4-a1 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_G7, MOVE_QUIET), "d4-g7 exists");

    // --- CASE 2: Blocked by Friends and Capturing Enemies ---
    // White Bishop on d4.
    // Friend on f6 (blocks f6, g7, h8).
    // Enemy on b6 (captures b6, blocks a7).
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_BISHOP, SQ_D4);
    place_piece_on_sq(&board, WHITE_PAWN, SQ_F6);   // Friendly Blocker
    place_piece_on_sq(&board, BLACK_PAWN, SQ_B6);   // Enemy Target

    list.count = 0;
    generate_all_bishop_legal_moves(&board, &list);

    // NW: c5 (Quiet), b6 (Capture) -> 2 moves
    // NE: e5 (Quiet) -> 1 move
    // SW: c3, b2, a1 -> 3 moves
    // SE: e3, f2, g1 -> 3 moves
    // Total = 9
    ASSERT_INT_EQ(9, list.count, "Bishop blockage/capture test count corrected");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_C5, MOVE_QUIET), "d4-c5 quiet exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_B6, MOVE_CAPTURE), "d4xb6 capture exists");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_D4, SQ_F6, MOVE_QUIET), "d4-f6 blocked by friend");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_D4, SQ_A7, MOVE_QUIET), "d4-a7 blocked by enemy on b6");

    // --- CASE 3: Corner Bishop ---
    // Bishop on a1 only has one diagonal (7 squares).
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_BISHOP, SQ_A1);

    list.count = 0;
    generate_all_bishop_legal_moves(&board, &list);

    ASSERT_INT_EQ(7, list.count, "Bishop at a1 move count");

    return 0;
}

static int test_rook_move_generation() {
    Board board;
    MoveList list;

    // --- CASE 1: Rook in the Center (Empty Board) ---
    // A Rook on d4 always has 14 moves on an empty board (7 on the file, 7 on the rank).
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_ROOK, SQ_D4);

    list.count = 0;
    generate_all_rook_legal_moves(&board, &list);

    ASSERT_INT_EQ(14, list.count, "Rook at d4 empty board count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_D1, MOVE_QUIET), "d4-d1 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_D8, MOVE_QUIET), "d4-d8 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_A4, MOVE_QUIET), "d4-a4 exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_H4, MOVE_QUIET), "d4-h4 exists");

    // --- CASE 2: Blocked Rank and Open File ---
    // White Rook on d4.
    // Friendly blocker on b4 (blocks a4, b4).
    // Enemy target on f4 (captures f4, blocks g4, h4).
    // File (D) remains fully open.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_ROOK, SQ_D4);
    place_piece_on_sq(&board, WHITE_PAWN, SQ_B4);   // Friendly Blocker (West)
    place_piece_on_sq(&board, BLACK_PAWN, SQ_F4);   // Enemy Target (East)

    list.count = 0;
    generate_all_rook_legal_moves(&board, &list);

    // Vertical (File D): 7 moves (open)
    // Horizontal West: c4 (Quiet) -> 1 move
    // Horizontal East: e4 (Quiet), f4 (Capture) -> 2 moves
    // Total = 7 + 1 + 2 = 10 moves.
    ASSERT_INT_EQ(10, list.count, "Rook mixed blockage test count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_C4, MOVE_QUIET), "d4-c4 quiet exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_F4, MOVE_CAPTURE), "d4xf4 capture exists");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_D4, SQ_B4, MOVE_QUIET), "d4-b4 blocked by friend");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_D4, SQ_G4, MOVE_QUIET), "d4-g4 blocked by enemy on f4");

    // --- CASE 3: Corner Rook ---
    // Rook on a1 has 14 moves (7 on Rank 1, 7 on File A).
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_ROOK, SQ_A1);

    list.count = 0;
    generate_all_rook_legal_moves(&board, &list);

    ASSERT_INT_EQ(14, list.count, "Rook at a1 move count");

    return 0;
}

static int test_queen_move_generation() {
    Board board;
    MoveList list;

    // --- CASE 1: Queen in the Center (Empty Board) ---
    // A Queen on d4 has 13 (Bishop moves) + 14 (Rook moves) = 27 moves.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_QUEEN, SQ_D4);

    list.count = 0;
    generate_all_queen_legal_moves(&board, &list);

    ASSERT_INT_EQ(27, list.count, "Queen at d4 empty board count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_D8, MOVE_QUIET), "d4-d8 (Rook-like) exists");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_A1, MOVE_QUIET), "d4-a1 (Bishop-like) exists");

    // --- CASE 2: Multi-directional Blockage ---
    // White Queen on d4.
    // Friendly blocker on d6 (North): blocks d6, d7, d8.
    // Enemy target on f6 (Northeast): captures f6, blocks g7, h8.
    // All other 6 directions are open.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_QUEEN, SQ_D4);
    place_piece_on_sq(&board, WHITE_PAWN, SQ_D6);   // Friendly Blocker (North)
    place_piece_on_sq(&board, BLACK_PAWN, SQ_F6);   // Enemy Target (Northeast)

    list.count = 0;
    generate_all_queen_legal_moves(&board, &list);

    // Vertical North: d5 (Quiet) -> 1 move
    // Northeast: e5 (Quiet), f6 (Capture) -> 2 moves
    // Everything else is open (27 total - 3 blocked in North - 2 blocked in NE)
    // 27 - 3 - 2 = 22 moves.
    ASSERT_INT_EQ(22, list.count, "Queen mixed blockage test count");
    ASSERT_BOOL_EQ(true, has_move(&list, SQ_D4, SQ_F6, MOVE_CAPTURE), "d4xf6 capture exists");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_D4, SQ_D6, MOVE_QUIET), "d4-d6 blocked by friend");
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_D4, SQ_G7, MOVE_QUIET), "d4-g7 blocked by enemy on f6");

    // --- CASE 3: Queen in the Corner (a1) ---
    // Rook-like: 14 moves. Bishop-like: 7 moves. Total = 21 moves.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    place_piece_on_sq(&board, WHITE_QUEEN, SQ_A1);

    list.count = 0;
    generate_all_queen_legal_moves(&board, &list);

    ASSERT_INT_EQ(21, list.count, "Queen at a1 move count");

    return 0;
}

static int test_king_move_generation() {
    Board board;
    MoveList list;

    // --- CASE 1: King in Center (Empty) ---
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    board.castling_rights = CASTLE_NONE;
    place_piece_on_sq(&board, WHITE_KING, SQ_E4);
    list.count = 0;
    generate_all_king_legal_moves(&board, &list);
    ASSERT_INT_EQ(8, list.count, "King at e4 count");

    // --- CASE 2: Castling Rights + Blocked Path ---
    // White has rights, but a Bishop is on f1.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    board.castling_rights = CASTLE_WHITE_OO;
    place_piece_on_sq(&board, WHITE_KING, SQ_E1);
    place_piece_on_sq(&board, WHITE_BISHOP, SQ_F1);
    list.count = 0;
    generate_all_king_legal_moves(&board, &list);

    // Should have normal moves (d1, d2, e2, f2) but NOT castling
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_E1, SQ_G1, MOVE_KING_CASTLE), "No castle through pieces");

    // --- CASE 3: Castling into Check ---
    // Enemy Rook attacks f1.
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    board.castling_rights = CASTLE_WHITE_OO;
    place_piece_on_sq(&board, WHITE_KING, SQ_E1);
    place_piece_on_sq(&board, BLACK_ROOK, SQ_F8); // Attacks f1
    list.count = 0;
    generate_all_king_legal_moves(&board, &list);

    ASSERT_BOOL_EQ(false, has_move(&list, SQ_E1, SQ_G1, MOVE_KING_CASTLE), "No castle through check");

    // --- CASE 4: Queenside B1 Attack ---
    // Rule: King can castle even if B1 is attacked (only D1 and C1 matter).
    clear_board(&board);
    board.side_to_move = COLOR_WHITE;
    board.castling_rights = CASTLE_WHITE_OOO;
    place_piece_on_sq(&board, WHITE_KING, SQ_E1);
    place_piece_on_sq(&board, BLACK_BISHOP, SQ_G6); // Attacks B1 via diagonal

    list.count = 0;
    generate_all_king_legal_moves(&board, &list);

    ASSERT_BOOL_EQ(true, has_move(&list, SQ_E1, SQ_C1, MOVE_QUEEN_CASTLE), "Castle allowed if B1 attacked");

    // Ensure the landing square is C1, not A1.
    ASSERT_BOOL_EQ(false, has_move(&list, SQ_E1, SQ_A1, MOVE_QUEEN_CASTLE), "King should not land on Rook square");

    return 0;
}

int main(void) {
    if (test_pawn_move_generation() != 0) return 1;
    if (test_knight_move_generation() != 0) return 1;
    if (test_bishop_move_generation() != 0) return 1;
    if (test_rook_move_generation() != 0) return 1;
    if (test_queen_move_generation() != 0) return 1;
    if (test_king_move_generation() != 0) return 1;

    printf("✅ All tests in movegen_test.c passed.\n");
    return 0;
}
