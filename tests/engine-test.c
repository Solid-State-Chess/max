#include <stdio.h>
#include <string.h>
#define MAX_CONSOLE
#define MAX_DEBUG
#include "max/board.h"
#include "max/engine.h"
#include "test.h"
#include <stdlib.h>

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


int engine_tests(void) {
    max_engine_t *engine = malloc(sizeof(*engine));
    max_engine_new(engine);
    
    max_move_t buf[256];

    for(;;) {
        max_board_t prev;
        memcpy(&prev, &engine->board, sizeof(prev));

        max_searchresult_t search;
        max_engine_search(engine, &search, 5);

        /*if(!board_same(&prev, &engine->board)) {
            exit(-1);
        }*/

        printf(
            "%c%d->%c%d @ %d - %'u nodes\n",
            (search.bestmove.from & 7) + 'a',
            (search.bestmove.from >> 4) + 1,
            (search.bestmove.to & 7) + 'a',
            (search.bestmove.to >> 4) + 1,
            search.best_score,
            engine->search.nodes
        );

        max_board_make_move(&engine->board, search.bestmove);

        max_board_debugprint(&engine->board);
        
        for(;;) {
            char ff, tf;
            char fr, tr;
            int c;

            while ((c = getchar()) != '\n' && c != EOF) {}
            puts("MOVE: ");
            scanf("%c%c%c%c", &ff, &fr, &tf, &tr);

            max_bpos_t from = max_bpos_new(ff - 'a', fr - '1');
            max_bpos_t to   = max_bpos_new(tf - 'a', tr - '1');
            
            max_movelist_t moves = max_movelist_new(buf);
            memset(buf, 0, sizeof(buf));
            max_board_movegen_pseudo(&engine->board, &moves);
            
            bool moved = false;
            for(unsigned i = 0; i < moves.len; ++i) {
                if(moves.moves[i].from == from && moves.moves[i].to == to) {
                    if(max_board_move_is_valid(&engine->board, moves.moves[i])) {
                        max_board_make_move(&engine->board, moves.moves[i]);
                        moved = true;
                        break;
                    }
                }
            }

            if(moved) {
                break;
            }
        }
    }

    return 1;
}
