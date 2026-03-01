#include "castling_rights.h"

inline CastlingRights char_to_castling_rights(const char c) {
    switch (c) {
        case 'K': return CASTLE_WHITE_OO;
        case 'Q': return CASTLE_WHITE_OOO;
        case 'k': return CASTLE_BLACK_OO;
        case 'q': return CASTLE_BLACK_OOO;
        default: return CASTLE_NONE;
    }
}
