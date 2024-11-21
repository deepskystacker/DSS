#pragma once

// Here we organise, which vector library to use, e.g. Simde for OSX builds, etc.
// Unfortunately, __has_include is not supported for APPLE builds.

#if defined (__APPLE__) // ARM -> We always have to emulate the x86-AVX instructions.

#define SIMDE_ENABLE_NATIVE_ALIASES
#include "simde/x86/avx512.h"

#else

#if __has_include(<immintrin.h>)

#include <immintrin.h>

#else // If, for some reason, immintrin.h is not found -> fall back to Simde.

#define SIMDE_ENABLE_NATIVE_ALIASES
#include "simde/x86/avx512.h"

#endif
#endif
