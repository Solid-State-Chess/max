/// \file move.h

#pragma once

#include <stdint.h>
#include "max/board/loc.h"


/// \ingroup board
/// @{

/// \defgroup move Moves
/// Representation of moves on the chessboard, with packed and sparse representations available
/// containing all information needed to make and unmake the move
/// @{

/// Typedef over a uint8_t representing a four-bit tag indicating that a move performs some action
/// other than simply shifting pieces - attacks, promotions, en passant, and castling.
typedef uint8_t max_movetag_t;

/// A move that has been packed into two bytes using 6 bit square encoding.
/// This leaves 4 bits free for move flags like capture, promotions, and castling.
///
/// \section Representation
///
/// Bit no.
///
///              15..12                       11..6                      5..0
///     [4 bits - max_movetag_t tag] [6 bits - max_6bit_t to] [6 bits - max_6bit_t from]
typedef uint16_t max_pmove_t;

/// @}

/// @}
