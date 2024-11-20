#pragma once

// Here we organise, which vector library to use, e.g. Simde for OSX builds, etc.

#if __has_include(<immintrin.h>)

#include <immintrin.h>

#else

#define SIMDE_ENABLE_NATIVE_ALIASES
#include "simde/x86/avx512.h"

#endif
