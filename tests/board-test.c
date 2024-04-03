#include "max/piece.h"
#include "max/square.h"
#include <stdlib.h>
#include <string.h>
#define MAX_CONSOLE
#include "max/move.h"
#include "test.h"
#include "max/board.h"

static bool board_same(max_board_t *a, max_board_t *b) {
    if(memcmp(a->pieces, b->pieces, sizeof(a->pieces)) != 0) {
        return false;
    }

    static max_piececode_t PIECES[] = {MAX_PIECECODE_PAWN, MAX_PIECECODE_KNIGHT, MAX_PIECECODE_BISHOP, MAX_PIECECODE_ROOK, MAX_PIECECODE_QUEEN, MAX_PIECECODE_KING};
    for(unsigned i = 0; i <= 1; ++i) {
        for(unsigned j = 0; j < sizeof(PIECES); ++j) {
            max_piecelist_t *al = max_pieces_get_list(&a->sides[i], PIECES[j]);
            max_piecelist_t *bl = max_pieces_get_list(&b->sides[i], PIECES[j]);
            for(unsigned k = 0; k < al->len; ++k) {
                bool match = false;
                for(unsigned l = 0; l < bl->len; ++l) {
                    if(al->pos[k] == bl->pos[l]) { match = true; break; }
                }
                if(!match) {
                    return false;
                }
            }
        }
    }
    
    if(a->captures.head != b->captures.head || memcmp(a->captures.captures, b->captures.captures, a->captures.head) != 0) {
        printf("%u to %u\n", a->captures.head, b->captures.head);
        return false;
    }



    return true;
}

static size_t CHECKS = 0;

size_t perft(max_board_t *board, unsigned n) {
    size_t count = 0;
    if(n == 0) { return 1; }

    max_movelist_t moves;
    max_movelist_new(&moves);
    
    max_board_movegen_pseudo(board, &moves);
    
    size_t nmoves = 0;
    for(unsigned i = 0; i < moves.len; ++i) {
        if(!max_board_move_is_valid(board, moves.moves[i])) {
            continue;
        }
        max_board_make_move(board, moves.moves[i]);
        count += perft(board, n - 1);
        max_board_unmake_move(board, moves.moves[i]);
    }

    return count;
}


int board_tests(void) {
    max_board_t board;
    max_board_reset(&board);
    
    ASSERT_EQ(size_t, perft(&board, 2), 400, "%zu");
    ASSERT_EQ(size_t, perft(&board, 3), 8902, "%zu");
    ASSERT_EQ(size_t, perft(&board, 5), 4865609, "%zu");
    
    return 0;
}
