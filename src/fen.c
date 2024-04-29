#include "max/board/board.h"
#include "max/board/piece.h"
#include <ctype.h>
#include <string.h>
#include "max/fen.h"
#include "board/private.h"

static bool max_board_parse_fen_piece(max_board_t *board, char code, uint8_t rank, uint8_t file) {
    if(!isalpha(code)) {
        return false;
    }

    max_piececode_t piece = 0;

    switch(tolower(code)) {
        case 'p': piece |= MAX_PIECECODE_PAWN;   break;
        case 'n': piece |= MAX_PIECECODE_KNIGHT; break;
        case 'b': piece |= MAX_PIECECODE_BISHOP; break;
        case 'r': piece |= MAX_PIECECODE_ROOK;   break;
        case 'q': piece |= MAX_PIECECODE_QUEEN;  break;
        case 'k': piece |= MAX_PIECECODE_KING;   break;
        default: return false;
    }
    
    max_pieces_t *side = NULL;
    if(isupper(code)) {
        piece |= MAX_PIECECODE_BLACK;
        side = &board->black;
    } else {
        piece |= MAX_PIECECODE_WHITE;
        side = &board->white;
    }

    max_board_add_piece(board, side, piece, max_bpos_new(file, rank));

    return true;
}

static int max_board_parse_fen_rank(max_board_t *board, char *rankstr, uint8_t rank, char **error) {
    uint8_t file = 0;
    uint8_t i  = 0;
    while(file < 8) {
        if(isdigit(rankstr[i])) {
            file += rankstr[i] - '1';
            i += 1;
            continue;
        }

        if(!max_board_parse_fen_piece(board, *rankstr, rank, file)) {
            *error = rankstr;
            return -1;
        }

        i += 1;
        file += 1;
    }

    return i;
}

bool max_board_parse_from_fen(max_board_t *board, char *str, char **error) {
    *error = NULL;
    max_board_clear(board);

    for(uint8_t rank = 0; rank < 8; ++rank) {
        int inc = 0;
        if((inc = max_board_parse_fen_rank(board, str, rank, error)) < 0) {
            return false;
        }

        str += inc;

        if(rank != 0 && rank != 7) {
            if(*str != '/') {
                *error = str;
                return false;
            }
            str += 1;
        }
    }

    str += 1;
    switch(*str) {
        case 'w': board->ply = 0; break;
        case 'b': board->ply = 1; break;
        default: *error = str; return false;
    }

    return true;
}
