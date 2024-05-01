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
MAX_INLINE_ALWAYS max_pieces_t* max_board_side_list(max_chessboard_t *board, max_side_t side) {
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
void max_board_add_piece_to_side(max_chessboard_t *board, max_pieces_t *side, max_0x88_t pos, max_piececode_t piece);

/// Remove the piece at the given position from the given side.
/// Updates the board's zobrist hash, but does NOT update the capture stack (this must be done manually).
void max_board_remove_piece_from_side(max_chessboard_t *board, max_pieces_t *side, max_0x88_t pos);

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

/// Remove the piece on the given square, looking up the correct side to remove the piece from by the 
/// color of the piece on the given square
MAX_INLINE_ALWAYS void max_board_remove_piece(max_chessboard_t *board, max_0x88_t pos) {
    max_board_remove_piece_from_side(
        board,
        max_board_side_list(board, max_piececode_side(board->pieces[pos.v])),
        pos
    );
}

/// @}

/// @}
