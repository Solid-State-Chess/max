#include "max/board/fen.h"
#include "max/board/board.h"
#include "max/board/piececode.h"
#include "max/board/side.h"
#include "max/board/squares.h"
#include "max/board/state.h"
#include "private/board/board.h"
#include "private/board/state.h"
#include <ctype.h>
#include <stdint.h>

/// Tagged union type returned by parser functions to indicate that parsing can continue
/// or has failed
typedef struct {
    bool ok;
    union {
        max_fen_parse_err_t err;
        char const *end;
    };
} max_fen_parse_result_t;

static max_fen_parse_result_t max_board_parse_fen_rank(max_board_t *board, char const *fen, uint8_t rank) {
    uint8_t file = 0;
    for(;;) {
        char c = *fen;
        if(isdigit(c)) {
            file += (c - '0');
            if(file > 8) {
                return (max_fen_parse_result_t){
                    .ok = false,
                    .err = MAX_FEN_ERR_BAD_FILE_COUNT,
                };
            }
        } else if(isalpha(c)) {
            uint8_t piece_kind;
            switch(tolower(c)) {
                case 'p': piece_kind = MAX_PIECECODE_PAWN; break;
                case 'n': piece_kind = MAX_PIECECODE_KNIGHT; break;
                case 'b': piece_kind = MAX_PIECECODE_BISHOP; break;
                case 'r': piece_kind = MAX_PIECECODE_ROOK; break;
                case 'q': piece_kind = MAX_PIECECODE_QUEEN; break;
                case 'k': piece_kind = MAX_PIECECODE_KING; break;
                default: {
                    return (max_fen_parse_result_t){
                        .ok = false,
                        .err = MAX_FEN_ERR_INVALID_PIECE
                    };
                } break;
            }

            max_side_t side = (isupper(c) == 0);
            max_piececode_t piece = max_piececode_new(max_piececode_color_for_side(side), piece_kind);
            max_board_add_piece(
                board,
                max_0x88_new(rank, file),
                piece
            );

            file += 1;
        } else if(c == '/') {
            if(file != 8) {
                return (max_fen_parse_result_t){
                    .ok = false,
                    .err = MAX_FEN_ERR_BAD_FILE_COUNT
                };
            }
            
            fen += 1;
            break;
        } else {
            if(file == 8) {
                break;
            } else {
                return (max_fen_parse_result_t){
                    .ok = false,
                    .err = (c == '\0') ? MAX_FEN_ERR_EOF : MAX_FEN_ERR_INVALID_PIECE
                };
            }
        }

        fen += 1;
    }

    return (max_fen_parse_result_t){
        .ok = true,
        .end = fen
    };
}

/// Parse two castle rights characters, of the form KQ or HA.
/// If HA (chess960 FEN), the initial rook file will be set to the file
static max_fen_parse_result_t max_board_parse_castle_rights(max_board_t *board, char const *fen, max_side_t side) {
    max_state_t *state = max_board_state(board);
    
    bool chess960_style = false;
    uint8_t hside_rook_file = 8;
    uint8_t aside_rook_file = 8;
    switch(tolower(*fen)) {
        case 'k': state->packed |= max_packed_state_hcastle(side); break;
        case '-': state->packed &= ~max_packed_state_hcastle(side); break;
        case '\0': return (max_fen_parse_result_t){ .ok = false, .err = MAX_FEN_ERR_EOF }; break;
        default: {
            chess960_style = true;
            hside_rook_file = tolower(*fen) - 'a';
            if(hside_rook_file > 7) {
                return (max_fen_parse_result_t){ .ok = false, .err = MAX_FEN_ERR_INVALID_CASTLE_RIGHTS };
            }

            state->packed |= max_packed_state_hcastle(side);
        } break;
    }

    fen += 1;
    switch(tolower(*fen)) {
        case 'q': {
            if(chess960_style) {
                return (max_fen_parse_result_t){ .ok = false, .err = MAX_FEN_ERR_INVALID_CASTLE_RIGHTS };
            } else {
                state->packed |= max_packed_state_acastle(side);
            }
        } break;
        case '-': state->packed &= ~max_packed_state_acastle(side); break;
        default: {
            aside_rook_file = tolower(*fen) - 'a';
            if(aside_rook_file > 7) {
                return (max_fen_parse_result_t){ .ok = false, .err = MAX_FEN_ERR_INVALID_CASTLE_RIGHTS };
            }

            state->packed |= max_packed_state_acastle(side);
        }
    };

    fen += 1;
    if(chess960_style && aside_rook_file != 8 && hside_rook_file != 8) {
        max_board_set_initial_rook_files(board, aside_rook_file, hside_rook_file);
    } else {
        max_board_set_initial_rook_files(board, MAX_FILE_A, MAX_FILE_H);
    }

    return (max_fen_parse_result_t){ .ok = true, .end = fen };
}

/// Skip all whitespace from the given character onwards
static inline char const *max_board_skip_whitespace(max_board_t *board, char const *fen) {
    while(isspace(*fen)) {
        fen += 1;
    }

    return fen;
}

max_fen_parse_err_t max_board_parse_from_fen(max_board_t *board, const char *fen) {
    max_board_reset(board);
    
    max_fen_parse_result_t res;

    for(uint8_t rank = 8; rank > 0; --rank) {
        res = max_board_parse_fen_rank(board, fen, rank - 1);
        if(!res.ok) {
            return res.err;
        } else {
            fen = res.end;
        }
    }

    fen = max_board_skip_whitespace(board, fen);

    max_side_t side = MAX_SIDE_WHITE;
    switch(tolower(*fen)) {
        case 'w': side = MAX_SIDE_WHITE; break;
        case 'b': side = MAX_SIDE_BLACK; break;
        case '\0': return MAX_FEN_ERR_EOF; break;
        default: return MAX_FEN_ERR_BAD_PLAY_SIDE; break;
    }

    fen += 1;

    board->ply = side;

    fen = max_board_skip_whitespace(board, fen);
    if(*fen == '-') {
        max_state_t *state = max_board_state(board);
        uint8_t mask = MAX_PSTATE_ASIDE_CASTLE | MAX_PSTATE_HSIDE_CASTLE;
        state->packed &= ~(mask << MAX_PSTATE_WCASTLE_POS);
        state->packed &= ~(mask << MAX_PSTATE_BCASTLE_POS);
        fen += 1;
    } else {
        res = max_board_parse_castle_rights(board, fen, MAX_SIDE_WHITE);
        if(!res.ok) { return res.err; }
        fen = res.end;
        res = max_board_parse_castle_rights(board, fen, MAX_SIDE_BLACK);
        if(!res.ok) { return res.err; }
        fen = res.end;
    }


    fen = max_board_skip_whitespace(board, fen);
    if(*fen == '-') {
        fen = fen + 1;
    } else {
        max_state_t *state = max_board_state(board);
        if(*fen == '\0') { return MAX_FEN_SUCCESS; }
        uint8_t file = *fen - 'a';
        if(file > 7) { return MAX_FEN_ERR_INVALID_EPSQUARE; }

        fen += 1;
        uint8_t rank = *fen - '1';
        if(rank > 7) { return MAX_FEN_ERR_INVALID_EPSQUARE; }
        
        state->packed = max_packed_state_set_epfile(state->packed, file);
    }

    return MAX_FEN_SUCCESS;
}


const char *max_fen_parse_err_str(max_fen_parse_err_t ec) {
    static const char *const STR[] = {
        [MAX_FEN_SUCCESS] = "Success",
        [MAX_FEN_ERR_EOF] = "Unexpected End of File",
        [MAX_FEN_ERR_INVALID_CASTLE_RIGHTS] = "Invalid castle rights string",
        [MAX_FEN_ERR_BAD_FILE_COUNT] = "Files on a rank did not add up to 8",
        [MAX_FEN_ERR_INVALID_EPSQUARE] = "Invalid en passant square string",
        [MAX_FEN_ERR_INVALID_PIECE] = "Invalid piece character in rank string",
        [MAX_FEN_ERR_BAD_PLAY_SIDE] = "Invalid side to play character",
        [MAX_FEN_ERR_BAD_RANK_COUNT] = "Number of ranks in FEN string was not 8",
    };

    return STR[ec];
}
