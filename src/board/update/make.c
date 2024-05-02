#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/state.h"
#include "private/board/board.h"
#include "private/board/capturelist.h"
#include "private/board/movegen/pawn.h"
#include "private/board/state.h"


void max_board_make_move(max_board_t *board, max_smove_t move) {
    max_side_t side = max_board_side(board);
    max_pieces_t *friendly = max_board_side_list(board, side);
    max_pieces_t *enemy    = max_board_side_list(board, max_board_enemy_side(board));
    
    max_state_t *old_state = max_board_state(board);
    max_state_t state = (max_state_t){
        .packed = old_state->packed,
        .check = {
            max_check_empty(),
            max_check_empty(),
        },
        .position = old_state->position
    };
    
    //Reset the en passant file from before
    state.packed |= MAX_PSTATE_EPFILE_INVALID;
    max_state_stack_push(&board->stack, state);
    
    //Shuffle the pieces as specified in the move
    if(move.tag & MAX_MOVETAG_CAPTURE) {
        max_piececode_t piece = max_board_remove_piece_from_side(board, enemy, move.to);
        max_captures_add(&board->captures, piece);
    }

    switch(move.tag & ~MAX_MOVETAG_CAPTURE) {
        case MAX_MOVETAG_NONE: break;
        // Update the en passant file
        case MAX_MOVETAG_DOUBLE: {
            max_board_state(board)->packed = max_packed_state_set_epfile(max_board_state(board)->packed, max_0x88_file(move.from));
        } break;

        case MAX_MOVETAG_ENPASSANT: {
            //Shift the destination 'down' relative to the side that is moving to get the captured pawn's position
            max_0x88_t captured_pos = max_0x88_move(move.to, -MAX_PAWN_ADVANCE_DIR[side]);
            max_piececode_t captured = max_board_remove_piece_from_side(board, enemy, captured_pos);

            MAX_ASSERT(captured.v != MAX_PIECECODE_EMPTY);

            max_captures_add(&board->captures, captured);
        } break;
    }

    max_board_move_piece_from_side(board, friendly, move.from, move.to);

    //Increment the ply to indicate that the other side is now to move 
    board->ply += 1;
}
