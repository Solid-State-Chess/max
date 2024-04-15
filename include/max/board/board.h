/// \file
/// Definition of the max engine's internal chessboard representation based on 0x88.
///
/// \see max_board_new()

#pragma once
#include "max/def.h"
#include "move.h"
#include "piece.h"
#include "state.h"
#include <stdint.h>


/// \brief Chessboard representation loosely based on 'New Architectures in Computer Chess' [1] by Fritz Reul.
///
/// \section repr Board Representation
/// The board is represented by three main data structures. First, the 8x16 #pieces array stores #max_piececode_t
/// indexed by a 0x88 index - enabling efficient off-the-board tests during movegen and direction lookup by position
/// difference. In addition, each side maintains individual lists for each piece type containing the board positions
/// of pieces of that type. In order to map from board position to list index, each side also maintains a 0x88 board
/// containing the list index of the piece on each square. While this adds memory requirements, the improvements to 
/// evaluation performance and movegen should outweigh the cost of bookkeeping the lists.
///
/// [1] https://pure.uvt.nl/ws/files/1098572/Proefschrift_Fritz_Reul_170609.pdf
typedef struct {
    union {
        struct {
            /// Piece lists and board position to list position map for white
            max_pieces_t white;
            /// Piece lists and board position to list position mapfor black
            max_pieces_t black;
        };
        
        /// Piece lists for both white and black stored as an array indexed by the current \link #max_side_t side to play \endlink
        /// Stored in a union with the #white and #black fields
        max_pieces_t sides[2];
    };
    
    /// Piece code 0x88 array for directly looking up piece codes by their
    /// #max_bpos_t positions. Most movegen operations use this board
    max_piececode_t pieces[MAX_BOARD_0x88_LEN];

    /// Stack tracking all captures made in this game enabling capture move unmaking
    max_board_capturestack_t captures;
    
    /// A stack of all irreversible state used to unmake moves efficiently without
    /// recomputing too much
    max_irreversible_stack_t stack;
    
    /// Ply (halfmove) counter, if the LSB is set (ply is odd) then black is to move
    /// \see #max_side_t
    uint16_t ply;
} max_board_t;

/// \brief Convenience typedef for a single-bit value representing the current side to move.
/// This value is standard and used for lookup tables in movegen, move validation, and evaluation - for
/// example, to look up the pawn promotion rank in an array for the current side to move.
///
/// 0 - White
///
/// 1 - Black
///
/// \see #MAX_SIDE_WHITE
/// \see #MAX_SIDE_BLACK
typedef uint8_t max_side_t;

enum {
    /// #max_side_t value indicating white side in arrays
    MAX_SIDE_WHITE = 0,
    /// #max_side_t value indicating black side in arrays
    MAX_SIDE_BLACK = 1
};

/// Get the current castle rights, check, and en passant state for this board
MAX_INLINE_ALWAYS max_irreversible_t* max_board_state(max_board_t *const board) {
    return max_irreversible_stack_peek(&board->stack);
}

/// Reset the given board's state stack pointer- after calling this moves made before the call must not be unmade
MAX_INLINE_ALWAYS void max_board_reset_stack(max_board_t *const board) {
    max_irreversible_stack_reset(&board->stack); 
}

/// Create a new chessboard structure initialized to the starting position,
/// using the given buffer as the irreversible state buffer - the size of this buffer
/// determines how many moves can be unmake between state resets
void max_board_new(max_board_t *const board, max_irreversible_t *ply_buf);

/// Make the given move on a chessboard, with NO CHECK for move validity (assumes valid moves taken from max_movegen)
void max_board_make_move(max_board_t *const board, max_move_t move);

/// Unmake the given move, restoring any captured pieces and ep / castling state
void max_board_unmake_move(max_board_t *const board, max_move_t move);

/// \defgroup utils Board Utilities
/// Convenience functions providing information about a #max_board_t.
/// Most of these functions do not have a specific purpose in mind - they are used
/// for all parts of the engine and move generation.
/// @{

/// Get a #max_side_t representing the side that will play a move this ply
MAX_INLINE_ALWAYS max_side_t max_board_friendly_side(max_board_t *board) {
    return board->ply & 1;
}

/// Get a #max_side_t index representing the side that will not play a move this ply
MAX_INLINE_ALWAYS max_side_t max_board_enemy_side(max_board_t *board) {
    return (board->ply & 1) ^ 1;
}

/// Get the piece list of the side that will play a move this ply
MAX_INLINE_ALWAYS max_pieces_t* max_board_friendly_pieces(max_board_t *board) {
    return &board->sides[max_board_friendly_side(board)];
}

/// Get piece list for the side that is not playing a move this ply
MAX_INLINE_ALWAYS max_pieces_t* max_board_enemy_pieces(max_board_t *board) {
    return &board->sides[max_board_enemy_side(board)];
}

/// Get the king's position for the side to play
MAX_INLINE_ALWAYS max_bpos_t max_board_friendly_king_pos(max_board_t *board) {
    return max_board_friendly_pieces(board)->king.pos[0];
}

/// Get a colormask for friendly pieces
MAX_INLINE_ALWAYS max_piececode_t max_board_friendly_colormask(max_board_t *board) {
    return MAX_PIECECODE_WHITE << max_board_friendly_side(board);
}

//// Get the direction (up or down) that enemy pawns will advance
MAX_INLINE_ALWAYS max_increment_t max_board_friendly_pawn_advance_dir(max_board_t *board) {
    return MAX_PAWN_DIR[max_board_friendly_side(board)];
}

// Get the direction (up or down) that enemy pawns will advance
MAX_INLINE_ALWAYS max_increment_t max_board_enemy_pawn_advance_dir(max_board_t *board) {
    return MAX_PAWN_DIR[max_board_enemy_side(board)];
}

/// Get the queenside castle right flag bits for the side to move.
///
/// \param ply Ply used to check which side will move for this turn.
/// \return A bitmask that can be used to check if the side to move has queenside castle rights.
MAX_INLINE_ALWAYS max_plyplate_t max_qcastle_flag(uint16_t ply) {
    return (MAX_PLYPLATE_KCASTLE << MAX_PLYPLATE_WCASTLE_OFFSET) << ((ply & 1) << 1);
}

/// Get the kingside castle flag bits for the side to move.
///
/// \param ply Ply used to check what side will move on this turn.
/// \return A bitmask that can be used to check if the current packed state allows the
/// side to play a kingside castle.
MAX_INLINE_ALWAYS max_plyplate_t max_kcastle_flag(uint16_t ply) {
    return (MAX_PLYPLATE_QCASTLE << MAX_PLYPLATE_WCASTLE_OFFSET) << ((ply & 1) << 1);
}

/// @}

#if defined(MAX_CONSOLE)

/// Print the given chessboard to the console
void max_board_debugprint(max_board_t const* board);

#endif
