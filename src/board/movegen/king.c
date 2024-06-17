#include "private/board/movegen/king.h"
#include "max/board/dir.h"
#include "max/board/move.h"
#include "max/board/side.h"
#include "max/board/squares.h"


max_0x88_dir_t *MAX_KING_MOVES = MAX_0x88_RAYS;

max_0x88_t MAX_CASTLE_KING_DEST[MAX_CASTLES_LEN][MAX_SIDES_LEN] = {
    [MAX_CASTLE_ASIDE] = {
        [MAX_SIDE_WHITE] = MAX_C1,
        [MAX_SIDE_BLACK] = MAX_C8,
    },
    [MAX_CASTLE_HSIDE] = {
        [MAX_SIDE_WHITE] = MAX_G1,
        [MAX_SIDE_BLACK] = MAX_G8,
    }
};

max_0x88_t MAX_CASTLE_ROOK_DEST[MAX_CASTLES_LEN][MAX_SIDES_LEN] = {
    [MAX_CASTLE_ASIDE] = {
        [MAX_SIDE_WHITE] = MAX_D1,
        [MAX_SIDE_BLACK] = MAX_D8,
    },
    [MAX_CASTLE_HSIDE] = {
        [MAX_SIDE_WHITE] = MAX_F1,
        [MAX_SIDE_BLACK] = MAX_F8,
    },
};


void max_board_movegen_castle(max_board_t *board, max_movelist_t *movelist, max_pieces_t *pieces, max_castle_side_t castle_side) {
    max_0x88_t kpos = *pieces->king.loc;
    max_0x88_t rook = pieces->initial_rook[castle_side];

    max_side_t color = max_board_side(board);
    max_0x88_t rdest = MAX_CASTLE_ROOK_DEST[castle_side][color];
    
    max_0x88_dir_t dir = max_0x88_line(rook, rdest);
    MAX_SANITY(dir != MAX_0x88_DIR_INVALID && "Rook on its starting square has no line to its castle square");

    max_0x88_t scan = rook;
    do {
        scan = max_0x88_move(scan, dir);
        if(scan.v != kpos.v && board->pieces[scan.v].v != MAX_PIECECODE_EMPTY) {
            return;
        }
    } while(scan.v != rdest.v);
    
    max_0x88_t kdest = MAX_CASTLE_KING_DEST[castle_side][color];
    dir = max_0x88_line(kpos, kdest);
    MAX_SANITY(dir != MAX_0x88_DIR_INVALID && "King on its starting square has no line to its castle square");
    scan = kpos;
    do {
        scan = max_0x88_move(scan, dir);
        if(scan.v != rook.v && board->pieces[scan.v].v != MAX_PIECECODE_EMPTY) {
            return;
        }
    } while(scan.v != kdest.v);

    max_movelist_add(movelist, max_smove_new(kpos, kdest, max_movetag_for_castle_side(castle_side)));
}
