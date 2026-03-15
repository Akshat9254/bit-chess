#include "move.h"
#include "square.h"

void move_to_string(const Move move, const Color color, char *str, const size_t size) {
    const Square from = move_get_from(move);
    const Square to = move_get_to(move);
    const U8 flags = move_get_flags(move);

    const char from_file = (char) ('a' + file_of_sq(move_get_from(move)));
    const char from_rank = (char) ('1' + rank_of_sq(from));
    const char to_file = (char) ('a' + file_of_sq(to));
    const char to_rank = (char) ('1' + rank_of_sq(to));

    if (is_promo(flags)) {
        const char promotion = piece_symbol_of(move_get_promo_piece(flags, color));
        snprintf(str, size, "%c%c%c%c%c", from_file, from_rank, to_file, to_rank, promotion);
    } else {
        snprintf(str, size, "%c%c%c%c", from_file, from_rank, to_file, to_rank);
    }
}
