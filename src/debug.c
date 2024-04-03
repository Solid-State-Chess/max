#include "max/board.h"
#include "max/piece.h"
#include "private.h"

#if !defined(MAX_CONSOLE)

#include <stdio.h>

static char piece_chars[18] = {
    [MAX_PIECECODE_EMPTY] = '-',
    [MAX_PIECECODE_PAWN]  = 'p',
    [MAX_PIECECODE_KNIGHT] = 'n',
    [MAX_PIECECODE_BISHOP] = 'b',
    [MAX_PIECECODE_ROOK]   = 'r',
    [MAX_PIECECODE_QUEEN]  = 'q',
    [MAX_PIECECODE_KING]   = 'k'
};

/// Get an ASCII character for the given chess piece
static char piece_char(max_piececode_t piece) {
    char code = piece_chars[piece & MAX_PIECECODE_TYPE_MASK];

    if(piece != MAX_PIECECODE_EMPTY && (piece & MAX_PIECECODE_COLOR_MASK) != MAX_PIECECODE_WHITE) {
        code = (code - 'a') + 'A';
    }
    
    return code;
}

void max_board_debugprint(max_board_t const* board) {
    uint8_t rank = 8;
    while(rank != 0) {
        rank -= 1;
        for(uint8_t file = 0; file <= 7; ++file) {
            max_piececode_t sq = board->pieces[max_bidx_new(file, rank)];
            putc(piece_char(sq), stdout);
        }
        putc('\n', stdout);
    }
    
    max_plyplate_t plate = board->stack[board->ply];

    uint8_t eprank = plate & MAX_PLYPLATE_EP_MASK;
    if(eprank != MAX_PLYPLATE_EP_INVALID) {
        printf("EN PASSANT ON THE %c FILE\n", eprank + 'A');
    }
    

    puts  ("===========STATUS=========\n");
    puts  ("       K-Castle   Q-Castle\n");
    printf("White:    %c          %c  \n", (plate & max_kcastle_flag(0)) ? 'Y' : 'N', (plate & max_qcastle_flag(0)) ? 'Y' : 'N');
    printf("Black:    %c          %c  \n", (plate & max_kcastle_flag(1)) ? 'Y' : 'N', (plate & max_qcastle_flag(1)) ? 'Y' : 'N');
}

#endif
