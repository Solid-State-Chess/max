/// \file move.h

#pragma once

#include <stdint.h>
#include "max/board/loc.h"
#include "max/def.h"


/// \ingroup board
/// @{

/// \defgroup move Moves
/// Representation of moves on the chessboard, with packed and sparse representations available
/// containing all information needed to make and unmake the move
/// @{

/// Typedef over a uint8_t representing a four-bit tag indicating that a move performs some action
/// other than simply shifting pieces - attacks, promotions, en passant, and castling.
/// \see max_pmove_t
/// \see max_smove_t
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

/// \defgroup pmove Packed Move Representation
/// Functions to operate on 2 byte packed moves.
/// Despite a seemingly small memory gain in small numbers,
/// the chess engine must maintain a list of all moves it is processing at every step of the
/// game tree, and thus small memory savings can accumulate to kilobytes of space.
/// @{

/// A move that has been packed into two bytes using 6 bit square encoding.
/// This leaves 4 bits free for move flags like capture, promotions, and castling.
///
/// \section Representation
///
///              15..12                       11..6                      5..0
///     [4 bits - max_movetag_t tag] [6 bits - max_6bit_t to] [6 bits - max_6bit_t from]
typedef uint16_t max_pmove_t;

/// \name Packed Move Bitmasks and Offsets
/// @{

/// Bitmask for the lower 6 bits of a 16 bit value
#define MAX_PMOVE_FROM_MASK (0x003F)
/// Bitmask for bits [11..6] of a 16 bit value
#define MAX_PMOVE_TO_MASK   (0x0FC0)
/// Bit position of the LSB of #MAX_PMOVE_TO_MASK, used to shift the destination square down
#define MAX_PMOVE_TO_POS    (6)
/// Bitmask for the upper four bits of a 16 bit value, used to store the move flags
#define MAX_PMOVE_TAG_MASK  (0xF000)
/// Bit position of the LSB of #MAX_PMOVE_TAG_MASK, used to shift the flags down
#define MAX_PMOVE_TAG_POS   (12)

/// @}

/// \name Packed Move Functions
/// @{

/// Get the 0x88 encoded source square from the given packed move.
MAX_INLINE_ALWAYS max_0x88_t max_pmove_from(max_pmove_t move) {
    return max_6bit_to_0x88((max_6bit_t){ .v = move & MAX_PMOVE_FROM_MASK });
}

/// Get the 0x88 encoded destination square from the given packed move.
MAX_INLINE_ALWAYS max_0x88_t max_pmove_to(max_pmove_t move) {
    return max_6bit_to_0x88((max_6bit_t){ .v = (move & MAX_PMOVE_TO_MASK) >> MAX_PMOVE_TO_POS });
}

/// Get the four bit move tag from the given packed move.
MAX_INLINE_ALWAYS max_movetag_t max_pmove_tag(max_pmove_t move) {
    return move >> MAX_PMOVE_TAG_POS;
}

/// @}

/// @}


/// \defgroup smove Sparse Move Representation
/// Move representation occupying three bytes in memory - storing source and destination squares in their
/// 0x88 representations. This representation is best for processing speed, but may cost a few kilobytes in
/// move list sizes.
/// @{

/// A move with from and to square, plus flags stored in a separate byte.
/// This structure uses three bytes to represent a move with positions already stored
/// in 0x88 form occupying one byte each - if memory is not a top priority this 
/// representation enables faster processing to avoid packing and unpacking destination squares.
typedef struct {
    /// Four bits used to represent any special attributes required to specify the moves side effects.
    max_movetag_t tag;
    /// A 0x88 board index identifying the square that was moved to
    max_0x88_t to;
    /// A 0x88 board index identifying the square that was moved from
    max_0x88_t from;
} max_smove_t;

/// @}


/// @}

/// @}
