#pragma once

// Here we organise, which vector library to use, e.g. Simde for OSX builds, etc.

#if defined (_MSC_VER)

#include <immintrin.h>

#elif defined (__GNUC__)

#include <immintrin.h>

#else

#include "simde/"

#endif
