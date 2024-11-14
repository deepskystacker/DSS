// ----------------------------------------------------------------------------
// Project : Z Class Library
// $Workfile$
// $Revision$
// $Date$
//
// Original Author: David C. Partridge
//
// +USE+ Description: 
// Module Description.
//
// ZTrace Class definition
//
// Freely cribbed in spirit if not in exact implementation from the 
// IBM Open Class IException and derived classes.
// 
// -USE-
// (C) Copyright 1998 David C. Partridge
//
// Language:    ANSI Standard C++
// Target:      Portable (with modifications) currently supported are:
//              Windows NT 4.x and later
//              Unix systems (untested)
//
// Modifications:
//   See history at end of file ...
// ----------------------------------------------------------------------------

#pragma once

#if !defined(ZDEFS_INCLUDED)
#include <zdefs.h>
#endif

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(none)
#endif

#include <string>

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(restore)
#endif
class QString;


class ZExceptionLocation;

class ZExport ZTrace
{
/**
 *
 *  ZTrace
 *
 *  The ZTrace class provides the tracing capabilities within the ZClass
 *  library.   Whenever tracing is being used and an exception is thrown
 *  then trace records are produced with information about the exception.
 *  If tracing is not being used, then the exception information is sent
 *  to stderr.
 *  
 *  The output trace records for an exception contain the following:
 *  
 *  o Error message text
 *  o Error ID
 *  o Exception class name
 *  o Information from the ZExceptionLocation class
 *
 *  By default the trace is disabled.   You turn tracing on by setting
 *  the environment variable Z_TRACE to the value "ON".
 *
 *  By default the trace information contains a prefix with an optional 
 *  sequence number, an optional timestamp (UTC),  and then the process
 *  and the thread where the trace request was issued.   You can remove
 *  the prefix information by setting Z_TRACE to the value "NOPREFIX".
 *  This also turns tracing on.
 *
 *  You can control where the trace information is sent by setting one of
 *  the following in the environment:
 *
 *  Z_TRACETO=STDERR  To send output to stderr (This is the default)
 *  Z_TRACETO=STDOUT  To send output to stdout
 *  Z_TRACETO=FILE    To send output to a file specified by the value
 *                    of the environment variable Z_TRACEFILE
 *  
 *  Specifying any of the above as the location for trace output also 
 *  turns on the trace.
 *
 *  In addition to turning the trace on or off using environment variables
 *  there are also static member functions to do the same thing from your
 *  program.
 *  
 *  Tracing can be turned on and off using the static functions enableTrace()
 *  and disableTrace().
 *
 *  The output destination can be changed to stderr or stdout using the 
 *  static functions writeToStandardError() or writeToStandardOutput().
 *  If you want to send the trace output to a file you can use the static
 *  function writeToFile(), but if you use this, the library will still 
 *  use the value of the Z_TRACEFILE to establish the file id.
 *
 *  The trace prefix can be enabled using enableWritePrefix() and can be
 *  disabled using disableWritePrefix().
 *
 *  The prefix area sequence number can be turned on or off using the static
 *  member functions enableWriteLineNumber() and disableWriteLineNumber()
 *  respectively.   It is on by default.
 *
 *  The prefix area timestamp can be controlled with the static member
 *  functions enableWriteTimeStamp() and disableWriteTimeStamp().  The
 *  timestamp is on by default.
 *
 *  To control the compilation of the trace calls in your code, a number of
 *  macros are available:
 *
 *  o The library defines the macro Z_TRACE_RUNTIME by default.  If this 
 *    macro is defined, the following macros are expanded:
 *
 *    ZMODTRACE_RUNTIME()   ZFUNCTRACE_RUNTIME()    ZTRACE_RUNTIME()
 *
 *  o If you define the macro Z_TRACE_DEVELOP, then the following macros,
 *    in addition to the RUNTIME macros, are expanded:
 *
 *    ZMODTRACE_DEVELOP()   ZFUNCTRACE_DEVELOP()    ZTRACE_DEVELOP()
 *
 *  o If you define the macro Z_TRACE_ALL, the following macros, in addition
 *    to the RUNTIME and DEVELOP macros, are expanded:
 *
 *    ZMODTRACE_ALL()       ZFUNCTRACE_ALL()        ZTRACE_ALL()
 *
 *  The ZMODTRACE macros accept as input a module or function name that it
 *  uses for construction and destruction tracing.
 *
 *  The ZFUNCTRACE macros accept no input, but use either the macro variable
 *  __PRETTY_FUNCTION__ (GCC) or __FUNCSIG__ (MS C++) or __FUNCTION__ in all
 *  other cases for construction and destruction tracing.
 *
 *  The ZTRACE macros accept a text string that is written to the trace.
 */

friend class ZTraceSetup;

public:
/*------------------------- Constructors -------------------------------------*/
/**
 *  You construct a ZTrace object using this default constructor.   If you do
 *  not specify the optional values, this constructor creates a ZTrace object,
 *  but no logging occurs on construction or destruction.
 */ 
  ZTrace                   ( const char* traceName =0,
                             const char* fileName=0,
                             long        lineNumber=0 );

 ~ZTrace                   ( );

/*-------------------------- Output/Location ---------------------------------*/

/**
 *  The write() methods allow you to specify trace information to be written
 *  either as a standard C++ string or as a C String.
 */ 
static void
  write                    ( const std::string& text ),
  write                    ( const QString&     text ),
  write                    (const char*    format, ...);

/**
* The dumpHex() method does what it says on the tin!
*/
static void dumpHex(const void* p, size_t len);

enum Destination           {
  standardError,
  standardOutput,
  file
  };

/**
 *  Returns the trace output destination for this trace object.   The return
 *  value is an enumerator defined in ZTrace::Destination
 */ 
static ZTrace::Destination
  traceDestination         ( );

/**
 *  These methods set the trace output destination.   They are equivalent
 *  to the values described above for Z_TRACETO=xxxxxxx.   If you use the
 *  writeToFile() method, the environment variable Z_TRACEFILE needs to be
 *  set to a valid fileid for any trace output to be produced.
 */ 
static void
  writeToStandardError     ( ),
  writeToStandardOutput    ( ),
  writeToFile              ( );

/*------------------------- Enable/Disable -----------------------------------*/
static void
  enableTrace              ( ),
  disableTrace             ( );

static bool
  isTraceEnabled           ( );

/*------------------------- Formatting ---------------------------------------*/
static void
  enableWriteLineNumber    ( ),
  disableWriteLineNumber   ( ),
  enableWritePrefix        ( ),
  disableWritePrefix       ( ),
  enableWriteTimeStamp     ( ),
  disableWriteTimeStamp    ( );

static bool
  isWriteLineNumberEnabled ( ),
  isWritePrefixEnabled     ( ),
  isWriteTimeStampEnabled  ( );

/*------------------------- Debugging ----------------------------------------*/
/**
 *  This mf writes a trace entry with the supplied string and then information
 *  from the supplied ZExceptionLocation object (which you would normally create
 *  using the ZEXCEPTION_LOCATION macro).
 */ 
static void
  writeDebugLocation       (const char* str, const ZExceptionLocation& location);

protected:
/*------------------------- Thread ID ----------------------------------------*/
static unsigned long
  threadId                 ( );

/*------------------------- Protected Output Operations ----------------------*/
static void
  writeString              ( const char*    text   ),
  writeFormattedString     ( const std::string& string,
                             const char*          marker );

private:
/*------------------------- Private ------------------------------------------*/

enum State {
  uninitialized=1,
  on=2,
  writeLineNumber=4,
  writePrefix=8,
  writeTimeStamp=16
  };
char
 *pszClTraceName;
static int
  iClState;
static ZTrace::Destination
  iClTraceLocation;
static size_t
  remainingStack ( );

};

#define Z_TRACE_RUNTIME

#ifdef __FUNCSIG__
#define __ZTRACE_FUNCTION__ __FUNCSIG__
#elif defined(__PRETTY_FUNCTION__)
#define __ZTRACE_FUNCTION__ __PRETTY_FUNCTION__
#else
#define __ZTRACE_FUNCTION__ __FUNCTION__
#endif

#ifdef Z_DEVELOP
  #define Z_TRACE_DEVELOP
#endif

#ifdef Z_TRACE_ALL
  #define Z_TRACE_DEVELOP
#endif

#ifdef Z_TRACE_ALL
   #define ZMODTRACE_ALL(modname)  ZTrace trc(modname, __FILE__, __LINE__ )
   #define ZFUNCTRACE_ALL()        ZTrace trc(__ZTRACE_FUNCTION__, __FILE__, __LINE__ )
   #define ZTRACE_ALL(...)         ZTrace::write(__VA_ARGS__)
   #ifndef Z_TRACE_DEVELOP
     #define Z_TRACE_DEVELOP
   #endif
#else
   #define ZMODTRACE_ALL(modname)
   #define ZFUNCTRACE_ALL()
   #define ZTRACE_ALL(...)
#endif

#ifdef Z_TRACE_DEVELOP
   #define ZMODTRACE_DEVELOP(modname) ZTrace trc(modname,  __FILE__, __LINE__ )
   #define ZFUNCTRACE_DEVELOP()       ZTrace trc(__ZTRACE_FUNCTION__, __FILE__, __LINE__ )
   #define ZTRACE_DEVELOP(...)        ZTrace::write(__VA_ARGS__)
   #ifndef Z_TRACE_RUNTIME
     #define Z_TRACE_RUNTIME
   #endif
#else
   #define ZMODTRACE_DEVELOP(modname)
   #define ZFUNCTRACE_DEVELOP()
   #define ZTRACE_DEVELOP(...)
#endif

#ifdef Z_TRACE_RUNTIME
   #define ZMODTRACE_RUNTIME(modname)  ZTrace trc(modname, __FILE__, __LINE__ )
   #define ZFUNCTRACE_RUNTIME()        ZTrace trc(__ZTRACE_FUNCTION__, __FILE__, __LINE__ )
   #define ZTRACE_RUNTIME(...)         ZTrace::write(__VA_ARGS__)
#else
   #define ZMODTRACE_RUNTIME(modname)
   #define ZFUNCTRACE_RUNTIME()
   #define ZTRACE_RUNTIME(...)
#endif

#ifdef Z_TRACE_DEVELOP
   #define ZTRACE_UNIX_NOP() \
               ZTrace::writeDebugLocation("Unix NOP", ZEXCEPTION_LOCATION() )
   #define ZTRACE_UNIX_NOTYET() \
               ZTrace::writeDebugLocation("Unix NOTYET", ZEXCEPTION_LOCATION() )
   #define ZTRACE_WIN_NOP()      \
               ZTrace::writeDebugLocation("Windows NOP", ZEXCEPTION_LOCATION() )
   #define ZTRACE_WIN_NOTYET()   \
               ZTrace::writeDebugLocation("Windows NOTYET", ZEXCEPTION_LOCATION() )
   #define ZTRACE_OS2_NOP()       \
               ZTrace::writeDebugLocation("OS/2 NOP", ZEXCEPTION_LOCATION() )
   #define ZTRACE_OS2_NOTYET()    \
               ZTrace::writeDebugLocation("OS/2 NOTYET", ZEXCEPTION_LOCATION() )
   #define ZTRACE_MVS_NOP()       \
               ZTrace::writeDebugLocation("MVS NOP", ZEXCEPTION_LOCATION() )
   #define ZTRACE_MVS_NOTYET()    \
               ZTrace::writeDebugLocation("MVS NOTYET", ZEXCEPTION_LOCATION() )
   #define ZTRACE_OS400_NOP()       \
               ZTrace::writeDebugLocation("OS400 NOP", ZEXCEPTION_LOCATION() )
   #define ZTRACE_OS400_NOTYET()    \
               ZTrace::writeDebugLocation("OS400 NOTYET", ZEXCEPTION_LOCATION() )
#else
   #define ZTRACE_UNIX_NOP()
   #define ZTRACE_UNIX_NOTYET()
   #define ZTRACE_WIN_NOP()
   #define ZTRACE_WIN_NOTYET()
   #define ZTRACE_OS2_NOP()
   #define ZTRACE_OS2_NOTYET()
   #define ZTRACE_MVS_NOP()
   #define ZTRACE_MVS_NOTYET()
   #define ZTRACE_OS400_NOP()
   #define ZTRACE_OS400_NOTYET()
#endif

#if (0)
// Modifications:
//
// +MH+ $Log$
//
// -MH-
#endif

