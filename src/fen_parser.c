#include <ctype.h>

#include "castling_rights.h"
#include "fen_parser.h"
#include "piece.h"

static bool skip_space(const char **p, FenError *err, const char *msg);
static bool parse_pieces(Board *board, const char **p, FenError *err);
static bool parse_side(Board *board, const char **p, FenError *err);
static bool parse_castling(Board *board, const char **p, FenError *err);
static bool parse_enpassant_square(Board *board, const char **p, FenError *err);
static bool parse_half_move_clock(Board *board, const char **p, FenError *err);
static bool parse_full_move_number(Board *board, const char **p, FenError *err);

static void set_error(FenError *fen_error, FenErrorCode error_code, const char *message);

const char *FEN_START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

bool fen_init_board(Board *board, const char *fen, FenError *fen_error) {
    assert(board);
    if (!fen || *fen == '\0') {
        set_error(fen_error, FEN_ERR_EMPTY, "Empty fen string");
        return false;
    }

    clear_board(board);
    const char *p = fen;

    if (!parse_pieces(board, &p, fen_error)) return false;
    if (!skip_space(&p, fen_error, "Missing space after pieces")) return false;

    if (!parse_side(board, &p, fen_error)) return false;
    if (!skip_space(&p, fen_error, "Missing space after side")) return false;

    if (!parse_castling(board, &p, fen_error)) return false;
    if (!skip_space(&p, fen_error, "Missing space after castling rights")) return false;

    if (!parse_enpassant_square(board, &p, fen_error)) return false;
    if (!skip_space(&p, fen_error, "Missing space after enpassant square")) return false;

    if (!parse_half_move_clock(board, &p, fen_error)) return false;
    if (!skip_space(&p, fen_error, "Missing space after half move clock")) return false;

    if (!parse_full_move_number(board, &p, fen_error)) return false;

    if (*p != '\0') {
        set_error(fen_error, FEN_ERR_INVALID_TERMINATOR, "Missing terminator after full move number");
        return false;
    }

    set_error(fen_error, FEN_OK, "Success");
    return true;
}

void fen_serialize_board(const Board *board, char *fen, const size_t fen_size) {
    assert(board);
    assert(fen);

    size_t i = 0;

    for (Rank rank = RANK_8; rank <= RANK_8; rank--) {
        U8 empty_count = 0;
        for (File file = FILE_A; file <= FILE_H; file++) {
            const Square sq = rank_file_to_sq(rank, file);
            const Piece piece = board->mailbox[sq];

            if (piece == PIECE_NONE) {
                empty_count++;
            } else {
                if (empty_count > 0) fen[i++] = (char)(empty_count + '0');
                empty_count = 0;
                fen[i++] = piece_symbol_of(piece);
            }
        }
        if (empty_count > 0) fen[i++] = (char)(empty_count + '0');
        if (rank > RANK_1) fen[i++] = '/';
    }

    fen[i++] = ' ';

    char c_str[5] = {0};
    int c_idx = 0;
    if (board->castling_rights == CASTLE_NONE) {
        c_str[c_idx] = '-';
    } else {
        if (board->castling_rights & CASTLE_WHITE_OO)  c_str[c_idx++] = 'K';
        if (board->castling_rights & CASTLE_WHITE_OOO) c_str[c_idx++] = 'Q';
        if (board->castling_rights & CASTLE_BLACK_OO)  c_str[c_idx++] = 'k';
        if (board->castling_rights & CASTLE_BLACK_OOO) c_str[c_idx] = 'q';
    }

    char ep_str[3] = "-";
    if (board->enpassant_sq != SQ_NONE) {
        ep_str[0] = (char)(file_of_sq(board->enpassant_sq) + 'a');
        ep_str[1] = (char)(rank_of_sq(board->enpassant_sq) + '1');
        ep_str[2] = '\0';
    }

    snprintf(fen + i, fen_size - i, "%c %s %s %u %u",
             board->side_to_move == COLOR_WHITE ? 'w' : 'b',
             c_str,
             ep_str,
             board->half_move_clock,
             board->full_move_number);
}

static bool skip_space(const char **p, FenError *err, const char *msg) {
    if (**p == '\0') {
        return true;
    }

    if (**p != ' ') {
        set_error(err, FEN_ERR_INVALID_SEGMENT_SEPARATOR, msg);
        return false;
    }

    (*p)++;
    return true;
}

static bool parse_pieces(Board *board, const char **p, FenError *err) {
    for (int r = RANK_8; r >= RANK_1; r--) {
        for (int f = FILE_A; f <= FILE_H; ) {
            char c = **p;
            if (c == '\0' || c == ' ') {
                set_error(err, FEN_ERR_OUT_OF_BOUNDS, "Unexpected piece placement end");
                return false;
            }

            if (isdigit(c)) {
                const int count = c - '0';
                if (count < 1 || count > 8 || (f + count) > 8) {
                    set_error(err, FEN_ERR_OUT_OF_BOUNDS, "Invalid empty square count");
                    return false;
                }
                f += count;
            } else {
                const Piece pc = piece_from_symbol(c);
                if (pc == PIECE_NONE) {
                    set_error(err, FEN_ERR_INVALID_PIECE, "Invalid piece symbol");
                    return false;
                }
                place_piece_on_sq(board, pc, rank_file_to_sq((Rank)r, (File)f));
                f++;
            }
            (*p)++;
        }
        if (r > RANK_1) {
            if (**p != '/') {
                set_error(err, FEN_ERR_INVALID_RANK_SEPARATOR, "Missing '/' between ranks");
                return false;
            }
            (*p)++;
        }
    }
    return true;
}

static bool parse_side(Board *board, const char **p, FenError *err) {
    if (**p != 'w' && **p != 'b') {
        set_error(err, FEN_ERR_INVALID_SIDE, "Missing 'w' or 'b' after piece placement");
        return false;
    }
    board->side_to_move = **p == 'w' ? COLOR_WHITE : COLOR_BLACK;
    (*p)++;
    return true;
}

static bool parse_castling(Board *board, const char **p, FenError *err) {
    board->castling_rights = CASTLE_NONE;
    if (**p == '-') {
        (*p)++;
        return true;
    }

    while (**p != ' ' && **p != '\0') {
        const CastlingRights cr = char_to_castling_rights(**p);
        if (cr == CASTLE_NONE) {
            set_error(err, FEN_ERR_INVALID_CASTLING_RIGHTS, "Invalid castling rights");
            return false;
        }
        board->castling_rights |= cr;
        (*p)++;
    }
    return true;
}

static bool parse_enpassant_square(Board *board, const char **p, FenError *err) {
    if (**p == '-') {
        board->enpassant_sq = SQ_NONE;
        (*p)++;
    } else {
        const File file = char_to_file(**p);
        if (file == FILE_NONE) {
            set_error(err, FEN_ERR_INVALID_ENPASSANT_SQUARE, "Invalid file for enpassant square");
            return false;
        }
        (*p)++;
        const Rank rank = char_to_rank(**p);
        if (rank == RANK_NONE) {
            set_error(err, FEN_ERR_INVALID_ENPASSANT_SQUARE, "Invalid rank for enpassant square");
            return false;
        }
        const Square sq = rank_file_to_sq(rank, file);
        board->enpassant_sq = sq;
        (*p)++;
    }

    return true;
}

static bool parse_half_move_clock(Board *board, const char **p, FenError *err) {
    if (**p == '\0') {
        board->half_move_clock = 0;
        return true;
    }

    if (!isdigit(**p)) {
        set_error(err, FEN_ERR_INVALID_HALF_MOVE_CLOCK, "Expected integer for half move clock");
        return false;
    }

    U8 half_move_clock = 0;
    while (isdigit(**p)) {
        half_move_clock = (half_move_clock * 10) + (**p - '0');
        (*p)++;
    }

    board->half_move_clock = half_move_clock;
    return true;
}

static bool parse_full_move_number(Board *board, const char **p, FenError *err) {
    if (**p == '\0') {
        board->full_move_number = 0;
        return true;
    }

    if (!isdigit(**p)) {
        set_error(err, FEN_ERR_INVALID_FULL_MOVE_NUMBER, "Expected integer for full move number");
        return false;
    }

    U16 full_move_number = 0;
    while (isdigit(**p)) {
        full_move_number = (full_move_number * 10) + (**p - '0');
        (*p)++;
    }

    board->full_move_number = full_move_number;
    return true;
}

static void set_error(FenError *fen_error, const FenErrorCode error_code, const char *message) {
    fen_error->code = error_code;
    fen_error->message = message;
}
