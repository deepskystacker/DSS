#pragma once

// Here we organise, which vector library to use, e.g. Simde for OSX builds, etc.
// Unfortunately, __has_include is not supported for APPLE builds.

// Note:
// When including Qt header files and compiling with MSVC, then Simde does not work. At least for Qt 6.8. 
// Reason: In Qt 6.8 <QCoreApplication> includes "qnumeric.h", which includes <intrin.h>, which includes all Microsoft SIMD header files. 
// You then get many errors of SIMD type redefinitions.
// Solution 1: Keep using <immintrin.h> whenever it is available.
// Solution 2: Do not define SIMDE_ENABLE_NATIVE_ALIASES, but then all Simde symbols are prefixed with "simde_". So we would have to 
//             changes thousands of data types and function calls, and we would be "locked-in" to Simde. 

#if defined (Q_OS_MACOS) && defined(Q_PROCESSOR_ARM) // ARM -> We always have to emulate the x86-AVX instructions.

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
