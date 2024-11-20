#pragma once

// Here we organise, which vector library to use, e.g. Simde for OSX builds, etc.

#if defined (_MSC_VER)

#include <immintrin.h>

#elif defined (__GNUC__) && (!defined __APPLE__)

#include <immintrin.h>

#else

#define SIMDE_ENABLE_NATIVE_ALIASES
#include "simde/x86/avx512.h"

#endif
