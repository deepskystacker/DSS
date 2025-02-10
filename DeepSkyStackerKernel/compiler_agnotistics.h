#pragma once

#if defined(_MSC_VER)
#define bswap_16(x) _byteswap_ushort(x)
#define 
#elif defined(__GNUC__)
#define bswap_16(x) __builtin_bswap16(x)
#else
#error Compiler not yet supported
#endif
