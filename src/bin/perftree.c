#include "max.h"
#include "max/board/board.h"
#include "max/board/fen.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/perft.h"
#include "max/board/zobrist.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define MOVEBUF_CAPACITY (1024)
#define USER_MOVEBUF_CAPACITY (64)

static void print_move(max_smove_t move) {
    printf("%c%c%c%c", MAX_0x88_FORMAT(move.from), MAX_0x88_FORMAT(move.to));
    if(max_movetag_is_promote(move.tag)) {
        char promote;
        switch(move.tag) {
            case MAX_MOVETAG_PKNIGHT: promote = 'n'; break;
            case MAX_MOVETAG_PBISHOP: promote = 'b'; break;
            case MAX_MOVETAG_PROOK:   promote = 'r'; break;
            case MAX_MOVETAG_PQUEEN:  promote = 'q'; break;
        }

        putchar(promote);
    }
}

static max_0x88_t parse_square(char const *sq) {
    uint8_t file = tolower(sq[0]) - 'a';
    uint8_t rank = sq[1] - '1';
    return max_0x88_new(rank, file);
}

static max_smove_t parse_move(char const **move) {
    max_0x88_t from = parse_square(*move);
    max_0x88_t to = parse_square((*move) + 2);
    char promote = (*move)[4];
    max_movetag_t tag = MAX_MOVETAG_NONE;
    switch(tolower(promote)) {
        case 'n': tag = MAX_MOVETAG_PKNIGHT; break;
        case 'b': tag = MAX_MOVETAG_PBISHOP; break;
        case 'r': tag = MAX_MOVETAG_PROOK;   break;
        case 'q': tag = MAX_MOVETAG_PQUEEN;  break;
    }

    if(tag != MAX_MOVETAG_NONE) {
        *move += 5;
    } else {
        *move += 4;
    }

    return max_smove_new(from, to, tag);
}

int main(int argc, char *argv[]) {
    static max_smove_t movebuf[MOVEBUF_CAPACITY];
    static max_smove_t user_movebuf[USER_MOVEBUF_CAPACITY];
    static max_state_t statebuf[10];
    max_init();

    max_board_t board;
    max_board_new(&board, statebuf, MAX_ZOBRIST_DEFAULT_SEED);

    if(argc != 4 && argc != 3) {
        printf("Invalid number of arguments\nusage %s depth \"fen\" \"moves\"\n", argv[0]);
        return -1;
    }
    
    char *end;
    uint8_t depth = strtoul(argv[1], &end, 10);
    
    max_fen_parse_err_t ec;
    if((ec = max_board_parse_from_fen(&board, argv[2])) != MAX_FEN_SUCCESS) {
        printf("Error parsing FEN string: %s\n", max_fen_parse_err_str(ec));
        return -1;
    }

    max_movelist_t user_moves;
    max_movelist_new(&user_moves, user_movebuf, MOVEBUF_CAPACITY);
    
    if(argc == 4) {
        const char *moves = argv[3];
        for(;;) {
            while(isspace(*moves)) {
                moves += 1;
            }

            if(*moves == '\0') {
                break;
            }

            max_smove_t move = parse_move(&moves);
            max_board_make_move(&board, move);
        }
    }
    
    max_board_movegen(&board, &user_moves);
    

    max_movelist_t moves;
    max_movelist_new(&moves, movebuf, MOVEBUF_CAPACITY);
    
    uint64_t count = 0;
    for(unsigned i = 0; i < user_moves.len; ++i) {
        max_smove_t move = user_moves.buf[i];

        if(max_board_legal(&board, move)) {
            max_board_make_move(&board, move);
            uint64_t nodes = max_board_perft(&board, moves, depth - 1);
            max_board_unmake_move(&board, move);
            
            print_move(move);
            printf(" %zu\n", nodes);

            count += nodes;
        }
    }

    printf("\n%zu\n\n", count);

    return 0;
}
