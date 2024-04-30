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
/// @{
typedef uint8_t max_movetag_t;

enum {
    /// Indicates that nothing happened other than a shifting of a piece from one square to another
    MAX_MOVETAG_NONE      = 0x00,
    /// Indicates that en passant capture has been performed on the current en passant file (stored elsewhere)
    MAX_MOVETAG_ENPASSANT = 0x01,
    /// Indicates that a pawn has promoted to a knight
    MAX_MOVETAG_PKNIGHT   = 0x02,
    /// Indicates that a pawn has promoted to a bishop
    MAX_MOVETAG_PBISHOP   = 0x03,
    /// Indicates that a pawn has promoted to a rook
    MAX_MOVETAG_PROOK     = 0x04,
    /// Indicates that a pawn has promoted to a queen
    MAX_MOVETAG_PQUEEN    = 0x05,
    /// A flag set in the fourth bit to indicate that a capture was made.
    /// This flag can ONLY be combined with the promotion flags to indicate that
    /// a pawn both captured a piece and promoted itself.
    MAX_MOVETAG_CAPTURE   = 0x08,
};

/// @}

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
