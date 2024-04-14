#pragma once
/// \mainpage Max Embedded Chess Engine
///
/// A chess engine meant for embedded processors which cannot easily
/// take advantage of 64-bit bitboards for faster move generation or use
/// megabytes large transposition tables.
///
/// \section getting-started Getting Started
/// \see max_engine_new()


#include "max/engine/engine.h"

/// Initialize all static tables and etc.
void max_init_statics(void);
