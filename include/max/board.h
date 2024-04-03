#pragma once
#include "max/def.h"
#include "max/move.h"
#include "max/piece.h"
#include <stdint.h>


/// Stack structure holding all pieces that have been captured, used for move unmaking
typedef struct {
    max_piececode_t captures[MAX_BOARD_PIECES_COUNT];
    uint8_t head;
} max_board_capturestack_t;


/// A plate in the history stack of a `max_board_t` tracking irreversible parts of the game to allow
/// move unmaking
///
/// [2 bits for black castle rights] - [2 bits for white castle rights] - [4 bits for en passant file index]
typedef uint8_t max_plyplate_t;

enum {
    /// Mask for 4 bits representing en passantable file (invalid file means no en passant in possible)
    MAX_PLYPLATE_EP_MASK     = 0b00001111,
    MAX_PLYPLATE_EP_INVALID  = MAX_PLYPLATE_EP_MASK,
    MAX_PLYPLATE_WCASTLEMASK = 0b00110000,
    MAX_PLYPLATE_BCASTLEMASK = 0b11000000,
    MAX_PLYPLATE_QCASTLE     = 0b00000001,
    MAX_PLYPLATE_KCASTLE     = 0b00000010,
    /// Amount to bit shift `MAX_PLYPLATE_QCASTLE` to reach the white castle right bits
    MAX_PLYPLATE_CASTLE_OFFSET = 4,
};

/// Zobrist hash of a board, updated incrementally with the board
typedef uint32_t max_zobrist_key_t;

/// Chessboard representation loosely based on 'New Architectures in Computer Chess' by Fritz Reul
typedef struct {
    /// Piece lists for white and black, tracking list position by square and square position by list position
    union {
        struct {
            max_pieces_t white;
            max_pieces_t black;
        };

        max_pieces_t sides[2];
    };
    
    /// Piece code 0x88 array for directly looking up piece by square
    max_piececode_t pieces[MAX_BOARD_0x88_LEN];
    
    /// Tracking for the last captured piece for move unmaking
    max_board_capturestack_t captures;
    /// A stack of all irreversible game state
    max_plyplate_t stack[MAX_BOARD_MAX_PLY];
    
    /// If the LSB is set (ply is odd), then black is to move
    uint16_t ply;
} max_board_t;

/// Get the piece list of the side that is currently to move
MAX_INLINE_ALWAYS max_pieces_t* max_board_get_to_move(max_board_t *board) {
    return &board->sides[(board->ply & 1)];
}

/// Get side state for the side that is not to move this ply
MAX_INLINE_ALWAYS max_pieces_t* max_board_get_enemy(max_board_t *board) {
    return &board->sides[(board->ply & 1) ^ 1];
}

/// Generate all pseudo-valid moves for the current side to move on the given board
void max_board_movegen_pseudo(max_board_t *const board, max_movelist_t *const moves);

/// Generate all pseudo-legal capture moves for the side to play
void max_board_capturegen_pseudo(max_board_t *const board, max_movelist_t *const moves);

/// Check if the given move is legal - it does not leave the king in check
bool max_board_move_is_valid(max_board_t *const board, max_move_t move);

/// Reset the given chessboard to the starting configuration
void max_board_new(max_board_t *const board);

/// Make the given move on a chessboard, with NO CHECK for move validity (assumes valid moves taken from max_movegen)
void max_board_make_move(max_board_t *const board, max_move_t move);

/// Unmake the given move, restoring any captured pieces and ep / castling state
void max_board_unmake_move(max_board_t *const board, max_move_t move);

/// Add a piece to the given capture stack
MAX_INLINE_ALWAYS void max_capturestack_push(max_board_capturestack_t *stack, max_piececode_t piece) {
    stack->captures[stack->head] = piece;
    stack->head += 1;
}

/// Pop the top element from the capture stack
MAX_INLINE_ALWAYS max_piececode_t max_capturestack_pop(max_board_capturestack_t *stack) {
    stack->head -= 1;
    max_piececode_t piece = stack->captures[stack->head];
    return piece;
}

#if defined(MAX_CONSOLE)

/// Print the given chessboard to the console
void max_board_debugprint(max_board_t const* board);

#endif

/// Remove a piece by position from the given side
MAX_HOT
MAX_INLINE_ALWAYS void max_board_remove_piece(max_board_t *board, max_pieces_t *side, max_bpos_t pos) {
    max_piececode_t piece = board->pieces[pos];
    board->pieces[pos] = MAX_PIECECODE_EMPTY;
    max_piecelist_t *list = max_pieces_get_list(side, piece);
    max_lidx_t index = side->index[pos];
    
    //Replace the removed element with the last element of the list
    list->len -= 1;
    max_bpos_t shuffled_pos = list->pos[list->len];
    list->pos[index] = shuffled_pos;
    side->index[shuffled_pos] = index;
}

/// Add a piece with the given code to the chessboard
MAX_HOT
MAX_INLINE_ALWAYS void max_board_add_piece(max_board_t *board, max_pieces_t *side, max_bpos_t pos, max_piececode_t piece) {
    board->pieces[pos] = piece;
    max_piecelist_t *list = max_pieces_get_list(side, piece);
    max_lidx_t index = list->len;
    list->pos[list->len] = pos;
    list->len += 1;

    side->index[pos] = index;
}

/// Shift the piece from the given location to the given location, replacing its old position with an empty square
MAX_HOT
MAX_INLINE_ALWAYS void max_board_shift_piece(max_board_t *board, max_pieces_t *side, max_bpos_t from, max_bpos_t to) {
    max_piececode_t piece = board->pieces[from];
    max_lidx_t index = side->index[from];
    max_piecelist_t *list = max_pieces_get_list(side, piece);
    list->pos[index] = to;
    side->index[to] = index;
    board->pieces[to] = piece;
    
    board->pieces[from] = MAX_PIECECODE_EMPTY;
}
