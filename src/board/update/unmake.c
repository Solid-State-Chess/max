#include "max/board/move.h"
#include "max/board/movegen.h"
#include "private/board/board.h"
#include "private/board/capturelist.h"
#include "private/board/movegen/king.h"
#include "private/board/movegen/pawn.h"
#include "private/board/state.h"


void max_board_unmake_move(max_board_t *board, max_smove_t move) {
    //Subtract from the game ply to affect which side is considered to-play
    board->ply -= 1;

    max_side_t side = max_board_side(board);
    max_pieces_t *friendly  = max_board_side_list(board, side);
    max_pieces_t *enemy = max_board_side_list(board, max_board_enemy_side(board));

    switch(move.tag & ~MAX_MOVETAG_CAPTURE) {
        case MAX_MOVETAG_NONE:
        case MAX_MOVETAG_DOUBLE: {
            //Move the piece back to its origin square (we need to take this into consideration when undoing promotions)
            max_board_move_piece_from_side(board, friendly, move.to, move.from);
        } break;

        case MAX_MOVETAG_ENPASSANT: {
            //Shift the en passant capture 'down' relative to the side to move to get the square that the captured
            //pawn must have been at
            max_0x88_t original_pos = max_0x88_move(move.to, -MAX_PAWN_ADVANCE_DIR[side]);

            max_piececode_t captured = max_captures_pop(&board->captures);
            max_board_add_piece_to_side(board, enemy, original_pos, captured);

            max_board_move_piece_from_side(board, friendly, move.to, move.from);
        } break;

        case MAX_MOVETAG_ACASTLE:
        case MAX_MOVETAG_HCASTLE: {
            max_castle_side_t castle = max_castle_side_for_movetag(move.tag);
            MAX_SANITY(
                board->pieces[MAX_CASTLE_ROOK_DEST[castle][side].v].v ==
                max_piececode_new(max_piececode_color_for_side(side), MAX_PIECECODE_ROOK).v
                &&
                "Friendly rook not on destination square when unmaking castle move"
            );
            max_board_move_piece_from_side(
                board,
                friendly,
                MAX_CASTLE_ROOK_DEST[castle][side],
                friendly->initial_rook[castle]
            );

            max_board_move_piece_from_side(board, friendly, move.to, move.from);
        } break;
        
        default: {
            MAX_SANITY(max_movetag_is_promote(move.tag));
            max_board_remove_piece_from_side(
                board,
                friendly,
                move.to
            );

            max_board_add_piece_to_side(
                board,
                friendly,
                move.from,
                max_piececode_new(side, MAX_PIECECODE_PAWN)
            );
        } break;
    }

    //Re-add any captured piece
    //This does nothing for en passant because EP captured pieces are NOT on the destination square
    if(move.tag & MAX_MOVETAG_CAPTURE) {
        max_piececode_t captured = max_captures_pop(&board->captures);
        max_board_add_piece_to_side(board, enemy, move.to, captured);
    }

    //Pop from the state stack last because the prior operations may have modified the zobrist key
    max_state_stack_pop(&board->stack);
}
