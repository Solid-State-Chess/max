#pragma once
/// \mainpage Max Embedded Chess Engine
///
/// A chess engine meant for embedded processors which cannot easily
/// take advantage of 64-bit bitboards for faster move generation or use
/// megabytes large transposition tables.
///
/// \section opts CMake Configure-Time Options
/// In addition to user-provided buffers and bounds, a number of CMake options are provided to
/// tune the space / time tradeoffs made in the engine.
///
/// \subsection MAX_ZOBRIST_64
/// When enabled, the engine will use 64-bit zobrist hash keys for the transposition table and threefold repetition.
/// This will massively decrease the probability of a key collision between two different positions,
/// allowing greater transposition table usage and reducing the likelihood of an erroneous table hit
/// affecting the engine's search.
///
/// In exchange, transposition table entries grow by a few bytes to accomodate the larger key size,
/// potentially causing a very large memory usage increase over 32-bit keys depending on the size of the table.

/// Initialize all static lookup tables used by the engine.
/// This function must be called before any boards are created (checked when MAX_ASSERTS is on)
void max_init(void);

#ifdef MAX_TESTS

/// Run all unit tests registered for the library.
void max_unit_tests(void);

#endif
