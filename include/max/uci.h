#pragma once

#include <stdbool.h>
#include <stdint.h>


/// Slice taken out of a UCI command string
typedef struct {
    char *ptr;
    uint16_t len;
} max_uci_slice_t;

/// Universal Chess Interface go command with all attributes
typedef struct {
    uint64_t nodes;
    uint64_t mate;
    uint32_t wtime_ms;
    uint32_t btime_ms;
    uint32_t winc_ms;
    uint32_t binc_ms;
    uint32_t movetime;
    uint16_t movestogo;
    uint16_t depth;
    bool ponder;
    bool infinite;
} max_uci_go_t;


