#include "max/board/movegen.h"
#include "max/board/dir.h"
#include "max/board/piececode.h"
#include "private/board/board.h"
#include "private/board/move.h"
#include "private/board/movegen.h"
#include "private/board/movegen/king.h"
#include "private/board/movegen/knight.h"
#include "private/board/movegen/pawn.h"
#include "private/board/movegen/slide.h"


bool max_board_movegen_attack(max_board_t *board, max_movelist_t *list, max_piecemask_t enemy, max_0x88_t from, max_0x88_t to) {
    if(max_0x88_valid(to)) {
        max_piececode_t piece = board->pieces[to.v];
        if(piece.v == MAX_PIECECODE_EMPTY) {
            max_movelist_add(list, max_smove_normal(from, to));
            return true;
        } else if(max_piececode_match(piece, enemy)) {
            max_movelist_add(list, max_smove_capture(from, to));
        }
    }

    return false;
}


void max_board_movegen(max_board_t *board, max_movelist_t *list) {
    max_side_t side = max_board_side(board);
    max_pieces_t *pieces = max_board_side_list(board, side);
    max_piecemask_t enemy = max_side_enemy_color_mask(side);

    max_board_movegen_pawns(board, list, pieces, enemy, side); 

    for(unsigned i = 0; i < pieces->knight.len; ++i) {
        max_0x88_t from = pieces->knight.loc[i];
        for(unsigned j = 0; j < MAX_KNIGHT_MOVES_LEN; ++j) {
            max_board_movegen_attack(board, list, enemy, from, max_0x88_move(from, MAX_KNIGHT_MOVES[j]));
        }
    }

    for(unsigned i = 0; i < pieces->bishop.len; ++i) {
        max_0x88_t from = pieces->bishop.loc[i];
        for(unsigned j = 0; j < MAX_0x88_DIAGONALS_LEN; ++j) {
            max_board_movegen_slide(board, list, enemy, from, MAX_0x88_DIAGONALS[j]);
        }
        
    }

    for(unsigned i = 0; i < pieces->rook.len; ++i) {
        max_0x88_t from = pieces->rook.loc[i];
        for(unsigned j = 0; j < MAX_0x88_CARDINALS_LEN; ++j) {
            max_board_movegen_slide(board, list, enemy, from, MAX_0x88_CARDINALS[j]);
        }
    }

    for(unsigned i = 0; i < pieces->queen.len; ++i) {
        max_0x88_t from = pieces->queen.loc[i];
        for(unsigned j = 0; j < MAX_0x88_RAYS_LEN; ++j) {
            max_board_movegen_slide(board, list, enemy, from, MAX_0x88_RAYS[j]);
        } 
    }
    

    max_0x88_t from = pieces->king.loc[0];
    for(unsigned i = 0; i < MAX_KING_MOVES_LEN; ++i) {
        max_board_movegen_attack(board, list, enemy, from, max_0x88_move(from, MAX_KING_MOVES[i]));
    }
}
