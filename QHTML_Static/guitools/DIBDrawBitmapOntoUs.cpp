/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBDrawBitmapOntoUs.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"
#include "CompatibleDC.h"

using namespace GS;


void CDIB::DrawBitmapOntoUs( HBITMAP hBitmap, int x, int y, int nSourceX, int nSourceY, int cx, int cy )
{
	//
	//	Copy the DC into the DIB.
	HDC hdc = GetDC( NULL );
	{
		BITMAP bm;
		GetObject( hBitmap, sizeof( bm ), &bm );

		CCompatibleDC hdcMemDest( hdc );
		CCompatibleDC hdcMemSource( hdc );

		HGDIOBJ hOld = SelectObject( hdcMemDest, m_hBitmap );
		HGDIOBJ hOld2 = SelectObject( hdcMemSource, hBitmap );
		BitBlt( hdcMemDest, x, y, cx, cy, hdcMemSource, nSourceX, nSourceY, SRCCOPY );
		SelectObject( hdcMemDest, hOld );
		SelectObject( hdcMemSource, hOld2 );
	}
	ReleaseDC( NULL, hdc );
}


