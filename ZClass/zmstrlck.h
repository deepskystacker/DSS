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

class ZMasterLock
{
public:
  ZMasterLock();
  ~ZMasterLock();

private:
/*------------------------------ Hidden Members ------------------------------*/
  ZMasterLock ( const ZMasterLock& masterLock );
ZMasterLock
 &operator=   ( const ZMasterLock& masterLock );

static void getLock();
static void releaseLock();
};

#if (0)
// Modifications:
//
// +MH+ $Log$
//
// -MH-
#endif

