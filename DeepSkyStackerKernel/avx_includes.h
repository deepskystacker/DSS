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

#if defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
#define SIMDE_ENABLE_NATIVE_ALIASES


#if defined (Q_OS_MACOS) && defined(Q_PROCESSOR_X86_64)
//
// This definition is required for compiling code on macOS x86_64 that includes the
// simde headers and is built without any AVX options.
// 
// If it's not set, you'll get errors like:
// 
// /Users/amonra/.vs/DSS/DeepSkyStackerKernel/simde/x86/avx512/types.h:634:27: error: typedef redefinition with different types ('simde__m128bh' (vector of 4 'simde_float32' values) vs '__attribute__((__vector_size__(8 * sizeof(__bf16)))) __bf16' (vector of 8 '__bf16' values))
//   634 |     typedef simde__m128bh __m128bh;
//       |                             ^
//  /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/21/include/emmintrin.h:48:16: note: previous definition is here
//    48 | typedef __bf16 __m128bh __attribute__((__vector_size__(16), __aligned__(16)));
//       |                ^
// 1 error generated.
//
#define SIMDE_X86_AVX512BF16_NATIVE
#endif

#if defined (Q_CC_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#include "simde/x86/avx512.h"
#include "simde/x86/svml.h"

#if defined (Q_CC_CLANG)
#pragma clang diagnostic pop
#endif

#elif defined (Q_OS_WIN) // If it is Windows, include <immintrin.h> for MSVC

#include <immintrin.h>

#else
#error "Unknown platform"
#endif
