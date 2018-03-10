/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GetDisplayWorkArea.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#pragma warning( disable: 4706 )
#define _NO_MULTIMON_SUPPORT

#ifndef _NO_MULTIMON_SUPPORT
	#pragma warning( disable: 4706 )
	#define COMPILE_MULTIMON_STUBS
#endif	//_NO_MULTIMON_SUPPORT

#if !defined (_WIN32_WCE)

extern void GetDisplayWorkArea( HWND hwnd, RECT &rc );
extern void GetDisplayWorkArea( POINT pt, RECT &rc );

void GetDisplayWorkArea( HWND hwnd, RECT &rc )
{
	HMONITOR hMon = MonitorFromWindow( hwnd, MONITOR_DEFAULTTONEAREST );
	if( hMon )
	{
		MONITORINFO m = { sizeof( m ) };
		if( GetMonitorInfo( hMon, &m ) )
		{
			rc = m.rcWork;
		}
	}
}


void GetDisplayWorkArea( POINT pt, RECT &rc )
{
	HMONITOR hMon = MonitorFromPoint( pt, MONITOR_DEFAULTTONEAREST );
	if( hMon )
	{
		MONITORINFO m = { sizeof( m ) };
		if( GetMonitorInfo( hMon, &m ) )
		{
			rc = m.rcWork;
		}
	}
}

#endif	//	!defined (_WIN32_WCE)

