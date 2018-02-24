/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBRectangle.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;

void CDIB::Rectangle( const RECT &rc, COLORREFA cr )
{
	const int nTop = max( rc.top, 0 );
	const int nBottom = min( rc.bottom, GetHeight() );
	const int nLeft = max( rc.left, 0 );
	const int nWidth = rc.right - rc.left;

	const BYTE r = GetRValue( cr );
	const BYTE g = GetGValue( cr );
	const BYTE b = GetBValue( cr );
	const BYTE a = GetAValue( cr );

	const int nNegativeAplha = 255 - a;
  
	const CLineArray & arrDest = GetLineArray();
	for( int y = nTop; y < nBottom; y++ )
	{
		DIB_PIXEL *pDest = arrDest[ y ];

		pDest += nLeft;
    pDest->rgbRed = static_cast<unsigned char>( ( pDest->rgbRed * nNegativeAplha + r * a ) >>8 );
    pDest->rgbGreen = static_cast<unsigned char>( ( pDest->rgbGreen * nNegativeAplha + g * a ) >>8 );
    pDest->rgbBlue = static_cast<unsigned char>( ( pDest->rgbBlue * nNegativeAplha + b * a ) >>8 );

		pDest += nWidth;
    pDest->rgbRed = static_cast<unsigned char>( ( pDest->rgbRed * nNegativeAplha + r * a ) >>8 );
    pDest->rgbGreen = static_cast<unsigned char>( ( pDest->rgbGreen * nNegativeAplha + g * a ) >>8 );
    pDest->rgbBlue = static_cast<unsigned char>( ( pDest->rgbBlue * nNegativeAplha + b * a ) >>8 );
	}
	DrawLine( rc.left, rc.top, rc.right, rc.top, cr );
	DrawLine( rc.left, rc.bottom, rc.right, rc.bottom, cr );
}


