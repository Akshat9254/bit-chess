#ifndef TT_H
#define TT_H

#include <stdio.h>

#include "types.h"

#define TT_SIZE (1 << 20)

typedef enum: U8 {
    TT_EXACT = 0,
    TT_LOWER = 1,
    TT_UPPER = 2,
} TT_ScoreType;

typedef struct {
    ZobristHash hash;
    int score;
    Move move;
    U8 depth;
    TT_ScoreType score_type;
} TTEntry;

extern TTEntry tt[TT_SIZE];

static inline size_t tt_index_from_hash(const ZobristHash hash) {
    return (hash & (TT_SIZE - 1));
}

static inline  bool tt_lookup(const ZobristHash board_hash, const U8 depth, const U8 ply,
    const int alpha, const int beta, int *tt_score, Move *tt_move) {
    const size_t tt_index = tt_index_from_hash(board_hash);
    const TTEntry *entry = &tt[tt_index];

    if (entry->hash == board_hash) {
        *tt_move = entry->move;

        int best_score = entry->score;
        if (best_score > VALUE_MATE_THRESHOLD) {
            best_score -= ply;
        } else if (best_score < -VALUE_MATE_THRESHOLD) {
            best_score += ply;
        }

        *tt_score = best_score;

        if (entry->depth >= depth) {
            if (entry->score_type == TT_EXACT) {
                return true;
            }

            if (entry->score_type == TT_LOWER && best_score >= beta) {
                return true;
            }

            if (entry->score_type == TT_UPPER && best_score <= alpha) {
                return true;
            }
        }
    }

    return false;
}

static inline void tt_store(const ZobristHash board_hash, const U8 depth, const U8 ply,
    const int old_alpha, const int beta, int best_score, const Move best_move) {
    const size_t tt_index = tt_index_from_hash(board_hash);
    const TTEntry *entry = &tt[tt_index];

    if (entry->hash != board_hash || depth >= entry->depth) {
        if (best_score > VALUE_MATE_THRESHOLD) {
            best_score += ply;
        } else if (best_score < -VALUE_MATE_THRESHOLD) {
            best_score -= ply;
        }

        TTEntry new_entry = {
            .hash = board_hash,
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
}

#endif
