/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBDrawWithAlphaChannel.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;

bool CDIB::DrawWithAlphaChannel( HDC hdc, int nX, int nY )
{
	//
	//	Grab a copy of what's there already...
	CDIB dib( hdc, nX, nY, GetWidth(), GetHeight() );

	//
	//	Now copy our stuff into the copy of the current screen...
	const CLineArray & arrScreen = dib.GetLineArray();
	const CLineArray & arrSrc = GetLineArray();

	unsigned char nNegativeAplha;
	for( int u = 0; u < static_cast< int >( arrScreen.GetSize() ); u++ )
	{
		DIB_PIXEL *pDest = arrScreen[ u ];
		DIB_PIXEL *pSrc = arrSrc[ u ];
		for( int c = 0; c < GetWidth(); c++, pDest++, pSrc++ )
		{
			const int nAlpha = pSrc->rgbReserved;
      nNegativeAplha = static_cast<unsigned char>( 255 - nAlpha );

      ALPHA( pSrc, pDest, rgbRed, nAlpha, nNegativeAplha );
      ALPHA( pSrc, pDest, rgbGreen, nAlpha, nNegativeAplha );
      ALPHA( pSrc, pDest, rgbBlue, nAlpha, nNegativeAplha );
		}
	}
	return dib.BasicDraw( hdc, nX, nY );
}

