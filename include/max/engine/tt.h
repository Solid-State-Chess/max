/// \file tt.h
#pragma once
#include "max/board/loc.h"
#include "max/board/zobrist.h"
#include "max/def.h"
#include "max/engine/score.h"
#include <stdint.h>

/// \ingroup engine
/// @{


/// \defgroup tt Transposition Table
/// The all-important transposition table allowing the engine to perform iterative deepening search and massively
/// speeding up game tree searches by eliminating already-searched portions of the tree.
/// @{

/// Key portion of a zobrist hash, created after masking away the lower index bits.
/// This is half the size of a true zobrist hash to aggresively reduce the size of the transposition table,
/// at the cost of increased table collision rate if the key portion is actually larger than the storage type.
/// @{

#ifdef MAX_ZOBRIST_64
typedef uint32_t max_ttentry_key_t;
#else
typedef uint16_t max_ttentry_key_t;
#endif

/// @}

#pragma pack(push, 1)

/// Packed attributes for a #max_ttentry_t, including the best move's from and to square,
/// the kind of node stored, and depth that the node was searched to.
/// ## Bit Layout
/// ```
/// [ 2 bits - node kind ][ 2 bits - depth ][6 bits - destination square][ 6 bits - source square ]
/// ```
typedef uint16_t max_ttentry_pattr_t;

enum {
    /// Mask for the lowest 6 bits of a #max_ttentry_pattr_t to get the packed move source square
    MAX_TTENTRY_PATTR_SOURCE_MASK = 0x007F,
    /// Mask for bits [11, 6] of a #max_ttentry_pattr_t to get the packed move destination square
    MAX_TTENTRY_PATTR_DEST_MASK   = 0x0FC0,
    /// Bit offset from the LSB that the 6 destination square bits are located at
    MAX_TTENTRY_PATTR_DEST_POS    = 6,
    /// Mask for bits [13, 12] of a #max_ttentry_pattr_t for the two depth bits
    MAX_TTENTRY_PATTR_DEPTH_MASK  = 0x3000,
    /// Bit offset from the LSB that the two depth bits are located
    MAX_TTENTRY_PATTR_DEPTH_POS   = 12,
    /// Bitmask for the upper 2 bits [15, 14] of a #max_ttentry_pattr_t containing the node kind
    MAX_TTENTRY_PATTR_KIND_MASK   = 0xC000,
    /// Bit offset from the LSB that the two node kind bits are located
    MAX_TTENTRY_PATTR_KIND_POS    = 14,
};

/// Create a new packed attribute from unpacked move source and destination squares,
/// node kind specifier, and node analysis depth.
MAX_INLINE_ALWAYS max_ttentry_pattr_t max_ttentry_pattr_new(
    max_0x88_t from,
    max_0x88_t to,
    max_nodekind_t kind,
    uint8_t depth
) {
    return 
        max_0x88_to_6bit(from).v |
        (max_0x88_to_6bit(to).v << MAX_TTENTRY_PATTR_DEST_POS) |
        (kind << MAX_TTENTRY_PATTR_KIND_POS) |
        (((depth + 1) >> 1) << MAX_TTENTRY_PATTR_DEPTH_POS);
}

/// Get the 0x88 source square packed into a #max_ttentry_pattr_t
MAX_INLINE_ALWAYS max_0x88_t max_ttentry_pattr_source(max_ttentry_pattr_t packed) {
    return max_6bit_to_0x88(
        max_6bit_raw(packed & MAX_TTENTRY_PATTR_SOURCE_MASK)
    );
}

/// Get the 0x88 destination square packed into a #max_ttentry_pattr_t
MAX_INLINE_ALWAYS max_0x88_t max_ttentry_pattr_dest(max_ttentry_pattr_t packed) {
    return max_6bit_to_0x88(
        max_6bit_raw((packed & MAX_TTENTRY_PATTR_DEST_MASK) >> MAX_TTENTRY_PATTR_DEST_POS)
    );
}

/// Get the depth packed into the given tranposition table entry attributes.
MAX_INLINE_ALWAYS uint8_t max_ttentry_pattr_depth(max_ttentry_pattr_t packed) {
    return ((packed & MAX_TTENTRY_PATTR_DEPTH_MASK) >> MAX_TTENTRY_PATTR_DEPTH_POS) << 1;
}

/// An entry in the transposition table containing the result of a prior evaluation.
/// The size of this entry has massive effects on the memory footprint of the program as the transposition table
/// should be the largest object in the engine, so reducing it's size is extremely important.
/// It represents a best or refutation move in as compact a form as possible to reflect this, and packs all attributes into
/// as few bits as possible.
typedef struct {
    /// Key portin of the zobrist hash used to access this position, used to ensure that all index
    /// collisions do not also result in a full hash collision, which would case the engine to use
    /// invalid transposition table entries far more often.
    max_ttentry_key_t key;
    /// Score of the node as determined by the search when this entry was created.
    /// If the node was alpha or beta cutoff, this is the value that caused the cutoff.
    max_score_t score;
    /// Packed attributes including the best or refutation move's source and destination squares, and
    /// the kind of node score and depth that the node was searched to.
    max_ttentry_pattr_t attr;
} max_ttentry_t;

#pragma pack(pop)

/// A transposition table of configurable power-of-two capacity.
/// This table stores a pointer to the continuous buffer that will be used to store transposition table
/// entries and the capacity of the buffer as a power of two.
typedef struct {
    /// Pointer to the buffer that transposition table entries are stored to.
    max_ttentry_t *buf;
    /// Capacity of the buffer stored as a power of two.
    /// Also used to create a bitmask that will separate the key and index parts of a zobrist hash.
    uint8_t nbit; 
} max_ttbl_t;

/// Get the index part of a zobrist hash according to the power-of-two capacity of the given tranposition table.
/// \return an index into the table's buffer to be used to get the entry associated with the given hash
MAX_INLINE_ALWAYS uint32_t max_ttbl_get_index(max_ttbl_t *tbl, max_zobrist_t hash) {
    return hash & ((1 << tbl->nbit) - 1);
}

/// Get the key part of a zobrist hash used to detect index collisions in the tranposition table.
MAX_INLINE_ALWAYS max_ttentry_key_t max_ttbl_get_key(max_ttbl_t *tbl, max_zobrist_t hash) {
    return hash >> (MAX_ZOBRIST_BITWIDTH - tbl->nbit);
}

/// Get a read-only pointer to the transposition table entry corresponding to a saved analysis of the given position.
/// \return NULL if no entry was found corresponding to the zobrist hash, or a pointer to an entry that must only be read from.
max_ttentry_t const* max_ttbl_probe_read(max_ttbl_t *tbl, max_zobrist_t hash);

/// Insert a new node score into the transposition table, potentially overwriting the previous stored score
void max_ttbl_probe_insert(max_ttbl_t *tbl, max_zobrist_t hash, max_nodescore_t score);

/// @}

/// @}
