#pragma once

#include "max/engine/engine.h"


#ifdef MAX_ENGINE_DIAGNOSTIC
#define DIAGNOSTIC(...) __VA_ARGS__
#else
#define DIAGNOSTIC(...)
#endif

void max_engine_sortmoves(max_engine_t *engine, max_movelist_t moves);
