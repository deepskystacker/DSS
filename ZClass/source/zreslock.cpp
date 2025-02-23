//***************************************************************************
// Project : ZClass Library
// $Workfile$
// $Revision$
// $Date$
//
// Original Author: David C. Partridge
//
// +USE+ Description: 
// Module Description.
//
//  This file contains the implementation of classes/functions declared
//  in zreslock.h.
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
// Turn this off and leave it off
#pragma warning(disable : 4702) // Unreachable code
#pragma warning(disable : 4710)  // function not expanded
#endif

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(none)
#endif

extern "C"
{
# include <stdio.h>
# include <string.h>
# include <limits.h>

#if defined(_WIN32)
# include <windows.h>
#elif defined(__MVS__)
# include <css.h>
#else // Assume Unix
# include <unistd.h>
# include <pthread.h>
# if defined(__sparc) || defined(__linux__) || defined(__hpux) || defined(__APPLE__)
#   include <sys/stat.h>
# endif
# include <fcntl.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>
# include <errno.h>
# include <signal.h>
# include "zinttimr.h"
#endif
}

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(restore)
#endif

#include "zexcept.h"
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

#if defined(ZCLASS_UNIX)
static SHARED_SEMAPHORE_HANDLE_TYPE createSem(key_t semKey);
static void                         deleteSem(SHARED_SEMAPHORE_HANDLE_TYPE semHandle);

#define ZC_SEM_LOCK        0    // the real semaphore
#define ZC_REF_LOCK        1    // A lock for the "reference counting" sem
#define ZC_REF_COUNT       2    // the "reference counting" sem
#define ZC_NUMBER_SEMS     3
/*------------------------------------------------------------------------------
| semaphore state tracing                                                      |
|                                                                              |
| If tracing is set to ALL, some debugging code which traces semaphore         |
| state info is included.  Otherwise this code is omitted.                     |
------------------------------------------------------------------------------*/
#ifdef Z_TRACE_ALL

// Define semaphore state tracing functions and macros
#define TRACE_SEM_STATE(semhdl)  traceSemState(semhdl)

void traceSemState( int handle );

void traceSemState( int handle )
{
  char traceBuffer[32] = {0};
  sprintf (traceBuffer, "semval = %d",
    semctl( handle, ZC_SEM_LOCK, GETVAL, 0));
  ZTRACE_ALL( traceBuffer) ;
  sprintf (traceBuffer, "sempid = %d",
    semctl( handle, ZC_SEM_LOCK, GETPID, 0));
  ZTRACE_ALL( traceBuffer) ;
  sprintf (traceBuffer, "semcnt = %d",
    semctl( handle, ZC_SEM_LOCK, GETNCNT, 0));
  ZTRACE_ALL( traceBuffer) ;
  sprintf (traceBuffer, "semzcnt = %d",
    semctl( handle, ZC_SEM_LOCK, GETZCNT, 0));
  ZTRACE_ALL( traceBuffer) ;
  sprintf (traceBuffer, "ref semval = %d",
    semctl( handle, ZC_REF_COUNT, GETVAL, 0));
  ZTRACE_ALL( traceBuffer) ;
  return;
}

#else

// Do not define semaphore tracing code
#define TRACE_SEM_STATE(semhdl)

#endif  // Z_TRACE_ALL

int doSemOp( int semID, struct sembuf semOpBuf[], int nBuf,
             int okRc1, int okRc2, int okRc3 )
{
  int result;

  // EINTR means we were interrupted by a signal.  In this case, just try again.
  while ( 0 != (result = semop( semID, semOpBuf, nBuf ) ) && errno == EINTR );

  if ( 0 != result )
  {
    if ( errno != okRc1 && errno != okRc2 && errno != okRc3 )
    {
      ZTHROWCLIBERROR("semop",
                      ZBaseErrorInfo::accessError,
                      ZException::recoverable);
    }
  }
  return result;
}

#endif // defined(ZCLASS_UNIX)

#if defined(__MVS__)
static inline unsigned long ZGetCurrentThread()
{
  unsigned long result = 0;
  int retcode = 0;
  CSSGTCB(&result, &retcode);
  return result;
}
#elif defined(_WIN32)
static inline unsigned long ZGetCurrentThread()
{
  return GetCurrentThreadId();
}
#elif defined(__hpux) && HPUX_VERS<1100 
static inline unsigned long ZGetCurrentThread()
{
  pthread_t current_thread;
  
  current_thread = pthread_self();
  return (unsigned long)current_thread.field2;
}
#else // Assume Unix
static inline unsigned long ZGetCurrentThread()
{
  return (unsigned long)pthread_self();
}
#endif

#if !defined(__MVS__)
/***************************************************************/
/* FUNCTION:  ZPrivateResource :: handle                       */
/*                                                             */
/* PURPOSE:   returns handle to the resource                   */
/***************************************************************/
PRIVATE_SEMAPHORE_HANDLE_TYPE& ZPrivateResource :: handle()
{
  return resourceHandleCl;
}

/***************************************************************/
/* FUNCTION:  ZSharedResource :: handle                        */
/*                                                             */
/* PURPOSE:   returns handle to the resource                   */
/***************************************************************/
SHARED_SEMAPHORE_HANDLE_TYPE& ZSharedResource :: handle()
{
  return resourceHandleCl;
}
#endif

/***************************************************************/
/* FUNCTION:  ZResource :: ZResource                           */
/*                                                             */
/* PURPOSE:   Constructor for base resource object             */
/***************************************************************/
ZResource ::  ZResource()
{
  lockingThread = 0;
  recursionCount = 0;
}


/***************************************************************/
/* FUNCTION:  ZResource :: ~ZResource                          */
/*                                                             */
/* PURPOSE:   Destructor for base resource object              */
/***************************************************************/
ZResource :: ~ZResource()
{
}

/***************************************************************/
/* FUNCTION:  ZPrivateResource :: ZPrivateResource             */
/*                                                             */
/* PURPOSE:   Constructor the key for a private resource.      */
/*                                                             */
/* NOTE:  A possible future enhancement is to change the       */
/*        NT implementation of this constructor to use a       */
/*        critical section object which is optimized, instead  */
/*        of a mutex object.                                   */
/***************************************************************/
ZPrivateResource :: ZPrivateResource()
#if defined(__MVS__)
: keyNameCl("")
#else                          
: resourceHandleCl(0)
#endif
{
#if defined(_WIN32)
  resourceHandleCl = CreateMutex( 0, false, 0 );
  if (resourceHandleCl == 0)
  {
    ZTHROWGUIERROR2( "CreateMutex",
                     ZBaseErrorInfo::accessError,
                     ZException::recoverable );
  }
#elif defined(__MVS__)
  // 
  // MVS is rather different - we don't use a handle, we use
  // a resource name which we build dynamically from the TOD
  // clock value (which we unpack as a hex string).
  //
  unsigned long long todValue = 0;
  int rc = 0;
  CSSGTOD(&todValue, &rc);
  ZASSERTSTATE(0ll != todValue);
  char todString[1 + 16] = {'\0'};
  sprintf(todString, "%8.8Xl%8.8Xl", todValue);
  keyNameCl = todString;
#else // Assume Unix
  pthread_mutex_t * mutex_p = new pthread_mutex_t;
  unsigned long ulRc = 0;

#if defined(__hpux) && (HPUX_VERS<1100 || defined(_PTHREADS_DRAFT4))
  ulRc = pthread_mutex_init(mutex_p, pthread_mutexattr_default);
#else
  ulRc = pthread_mutex_init(mutex_p, NULL);
#endif
  if (-1UL == ulRc)
  {
    ZTHROWCLIBERROR("pthread_mutex_init",
      ZBaseErrorInfo::accessError,
      ZException::recoverable);
  }
  resourceHandleCl = mutex_p;
#endif
}

/***************************************************************/
/* FUNCTION:  ZPrivateResource :: ~ZPrivateResource            */
/*                                                             */
/* PURPOSE:   Destructor.                                      */
/***************************************************************/
ZPrivateResource :: ~ZPrivateResource()
{
#if !defined(__MVS__)
  if (resourceHandleCl != 0)
  {
# if defined(_WIN32)
    CloseHandle( resourceHandleCl );
# else // Assume unix
    pthread_mutex_destroy(resourceHandleCl);
    delete resourceHandleCl;
# endif // defined(_WIN32)
  }

  resourceHandleCl = 0;
#endif // !defined(__MVS__)
}

/***************************************************************/
/* FUNCTION: ZPrivateResource :: lock                          */
/*                                                             */
/* PURPOSE: provide ability to explicitly acquire a resouce    */
/*          lock intead of requiring x = new ZResourceLock(yy) */
/***************************************************************/
ZResource& ZPrivateResource :: lock(long lTimeOut)
{
#if !defined(__MVS__)
  ZASSERT(resourceHandleCl);
#endif

  if ( lockingThread == ZGetCurrentThread() )
  {
      ++recursionCount;
      return *this;
  }


#if defined(__MVS__)
#if defined(CICS)
  // Something like ...
  EXEC CICS ENQ(keyNameCl.c_str());
#else
  int reqType = CSS_SER_RQTYP_EXCL;
  int resLen = keyNameCl.size();
  int Scope = CSS_SER_SCOPE_STEP;
  int retcode = 0;
  int reason = 0;
  const char QName[8] = {'Z', 'C', 'L', 'A', 'S', 'S', ' ', ' '};

  //
  // Call our C callable service to MVS ENQ
  //
  BCSSENQ(
    &QName[0],          // QName = "ZCLASS  "
    keyNameCl.c_str(),  // Resource name
    &reqType,           // Request type (Exclusive ENQ)
    &resLen,            // Length of resource name ??
    &Scope,             // Scope of lock (STEP level ENQ)
    &retcode,           // Return code
    &reason);           // Reason code

  if (0 != retcode)
  {
    ZTHROWSYSTEMERROR( retcode,
                       "BCSSENQ",
                       ZBaseErrorInfo::accessError,
                       ZException::recoverable );
  };
#endif // defined(CICS)
  lTimeOut;
    
#elif defined(_WIN32)
  unsigned long ulRC = 
    WaitForSingleObject( resourceHandleCl, lTimeOut );
  if (ulRC == WAIT_FAILED)
  {
    ZTHROWGUIERROR2( "WaitForSingleObject",
                     ZBaseErrorInfo::accessError,
                     ZException::recoverable );
  }
  else if (ulRC == WAIT_TIMEOUT)
  {
    //
    // We timed out waiting for the sema4 - some sort of exception needed.
    // Use an Out of System Resource to indicate the timer ran out.
    // (Yes that is a bit hokey, but it serves well).
    //
    ZOutOfSystemResource exc(
      "Timeout acquiring the semaphore for a lock() request.",
      WAIT_TIMEOUT, 
      ZException::recoverable);
    ZTHROW(exc);
  }
  else if (ulRC != WAIT_OBJECT_0)
  {
    ZTHROWSYSTEMERROR( ulRC,
                       "WaitForSingleObject",
                       ZBaseErrorInfo::accessError,
                       ZException::recoverable );
  }
#else // Assume Unix
  unsigned long ulRC = pthread_mutex_lock(resourceHandleCl);

  if (ulRC != 0)
  {

    ZTHROWCLIBERROR( "pthread_mutex_lock",
                     ZBaseErrorInfo::accessError,
                     ZException::recoverable );
  }
  lTimeOut = lTimeOut;
#endif

  lockingThread = ZGetCurrentThread();
  recursionCount = 1;
  return *this;
}


/***************************************************************/
/* FUNCTION: ZPrivateResource :: unlock                        */
/*                                                             */
/* PURPOSE: provide ability to explicitly release a resouce    */
/*          lock intead of requiring delete x                  */
/***************************************************************/
ZResource& ZPrivateResource :: unlock()
{
#if !defined(__MVS__)
  ZASSERT(resourceHandleCl);
#endif

  if ( lockingThread != ZGetCurrentThread() )
    return *this;

  if ( --recursionCount )
    return *this;

  lockingThread = 0;

#if defined(__MVS__)
#if defined(CICS)
  // Something like ...
  EXEC CICS DEQ(keyNameCl.c_str());
#else
  int reqType = CSS_SER_RQTYP_EXCL;
  int resLen = keyNameCl.size();
  int Scope = CSS_SER_SCOPE_STEP;
  int retcode = 0;
  int reason = 0;
  const char QName[8] = {'Z', 'C', 'L', 'A', 'S', 'S', ' ', ' '};

  //
  // Call our C callable service to MVS DEQ
  //
  BCSSDEQ(
    &QName[0],          // QName = "ZCLASS  "
    keyNameCl.c_str(),  // Resource name
    &resLen,            // Length of resource name ??
    &Scope,             // Scope of lock (STEP level ENQ)
    &retcode,           // Return code
    &reason);           // Reason code

  if (0 != retcode)
  {
    ZTHROWSYSTEMERROR( retcode,
                       "BCSSDEQ",
                       ZBaseErrorInfo::accessError,
                       ZException::recoverable );
  };
#endif // defined(CICS)

#elif defined(_WIN32)
  if (!ReleaseMutex( resourceHandleCl))
  {
      ZTHROWSYSTEMERROR(  GetLastError(),
                          "ReleaseMutex",
                          ZBaseErrorInfo::accessError,
                          ZException::recoverable );
  }
#else // Assume Unix
  unsigned long ulRC = 
    pthread_mutex_unlock(resourceHandleCl);

  if (ulRC != 0)
  {
    ZTHROWCLIBERROR( "pthread_mutex_unlock",
                     ZBaseErrorInfo::accessError,
                     ZException::recoverable );
  }
#endif

  return *this;
}

/***************************************************************/
/* FUNCTION:  ZSharedResource :: ZSharedResource               */
/*                                                             */
/* PURPOSE:   Construct the key for a named semaphore.         */
/***************************************************************/
#if !defined(__MVS__)
ZSharedResource :: ZSharedResource(const char* pszKeyName)
: resourceHandleCl(0)
, keyNameCl("")
#else
ZSharedResource :: ZSharedResource(const char* pszKeyName)
: keyNameCl("")
#endif
{
  ZASSERTPARM(0 != strlen(pszKeyName));
#if defined(__MVS__)
  keyNameCl += pszKeyName;
#elif defined(_WIN32)
  unsigned long ulRC = 0;

  // Backslash is NOT allowed in semaphore path names
  keyNameCl += pszKeyName;

  // If we are running on NT, need to ensure we allow sharing to
  // services (i.e. cannot use default security attributes)

  SECURITY_DESCRIPTOR sd = {0};
  SECURITY_ATTRIBUTES sa = {0};

  InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
  SetSecurityDescriptorDacl(&sd, TRUE, (PACL) NULL, FALSE);
  sa.nLength = sizeof(sa);
  sa.lpSecurityDescriptor = &sd;
  sa.bInheritHandle = TRUE;

  resourceHandleCl = CreateMutexA( &sa, false, keyNameCl.c_str() );

  if (resourceHandleCl == 0)
  {
    ulRC = GetLastError();

    if (ulRC == ERROR_ALREADY_EXISTS)
    {
      resourceHandleCl = OpenMutexA( MUTEX_ALL_ACCESS,
                                    true, keyNameCl.c_str());
      if (resourceHandleCl == 0)
      {
        ZTHROWGUIERROR2( "OpenMutex",
                         ZBaseErrorInfo::accessError,
                         ZException::recoverable );
      }
    }
    else if (ulRC != 0)
    {
      ZTHROWSYSTEMERROR( ulRC,
                         "CreateMutex",
                         ZBaseErrorInfo::accessError,
                         ZException::recoverable );
    }
  }
#else // Assume Unix
  key_t    semKey;

  // The key name is first checked to see if it is a preexisting
  // file.  If this is the case it is used.  If not a file named
  // /tmp/<pszKeyName> is created and used for the key.

  semKey = ftok(pszKeyName, 1);

  if (semKey != (key_t)-1 )
  {
    // key is existing file ... use it
    keyNameCl += pszKeyName;
  }
  else
  {
    mode_t default_mode = (S_IRUSR | S_IWUSR | S_IXUSR |
                           S_IRGRP | S_IXGRP |
                           S_IROTH | S_IXOTH);

    // key not existing file ... create a file in /tmp.  We use
    // open in such a way that if we have already created a /tmp
    // file we reuse it.  Need it to work this way so multiple
    // processes can use the semaphore !!
    keyNameCl += "/tmp";
    if (pszKeyName[0] != '/')
        keyNameCl += "/";
    keyNameCl += pszKeyName;

    int fd = open(keyNameCl.c_str(), O_RDONLY | O_CREAT, default_mode);
    if ( fd < 0 )
    {
      // open failed ... throw Clibrary exception 
      ZTHROWCLIBERROR("open",
                      ZBaseErrorInfo::accessError,
                      ZException::recoverable);
    }
    else
    {
      fchmod(fd, default_mode);
      close(fd);
    }

    // Finally, get the semaphore ID
    semKey = ftok(keyNameCl.c_str(), 1);
  }

  // call function to create and initialize sem if needed
  resourceHandleCl = createSem( semKey );
#endif
}

/***************************************************************/
/* FUNCTION:  ZSharedResource :: ~ZSharedResource              */
/*                                                             */
/* PURPOSE:   Destructor.                                      */
/***************************************************************/
ZSharedResource :: ~ZSharedResource()
{
#if !defined(__MVS__)
  if (resourceHandleCl != 0)
  {
#if defined(_WIN32)
    CloseHandle( resourceHandleCl );
#else
    deleteSem(resourceHandleCl);
#endif
  }
  resourceHandleCl = 0;
#endif // !defined(__MVS__)
}

/***************************************************************/
/* FUNCTION: ZSharedResource :: lock                           */
/*                                                             */
/* PURPOSE: provide ability to explicitly acquire a resouce    */
/*          lock intead of requiring x = new IResourceLock(yy) */
/***************************************************************/
ZResource& ZSharedResource :: lock(long lTimeOut)
{
#if !defined(__MVS__)
  ZASSERT(resourceHandleCl);
#endif

  if ( lockingThread == ZGetCurrentThread() )
  {
    ++recursionCount;
    return *this;
  }

#if defined(__MVS__)
#if defined(CICS)
  // Something like ...
  EXEC CICS ENQ(keyNameCl.c_str());
#else
  int reqType = CSS_SER_RQTYP_EXCL;
  int resLen = keyNameCl.size();
  int Scope = CSS_SER_SCOPE_SYSTEM;
  int retcode = 0;
  int reason = 0;
  const char QName[8] = {'Z', 'C', 'L', 'A', 'S', 'S', ' ', ' '};

  //
  // Call our C callable service to MVS ENQ
  //
  BCSSENQ(
    &QName[0],          // QName = "ZCLASS  "
    keyNameCl.c_str(),  // Resource name
    &reqType,           // Request type (Exclusive ENQ)
    &resLen,            // Length of resource name ??
    &Scope,             // Scope of lock (STEP level ENQ)
    &retcode,           // Return code
    &reason);           // Reason code

  if (0 != retcode)
  {
    ZTHROWSYSTEMERROR( retcode,
                       "BCSSENQ",
                       ZBaseErrorInfo::accessError,
                       ZException::recoverable );
  };
#endif // defined(CICS)
  lTimeOut;
    
#elif defined(_WIN32)
  unsigned long ulRC =
    WaitForSingleObject( resourceHandleCl, lTimeOut );
  if (ulRC == WAIT_FAILED)
  {
    ZTHROWGUIERROR2( "WaitForSingleObject",
                     ZBaseErrorInfo::accessError,
                     ZException::recoverable );
  }
  else if (ulRC == WAIT_TIMEOUT)
  {
    //
    // We timed out waiting for the sema4 - some sort of exception needed.
    // Use an Out of System Resource to indicate the timer ran out.
    // (Yes that is a bit hokey, but it serves well).
    //
    ZOutOfSystemResource exc(
      "Timeout acquiring the semaphore for a lock() request.",
                             WAIT_TIMEOUT, 
                             ZException::recoverable);
    ZTHROW(exc);
  }
  else if (ulRC != WAIT_OBJECT_0)
  {
    ZTHROWSYSTEMERROR( ulRC,
                       "WaitForSingleObject",
                       ZBaseErrorInfo::accessError,
                       ZException::recoverable );
  }
#else

#if !defined(__hpux) ||\
  (defined(__hpux) && HPUX_VERS>= 1100 && !defined(_PTHREADS_DRAFT4))
  ZIntervalTimer *pTimer = NULL;
#endif

  struct sembuf  semOpBuf[1];
  semOpBuf[0].sem_num = ZC_SEM_LOCK;
  semOpBuf[0].sem_op  = -1;         // Wait for the lock.
  semOpBuf[0].sem_flg = SEM_UNDO;   // Give it back when we terminate.

  // Handle timeout specification. The timeout value is
  // in milliseconds.  -1 means wait forever...0 means
  // return immediately.
  if (0 == lTimeOut)
  {
    semOpBuf[0].sem_flg |= IPC_NOWAIT;
  }
#if !defined(__hpux) ||\
  (defined(__hpux) && HPUX_VERS>= 1100 && !defined(_PTHREADS_DRAFT4))
  else if (lTimeOut != -1)
  {
    pTimer = new ZIntervalTimer(lTimeOut);
  }
#endif

  // Wait on the semaphore.
  unsigned long ulRC = semop( resourceHandleCl, semOpBuf, 1 );

#if !defined(__hpux) ||\
  (defined(__hpux) && HPUX_VERS>= 1100 && !defined(_PTHREADS_DRAFT4))
  //
  // Delete the interval timer regardless of whether it
  // fired.
  //
  if (pTimer)
    delete pTimer;
#endif

  if ( ulRC )
  {
    int rc = errno;

    if ( (rc == EAGAIN) || (rc == EINTR) )
    {
      // could not access the semaphore ... locked or timeout
      ZOutOfSystemResource exc(
        "Timeout acquiring the semaphore for a lock() request.",
                               EINTR, 
                               ZException::recoverable);
      ZTHROW(exc);
    }
    else
    { // some other error ... throw system error
      ZTHROWCLIBERROR( "semop",
                       ZBaseErrorInfo::accessError,
                       ZException::recoverable);
    }
  }

#endif

  lockingThread = ZGetCurrentThread();
  recursionCount = 1;

  return (*this);
}


/***************************************************************/
/* FUNCTION: ZSharedResource :: unlock                         */
/*                                                             */
/* PURPOSE: provide ability to explicitly release a resouce    */
/*          lock intead of requiring delete x                  */
/***************************************************************/
ZResource& ZSharedResource :: unlock()
{
#if !defined(__MVS__)
  ZASSERT(resourceHandleCl);
#endif

  if ( lockingThread != ZGetCurrentThread() )
    return *this;

  if ( --recursionCount )
    return *this;

  lockingThread = 0;

#if defined(__MVS__)
#if defined(CICS)
  // Something like ...
  EXEC CICS DEQ(keyNameCl.c_str());
#else
  int reqType = CSS_SER_RQTYP_EXCL;
  int resLen = keyNameCl.size();
  int Scope = CSS_SER_SCOPE_STEP;
  int retcode = 0;
  int reason = 0;
  const char QName[8] = {'Z', 'C', 'L', 'A', 'S', 'S', ' ', ' '};

  //
  // Call our C callable service to MVS DEQ
  //
  BCSSDEQ(
    &QName[0],          // QName = "ZCLASS  "
    keyNameCl.c_str(),  // Resource name
    &resLen,            // Length of resource name ??
    &Scope,             // Scope of lock (STEP level ENQ)
    &retcode,           // Return code
    &reason);           // Reason code

  if (0 != retcode)
  {
    ZTHROWSYSTEMERROR( retcode,
                       "BCSSDEQ",
                       ZBaseErrorInfo::accessError,
                       ZException::recoverable );
  };
#endif // defined(CICS)

#elif defined(_WIN32)
  if (!ReleaseMutex( resourceHandleCl))
  {
    ZTHROWSYSTEMERROR(  GetLastError(),
                        "ReleaseMutex",
                        ZBaseErrorInfo::accessError,
                        ZException::recoverable );
  }
#else
  struct sembuf  semOpBuf[1];
  semOpBuf[0].sem_num = ZC_SEM_LOCK;
  semOpBuf[0].sem_op  = 1;        // Free the lock.
  semOpBuf[0].sem_flg = SEM_UNDO; // Matches when we got the lock.
  doSemOp( resourceHandleCl, semOpBuf, 1, 0, 0, 0 );
#endif 

  return( *this );
}

#if defined(ZCLASS_UNIX)
/*------------------------------------------------------------------------------
| createSem                                                                    |
|                                                                              |
| local function to create a semaphore.  This function combines the functions  |
| of the OS/2 DosCreateMutexSem and DosOpenMutexSem functions.                 |
------------------------------------------------------------------------------*/
SHARED_SEMAPHORE_HANDLE_TYPE createSem( key_t semKey )
{
  int semID, rc;
  struct sembuf  semOpBuf[1];
  bool gotIt=false;
  while ( !gotIt )
  {
    // First, attempt to create a new semaphore in the system.  This call
    // will fail if there is already a semaphore with this key, such as
    // where another process has created it.
    //
    // Add two flags to support SUN platform
    #ifdef __sparc
    semID  = semget(semKey,           // key
                      ZC_NUMBER_SEMS,   // semaphore count
                      IPC_CREAT | IPC_EXCL |    // flags
                      SEM_R | SEM_A|
                      S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    #else
    semID  = semget(semKey,           // key
                       ZC_NUMBER_SEMS,   // semaphore count
                       IPC_CREAT | IPC_EXCL |    // flags
                       S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) ;
    #endif
    gotIt = (semID != -1 );
    if ( gotIt )
    {
      TRACE_SEM_STATE( semID );
      //
      // We created the semaphore block.
      //
      // First, set and increment the reference count, so no one will remove the
      // semaphore block while we are using it.  Use SEM_UNDO so it will
      // be decremented when we leave.
      //
      if ( 0 != semctl( semID, ZC_REF_COUNT, SETVAL, 0 ) )
      {
        ZTHROWCLIBERROR( "semctl",
                        ZBaseErrorInfo::accessError,
                        ZException::recoverable);
      }
      semOpBuf[0].sem_num = ZC_REF_COUNT;
      semOpBuf[0].sem_op  = 1;       // increment count.
      semOpBuf[0].sem_flg = SEM_UNDO;// decrement when we terminate.
      doSemOp( semID, semOpBuf, 1, 0, 0, 0 );
      TRACE_SEM_STATE( semID );
      //
      // Then initialize ZC_SEM_LOCK and ZC_REF_LOCK to 1, making them available.
      //
      if ( 0 != semctl( semID, ZC_SEM_LOCK, SETVAL, 1 ) )
      {
        ZTHROWCLIBERROR( "semctl",
                        ZBaseErrorInfo::accessError,
                        ZException::recoverable);
      }
      TRACE_SEM_STATE( semID );
      if ( 0 != semctl( semID, ZC_REF_LOCK, SETVAL, 1 ) )
      {
        ZTHROWCLIBERROR( "semctl",
                        ZBaseErrorInfo::accessError,
                        ZException::recoverable);
      }
    }
    else
    {
      // Could not create the semaphore.  This is supposed to happen
      // if it already exists.  Now try to get an already existing
      // semaphore.

      // create access to the block...don't need to initialize
      #ifdef __sparc
      semID  = semget(semKey,           // key
                      ZC_NUMBER_SEMS,   // semaphore count
                      SEM_R | SEM_A|
                      S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
      #else
      semID = semget(semKey,           // key
                   ZC_NUMBER_SEMS,   // semaphore count
                   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) ;
      #endif
      //
      // If we failed to get it due to ENOENT, the semaphore has been
      // removed.  In this case loop back and try to create it again.
      if ( semID ==  -1 )
      {
        rc = errno;
        if ( rc != ENOENT )
        {
          // failed.  Can't access for some other reason
          ZTHROWCLIBERROR( "semget",
                          ZBaseErrorInfo::outOfSystemResource,
                          ZException::recoverable );
        }
        // Loop back to try again.
      }
      else // The semaphore block exists.
      {
        // We have the semaphore block, but there is no guarantee how long it
        // will exist until we get the reference lock.
        //
        semOpBuf[0].sem_num = ZC_REF_LOCK;
        semOpBuf[0].sem_op  = -1;      // Wait for the lock.
        semOpBuf[0].sem_flg = SEM_UNDO;// Give it back when we terminate.
        gotIt = !(doSemOp( semID, semOpBuf, 1, EINVAL, EIDRM, 0 ) );
        if ( gotIt )
        {
          TRACE_SEM_STATE( semID );
          // We have the reference lock.
          // Increment the reference count to prevent the semaphore block
          // being removed until we call deleteSem.
          //
          semOpBuf[0].sem_num = ZC_REF_COUNT;
          semOpBuf[0].sem_op  = 1;       // increment count.
          semOpBuf[0].sem_flg = SEM_UNDO;// decrement when we terminate.
          doSemOp( semID, semOpBuf, 1, 0, 0, 0 );
          TRACE_SEM_STATE( semID );
          //
          // Free the reference lock.
          //
          semOpBuf[0].sem_num = ZC_REF_LOCK;
          semOpBuf[0].sem_op  = 1;       // Free the reference lock.
          semOpBuf[0].sem_flg = SEM_UNDO;// Matches when we got the lock.
          doSemOp( semID, semOpBuf, 1, 0, 0, 0 );
        }
      }
    }
  } // while
  TRACE_SEM_STATE( semID );

  return semID;
}

/*------------------------------------------------------------------------------
| deleteSem                                                                    |
|                                                                              |
| Local function to delete a semaphore.  This function approximates the        |
| OS/2 DosCloseMutexSem function in that it releases the access of the caller  |
| to the semaphore, and if this is the last usage frees system resources.      |
------------------------------------------------------------------------------*/
static void deleteSem( SHARED_SEMAPHORE_HANDLE_TYPE semID  )
{
  //
  // We want to decrement the reference count semaphore, and remove the
  // semaphore block if we decrement it to zero.
  // We might not have permission to remove it.  In this case it is left
  // with a reference count of zero, available for re-use.
  //
  //
  // Use the reference semaphore lock to make sure the reference count
  // will not change while we depend on its value.
  //
  struct sembuf  semOpBuf[4];
  bool removed = false;

  semOpBuf[0].sem_num = ZC_REF_LOCK;
  semOpBuf[0].sem_op  = -1;      // Wait for the lock.
  semOpBuf[0].sem_flg = SEM_UNDO;// Give it back if you terminate.
  doSemOp( semID, semOpBuf, 1, 0, 0, 0 );
  //
  // We have acquired the reference lock.
  // Decrement and check the reference count.
  // SEM_ORDER specifies the operations are done in order, non-atomically.
  //
  semOpBuf[0].sem_num = ZC_REF_COUNT;
  semOpBuf[0].sem_op  = -1;
#if (0)
  semOpBuf[0].sem_flg = SEM_UNDO | SEM_ORDER; // Match the SEM_UNDO in semCreate.
#else
  semOpBuf[0].sem_flg = SEM_UNDO; // Match the SEM_UNDO in semCreate.
#endif

  semOpBuf[1].sem_num = ZC_REF_COUNT;
  semOpBuf[1].sem_op  = 0;
  semOpBuf[1].sem_flg = IPC_NOWAIT;             // Just check the value.
  if ( 0 == doSemOp( semID, semOpBuf, 2, EAGAIN, 0, 0 ) )
  {
    // The reference count is now zero.
    // Try to remove the semaphore block.
    //
    //@@ Check parameters
    removed = ( 0 == semctl( semID, 0, IPC_RMID, 0) );
  }
  if ( !removed )
  {
    // Free the reference lock.
    semOpBuf[0].sem_num = ZC_REF_LOCK;
    semOpBuf[0].sem_op  = 1;       // Free the lock.
    semOpBuf[0].sem_flg = SEM_UNDO;// Matches when we got the lock.
    doSemOp( semID, semOpBuf, 1, 0, 0, 0 );
  }
}
#endif // defined(ZCLASS_UNIX)

/***************************************************************/
/* FUNCTION:  ZResourceLock :: ZResourceLock                   */
/*                                                             */
/* PURPOSE:   Construct an instance of a resource lock.        */
/***************************************************************/
ZResourceLock :: ZResourceLock(ZResource& res, long lTimeOut)
{
  this->presCl = &res;
  setLock( lTimeOut );
}


/***************************************************************/
/* FUNCTION:  ZResourceLock :: ~ZResourceLock                  */
/*                                                             */
/* PURPOSE:   Free the lock on the resource.                   */
/***************************************************************/
ZResourceLock :: ~ZResourceLock()
{
  clearLock();
}

/***************************************************************/
/* FUNCTION: ZResourceLock :: setLock                          */
/*                                                             */
/* PURPOSE: sets (requests) the semaphore                      */
/***************************************************************/
ZResourceLock& ZResourceLock::setLock(long lTimeOut)
{
  presCl->lock(lTimeOut);
  return ( *this );
}

/***************************************************************/
/* FUNCTION: ZResourceLock :: clearLock                        */
/*                                                             */
/* PURPOSE: clears the associated semaphore                    */
/***************************************************************/
ZResourceLock& ZResourceLock::clearLock()
{
  presCl->unlock();
  return( *this );
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
