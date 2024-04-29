#include "max/board/board.h"
#include "max/board/piece.h"
#include "max/board/square.h"

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

void max_board_debugprint(max_board_t * board) {
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
    printf("Zobrist Hash: %0X\n", board->zobrist.hash);
    printf("Stack Plates: %d\n", board->stack.head + 1);
    puts  ("       K-Castle   Q-Castle");
    printf("White:    %c          %c  \n", (plate & max_kcastle_flag(0)) ? 'Y' : 'N', (plate & max_qcastle_flag(0)) ? 'Y' : 'N');
    printf("Black:    %c          %c  \n", (plate & max_kcastle_flag(1)) ? 'Y' : 'N', (plate & max_qcastle_flag(1)) ? 'Y' : 'N');
    print_checker(state->check.attacks[0]);
    print_checker(state->check.attacks[1]);
    
}

static void max_board_print_positions(max_piecelist_t *list) {
    for(uint8_t i = 0; i < list->len; ++i) {
        printf("%c%c", MAX_BPOS_FORMAT(list->pos[i]));
        if(i != list->len - 1) {
            fputs(" | ", stdout);
        }
    }
}


void max_board_debugprint_list(max_pieces_t *pieces) {
    puts("PAWNS: ");
    max_board_print_positions((max_piecelist_t*)&pieces->pawns);
    putchar('\n');
    puts("KNIGHTS: ");
    max_board_print_positions((max_piecelist_t*)&pieces->knights);
    putchar('\n');
    puts("BISHOPS: ");
    max_board_print_positions((max_piecelist_t*)&pieces->bishops);
    putchar('\n');
    puts("ROOKS: ");
    max_board_print_positions((max_piecelist_t*)&pieces->rooks);
    putchar('\n');
    puts("QUEENS: ");
    max_board_print_positions((max_piecelist_t*)&pieces->queens);
    putchar('\n');
    puts("KINGS: ");
    max_board_print_positions((max_piecelist_t*)&pieces->king);
    putchar('\n');
}

#endif
