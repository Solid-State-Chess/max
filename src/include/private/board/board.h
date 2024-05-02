#pragma once

#include "max/board/board.h"
#include "max/board/loc.h"
#include "max/board/state.h"
#include "max/def.h"

/// \ingroup board
/// @{

/// \name Private Functions
/// @{

/// Get the pieces structure for the given side.
MAX_INLINE_ALWAYS max_pieces_t* max_board_side_list(max_board_t *board, max_side_t side) {
    return &board->lists[side];
}

/// Peek the top of the state stack for the game state on the current ply.
MAX_INLINE_ALWAYS max_state_t* max_board_state(max_board_t *board) {
    return max_state_stack_peek(&board->stack);
}

/// Add a piece to the board at the given position.
/// Updates the current zobrist hash, adds a piece to it's corresponding side's piece list,
/// updates the index and piece code boards as required.
/// \note No bounds checking is performed on the piece lists except when debug assertions are enabled
void max_board_add_piece_to_side(max_board_t *board, max_pieces_t *side, max_0x88_t pos, max_piececode_t piece);

/// Remove the piece at the given position from the given side.
/// Updates the board's zobrist hash, but does NOT update the capture stack (this must be done manually).
/// \return The piece code of the piece that was removed from the given square
max_piececode_t max_board_remove_piece_from_side(max_board_t *board, max_pieces_t *side, max_0x88_t pos);

/// Add a piece to the given board, determining the side to add its position to by the color of the 
/// piececode.
/// \see max_board_add_piece_to_side()
MAX_INLINE_ALWAYS void max_board_add_piece(max_board_t *board, max_0x88_t pos, max_piececode_t piece) {
    max_board_add_piece_to_side(
        board,
        max_board_side_list(board, max_piececode_side(piece)),
        pos,
        piece
    );
}

/// Shift a piece from the given square to another square.
/// Takes an already retrieved side state to modify, which MUST be the same side as the piece located on 
/// the to square belongs to.
void max_board_move_piece_from_side(max_board_t *board, max_pieces_t *side, max_0x88_t from, max_0x88_t to);

/// Shift a piece from the given square to another square, looking up the side state that must be modified by the
/// color of the piece on the given square
/// \see max_board_remove_piece_from_side()
MAX_INLINE_ALWAYS void max_board_move_piece(max_board_t *board, max_0x88_t from, max_0x88_t to) {
    max_board_move_piece_from_side(
        board,
        max_board_side_list(board, max_piececode_side(board->pieces[from.v])),
        from,
        to
    );
}

/// Remove the piece on the given square, looking up the correct side to remove the piece from by the 
/// color of the piece on the given square.
/// \see max_board_remove_piece_from_side()
MAX_INLINE_ALWAYS max_piececode_t max_board_remove_piece(max_board_t *board, max_0x88_t pos) {
    return max_board_remove_piece_from_side(
        board,
        max_board_side_list(board, max_piececode_side(board->pieces[pos.v])),
        pos
    );
}

/// @}

/// @}
