#include "max/engine/score.h"


void max_scorelist_sort(max_scorelist_t *scorelist) {
    /*uint8_t i = 1;
    while(i < scorelist->moves.len) {
        uint8_t j = i;
        while(j > 0 && scorelist->scores[j - 1] < scorelist->scores[j]) {
            max_score_t tmp = scorelist->scores[j - 1];
            max_smove_t move = scorelist->moves.buf[j - 1];

            scorelist->scores[j - 1] = scorelist->scores[j];
            scorelist->moves.buf[j - 1] = scorelist->moves.buf[j];
            scorelist->moves.buf[j] = move;
            scorelist->scores[j] = tmp;
            j -= 1;
        }

        i += 1;
    }*/

    bool sorted = false;
    for(uint8_t j = 0; j < 100 && !sorted; ++j) {
        sorted = true;
        for(unsigned i = 1; i < scorelist->moves.len; ++i) {
            if(scorelist->scores[i - 1] < scorelist->scores[i]) {
                max_score_t tmp_score = scorelist->scores[i - 1];
                max_smove_t tmp_move  = scorelist->moves.buf[i - 1];
                
                scorelist->scores[i - 1] = scorelist->scores[i];
                scorelist->moves.buf[i - 1] = scorelist->moves.buf[i];

                scorelist->scores[i] = tmp_score;
                scorelist->moves.buf[i] = tmp_move;
                sorted = false;
            }
        }
    }
}
