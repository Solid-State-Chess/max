#include "private/board/movegen/pawn.h"
#include "max/board/dir.h"
#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/board/squares.h"
#include "max/board/state.h"
#include "private/board/board.h"
#include "private/board/move.h"


max_0x88_dir_t MAX_PAWN_ATTACK_SIDES[MAX_PAWN_ATTACK_SIDE_LEN] = {
    MAX_0x88_DIR_RIGHT,
    MAX_0x88_DIR_LEFT,
};

max_0x88_dir_t MAX_PAWN_ADVANCE_DIR[MAX_SIDES_LEN] = {
    MAX_0x88_DIR_UP,
    MAX_0x88_DIR_DOWN,
};

uint8_t MAX_PAWN_PROMOTE_RANK[MAX_SIDES_LEN] = {
};

uint8_t MAX_PAWN_EP_RANK[MAX_SIDES_LEN] = {
};

/// Helper to add all four possible promotions to the given movelist with the given tag (meant for capture)
static void max_board_movegen_pawn_promotions(max_movelist_t *list, max_0x88_t from, max_0x88_t to, max_movetag_t tag) {
    max_movelist_add(list, max_smove_new(from, to, tag | MAX_MOVETAG_PQUEEN));
    max_movelist_add(list, max_smove_new(from, to, tag | MAX_MOVETAG_PKNIGHT));
    max_movelist_add(list, max_smove_new(from, to, tag | MAX_MOVETAG_PROOK));
    max_movelist_add(list, max_smove_new(from, to, tag | MAX_MOVETAG_PBISHOP));
}

/// Attack a square on the promotion rank and add promotion + capture moves
static void max_board_movegen_pawn_attack_promote(max_board_t *board, max_movelist_t *list, max_piecemask_t enemy, max_0x88_t from, max_0x88_t to) {
    if(max_0x88_valid(to) && max_piececode_match(board->pieces[to.v], enemy)) {
        max_board_movegen_pawn_promotions(list, from, to, MAX_MOVETAG_CAPTURE); 
    }
}

/// Make a pawn attack from the given square out towards the given square - only generates attack moves and does not allow quiet moves
static void max_board_movegen_pawn_attack(max_board_t *board, max_movelist_t *list, max_piecemask_t enemy, max_0x88_t from, max_0x88_t to) {
    if(max_0x88_valid(to) && max_piececode_match(board->pieces[to.v], enemy)) {
        max_movelist_add(list, max_smove_capture(from, to));
    }
}

void max_board_movegen_pawns(max_board_t *board, max_movelist_t *list, max_pieces_t *pieces, max_piecemask_t enemy, max_side_t side) {
    max_0x88_dir_t const advance = MAX_PAWN_ADVANCE_DIR[side];
    uint8_t const promote_rank = MAX_PAWN_PROMOTE_RANK[side];
    uint8_t const en_passant_rank = MAX_PAWN_EP_RANK[side];

    max_state_t *state = max_board_state(board);
    bool has_ep = max_packed_state_has_ep(state->state);

    for(unsigned i = 0; i < pieces->pawn.len; ++i) {
        max_0x88_t from = pieces->pawn.loc[i];
        max_0x88_t advanced_from = max_0x88_move(from, MAX_PAWN_ADVANCE_DIR[side]);
        if(max_0x88_rank(advanced_from) == promote_rank) {
            if(board->pieces[advanced_from.v].v == MAX_PIECECODE_EMPTY) {
                max_board_movegen_pawn_promotions(list, from, advanced_from, MAX_MOVETAG_NONE);
            }
            max_board_movegen_pawn_attack_promote(board, list, enemy, from, max_0x88_move(advanced_from, MAX_PAWN_ATTACK_SIDES[0])); 
            max_board_movegen_pawn_attack_promote(board, list, enemy, from, max_0x88_move(advanced_from, MAX_PAWN_ATTACK_SIDES[1]));
        } else {
            if(board->pieces[advanced_from.v].v == MAX_PIECECODE_EMPTY) {
                max_movelist_add(list, max_smove_normal(from, advanced_from));
            }
            max_board_movegen_pawn_attack(board, list, enemy, from, max_0x88_move(advanced_from, MAX_PAWN_ATTACK_SIDES[0])); 
            max_board_movegen_pawn_attack(board, list, enemy, from, max_0x88_move(advanced_from, MAX_PAWN_ATTACK_SIDES[1]));
        }
        
        if(has_ep && max_0x88_rank(from) == en_passant_rank) {
            uint8_t filediff = (7 + max_0x88_file(from)) - max_packed_state_epfile(state->state);
            if(filediff == 6 || filediff == 8) {

                max_0x88_t epsquare = max_0x88_new(en_passant_rank, max_packed_state_epfile(state->state));
                epsquare = max_0x88_move(epsquare, advance);
                max_movelist_add(list, max_smove_new(from, epsquare, MAX_MOVETAG_ENPASSANT));
            }
        }
    }
}
