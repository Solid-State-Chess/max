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
    /// Indicates a pawn double move from the homerow that enables en passant capture
    MAX_MOVETAG_DOUBLE    = 0x06,
    /// Indicates a castle move, kingside or queenside is determined by the destination square
    MAX_MOVETAG_CASTLE    = 0x07,
    /// A flag set in the fourth bit to indicate that a capture was made.
    /// This flag can ONLY be combined with the promotion flags to indicate that
    /// a pawn both captured a piece and promoted itself.
    MAX_MOVETAG_CAPTURE   = 0x08,
};

/// Check if the given move tag represents a promotion to any piece type.
MAX_INLINE_ALWAYS bool max_movetag_is_promote(max_movetag_t tag) {
    return tag > MAX_MOVETAG_ENPASSANT && tag <= MAX_MOVETAG_PQUEEN;
}

/// Side that castling can be performed, either 0 or 1.
/// ```
/// 0 - A side castling
/// 1 - H side castling
/// ```
typedef uint8_t max_castle_side_t;

enum {
    MAX_CASTLE_ASIDE = 0,
    MAX_CASTLE_HSIDE = 1,
};

/// Length of arrays indexed by a castle side bit
#define MAX_CASTLES_LEN (2)

/// @}


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

/// Create a new sparse move from the given source and destination square, and metadata
MAX_INLINE_ALWAYS max_smove_t max_smove_new(max_0x88_t from, max_0x88_t to, max_movetag_t tag) {
    return (max_smove_t){
        .tag = tag,
        .to = to,
        .from = from,
    };
}

/// Create a new #max_smove_t representing a capture made by a piece from from the given source square
/// to an enemy on the given destination square
MAX_INLINE_ALWAYS max_smove_t max_smove_capture(max_0x88_t from, max_0x88_t to) {
    return max_smove_new(from, to, MAX_MOVETAG_CAPTURE);
}

/// Create a new #max_smove_t representing a 'quiet' move from the given square to the given square.
MAX_INLINE_ALWAYS max_smove_t max_smove_normal(max_0x88_t from, max_0x88_t to) {
    return max_smove_new(from, to, MAX_MOVETAG_NONE);
}

/// @}

/// A list of (optionally packed) moves filled during movegen and searched
/// by the engine. Because the engine must perform move ordering to maximize
/// the number of pruned nodes during a search, the full list of every possible move
/// at every depth level must be recorded - potentially representing a significant amount 
/// of memory usage.
typedef struct {
    /// User-provided buffer to write moves to
    /// @{

#ifdef MAX_PACKED_MOVE
    max_pmove_t *buf;
#else
    max_smove_t *buf;
#endif

    /// @}

    /// User-specified capacity of the buffer, used by debug assertions
    /// to ensure we don't overrun the array
    uint16_t capacity;
    /// Number of moves stored in this list
    uint16_t len;
} max_movelist_t;


/// Create a new list of moves using the provided buffer of the given capacity
/// to store moves in.
/// \param list [out] List to initialize with the passed buffer
/// \param buf [in] Buffer to utilize as storage for moves, must have the capacity passed to this function
/// \param capacity Capacity of the provided buffer in number of elements
MAX_INLINE_ALWAYS void max_movelist_new(max_movelist_t *list, max_smove_t *buf, uint16_t capacity) {
    list->buf = buf;
    list->capacity = capacity;
    list->len = 0;
}

/// Clear the given move list, resetting its length to zero without modifying any other state.
MAX_INLINE_ALWAYS void max_movelist_clear(max_movelist_t *list) {
    list->len = 0;
}

/// Get a new 'slice' of the given movelist representing a new, empty list with buffer positioned at the end
/// of filled elements of the given buffer.
MAX_INLINE_ALWAYS max_movelist_t max_movelist_slice(max_movelist_t *list) {
    return (max_movelist_t){
        .buf = list->buf + list->len,
        .capacity = list->capacity - list->len,
        .len = 0,
    };
}

/// @}


/// @}
