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
//   Implementation of the classes
//    ZBaseErrorInfo
//    ZGUIErrorInfo
//    ZSystemErrorInfo
//    ZCLibErrorInfo
//
//  This file contains the implementation of classes/functions declared
// in zexcept.h.
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

#include "zdefs.h"

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(none)
#endif

extern "C"
{
#ifdef _WIN32
  #include <windows.h>
#endif
#if defined(ZCLASS_UNIX)
    #include <unistd.h>
    #include <pthread.h>
#endif
  #include <string.h>
  #include <errno.h>
}

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(restore)
#endif

#include <zexcept.h>

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
#pragma warning(disable : 4702)  // Unreachable code
#endif

#define ZC_ASSERTPARM 1

/*------------------------------------------------------------------------------
| ZBaseErrorInfo::ZBaseErrorInfo                                               |
------------------------------------------------------------------------------*/
ZBaseErrorInfo::ZBaseErrorInfo ( )
{ }

/*------------------------------------------------------------------------------
| ZBaseErrorInfo::~ZBaseErrorInfo                                              |
------------------------------------------------------------------------------*/
ZBaseErrorInfo::~ZBaseErrorInfo ( )
{ }

/*------------------------------------------------------------------------------
| ZBaseErrorInfo::throwError                                                   |
------------------------------------------------------------------------------*/
void ZBaseErrorInfo :: throwError ( const ZExceptionLocation&  location,
                                ExceptionType              name,
                                ZException::Severity       severity,
                                ZException::ErrorCodeGroup errorGroup )
{
  switch (name)
  {
    case accessError:
    {
      ZAccessError exc(text(), errorId(), severity);
      exc.setErrorCodeGroup(errorGroup);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case deviceError:
    {
      ZDeviceError exc(text(), errorId(), severity);
      exc.setErrorCodeGroup(errorGroup);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case invalidParameter:
    {
      ZInvalidParameter exc(text(), errorId(), severity);
      exc.setErrorCodeGroup(errorGroup);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case invalidRequest:
    {
      ZInvalidRequest exc(text(), errorId(), severity);
      exc.setErrorCodeGroup(errorGroup);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case outOfSystemResource:
    {
      ZOutOfSystemResource exc(text(), errorId(), severity);
      exc.setErrorCodeGroup(errorGroup);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case outOfWindowResource:
    {
      ZOutOfWindowResource exc(text(), errorId(), severity);
      exc.setErrorCodeGroup(errorGroup);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case outOfMemory:
    {
      ZOutOfMemory exc(text(), errorId(), severity);
      exc.setErrorCodeGroup(errorGroup);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case resourceExhausted:
    {
      ZResourceExhausted exc(text(), errorId(), severity);
      exc.setErrorCodeGroup(errorGroup);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
  } /* endswitch */
}


/*------------------------------------------------------------------------------
| ZGUIErrorInfo::ZGUIErrorInfo                                                 |
------------------------------------------------------------------------------*/
ZGUIErrorInfo::ZGUIErrorInfo ( const char *pGUIFunctionName )
{
  const char* pErrText = "No error text is available.";
#if defined(_WIN32)
  this->ulClErrorId = GetLastError();
  char szText[512];
  /********************************************************************/
  /* Format the message from the system message table resources       */
  /********************************************************************/
  size_t szMessageLength = 
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                  0,
                  this->ulClErrorId,
                  // Default language
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR)szText,
                  255,
                  NULL);
  if( szMessageLength)
  {
     /********************************************************************/
     /* Remove CRLF control characters if found                          */
     /********************************************************************/
     if ((szText[szMessageLength-2] == '\r')  &&
         (szText[szMessageLength-1] == '\n'))
     {
       szText[szMessageLength-2] = 0;
     }
     pErrText = szText;
  };
#endif

  if (pGUIFunctionName)
  {
    this->pClFuncNameText = new char[strlen(pGUIFunctionName) +
                                     strlen(pErrText)+2];
    strcpy(this->pClFuncNameText, pGUIFunctionName);
    strcat(this->pClFuncNameText, ":");
    strcat(this->pClFuncNameText, pErrText);
  }
  else
  {
    this->pClFuncNameText = new char[strlen(pErrText)+1];
    strcpy(this->pClFuncNameText, pErrText);
  }
}

/*------------------------------------------------------------------------------
| ZGUIErrorInfo::~ZGUIErrorInfo                                                |
------------------------------------------------------------------------------*/
ZGUIErrorInfo::~ZGUIErrorInfo ( )
{
  if (this->pClFuncNameText)
    delete [] this->pClFuncNameText;
}

/*------------------------------------------------------------------------------
| ZGUIErrorInfo::text                                                          |
------------------------------------------------------------------------------*/
const char* ZGUIErrorInfo::text ( ) const
{
  return( this->pClFuncNameText );
}

/*------------------------------------------------------------------------------
| ZGUIErrorInfo::errorId                                                       |
------------------------------------------------------------------------------*/
unsigned long ZGUIErrorInfo::errorId ( ) const
{
  return( this->ulClErrorId );
}

/*------------------------------------------------------------------------------
| ZGUIErrorInfo::operator const char*                                          |
------------------------------------------------------------------------------*/
ZGUIErrorInfo::operator const char* () const
{
  return( this->pClFuncNameText );
}

/*------------------------------------------------------------------------------
| ZGUIErrorInfo::isAvailable                                                   |
------------------------------------------------------------------------------*/
bool ZGUIErrorInfo::isAvailable ( ) const
{
  if (this->ulClErrorId != 0)
    return( 1 );
  else
    return( 0 );
}

/*------------------------------------------------------------------------------
| ZGUIErrorInfo::throwGUIError                                                 |
------------------------------------------------------------------------------*/
void ZGUIErrorInfo :: throwGUIError ( const char*               pFunctionName,
                                      const ZExceptionLocation  &location,
                                      ZBaseErrorInfo::ExceptionType name,
                                      ZException::Severity      severity )
{
  ZGUIErrorInfo errInfo(pFunctionName);
  errInfo.throwError(location, name, severity, ZException::presentationSystem );
}

/*------------------------------------------------------------------------------
| ZSystemErrorInfo::ZSystemErrorInfo                                           |
------------------------------------------------------------------------------*/
ZSystemErrorInfo::ZSystemErrorInfo ( unsigned long ulSystemErrorId,
                                     const char    *pSystemFunctionName )
: bClError(true)
, ulClErrorId(ulSystemErrorId)
{
  static const char defaultMessage[] =
    "No error text is available.";

  char szText[512];
  unsigned long ulRc = 1;
  size_t szMessageLength = 0;
#if defined(_WIN32)
  /********************************************************************/
  /* Format the message from the system message table resources       */
  /********************************************************************/
  szMessageLength =
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                  0,
                  ulSystemErrorId,
                  // Default language
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR)szText,
                  255,
                  NULL);

  /********************************************************************/
  /* Handle message not found condition                               */
  /********************************************************************/
  if (szMessageLength)
     ulRc = 0;
  else
     ulRc = 1;
#endif

  if (ulRc)
  {
    this->bClError = false;
    szMessageLength = strlen(defaultMessage);
    strcpy(szText, defaultMessage);
  }

  /********************************************************************/
  /* Remove CRLF control characters if found                          */
  /********************************************************************/
  if ((szText[szMessageLength-2] == '\r')  &&
      (szText[szMessageLength-1] == '\n'))
  {
    szMessageLength = szMessageLength - 2;
  }

  if (pSystemFunctionName)
  {
    this->pClErrorText = new char[szMessageLength +
                                  strlen(pSystemFunctionName) + 2];
    strcpy(this->pClErrorText, pSystemFunctionName);
    strcat(this->pClErrorText, ":");
    strncat(this->pClErrorText, szText, szMessageLength);
  }
  else
  {
    this->pClErrorText = new char[szMessageLength + 1];
    strncpy(this->pClErrorText, szText, szMessageLength);
    this->pClErrorText[szMessageLength] = '\0';
  }
}

/*------------------------------------------------------------------------------
| ZSystemErrorInfo::~ZSystemErrorInfo                                          |
------------------------------------------------------------------------------*/
ZSystemErrorInfo::~ZSystemErrorInfo ( )
{
  if (this->pClErrorText)
    delete [] this->pClErrorText;
}

/*------------------------------------------------------------------------------
| ZSystemErrorInfo::text                                                       |
------------------------------------------------------------------------------*/
const char* ZSystemErrorInfo::text ( ) const
{
  return( (const char*)this->pClErrorText );
}

/*------------------------------------------------------------------------------
| ZSystemErrorInfo::operator const char*                                       |
------------------------------------------------------------------------------*/
ZSystemErrorInfo::operator const char* () const
{
  return( text() );
}

/*------------------------------------------------------------------------------
| ZSystemErrorInfo::errorId                                                    |
------------------------------------------------------------------------------*/
unsigned long ZSystemErrorInfo::errorId ( ) const
{
  return( this->ulClErrorId );
}

/*------------------------------------------------------------------------------
| ZSystemErrorInfo::isAvailable                                                |
------------------------------------------------------------------------------*/
bool ZSystemErrorInfo::isAvailable ( ) const
{
  if (!this->bClError)
    return( 0 );

  return( 1 );
}

/*------------------------------------------------------------------------------
| ZSystemErrorInfo::throwSystemError                                           |
------------------------------------------------------------------------------*/
void ZSystemErrorInfo ::
                 throwSystemError ( unsigned long             ulSystemErrorId,
                                    const char                *pFunctionName,
                                    const ZExceptionLocation  &location,
                                    ZBaseErrorInfo::ExceptionType name,
                                    ZException::Severity      severity )
{
  ZSystemErrorInfo errInfo(ulSystemErrorId, pFunctionName);

  switch (name)
  {
    case accessError:
    {
      ZAccessError exc(errInfo.text(), errInfo.errorId(), severity);
      exc.setErrorCodeGroup(ZException::operatingSystem);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case deviceError:
    {
      ZDeviceError exc(errInfo.text(), errInfo.errorId(), severity);
      exc.setErrorCodeGroup(ZException::operatingSystem);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case invalidParameter:
    {
      ZInvalidParameter exc(errInfo.text(), errInfo.errorId(), severity);
      exc.setErrorCodeGroup(ZException::operatingSystem);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case invalidRequest:
    {
      ZInvalidRequest exc(errInfo.text(), errInfo.errorId(), severity);
      exc.setErrorCodeGroup(ZException::operatingSystem);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case outOfSystemResource:
    {
      ZOutOfSystemResource exc(errInfo.text(), errInfo.errorId(), severity);
      exc.setErrorCodeGroup(ZException::operatingSystem);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case outOfWindowResource:
    {
      ZOutOfWindowResource exc(errInfo.text(), errInfo.errorId(), severity);
      exc.setErrorCodeGroup(ZException::operatingSystem);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case outOfMemory:
    {
      ZOutOfMemory exc(errInfo.text(), errInfo.errorId(), severity);
      exc.setErrorCodeGroup(ZException::operatingSystem);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
    case resourceExhausted:
    {
      ZResourceExhausted exc(errInfo.text(), errInfo.errorId(), severity);
      exc.setErrorCodeGroup(ZException::operatingSystem);
      exc.addLocation(location);
      exc.logExceptionData();
      throw exc;
    }
  } /* endswitch */
}

/*------------------------------------------------------------------------------
| ZCLibErrorInfo::~ZClibErrorInfo                                              |
------------------------------------------------------------------------------*/
ZCLibErrorInfo::~ZCLibErrorInfo ( )
{
  if (this->pClErrorText)
    delete [] this->pClErrorText;
}

/*------------------------------------------------------------------------------
| ZCLibErrorInfo::text                                                         |
------------------------------------------------------------------------------*/
const char* ZCLibErrorInfo::text ( ) const
{
  return( (const char*)this->pClErrorText );
}

/*------------------------------------------------------------------------------
| ZCLibErrorInfo::operator const char*                                         |
------------------------------------------------------------------------------*/
ZCLibErrorInfo::operator const char* () const
{
  return( text() );
}

/*------------------------------------------------------------------------------
| ZCLibErrorInfo::errorId                                                      |
------------------------------------------------------------------------------*/
unsigned long ZCLibErrorInfo::errorId ( ) const
{
  return( this->ulClErrno );
}

/*------------------------------------------------------------------------------
| ZCLibErrorInfo::isAvailable                                                  |
------------------------------------------------------------------------------*/
bool ZCLibErrorInfo::isAvailable ( ) const
{
  return( this->bClError );
}

/*------------------------------------------------------------------------------
| ZCLibErrorInfo::ZCLibErrorInfo                                               |
|                                                                              |
|  The strerror C function is used to retrieve the error text for errno.       |
------------------------------------------------------------------------------*/
ZCLibErrorInfo::ZCLibErrorInfo ( const char *pCLibFunctionName )
{
  char *pErrorStr = strerror(errno);
  char noErrorText[] = "No Error text is available";

  this->ulClErrno = (unsigned long) errno;

  this->bClError = strlen(pErrorStr) ? true : false;

  if (pCLibFunctionName)
  {
    if (this->bClError)
    {
      this->pClErrorText = new char[strlen(pErrorStr) +
                                    strlen(pCLibFunctionName) + 2];
      strcpy(this->pClErrorText, pCLibFunctionName);
      strcat(this->pClErrorText, ":");
      strcat(this->pClErrorText, pErrorStr);
    }
    else
    {
      this->pClErrorText = new char[strlen(noErrorText) +
                                    strlen(pCLibFunctionName) + 2];
      strcpy(this->pClErrorText, pCLibFunctionName);
      strcat(this->pClErrorText, ":");
      strcat(this->pClErrorText, noErrorText);
    }
  }
  else
  {
    if (this->bClError)
    {
      this->pClErrorText = new char[strlen(pErrorStr) + 1];
      strcpy(this->pClErrorText, pErrorStr);
    }
    else
    {
      this->pClErrorText = new char[strlen(noErrorText) + 1];
      strcpy(this->pClErrorText, noErrorText);
    }
  }
}

/*------------------------------------------------------------------------------
| ZCLibErrorInfo::throwCLibError                                               |
------------------------------------------------------------------------------*/
void ZCLibErrorInfo::throwCLibError( const char                *pFunctionName,
                                     const ZExceptionLocation  &location,
                                     ZBaseErrorInfo::ExceptionType name,
                                     ZException::Severity      severity )
{
  ZCLibErrorInfo clibError( pFunctionName );
  clibError.throwError( location, name, severity, ZException::CLibrary );
}

/*------------------------------------------------------------------------------
| ZExcept__assertParameter                                                     |
------------------------------------------------------------------------------*/
void ZExcept__assertParameter ( const char               *pAssertExpression,
                                const ZExceptionLocation &location )
{
  ZInvalidParameter invParm(pAssertExpression,
                            ZC_ASSERTPARM,
                            ZException::recoverable);
  invParm.setErrorCodeGroup(ZException::other);
  invParm.addLocation(location);
  invParm.logExceptionData();
  throw invParm;
}

/*------------------------------------------------------------------------------
| ZExcept__assertState                                                         |
------------------------------------------------------------------------------*/
void ZExcept__assertState ( const char               *pAssertExpression,
                            const ZExceptionLocation &location )
{
  ZInvalidRequest invReq(pAssertExpression,
                         ZC_ASSERTPARM,
                         ZException::recoverable);
  invReq.setErrorCodeGroup(ZException::other);
  invReq.addLocation(location);
  invReq.logExceptionData();
  throw invReq;
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
