#include "test.h"

#include "max/ccr.h"

int main(int argc, char *argv[]) {
    max_board_t board = {0};
    board.ranks[7] = 0x0000E000;

    max_ccr_square_t piece = max_board_get_piece_mask(&board, 4, 7);
    ASSERT(piece == (MAX_CCR_KING | MAX_CCR_WHITE), "max_board_get_piece does not retrieve correct piece, got %02X", piece);
    return 0;
}
