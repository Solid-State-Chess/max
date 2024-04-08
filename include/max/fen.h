#pragma once

#include "max/board.h"
#include <stdbool.h>

/// Attempt to parse a board position in Forsyth-Edwards Notation from the given string
/// Returns true if parsing succeeded, and false if parsing failed
bool max_board_parse_from_fen(max_board_t *board, char const *str);
