#include "private/board/movegen/pawn.h"
#include "max/board/dir.h"
#include "private/board/move.h"


max_0x88_dir_t MAX_PAWN_ATTACK_SIDES[MAX_PAWN_ATTACK_SIDE_LEN] = {
    MAX_0x88_DIR_RIGHT,
    MAX_0x88_DIR_LEFT,
};

max_0x88_dir_t MAX_PAWN_ADVANCE_DIR[MAX_SIDES_LEN] = {
    MAX_0x88_DIR_UP,
    MAX_0x88_DIR_DOWN,
};

/// Make a pawn attack from the given square out towards the given square - only generates attack moves and does not allow quiet moves
static void max_board_movegen_pawn_attack(max_board_t *board, max_movelist_t *list, max_piecemask_t enemy, max_0x88_t from, max_0x88_t to) {
    if(max_0x88_valid(to) && max_piececode_match(board->pieces[to.v], enemy)) {
        max_movelist_add(list, max_smove_capture(from, to));
    }
}

void max_board_movegen_pawns(max_board_t *board, max_movelist_t *list, max_pieces_t *pieces, max_piecemask_t enemy, max_side_t side) {
    max_0x88_dir_t advance = MAX_PAWN_ADVANCE_DIR[side];
    for(unsigned i = 0; i < pieces->pawn.len; ++i) {
        max_0x88_t from = pieces->pawn.loc[i];
        max_0x88_t advanced_from = max_0x88_move(from, MAX_PAWN_ADVANCE_DIR[side]);
        

        max_board_movegen_pawn_attack(board, list, enemy, from, max_0x88_move(advanced_from, MAX_PAWN_ATTACK_SIDES[0])); 
        max_board_movegen_pawn_attack(board, list, enemy, from, max_0x88_move(advanced_from, MAX_PAWN_ATTACK_SIDES[1])); 
    }
}
