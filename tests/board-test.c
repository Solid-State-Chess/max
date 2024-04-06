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

size_t perft(max_board_t *board, max_movelist_t moves, unsigned n) {
    size_t count = 0;
    if(n == 0) { return 1; }

    max_board_movegen_pseudo(board, &moves);
    
    size_t nmoves = 0;
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
        if((board->pieces[move.from] & MAX_PIECECODE_TYPE_MASK) == MAX_PIECECODE_BISHOP) {
            int8_t fx = move.from & 0x7;
            int8_t tx = move.to & 0x7;
            int8_t fy = move.from >> 4;
            int8_t ty = move.to >> 4;

            if(abs(fx - tx) != abs(fy - ty) || (move.from == 0x75 && move.to == 0x13)) {
                printf("BAD BISHOP %X->%X\n", move.from, move.to);
                exit(-1);
            }
        }

        max_board_t copy;
        memcpy(&copy, board, sizeof(copy));
        max_board_make_move(board, moves.moves[i]);

        count += perft(board, max_movelist_new(moves.moves + moves.len), n - 1);
        max_board_unmake_move(board, moves.moves[i]);
        
        /*if(!board_same(&copy, board)) {
            puts("NOT SAME");
            max_board_debugprint(&copy);
            max_board_debugprint(board);
            exit(-1);
        }*/
    }

    return count;
}


int board_tests(void) {
    max_move_t buf[1024];

    max_board_t board;
    max_board_new(&board);

    max_board_t original;
    memcpy(&original, &board, sizeof(max_board_t));
    
    max_movelist_t moves = max_movelist_new(buf);

    //ASSERT_EQ(size_t, perft(&board, moves, 2), 400, "%zu");
    //ASSERT_EQ(size_t, perft(&board, moves, 3), 8902, "%zu");
    size_t nodes = perft(&board, moves, 4);
    printf("%zu\nCAPTURE: %zu\nEP: %zu\nCHECK: %zu\n", nodes, CAPTURES, EP, CHECKS);

    exit(-1);

    //ASSERT_EQ(size_t, perft(&board, moves, 6), 119060324, "%zu");

    if(!board_same(&board, &original)) {
        puts("Move making / unmaking not good");
        max_board_debugprint(&board);
        return 1;
    }
    
    return 0;
}
