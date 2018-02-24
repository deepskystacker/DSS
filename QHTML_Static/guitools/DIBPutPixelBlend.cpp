/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBPutPixelBlend.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;


void CDIB::PutPixelBlend( int x, int y, BYTE r, BYTE g, BYTE b, BYTE a )
{
	if( a )
	{
		const CLineArray & arrDest = GetLineArray();
		if( y < static_cast< int >( arrDest.GetSize() ) && y >= 0 )
		{
			DIB_PIXEL *pDest = arrDest[ y ];
			if( x >= 0 && x < GetWidth() )
			{
				const int nNegativeAplha = 255 - a;
				DIB_PIXEL *pPixel = pDest + x;
				pPixel->rgbRed = static_cast<unsigned char>( ( pPixel->rgbRed * nNegativeAplha + r * a ) >>8 );
				pPixel->rgbGreen = static_cast<unsigned char>( ( pPixel->rgbGreen * nNegativeAplha + g * a ) >>8 );
				pPixel->rgbBlue = static_cast<unsigned char>( ( pPixel->rgbBlue * nNegativeAplha + b * a ) >>8 );
			}
		}
	}
}


