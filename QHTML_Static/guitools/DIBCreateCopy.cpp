/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBCreateCopy.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"
#include "CompatibleDC.h"

using namespace GS;



CDIB *CDIB::CreateCopy( int nCX, int nCY ) const
{
#if !defined (_WIN32_WCE)
	HDC hdc = GetDC( NULL );

	CDIB *pdib = new CDIB( hdc, nCX, nCY );
	CCompatibleDC hdcMem( hdc );

	HGDIOBJ hOld = SelectObject( hdcMem, pdib->GetBitmap() );

	::StretchDIBits( hdcMem,
              0,
              0,
              nCX,
              nCY,
              0,
              0,
              GetWidth(),
              GetHeight(),
              m_pBits,
              (BITMAPINFO *) &m_bi,
              DIB_RGB_COLORS,
              SRCCOPY );

	SelectObject( hdcMem, hOld );

	ReleaseDC( NULL, hdc );
	if( m_bTransparentColorSet )
	{
		pdib->SetColourTransparent( m_crTransparent );
	}

	if( m_bHasAlpha )
	{
		pdib->SetHasAlphaChannel( m_bHasAlpha );
	}
	return pdib;
#else	//	#if !defined (_WIN32_WCE)
	ASSERT( FALSE );
	return NULL;
#endif	//	#if !defined (_WIN32_WCE)
}


