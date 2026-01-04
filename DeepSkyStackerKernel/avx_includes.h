#pragma once
/****************************************************************************
**
** Copyright (C) 2024, 2025 Martin Toeltsch
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/

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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "simde/x86/avx512.h"
#pragma clang diagnostic pop
#else

#if __has_include(<immintrin.h>)

#include <immintrin.h>

#else // If, for some reason, immintrin.h is not found -> fall back to Simde.

#define SIMDE_ENABLE_NATIVE_ALIASES
#include "simde/x86/avx512.h"

#endif
#endif
