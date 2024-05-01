#pragma once

#include "max/board/board.h"
#include "max/board/loc.h"
#include "max/board/state.h"
#include "max/def.h"

/// \ingroup board
/// @{

/// \name Private Functions
/// @{

/// Get the piece lists for the given side
MAX_INLINE_ALWAYS max_plist_t* max_board_side_list(max_chessboard_t *board, max_side_t side) {
    return &board->lists[side];
}

/// Get the game state for the current ply
MAX_INLINE_ALWAYS max_state_t* max_board_state(max_chessboard_t *board) {
    return max_state_stack_peek(&board->stack);
}

/// Add a piece to the board at the given position.
/// Updates the current zobrist hash, adds a piece to it's corresponding side's piece list,
/// updates the index and piece code boards as required.
/// \note No bounds checking is performed on the piece lists except when debug assertions are enabled
void max_board_add_piece_to_side(max_chessboard_t *board, max_plist_t *side, max_0x88_t pos, max_piececode_t piece);

/// Add a piece to the given board, determining the side to add its position to by the color of the 
/// piececode.
/// \see max_board_add_piece_to_side()
MAX_INLINE_ALWAYS void max_board_add_piece(max_chessboard_t *board, max_0x88_t pos, max_piececode_t piece) {
    max_board_add_piece_to_side(
        board,
        max_board_side_list(board, max_piececode_side(piece)),
        pos,
        piece
    );
}

/// @}

/// @}
