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
//  in zmstrlck.h.
//    ZMasterLock
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

#include "zdefs.h"
#include "zmstrlck.h"
#if TARGET_OS_MAC
#include <chrono>
#include <thread>
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN	/* Exclude rarely-used stuff from Windows headers */
#include <windows.h>
#endif

#if defined(ZCLASS_UNIX)
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/time.h>
#endif

#if defined (__MVS__)
#include <css.h>
static  const char QNAME[8] = {'Z', 'M', 'S', 'T', 'R', 'L', 'C', 'K'};
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
** Define the critical section/mutex/whatever
*/
#if defined(_WIN32)
/*
** Variable to control initialisation
*/
static volatile long _InitFlag = 0L;

static CRITICAL_SECTION _CritSec = {0};
/*
** Cleanup function called during atexit() processing
*/
static void _CleanUp(void)
{
        long InitFlagValue = InterlockedExchange( (long *)&_InitFlag, 3L );
        if ( InitFlagValue == 2L )
            /* Should be okay to delete critical section */
            DeleteCriticalSection( &_CritSec );
}

#elif TARGET_OS_MAC
/*
** Variable to control initialisation
*/
static std::atomic<long> _InitFlag(0L);

static MPCriticalRegionID _CritSec = { 0 };
/*
** Cleanup function called during atexit() processing
*/
static void _CleanUp(void)
{
	long InitFlagValue = _InitFlag.exchange(3L);
	if (InitFlagValue == 2L)
		/* Should be okay to delete critical section */
		MPDeleteCriticalRegion(_CritSec);
}

#elif defined(ZCLASS_UNIX)
#if defined(__hpux) && (HPUX_VERS<1100 || defined(_PTHREADS_DRAFT4))
static pthread_once_t onceBlock = pthread_once_init;
#else
static pthread_once_t onceBlock = PTHREAD_ONCE_INIT;
#endif
static pthread_mutex_t _CritSec;

/*
** Cleanup function called during atexit() processing
*/
static void _CleanUp(void)
{
  pthread_mutex_destroy( &_CritSec );
}

void initialize_mutex ()
{
  /*
  ** Initialise the mutex
  */
#if defined(__hpux) && (HPUX_VERS<1100 || defined(_PTHREADS_DRAFT4))
  pthread_mutex_init(&_CritSec, pthread_mutexattr_default);
#else
  pthread_mutex_init(&_CritSec, NULL);
#endif
  /*
  ** Set up to clean up the mutex as we terminate
  **
  ** We don't do this anymore because it causes problems on AIX
  ** with a SIGILL when the library is unloaded before the process
  ** terminates.
  */
  // atexit(_CleanUp);
}


#elif defined(__MVS__)
/* Mutex Context does not exist */

#else
#error "Not yet implemented"
#endif

ZMasterLock::ZMasterLock()
{
#if defined(ZCLASS_UNIX)
  pthread_once (&onceBlock, initialize_mutex);
#endif
  ZMasterLock::getLock();
}

ZMasterLock::~ZMasterLock()
{
  ZMasterLock::releaseLock();
}


void ZMasterLock::getLock()
{
#if defined(_WIN32)
        static const DWORD ONE_MILLISECOND = 1; // Milliseconds
        /* Most common case - just enter the critical section */

        if ( _InitFlag == 2L ) {
            EnterCriticalSection( &_CritSec );
            return;
        }

        /*
        ** If the initialisation flag is zero, then the
        ** critical section needs to be initialised, and
        ** the cleanup code needs to be registered.
        */
        if ( _InitFlag == 0L ) {

            long InitFlagVal;

            if ( (InitFlagVal = InterlockedExchange( (long *)&_InitFlag, 1L )) == 0L ) {
                InitializeCriticalSection( &_CritSec );
                atexit( _CleanUp );
                _InitFlag = 2L;
            }
            else if ( InitFlagVal == 2L )
                _InitFlag = 2L;
        }

        /*
        ** If necessary, wait while another thread finishes initialising
        */
        while ( _InitFlag == 1L )
            Sleep( ONE_MILLISECOND );

        /*
        ** Initialisation is complete, so enter the critical section
        */
        if ( _InitFlag == 2L )
            EnterCriticalSection( &_CritSec );

#elif TARGET_OS_MAC
	/* Most common case - just enter the critical section */

	if (_InitFlag == 2L) {
		MPEnterCriticalRegion(_CritSec, kDurationForever);
		return;
	}

	/*
	** If the initialisation flag is zero, then the
	** critical section needs to be initialised, and
	** the cleanup code needs to be registered.
	*/
	if (_InitFlag == 0L) {

		long InitFlagVal;

		if ((InitFlagVal = _InitFlag.exchange(1L)) == 0L)
		{
			MPCreateCriticalRegion(&_CritSec);
			atexit(_CleanUp);
			_InitFlag = 2L;
		}
		else if (InitFlagVal == 2L)
			_InitFlag = 2L;
	}

	/*
	** If necessary, wait while another thread finishes initialising
	*/
	while (_InitFlag == 1L)
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	/*
	** Initialisation is complete, so enter the critical section
	*/
	if (_InitFlag == 2L)
		MPEnterCriticalRegion(_CritSec, kDurationForever);

#elif defined(ZCLASS_UNIX)
        int rc = 0;

        /* Most common case - just lock the mutex */

        rc = pthread_mutex_lock( &_CritSec );
        return;

#elif defined(__MVS__)
        int RC = 0;
        int ReqType=CSS_SER_RQTYP_EXCL;
        int RLen=0;
        int Scope=CSS_SER_SCOPE_STEP;
        int RET=CSS_SER_RQTYP_HAVE;
        char semname[64] = {'\0'};
        unsigned long pid = 0;

        CSSGASN(&pid, &RC); /* ASID */

        sprintf(semname,"%ld",pid);
        RLen=strlen(semname);

        BCSSENQ( QNAME, semname,  &ReqType, &RLen, &Scope, &RET, &RC);

        if ( RC != 0 )
        {
        /* what to do here */
        }      

#else
#error "Not yet implemented"
#endif
}

void ZMasterLock::releaseLock()
{
#if defined(_WIN32)
        if ( _InitFlag == 2L )
            LeaveCriticalSection( &_CritSec );

#elif TARGET_OS_MAC
	if (_InitFlag == 2L)
		MPExitCriticalRegion(&_CritSec);

#elif defined(ZCLASS_UNIX)
        int rc = pthread_mutex_unlock( &_CritSec );
        if (0 != rc)
        {
          /* what to do here */
        }
#elif defined(__MVS__)
        int RC = 0;
        int RLen=0;
        int Scope=CSS_SER_SCOPE_STEP;
        int RET=CSS_SER_RQTYP_HAVE;
        char semname[64] = {'\0'};
        unsigned long pid = 0;

        CSSGASN(&pid, &RC); /* ASID */

        sprintf(semname,"%ld",pid);
        RLen=strlen(semname);

                                                                                
        BCSSDEQ(QNAME, semname, &RLen, &Scope, &RET, &RC);                      
    
        if ( RC != 0 )          
        {
        /* what to do here */
        }                                                      

#else
#error "Not yet implemented"
#endif
}
