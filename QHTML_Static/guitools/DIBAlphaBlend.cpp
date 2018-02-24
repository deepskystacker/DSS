/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBAlphaBlend.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;

void CDIB::AlphaBlend( HDC hdc, int nX, int nY, unsigned char nAlpha )
{
	//
	//	Grab a copy of what's there already...
	CDIB dib( hdc, nX, nY, GetWidth(), GetHeight() );

	//
	//	Now copy our stuff into the copy of the current screen...
	const CLineArray & arrScreen = dib.GetLineArray();
	const CLineArray & arrSrc = GetLineArray();

	if( m_bHasAlpha )
	{
		const int uLines = static_cast< int >( arrScreen.GetSize() );
		for( int u = 0; u < uLines; u++ )
		{
			DIB_PIXEL *pDest = arrScreen[ u ];
			DIB_PIXEL *pSrc = arrSrc[ u ];
			for( int c = 0; c < GetWidth(); c++, pDest++, pSrc++ )
			{
				if( pSrc->rgbReserved )
				{
					unsigned char nPixelAlpha = static_cast<unsigned char>( pSrc->rgbReserved * 256 / 255 * nAlpha / 256 );
					unsigned char nNegativeAplha = static_cast<unsigned char>( 255 - nPixelAlpha );
					
					ALPHA( pSrc, pDest, rgbRed, nPixelAlpha, nNegativeAplha );
					ALPHA( pSrc, pDest, rgbGreen, nPixelAlpha, nNegativeAplha );
					ALPHA( pSrc, pDest, rgbBlue, nPixelAlpha, nNegativeAplha );
				}
			}
		}
	}
	else
	{
		unsigned char nNegativeAplha = static_cast<unsigned char>( 255 - nAlpha );

		const int uLines = static_cast< int >( arrScreen.GetSize() );
		for( int u = 0; u < uLines; u++ )
		{
			DIB_PIXEL *pDest = arrScreen[ u ];
			DIB_PIXEL *pSrc = arrSrc[ u ];
			for( int c = 0; c < GetWidth(); c++, pDest++, pSrc++ )
			{
				ALPHA( pSrc, pDest, rgbRed, nAlpha, nNegativeAplha );
				ALPHA( pSrc, pDest, rgbGreen, nAlpha, nNegativeAplha );
				ALPHA( pSrc, pDest, rgbBlue, nAlpha, nNegativeAplha );
			}
		}
	}
	dib.Draw( hdc, nX, nY );
}
