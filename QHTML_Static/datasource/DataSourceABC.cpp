/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DataSourceABC.cpp
Owner:	russf@gipsysoft.com
Purpose:	Data source abstract base class.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DataSourceABC.h"

bool CDataSourceABC::MSBReadLong( long &l)
{
  BYTE buffer[4];
  if( !ReadBytes( buffer, 4 ) )
    return false;

  l = (unsigned int) (buffer[0] << 24);
  l |= (unsigned int) (buffer[1] << 16);
  l |= (unsigned int) (buffer[2] << 8);
  l |= (unsigned int) (buffer[3]);

  return true;
}
