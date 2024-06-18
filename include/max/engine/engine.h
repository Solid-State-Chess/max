#pragma once
#include "max/board/board.h"
#include "max/board/state.h"
#include "max/engine/eval/param.h"
#include "max/engine/tt.h"

/// \defgroup engine Chess Engine
/// The actual chess engine including search algorithm and transposition table.
/// @{

#ifdef MAX_ENGINE_DIAGNOSTIC

typedef struct {
    uint64_t nodes;
    uint64_t ttbl_hits;
    uint64_t ttbl_used;
} max_engine_diagnostic_t;

#endif

/// All state required by the chess engine to search a game tree and evaluate positions.
/// Includes a pointer to a #max_board_t that will be used to search a tree by making and unmaking moves,
/// and configurable evaluation parameters.
typedef struct {
    /// The board containing all state of the chess game being played by the engine,
    /// used for move making / unmaking.
    /// The engine must have ownership of this structure when a search is in progress,
    /// as it will be continually updated and must not be modified externally.
    max_board_t board;
    /// Evaluation parameters used to fine tune the behavior of the engine.
    max_eval_params_t param;
    
    #ifdef MAX_ENGINE_DIAGNOSTIC

    /// Diagonistics reset after every search
    max_engine_diagnostic_t diagnostic;
    
    #endif

    /// Transposition table from which previous evaluations can be probed and reused.
    /// \see #max_ttbl_t
    max_ttbl_t table;
    /// Move list used to store moves that lead to lower positions in the game tree search
    max_movelist_t moves;

    uint64_t time;
} max_engine_t;

/// Parameters used to initialize a #max_engine_t
typedef struct {
    /// A buffer required for the board's irreversible state stack.
    struct {
        /// The storage used for the state stack's plates when making and unmaking moves.
        max_state_t *stack;
        /// Capacity in number of elements of the stack.
        /// This must be at least 3 to accomodate draws by threefold repetition.
        uint8_t capacity;
    } board;

    /// Transposition table buffer and capacity bits.
    struct {
        /// A pointer to an allocated buffer used to store entries in the transposition table.
        /// The capacity of this buffer must be a power of two = 1 << nbit.
        max_ttentry_t *buf;
        /// Number of bits to shift left a 1 in order to get the capacity of the provided buffer.
        /// The capacity must be a power of two in order to allow efficient index creation by masking
        /// the lower bits of a zobrist key.
        uint8_t nbit;
    } ttbl;
    
    /// Buffer used to store moves when descending into the game tree.
    struct {
        /// Pointer to the buffer used to store moves during movegen in the game search.
        max_smove_t *buf;
        // Capacity of the move buffer in number of elements possible to store.
        uint32_t capacity;
    } moves;
} max_engine_init_params_t;

/// The result of a completed search, indicating the best move,
/// and the score assigned to that move.
typedef struct {
    max_score_t score;
    max_smove_t best;
    uint8_t depth;
    bool gameover;
} max_search_result_t;

/// Create a new engine with the given buffers to use for lookup tables.
/// Automatically initializes the contained board without adding any pieces,
/// it is the user's responsibility to modify the board after this method to add a starting position.
void max_engine_new(max_engine_t *engine, max_engine_init_params_t *init, max_eval_params_t param);

/// Attempt to locate the best move for the current side to play with iterative deepening.
/// \param [in] engine The engine containing the currently analyzed chess board
/// \param [out] search Pointer to an uninitialized search result theat will be filled with the result of the search.
void max_engine_search(max_engine_t *engine, max_search_result_t *search);

/// @}
