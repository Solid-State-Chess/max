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
                    printf("NO MATCH FOR %x\n", al->pos[k]);
                    //return false;
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

size_t perft(max_board_t *board, unsigned n, max_movelist_t *stack) {
    size_t count = 0;
    if(n == 0) { return 1; }
    max_movelist_t moves;
    max_movelist_new(&moves);
    
    max_board_movegen_pseudo(board, &moves);
    
    size_t nmoves = 0;
    for(unsigned i = 0; i < moves.len; ++i) {
        for(unsigned j = 0; j < moves.len; ++j) {
            if(i != j && moves.moves[i].from == moves.moves[j].from && moves.moves[i].to == moves.moves[j].to) {
                max_move_t move1 = moves.moves[i];
                printf("%c%d->%c%d\n", (move1.from & 7) + 'a', (move1.from >> 4) + 1, (move1.to & 7) + 'a', (move1.to >> 4) + 1);
                printf("INVALID %u %u AT N=%u\n", i, j, n);
                
                for(unsigned k = 0; k < stack->len; ++k) {
                    max_move_t move = stack->moves[k];
                    printf("%c%d->%c%d\n", (move.from & 7) + 'a', (move.from >> 4) + 1, (move.to & 7) + 'a', (move.to >> 4) + 1);
                }
                max_board_debugprint(board);
                exit(-1);
            }
        }
        if(!max_board_move_is_valid(board, moves.moves[i])) {
            continue;
        }
        nmoves += 1;
        max_board_t before;
        memcpy(&before, board, sizeof(before));
        max_board_make_move(board, moves.moves[i]);
        max_board_t after;
        memcpy(&after, board, sizeof(after));
        max_movelist_add(stack, moves.moves[i]);
        count += perft(board, n - 1, stack);
        stack->len -= 1;

        max_board_unmake_move(board, moves.moves[i]);
        if(!board_same(&before, board)) {
            max_board_debugprint(&before);
            max_board_debugprint(&after);
            max_board_debugprint(board);
            exit(-1);
        }
    }

    if(nmoves == 0) {
        return 1;
    }

    return count;
}


int board_tests(void) {
    max_board_t board;
    max_board_reset(&board);

    printf("WHITE %p\nBLACK %p\n", &board.white, &board.black);
    
    max_movelist_t list;
    max_movelist_new(&list);
    size_t count = perft(&board, 5, &list);

    printf("%zu POSITIONS\n", count);
    
    for(;;) {
        max_movelist_t moves;
        max_movelist_new(&moves);

        max_board_movegen_pseudo(&board, &moves);

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
