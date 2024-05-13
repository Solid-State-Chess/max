#include "max/board/board.h"
#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/movegen/pawn.h"
#include "max/board/squares.h"
#include "max/board/state.h"
#include "private/board/board.h"
#include "private/board/capturelist.h"
#include "private/board/movegen.h"
#include "private/board/movegen/king.h"
#include "private/board/state.h"


void max_board_make_move(max_board_t *board, max_smove_t move) {
    max_side_t side = max_board_side(board);
    max_side_t enemy_side = max_board_enemy_side(board);
    max_pieces_t *friendly = max_board_side_list(board, side);
    max_pieces_t *enemy    = max_board_side_list(board, enemy_side);

    MAX_SANITY_WITH(
        move.to.v != enemy->king.loc->v &&
        "Enemy king is captured by a move",
        {
            max_board_print(board);
        }
    );
    
    max_state_t *old_state = max_board_state(board);

    #ifdef MAX_ASSERTS_SANITY
    old_state->move = move;
    #endif

    max_state_t state = (max_state_t){
        .packed = old_state->packed,
        .check = {
            max_check_empty(),
            max_check_empty(),
        },
        .position = old_state->position
    };
    
    //Reset the en passant file from the previous packed state, but keep the castle rights
    state.packed = max_packed_state_set_epfile(state.packed, MAX_FILE_INVALID);
    //If the player moved their king, clear both castle rights bits
    //This also removes castle rights after castling even if the king did not move to castle,
    //supporting FIDE's chess960 rules
    if(move.from.v == friendly->king.loc->v) {
        state.packed &= ~(max_packed_state_hcastle(side) | max_packed_state_acastle(side));
    } else if(move.from.v == friendly->initial_rook[MAX_CASTLE_ASIDE].v) {
        state.packed &= ~max_packed_state_acastle(side);
    } else if(move.from.v == friendly->initial_rook[MAX_CASTLE_HSIDE].v) {
        state.packed &= ~max_packed_state_hcastle(side);
    }
    
    //Shuffle the pieces as specified in the move
    if(move.tag & MAX_MOVETAG_CAPTURE) {
        max_piececode_t piece = max_board_remove_piece_from_side(board, enemy, move.to);
        MAX_SANITY(piece.v != MAX_PIECECODE_EMPTY && "Captured piece is empty");
        max_captures_add(&board->captures, piece);

        if(move.to.v == enemy->initial_rook[MAX_CASTLE_ASIDE].v) {
            state.packed &= ~max_packed_state_acastle(enemy_side);
        } else if(move.to.v == enemy->initial_rook[MAX_CASTLE_HSIDE].v) {
            state.packed &= ~max_packed_state_hcastle(enemy_side);
        }
    } else {
        MAX_SANITY(board->pieces[move.to.v].v == MAX_PIECECODE_EMPTY);
    }

    max_state_stack_push(&board->stack, state);

    switch(move.tag & ~MAX_MOVETAG_CAPTURE) {
        case MAX_MOVETAG_NONE: {
            max_board_move_piece_from_side(board, friendly, move.from, move.to);
        } break;
        // Update the en passant file
        case MAX_MOVETAG_DOUBLE: {
            max_board_state(board)->packed = max_packed_state_set_epfile(max_board_state(board)->packed, max_0x88_file(move.from));
            max_board_move_piece_from_side(board, friendly, move.from, move.to);
        } break;

        case MAX_MOVETAG_ENPASSANT: {
            //Shift the destination 'down' relative to the side that is moving to get the captured pawn's position
            max_0x88_t captured_pos = max_0x88_move(move.to, -MAX_PAWN_ADVANCE_DIR[side]);
            MAX_SANITY(max_0x88_valid(captured_pos) && "En passant capture square is not valid");

            max_piececode_t captured = max_board_remove_piece_from_side(board, enemy, captured_pos);
            
            //Ensure that the captured piece is actually an enemy pawn
            MAX_SANITY_WITH(
                captured.v == max_piececode_new(
                    max_board_enemy_side(board),
                    MAX_PIECECODE_PAWN
                ).v &&
                "Piece captured en passant is not an enemy pawn",
                { max_board_print(board); }
            );

            max_captures_add(&board->captures, captured);

            max_board_move_piece_from_side(board, friendly, move.from, move.to);
        } break;

        case MAX_MOVETAG_ACASTLE:
        case MAX_MOVETAG_HCASTLE: {
            max_castle_side_t castle = max_castle_side_for_movetag(move.tag);
            MAX_SANITY_WITH(
                board->pieces[friendly->initial_rook[castle].v].v ==
                max_piececode_new(side, MAX_PIECECODE_ROOK).v &&
                "Friendly rook is not on required square for castling",
                {
                    max_board_print(board);
                }
            );

            max_board_move_piece_from_side(
                board,
                friendly,
                friendly->initial_rook[castle],
                MAX_CASTLE_ROOK_DEST[castle][side]
            );

            max_board_move_piece_from_side(board, friendly, move.from, move.to);
        } break;
        
        //Only MAX_MOVETAG_P* promotion moves
        default: {
            MAX_SANITY(max_movetag_is_promote(move.tag));
            max_piececode_t promoted = max_piececode_for_movetag_promote(move.tag, side);
            max_board_add_piece_to_side(
                board,
                friendly,
                move.to,
                promoted
            );

            max_board_remove_piece_from_side(
                board,
                friendly,
                move.from
            );
        } break;
    }
    

    board->ply += 1;

    //Increment the ply to indicate that the other side is now to move 
    max_board_update_check(board, move);
}
