#pragma once
#include "max/board/capturelist.h"


/// \ingroup state
/// @{

/// \name Private Functions

/// Initialize the given capture list's length to 0 without modifying the contents.
MAX_INLINE_ALWAYS void max_captures_new(max_captures_t *caps) { caps->len = 0; }

/// @}

/// @}
