/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DrawBitmapShadow.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

extern void DrawBitmapShadow( HDC hdc, HBITMAP bmp, COLORREF crBackground, COLORREF crShadow, int x, int y, int nOffset, int nSourceX, int nSourceY, int nCX, int nCY );

void DrawBitmapShadow( HDC hdc, HBITMAP bmp, COLORREF crBackground, COLORREF crShadow, int x, int y, int nOffset, int nSourceX, int nSourceY, int nCX, int nCY )
{
	const int nBlur = 1;
	const int nWidth = nCX + nOffset + nBlur * 2;
	const int nHeight = nCY + nOffset + nBlur * 2;

	GS::CDIB dibDest( hdc, nWidth, nHeight );
	dibDest.ClearToColour( crBackground );
	dibDest.DrawBitmapOntoUs( bmp, nBlur, nBlur, nSourceX, nSourceY, nCX, nCY );

	const BYTE bRed = GetRValue( crBackground )
						, bGreen = GetGValue( crBackground )
						, bBlue GetBValue( crBackground )
						;

	const BYTE bShadowRed = GetRValue( crShadow )
						, bShadowGreen = GetGValue( crShadow )
						, bShadowBlue = GetBValue( crShadow )
						;

	const int nAlpha = 150;

	const GS::CDIB::CLineArray & arrDest = dibDest.GetLineArray();
	const int uSourceLines = static_cast< int >( arrDest.GetSize() );

	for( int u = 0; u < uSourceLines; u++ )
	{
		DIB_PIXEL *pDest = arrDest[ u ];
		for( int c = 0; c < nWidth; c++, pDest++ )
		{
			if( pDest->rgbRed != bRed || pDest->rgbGreen != bGreen || pDest->rgbBlue != bBlue )
			{
				pDest->rgbRed = bShadowRed;
				pDest->rgbGreen = bShadowGreen;
				pDest->rgbBlue = bShadowBlue;
				pDest->rgbReserved = nAlpha;
			}
			else
			{
				pDest->rgbReserved = 0;
				pDest->rgbRed = pDest->rgbGreen = pDest->rgbBlue = 0;
			}
		}
	}

	dibDest.Blur( nBlur );
	dibDest.DrawWithAlphaChannel( hdc, x + nOffset - nBlur, y + nOffset - nBlur );

}
