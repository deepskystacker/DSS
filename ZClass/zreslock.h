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
//  Declaration of the classes:
//    ZResource
//    ZPrivateResource
//    ZSharedResource
//    ZResourceLock
//
// Freely cribbed in spirit if not in exact implementation from the 
// IBM Open Class IResource and related classes.
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

class ZResourceLock;
class ZPrivateResource;
class ZSharedResource;

#if !defined(ZDEFS_INCLUDED)
#include <zdefs.h>
#endif

#if defined(ZCLASS_UNIX)
#include <unistd.h>
#include <pthread.h>
#endif

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

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(none)
#endif

#include <string>

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(restore)
#endif

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

#if defined(_WIN32)
typedef HANDLE PRIVATE_SEMAPHORE_HANDLE_TYPE;
typedef HANDLE SHARED_SEMAPHORE_HANDLE_TYPE;
#elif defined(__MVS__)
#elif defined(ZCLASS_UNIX)
typedef pthread_mutex_t * PRIVATE_SEMAPHORE_HANDLE_TYPE;
typedef int SHARED_SEMAPHORE_HANDLE_TYPE;
#else 
#error Unsupported operating system
#endif

class ZExport ZResource  {
public:
/*------------------------------- Constructors -------------------------------*/
  ZResource( );
virtual
 ~ZResource( );

/*----------------------------- Resource Locking -----------------------------*/
virtual ZResource& lock           ( long timeOut = -1 ) = 0;
virtual ZResource& unlock         ( ) = 0;

private:
/*------------------------------ Hidden Members ------------------------------*/
  ZResource    ( const ZResource& resource );
ZResource
 &operator=    ( const ZResource& resource );

/*--------------------------------- Private ----------------------------------*/
#if defined(_WIN32) || defined(__MVS__)
unsigned long
  lockingThread;
#elif defined(ZCLASS_UNIX)
unsigned long
  lockingThread;
#else
#error Unsupported operating system
#endif

long
  recursionCount;

friend class ZResourceLock;
friend class ZPrivateResource;
friend class ZSharedResource;
}; // ZResource


class ZExport ZPrivateResource : public ZResource
{
typedef ZResource
  Inherited;
public:
/*------------------------------- Constructors -------------------------------*/
  ZPrivateResource   ( );
virtual
 ~ZPrivateResource   ( );
virtual ZResource& lock           ( long timeOut = -1 );
virtual ZResource& unlock         ( );

#if defined(__MVS__)
//
// ONLY on MVS we use a generated ENQ resource name
//
std::string
  keyName           ( ) const { return keyNameCl; }
#endif

protected:
#if !defined(__MVS__)
//
// The following method does not exist on MVS ... we don't use a handle
// there, just an ENQ name which we build ourself internally
//
PRIVATE_SEMAPHORE_HANDLE_TYPE
 &handle();
#endif

private:
/*------------------------------ Hidden Members ------------------------------*/
  ZPrivateResource    ( const ZPrivateResource& privateResource );
ZPrivateResource
 &operator=           ( const ZPrivateResource& privateResource );

/*--------------------------------- Private ----------------------------------*/
#if defined(__MVS__)
//
// On MVS ZPrivateResource and ZSharedResource are effectively the same,
// as both of them just ENQ on a resource name.   In fact the ONLY difference
// is that ZPrivateResource generates the resource name internally in the 
// constructor.   ZSharedResource, OTOH, requires the resource name to be 
// supplied to the constructor.
//
std::string
  keyNameCl;
#else
PRIVATE_SEMAPHORE_HANDLE_TYPE
  resourceHandleCl;
#endif

friend class ZResourceLock;
}; // ZPrivateResource


class ZExport ZSharedResource : public ZResource
{
typedef ZResource
  Inherited;
public:
/*------------------------------- Constructors -------------------------------*/
  ZSharedResource   ( const char* keyName );
virtual
 ~ZSharedResource   ( );

virtual ZResource& lock           ( long timeOut = -1 );
virtual ZResource& unlock         ( );
/*---------------------------- Resource Information --------------------------*/

std::string
  keyName           ( ) const { return keyNameCl; }

protected:
#if !defined(__MVS__)
//
// The following method does not exist on MVS ... we don't use a handle
// there, just the ENQ name in keyNameCl
//
SHARED_SEMAPHORE_HANDLE_TYPE
 &handle();
#endif

private:
/*------------------------------ Hidden Members ------------------------------*/
  ZSharedResource    ( const ZSharedResource& sharedResource );
ZSharedResource
 &operator=          ( const ZSharedResource& sharedResource );

/*--------------------------------- Private ----------------------------------*/
#if !defined(__MVS__)
//
// This member doesn't exist on MVS - we don't use handles there
//
SHARED_SEMAPHORE_HANDLE_TYPE
  resourceHandleCl;
#endif

#if defined(_MSC_VER)
#pragma warning(disable : 4251)  // class needs to have dll-interface to be used by clients
#endif
std::string
  keyNameCl;
#if defined(_MSC_VER)
#pragma warning(default : 4251)  // class needs to have dll-interface to be used by clients
#endif

friend class ZResourceLock;
}; // ZSharedResource


class ZExport ZResourceLock 
{
public:

/*------------------------------- Constructors -------------------------------*/
  ZResourceLock   ( ZResource& resource,
                    long       timeOut = -1 );
virtual
  ~ZResourceLock  ( );

protected:
/*----------------------------- Resource Locking -----------------------------*/
virtual ZResourceLock &setLock        ( long timeOut = -1 );
virtual ZResourceLock &clearLock      ( );

private:
/*------------------------------ Hidden Members ------------------------------*/
  ZResourceLock ( const ZResourceLock& resourceLock );
ZResourceLock
 &operator=     ( const ZResourceLock& resourceLock );

/*--------------------------------- Private ----------------------------------*/
ZResource
 *presCl;

friend class ZResource;
}; // ZResourceLock

#if (0)
// Modifications:
//
// +MH+ $Log$
//
// -MH-
#endif

