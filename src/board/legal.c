#include "max/board/board.h"
#include "max/board/movegen.h"
#include "private/board/board.h"

bool max_board_legal(max_board_t *board, max_smove_t move) {
    static max_smove_t buf[512];
    max_movelist_t moves;
    max_movelist_new(&moves, buf, 512);
    max_board_make_move(board, move);

    max_board_movegen(board, &moves);
    for(unsigned i = 0; i < moves.len; ++i) {
        if(moves.buf[i].to.v == max_board_side_list(board, max_board_enemy_side(board))->king.loc->v) {
            max_board_unmake_move(board, move);
            return false;
        }
    }
    
    max_board_unmake_move(board, move);
    return true;
}
