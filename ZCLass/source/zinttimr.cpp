/**
 * FILE NAME: zinttimr.cpp
 *
 * DESCRIPTION:
 *   Definition of the classes:
 *     ZIntervalTimer
 *
 * Used to implement waits on Unix
 *
 */
#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(none)
#endif

extern "C" {
    #include <unistd.h>
    #include <pthread.h>
    #include <signal.h>
    #include <sys/time.h>
}

#if defined(_AIX) && defined(__IBMCPP__)
#pragma info(restore)
#endif

#include "ztrace.h"
#include "zinttimr.h"
#include "zexcept.h"
#include "zmstrlck.h"

#if !defined(ZCLASS_UNIX)
#error This source file only used on Unix or Unix like systems
#endif

// ****************************************************************
// ****************************************************************
//
// This is effectively an empty source file if compiled on HP_UX 10
// or if comiled on HP_UX 11 or higher with _PTHREADS_DRAFT4 !!!!!!
//
// ****************************************************************
// ****************************************************************
#if !defined(__hpux) ||\
  (defined(__hpux) && HPUX_VERS>= 1100 && !defined(_PTHREADS_DRAFT4))

#if !defined(_AIX)
//
// Most Unix systems don't have a fetch_and_add atomic operation
// (AIX being the notable exception).   For all the other Unix
// systems we'll provide roughly equivalent function here.
// 
// It's not efficient, but it should serve.
//
typedef int * atomic_p;

static int fetch_and_add(atomic_p word_addr, int value)
{
  int result = 0;
  //
  // Set the return value from the original value
  // and increment the value.  All under the control
  // of the library master lock 
  //
  {
    ZMasterLock lock;
    result = *word_addr;
    (*word_addr) += value;
  }
  return result;
}
#else
//
// AIX does define the fetch_and_add atomic operations, so we will use it
//
#include <sys/atomic_op.h>
#endif // !defined(_AIX)

static int alarm_count = 0;
static struct sigaction global_action;
static struct itimerval global_interval;

/*------------------------------------------------------------------------------
| intervaltimerHandler                                                         |
|                                                                              |
| This function is a signal handler for the timeout on wait for a semaphore.   |
| It does nothing...the desired effect is that semop returns EINTR as a        |
| result of the signal arriving.                                               |
------------------------------------------------------------------------------*/
void intervaltimerHandler(int signalID)
{
  signalID = signalID;
  return;
}

/*------------------------------------------------------------------------------
| ZIntervalTimer constructor                                                   |
|                                                                              |
| Installs a dummy signal handler for SIGALRM and starts the timer.            |
------------------------------------------------------------------------------*/
ZIntervalTimer::ZIntervalTimer( unsigned long lTimeOutMS ) 
{
  int old_alarm_count = fetch_and_add((atomic_p)&alarm_count, 1);

  if (old_alarm_count == 0)
  {
    // For the first alarm, we remember the global signal handler
    // (global_action) and register our signal handler here.

    struct sigaction  action;    // new signal handler info

    // Set up a signal handler to accept the timeout signal
    // should a timeout happen
    action.sa_handler = &intervaltimerHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGALRM, &action, &global_action);
  }

  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set,SIGALRM);

  // Unblock the signal for the current thread
#if (_XOPEN_SOURCE >= 500)
  pthread_sigmask(SIG_UNBLOCK, &set, &old_sigset_t);
#else
  sigthreadmask(SIG_UNBLOCK, &set, &old_sigset_t);
#endif

  // Setup a timer and GO!
  struct itimerval interval;
  interval.it_interval.tv_sec = 0;
  interval.it_interval.tv_usec = 0;
  interval.it_value.tv_sec = (int)(lTimeOutMS/1000);
  interval.it_value.tv_usec = 
      (lTimeOutMS - (interval.it_value.tv_sec * 1000)) * 1000;

  if (old_alarm_count == 0)
  {
    // For the first alarm, we remember the global timer interval.
    // The reason for the old_interval settings is that the thread
    // that sets the first alarm may not be the first thread to be
    // waken up. We have to make sure that our chain of interval timers
    // works in all cases.
    setitimer(ITIMER_REAL, &interval, &global_interval);
    old_interval.it_interval.tv_sec = 0;
    old_interval.it_interval.tv_usec = 0;
    old_interval.it_value.tv_sec = interval.it_value.tv_sec;
    old_interval.it_value.tv_usec = interval.it_value.tv_usec;
  }
  else
  {
    setitimer(ITIMER_REAL, &interval, &old_interval);
  }
}

/*------------------------------------------------------------------------------
| ZIntervalTimer destructor                                                    |
|                                                                              |
| Cancels the timer and restores the SIGALRM signal state.                     |
------------------------------------------------------------------------------*/
ZIntervalTimer::~ZIntervalTimer( )
{
  int old_alarm_count = fetch_and_add((atomic_p)&alarm_count, -1);

#if (_XOPEN_SOURCE >= 500)
  // Restore the previous signal mask for this thread
  pthread_sigmask(SIG_SETMASK, &old_sigset_t, 0);
#else
  sigthreadmask(SIG_SETMASK, &old_sigset_t, 0);
#endif

  if (old_alarm_count == 1)
  {
    
    // This is the last alarm!!
    
    // remove our signal handler and put back whatever was there.
    // signal handler is a process-wide thing
    sigaction(SIGALRM, &global_action, 0);
    setitimer(ITIMER_REAL, &global_interval, 0);

    // Due to certain combinations of events, most likely a result of
    // client program error, it is possible that our alarm signal is
    // still pending.  The default behavior is to terminate the program
    // without a specific signal handler.  Therefore we attempt to
    // remove this signal if it is pending.  No exceptions are thrown
    // if something goes wrong in the process, however.
    sigset_t     sigsBlocked;
    sigemptyset( &sigsBlocked );
    sigpending( &sigsBlocked );
    if (sigismember(&sigsBlocked, SIGALRM))
    {

      struct sigaction  resetSignal;
      resetSignal.sa_handler = SIG_IGN;
      resetSignal.sa_flags = 0;
      sigemptyset( &resetSignal.sa_mask );
      sigaddset( &resetSignal.sa_mask, SIGALRM );
      sigaction( SIGALRM, &resetSignal, NULL );
    }
    
  }
  else
  {
    // Restore the old interval timer for the
    // next SIGALRM. This is definitely not the best solution because
    // the timeout value is not accurate. However, it avoids deadlock
    // by making sure that there is a chain of SIGALRM delivered to
    // the process.
    setitimer(ITIMER_REAL, &old_interval, 0);
  }
}
#endif
