#include "max/board.h"
#include "max/move.h"
#include "max/piece.h"
#include "max/square.h"
#include "private.h"
#include <string.h>

const max_increment_t PAWN_INC[2] = {MAX_INCREMENT_UP, MAX_INCREMENT_DOWN};

static void max_board_postinit(max_board_t *const board);

void max_board_new(max_board_t *const board) {
    board->ply = 0;
    memset(&board->pieces, MAX_PIECECODE_INVAL, sizeof(board->pieces));
    memset(board->sides, 0, sizeof(board->sides));
    memset(&board->captures, 0, sizeof(board->captures));
    board->stack[0] = MAX_PLYPLATE_WCASTLEMASK | MAX_PLYPLATE_BCASTLEMASK | MAX_PLYPLATE_EP_INVALID;
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
            max_bidx_t pos = max_bidx_new(x, y);
            max_piececode_t piece = board->pieces[pos];
            
            if(piece == MAX_PIECECODE_INVAL) {
                board->pieces[pos] = MAX_PIECECODE_EMPTY;
                continue;
            }
            
            max_sidestate_t *state = &board->sides[(piece & MAX_PIECECODE_BLACK) >> 6];
            max_piecelist_t *list = max_pieces_get_list(&state->piecelist, piece);
            state->index[pos] = max_piecelist_add(list, pos);
        }
    }
}


void max_board_make_move(max_board_t *const board, max_move_t move) {
    static uint8_t KCASTLE_ROOK_SQUARE[2] = {MAX_H1, MAX_H8};
    static uint8_t QCASTLE_ROOK_SQUARE[2] = {MAX_A1, MAX_A8};

    max_piececode_t piece = board->pieces[move.from];
    //0 if white, 1 if black
    uint8_t side_to_move = board->ply & 1;

    max_sidestate_t *side = &board->sides[side_to_move];
    max_piecelist_t *list = max_pieces_get_list(&side->piecelist, piece);
    max_lidx_t moved_idx = side->index[move.from];

    max_plyplate_t state_plate = board->stack[board->ply] | MAX_PLYPLATE_EP_INVALID;
    /*if(piece & MAX_PIECECODE_KING) {
        state_plate &= ~(MAX_PLYPLATE_WCASTLEMASK << (side_to_move << 1));
    } else {
        if(piece & MAX_PIECECODE_ROOK) {
            if(move.from == KCASTLE_ROOK_SQUARE[side_to_move]) {
                state_plate &= ~((MAX_PLYPLATE_KCASTLE << MAX_PLYPLATE_CASTLE_OFFSET) << (side_to_move << 1));
            } else if(move.from == QCASTLE_ROOK_SQUARE[side_to_move]) {
                state_plate &= ~((MAX_PLYPLATE_QCASTLE << MAX_PLYPLATE_CASTLE_OFFSET) << (side_to_move << 1));
            }
        }
    }*/

    #define CAPTUREPIECE(at) do {                                                                   \
        max_sidestate_t *enemy = &board->sides[side_to_move ^ 1];                                   \
        max_piececode_t captured = board->pieces[(at)];                                             \
        max_lidx_t captured_idx  = enemy->index[(at)];                                              \
        max_capturestack_push(&board->captures, captured);                                          \
        max_piecelist_remove(max_pieces_get_list(&enemy->piecelist, captured), captured_idx);       \
    } while(0)

    #define SWAPROOK(from, to) do {                                                                \
        board->pieces[(to)] = board->pieces[(from)];                                               \
        board->pieces[(from)] = MAX_PIECECODE_EMPTY;                                               \
        side->piecelist.rooks.pos[side->index[(from)]] = (to);                                     \
    } while(0)

    switch(move.attr) {
        case MAX_MOVE_NORMAL: break;
        case MAX_MOVE_DOUBLE: {
            state_plate &= ~(MAX_PLYPLATE_EP_INVALID);
            //Set the en-passant file
            state_plate |= move.from & 7;
        } break;

        case MAX_MOVE_EN_PASSANT: {
            max_bidx_t takeat = max_bidx_inc(move.to, PAWN_INC[side_to_move ^ 1]);
            CAPTUREPIECE(takeat);
            board->pieces[takeat] = MAX_PIECECODE_EMPTY;
        } break;
        case MAX_MOVE_CAPTURE: CAPTUREPIECE(move.to); break;

        case MAX_MOVE_KCASTLE: {
            max_bidx_t old_rook_pos = max_bidx_inc(move.to, MAX_INCREMENT_RIGHT);
            max_bidx_t rook_pos = max_bidx_inc(move.to, MAX_INCREMENT_LEFT);
            SWAPROOK(old_rook_pos, rook_pos); 
        } break;

        case MAX_MOVE_QCASTLE: {
            max_bidx_t old_rook_pos = max_bidx_inc(move.to, MAX_INCREMENT_RIGHT);
            max_bidx_t rook_pos = max_bidx_inc(move.to, MAX_INCREMENT_LEFT);
            SWAPROOK(old_rook_pos, rook_pos);
        } break;
    }

    board->pieces[move.to] = piece;
    board->pieces[move.from] = MAX_PIECECODE_EMPTY;
    side->index[move.to] = moved_idx;
    list->pos[moved_idx] = move.to;

    
    board->ply += 1;
    board->stack[board->ply] = state_plate;

    #undef SWAPROOK
    #undef CAPTUREPIECE
}

void max_board_unmake_move(max_board_t *const board, max_move_t move) {
    board->ply -= 1;
    uint8_t side_to_move = board->ply & 1;
    max_piececode_t piece = board->pieces[move.to];
    max_sidestate_t *side = &board->sides[side_to_move];
    max_piecelist_t *positions = max_pieces_get_list(&side->piecelist, piece);

    max_lidx_t moved_index = side->index[move.to];

    #define UNCAPTURE(from) do {                                                                                    \
        max_sidestate_t *enemy = &board->sides[side_to_move ^ 1];                                                   \
        max_piececode_t captured = max_capturestack_pop(&board->captures);                                          \
        max_lidx_t captured_idx = max_piecelist_add(max_pieces_get_list(&enemy->piecelist, captured), (from));      \
        enemy->index[(from)] = captured_idx;                                                                        \
        board->pieces[(from)] = captured;                                                                           \
    } while(0)

    switch(move.attr) {
        case MAX_MOVE_NORMAL:
        case MAX_MOVE_DOUBLE: {
            board->pieces[move.to] = MAX_PIECECODE_EMPTY;
        } break;

        case MAX_MOVE_EN_PASSANT: {
            max_bidx_t takeat = max_bidx_inc(move.to, PAWN_INC[side_to_move ^ 1]);
            UNCAPTURE(takeat);
            board->pieces[move.to] = MAX_PIECECODE_EMPTY;
        } break;
        case MAX_MOVE_CAPTURE: {
            UNCAPTURE(move.to);
        } break;
    }

    board->pieces[move.from] = piece;
    side->index[move.from]  = moved_index;
    positions->pos[moved_index] = move.from;
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
            max_piececode_t sq = board->pieces[max_bidx_new(file, rank)];
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
