/**
 * FILE NAME: zinttimr.h
 * 
 * DESCRIPTION:
 *   Declaration of the classes:
 *     ZIntervalTimer
 *
 * Used to implement waits on Unix
 * 
 */

#ifndef ZINTTIMR_INCLUDED
#define ZINTTIMR_INCLUDED

#include <signal.h>
#include <sys/time.h>

/**
 *  The ZIntervalTimer class is a private class for managing an interval
 *  timer.  It is used to setup the signal handler and timer, and remove 
 *  these items when done.
 */
class ZIntervalTimer {
public:
    ZIntervalTimer( unsigned long lTimeOutMS );
    ~ZIntervalTimer();
    
private:
    sigset_t old_sigset_t;
    struct itimerval old_interval;
};

#endif

