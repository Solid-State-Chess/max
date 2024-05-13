/// \file perft.h
#pragma once
#include "max/board/board.h"
#include <stdint.h>


/// \ingroup board
/// @{

/// Perform a perft test up to the given depth.
/// During this test, all moves for the side to play will be played, then all moves for the other side until
/// the desired depth has been reached.
/// Then, all valid position nodes arising from these moves are counted and returned, to be used for verifying
/// move generation and validation.
/// \param depth The desired perft recursion depth
uint64_t max_board_perft(max_board_t *board, max_movelist_t moves, uint8_t depth);


/// @}
