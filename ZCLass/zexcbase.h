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
// ZException Class Hierarchy definition
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

/*******************************************************************************
* FILE NAME: zexcbase.h                                                        *
*                                                                              *
* DESCRIPTION:                                                                 *
*   Declaration of the classes:                                                *
*     ZExceptionLocation                                                       *
*     ZException                                                               *
*     ZAccessError                                                             *
*     ZAssertionFailure                                                        *
*     ZDeviceError                                                             *
*     ZInvalidParameter                                                        *
*     ZInvalidRequest                                                          *
*     ZResourceExhausted                                                       *
*     ZOutOfMemory                                                             *
*     ZOutOfSystemResource                                                     *
*     ZOutOfWindowResource                                                     *
*                                                                              *
*   This file also contains many of the macros used to implement the           *
*   library exception handling mechanism.  This includes the ZASSERT, ZTHROW,  *
*   ZRETHROW, ZEXCEPTION_LOCATION, ZEXCLASSDECLARE, and ZEXCLASSIMPLEMENT      *
*   macros.                                                                    *
*                                                                              *
*******************************************************************************/

#ifndef ZDEFS_INCLUDED
#include <zdefs.h>
#endif

class ZExcText;

class ZExport ZOutOfMemoryHandler
{
public:
/**
 * Use an instance of this class to cause a ZOutOfMemory exception
 * to be thrown when new fails to allocate memory.
 *
 * You can use this class in one of two ways:
 *
 * 1) To globally set the out of memory handler on an application wide
 *    basis, use the singleton creation capability of the the class by
 *    using ZOutOfMemoryHandler::createInstance().   This will persist
 *    until your application terminates.
 *
 * 2) For a finer level of control, you can create an instance of this
 *    class yourself, and delete it explicitly when it is no longer
 *    needed.  Typically you would create it on the stack so that when 
 *    it goes out of scope it will be deleted.
 *
 */
  static void createInstance();
  ~ZOutOfMemoryHandler();
  ZOutOfMemoryHandler();
private:
#if defined(_MSC_VER)
  static int newHandler(size_t);
#else
  static void newHandler();
#endif
  void *fOriginalNewHandler;

  //
  // Pointer to the single instance of the class.
  //
  static ZOutOfMemoryHandler* outOfMemoryHandler;

};

class ZExport ZExceptionLocation
{
/**
 *
 * Objects of this class save the location information when you throw or
 * rethrow an exception.  The class itself throws no exceptions as that's
 * the last thing you want when your in the middle of throwing one yourself.
 *
 * Typically, you would use either the ZTHROW or ZRETHROW macros to create
 * the ZException location object.  You can if you wish create your own 
 * ZExceptionLocation object by constructing it yourself or using the 
 * ZEXCEPTION_LOCATION macro.
 */
public:
/**
 * Constructor
 *
 * You can create objects of this class by either:
 *
 * *  Using the ctor
 *
 *    fileName      The source file containing the function that created this
 *                  object.
 *
 *    functionName  The name of the function
 *
 *    lineNumber    The line number of the statement at which the object was
 *                  created.
 *
 * *  Using the ZEXCEPTION_LOCATION macro. This macro captures the current 
 *    location information using compiler defined macros
 */
  ZExceptionLocation (const char *  fileName = 0,
                      const char *  functionName = 0,
                      unsigned long lineNumber = 0);
//
// Note that the DEFAULT destructor is used. We only hold the
// supplied pointers and number, we don't own the storage they 
// point to which will typically be in the constant segment ... 
//

//
// Getters
//
/**
 *
 * Public functions:
 *
 *  fileName()      returns the source file name where the exception was thrown
 *                  or rethrown.
 *
 *  functionName()  returns the name of the function in which ...
 *
 *  lineNumber()    returns the line number in the file at which ...
 */
const char * fileName () const;
const char * functionName() const;

unsigned long
  lineNumber() const;

//
// Attributes
//
private:
const char * pfileName;
const char * pfunctionName;

unsigned long
  lineNum;
};

//
// Definition of the ZEXCEPTION_LOCATION macro
//
#if defined (__FUNCTION__)
  #define   ZEXCEPTION_LOCATION() ZExceptionLocation(__FILE__, __FUNCTION__, __LINE__)
#else
  #define   ZEXCEPTION_LOCATION() ZExceptionLocation(__FILE__, 0, __LINE__)
#endif

//
// Define the exception throwing macros
//
#define ZTHROW(exc)\
  exc.addLocation(ZEXCEPTION_LOCATION());\
  exc.logExceptionData();\
  throw exc;

#define ZRETHROW(exc)\
  exc.addLocation(ZEXCEPTION_LOCATION());\
  exc.logExceptionData();\
  throw;

class ZExport ZException
{
/**
 *
 *  ZException
 *
 *  This is a base class from which you can derive other exception classes.  None
 *  of the methods of this clas may throw an exception as that's the last thing
 *  you want while you're in the middle of throwing one yourself.
 *
 *  If you use the ZException and derived classes, then the C++ new handler is 
 *  set so that an ZOutOfMemory exception will be thrown when the heap is used up.
 *
 *  Each object contains:
 *
 *    A stack of exception text message strings
 *    
 *    An error id
 *  
 *    A severity code
 *
 *    An error code group
 *    
 *    Information about where the exception was thrown/rethrown
 *
 *  ZException provides all the functionality required by itself and any derived 
 *  classes (that's a statement that you really don't want to sub-class this 
 *  except using the supplied macros).  Think hard before doing so ...
 *
 *  A number of derived classes are pre-defined and you can easily derive your
 *  own subclass using the supplied macros.  Normally you wouldn't want to create
 *  an object of this class (ZException), but would create and throw an object of
 *  a derived class.
 *
 *  The hierarchy of the derived classes is defined lower down this header file
 *  where you find the ZEXCLASSDECLARE macro definition.  The hierarchy is 
 *  defined using that macro.
 *
 *  Also supplied is a ZEXCLASSIMPLEMENT macro which allows you to implement a
 *  derived class of ZException or one of its derived classes.
 *
 */

public:

enum Severity { unrecoverable, recoverable };

/**
 *
 * Constructors:
 *
 * The main constructor is defined below. It takes the following parameters
 *
 *  errorText   The text describing the error (const char *)
 *
 *  errorId     (optional) The identifier you want associated with this error
 *
 *  severity    (optional) Use the enumeration ZException::Severity to specify
 *              the severity of the error.  The default is unrecoverable.
 *
 * A copy ctor is also supplied for the compiler to use when the exception is
 * thrown
 *
 * You should create your ZException objects on the stack and throw them not
 * their address.
 *
 *  e.g.
 *  
 *    MyZExcptionDerivedClass ex("Something horrid this way comes");
 *    ZTHROW(ex);
 *
 * You should catch them by reference to avoid slicing.
 *
 *  e.g.
 *
 *  catch(MyZExceptionDerivedClass& ex)
 *  {
 *    do things here
 *    // possibly rethrow the exception thus ...
 *    ZRETHROW(ex);
 *  }
 */
  ZException( const char*       errorText,
              unsigned long     errorId = 0,
              Severity          severity = ZException::unrecoverable );
  ZException( const ZException& exception);

virtual
 ~ZException();

class TraceFn 
{
/**
 *
 *  ZException::TraceFn embedded class
 *
 * Objects of class ZException and its derived classes use ZException::TraceFn
 * to log exception data.
 *
 * A default derived class is provided as part of the ZException class which
 * writes all the log information to stderr.
 * 
 * If you use the ZTrace class, it registers a TraceFn derived object which
 * overrides the write() function.  If the trace is active, it uses ZTrace to
 * write out the data buffers, so that the exceptions will be logged to
 * wherever the Z_TRACETO environment variable points.  If the trace
 * is not active, then the trace data will just be written to stderr. 
 *
 * If you wish to write your own logging mechanism, derive your own class from
 * ZException::TraceFn and register it using ZException::setTraceFunction.
 *
 * You can completely take over exception logging by overriding the logData
 * function (look at the code for ZException::TraceFn::logData in ZExcDase.cpp
 * for guidance). You are passed the ZException object so you can completely
 * customise the logging of exception data.
 *
 * If all you want to do is change how the data is written (e.g. to a file or
 * named pipe), you only need override the write function.
 *
 * If you want to write all the data somewhere with one call, you can use the
 * exceptionLogged function which is called when the last buffer of data has 
 * been passed to the write function.   This lets you collect all the data by 
 * overriding only write and exceptionLogged and triggering the write from the
 * call to exceptionLogged.
 *
 * The constructor is protected. So you can only create objects of this class 
 * using a derived class.
 */

public:
virtual void write   ( const char * buffer );
virtual void logData ( ZException& exception);

protected:
// Constructors
  TraceFn ();

virtual void exceptionLogged ( );
}; // TraceFn

//
// ********** Error code 
//
typedef const char * ErrorCodeGroup;

/**
 *
 * Just some constant text strings for determining the source of the error
 */
static ErrorCodeGroup const baseLibrary;
static ErrorCodeGroup const CLibrary;
static ErrorCodeGroup const operatingSystem;
static ErrorCodeGroup const presentationSystem;
static ErrorCodeGroup const other;

/**
 *
 * Set the id of the source of the error into the exception
 */
ZException &setErrorCodeGroup ( ErrorCodeGroup errorGroup );

/**
 *
 * Get the id of the source of the error
 */
ErrorCodeGroup errorCodeGroup () const;
//
// ********** Exception Text
//
/**
 * Append the supplied errorText to the text string at the top of the
 * stack of error strings.
 */
ZException &appendText (const char * errorText);
/**
 * Add the supplied errorText to the top of the exception text stack.
 */ 
ZException &setText    (const char * errorText);

/**
 * Returns a const char *  pointing to an exception text string at the
 * specified location in the stack. The default is to return the topmost
 * entry.  If you supply an invalid index, a null pointer is returned.
 */ 
const char *text ( unsigned long indexFromTop = 0) const;
/**
 * Returns the number of text strings in the exception text stack.
 */
unsigned long textCount() const;

//
// ********** Exception Severity
//
/**
 * This can be used to override the severity of the exception which
 * was initially set by the thrower. Severity is either unrecoverable
 * or recoverable.
 */
ZException &setSeverity ( Severity severity );
/**
 * Returns true if the exception is recoverable.
 */
virtual bool isRecoverable () const;

//
// ********** Error Information
//
/**
 * Set the error ID - perhaps a function return code, or an application defined
 * error code.
 */
ZException &setErrorId (unsigned long errorId);
/**
 * Returns the error ID of this exception
 */
unsigned long errorId () const;

//
// ********** Exception Location
//
/**
 * Adds location information to the exception.  If you use the ZTHROW/ZRETHROW
 * macros, this will be done for you automagically when the exception is thrown
 * of rethrown. An array of up to five exception locations is held in the 
 * exception object.
 */
virtual ZException &addLocation (const ZExceptionLocation& location);
/**
 * Returns the number of locations held in the exception location array.
 */
unsigned long locationCount () const;
/**
 * Returns the ZExceptionLocation object at the specified index. The 0th. entry
 * is the location at which the exception was originally thrown.  If you supply
 * and invalid index a null pointer is returned.
 */
const ZExceptionLocation *locationAtIndex ( unsigned long locationIndex = 0) const;

//
// ********** Exception Type
//
/**
 * Returns the name of this object's class.
 */
virtual const char *name () const;

//
// ********** Exception Logging
//
/**
 * Register a replacement object of class ZException::TraceFn to be used to log
 * exception data.  You are returned a pointer to the previous object so that 
 * you can restore the status quo ante when you no longer wish to override the
 * default behaviour.  If you do not register your own tracing object, exception
 * data is written to standard error. 
 */
static ZException::TraceFn *setTraceFunction (ZException::TraceFn& traceFunction);
/**
 * Logs the exception data using the current TraceFn object.  Used by the ZTHROW
 * and ZRETHROW macros.
 */
virtual ZException &logExceptionData ();

//
// ********** Throw Support
//
/**
 * To provide support for the ZASSERT macro. Its function is to:
 *
 * * Create a ZAssertionFailure exception.
 * * Add the location information to it
 * * Log the exception data.
 * * Throw the exception.
 */
static void  
  assertParameter ( const char *       exceptionText, 
                    ZExceptionLocation location );

private:
//
// Nobody assigns me
//
ZException
 &operator = (const ZException& exc);

//
// If this gal really needs to rummage in my privates she can
//
friend class ZException::TraceFn;

Severity
  exsevCl;            // Unrecoverable/recoverable

unsigned long
  ulClErrorId;        // The error code supplied by the user

//
// Surely no-one will want to pass an exception through more than
// 4 additional catch/rethrow cycles ... whatever, we limit it to
// 5 entries.  If someone exceeds, just overwrite the last one
//
ZExceptionLocation
  exlocClArray[5];    // Array of exception locations

unsigned long
  ulexlocClCount;     // Number of valid exception locations
unsigned long
  ulClTxtLvlCount;    // Number of levels of messages we have

ZExcText
 *msgtxtClTop;        // -> first text object

ErrorCodeGroup
  fErrorGroup;        // -> text defining the error code group
}; // ZException

#if !defined(NDEBUG)
  #define ZASSERT(test)\
      if(!(test))\
      {\
         ZException::assertParameter("The following expression must be true, but evaluated to false: " #test,\
         ZEXCEPTION_LOCATION());\
      }
#else
  #define ZASSERT(test)
#endif

/**
 * ZEXCLASSDECLARE macro
 * 
 * Create a declaration of a derived class of ZException or one of its
 * derived classes.
 */
#define ZEXCLASSDECLARE(child,parent) class ZExport child : public parent {\
public:\
  child(const char* a, unsigned long b = 0,\
        Severity c = ZException::unrecoverable);\
  virtual const char* name() const;\
  virtual ~child();\
  child(const child &);\
private:\
  child& operator = ( const child & );\
}

/**
 * ZEXCLASSIMPLEMENT macro
 * 
 * Create a definition/implementation of a derived class of ZException or one 
 * of its derived classes.
 */
#define ZEXCLASSIMPLEMENT(child,parent)\
  child :: child(const char* a, unsigned long b,\
                 Severity c)\
    : parent(a, b, c)\
        { }\
  const char* child :: name() const\
    {\
     return ( # child);\
    }\
  child :: ~child() {;}\
  child :: child(const child & a) : parent(a) {;}

  /****************************************************************/
  /* Deeclaration of ZException subclasses using macro.           */
  /****************************************************************/

ZEXCLASSDECLARE(ZAccessError,ZException);
ZEXCLASSDECLARE(ZAssertionFailure,ZException);
ZEXCLASSDECLARE(ZDeviceError,ZException);
ZEXCLASSDECLARE(ZInvalidParameter,ZException);
ZEXCLASSDECLARE(ZInvalidRequest,ZException);
ZEXCLASSDECLARE(ZResourceExhausted,ZException);
  ZEXCLASSDECLARE(ZOutOfMemory,ZResourceExhausted);
  ZEXCLASSDECLARE(ZOutOfSystemResource,ZResourceExhausted);
  ZEXCLASSDECLARE(ZOutOfWindowResource,ZResourceExhausted);

/*-------------------------- Pragma Library Support --------------------------*/

#if (0)
// Modifications:
//
// +MH+ $Log$
//
// -MH-
#endif

