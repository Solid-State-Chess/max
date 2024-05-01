#include "max/board/board.h"
#include "max/board/loc.h"
#include "max/board/piececode.h"
#include "max/board/squares.h"
#include "max/board/state.h"
#include "private/board/board.h"
#include "private/board/capturelist.h"
#include "private/board/piecelist.h"
#include "private/board/state.h"
#include "private/board/zobrist.h"

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

void max_board_add_piece_to_side(max_chessboard_t *board, max_pieces_t *side, max_0x88_t pos, max_piececode_t piece) {
    MAX_ASSERT(board->pieces[pos.v].v == MAX_PIECECODE_EMPTY);

    board->pieces[pos.v] = piece;

    max_loclist_t *list = max_plist_get_list(side, piece);
    max_lidx_t idx = max_loclist_add(list, pos);
    board->indices[pos.v] = idx;

    max_state_t *state = max_board_state(board);
    state->position ^= max_zobrist_position_element(&board->zobrist_state, pos, piece);
}


void max_board_remove_piece_from_side(max_chessboard_t *board, max_pieces_t *side, max_0x88_t pos) {
    MAX_ASSERT(board->pieces[pos.v].v != MAX_PIECECODE_EMPTY);

    max_piececode_t piece = board->pieces[pos.v];
    board->pieces[pos.v].v = MAX_PIECECODE_EMPTY;

    max_lidx_t idx = board->indices[pos.v];
    max_loclist_t *list = max_plist_get_list(side, piece);
    max_loclist_remove(list, idx);

    max_state_t *state = max_board_state(board);
    state->position ^= max_zobrist_position_element(&board->zobrist_state, pos, piece);
}

static void max_board_add_mirrored(max_chessboard_t *board, max_0x88_t pos, uint8_t piecetype) {
    max_0x88_t mirrored = max_0x88_mirror_y(pos);
    max_board_add_piece_to_side(board, &board->side.white, pos, max_piececode_new(MAX_PIECECODE_WHITE, piecetype));
    max_board_add_piece_to_side(board, &board->side.black, mirrored, max_piececode_new(MAX_PIECECODE_BLACK, piecetype));
}

void max_chessboard_default_pos(max_chessboard_t *board) {
    for(unsigned i = MAX_RANK_2; i <= MAX_H2.v; ++i) {
        max_0x88_t pos = max_0x88_raw(i);
        max_board_add_mirrored(board, pos, MAX_PIECECODE_PAWN);
    }
    
    max_board_add_mirrored(board, MAX_A1, MAX_PIECECODE_ROOK);
    max_board_add_mirrored(board, MAX_H1, MAX_PIECECODE_ROOK);
    max_board_add_mirrored(board, MAX_B1, MAX_PIECECODE_KNIGHT);
    max_board_add_mirrored(board, MAX_G1, MAX_PIECECODE_KNIGHT);
    max_board_add_mirrored(board, MAX_C1, MAX_PIECECODE_BISHOP);
    max_board_add_mirrored(board, MAX_F1, MAX_PIECECODE_BISHOP);

    max_board_add_mirrored(board, MAX_D1, MAX_PIECECODE_QUEEN);
    max_board_add_mirrored(board, MAX_E1, MAX_PIECECODE_KING);
}


#ifdef MAX_CONSOLE
#include <stdio.h>
#include <ctype.h>

#ifdef MAX_ZOBRIST_64
#include <inttypes.h>
#endif

static char MAX_PIECETYPE_LETTER[MAX_PIECEINDEX_LEN] = {
    [MAX_PIECEINDEX_PAWN]   = 'p',
    [MAX_PIECEINDEX_KNIGHT] = 'n',
    [MAX_PIECEINDEX_BISHOP] = 'b',
    [MAX_PIECEINDEX_ROOK]   = 'r',
    [MAX_PIECEINDEX_QUEEN]  = 'q',
    [MAX_PIECEINDEX_KING]   = 'k'
};

/// Print a single line to represent the type and origin of check
static void print_check(max_check_t check) {
    if(max_check_is_sliding(check)) {
        fputs("sliding ", stdout);
    }
    printf("check delivered from %c%c", MAX_0x88_FORMAT(check.origin));
}

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

        fputs("    ", stdout);
        
        switch(y) {
            case 8: printf("STATE"); break;
            
            case 7:
#ifdef MAX_ZOBRIST_64
    printf("Zobrist Key: %" PRIX64 "\n", max_board_state(board)->position);
#else
    printf("Zobrist Key: %0X", max_board_state(board)->position);
#endif
            break;

            case 6: {
                max_state_t *state = max_board_state(board);
                uint8_t checks = max_state_checks(state);
                if(checks > 0) {
                    print_check(state->check[0]);
                    if(checks > 1) {
                        print_check(state->check[1]);
                    }
                }
            } break;
        }

        putchar('\n');
    }

    fputs("\n   ", stdout);

    for(char x = 'A'; x <= 'H'; ++x) {
        putchar(x);
        putchar(' ');
    }

    putchar('\n');
}

#endif
