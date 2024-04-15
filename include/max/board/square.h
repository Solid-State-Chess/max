#pragma once
/// \file
/// Definitions for types representing positions and directions on a 0x88 chessboard.
///
/// \see #max_bpos_t
/// \see #max_increment_t
/// \see #max_line_t


#include "max/def.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/// \defgroup bpos 0x88 Board Position
/// Functions and structures that map rank and file coordinates to an index in an array.
/// @{

/// Rank and file coordinates packed into a single byte in Least Significant File format.
/// [4 Bit Rank] [4 Bit File]
/// This method of packing coordinates allows invalid board positions to be expressed - 
/// if the most significant bit of either nibble is set, then the board position is not valid (see #MAX_BPOS_INVALID_MASK).
///
/// \see max_bpos_new()
/// \see #MAX_FILE_MASK
/// \see #MAX_RANK_MASK
typedef uint8_t max_bpos_t;

/// Utility macro to provide printf style format arguments for a #max_bpos_t.
/// Note that the format string must contain "%c%c" to format rank and file correctly
#define MAX_BPOS_FORMAT(pos) (max_bpos_file(pos) + 'a'), (max_bpos_rank(pos) + '1')

/// Create a new board position index from the given rank and file in the range 0..7.
MAX_INLINE_ALWAYS max_bpos_t max_bpos_new(uint8_t file, uint8_t rank) {
    return (rank << 4) + file;
}

#define FILE(letter, lsn)         \
    MAX_##letter##1 = 0x0##lsn,   \
    MAX_##letter##2 = 0x1##lsn,   \
    MAX_##letter##3 = 0x2##lsn,   \
    MAX_##letter##4 = 0x3##lsn,   \
    MAX_##letter##5 = 0x4##lsn,   \
    MAX_##letter##6 = 0x5##lsn,   \
    MAX_##letter##7 = 0x6##lsn,   \
    MAX_##letter##8 = 0x7##lsn

enum {
    FILE(A, 0),
    FILE(B, 1),
    FILE(C, 2),
    FILE(D, 3),
    FILE(E, 4),
    FILE(F, 5),
    FILE(G, 6),
    FILE(H, 7),
};

#undef FILE

enum {
    /// Bitmask for the lower nibble of a #max_bpos_t
    MAX_FILE_MASK = 0x0F,
    MAX_FILE_A    = 0x00,
    MAX_FILE_B    = 0x01,
    MAX_FILE_C    = 0x02,
    MAX_FILE_D    = 0x03,
    MAX_FILE_E    = 0x04,
    MAX_FILE_F    = 0x05,
    MAX_FILE_G    = 0x06,
    MAX_FILE_H    = 0x07,
};

enum {
    /// Bitmask for the upper nibble of a #max_bpos_t
    MAX_RANK_MASK = 0xF0,
    MAX_RANK_1    = 0x00,
    MAX_RANK_2    = 0x10,
    MAX_RANK_3    = 0x20,
    MAX_RANK_4    = 0x30,
    MAX_RANK_5    = 0x40,
    MAX_RANK_6    = 0x50,
    MAX_RANK_7    = 0x60,
    MAX_RANK_8    = 0x70,
};

enum {
    /// Bitmask used to check if a given #max_bpos_t is invalid with an AND operation.
    /// \see max_bpos_valid()
    MAX_BPOS_INVALID_MASK = 0x88,
};

/// Check if the given position is valid on the board
MAX_INLINE_ALWAYS bool max_bpos_valid(max_bpos_t pos) { return (pos & MAX_BPOS_INVALID_MASK) == 0; }

/// Compress the given 0x88 board index into a [0..63] range that more compactly represents all
/// valid chessboard squares. Used to compress lookup tables from 128 to 64 elements at the cost of
/// a few more operations required to get the lookup table index.
MAX_INLINE_ALWAYS uint8_t max_bpos_compress(max_bpos_t pos) { return (pos + (pos & 7)) >> 1; }

/// Get the rank between 0 and 7 of the given position
MAX_INLINE_ALWAYS uint8_t max_bpos_rank(max_bpos_t pos) { return pos >> 4; }

/// Get the file between 0 and 7 of the given position
MAX_INLINE_ALWAYS uint8_t max_bpos_file(max_bpos_t pos) { return pos & MAX_FILE_MASK; }

/// Type representing a change in a `max_bpos_t` - common constants for the cardinal directions are defined.
///
/// \see #MAX_CARDINALS
/// \see #MAX_DIAGONALS
typedef int8_t max_increment_t;

/// Get a unique index given by the difference between two positions.
/// 0x88 coordinate differences have the useful property of being unique with respect to 
/// distance and direction - so the value from this function in the range [0..240) can be used in lookup tables
/// to find values by direction.
///
/// \see #MAX_DIRECTION_BY_DIFF
MAX_INLINE_ALWAYS max_bpos_t max_bpos_diff(max_bpos_t from, max_bpos_t to) {
    return (0x77 + from) - to;
}

/// Apply the given direction to the board position
MAX_INLINE_ALWAYS max_bpos_t max_bpos_inc(max_bpos_t pos, max_increment_t inc) {
    return (max_bpos_t)((max_increment_t)(pos) + inc);
}

enum {
    MAX_INCREMENT_UP    = (max_increment_t) 0x10,
    MAX_INCREMENT_DOWN  = (max_increment_t)-0x10,
    MAX_INCREMENT_RIGHT = (max_increment_t) 0x01,
    MAX_INCREMENT_LEFT  = (max_increment_t)-0x01,

    MAX_INCREMENT_UR    = MAX_INCREMENT_UP   + MAX_INCREMENT_RIGHT,
    MAX_INCREMENT_UL    = MAX_INCREMENT_UP   + MAX_INCREMENT_LEFT,
    MAX_INCREMENT_DR    = MAX_INCREMENT_DOWN + MAX_INCREMENT_RIGHT,
    MAX_INCREMENT_DL    = MAX_INCREMENT_DOWN + MAX_INCREMENT_LEFT,
};

/// An array of directions for all four diagonal vectors
extern const max_increment_t MAX_DIAGONALS[4];
/// An array of directions for all four cardinal vectors
extern const max_increment_t MAX_CARDINALS[4];

#define MAX_ABSINTRIN

/// Get the absolute value of the given increment, utilizing a bitwise implementation if #MAX_ABSINTRIN is defined
MAX_INLINE_ALWAYS max_increment_t max_increment_abs(max_increment_t i) {
    #ifdef MAX_ABSINTRIN
    return abs(i);
    #else
    max_increment_t const mask = i >> (sizeof(max_increment_t) * CHAR_BIT - 1);
    return (i + mask) ^ mask;
    #endif
}

/// Check if the given increment is diagonal
MAX_INLINE_ALWAYS bool max_increment_is_diagonal(max_increment_t inc) {
    inc = max_increment_abs(inc);

    return inc == 15 || inc == 17;
}

/// Check if the given nonzero increment points in a cardinal direction.
MAX_INLINE_ALWAYS max_increment_t max_increment_is_cardinal(max_increment_t inc) { return !max_increment_is_diagonal(inc); }

/// Length of the static #MAX_KNIGHT_MOVES array
/// \see #MAX_KNIGHT_MOVES
#define MAX_KNIGHT_MOVES_LEN (8)

/// All knight increments, for iterating during movegen / attack checking
extern const max_increment_t MAX_KNIGHT_MOVES[MAX_KNIGHT_MOVES_LEN];

/// Length of the static #MAX_KING_MOVES array
#define MAX_KING_MOVES_LEN (8)

/// Increments for all king moves
extern const max_increment_t MAX_KING_MOVES[MAX_KING_MOVES_LEN];

/// All pawn attack directions, used for movegen and checking
extern const max_increment_t MAX_PAWNSIDES[2];

/// Increment for a pawn advancement indexed by turn
extern const max_increment_t MAX_PAWN_DIR[2];

/// Bitmask for board positions of the promotion rank, indexed by the turn bit
extern const max_bpos_t PAWN_PROMOTERANK[2];

//Homerow lookup table, indexed by the current side to move
extern const max_bpos_t PAWN_HOMEROW[2];

/// Lookup table for rank bitmasks matching the rank that a pawn can be en passanted, indexed by the side to move
extern const max_bpos_t PAWN_EPRANK[2];

/// Array indexed by board position differences giving ray direction (if any) from a square to another square
extern max_increment_t MAX_DIRECTION_BY_DIFF[240];

/// Information for a ray originating from a board position and
/// travelling along a direction.
///
/// \section warning Warnings
/// The \link #max_line_t::origin origin \endlink MUST be the first member of this struct,
/// as #max_checker_t depends on this arrangement to store either a jumping attack OR a sliding
/// ray attack without the need for an additional tag. See #max_checker_t for more on this requirement.
///
/// \see #max_checker_t
typedef struct {
    /// Square that the ray originates from
    max_bpos_t origin;
    /// Direction that the ray travels outward from the origin square
    max_increment_t line;
} max_line_t;


/// @}
