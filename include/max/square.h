#pragma once

#include "max/def.h"
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t max_bpos_t;

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

enum {
    MAX_FILE_MASK = 0x0F,
    MAX_FILE_A    = 0x0,
    MAX_FILE_B    = 0x1,
    MAX_FILE_C    = 0x2,
    MAX_FILE_D    = 0x3,
    MAX_FILE_E    = 0x4,
    MAX_FILE_F    = 0x5,
    MAX_FILE_G    = 0x6,
    MAX_FILE_H    = 0x7,
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

/// Check if the given position is valid or out of bounds
MAX_INLINE_ALWAYS bool max_bpos_valid(max_bpos_t pos) { return (pos & 0x88) == 0; }

/// Get a difference index between 0 and 238 to index lookup tables by square difference
MAX_INLINE_ALWAYS max_bpos_t max_bpos_diff(max_bpos_t from, max_bpos_t to) {
    return 0x77 + from - to;
}

/// Type representing board position increments
typedef int8_t max_increment_t;

/// Apply the given increment to the given position
MAX_INLINE_ALWAYS max_bpos_t max_bpos_inc(max_bpos_t pos, max_increment_t inc) {
    return pos + inc;
}

enum {
    MAX_INCREMENT_UP    = (max_increment_t)0x10,
    MAX_INCREMENT_DOWN  = (max_increment_t)-0x10,
    MAX_INCREMENT_RIGHT = (max_increment_t)0x01,
    MAX_INCREMENT_LEFT  = (max_increment_t)-0x01,

    MAX_INCREMENT_UR    = MAX_INCREMENT_UP + MAX_INCREMENT_RIGHT,
    MAX_INCREMENT_UL    = MAX_INCREMENT_UP + MAX_INCREMENT_LEFT,
    MAX_INCREMENT_DR    = MAX_INCREMENT_DOWN + MAX_INCREMENT_RIGHT,
    MAX_INCREMENT_DL    = MAX_INCREMENT_DOWN + MAX_INCREMENT_LEFT,
};


/// Direction between two squares by 0x88 difference, used for check and pin calculations
extern max_increment_t MAX_DIRECTION_BY_DIFF[140];

#undef FILE
