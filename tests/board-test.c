#include "max.h"
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
static size_t CAPTURES = 0;
static size_t EP = 0;

size_t perft(max_board_t *board, max_movelist_t moves, max_move_t *history, unsigned n) {
    size_t count = 0;
    max_bpos_t kpos = max_board_king_pos(board);
    if(board->sides[0].king.len == 0 || board->sides[1].king.len == 0) {
        puts("BAD BOARD");
        max_board_debugprint(board);
        for(unsigned i = 8; i > n; --i) {
            printf(
                "%c%c%c%c\n",
                MAX_BPOS_FORMAT(history[i - 1].from),
                MAX_BPOS_FORMAT(history[i - 1].to)
            );
        }
        exit(-1);
    }

    if(n == 0) { return 1; }

    max_board_movegen_pseudo(board, &moves);
    
    for(unsigned i = 0; i < moves.len; ++i) {
        if(!max_board_move_is_valid(board, moves.moves[i])) {
            continue;
        }

        if(board->pieces[moves.moves[i].to] != MAX_PIECECODE_EMPTY) {
            CAPTURES += 1;
        }

        if(moves.moves[i].attr == MAX_MOVE_EN_PASSANT) {
            EP += 1;
        }

        max_move_t move = moves.moves[i];
        max_board_t copy;
        memcpy(&copy, board, sizeof(copy));
        max_board_make_move(board, move);
        
        history[n] = move;
        count += perft(board, max_movelist_new(moves.moves + moves.len), history, n - 1);
        max_board_unmake_move(board, move);

        if(!board_same(&copy, board)) {
            puts("Move unmaking is invalid");
            max_board_debugprint(board);
            for(unsigned i = 8; i > n; --i) {
                printf(
                    "%c%c%c%c\n",
                    MAX_BPOS_FORMAT(history[i - 1].from),
                    MAX_BPOS_FORMAT(history[i - 1].to)
                );
            }
            exit(-1);

        }
    }

    return count;
}


int board_tests(void) {
    max_init_statics();

    max_move_t buf[2056];

    max_board_t board;
    max_board_new(&board);

    max_board_t original;
    memcpy(&original, &board, sizeof(max_board_t));
    
    max_movelist_t moves = max_movelist_new(buf);
    max_move_t history[10];


    //ASSERT_EQ(size_t, perft(&board, moves, history, 2), 400, "%zu");
    //ASSERT_EQ(size_t, perft(&board, moves, history, 3), 8902, "%zu");
    /*ASSERT_EQ(size_t, perft(&board, moves, history, 4), 197281, "%zu");
    ASSERT_EQ(size_t, perft(&board, moves, history, 5), 4865609  , "%zu");
    ASSERT_EQ(size_t, perft(&board, moves, history, 6), 119060324, "%zu");
    ASSERT_EQ(size_t, perft(&board, moves, history, 7), 3195901860 , "%zu");*/
    
    //max_board_make_move(&board, max_move_new(MAX_E2, MAX_E4, MAX_MOVE_DOUBLE));

    size_t nodes = perft(&board, moves, history, 8);
    printf("%zu\nCAPTURE: %zu\nEP: %zu\nCHECK: %zu\n", nodes, CAPTURES, EP, CHECKS);

    if(!board_same(&board, &original)) {
        puts("Move making / unmaking not good");
        max_board_debugprint(&board);
        return 1;
    }
    
    return 0;
}
