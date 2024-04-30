#pragma once

#define FILE(letter, lsn)         \
    MAX_##letter##1 = 0x0##lsn,   \
    MAX_##letter##2 = 0x1##lsn,   \
    MAX_##letter##3 = 0x2##lsn,   \
    MAX_##letter##4 = 0x3##lsn,   \
    MAX_##letter##5 = 0x4##lsn,   \
    MAX_##letter##6 = 0x5##lsn,   \
    MAX_##letter##7 = 0x6##lsn,   \
    MAX_##letter##8 = 0x7##lsn

enum {
    FILE(A, 0),
    FILE(B, 1),
    FILE(C, 2),
    FILE(D, 3),
    FILE(E, 4),
    FILE(F, 5),
    FILE(G, 6),
    FILE(H, 7),
};
