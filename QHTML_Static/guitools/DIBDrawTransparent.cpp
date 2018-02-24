/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBDrawTransparent.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;

bool CDIB::DrawTransparent( HDC hdc, int nX, int nY, COLORREF crTransparent )
{
	//
	//	Grab a copy of what's there already...
	CDIB dib( hdc, nX, nY, GetWidth(), GetHeight() );

	//
	//	Now copy our stuff into the copy of the current screen...

	const CLineArray & arrScreen = dib.GetLineArray();
	const CLineArray & arrSrc = GetLineArray();

	BYTE bRed, bGreen, bBlue;
	bRed = GetRValue( crTransparent );
	bGreen = GetGValue( crTransparent );
	bBlue = GetBValue( crTransparent );

	for( int u = 0; u < static_cast< int >( arrScreen.GetSize() ); u++ )
	{
		DIB_PIXEL *pDest = arrScreen[ u ];
		DIB_PIXEL *pSrc = arrSrc[ u ];
		for( int c = 0; c < GetWidth(); c++, pDest++, pSrc++ )
		{
			if( pSrc->rgbRed != bRed || pSrc->rgbGreen != bGreen || pSrc->rgbBlue != bBlue )
			{
				pDest->rgbRed = pSrc->rgbRed;
				pDest->rgbGreen = pSrc->rgbGreen;
				pDest->rgbBlue = pSrc->rgbBlue;
			}
		}
	}
	return dib.Draw( hdc, nX, nY );
}
