#pragma once

#include "max/board/loc.h"

#define MAX_A1 max_0x88_raw(0x00)
#define MAX_A2 max_0x88_raw(0x10)
#define MAX_A3 max_0x88_raw(0x20)
#define MAX_A4 max_0x88_raw(0x30)
#define MAX_A5 max_0x88_raw(0x40)
#define MAX_A6 max_0x88_raw(0x50)
#define MAX_A7 max_0x88_raw(0x60)
#define MAX_A8 max_0x88_raw(0x70)

#define MAX_B1 max_0x88_raw(0x01)
#define MAX_B2 max_0x88_raw(0x11)
#define MAX_B3 max_0x88_raw(0x21)
#define MAX_B4 max_0x88_raw(0x31)
#define MAX_B5 max_0x88_raw(0x41)
#define MAX_B6 max_0x88_raw(0x51)
#define MAX_B7 max_0x88_raw(0x61)
#define MAX_B8 max_0x88_raw(0x71)

#define MAX_C1 max_0x88_raw(0x02)
#define MAX_C2 max_0x88_raw(0x12)
#define MAX_C3 max_0x88_raw(0x22)
#define MAX_C4 max_0x88_raw(0x32)
#define MAX_C5 max_0x88_raw(0x42)
#define MAX_C6 max_0x88_raw(0x52)
#define MAX_C7 max_0x88_raw(0x62)
#define MAX_C8 max_0x88_raw(0x72)


#define MAX_D1 max_0x88_raw(0x03)
#define MAX_D2 max_0x88_raw(0x13)
#define MAX_D3 max_0x88_raw(0x23)
#define MAX_D4 max_0x88_raw(0x33)
#define MAX_D5 max_0x88_raw(0x43)
#define MAX_D6 max_0x88_raw(0x53)
#define MAX_D7 max_0x88_raw(0x63)
#define MAX_D8 max_0x88_raw(0x73)

#define MAX_E1 max_0x88_raw(0x04)
#define MAX_E2 max_0x88_raw(0x14)
#define MAX_E3 max_0x88_raw(0x24)
#define MAX_E4 max_0x88_raw(0x34)
#define MAX_E5 max_0x88_raw(0x44)
#define MAX_E6 max_0x88_raw(0x54)
#define MAX_E7 max_0x88_raw(0x64)
#define MAX_E8 max_0x88_raw(0x74)

#define MAX_F1 max_0x88_raw(0x05)
#define MAX_F2 max_0x88_raw(0x15)
#define MAX_F3 max_0x88_raw(0x25)
#define MAX_F4 max_0x88_raw(0x35)
#define MAX_F5 max_0x88_raw(0x45)
#define MAX_F6 max_0x88_raw(0x55)
#define MAX_F7 max_0x88_raw(0x65)
#define MAX_F8 max_0x88_raw(0x75)

#define MAX_G1 max_0x88_raw(0x06)
#define MAX_G2 max_0x88_raw(0x16)
#define MAX_G3 max_0x88_raw(0x26)
#define MAX_G4 max_0x88_raw(0x36)
#define MAX_G5 max_0x88_raw(0x46)
#define MAX_G6 max_0x88_raw(0x56)
#define MAX_G7 max_0x88_raw(0x66)
#define MAX_G8 max_0x88_raw(0x76)

#define MAX_H1 max_0x88_raw(0x07)
#define MAX_H2 max_0x88_raw(0x17)
#define MAX_H3 max_0x88_raw(0x27)
#define MAX_H4 max_0x88_raw(0x37)
#define MAX_H5 max_0x88_raw(0x47)
#define MAX_H6 max_0x88_raw(0x57)
#define MAX_H7 max_0x88_raw(0x67)
#define MAX_H8 max_0x88_raw(0x77)

#define MAX_FILE_MASK_A 0x00
#define MAX_FILE_MASK_B 0x01
#define MAX_FILE_MASK_C 0x02
#define MAX_FILE_MASK_D 0x03
#define MAX_FILE_MASK_E 0x04
#define MAX_FILE_MASK_F 0x05
#define MAX_FILE_MASK_G 0x06
#define MAX_FILE_MASK_H 0x07


#define MAX_RANK_MASK_1 0x00
#define MAX_RANK_MASK_2 0x10
#define MAX_RANK_MASK_3 0x20
#define MAX_RANK_MASK_4 0x30
#define MAX_RANK_MASK_5 0x40
#define MAX_RANK_MASK_6 0x50
#define MAX_RANK_MASK_7 0x60
#define MAX_RANK_MASK_8 0x70

enum {
    MAX_RANK_1 = 0,
    MAX_RANK_2 = 1,
    MAX_RANK_3 = 2,
    MAX_RANK_4 = 3,
    MAX_RANK_5 = 4,
    MAX_RANK_6 = 5,
    MAX_RANK_7 = 6,
    MAX_RANK_8 = 7,
    MAX_RANK_INVALID = 8
};

#undef max_0x88_raw
