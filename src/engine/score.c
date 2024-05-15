#include "max/engine/score.h"


void max_scorelist_sort(max_scorelist_t *scorelist) {
    uint8_t i = 1;
    while(i < scorelist->moves.len) {
        uint8_t j = i;
        while(j > 0 && scorelist->scores[j - 1] > scorelist->scores[j]) {
            max_score_t tmp = scorelist->scores[j - 1];
            max_smove_t move = scorelist->moves.buf[j - 1];

            scorelist->scores[j - 1] = scorelist->scores[j];
            scorelist->moves.buf[j - 1] = scorelist->moves.buf[j];
            scorelist->moves.buf[j] = move;
            scorelist->scores[j] = tmp;
            j -= 1;
        }

        i += 1;
    }
}
