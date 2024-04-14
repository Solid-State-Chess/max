#include "max/board.h"
#include "max/piece.h"
#include "max/square.h"
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

static void print_checker(max_checker_t check) {
    if(max_checker_is_valid(check)) {
        if(max_checker_is_sliding(check)) {
            printf("Sliding check delivered from %c%c\n", MAX_BPOS_FORMAT(check.attack.ray.origin));
        } else {
            printf("Check delivered from %c%c\n", MAX_BPOS_FORMAT(check.attack.jump));
        }
    }
}

void max_board_debugprint(max_board_t const* board) {
    uint8_t rank = 8;
    while(rank != 0) {
        printf("%d | ", rank);
        rank -= 1;
        for(uint8_t file = 0; file <= 7; ++file) {
            max_piececode_t sq = board->pieces[max_bpos_new(file, rank)];
            putc(piece_char(sq), stdout);
            putc(' ', stdout);
        }
        putc('\n', stdout);
    }
    
    printf("    ");
    for(uint8_t file = 0; file <= 7; ++file) {
        putc(file + 'A', stdout);
        putc(' ', stdout);
    }
    putc('\n', stdout);
    
    max_irreversible_t *state = max_board_state(board);
    max_plyplate_t plate = state->packed_state;

    uint8_t eprank = plate & MAX_PLYPLATE_EP_MASK;
    if(eprank != MAX_PLYPLATE_EP_INVALID) {
        printf("EN PASSANT ON THE %c FILE\n", eprank + 'A');
    }
    

    puts  ("===========STATUS=========\n");
    puts  ("       K-Castle   Q-Castle\n");
    printf("White:    %c          %c  \n", (plate & max_kcastle_flag(0)) ? 'Y' : 'N', (plate & max_qcastle_flag(0)) ? 'Y' : 'N');
    printf("Black:    %c          %c  \n", (plate & max_kcastle_flag(1)) ? 'Y' : 'N', (plate & max_qcastle_flag(1)) ? 'Y' : 'N');
    print_checker(state->check.attacks[0]);
    print_checker(state->check.attacks[1]);
    
}

#endif