/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	RegisterWindow.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include "QHTMControlSection.h"

extern bool FASTCALL RegisterWindow( HINSTANCE hInst );

bool FASTCALL RegisterWindow( HINSTANCE hInst )
{
	//	Needs an instance handle!
	ASSERT( hInst );

	WNDCLASS wc = { sizeof( WNDCLASS ) };
	if( !GetClassInfo( hInst, QHTM_CLASSNAME, &wc ) )
	{
#ifndef UNDER_CE
		wc.style			= CS_BYTEALIGNCLIENT;
#else	//	UNDER_CE
		wc.style			= 0;
#endif	//	UNDER_CE
		wc.lpfnWndProc	= (WNDPROC)CQHTMControlSection::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= sizeof( CQHTMControlSection * );
		wc.hInstance		= hInst;
		wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)COLOR_WINDOW + 1;
		wc.lpszMenuName	= NULL;
		wc.lpszClassName	= QHTM_CLASSNAME;

		return RegisterClass( &wc ) != 0;
	}
	return true;
}

