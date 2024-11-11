#if !defined(ZDEFS_INCLUDED)
#define ZDEFS_INCLUDED
//***************************************************************************
// Project : Z Class Library
// $Workfile$
// $Revision$
// $Date$
//
// Original Author: David C. Partridge
//
// Module Description.
//
// defs.h C++ header file to handle definitions for things like exporting of 
//        entry points and classes based on compiler in use. 
// 
// (C) Copyright 1999 David C. Partridge
//
// Language:    ANSI C++
// Compilers:   Microsoft Visual C++ 5.0
//              IBM Visual Age C++ 3.5 or higher
// Target:      Portable (in theory)
//              Windows NT 4.x and later
//              OS/2 V3 (Warp) and later
//
// Modifications:
//   See history at end of file ...
//
//***************************************************************************/

//
// Initially we define this to nothing.
//
#define ZExport

#if defined(_MSC_VER)
# if !defined(__wtypes_h__)
#   include <wtypes.h>
# endif
//
// If we're actually BUILDING the Z library as a DLL on Windows
// then we want to define ZExport to be __declspec(dllexport)
//
// As only a small inefficiency is caused to apps by the call thunk
// we don't handle the issue of whether the user is calling into us
// as a static library or a DLL.
//
# if defined(BUILDING_ZLIB_AS_DLL) && defined(_MSC_VER)
#   if defined(ZExport)
#     undef ZExport
#   endif
#   define ZExport __declspec(dllexport)
# endif

#endif

#if defined(__IBMCPP__) && defined(__MVS__)
#if __IBMCPP__ <= 22100
#define ZCLASS_EXPLICIT_NOT_SUPPORTED
#define ZCLASS_NAMESPACE_NOT_SUPPORTED
#endif
#endif

#if defined(_AIX) && defined(__IBMCPP__)
# if __IBMCPP__ < 500
#   error Need version 5 of IBM Visual Age C++ to compile for AIX
# endif
#endif

#if defined(__hpux) && !defined(_HPUX_SOURCE)
#error Preprocessor macro _HPUX_SOURCE not defined
#endif
#if defined(__hpux) && !defined(HPUX_VERS)
#error Preprocessor macro HPUX_VERS must be defined to e.g 1020 or 1100 by makefile
#endif

#if defined(_AIX) || defined(__sun) || defined(__sun__) ||\
  defined(__linux__) || defined(__hpux) || defined(__APPLE__)
#if !defined(ZCLASS_UNIX)
#define ZCLASS_UNIX
#endif
#endif

#if (0)
// Modifications:
//
// $Log$
//
#endif

#endif // !defined(ZDEFS_INCLUDED)
