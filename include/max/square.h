#pragma once

#include "max/def.h"
#include <stdbool.h>
#include <stdint.h>

/// A 0x88 board position that indexes a board array
typedef uint8_t max_bpos_t;

#define MAX_BPOS_FORMAT(pos) (max_bpos_file(pos) + 'a'), (max_bpos_rank(pos) + '1')

/// Create a new board position index from the given rank and file in the range 0..7
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
    /// Bitmask used to check if a given board position is invalid
    MAX_BPOS_INVALID_MASK = 0x88,
};

/// Check if the given position is valid within the 8x8 board
MAX_INLINE_ALWAYS bool max_bpos_valid(max_bpos_t pos) { return (pos & MAX_BPOS_INVALID_MASK) == 0; }

/// Get the rank between 0 and 7 of the given position
MAX_INLINE_ALWAYS uint8_t max_bpos_rank(max_bpos_t pos) { return pos >> 4; }

/// Get the file between 0 and 7 of the given position
MAX_INLINE_ALWAYS uint8_t max_bpos_file(max_bpos_t pos) { return pos & MAX_FILE_MASK; }

/// Type representing changes in a `max_bpos_t`
typedef int8_t max_increment_t;

/// Get a difference index between 0 and 238 to index lookup tables by square difference
MAX_INLINE_ALWAYS max_bpos_t max_bpos_diff(max_bpos_t from, max_bpos_t to) {
    return (0x77 + from) - to;
}

/// Apply the given increment to the given position
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


#define MAX_KNIGHT_MOVES_LEN (8)

/// All knight increments, for iterating during movegen / attack checking
extern const max_increment_t MAX_KNIGHT_MOVES[MAX_KNIGHT_MOVES_LEN];

#define MAX_KING_MOVES_LEN (8)

/// Increments for all king moves
extern const max_increment_t MAX_KING_MOVES[MAX_KING_MOVES_LEN];

/// All pawn attack directions, used for movegen and checking
extern const max_increment_t MAX_PAWNSIDES[2];

/// Increment for a pawn advancement indexed by turn
extern const max_increment_t MAX_PAWN_DIR[2];

/// Array indexed by board position differences giving ray direction (if any) from a square to another square
extern max_increment_t MAX_DIRECTION_BY_DIFF[240];

/// Information for a detected slide from a square along an increment
typedef struct {
    /// Square that the ray originates from
    max_bpos_t origin;
    /// Direction that the ray travels outward from the origin square
    max_increment_t line;
} max_line_t;

/// Data for a piece delivering check - includes state required to track if the check is delivered
/// by a sliding piece
typedef struct {
    union {
        /// Pawn, king, or knight attacker location
        max_bpos_t jump;
        /// Bishop, rook, or queen attacker location and ray direction
        max_line_t ray;
    } attack;
} max_checker_t;

/// Get an invalid checker structure representing the absence of detected check
MAX_INLINE_ALWAYS
max_checker_t max_checker_invalid(void) {
    return (max_checker_t){
        .attack.ray = (max_line_t){
            .origin = MAX_BPOS_INVALID_MASK,
            .line = 0
        }
    };
}

/// Return true if the given checker structure represents check delivered by a sliding piece
MAX_INLINE_ALWAYS
bool max_checker_is_sliding(max_checker_t checker) { return checker.attack.ray.line != 0; }

/// Return true if the given checker structure represents check delivered by a jumping piece
MAX_INLINE_ALWAYS
bool max_checker_is_jumping(max_checker_t checker) { return !max_checker_is_sliding(checker); }

/// Check if the given checker structure represents a valid checking piece
MAX_INLINE_ALWAYS
bool max_checker_is_valid(max_checker_t checker) { return max_bpos_valid(checker.attack.jump); }

/// Detected information for single and double check
typedef struct {
    max_checker_t attacks[2];
} max_check_t;

/// Return true if the given checks structure contains two valid pieces delivering check
MAX_INLINE_ALWAYS
bool max_check_is_double(max_check_t check) {
    return max_checker_is_valid(check.attacks[0]) && max_checker_is_valid(check.attacks[1]);
}

/// Return true if either one or two pieces are delivering check to the side to play
MAX_INLINE_ALWAYS
bool max_check_exists(max_check_t check) {
    return max_checker_is_valid(check.attacks[0]);
}

/// Reset the given detected checks structure, invalidating all attacks
MAX_INLINE_ALWAYS
void max_check_reset(max_check_t *check) {
    check->attacks[0] = max_checker_invalid();
    check->attacks[1] = max_checker_invalid();
}
