#include "max/square.h"
#include <stdlib.h>
#include <string.h>
#define MAX_CONSOLE
#include "max.h"
#include "max/move.h"
#include "test.h"
#include "max/board.h"


size_t board_recur(max_board_t *board, unsigned n) {
    size_t count = 0;
    if(n == 0) { return 1; }
    max_movelist_t moves;
    max_movelist_new(&moves);
    
    max_movegen(&moves, board);

    for(unsigned i = 0; i < moves.len; ++i) {
        max_board_t before;
        memcpy(&before, board, sizeof(before));
        max_board_make_move(board, moves.moves[i]);
        max_board_t after;
        memcpy(&after, board, sizeof(after));
        count += board_recur(board, n - 1);
        max_board_unmake_move(board, moves.moves[i]);
        if(memcmp(before.pieces, board->pieces, sizeof(board->pieces)) != 0) {
            max_board_debugprint(&before);
            max_board_debugprint(&after);
            max_board_debugprint(board);
            exit(-1);
        }
    }

    return count;
}


int board_tests(void) {
    max_board_t board;
    max_board_reset(&board);

    size_t count = board_recur(&board, 4);

    printf("%zu POSITIONS\n", count);
    
    for(;;) {
        max_movelist_t moves;
        max_movelist_new(&moves);
        max_movegen(&moves, &board);

        max_board_debugprint(&board);
        
        char ff, ft;
        unsigned rf, rt;
        scanf("%c%d%c%d", &ff, &rf, &ft, &rt);
        
        uint8_t from = max_bidx_new(ff - 'a', rf - 1);
        uint8_t to   = max_bidx_new(ft - 'a', rt - 1);


        for(unsigned i = 0; i < moves.len; ++i) {
            max_move_t move = moves.moves[i];
            printf("%c%d->%c%d\n", (move.from & 7) + 'a', (move.from >> 4) + 1, (move.to & 7) + 'a', (move.to >> 4) + 1);
            if(moves.moves[i].from == from && moves.moves[i].to == to) {
                max_board_make_move(&board, moves.moves[i]);
                break;
            }
        }
    }


    max_board_debugprint(&board);

    return 1;
}
