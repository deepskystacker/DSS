//***************************************************************************
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
// ZTrace implementation
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
//
//***************************************************************************/

//***************************************************************************/
#if defined(_MSC_VER)

/////////////////////////////////////////////////////////////////////////////
// Turn off warnings for /W4
// To resume any of these warning: #pragma warning(default: 4xxx)
// which should be placed after the Standard C++ include files
#ifndef ALL_WARNINGS
// warnings generated with Standard C++ include files
#pragma warning(disable : 4018)  // signed/unsigned mismatch
#pragma warning(disable : 4100)  // unreferenced formal parameter
#pragma warning(disable : 4146)  // unary minus applied to unsigned
#pragma warning(disable : 4201)  // nameless struct/union
#pragma warning(disable : 4244)  // loss of precision on conversion
#pragma warning(disable : 4245)  // conversion signed/unsigned mismatch
#pragma warning(disable : 4511)  // copy ctor could not be generated
#pragma warning(disable : 4512)  // assignment operator could not be generated
#pragma warning(disable : 4663)  // C++ template specialisation change
#endif //!ALL_WARNINGS
#endif //_MSC_VER

#if defined(_MSC_VER)
#pragma warning(disable : 4710)  // Function not inlined
#endif

#include "zdefs.h"

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(none)
#endif

#if defined(_WIN32)
# include <windows.h>

#elif defined(__MVS__)
# include <css.h>

#elif defined(__OS2__)
# include <os2.h>
#endif

extern "C"
{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>
    #include <sys/timeb.h>
    #include <sys/types.h>
    
#if defined(ZCLASS_UNIX)
    #include <unistd.h>
    #include <pthread.h>
#endif
}

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(restore)
#endif

#include "ztrace.h"
#include "zexcept.h"
#include "zreslock.h"
#include "zmstrlck.h"
#include "zptr.h"

#include <vector>
#include <QString>

#if defined(_MSC_VER)
/////////////////////////////////////////////////////////////////////////////
// Turn warnings back on for /W4
#ifndef ALL_WARNINGS
// warnings generated with Standard C++ include files
#pragma warning(default : 4018)  // signed/unsigned mismatch
#pragma warning(default : 4100)  // unreferenced formal parameter
#pragma warning(default : 4146)  // unary minus applied to unsigned
#pragma warning(default : 4201)  // nameless struct/union
#pragma warning(default : 4244)  // loss of precision on conversion
#pragma warning(default : 4245)  // conversion signed/unsigned mismatch
#pragma warning(default : 4511)  // copy ctor could not be generated
#pragma warning(default : 4512)  // assignment operator could not be generated
#pragma warning(default : 4663)  // C++ template specialisation change
#endif //!ALL_WARNINGS
#endif //_MSC_VER

//
// Make sure we are constructed before user objects
//
#if defined(_MSC_VER)
#pragma warning(disable : 4073) // initializers put in library init. area
#pragma init_seg(lib)
#endif

//
// If we're using some compilers/headers, then the struct returned by ftime is a
// struct _timeb rather than a struct timeb.  The only difference is the name.
// Similarly ftime may be known as _ftime. 
// We'll hide this difference with a couple of #defines
// 
#if defined(_MSC_VER)
#define timeb _timeb
#define ftime _ftime
#endif

#define Z_MAX_THREADS 255
int ZTrace::iClState = ZTrace::uninitialized;

ZTrace::Destination ZTrace::iClTraceLocation = ZTrace::standardError;

static unsigned ZTrace__uClIndent[Z_MAX_THREADS] = {0};
static unsigned long ZTrace__ulClProcessId =0;
static bool ZTrace__fCheckMemory = false;
static bool ZTrace__fCheckStack = false;
static const unsigned INDENT_LEVEL = 2;
static unsigned long indentThreadID[Z_MAX_THREADS] = {0};

// ----------------------------------------------------------------------------
//  Lazy evaluation access methods for the statics
// ----------------------------------------------------------------------------
static ZPrivateResource &traceFunction_Lock()
{
  static ZPrivateResource *theLock = 0;
  if (! theLock)
  {
    ZMasterLock lockInit;
    if (! theLock)
        theLock = new ZPrivateResource;
  }
  return *theLock;
}

struct ZTrace_Init : ZException::TraceFn
{
  ZTrace_Init();
  virtual ~ZTrace_Init();
  virtual void write(const char * exceptionText);
};

ZTrace_Init::ZTrace_Init()
{
  // Add this object as the exception trace function
  ZException::setTraceFunction(*this);
}

ZTrace_Init::~ZTrace_Init()
{
}

void ZTrace_Init::write(const char *exceptionText)
{
  ZTrace log;
  if (log.isTraceEnabled())
  {
    log.write(exceptionText);
  }
  else
  {
    fprintf(stderr,"%s\n", exceptionText);
  }
}

class ZTraceSetup
{
public:
  static bool traceOn()
  {
    static ZTrace_Init * initializer;
    if (! initializer)
    {
      ZResourceLock aLock(traceFunction_Lock());
      if (! initializer)
          initializer = new ZTrace_Init;
    }
    if (ZTrace::iClState & ZTrace::uninitialized)
        initialize();
    return 0 != (ZTrace::iClState & ZTrace::on);
  }
  static void initialize();
#if defined (__MVS__)
static long
  setEnvVarsFromFile(const char * const fileid);
static const char * const
  traceControlDDName;
#endif
};

#if defined(__MVS__)
const char * const ZTraceSetup::traceControlDDName = "DD:ZTRCCTL";
#endif

/*------------------------------------------------------------------------------
| ZTrace::ZTrace                                                               |
|                                                                              |
| Default constructor for tracing.                                             |
------------------------------------------------------------------------------*/
ZTrace :: ZTrace(const char* pszTraceName,
                 const char* pszFileName,
                 long lLineNo)
        : pszClTraceName(0)
{
  char acWork[20] = {0};
  if(ZTraceSetup::traceOn())
  {
     if (pszTraceName!=0)
     {
       pszClTraceName = (char*)pszTraceName;
       std::string str(pszTraceName);
       if(isWriteLineNumberEnabled() && lLineNo>0)
       {
         /*
         ** We want just the filename rather than the whole path
         ** and file name ...
         ** Point to last character of filename
         */
         const char * pName = &pszFileName[strlen(pszFileName)-1];
#if defined(_MSC_VER)
#pragma warning(disable : 4127)
#endif
         while (true)
         {
           if( ( pName == ( pszFileName - 1 ) ) ||
             ( *pName == '/' ) ||
             ( *pName == '\\' ) )
           {
             pName++;
             break;
           }

           pName--;
         }
#if defined(_MSC_VER)
#pragma warning(default : 4127)
#endif
         str.append("(").append(pName).append(":");
         sprintf_s(acWork, "%ld", lLineNo); 
         str.append(acWork).append(")");
       }
       writeFormattedString(str, "+");
     }
     else
       pszClTraceName = 0;
  }
}


/*------------------------------------------------------------------------------
| ZTrace::~ZTrace                                                              |
|                                                                              |
| Write exit trace and flush the buffer.                                       |
------------------------------------------------------------------------------*/
ZTrace :: ~ZTrace()
{
   if(ZTraceSetup::traceOn())
   {
      if (pszClTraceName!=0)
      {
        writeFormattedString(std::string(pszClTraceName),"-");
      }
   }
}


/*------------------------------------------------------------------------------
| ZTrace::write                                                                |
|                                                                              |
| Write an string using writeFormattedString after adding the appropriate      |
| padding for indentation.                                                     |
------------------------------------------------------------------------------*/
void ZTrace :: write(const std::string& strString)
{
   if(ZTraceSetup::traceOn())
   {
       writeFormattedString(strString, ">");
   }
#if !defined(NDEBUG) && defined(_WIN32)
   if(ZTrace__fCheckMemory)
     _CrtCheckMemory( );
#endif

}

/*------------------------------------------------------------------------------
| ZTrace::write                                                                |
|                                                                              |
| Write a QString by converting to a std::string and passing it on             |
------------------------------------------------------------------------------*/
void ZTrace::write(const QString& strString)
{
    write(strString.toStdString());
}

#if (0)
/*------------------------------------------------------------------------------
| ZTrace::write                                                                |
|                                                                              |
| Write a char array after adding the appropriate padding for indentation.     |
------------------------------------------------------------------------------*/
void ZTrace::write(const char* pszText)
{
  write(std::string(pszText));
}
#endif

/*------------------------------------------------------------------------------
| ZTrace::write                                                                |
|                                                                              |
| Write a char array after adding the appropriate padding for indentation.     |
------------------------------------------------------------------------------*/
void ZTrace::write(const char* pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	size_t len = std::vsnprintf(NULL, 0, pszFormat, args);
	va_end(args);
	std::vector<char> vec(len + 1);
	va_start(args, pszFormat);
	std::vsnprintf(&vec[0], len + 1, pszFormat, args);
	va_end(args);
	write(std::string(&vec[0]));
}

static constexpr char HEX[] = "0123456789ABCDEF";
/*------------------------------------------------------------------------------
| ZTrace::dumpHex                                                            |
|                                                                              |
| Format a block of data into hex and dump                                     |
|                                                                              |
| Offset    Data                                                               |
| XXXXXXXX: XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX   ................             |
| 0         1         2         3         4         5         6         7      |
| 012345678901234567890123456789012345678901234567890123456789012345678901     |
|                                                                              |
------------------------------------------------------------------------------*/
void ZTrace::dumpHex(const void* DataPointer, size_t DataLength)
{
    if (NULL != DataPointer)
    {
        char             StringBuffer[100] = { '\0' };
        unsigned char* p = (unsigned char*)DataPointer;
        unsigned char* pd;
        unsigned char* pa;
        size_t           PointerSize = sizeof(void*);
        size_t           DataOffset = (PointerSize * 2) + 2;
        size_t           AsciiOffset = DataOffset + 38;
        size_t           LineLength = AsciiOffset + 16;
        size_t           index, i;
        int              n;


        index = 0;
        while (index < DataLength)
        {
            pd = (unsigned char*)StringBuffer + DataOffset;
            pa = (unsigned char*)StringBuffer + AsciiOffset;

            memset(StringBuffer, ' ', LineLength);
            n = snprintf(StringBuffer, sizeof(StringBuffer), "%p", p);
            StringBuffer[n] = ':';

            for (i = 0; (i < 16) && (index < DataLength); i++)
            {
                if ((i % 4) == 0) pd++;

                *pd++ = HEX[*p / 16];
                *pd++ = HEX[*p % 16];
                *pa++ = (' ' == *p || isgraph(*p)) ? *p : '.';

                index++;
                p++;
            }

            ZTrace::write(std::string(StringBuffer));
        }
    }
}



/*------------------------------------------------------------------------------
| ZTrace::writeFormattedString                                                 |
|                                                                              |
| Write a formatted string using writeString.                             |
------------------------------------------------------------------------------*/
void  ZTrace :: writeFormattedString(const std::string& strString,
                                     const char* pszMarker)
{
   ZResourceLock aLock(traceFunction_Lock());

   static unsigned long ulSequence = 0;

   unsigned long ulThreadId = threadId();
   int indentId;

   { 
       int index = ulThreadId % Z_MAX_THREADS;
       int count = Z_MAX_THREADS;
       while (count-- > 0 &&
              indentThreadID[index] != ulThreadId && indentThreadID[index] != 0)
       {
           if (++index >= Z_MAX_THREADS) index = 0;
       }
       if (count == 0)
           indentId = 0;
       else
       {
           indentThreadID[index] = ulThreadId;
           indentId = index;
       }
   }

   if(*pszMarker=='-' && ZTrace__uClIndent[indentId] >= INDENT_LEVEL)
      ZTrace__uClIndent[indentId]-=INDENT_LEVEL;


   std::string strOut(strString);
   std::string strPrefix(ZTrace__uClIndent[indentId], ' ');
   strPrefix+=pszMarker;

   if(isWritePrefixEnabled())
   {
      // We do all the formatting work using traditional C sprintf() rather than
      // using an ostringstream for performance and because we can't guarantee 
      // that everything we need has been built yet.
      char buffer[32] = {0};
      std::string strWork;
      
      //
      // If we are writing the trace record number into the trace file, add this
      // now
      //
      if (isWriteLineNumberEnabled())
      {
        // Output the line number right justified with leading zeros to a width of 8
        sprintf_s(buffer, "%08lu", ulSequence);
        strWork.append(&buffer[0]).append(" ");
        memset(buffer, 0, sizeof(buffer));
      }

      //
      // If we're writing a timestamp, do so now
      //
      if (isWriteTimeStampEnabled())
      {
        struct timeb tstruct = {0,0,0,0};
        struct tm *gmt;
        char timebuff[21] = {0};

        ftime(&tstruct);
        gmt = gmtime(&(tstruct.time));
        strftime(&timebuff[0], sizeof(timebuff) - 1, "%Y/%m/%d %H:%M:%S", gmt);
        sprintf(buffer, "%s.%03u", &timebuff[0], tstruct.millitm);
        strWork.append(&buffer[0]).append(" ");
        memset(buffer, 0, sizeof(buffer));
      }

      // Output the process id right justified with leading zeros to a width of 6
      sprintf(buffer, "%06lu", ZTrace__ulClProcessId);
      strWork.append(&buffer[0]).append(" ");
      memset(buffer, 0, sizeof(buffer));

      // Output the thread id right justified with leading zeros to width of 8
      sprintf(buffer, "%08lx", ulThreadId);
      strWork.append(&buffer[0]).append(" ");
      memset(buffer, 0, sizeof(buffer));

      // If we want the remaining stack size output it now right justified with
      // leading zeros and a width of 7
      if(ZTrace__fCheckStack)
      {
        sprintf(buffer, "%07zd", ZTrace::remainingStack());
        strWork.append(&buffer[0]).append(" ");
        memset(buffer, 0, sizeof(buffer));
      }

      // And finally the prefix string of blanks etc. we built earlier
      strWork+=strPrefix;
      
      // Now extract the new prefix string from the stream
      strPrefix = strWork;
   }

   // Work through the string adding the prefix info where a newline is found
   std::string::size_type endIndex = strOut.length();
   for (std::string::size_type i = 0; i < endIndex; i ++)
   {
     if ('\n' == strOut[i])
     {
       strOut.replace(i, 1, std::string("\n") + strPrefix);
       //
       // BEWARE!!! We've just changed the length of the string.
       // So we need to reset the loop end point.
       //
       endIndex = strOut.length();
     }
   }
   strOut = strPrefix+strOut+"\n";

   writeString(strOut.c_str());

   ulSequence++;
   if(*pszMarker=='+')
      ZTrace__uClIndent[indentId]+=INDENT_LEVEL;

}

/*------------------------------------------------------------------------------
| ZTrace::threadId                                                             |
|                                                                              |
| Determine the threadId of the current thread                                 |
------------------------------------------------------------------------------*/
unsigned long ZTrace::threadId()
{
#if defined(__OS2__)
   unsigned long traceid;

   PTIB    ptib;      /*  Thread Information Block  */
   PPIB    ppib;      /*  Process Information Block */

   DosGetInfoBlocks(&ptib, &ppib);

   traceid = ptib->tib_ptib2->tib2_ultid;
   ZTrace__ulClProcessId = ppib->pib_ulpid;
   return (traceid);

#elif defined(_WIN32)
  ZTrace__ulClProcessId = GetCurrentProcessId();
  unsigned long ourTid = GetCurrentThreadId();
  return (ourTid);

#elif defined(__MVS__)
  unsigned long traceid = 1;
# if defined(CICS)
  ZTrace__ulClProcessId = 1;
  //
  // Need to do something here!
  //

# else
  int retcode = 0;
  CSSGTCB(&traceid, &retcode);  // Get the TCB address as ulong
  CSSGASN(&ZTrace__ulClProcessId, &retcode); // ASID
# endif // defined(CICS)
  return (traceid);

#elif defined(ZCLASS_UNIX)
  ZTrace__ulClProcessId = (unsigned long) getpid();
# if defined(__hpux) && HPUX_VERS<1100
  pthread_t current_thread = pthread_self();
  unsigned long ourTid = (unsigned long)current_thread.field2;
# else
  unsigned long ourTid = (unsigned long)pthread_self();
# endif
  return (ourTid);
#endif 
}

/*------------------------------------------------------------------------------
| ZTrace::remainingStack                                                       |
|                                                                              |
| Determine the size of the remaining stack of the current thread.             |
------------------------------------------------------------------------------*/
size_t ZTrace::remainingStack()
{

#ifdef __OS2__
	PTIB    ptib;      /*  Thread Information Block  */
	PPIB    ppib;      /*  Process Information Block */
	DosGetInfoBlocks(&ptib, &ppib);
	void
		*base = ptib->tib_pstack,
		*cur = &ptib;
	return (char*)cur - (char*)base;
#endif

#ifdef _WIN32
	ZMngPointer <MEMORY_BASIC_INFORMATION> pStackInfo(new MEMORY_BASIC_INFORMATION());

	// The stack pointer (ESP or RSP) will point to the last stack variable in the 
	// function (in this case StackPtr).
	PCHAR StackPtr = (PCHAR)(PVOID)(&StackPtr);

	// Query for r current stack allocation information.
	VirtualQuery(StackPtr, pStackInfo, sizeof(MEMORY_BASIC_INFORMATION));

	// Remaining space on the stack is the current address (StackPtr) minus the base
	// (remember: the stack grows downward in the address space)
	return StackPtr - (PCHAR)(pStackInfo->AllocationBase);
	/*
	PNT_TIB  ptib = 0;
	PVOID    currentEsp = 0;
	__asm
	{
	  mov  eax,FS:[18h]
	  mov  [ptib],eax
	  mov  [currentEsp],esp
	}
	return (char*)currentEsp - (char*)ptib->StackLimit;
	*/

	/*
	CONTEXT context;
	context.ContextFlags = CONTEXT_CONTROL;
	GetThreadContext(GetCurrentThread(), &context);
	void
	  *base = (void *)context.Esp,
	  *cur  = &context;
	return (char*)cur - (char*)base;
	*/
  
#endif

#ifdef __MVS__
  return 0;
#endif

#ifdef __OS400__
  return 0;
#endif

#ifdef ZCLASS_UNIX
  return 0;
#endif
}

/*------------------------------------------------------------------------------
| ZTrace::writeString                                                          |
|                                                                              |
| Write a text string to file.                                                 |
|                                                                              |
| Notes:                                                                       |
|                                                                              |
|   1) We cannot use cout or cerr. since these are C++ objects that may not    |
|      be initialized when this routine is called.                             |
|   2) For Unix, standardOutput and queue are treated identically.             |
------------------------------------------------------------------------------*/
void  ZTrace :: writeString(const char* pszString)
{

   switch(ZTrace::iClTraceLocation)
   {
     case ZTrace::standardError :
       {
         {
           fprintf(stderr, "%s", pszString);
           fflush(stderr);
         }
         break;
       }

     case ZTrace::standardOutput :
       {
         fprintf(stdout, "%s", pszString);
         fflush(stdout);
         break;
       }

     case ZTrace::file :
       {
           FILE* fp{ nullptr };
#if defined(_WIN32)

           // check to see whether the environment variable
           // is defined
           wchar_t* envptr = _wgetenv(L"Z_TRACEFILE");

           // If (environment variable defined) {
           //   - strip the leading and trailing blanks from the value
           //   - use the value as filename
           //   - open the file
           //   - if (open successful) {
           //       - append the provided string the file
           //       - close the file
           //     } else {
           //       - do nothing
           //     }
           // } else {
           //   - do nothing
           // }
           if (envptr)
           {
               std::wstring strTraceOutputFile(envptr);

               // Strip leading and trailing blanks
               std::wstring::size_type index =
                   strTraceOutputFile.find_first_not_of(L' ');
               if (0 != index && std::wstring::npos != index)
                   strTraceOutputFile.erase(0, index);
               index =
                   strTraceOutputFile.find_last_not_of(L' ');
               if (index < (strTraceOutputFile.length() - 1))
                   strTraceOutputFile.erase(1 + index);

               fp = _wfopen(strTraceOutputFile.c_str(), L"a");
           }
#else
           // check to see whether the environment variable
           // is defined
           char* envptr = getenv("Z_TRACEFILE");

           // If (environment variable defined) {
           //   - strip the leading and trailing blanks from the value
           //   - use the value as filename
           //   - open the file
           //   - if (open successful) {
           //       - append the provided string the file
           //       - close the file
           //     } else {
           //       - do nothing
           //     }
           // } else {
           //   - do nothing
           // }
           if (envptr)
           {
               std::string strTraceOutputFile(envptr);

               // Strip leading and trailing blanks
               std::string::size_type index =
                   strTraceOutputFile.find_first_not_of(' ');
               if (0 != index && std::string::npos != index)
                   strTraceOutputFile.erase(0, index);
               index =
                   strTraceOutputFile.find_last_not_of(' ');
               if (index < (strTraceOutputFile.length() - 1))
                   strTraceOutputFile.erase(1 + index);

               fp = fopen(strTraceOutputFile.c_str(), "a");
           }
#endif

           if (fp)
           {
             fprintf(fp, "%s", pszString);
             fclose(fp);
           }
       }
       break;
   }
}



/*------------------------------------------------------------------------------
| ZTraceSetup::initialize                                                      |
|                                                                              |
| Initialize uses the following environment variables to set up the tracing    |
| options.                                                                     |
|                                                                              |
| Environment variables and possible values:                                   |
|   Z_TRACE                                                                    |
|      ON                - turns tracing on                                    |
|      NOPREFIX          - no prefixes                                         |
|   Z_TRACETO                                                                  |
|      STDERR | ERR      - standard error for tracing output                   |
|      STDOUT | OUT      - standard output for tracing output                  |
|      FILE              - tracing output to file                              |
|   Z_CHECKMEMORY                                                              |
|      ON | TRUE         - check memory                                        |
|   Z_CHECKSTACK                                                               |
|      ON | TRUE         - check stack                                         |
------------------------------------------------------------------------------*/

void ZTraceSetup::initialize()
{
   char *envptr = 0;

   std::string::size_type index = 0;
   std::string::iterator si;
   /******************************************************************/
   /* Reset uninitialized bit and setup trace defaults.  OR values   */
   /* in to prevent resetting other trace bits that may have been    */
   /* set PRIOR to initialization being called.                      */
   /******************************************************************/
   ZTrace::iClState &= ~ZTrace::uninitialized;
   ZTrace::iClState |= 
     ZTrace::writeLineNumber | 
     ZTrace::writeTimeStamp | 
     ZTrace::writePrefix;

#if defined(__MVS__)
   setEnvVarsFromFile(ZTraceSetup::traceControlDDName);
#endif

   /* Decide whether tracing is on */
   envptr = getenv("Z_TRACE");
   if (0 != envptr)
   {
     std::string strTrace(envptr);
     // Strip leading and trailing blanks
     index = strTrace.find_first_not_of(' ');
     if (0 != index && std::string::npos != index)
       strTrace.erase(0, index);
     index = strTrace.find_last_not_of(' ');
     if (index < (strTrace.length() - 1))
       strTrace.erase(1+index);
     // and make uppercase
     for(si = strTrace.begin(); si < strTrace.end(); si++)
     {
       *si = (char)toupper(*si);
     }

     if(strTrace== "ON")
        ZTrace::iClState |=ZTrace::on;
     else if(strTrace == "NOPREFIX")
     {
        ZTrace::iClState |=ZTrace::on;
        ZTrace::iClState &= ~ZTrace::writePrefix;
     }
   }

   /* decide where to send the output */
   envptr = getenv("Z_TRACETO");
   if (0 != envptr)
   {
     std::string strTraceTo(envptr);
     // Strip leading and trailing blanks
     index = strTraceTo.find_first_not_of(' ');
     if (0 != index && std::string::npos != index)
       strTraceTo.erase(0, index);
     index = strTraceTo.find_last_not_of(' ');
     if (index < (strTraceTo.length() - 1))
       strTraceTo.erase(1+index);
     // and make uppercase
     for(si = strTraceTo.begin(); si < strTraceTo.end(); si++)
     {
       *si = (char)toupper(*si);
     }

     if(strTraceTo.length() > 2)
     {
        /* Turn tracing on */
        ZTrace::iClState |=ZTrace::on;

        /* Setup output location; default is Standard Error  */
        if(strTraceTo=="STDERR" || strTraceTo=="ERR")
          ZTrace::iClTraceLocation =ZTrace::standardError;
        else if(strTraceTo=="STDOUT" || strTraceTo=="OUT")
          ZTrace::iClTraceLocation=ZTrace::standardOutput;
        else if(strTraceTo=="FILE")
          ZTrace::iClTraceLocation=ZTrace::file;
        else
          ZTrace::iClTraceLocation=ZTrace::standardError;
     }
   }

#ifndef NDEBUG
   /* decide whether to check memory */
   envptr = getenv("Z_CHECKMEMORY");
   if (0 != envptr)
   {
     std::string strCheckMemory(envptr);
     // Strip leading and trailing blanks
     index = strCheckMemory.find_first_not_of(' ');
     if (0 != index && std::string::npos != index)
       strCheckMemory.erase(0, index);
     index = strCheckMemory.find_last_not_of(' ');
     if (index < (strCheckMemory.length() - 1))
       strCheckMemory.erase(1+index);
     // and make uppercase
     for(si = strCheckMemory.begin(); si < strCheckMemory.end(); si++)
     {
       *si = (char)toupper(*si);
     }

     if(strCheckMemory.length() > 1)
     {
        if(strCheckMemory=="ON" | strCheckMemory=="TRUE")
          ZTrace__fCheckMemory = true;
     }
   }
#endif

   /* decide whether to check stack usage */
   envptr = getenv("Z_CHECKSTACK");
   if (0 != envptr)
   {
     std::string strCheckStack(envptr);
     // Strip leading and trailing blanks
     index = strCheckStack.find_first_not_of(' ');
     if (0 != index && std::string::npos != index)
       strCheckStack.erase(0, index);
     index = strCheckStack.find_last_not_of(' ');
     if (index < (strCheckStack.length() - 1))
       strCheckStack.erase(1+index);
     // and make uppercase
     for(si = strCheckStack.begin(); si < strCheckStack.end(); si++)
     {
       *si = (char)toupper(*si);
     }

     if(strCheckStack.length() > 1)
     {
        if(strCheckStack=="ON" | strCheckStack=="TRUE")
          ZTrace__fCheckStack = true;
     }
   }
}

#if defined(__MVS__)
/*------------------------------------------------------------------------------
| ZTraceSetup::setEnvVarsFromFile                                              |
|                                                                              |
| MVS systems do not in general have environment variables set unless you are  |
| running under the OE shell.   However it is possible to set them using the   |
| CRT putenv() API.   So long as the C/C++ environment persists, so will any   |
| variables set using putenv().                                                |
|                                                                              |
| In order to allow the trace to be controlled in a batch address space, we    |
| read a file containing what we hope are lines containing strings of a form   |
| acceptable to putenv().  For example:                                        |
|                                                                              |
| Z_TRACETO=FILE                                                               |
| Z_TRACEFILE=DD:TRACEOUT                                                      |
------------------------------------------------------------------------------*/
long ZTraceSetup::setEnvVarsFromFile(
  const char * const pfname
  )
{
    FILE *pfs = 0;
    char buff[128];
    register char *p,*pt;
    long rc = 0, flag = 0;

    /*
     *  Open the file: if error return immediately
     */
    if ((pfs = fopen(pfname, "r")) == NULL)
       return -1;

    /*
     *  Read all records
     */
    while(fgets(buff, sizeof(buff), pfs)) {

        if (p = strrchr(buff, '\n'))
            *p = '\0';

       /*
        *  Compress blanks between tokens
        */
        flag = 0;
        for(p = pt = buff; *p; p++) {

            switch(flag) {
                case 0:         /* begin scan:
                                   .. compress blank and search for
                                   .. an '=' characeter               */
                    if (*p != ' ') {
                        *pt++ = *p;
                        if (*p == '=')
                           flag = 1;
                    }
                    break;
                case 1:         /* after encounterd an '=' char:
                                   .. compress blank until 1st   not
                                   .. blank character                 */
                    if (*p != ' ') {
                       *pt++ = *p;
                       flag = 2;
                    }
                    break;
                default:        /* after 1st non blank after '=' char
                                   .. copy as it is                   */
                    *pt++ = *p;
                    break;
            }
        }
        *pt = '\0';

        /*
         *  Skip blank and comment line
         */
        if ((buff[0] != '\0') && (buff[0] != ';'))
            if (putenv(buff) == 0)
                rc++;
    }

    /*
     *  Check for any error encounterd
     */
    if (feof(pfs)) {

        fclose(pfs);
        return (rc);

    } else {

        fclose(pfs);
        return (-1);
    }

}
#endif

void ZTrace :: enableWriteLineNumber()
{
  ZTrace::iClState |= ZTrace::writeLineNumber;
}

void ZTrace :: disableWriteLineNumber()

{
  ZTrace::iClState &= ~ZTrace::writeLineNumber;
}

bool ZTrace :: isWriteLineNumberEnabled()
{
  return 0 != (ZTrace::iClState & ZTrace::writeLineNumber);
}


void ZTrace :: enableWriteTimeStamp()
{
  ZTrace::iClState |= ZTrace::writeTimeStamp;
}

void ZTrace :: disableWriteTimeStamp()

{
  ZTrace::iClState &= ~ZTrace::writeTimeStamp;
}


bool ZTrace :: isWriteTimeStampEnabled()
{
  return 0 != (ZTrace::iClState & ZTrace::writeTimeStamp);
}


void ZTrace::enableTrace()
{
  ZTrace::iClState |= ZTrace::on;
}

void ZTrace::disableTrace()
{
  ZTrace::iClState &= ~ZTrace::on;
}

bool ZTrace::isTraceEnabled()
{
  return 0 != (ZTrace::iClState & ZTrace::on);
}

void ZTrace::writeToStandardError()
{
   ZTrace::iClTraceLocation = ZTrace::standardError;
}

void ZTrace::writeToStandardOutput()
{
   ZTrace::iClTraceLocation = ZTrace::standardOutput;
}

void ZTrace::writeToFile()
{
   ZTrace::iClTraceLocation = ZTrace::file;
}

ZTrace::Destination ZTrace::traceDestination()
{
   return(ZTrace::iClTraceLocation);
}

void ZTrace::enableWritePrefix()
{
  ZTrace::iClState |= ZTrace::writePrefix;
}

void ZTrace::disableWritePrefix()
{
  ZTrace::iClState &= ~ZTrace::writePrefix;
}

bool ZTrace::isWritePrefixEnabled()
{
  return 0 != (ZTrace::iClState & ZTrace::writePrefix);
}

void ZTrace::writeDebugLocation(const char* str, const ZExceptionLocation& location)
{
  std::string text("");
  if (str) 
    text = str;
  text += std::string("< file: ") + location.fileName();
  text += std::string(" function: ") + location.functionName();
  char buffer[11] = {0};
  sprintf(buffer, "%lu", location.lineNumber());
  text += std::string(" line: ") + std::string(buffer);
  ZTrace::write(text);
}
#if (0)
// Modifications:
//
// +MH+ $Log$
//
// -MH-
#endif

#if defined(_MSC_VER)
#ifndef ALL_WARNINGS
// warnings generated with Standard C++ include files
#pragma warning(disable : 4018)  // signed/unsigned mismatch
#pragma warning(disable : 4100)  // unreferenced formal parameter
#pragma warning(disable : 4146)  // unary minus applied to unsigned
#pragma warning(disable : 4201)  // nameless struct/union
#pragma warning(disable : 4244)  // loss of precision on conversion
#pragma warning(disable : 4245)  // conversion signed/unsigned mismatch
#pragma warning(disable : 4511)  // copy ctor could not be generated
#pragma warning(disable : 4512)  // assignment operator could not be generated
#pragma warning(disable : 4663)  // C++ template specialisation change
#endif //!ALL_WARNINGS
#endif //_MSC_VER
