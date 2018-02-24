/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	CreateRoundCornerRegion.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "GuiTools.h"


HRGN GS::CreateRoundCornerRegion( const WinHelper::CRect &rc, UINT uRoundCorners, UINT uRoundness )
{
#ifndef UNDER_CE
	HRGN hrgn = CreateRoundRectRgn( rc.left, rc.top, rc.right + 1, rc.bottom + 1, uRoundness, uRoundness );
	HRGN hrgn2 = NULL;

	if( !( uRoundCorners & GS::knGFCRoundLeftTop ) )
	{
		hrgn2 = CreateRectRgn( rc.left, rc.top, rc.left + uRoundness, rc.top + uRoundness );
		CombineRgn( hrgn, hrgn, hrgn2, RGN_OR );
		DeleteObject( hrgn2 );
	}

	if( !( uRoundCorners & GS::knGFCRoundRightTop ) )
	{
		hrgn2 = CreateRectRgn( rc.right - uRoundness, rc.top, rc.right, rc.top + uRoundness );
		CombineRgn( hrgn, hrgn, hrgn2, RGN_OR );
		DeleteObject( hrgn2 );
	}

	if( !( uRoundCorners & GS::knGFCRoundLeftBottom ) )
	{
		hrgn2 = CreateRectRgn( rc.left, rc.bottom - uRoundness, rc.left + uRoundness, rc.bottom );
		CombineRgn( hrgn, hrgn, hrgn2, RGN_OR );
		DeleteObject( hrgn2 );
	}

	if( !( uRoundCorners & GS::knGFCRoundRightBottom ) )
	{
		hrgn2 = CreateRectRgn( rc.right - uRoundness, rc.bottom - uRoundness, rc.right, rc.bottom );
		CombineRgn( hrgn, hrgn, hrgn2, RGN_OR );
		DeleteObject( hrgn2 );
	}

	return hrgn;
#else	//	UNDER_CE
	ASSERT( FALSE );
	return NULL;
#endif	//	UNDER_CE
}
