#pragma once

#define F(name, val) MAX_##name = val,

enum {
    #include "./square.inc"
};

#undef F
