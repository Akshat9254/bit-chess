#ifndef FEN_PARSER_H
#define FEN_PARSER_H

#include "board.h"

extern const char *FEN_START_POS;

typedef enum {
    FEN_OK,
    FEN_ERR_EMPTY,
    FEN_ERR_OUT_OF_BOUNDS,
    FEN_ERR_INVALID_PIECE,
    FEN_ERR_INVALID_RANK_SEPARATOR,
    FEN_ERR_INVALID_SEGMENT_SEPARATOR,
    FEN_ERR_INVALID_SIDE,
    FEN_ERR_INVALID_CASTLING_RIGHTS,
    FEN_ERR_INVALID_ENPASSANT_SQUARE,
    FEN_ERR_INVALID_HALF_MOVE_CLOCK,
    FEN_ERR_INVALID_FULL_MOVE_NUMBER,
    FEN_ERR_INVALID_TERMINATOR
} FenErrorCode;

typedef struct {
    FenErrorCode code;
    const char *message;
} FenError;

bool fen_init_board(Board *board, const char *fen, FenError *fen_error);
void fen_serialize_board(const Board *board, char *fen, size_t fen_size);

#endif
