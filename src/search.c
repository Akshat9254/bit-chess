#include "search.h"

#include "eval.h"
#include "move.h"
#include "movegen.h"
#include "tt.h"
#include "utils.h"

SearchInfo search_info;

static const int INF = 100000;
static const int MATE_VALUE = 30000;
static const U16 MVV_LVA_CAPTURE_MOVE_OFFSET = 10000;

static const U16 MVV_LVA_SCORES[] = {
    [PT_PAWN]   = 1,
    [PT_KNIGHT] = 2,
    [PT_BISHOP] = 3,
    [PT_ROOK]   = 4,
    [PT_QUEEN]  = 5,
    [PT_KING]   = 6,
};

static int negamax(Board *board, U8 depth, int alpha, int beta);
static int quiescence(Board *board, int alpha, int beta);
static void sort_next_move(MoveList *move_list, size_t current_index);
static void assign_move_scores(const Board *board, Move best_move, MoveList *legal_moves);

Move search_for_best_move(Board *board) {
    search_info.start_time = get_time_ms();
    search_info.nodes = 0;
    search_info.aborted = false;

    Move best_move = MOVE_NONE;

    for (U8 depth = 1; depth <= MAX_DEPTH; depth++) {
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

            if (search_info.aborted) {
                break;
            }

            if (score > alpha) {
                alpha = score;
                best_move_in_this_itr = move;
            }
        }

        if (!search_info.aborted) {
            best_move = best_move_in_this_itr;
            const TTEntry new_entry = {
                .hash = board->hash,
                .score = alpha,
                .move = best_move,
                .depth = depth,
                .score_type = TT_EXACT,
            };

            tt[tt_index_from_hash(board->hash)] = new_entry;
        }

        if (get_time_ms() - search_info.start_time >= search_info.soft_limit) {
            break;
        }
    }

    return best_move;
}

static int negamax(Board *board, const U8 depth, int alpha, const int beta) {
    if ((search_info.nodes++ & 2047) == 0) {
        if (get_time_ms() - search_info.start_time >= search_info.hard_limit) {
            search_info.aborted = true;
        }
    }

    if (search_info.aborted) {
        return 0;
    }

    const size_t tt_index = tt_index_from_hash(board->hash);
    const TTEntry *entry = &tt[tt_index];

    Move hash_move = MOVE_NONE;
    if (entry->hash == board->hash) {
        hash_move = entry->move;

        if (entry->depth >= depth) {
            if (entry->score_type == TT_EXACT) {
                return entry->score;
            }

            if (entry->score_type == TT_LOWER && entry->score >= beta) {
                return entry->score;
            }

            if (entry->score_type == TT_UPPER && entry->score <= alpha) {
                return entry->score;
            }
        }
    }

    if (depth == 0) {
        return quiescence(board, alpha, beta);
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
    Move best_move = MOVE_NONE;
    assign_move_scores(board, hash_move, &legal_moves);
    const int old_alpha = alpha;

    for (size_t i = 0; i < legal_moves.count; i++) {
        sort_next_move(&legal_moves, i);

        StateInfo state = {0};
        make_move(board, legal_moves.moves[i], &state);
        const int score = -negamax(board, depth - 1, -beta, -alpha);
        unmake_move(board, &state);

        if (search_info.aborted) {
            break;
        }

        if (score > best_score) {
            best_score = score;
            best_move = legal_moves.moves[i];
        }

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            break;
        }
    }

    if (entry->hash != board->hash || depth >= entry->depth) {
        TTEntry new_entry = {
            .hash = board->hash,
            .score = best_score,
            .move = best_move,
            .depth = depth,
        };

        if (best_score >= beta) {
            new_entry.score_type = TT_LOWER;
        } else if (best_score <= old_alpha) {
            new_entry.score_type = TT_UPPER;
        } else {
            new_entry.score_type = TT_EXACT;
        }

        tt[tt_index] = new_entry;
    }

    return best_score;
}

static int quiescence(Board *board, int alpha, const int beta) {
    if ((search_info.nodes++ & 2047) == 0) {
        if (get_time_ms() - search_info.start_time >= search_info.hard_limit) {
            search_info.aborted = true;
        }
    }

    if (search_info.aborted) {
        return 0;
    }

    const int stand_pat = eval(board);
    if (stand_pat >= beta) {
        return beta;
    }

    if (stand_pat > alpha) {
        alpha = stand_pat;
    }

    MoveList capture_moves = {0};
    generate_all_legal_capture_moves(board, &capture_moves);

    assign_move_scores(board, MOVE_NONE, &capture_moves);

    for (size_t i = 0; i < capture_moves.count; i++) {
        sort_next_move(&capture_moves, i);
        StateInfo state = {0};
        make_move(board, capture_moves.moves[i], &state);

        const int score = -quiescence(board, -beta, -alpha);

        unmake_move(board, &state);

        if (search_info.aborted) {
            return 0;
        }

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
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
            legal_moves->scores[i] = (U16) INF;
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
