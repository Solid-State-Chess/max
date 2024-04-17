#include "max/engine/tt.h"


void max_ttbl_new(max_ttbl_t *tbl) {
    for(unsigned i = 0; i < MAX_TTBL_LEN; ++i) {
        tbl->array[i].attr.gravestone = true;
    } 
}
