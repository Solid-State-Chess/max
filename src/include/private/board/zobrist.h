#pragma once
#include "max/board/zobrist.h"

extern max_zobrist_t POSITION_ELEMENTS[];

/// Initialize all static zobrist element arrays using the provided RNG seed
void max_zobrist_init_static(max_zobrist_t seed);
