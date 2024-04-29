/// \file board.h
/// Definition of the full chessboard representation.


#pragma once


#include <stdint.h>

#include "max/board/loc.h"
#include "max/board/piececode.h"
#include "max/board/piecelist.h"

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
/// In addition to the piece bookkeeping, we also track certain irreversible state of the game - castle rights,
/// en passant availability, captures made, and the last two positions reached to determine draws by threefold repetition.
/// 
/// 
/// [1] https://pure.uvt.nl/ws/files/1098572/Proefschrift_Fritz_Reul_170609.pdf
typedef struct {
    /// Piece type and color data array indexed by a packed board location #max_loc_t.
    /// This array is the most commonly accessed during most operations, as it allows us to
    /// find a piece by location.
    /// The locations of each piece on this board must be kept in sync with the #sides piece lists
    /// during all operations on the board.
    max_piececode_t pieces[MAX_0x88_LEN];
    
    /// A reverse map storing the list position of the piece on each square indexed by a 0x88 board location.
    /// These indices map into either the #white or #black piece lists depending on the color and type of the piececode
    /// in #pieces located at the same square.
    /// This structure must be kept in sync with the #pieces array and the respective piece lists for each side.
    max_plidx_t indices[MAX_0x88_LEN];
    
    /// Piece lists for white and black tracking the positions of every piece on the board.
    /// Stored as a union between a two-element array and a structure to allow access by side name
    /// and efficient indexing by a #max_side_t
    union {
        struct {
            max_plist_t white;
            max_plist_t black;
        } side;

        max_plist_t lists[2];
    };
    
    /// Counter of the number of plies (halfmoves) that have been played so far.
    /// This counter is also used to derive the current #max_side_t index by bitwise ANDing
    /// with 1 (e.g. an odd ply means black is to move on that ply)
    uint16_t ply;
} max_chessboard_t;

/// @}
