/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_GetHTMLHeight.cpp
Owner:	russf@gipsysoft.com
Purpose:	Give some HTML and a restriction of width measure the height
					of the HTML.
----------------------------------------------------------------------*/
#include "stdafx.h"

#include "qhtm.h"
#include "Defaults.h"
#include "HtmlSection.h"

extern "C" BOOL WINAPI QHTM_GetHTMLHeight( HDC hdc, LPCTSTR pcsz, HINSTANCE hInst, UINT uFlags, UINT uWidth, UINT *lpuHeight )
{
#ifdef QHTM_ALLOW_RENDER
	if( !IsBadWritePtr( lpuHeight, sizeof( UINT ) ) )
	{
		if( lpuHeight && pcsz && hdc )
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

			WinHelper::CRect rcDraw( 0, 0, uWidth, 5 );
			GS::CDrawContext dc( NULL, hdc, false );
			sectHTML.OnLayout( rcDraw, dc );
			WinHelper::CSize size( sectHTML.GetSize() );
			if (size.cx > (int)uWidth)
				size.cy += GetSystemMetrics(SM_CYHSCROLL);
			*lpuHeight = size.cy;
			return TRUE;
		}
	}
#else	//	QHTM_ALLOW_RENDER
	UNREFERENCED_PARAMETER( hdc );
	UNREFERENCED_PARAMETER( pcsz );
	UNREFERENCED_PARAMETER( hInst );
	UNREFERENCED_PARAMETER( uFlags );
	UNREFERENCED_PARAMETER( uWidth );
	UNREFERENCED_PARAMETER( lpuHeight );
#endif	//	QHTM_ALLOW_RENDER
	return FALSE;
}

