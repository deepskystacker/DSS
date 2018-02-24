/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_Trace.cpp
Owner:	russf@gipsysoft.com
Purpose:	The bulk of this code has been taken from DebugHLP.
----------------------------------------------------------------------*/
#include "stdafx.h"

#ifdef QHTM_TRACE_ENABLED	

#include <DebugHlp/DebugHlp.h>
#include "QHTM_Trace.h"
#include <stdio.h> 



static HANDLE g_hHeap = GetProcessHeap();

static int Startup()
{
	AllocConsole();
	return 0;
}

static int g_nDummy = Startup();

inline LPVOID GetBuffer()
{
	return HeapAlloc( g_hHeap, HEAP_ZERO_MEMORY, 512 );
}


LPVOID ReallocBuffer( LPVOID pszBuffer, int nAllocated )
{
	LPVOID pVoid = HeapReAlloc( g_hHeap, HEAP_ZERO_MEMORY, pszBuffer, nAllocated );
	if( !pVoid )
	{
		HeapFree( g_hHeap, 0, pszBuffer );
		pszBuffer = NULL;
	}
	else
	{
		pszBuffer = pVoid;
	}
	return pszBuffer;
}



LPTSTR vmalprintf( LPCTSTR pcszFormat, va_list list )
{
  bool  bSuccess = FALSE;

	LPSTR pszBuffer = reinterpret_cast<LPTSTR>( GetBuffer() );

	int nAllocated = 512;
	do
  {
		if( _vsnprintf( pszBuffer, nAllocated - 1, pcszFormat, list ) < 0 )
	  {
			nAllocated *= 2;
			pszBuffer = reinterpret_cast<LPTSTR>( ReallocBuffer( pszBuffer, nAllocated ) );
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


void _cdecl QHTMTrace( LPCSTR pcszFormat, ... )
{
 	va_list marker;
	va_start( marker, pcszFormat );

	LPTSTR pszBuffer = vmalprintf( pcszFormat, marker );
	OutputDebugStringA( pszBuffer );
	DWORD dw;
	WriteFile( GetStdHandle( STD_OUTPUT_HANDLE ), pszBuffer, _tcslen( pszBuffer ) * sizeof( TCHAR ), &dw, NULL );
	VERIFY( HeapFree( GetProcessHeap(), 0, pszBuffer ) );

	va_end( marker );
}

#endif	//	QHTM_TRACE_ENABLED
