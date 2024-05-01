#include "max/board/board.h"
#include "max/board/loc.h"
#include "max/board/piececode.h"
#include "max/board/side.h"
#include "max/board/squares.h"
#include "private/board/board.h"
#include "private/board/capturelist.h"
#include "private/board/piecelist.h"
#include "private/board/state.h"
#include "private/board/zobrist.h"
#include <ctype.h>

static void max_chessboard_init_pieces(max_chessboard_t *board) {
    for(unsigned i = 0; i < MAX_0x88_LEN; ++i) {
        board->pieces[i].v = MAX_PIECECODE_INVALID;
    }
    
    for(unsigned i = 0; i < MAX_6BIT_LEN; ++i) {
        board->pieces[max_6bit_to_0x88(max_6bit_raw(i)).v].v = MAX_PIECECODE_EMPTY;
    }
}


void max_chessboard_new(max_chessboard_t *board, max_state_t *buffer, uint64_t seed) {
    max_zobrist_elements_init(&board->zobrist_state, seed);
    max_chessboard_init_pieces(board);

    max_plist_new(&board->side.white);
    max_plist_new(&board->side.black);

    max_captures_new(&board->captures);
    max_state_stack_new(&board->stack, buffer, max_state_default());

    board->ply = 0;
}

void max_board_add_piece_to_side(max_chessboard_t *board, max_plist_t *side, max_0x88_t pos, max_piececode_t piece) {
    MAX_ASSERT(board->pieces[pos.v].v == MAX_PIECECODE_EMPTY);

    board->pieces[pos.v] = piece;
    max_loclist_t *list = max_plist_get_list(side, piece);
    max_loclist_add(list, pos);

    max_state_t *state = max_state_stack_peek(&board->stack);
    state->position ^= max_zobrist_position_element(&board->zobrist_state, pos, piece);
}

void max_chessboard_default_pos(max_chessboard_t *board) {
    for(unsigned i = MAX_RANK_2; i <= MAX_H2.v; ++i) {
        max_0x88_t pos = max_0x88_raw(i);
        max_board_add_piece(board, pos, max_piececode_new(MAX_PIECECODE_WHITE, MAX_PIECECODE_PAWN));
        max_board_add_piece(board, max_0x88_mirror_y(pos), max_piececode_new(MAX_PIECECODE_BLACK, MAX_PIECECODE_PAWN));
    }
}


#ifdef MAX_CONSOLE
#include <stdio.h>
#include <inttypes.h>

static char MAX_PIECETYPE_LETTER[MAX_PIECEINDEX_LEN] = {
    [MAX_PIECEINDEX_PAWN]   = 'p',
    [MAX_PIECEINDEX_KNIGHT] = 'n',
    [MAX_PIECEINDEX_BISHOP] = 'b',
    [MAX_PIECEINDEX_ROOK]   = 'r',
    [MAX_PIECEINDEX_QUEEN]  = 'q',
    [MAX_PIECEINDEX_KING]   = 'k'
};

void max_chessboard_print(max_chessboard_t *board) {
    for(unsigned y = 8; y > 0; --y) {
        printf("%d ", y);
        for(unsigned x = 0; x < 8; ++x) {
            putchar(' ');

            max_0x88_t loc = max_0x88_new(y - 1, x);
            max_piececode_t piece = board->pieces[loc.v];
            if(piece.v == MAX_PIECECODE_EMPTY) {
                putchar('-');
            } else {
                char letter = MAX_PIECETYPE_LETTER[max_piececode_kind_index(piece)];
                if(piece.v & MAX_PIECECODE_BLACK) {
                    letter = toupper(letter);
                }

                putchar(letter);
            }
        }
        putchar('\n');
    }

    fputs("   ", stdout);

    for(char x = 'A'; x <= 'H'; ++x) {
        putchar(x);
        putchar(' ');
    }

    putchar('\n');
    
#ifdef MAX_ZOBRIST_64
    printf("Zobrist Key: %" PRIX64 "\n", max_board_state(board)->position);
#else
    printf("Zobrist Key: %0X\n", max_board_state(board)->position);
#endif
}

#endif
