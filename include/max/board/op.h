#pragma once
/// \file
/// Declarations for common board geometry operations - used by both the board when validating moves and
/// by the engine to evaluate positions.

#include "max/board/board.h"
#include <stdbool.h>

/// \defgroup operations Board Operations
/// Functions that scan a #max_board_t to provide information like checking lines, pins, and sliding
/// piece control. These functions are utilized internally in move validation to ensure that a pinned piece
/// does not leave a hanging king.
///
/// @{
///

/// Check that the two given positions on the chessboard are on the same diagonal or cardinal line and that
/// the space between them is unoccupied by both white and black pieces.
///
/// \return true if #pos and #to lie on the same line and only empty squares are between the two
MAX_INLINE_ALWAYS bool max_board_is_empty_between(max_board_t *const board, max_bpos_t pos, max_bpos_t to) {
    max_increment_t dir = MAX_DIRECTION_BY_DIFF[max_bpos_diff(pos, to)];
    if(dir == 0) {
        return false;
    }

    do {
        pos = max_bpos_inc(pos, dir);
    } while(board->pieces[pos] == MAX_PIECECODE_EMPTY);

    return pos == to;
}

/// Get all attacks by the enemy side for the current board ply on the given square.
///
/// \param board Board to read piece location data from
/// \param pos Board position that a superpiece will be placed at
/// \param [out] attacks Pointer to the beginning of a buffer that detected attacks will be stored in, or
/// NULL if we only want to find the number of attacks on the given square
/// \return Number of attacks that have been written to the #attacks buffer
uint8_t max_board_attacks(max_board_t *const board, max_bpos_t pos, max_checker_t *attacks);

/// Check if a piece on the given square is pinned to the king by an enemy sliding attacker.
/// 
/// \param pos Position of a piece to check for pin
/// \returns true if the piece lies on an x-ray attack from an enemy bishop, rook, or queen
bool max_board_is_pinned(max_board_t *const board, max_bpos_t pos);

/// Check if a piece moving from the given square to another square would exit a pinning line from an enemy piece
///
/// \param from The square that would be left empty after the piece moves
/// \param to The square that the piece is moving to, if this is on the same x-ray line then the piece does not exit check
bool max_board_move_exits_pin(
    max_board_t *const board,
    max_bpos_t from,
    max_bpos_t to
);

/// @}
