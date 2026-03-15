#include "search.h"

#include "eval.h"
#include "move.h"
#include "movegen.h"

static const int INF = 40000;
static const int MATE_VALUE = 30000;
static const U16 MVV_LVA_CAPTURE_MOVE_OFFSET = 10000;

static const U16 MVV_LVA_SCORES[] = {
    [PT_PAWN] = 1,
    [PT_KNIGHT] = 2,
    [PT_BISHOP] = 3,
    [PT_ROOK] = 4,
    [PT_QUEEN] = 5,
    [PT_KING] = 6,
};

static int negamax(Board *board, U8 depth, int alpha, int beta);
static void sort_next_move(MoveList *move_list, size_t current_index);
static void assign_move_scores(const Board *board, Move best_move, MoveList *legal_moves);

Move search_for_best_move(Board *board, const U8 max_depth) {
    Move best_move = MOVE_NONE;

    for (U8 depth = 1; depth <= max_depth; depth++) {
        MoveList legal_moves = {0};
        generate_all_legal_moves(board, &legal_moves);

        assign_move_scores(board, best_move, &legal_moves);

        Move best_move_in_this_itr = MOVE_NONE;
        int alpha = -INF;
        const int beta = INF;

        for (size_t i = 0; i < legal_moves.count; i++) {
            sort_next_move(&legal_moves, i);

            StateInfo state = {0};
            const Move move = legal_moves.moves[i];

            make_move(board, move, &state);
            const int score = -negamax(board, depth - 1, -beta, -alpha);
            unmake_move(board, &state);

            if (score > alpha) {
                alpha = score;
                best_move_in_this_itr = move;
            }
        }

        best_move = best_move_in_this_itr;
    }

    return best_move;
}

static int negamax(Board *board, const U8 depth, int alpha, const int beta) {
    if (depth == 0) {
        return eval(board);
    }

    MoveList legal_moves = {0};
    generate_all_legal_moves(board, &legal_moves);

    if (legal_moves.count == 0) {
        if (is_king_in_check(board, board->side_to_move)) {
            return -MATE_VALUE - depth;
        }
        return 0;
    }

    int best_score = -INF;
    assign_move_scores(board, MOVE_NONE, &legal_moves);

    for (size_t i = 0; i < legal_moves.count; i++) {
        sort_next_move(&legal_moves, i);

        StateInfo state = {0};
        make_move(board, legal_moves.moves[i], &state);

        const int score = -negamax(board, depth - 1, -beta, -alpha);

        unmake_move(board, &state);

        if (score > best_score) {
            best_score = score;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            break;
        }
    }

    return best_score;
}

void sort_next_move(MoveList *move_list, const size_t current_index) {
    for (size_t i = current_index + 1; i < move_list->count; i++) {
        if (move_list->scores[i] > move_list->scores[current_index]) {
            const Move temp_move = move_list->moves[i];
            move_list->moves[i] = move_list->moves[current_index];
            move_list->moves[current_index] = temp_move;

            const U16 temp_score = move_list->scores[i];
            move_list->scores[i] = move_list->scores[current_index];
            move_list->scores[current_index] = temp_score;
        }
    }
}

void assign_move_scores(const Board *board, const Move best_move, MoveList *legal_moves) {
    for (size_t i = 0; i < legal_moves->count; i++) {
        const Move move = legal_moves->moves[i];
        const Square from = move_get_from(move);
        const Square to = move_get_to(move);
        const U8 flags = move_get_flags(move);

        if (move == best_move) {
            legal_moves->scores[i] = INF;
        } else if (is_capture(flags)) {
            const Piece attacker = board->mailbox[from];
            const Piece victim = board->mailbox[to];

            legal_moves->scores[i] = (MVV_LVA_SCORES[piece_type_of(victim)] * 10) -
                                    MVV_LVA_SCORES[piece_type_of(attacker)] +
                                    MVV_LVA_CAPTURE_MOVE_OFFSET;
        } else {
            legal_moves->scores[i] = 0;
        }
    }
}
