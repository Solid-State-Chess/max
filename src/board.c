#include "max/board.h"
#include "max/move.h"
#include "max/piece.h"
#include "max/square.h"
#include <string.h>


static void max_board_postinit(max_board_t *const board);

void max_board_new(max_board_t *const board) {
    board->ply = 0;
    memset(&board->pieces, MAX_PIECECODE_INVAL, sizeof(board->pieces));
    memset(board->sides, 0, sizeof(board->sides));
    memset(&board->captures, 0, sizeof(board->captures));
}

void max_board_reset(max_board_t *const board) {
    max_board_new(board);
    
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

    max_board_postinit(board);
}

/// Initialize piece lists for each side by iterating through each square and set all valid empty squares to the proper piece code
static void max_board_postinit(max_board_t *const board) {
    for(uint8_t x = 0; x < 8; ++x) {
        for(uint8_t y = 0; y < 8; ++y) {
            max_bidx_t pos = (max_bidx_t){.parts.file = x, .parts.rank = y};
            max_piececode_t piece = board->pieces[pos.bits];
            
            if(piece == MAX_PIECECODE_INVAL) {
                board->pieces[pos.bits] = MAX_PIECECODE_EMPTY;
                continue;
            }
            
            max_sidestate_t *state = &board->sides[(piece & MAX_PIECECODE_BLACK) >> 6];
            max_piecelist_t *list = max_pieces_get_list(&state->piecelist, piece);
            state->index[pos.bits] = max_piecelist_add(list, pos);
        }
    }
}


void max_board_make_move(max_board_t *const board, max_move_t move) {
    max_piececode_t piece = board->pieces[move.from.bits];
    max_sidestate_t *side = max_board_get_to_move(board);
    max_piecelist_t *list = max_pieces_get_list(&side->piecelist, piece);


    max_lidx_t index = side->index[move.from.bits];

    switch(move.attr) {
        case MAX_MOVE_CAPTURE: {
            max_sidestate_t *enemy = max_board_get_enemy(board);

            max_piececode_t captured = board->pieces[move.to.bits];
            max_lidx_t captured_idx  = enemy->index[move.to.bits];

            max_capturestack_push(&board->captures, captured);

            max_piecelist_remove(max_pieces_get_list(&enemy->piecelist, captured), captured_idx);
        } break;
    }

    board->pieces[move.to.bits] = piece;
    board->pieces[move.from.bits] = MAX_PIECECODE_EMPTY;
    side->index[move.to.bits] = index;
    list->pos[index] = move.to;
    board->ply += 1;
}

void max_board_unmake_move(max_board_t *const board, max_move_t move) {
    board->ply -= 1;
    max_piececode_t piece = board->pieces[move.to.bits];
    max_sidestate_t *side = max_board_get_to_move(board);
    max_piecelist_t *list = max_pieces_get_list(&side->piecelist, piece);
    max_lidx_t index = side->index[move.to.bits];

    switch(move.attr) {
        case MAX_MOVE_NORMAL:
        case MAX_MOVE_DOUBLE: {
            board->pieces[move.to.bits] = MAX_PIECECODE_EMPTY;
        } break;
        case MAX_MOVE_CAPTURE: {
            max_sidestate_t *enemy = max_board_get_enemy(board);
            max_piececode_t captured = max_capturestack_pop(&board->captures);
            max_lidx_t captured_idx = max_piecelist_add(max_pieces_get_list(&enemy->piecelist, captured), move.to);
            enemy->index[move.to.bits] = captured_idx;
            board->pieces[move.to.bits] = captured;
        } break;
    }

    board->pieces[move.from.bits] = piece;
    side->index[move.from.bits]  = index;
    list->pos[index] = move.from;
}


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

void max_board_debugprint(max_board_t const* board) {
    uint8_t rank = 8;
    while(rank != 0) {
        rank -= 1;
        for(uint8_t file = 0; file <= 7; ++file) {
            max_piececode_t sq = board->pieces[(max_bidx_t){.parts.file = file, .parts.rank = rank}.bits];
            char code = piece_chars[sq & MAX_PIECECODE_TYPE_MASK];

            if(sq != MAX_PIECECODE_EMPTY && (sq & MAX_PIECECODE_COLOR_MASK) != MAX_PIECECODE_WHITE) {
                code = (code - 'a') + 'A';
            }

            putc(code, stdout);
        }
        putc('\n', stdout);
    }
}

#endif
