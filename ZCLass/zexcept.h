#pragma once
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
//   Declaration of the classes
//    ZBaseErrorInfo
//    ZGUIErrorInfo
//    ZSystemErrorInfo
//    ZCLibErrorInfo
//
//  This file also contains several macros and functions which can be used
//  to facilitate throwing exceptions.  This includes the ZASSERTPARM and
//  ZASSERTSTATE macros, and the ZExcept__assertParameter and
//  ZExcept__assertState functions.
//                                                                             
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

#include <zexcbase.h>

#ifndef ZDEFS_INCLUDED
#include <zdefs.h>
#endif

class ZExport ZBaseErrorInfo 
{
public:
/*------------------------------- Constructors ------------------------------ */
 ZBaseErrorInfo  ( );

virtual
 ~ZBaseErrorInfo ( );

/*---------------------------- Error Information -----------------------------*/
/**
 * the const char* cast operator returns the error text.
 */
virtual
  operator const char* ( ) const = 0;

/**
 * text returns the error text.
 */
virtual const char
 *text ( ) const = 0;

/**
 * Returns the errorId.
 */
virtual unsigned long
  errorId ( ) const = 0;

/**
 * If error information is available, true is returned.
 */
virtual bool
  isAvailable ( ) const = 0;

enum ExceptionType { accessError, deviceError, invalidParameter,
                     invalidRequest, outOfSystemResource, outOfWindowResource,
                     outOfMemory, resourceExhausted };

/*------------------------------ Throw Support -------------------------------*/
void
  throwError ( const ZExceptionLocation&  location,
               ExceptionType              name = accessError,
               ZException::Severity       severity = ZException::recoverable,
               ZException::ErrorCodeGroup errorGroup =
                                             ZException::baseLibrary );

}; // ZBaseErrorInfo

class ZExport ZGUIErrorInfo : public ZBaseErrorInfo {
/**
 *
 * Objects of this class represent error information you can include in an 
 * exception object.  If you get an API error from an Windows API, you can 
 * construct an object of this class. You can then use the error text to 
 * build a derived class of ZException.
 *
 * The following macros are provided for throwing exceptions constructed with
 * ZGUIErrorInfo information:
 *
 * ZTHROWGUIERROR
 *
 * This macro takes as its only parameter the name of the GUI function that 
 * returned an error condition. The macro then calls ZGUIErrorInfo::throwGUIError
 * which:
 *
 * 1. Creates a ZGUIErrorInfo object
 * 2. Uses the object to create an object of type ZAccessError
 * 3. Adds the ZException::presentationSystem error group to the object
 * 4. Adds location information
 * 5. Logs the exception
 * 6. Throws the exception
 *
 * ZTHROWGUIERROR2
 *
 * This macro can throw any ZException derived excpetion. It accepts the
 * following parameters:
 *
 * function         The name of the GUI function returning the error code.
 * exceptionType    The type of exception to be thrown. Use the ZBaseErrorInfo::
 *                  ExceptionType enumeration to specify the type of exception.
 *                  The default is ZAccessError.
 * severity         Use the enumeration ZException::Severity to specify the 
 *                  of the error. The default is recoverable.
 *
 * The macro calls ZGUIErrorInfo::throwGUIError which:
 *
 * 1. Creates a ZGUIErrorInfo object
 * 2. Uses the object to create an exception object of the specified type
 * 3. Adds the ZException::presentationSystem error group to the object
 * 4. Adds location information
 * 5. Logs the exception
 * 6. Throws the exception
 */

typedef ZBaseErrorInfo
  Inherited;
public:
/*------------------------------- Constructors -------------------------------*/
/**
 * You can only construct objects of this class using this constructor.  Copy 
 * construction and assignment is disallowed.  The parameter specifies the name
 * of the failing GUI function.
 */
  ZGUIErrorInfo ( const char* GUIFunctionName = 0 );

virtual
 ~ZGUIErrorInfo ( );

/*---------------------------- Error Information -----------------------------*/
/**
 * the const char* cast operator returns the error text.
 */
virtual
  operator const char* ( ) const;

/**
 * text returns the error text.
 */
virtual const char
 *text ( ) const;

/**
 * Returns the errorId.  In the case of a Windows GUI error, the constructor obtains
 * the errorId using getLastError.
 */
virtual unsigned long
  errorId ( ) const;

/**
 * If error information is available, true is returned.
 */
virtual bool
  isAvailable ( ) const;

/*------------------------------ Throw Support -------------------------------*/
/**
 * Used by ZTHROWGUIERROR(2) to throw the exception.
 */
static void
  throwGUIError  ( const char*               functionName,
                   const ZExceptionLocation& location,
                   ZBaseErrorInfo::ExceptionType name = accessError,
                   ZException::Severity      severity = ZException::recoverable );

private:
/*----------------------------- Hidden Functions -----------------------------*/
  ZGUIErrorInfo ( const ZGUIErrorInfo& errorInformation );

ZGUIErrorInfo
 &operator= ( const ZGUIErrorInfo& errorInformation );

/*--------------------------------- Private ----------------------------------*/
  unsigned long
    ulClErrorId;

char
 *pClFuncNameText;

}; // ZGUIErrorInfo

class ZExport ZSystemErrorInfo : public ZBaseErrorInfo {
/**
 *
 * Objects of this class represent error information you can include in an 
 * exception object.  When a system API fails, you can use the supplied macro
 * to construct objects of the ZSystemErrorInfo class, and throw an exception.
 *
 * The following macro is provided for throwing exceptions constructed with
 * ZSystemErrorInfo information:
 *
 * ZTHROWSYSTEMERROR
 *
 * This macro accepts the following parameters:
 *
 * errorId          The error code returned from the system API
 * function         The name of the system API that returned the error
 * exceptionType    The type of exception to be thrown. Use the ZBaseErrorInfo::
 *                  ExceptionType enumeration to specify the type of exception.
 *                  The default is ZAccessError.
 * severity         Use the enumeration ZException::Severity to specify the 
 *                  of the error. The default is recoverable.
 *
 * The macro calls ZSystemErrorInfo::throwSystemError which:
 *
 * 1. Creates a ZSystemErrorInfo object
 * 2. Uses the object to create an exception object of the specified type
 * 3. Adds the ZException::operatingSystem error group to the object
 * 4. Adds location information
 * 5. Logs the exception
 * 6. Throws the exception
 */

typedef ZBaseErrorInfo
  Inherited;
public:
/*------------------------------- Constructors -------------------------------*/
/**
 * You can only construct objects of this class using this constructor.  Copy 
 * construction and assignment is disallowed.  The parameters are:
 *
 *  systemErrorId       error code from the system API
 *  systemFunctionName  name of the system API returning the error code.
 *
 * The constructor will attempt to extract the system error message from the 
 * system message table resources using the FormatMessage API.  If no error text
 * can be obtained, the default text "No error text is available." will be used.
 */
  ZSystemErrorInfo ( unsigned long systemErrorId,
                     const char* systemFunctionName = 0 );

virtual
 ~ZSystemErrorInfo ( );

/*---------------------------- Error Information -----------------------------*/
/**
 * the const char* cast operator returns the error text.
 */
virtual
  operator const char* ( ) const;

/**
 * text returns the error text.
 */
virtual const char
 *text ( ) const;

/**
 * Returns the errorId.
 */
virtual unsigned long
  errorId ( ) const;

/**
 * If error information is available, true is returned.
 */
virtual bool
  isAvailable ( ) const;

/*------------------------------ Throw Support -------------------------------*/
/**
 * Used by ZTHROWSYSTEMERROR to throw the exception.
 */
static void
  throwSystemError ( unsigned long             systemErrorId,
                     const char*               functionName,
                     const ZExceptionLocation& location,
                     ZBaseErrorInfo::ExceptionType name = accessError,
                     ZException::Severity      severity = ZException::recoverable );

private:
  ZSystemErrorInfo ( const ZSystemErrorInfo& errorInformation );

ZSystemErrorInfo
 &operator= ( const ZSystemErrorInfo& errorInformation );

/*--------------------------------- Private ----------------------------------*/
bool
  bClError;
unsigned long
  ulClErrorId;
char
 *pClErrorText;

}; // ZSystemErrorInfo

class ZExport ZCLibErrorInfo : public ZBaseErrorInfo {
/**
 *
 * Objects of this class represent error information you can include in an 
 * exception object.  When a C library API fails, you can use the supplied macro
 * to construct objects of the ZCLibErrorInfo class, and throw an exception.
 *
 * The following macro is provided for throwing exceptions constructed with
 * ZClibErrorInfo information:
 *
 * ZTHROWCLIBERROR
 *
 * This macro accepts the following parameters:
 *
 * function         The name of the C library API that returned the error
 * exceptionType    The type of exception to be thrown. Use the ZBaseErrorInfo::
 *                  ExceptionType enumeration to specify the type of exception.
 *                  The default is ZAccessError.
 * severity         Use the enumeration ZException::Severity to specify the 
 *                  of the error. The default is recoverable.
 *
 * The macro calls ZCLibErrorInfo::throwCLibError which:
 *
 * 1. Creates a ZCLibErrorInfo object
 * 2. Uses the object to create an exception object of the specified type
 * 3. Adds the ZException::CLibrary error group to the object
 * 4. Adds location information
 * 5. Logs the exception
 * 6. Throws the exception
 */
typedef ZBaseErrorInfo
  Inherited;
public:
/*------------------------------- Constructors -------------------------------*/
/**
 * You can only construct objects of this class using this constructor.  Copy 
 * construction and assignment is disallowed.  The parameters are:
 *
 *  CLibFunctionName  name of the C library API returning the error code.
 *
 * The constructor will set the errorId from the C variable errno. The 
 * strerror C function is used to retrieve the error text for errno.
 * If no error text can be obtained, the default text "No error text is available." 
 * will be used.
 */
ZCLibErrorInfo ( const char* CLibFunctionName = 0 );

virtual
 ~ZCLibErrorInfo ( );

/*---------------------------- Error Information -----------------------------*/
/**
 * the const char* cast operator returns the error text.
 */
virtual
operator const char* ( ) const;

/**
 * text returns the error text.
 */
virtual const char
 *text ( ) const;

/**
 * Returns the errorId which is set to the value of errno.
 */
virtual unsigned long
  errorId ( ) const;

/**
 * If error information is available, true is returned.
 */
virtual bool
  isAvailable ( ) const;

/*------------------------------ Throw Support -------------------------------*/
/**
 * Used by ZTHROWCLIBERROR to throw the exception.
 */
static void
  throwCLibError (   const char*               functionName,
                     const ZExceptionLocation& location,
                     ZBaseErrorInfo::ExceptionType name = accessError,
                     ZException::Severity      severity = ZException::recoverable );

private:
/*----------------------------- Hidden Functions -----------------------------*/
  ZCLibErrorInfo ( const ZCLibErrorInfo& errorInformation );

ZCLibErrorInfo
 &operator= ( const ZCLibErrorInfo& errorInformation );

/*--------------------------------- Private ----------------------------------*/
bool
  bClError;
unsigned long
  ulClErrno;
char
 *pClErrorText;

}; // ZCLibErrorInfo


void ZExport
  ZExcept__assertParameter    ( const char*               assertExpression,
                                const ZExceptionLocation& location );
void ZExport
  ZExcept__assertState        ( const char*               assertExpression,
                                const ZExceptionLocation& location );

#define ZASSERTPARM(test)\
    if(!(test))\
    {\
       ZExcept__assertParameter("The following expression must be true, but evaluated to false: " #test,\
       ZEXCEPTION_LOCATION());\
    }

#define ZASSERTSTATE(test)\
    if(!(test))\
    {\
       ZExcept__assertState("The following expression must be true, but evaluated to false: " #test,\
       ZEXCEPTION_LOCATION());\
    }

#define ZTHROWGUIERROR(func)\
    ZGUIErrorInfo::throwGUIError(func, ZEXCEPTION_LOCATION())

#define ZTHROWGUIERROR2(func, exType, sev)\
    ZGUIErrorInfo::throwGUIError(func, ZEXCEPTION_LOCATION(), exType, sev)

#define ZTHROWSYSTEMERROR(id, func, exType, sev)\
    ZSystemErrorInfo::throwSystemError(id, func, \
                                       ZEXCEPTION_LOCATION(), exType, sev)

#define ZTHROWCLIBERROR(func, exType, sev)\
    ZCLibErrorInfo::throwCLibError(func, ZEXCEPTION_LOCATION(), exType, sev)

#if (0)
// Modifications:
//
// +MH+ $Log$
//
// -MH-
#endif

