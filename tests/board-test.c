#include "max.h"
#include "max/board/piece.h"
#include "max/board/square.h"
#include <stdlib.h>
#include <string.h>
#define MAX_CONSOLE
#include "max/board/move.h"
#include "test.h"
#include "max/board/board.h"


static bool board_same(max_board_t *a, max_board_t *b) {
    if(memcmp(a->pieces, b->pieces, sizeof(a->pieces)) != 0) {
        return false;
    }

    static max_piececode_t PIECES[] = {
        MAX_PIECECODE_PAWN,
        MAX_PIECECODE_KNIGHT,
        MAX_PIECECODE_BISHOP,
        MAX_PIECECODE_ROOK,
        MAX_PIECECODE_QUEEN,
        MAX_PIECECODE_KING
    };
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
    
    max_irreversible_t *a_state = max_board_state(a);
    max_irreversible_t *b_state = max_board_state(b);

    if(max_checker_is_valid(a_state->check.attacks[0])) {
        if(!max_checker_is_valid(b_state->check.attacks[0])) {
            puts("BAD CHECK ROLLBACK");
            return false;
        }
    }
    
    #define VALID_CHECK_ROLLBACK(n, msg)                                                            \
        if(max_checker_is_valid(a_state->check.attacks[(n)])) {                                           \
            if(                                                                                     \
                !max_checker_is_valid(b_state->check.attacks[(n)])                                        \
            ) {                                                                                     \
                puts("BAD " #msg " CHECK ROLLBACK");                                                \
                return false;                                                                       \
            }                                                                                       \
        }

    VALID_CHECK_ROLLBACK(0, "SINGLE")
    VALID_CHECK_ROLLBACK(1, "DOUBLE")



    return true;
}

#define SEARCH_PLY (7)

size_t perft(max_board_t *board, max_movelist_t moves, max_move_t *history, unsigned n) {
    size_t count = 0;
    max_bpos_t kpos = max_board_king_pos(board);
    if(board->sides[0].king.len == 0 || board->sides[1].king.len == 0) {
        puts("BAD BOARD");
        max_board_debugprint(board);
        for(unsigned i = 0; i <= SEARCH_PLY; ++i) {
            printf(
                "%c%c%c%c\n",
                MAX_BPOS_FORMAT((history - i)->from),
                MAX_BPOS_FORMAT((history - i)->to)
            );
        }
        exit(-1);
    }

    switch(n) {
        case 0: {
            return 1;
        } break;
        case 1: {
            size_t count = 0;
            max_board_movegen_pseudo(board, &moves);
            for(unsigned i = 0; i < moves.len; ++i) {
                if(max_board_move_is_valid(board, moves.moves[i])) {
                    count += 1;
                }
            }

            return count;
        } break;
        default: {
            max_board_movegen_pseudo(board, &moves);
    
            for(unsigned i = 0; i < moves.len; ++i) {
                if(!max_board_move_is_valid(board, moves.moves[i])) {
                    continue;
                }

                max_move_t move = moves.moves[i];
                /*max_board_t copy;
                memcpy(&copy, board, sizeof(copy));*/
                max_board_make_move(board, move);

                *history = move;
                count += perft(board, max_movelist_new(moves.moves + moves.len), history + 1, n - 1);
                max_board_unmake_move(board, move);

                /*if(!board_same(&copy, board)) {
                    puts("Move unmaking is invalid");
                    puts("====BEFORE====");
                    max_board_debugprint(&copy);
                    
                    puts("====AFTER====");
                    max_board_debugprint(board);

                    puts("====MOVES====");
                    for(unsigned i = 0; i < 5 - n; ++i) {
                        printf(
                            "%c%c%c%c\n",
                            MAX_BPOS_FORMAT(history[i].from),
                            MAX_BPOS_FORMAT(history[i].to)
                        );
                    }
                    exit(-1);

                }*/
            }

            /*if(count == 0) {
                max_board_debugprint(board);
            }*/

            return count;

        } break;
    }
}


int board_tests(void) {
    max_irreversible_t stack[16];
    max_init_statics();

    max_move_t buf[2056];

    max_board_t board;
    max_board_new(&board, stack);

    max_board_t original;
    memcpy(&original, &board, sizeof(max_board_t));

    //max_board_debugprint(&board);
    
    max_movelist_t moves = max_movelist_new(buf);
    max_move_t history[10];


    ASSERT_EQ(size_t, perft(&board, moves, history, 2), 400, "%zu");
    ASSERT_EQ(size_t, perft(&board, moves, history, 3), 8902, "%zu");
    ASSERT_EQ(size_t, perft(&board, moves, history, 4), 197281, "%zu");
    ASSERT_EQ(size_t, perft(&board, moves, history, 5), 4865609  , "%zu");
    ASSERT_EQ(size_t, perft(&board, moves, history, 6), 119060324, "%zu");
    ASSERT_EQ(size_t, perft(&board, moves, history, 7), 3195901860 , "%zu");
    
    //max_board_make_move(&board, max_move_new(MAX_E2, MAX_E4, MAX_MOVE_DOUBLE));
    
    /*time_t begin = time(NULL);

    size_t nodes = perft(&board, moves, history, SEARCH_PLY);
    
    time_t end = time(NULL);
    printf("%zu N\n%zu s\n", nodes, end - begin);*/

    if(!board_same(&board, &original)) {
        puts("Move making / unmaking not good");
        max_board_debugprint(&board);
        return 1;
    }
    
    return 0;
}
