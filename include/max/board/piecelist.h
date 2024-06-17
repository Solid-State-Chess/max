/// \file piecelist.h

#pragma once

#include <stdint.h>
#include "max/board/loc.h"
#include "max/board/move.h"


/// \ingroup board
/// @{

/// \defgroup piecelist Piece Lists
/// Piece lists store the board location of each piece on the chessboard.
/// These allow efficient lookups during move generation and material / positional evaluation.
/// @{

/// Simple wrapper typedef over a uint8_t.
/// This type is an index into a #max_loclist_t
typedef uint8_t max_lidx_t;

/// A piece list storing the locations of one piece type indexed by #max_lidx_t.
/// This typedef has a variable size, and is thus only operated on by pointers.
/// In practice, it is provided so that piece list functions have a uniform interface
/// for piece lists of different static sizes.
typedef struct {
    /// Length of the locations array
    max_lidx_t len;
    /// Array of piece locations with a variable length - this will point to an
    /// array with a capacity guaranteed to be enough to hold all pieces of a given type.
    max_0x88_t loc[];
} max_loclist_t;

/// \name Maximum capacity required for piece lists.
/// @{
#define MAX_LIST_CAP_KING   (1)
#define MAX_LIST_CAP_QUEEN  (9)
#define MAX_LIST_CAP_ROOK   (10)
#define MAX_LIST_CAP_BISHOP (10)
#define MAX_LIST_CAP_KNIGHT (10)
#define MAX_LIST_CAP_PAWN   (8)
/// @}

/// Structure storing piece location lists for one side of the chess game.
/// In the chessboard, two of these structures are used for white and black respectively.
typedef struct {
    /// Piece lists for each piece type with static maximum capacities.
    /// @{

    struct {
        max_lidx_t len;
        max_0x88_t loc[MAX_LIST_CAP_KING];
    } king;

    struct {
        max_lidx_t len;
        max_0x88_t loc[MAX_LIST_CAP_QUEEN];
    } queen;

    struct {
        max_lidx_t len;
        max_0x88_t loc[MAX_LIST_CAP_ROOK];
    } rook;

    struct {
        max_lidx_t len;
        max_0x88_t loc[MAX_LIST_CAP_BISHOP];
    } bishop;

    struct {
        max_lidx_t len;
        max_0x88_t loc[MAX_LIST_CAP_KNIGHT];
    } knight;

    struct {
        max_lidx_t len;
        max_0x88_t loc[MAX_LIST_CAP_PAWN];
    } pawn;
    
    /// Initial positions of the A and H side rooks.
    /// For chess960, the two rooks must always be left (A side) and right
    /// (H side) of the king, but they may start on any square.
    /// In order to track castling rights and compute castling validity, we must know the initial
    /// square of the rook
    max_0x88_t initial_rook[MAX_CASTLES_LEN];

    /// @}
} max_pieces_t;


#ifdef MAX_CONSOLE
/// Print the given piece list to the console
void max_pieces_print(max_pieces_t *pieces);
#endif


/// @}

/// @}
