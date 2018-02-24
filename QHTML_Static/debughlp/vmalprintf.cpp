/*----------------------------------------------------------------------
Copyright (c) 1998-present Russ Freeman. All Rights Reserved.
Web site: http://www.gipsysoft.com/

This software is provided 'as-is', without any express or implied warranty.

In no event will the author be held liable for any damages arising from the
use of this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject
to the following restrictions: 

1) This source code may not be distributed as part of a commercial library
   without written permission from the author
2) The origin of this software must not be misrepresented; you must not claim
   that you wrote the original software. If you use this software in a product,
	 an acknowledgment in the product documentation is requested but not required.
3) Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
4) Altered source is encouraged to be submitted back to the original author so
   it can be shared with the community. Please share your changes. Don't make me
	 use GPL!
5) This notice may not be removed or altered from any source distribution.
6) If you have any confusion then ask

File:	vmalprintf.cpp
Owner:	russf@gipsysoft.com
Purpose:	printf style functions that return a heap allocated memory block
					Uses Heap API for memory allocations for the output buffer.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
static int g_knDefaultBufferSize = 2048;


LPWSTR DBGH_vmalprintfW( LPCWSTR pcszFormat, va_list list )
{
  bool  bSuccess = FALSE;

	LPWSTR pszBuffer = reinterpret_cast<LPWSTR>( malloc( g_knDefaultBufferSize * 2 ) );

	int nAllocated = g_knDefaultBufferSize;
	do
  {
		if( _vsnwprintf( pszBuffer, nAllocated - 1, pcszFormat, list ) < 0 )
	  {
			nAllocated *= 2;
			pszBuffer = reinterpret_cast<LPWSTR>( realloc( pszBuffer, nAllocated * 2 ) );
			if( !pszBuffer )
			{
				return NULL;
			}
	  }
    else
		{
      bSuccess = TRUE;
		}
  } while( !bSuccess );

	return pszBuffer;
}


LPSTR DBGH_vmalprintfA( LPCSTR pcszFormat, va_list list )
{
  bool  bSuccess = FALSE;

	LPSTR pszBuffer = reinterpret_cast<LPSTR>( malloc( g_knDefaultBufferSize ) );

	int nAllocated = g_knDefaultBufferSize;
	do
  {
		if( _vsnprintf( pszBuffer, nAllocated - 1, pcszFormat, list ) < 0 )
	  {
			nAllocated *= 2;
			pszBuffer = reinterpret_cast<LPSTR>( realloc( pszBuffer, nAllocated ) );
			if( !pszBuffer )
			{
				return NULL;
			}
	  }
    else
		{
      bSuccess = TRUE;
		}
  } while( !bSuccess );

	return pszBuffer;
}
