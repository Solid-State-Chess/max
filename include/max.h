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
/// \subsection MAX_PACKED_MOVE
/// \note This option is only meant for extremely space-constrained systems for which a kilobyte or so savings is worth spending the 
/// cycles on unpacking moves before playing them in the engine.
///
/// This option controls the size of moves when they are stored in a #max_movelist_t ONLY (internally, the board always uses the unpacked representation for move making and unmaking).
/// Packed moves save one byte per entry in the engine's movelists - potentially adding up to a few kilobytes of savings.
/// In exchange, a few more cycles are required to play each move as moves are unpacked to their 0x88 square representation.
/// 



#ifdef MAX_TESTS

/// Run all unit tests registered for the library.
void max_unit_tests(void);

#endif
