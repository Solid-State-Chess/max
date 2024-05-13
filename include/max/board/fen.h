#pragma once
#include "board.h"

/// \ingroup board
/// @{

/// \defgroup parse PGN and FEN parsing
/// Utilities to initialize a chess position from a FEN string.
/// @{

/// Errors that may occur when parsing a string in FEN notation.
/// \see max_board_parse_from_fen()
typedef enum {
    /// Parsing succeeded and the board is ready to be used
    MAX_FEN_SUCCESS = 0,
    /// Parsing was succeeding but the string ended to early
    MAX_FEN_ERR_EOF,
    /// One of the characters meant to represent a piece on a given rank was not recognized
    MAX_FEN_ERR_INVALID_PIECE,
    /// The number of rank segments was not equal to eight
    MAX_FEN_ERR_BAD_RANK_COUNT,
    /// The number of spaces and pieces did not add up to 8
    MAX_FEN_ERR_BAD_FILE_COUNT,
    /// The side to play specifier was not 'w' or 'b'
    MAX_FEN_ERR_BAD_PLAY_SIDE,
    /// Castle rights was not either of the form KQkq or HAha
    MAX_FEN_ERR_INVALID_CASTLE_RIGHTS,
    /// The en-passant square could not be parsed, or when parsed it was off the board
    MAX_FEN_ERR_INVALID_EPSQUARE,
} max_fen_parse_err_t;

/// Parse a Forsyth-Edwards notation string to initialize the given chess board, adding pieces as described
/// by the string.
/// Also sets the castle rights, en passant file, and side to move from the string
max_fen_parse_err_t max_board_parse_from_fen(max_board_t *board, char const *fen);

#ifdef MAX_CONSOLE

/// Get a printable string for the given FEN parsing error code
char const *max_fen_parse_err_str(max_fen_parse_err_t ec);

#endif


/// @}

/// @}
