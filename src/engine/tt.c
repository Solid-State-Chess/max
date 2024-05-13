#include "max/engine/tt.h"
#include "max/assert.h"
#include "private/engine/tt.h"

void max_ttbl_new(max_ttbl_t *tbl, max_ttentry_t *buf, uint8_t nbits) {
    MAX_ASSERT(nbits < 32 && "Cannot create a transposition table with a buffer larger than 4GiB");
    uint32_t capacity = 1 << nbits;
    for(uint32_t i = 0; i < capacity; ++i) {
        buf[i] = max_ttentry_gravestone();
    }

    tbl->buf = buf;
    tbl->nbit = nbits;
}
