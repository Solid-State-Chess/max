/// \file board.h
/// Definition of the full chessboard representation.


#pragma once


#include <stdbool.h>
#include <stdint.h>

#include "max/board/capturelist.h"
#include "max/board/state.h"
#include "max/board/loc.h"
#include "max/board/piececode.h"
#include "max/board/piecelist.h"
#include "max/def.h"

/// \defgroup board Chessboard
/// Representation of the full state of an actual chess game. 
/// These data structures are used by the engine search functions to actually search and evaluate a game tree.
/// @{

/// Chessboard representation based on the architecture proposed in 'New Architectures in Computer Chess' [1] by Fritz Reul.
/// The structure mainly consists of two data structures that must be kept in sync over the course of move making and unmaking,
/// the piece code by square array and board position-by-type lists maintained for each side.
/// The additional memory and complexity costs of the redundant structures are heavily outweighed by the gains
/// in efficiency during evaluation and move generation.
///
/// \section irrev Irreversible State
/// In addition to the piece bookkeeping, we also track certain irreversible state of the game - castle rights,
/// en passant availability, captures made, and the last two positions reached to determine draws by threefold repetition. 
/// 
/// Two data structures are responsible for the bookkeeping required to make and unmake moves that change the state irreversibly.
/// The capture stack stores the type and color code of each captured piece, enabling capture moves to be unmade by popping from the stack.
/// In addition to this, a second stack maintains irreversible state computed for each ply - castle rights, en passant validity, check attack
/// data, and a unique zobrist hash of the position.
/// 
/// [1] https://pure.uvt.nl/ws/files/1098572/Proefschrift_Fritz_Reul_170609.pdf
typedef struct {
    /// Statically initialized arrays used to iteratively compute zobrist hash keys.
    max_zobrist_elements_t zobrist_state;

    /// Piece type and color data array indexed by a packed board location #max_loc_t.
    /// This array is the most commonly accessed during most operations, as it allows us to
    /// find a piece by location.
    /// The locations of each piece on this board must be kept in sync with the #sides piece lists
    /// during all operations on the board.
    max_piececode_t pieces[MAX_0x88_LEN];
    
    /// A reverse map storing the list index of the piece on each square indexed by a 0x88 board location.
    /// These indices map into either the #white or #black piece lists depending on the color and type of the piececode
    /// in #pieces located at the same square.
    /// This structure must be kept in sync with the #pieces array and the respective piece lists for each side.
    max_lidx_t indices[MAX_0x88_LEN];
    
    /// Piece lists for white and black tracking the positions of every piece on the board.
    /// Stored as a union between a two-element array and a structure to allow access by side name
    /// and efficient indexing by a #max_side_t
    union {
        struct {
            max_pieces_t white;
            max_pieces_t black;
        } side;

        max_pieces_t lists[2];
    };
    
    /// A stack of all captures made over the course of the game, enabling move unmaking for capture moves.
    max_captures_t captures;
    
    /// The variable-size stack of game states used to make and unmake moves - 
    /// *and* to determine draws by threefold repetition.
    /// It is guaranteed that the stack always contains at least one element in order to represent the
    /// game state on the current ply, and in addition the board will always attempt to keep
    /// a least three valid state entries in the stack in order to determine draws by threefold repetition.
    max_state_stack_t stack;
    
    /// Counter of the number of plies (halfmoves) that have been played so far.
    /// This counter is also used to derive the current #max_side_t index by bitwise ANDing
    /// with 1 (e.g. an odd ply means black is to move on that ply)
    uint16_t ply;
} max_board_t;

/// Create a new chessboard with no pieces on the board and a default state
/// (both sides able to castle, en passant is not available).
///
/// \param [out] board A pointer to an uninitialized board structure that will be initialized
/// \param [in] buffer A pointer to the buffer that will be used to maintain the state stack of the board
/// \param [in] seed Seed to use for the random number generator when creating zobrist elements
void max_board_new(max_board_t *board, max_state_t *buffer, uint64_t seed);

/// Reset the given chessboard, removing any pieces and resetting the capture and state stacks.
void max_board_reset(max_board_t *board);

/// Clear the given board, then add pieces in their default positions.
/// This effectively begins a new game on the board, clearing all prior state.
void max_board_default_pos(max_board_t *board);

/// Check if the given board has drawn by threefold repetition.
/// Note that the state stack must have at least three elements in
/// order to compare past positions.
/// \return true if a position has been repeated three times
MAX_INLINE_ALWAYS bool max_board_threefold(max_board_t *board) {
    if(board->stack.head < 2) {
        return false;
    }

    max_state_t *head = board->stack.head_ptr;

    return head->position ==(head - 1)->position &&
        head->position == (head - 2)->position;
}

/// Get a side flag for the current side to play on the board's ply
MAX_INLINE_ALWAYS max_side_t max_board_side(max_board_t *board) {
    return board->ply & 1;
}

/// Get a side flag for the enemy to the current side to play on the board's ply
MAX_INLINE_ALWAYS max_side_t max_board_enemy_side(max_board_t *board) {
    return max_board_side(board) ^ 1;
}

#ifdef MAX_CONSOLE

/// Draw a debug representation of the given chessboard to stdout
void max_board_print(max_board_t *board);

#endif

/// @}
