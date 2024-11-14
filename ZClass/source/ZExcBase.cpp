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
// ZException Class Hierarchy implementation
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

#if defined(_WINDOWS) && !defined(NDEBUG)
#include "vld.h"
#endif

#include "zdefs.h"

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(none)
#endif

extern "C" {
#include <stdio.h>
#include <string.h>
#if defined(ZCLASS_UNIX)
    #include <unistd.h>
    #include <pthread.h>
#endif
}

#include <new>

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(restore)
#endif

#include "zexcbase.h"
#include "zmstrlck.h"
#include "zreslock.h"

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

#if defined(_MSC_VER)
#pragma warning(disable : 4710)  // Function not inlined
#endif

// Initialize ErrorCodeGroup strings
ZException::ErrorCodeGroup const ZException::baseLibrary = "Z Class Library";
ZException::ErrorCodeGroup const ZException::CLibrary = "Standard C Library";
ZException::ErrorCodeGroup const ZException::operatingSystem = "Operating System";
ZException::ErrorCodeGroup const ZException::presentationSystem = "Presentation System";
ZException::ErrorCodeGroup const ZException::other = "Other Error Group";

static const char* headerText =
         "%s exception thrown.\n   function: %s\n   file: %s\n   line: %d";

// ----------------------------------------------------------------------------
//  Lazy evaluation access methods for the statics
// ----------------------------------------------------------------------------
static ZPrivateResource &traceFunction_Lock()
{
  static ZPrivateResource *theLock = 0;
  if (! theLock)
  {
#if defined(_WINDOWS) && !defined(NDEBUG)
    // Visual Leak Detector reports these as memory leaks, which is technically correct
    // but as we know about them and they are harmless turn leak detection off here,
    // and turn it on again after the allocation
    VLDDisable();
#endif

    ZMasterLock lockInit;
    if (! theLock)
        theLock = new ZPrivateResource;

#if defined(_WINDOWS) && !defined(NDEBUG)
    VLDEnable();
#endif
  }
  return *theLock;
}

static ZException::TraceFn *pTraceFn = 0;
static ZException::TraceFn *pUserTraceFn = 0;

// ----------------------------------------------------------------------------
//  A dummy derivative of the standard trace function
// ----------------------------------------------------------------------------
class ZExceptTraceFn : public ZException::TraceFn
{  }; // ZExceptTraceFn


static ZException::TraceFn& traceFunction()
{
  static ZExceptTraceFn * pDefaultTraceFn = 0;
  if(pUserTraceFn)
    return *pUserTraceFn;

  if (! pDefaultTraceFn)
  {
#if defined(_WINDOWS) && !defined(NDEBUG)
      // Visual Leak Detector reports these as memory leaks, which is technically correct
      // but as we know about them and they are harmless turn leak detection off here,
      // and turn it on again after the allocation
      VLDDisable();
#endif

    // Get access to the local lock
    ZResourceLock aLock(traceFunction_Lock());
    if (! pDefaultTraceFn)
        pDefaultTraceFn = new ZExceptTraceFn;

#if defined(_WINDOWS) && !defined(NDEBUG)
    VLDEnable();
#endif

  }

  if(!pTraceFn)
    pTraceFn = pDefaultTraceFn;
  return *pTraceFn;
}

class ZExcText {
public:
  ZExcText  ( const char* errText,
              const ZExcText* msgtxtOld );
  ZExcText  ( const ZExcText& msg );

  ~ZExcText ( ) {
       if (pszClMsg)
          delete [] pszClMsg;
       if (msgtxtClNext)
          delete msgtxtClNext; }

const char
 *text ( ) const {
   return pszClMsg; }

ZExcText
 *next ( ) const {
   return msgtxtClNext; }

void
  appendText ( const char* errText );

char
 *pszClMsg;
ZExcText
 *msgtxtClNext;
}; // ZExcText

#if defined(_MSC_VER)
//
// If we're using MS Visual C we need to map any calls to set_new_handler()
// so that we call _set_new_handler() which has a different function signature
// This function does the dirty for us ...
//
/*
new_handler __cdecl set_new_handler(new_handler new_p) 
{
  return (new_handler) _set_new_handler((_PNH) new_p);
}
*/
#endif

//
// The actual pointer to the real instance of the ZOutOfMemoryHandler
//
ZOutOfMemoryHandler* ZOutOfMemoryHandler::outOfMemoryHandler = 0;

void ZOutOfMemoryHandler::createInstance()
{
  if (! outOfMemoryHandler)
  {
    ZMasterLock lockInit;
    if (! outOfMemoryHandler)
        outOfMemoryHandler = new ZOutOfMemoryHandler;
  }
  return;
}

ZOutOfMemoryHandler :: ZOutOfMemoryHandler()
{
  //
  // Register newHandler function.  The nasty cast is to handle
  // the transformation of the function signature MS Visual C++
  // _set_new_handler() so that it appears as if it is the Standard
  // C++ set_new_handler()
  //
  std::set_new_handler((void(*)())ZOutOfMemoryHandler::newHandler);
}

ZOutOfMemoryHandler :: ~ZOutOfMemoryHandler()
{
  std::set_new_handler((void(*)())fOriginalNewHandler);
}

#if defined(_MSC_VER)
#pragma warning(disable : 4702)  // Unreachable code
int ZOutOfMemoryHandler::newHandler(size_t size)
{
  size = size;
  ZOutOfMemory exception("Unable to allocate memory", 0, ZException::unrecoverable);
  ZTHROW(exception);
}
#pragma warning(default : 4702)  // Unreachable code
#else
void ZOutOfMemoryHandler::newHandler()
{
  ZOutOfMemory exception("Unable to allocate memory", 0, ZException::unrecoverable);
  ZTHROW(exception);
}
#endif

/****************************************************************/
/*  ZExcText Member Functions                                   */
/****************************************************************/

ZExcText :: ZExcText ( const char* errText,
                       const ZExcText* msgtxtOld )
: pszClMsg(0)
/****************************************************************/
/* Main ZExcText Constructor                                    */
/****************************************************************/
{
    void(*poldnh)() = std::set_new_handler(0);
    pszClMsg = new char[strlen(errText)+1];
    if (pszClMsg)
      strcpy(pszClMsg, errText);
    msgtxtClNext = (ZExcText*)msgtxtOld;
    std::set_new_handler(poldnh);
}

ZExcText :: ZExcText ( const ZExcText& msg )
: pszClMsg(0)
/****************************************************************/
/* Copy Constructor                                             */
/****************************************************************/
{
    void(*poldnh)() = std::set_new_handler(0);
    pszClMsg = new char[strlen(msg.pszClMsg)+1];
    if (pszClMsg)
    {
       strcpy(pszClMsg, msg.pszClMsg);
       if (msg.msgtxtClNext)
       {
          msgtxtClNext = new ZExcText(*(msg.msgtxtClNext));
       }
       else msgtxtClNext = 0;
    }
    else
    {
       pszClMsg = msg.pszClMsg;
       msgtxtClNext = msg.msgtxtClNext;
    }
    std::set_new_handler(poldnh);
}

void ZExcText :: appendText ( const char* errText )
/****************************************************************/
/*                                                              */
/****************************************************************/
{
    void(*poldnh)() = std::set_new_handler(0);
    char* pszText;
    if (pszClMsg)
       pszText = new char[strlen(pszClMsg)+strlen(errText)+1];
    else
       pszText = new char[strlen(errText)+1];
    if (pszText)
    {
      if (pszClMsg)
      {
         strcpy(pszText, pszClMsg);
         delete [] pszClMsg;
         strcat(pszText, errText);
         pszClMsg = pszText;
      }
      else
      {
         strcpy(pszText, errText);
         pszClMsg = pszText;
      }
    }
    std::set_new_handler(poldnh);
}

/****************************************************************/
/*  ZExceptionLocation Member Functions                         */
/****************************************************************/


ZExceptionLocation::ZExceptionLocation (const char *  fileName,
                                        const char *  functionName,
                                        unsigned long lineNumber)
: pfileName(fileName)
, pfunctionName(functionName)
, lineNum(lineNumber)
/****************************************************************/
/* Constructor for ZExceptionLocation.                          */
/****************************************************************/
{
}

const char * ZExceptionLocation::fileName () const
/****************************************************************/
/* Returns the path qualified file name where an exception was  */
/* thrown or re-thrown via the ZTHROW macro.                    */
/****************************************************************/
{
  return pfileName;
}

const char * ZExceptionLocation::functionName() const
/****************************************************************/
/* Returns the name of the function from which an exception was */
/* thrown or rethrown via the ZTHROW macro.                     */
/****************************************************************/
{
   if (pfunctionName)
      return pfunctionName;
   return "Unknown";
}

unsigned long ZExceptionLocation::lineNumber() const
/****************************************************************/
/* Returns the line number in the source file where an          */
/* exception was thrown or re-thrown via the ZTHROW macro.      */
/****************************************************************/
{
  return lineNum;
}

/****************************************************************/
/*  ZException Member Functions                                 */
/****************************************************************/

ZException::ZException(const char* exMsgText,
                       unsigned long errorId,
                       Severity exSev)
: exsevCl(exSev)
, ulClErrorId(errorId)
, ulexlocClCount(0)
, fErrorGroup(baseLibrary)
/****************************************************************/
/* Constructor. The exception text is passed in.                */
/****************************************************************/
{
   if (exMsgText != 0)
   {
      void(*poldnh)() = std::set_new_handler(0);
      msgtxtClTop = new ZExcText(exMsgText, 0);
      if (msgtxtClTop)
        ulClTxtLvlCount = 1;
      else
        ulClTxtLvlCount = 0;
      std::set_new_handler(poldnh);
   }
   else         // null pointer to msg passed
   {
      ulClTxtLvlCount = 0;
      msgtxtClTop = 0;
   }
}

ZException::ZException(const ZException& exc)
/****************************************************************/
/* Copy constructor                                             */
/****************************************************************/
{
   fErrorGroup = exc.fErrorGroup;
   exsevCl = exc.exsevCl;
   ulClErrorId = exc.ulClErrorId;
   ulexlocClCount = exc.ulexlocClCount;
   ulClTxtLvlCount = exc.ulClTxtLvlCount;
   if (exc.msgtxtClTop != 0)
     msgtxtClTop = new ZExcText(*(exc.msgtxtClTop));
   else msgtxtClTop = 0;
   for (unsigned long i=0; i<exc.locationCount(); i++ )
   {
      exlocClArray[i] = exc.exlocClArray[i];
   } /* endfor */
}

ZException::~ZException()
/****************************************************************/
/* Destructor.                                                  */
/****************************************************************/
{
   if (msgtxtClTop)
     delete msgtxtClTop;
}

ZException& ZException::setErrorCodeGroup ( ErrorCodeGroup errorGroup )
/****************************************************************/
/* Sets the error code group.                                   */
/****************************************************************/
{
   fErrorGroup = errorGroup;
   return *this;
}

ZException::ErrorCodeGroup ZException::errorCodeGroup ( ) const
/****************************************************************/
/* Returns the error code group.                                */
/****************************************************************/
{
   return fErrorGroup;
}

ZException& ZException::appendText(const char* exMsgText)
/****************************************************************/
/* Concatenates a string to the exception text.                 */
/****************************************************************/
{
   if (exMsgText != 0)
   {
      if (msgtxtClTop)
        msgtxtClTop->appendText(exMsgText);
      else
        setText(exMsgText);
   }
   return *this;
}

ZException& ZException::setText(const char* exMsgText)
/****************************************************************/
/* Adds a new ZExcText to the top of the message "stack".       */
/****************************************************************/
{
   if (exMsgText != 0)
   {
      void(*poldnh)() = std::set_new_handler(0);
      ZExcText* msgtxtOld = msgtxtClTop;
      msgtxtClTop = new ZExcText(exMsgText, msgtxtOld);
      if (msgtxtClTop)
         ulClTxtLvlCount++;
      else msgtxtClTop= msgtxtOld;
      std::set_new_handler(poldnh);
   }
    return *this;
}

ZException& ZException::setSeverity(Severity exsev)
/****************************************************************/
/* Sets the exception severity level.                           */
/****************************************************************/
{
   exsevCl = exsev;
   return *this;
}

ZException& ZException::setErrorId(unsigned long errorId)
/****************************************************************/
/* Sets the Error Id.                                           */
/****************************************************************/
{
   ulClErrorId = errorId;
   return *this;
}

ZException& ZException::addLocation(const ZExceptionLocation& exloc)
/****************************************************************/
/* Adds the location information to the instance data of the    */
/* exception class.  Used by the ZTHROW macro.                  */
/****************************************************************/
{
   if(ulexlocClCount == 5)
      ulexlocClCount--;          // reset to fill in last entry
   exlocClArray[ulexlocClCount] = exloc;
   ulexlocClCount++;
   return *this;
}

bool ZException::isRecoverable() const
/****************************************************************/
/* Return true if severity is recoverable.                      */
/****************************************************************/
{
   return (recoverable == exsevCl);
}

const char* ZException::name() const
/****************************************************************/
/* Returns the name of the exception class.                     */
/****************************************************************/
{
   return "ZException";
}

const char* ZException::text(unsigned long indexFromTop) const
/****************************************************************/
/* Returns the exception text from an entry in the stack.       */
/****************************************************************/
{
  ZExcText* tmp = msgtxtClTop;
  if (textCount() > indexFromTop)
  {
     for (unsigned long i = 0; i < indexFromTop; ++i)
     {
        tmp = tmp->next();
     } /* endfor */
     return tmp->text();
  }
  else
     return 0;
}

unsigned long  ZException::textCount() const
/****************************************************************/
/* Returns the number of levels of message text.                */
/****************************************************************/
{
   return ulClTxtLvlCount;
}

unsigned long ZException::errorId() const
/****************************************************************/
/* Returns the error Id.                                        */
/****************************************************************/
{
  return ulClErrorId;
}

unsigned long ZException :: locationCount() const
/****************************************************************/
/* Return the number of locations in the array.                 */
/****************************************************************/
{
   return ulexlocClCount;
}

const ZExceptionLocation* ZException :: locationAtIndex(unsigned long locIndex) const
/****************************************************************/
/* Return a pointer to the exception location information at    */
/* the index specified.  Index is from 0 to max - 1             */
/****************************************************************/
{
   if (locIndex < ulexlocClCount)
   {
     return &(exlocClArray[locIndex]);
   }
   return 0;
}

ZException& ZException :: logExceptionData ( )
/****************************************************************/
/* Log out error information                                    */
/****************************************************************/
{
  traceFunction().logData(*this);
  return *this;
}

ZException::TraceFn*
ZException::setTraceFunction(ZException::TraceFn& newFunction)
/****************************************************************/
/* Set the pointer to the exception logging function            */
/****************************************************************/
{
  ZResourceLock aLock(traceFunction_Lock());
  ZException::TraceFn* oldFunction = &(traceFunction());
  pUserTraceFn = &newFunction;
  return oldFunction;
}

#if defined(_MSC_VER)
#pragma warning(disable : 4702)  // Unreachable code
#endif
void ZException :: assertParameter ( const char* exceptionText,
                                     ZExceptionLocation location )
/****************************************************************/
/****************************************************************/
{
   ZAssertionFailure invParm(exceptionText);
   invParm.addLocation(location);
   invParm.setErrorCodeGroup(other);
   invParm.logExceptionData();
   throw invParm;
}
#if defined(_MSC_VER)
#pragma warning(default : 4702)  // Unreachable code
#endif

  /****************************************************************/
  /* ZException::TraceFn functions.                               */
  /****************************************************************/

ZException::TraceFn::TraceFn ( )
{ }

void ZException::TraceFn::exceptionLogged ( )
/****************************************************************/
/* Default completion function.                                 */
/****************************************************************/
{ }

void ZException::TraceFn::write ( const char* buffer )
/****************************************************************/
/* Default write function.                                      */
/****************************************************************/
{
   fprintf(stderr,"%s\n", buffer);
}

void ZException::TraceFn::logData ( ZException& exception )
/****************************************************************/
/* Default logData function.                                    */
/****************************************************************/
{
  void(*poldnh)() = std::set_new_handler(0);

  // Determine the largest buffer we will need.
  size_t maxLength = 200;
  size_t length = 0;
  if (exception.ulexlocClCount != 0)
     length = strlen(::headerText) + 1 +
              strlen(exception.name()) +
              strlen(exception.exlocClArray[exception.ulexlocClCount - 1].functionName()) +
              strlen(exception.exlocClArray[exception.ulexlocClCount - 1].fileName()) + 8;
  if (length > maxLength)
     maxLength = length;
  if (exception.msgtxtClTop)
  {
     for (int i = (exception.textCount()-1); i >= 0 ;i-- )
       {
          length = strlen(exception.text(i)) + 6;
          if (length > maxLength)
             maxLength = length;
       } /* endfor */
  }

  char * pbuf = new char[maxLength + 1];

  if (pbuf)
  {
    if (exception.ulexlocClCount != 0)
    {
      sprintf(pbuf,::headerText,
              exception.name(),
              exception.exlocClArray[exception.ulexlocClCount - 1].functionName(),
              exception.exlocClArray[exception.ulexlocClCount - 1].fileName(),
              exception.exlocClArray[exception.ulexlocClCount - 1].lineNumber());
    } else
      sprintf(pbuf,"\n%s exception thrown.\n   Location information is unavailable.", exception.name());
    traceFunction().write(pbuf);
    if (exception.errorId())
      sprintf(pbuf,"   Error Id is %ld", exception.errorId());
    else
      sprintf(pbuf,"   Error Id is unavailable.");
    traceFunction().write(pbuf);
    if (exception.errorCodeGroup())
       sprintf(pbuf,"   Error Code group is %s", exception.errorCodeGroup());
    else
      sprintf(pbuf,"    Error Code group is unavailable.");
    traceFunction().write(pbuf);
    if (exception.msgtxtClTop)
    {
       sprintf(pbuf,"   Exception text is:");
       traceFunction().write(pbuf);
       for (int i = (exception.textCount()-1); i >= 0 ;i-- )
         {
            sprintf(pbuf,"      %s", exception.text(i));
            traceFunction().write(pbuf);
         } /* endfor */
    }
    else
    {
       sprintf(pbuf,"   Exception text is unavailable.");
       traceFunction().write(pbuf);
    }
    delete [] pbuf;
    exceptionLogged();
  } else                       // new failed, no buffer available
  {
    if (exception.ulexlocClCount != 0)
    {
      fprintf(stderr,
        "%s exception thrown.\n   function: %s\n   file: %s\n   line: %ld.\n",
        exception.name(),
        exception.exlocClArray[exception.ulexlocClCount - 1].functionName(),
        exception.exlocClArray[exception.ulexlocClCount - 1].fileName(),
        exception.exlocClArray[exception.ulexlocClCount - 1].lineNumber());
    } else
      fprintf(stderr,
        "\n%s exception thrown.\n   Location information is unavailable.\n",
        exception.name());
    if (exception.errorId())
      fprintf(stderr,
        "   Error ID is %ld\n",
        exception.errorId());
    else
      fprintf(stderr,"   Error ID is unavailable.\n");
    if (exception.errorCodeGroup())
       fprintf(stderr,
         "  Error Code group is %s\n", exception.errorCodeGroup());
    else
      fprintf(stderr,
        "   Error Code group is unavailable.\n");
    if (exception.msgtxtClTop)
    {
       fprintf(stderr,"   Exception text is:\n");
       for (int i = (exception.textCount()-1); i >= 0 ;i-- )
         {
            fprintf(stderr,"      %s\n", exception.text(i));
         } /* endfor */
    }
    else
       fprintf(stderr,"   Exception text is unavailable.\n");
  } /* endif */

  std::set_new_handler(poldnh);
}

  /****************************************************************/
  /* Definition of ZException subclasses using macro.             */
  /****************************************************************/

ZEXCLASSIMPLEMENT(ZAccessError, ZException);
ZEXCLASSIMPLEMENT(ZAssertionFailure, ZException);
ZEXCLASSIMPLEMENT(ZDeviceError, ZException);
ZEXCLASSIMPLEMENT(ZInvalidParameter, ZException);
ZEXCLASSIMPLEMENT(ZInvalidRequest, ZException);
ZEXCLASSIMPLEMENT(ZResourceExhausted, ZException);
  ZEXCLASSIMPLEMENT(ZOutOfMemory, ZResourceExhausted);
  ZEXCLASSIMPLEMENT(ZOutOfSystemResource, ZResourceExhausted);
  ZEXCLASSIMPLEMENT(ZOutOfWindowResource, ZResourceExhausted);

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
