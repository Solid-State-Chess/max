#pragma once

/// \ingroup board
/// @{

/// \ingroup piececode
/// @{

/// \name Private Functions
/// @{

#ifdef MAX_TESTS

/// Perform unit tests for piece codes and piece masks to ensure changes to piece codes do not
/// unknowingly interfere with assumed behavior in the engine.
void max_piececode_unit_tests(void);

#endif

/// @}

/// @}

/// @}
