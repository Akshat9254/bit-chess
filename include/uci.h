#ifndef UCI_H
#define UCI_H

#include "board.h"

void uci_loop(Board *board);
void parse_position(Board *board, const char *line);
void parse_go(Board *board, const char *line);

#endif
