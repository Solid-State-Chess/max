#pragma once

#include "max/def.h"
#include <stdbool.h>
#include <stdint.h>

/// Position on a 0x88 board
typedef union {
    struct {
        uint8_t file : 4;
        uint8_t rank : 4;
    } parts;

    uint8_t bits;
} max_bidx_t;


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
    MAX_RANK_1    = 0x0,
    MAX_RANK_2    = 0x1,
    MAX_RANK_3    = 0x2,
    MAX_RANK_4    = 0x3,
    MAX_RANK_5    = 0x4,
    MAX_RANK_6    = 0x5,
    MAX_RANK_7    = 0x6,
    MAX_RANK_8    = 0x7,
};

/// Check if the given position is valid or out of bounds
MAX_INLINE_ALWAYS bool max_bidx_valid(max_bidx_t pos) { return (pos.bits & 0x88) == 0; }

/// Type representing board position increments
typedef int8_t max_increment_t;

/// Apply the given increment to the given position
MAX_INLINE_ALWAYS max_bidx_t max_bidx_inc(max_bidx_t pos, max_increment_t inc) {
    return (max_bidx_t){.bits = pos.bits + inc};
}

enum {
    MAX_INCREMENT_UP    = (max_increment_t)0x10,
    MAX_INCREMENT_DOWN  = (max_increment_t)-0x10,
    MAX_INCREMENT_RIGHT = (max_increment_t)0x01,
    MAX_INCREMENT_LEFT  = (max_increment_t)-0x01,
};

#undef FILE
