#include "max/board.h"
#include "max/piece.h"
#include "max/square.h"
#include "private.h"
#include <string.h>

const max_increment_t PAWN_INC[2] = {MAX_INCREMENT_UP, MAX_INCREMENT_DOWN};

const max_increment_t KNIGHT_MOVES[8] = {
    MAX_INCREMENT_UP    + MAX_INCREMENT_UR,
    MAX_INCREMENT_UP    + MAX_INCREMENT_UL,
    MAX_INCREMENT_DOWN  + MAX_INCREMENT_DR,
    MAX_INCREMENT_DOWN  + MAX_INCREMENT_DL,
    MAX_INCREMENT_RIGHT + MAX_INCREMENT_UR,
    MAX_INCREMENT_RIGHT + MAX_INCREMENT_DR,
    MAX_INCREMENT_LEFT  + MAX_INCREMENT_UL,
    MAX_INCREMENT_LEFT  + MAX_INCREMENT_DL,
};



static void max_board_init_lists(max_board_t *const board);

void max_board_new(max_board_t *const board) {
    board->ply = 0;
    memset(&board->pieces, MAX_PIECECODE_INVAL, sizeof(board->pieces));
    memset(board->sides, 0, sizeof(board->sides));
    memset(&board->captures, 0, sizeof(board->captures));
    board->stack[0] = MAX_PLYPLATE_WCASTLEMASK | MAX_PLYPLATE_BCASTLEMASK | MAX_PLYPLATE_EP_INVALID;
    
    for(unsigned i = MAX_A2; i <= MAX_H2; i += 1) {
        board->pieces[i] = MAX_PIECECODE_PAWN | MAX_PIECECODE_WHITE;
    }

    for(unsigned i = MAX_A7; i <= MAX_H7; i += 1) {
        board->pieces[i] = MAX_PIECECODE_PAWN | MAX_PIECECODE_BLACK;
    }

    #define SYMMETRY(r1, r2, kind) do {                                               \
        board->pieces[MAX_##r1##1] = board->pieces[MAX_##r2##1] = kind | MAX_PIECECODE_WHITE; \
        board->pieces[MAX_##r1##8] = board->pieces[MAX_##r2##8] = kind | MAX_PIECECODE_BLACK; \
    } while(0)

    SYMMETRY(A, H, MAX_PIECECODE_ROOK); 
    SYMMETRY(B, G, MAX_PIECECODE_KNIGHT);
    SYMMETRY(C, F, MAX_PIECECODE_BISHOP);

    #undef SYMMETRY

    board->pieces[MAX_D1] = MAX_PIECECODE_QUEEN | MAX_PIECECODE_WHITE;
    board->pieces[MAX_D8] = MAX_PIECECODE_QUEEN | MAX_PIECECODE_BLACK;

    board->pieces[MAX_E1] = MAX_PIECECODE_KING | MAX_PIECECODE_WHITE;
    board->pieces[MAX_E8] = MAX_PIECECODE_KING | MAX_PIECECODE_BLACK;

    max_board_init_lists(board);
    max_board_init_statics(board);
}

/// Initialize piece lists for each side by iterating through each square and set all valid empty squares to the proper piece code
static void max_board_init_lists(max_board_t *const board) {
    for(uint8_t x = 0; x < 8; ++x) {
        for(uint8_t y = 0; y < 8; ++y) {
            max_bpos_t pos = max_bpos_new(x, y);
            max_piececode_t piece = board->pieces[pos];
            
            if(piece == MAX_PIECECODE_INVAL) {
                board->pieces[pos] = MAX_PIECECODE_EMPTY;
                continue;
            }
            
            max_pieces_t *side = &board->sides[(piece & MAX_PIECECODE_BLACK) >> 6];
            max_board_add_piece(board, side, pos, piece);
        }
    }
}
