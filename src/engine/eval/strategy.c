#include "max/engine/eval/strategy.h"
#include "max/board/dir.h"
#include "max/board/movegen/pawn.h"
#include "max/board/piececode.h"


max_score_t max_engine_pawn_attacks_span(const max_board_t *board, max_0x88_t sq, max_side_t enemy) {
    const max_0x88_dir_t advance = MAX_PAWN_ADVANCE_DIR[enemy];
    const max_piececode_t enemy_pawn = max_piececode_new(enemy, MAX_PIECECODE_PAWN);
    
    max_score_t count = 0;

    for(;;) {
        sq = max_0x88_move(sq, advance);
        if(!max_0x88_valid(sq)) {
            break;
        }

        //No need to do validity checking for the moved square because returned piececode will always be MAX_PIECECODE_INVALID
        //if the moved position is now off the board
        if(board->pieces[max_0x88_move(sq, MAX_0x88_DIR_LEFT).v].v == enemy_pawn.v) {
            count += 1;
        }
        if(board->pieces[max_0x88_move(sq, MAX_0x88_DIR_RIGHT).v].v == enemy_pawn.v) {
            count += 1;
        }

        sq = max_0x88_move(sq, advance);
    }

    return count;
}
