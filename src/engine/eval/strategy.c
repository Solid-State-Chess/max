#include "max/engine/eval/strategy.h"
#include "max/board/dir.h"
#include "max/board/movegen/pawn.h"
#include "max/board/piececode.h"
#include "max/board/zobrist.h"


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

#include <stdio.h>

max_score_t max_engine_outpost(max_score_t outpost_bonus, const max_board_t *board, max_0x88_t sq, max_side_t side) {
    const max_side_t enemy = max_side_enemy(side);
    uint8_t rank = max_0x88_rank(sq);
    if(rank < 3 || rank > 6) {
        return 0;
    }
    
    const max_piececode_t friendly_pawn = max_piececode_new(side, MAX_PIECECODE_PAWN);
    const max_0x88_t protector_pawn = max_0x88_move(sq, MAX_PAWN_ADVANCE_DIR[enemy]);

    //No need for bounds checks as invalid indices wil return MAX_PIECECODE_INVALID
    if(
        board->pieces[max_0x88_move(protector_pawn, MAX_0x88_DIR_LEFT).v].v != friendly_pawn.v &&
        board->pieces[max_0x88_move(protector_pawn, MAX_0x88_DIR_RIGHT).v].v != friendly_pawn.v
    ) {
        printf("No protector %0x\n", board->pieces[max_0x88_move(protector_pawn, MAX_0x88_DIR_RIGHT).v].v);
        return 0;
    }

    return outpost_bonus >> (max_engine_pawn_attacks_span(board, sq, enemy));
}

#ifdef MAX_TESTS
#include "private/test.h"
#include "max/board/fen.h"

void max_engine_strategic_eval_tests(void) {
    static const max_score_t OUTPOST_BONUS = 1000;
    max_state_t buf[10];
    max_board_t board;
    max_board_new(&board, buf, MAX_ZOBRIST_DEFAULT_SEED);
    
    ASSERT(max_board_parse_from_fen(&board, "8/8/8/pr6/Np6/1P6/8/8 w - -") == MAX_FEN_SUCCESS, "");
    
    MAX_TEST_ASSERT_WITH(
        max_engine_outpost(OUTPOST_BONUS, &board, max_0x88_new(3, 0), MAX_SIDE_WHITE) == OUTPOST_BONUS,
        {
            printf("A4 not marked as an outpost square\n");
            max_board_print(&board);
        }
    );
}

#endif
