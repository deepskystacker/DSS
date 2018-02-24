/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_RenderHTML.cpp
Owner:	russf@gipsysoft.com
Purpose:	Give some HTML and a restriction of width render the HTML to the device.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <limits.h>
#include "qhtm.h"
#include "Defaults.h"
#include "HtmlSection.h"


extern "C" BOOL WINAPI QHTM_RenderHTML( HDC hdc, LPCTSTR pcsz, HINSTANCE hInst, UINT uFlags, UINT uWidth )
{
#ifdef QHTM_ALLOW_RENDER
	WinHelper::CRect rcDraw( 0, 0, uWidth, INT_MAX );
	return QHTM_RenderHTMLRect( hdc, pcsz, hInst, uFlags, &rcDraw );
#else	//	QHTM_ALLOW_RENDER
	UNREFERENCED_PARAMETER( hdc );
	UNREFERENCED_PARAMETER( pcsz );
	UNREFERENCED_PARAMETER( hInst );
	UNREFERENCED_PARAMETER( uFlags );
	UNREFERENCED_PARAMETER( uWidth );
	return FALSE;
#endif	//	QHTM_ALLOW_RENDER
}

extern "C" BOOL WINAPI QHTM_RenderHTMLRect( HDC hdc, LPCTSTR pcsz, HINSTANCE hInst, UINT uFlags, LPCRECT lprc )
{
#ifdef QHTM_ALLOW_RENDER
	if( pcsz && hdc )
	{
		CHTMLSection sectHTML( &g_defaults );

		if( uFlags & QHTM_RENDER_FLAG_TRANSPARENT )
		{
			sectHTML.Transparent( true );
		}

		uFlags = uFlags & 3;

		switch( uFlags )
		{
		case QHTM_SOURCE_TEXT:
			sectHTML.SetHTML( pcsz, static_cast<UINT>( _tcslen( pcsz ) ), NULL );
			break;

		case QHTM_SOURCE_RESOURCE:
			if( !sectHTML.SetHTML( hInst, pcsz ) )
			{
				return FALSE;
			}
			break;

		case QHTM_SOURCE_FILENAME:
			if( !sectHTML.SetHTMLFile( pcsz ) )
			{
				return FALSE;
			}
			break;
		}

		WinHelper::CRect rcDraw( *lprc );

		//
		//	Needs to be scoped because firstly we are just measuring and this doesn't require a clip rect
		{
			GS::CDrawContext dc( NULL, hdc, true );
			sectHTML.OnLayout( rcDraw, dc );
		}
		const WinHelper::CSize size( sectHTML.GetSize() );
		rcDraw.bottom = rcDraw.top + size.cy;

		//
		//	Now we know the size, we can pass this into the clip rect.
		{
			GS::CDrawContext dc( &rcDraw, hdc, true );
			sectHTML.OnLayout( rcDraw, dc );
			SelectPalette( hdc, GS::GetCurrentWindowsPalette(), TRUE );
			RealizePalette( hdc );
			sectHTML.OnDraw( dc );
		}
		return TRUE;
	}
#else	//	QHTM_ALLOW_RENDER
	UNREFERENCED_PARAMETER( hdc );
	UNREFERENCED_PARAMETER( pcsz );
	UNREFERENCED_PARAMETER( hInst );
	UNREFERENCED_PARAMETER( uFlags );
	UNREFERENCED_PARAMETER( lprc );
#endif	//	QHTM_ALLOW_RENDER
	return FALSE;
}


