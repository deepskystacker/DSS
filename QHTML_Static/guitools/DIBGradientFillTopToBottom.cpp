/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBGradientFillTopToBottom.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;

void CDIB::GradientFillTopToBottom( const RECT &rc, COLORREF crTop, COLORREF crBottom )
{
	const int nTop = max( rc.top, 0 );
	const int nBottom = min( rc.bottom, GetHeight() );
	const int nLeft = max( rc.left, 0 );
	const int nRight = min( rc.right, GetWidth() );

  const int r1 = GetRValue( crTop ), g1 = GetGValue( crTop ), b1 = GetBValue( crTop ), a1 = GetAValue( crTop );
  const int r2 = GetRValue( crBottom ), g2 = GetGValue( crBottom ), b2 = GetBValue( crBottom ), a2 = GetAValue( crBottom );

	int nEnd = rc.bottom - rc.top;

	const CLineArray & arrDest = GetLineArray();
	int nLine = 0;
	for( int y = nTop; y < nBottom; y++, nLine++ )
	{
		DIB_PIXEL *pDest = arrDest[ y ];
		pDest += nLeft;
    int r,g,b, a; 

    r = r1 + (nLine * (r2-r1) / nEnd); 
    g = g1 + (nLine * (g2-g1) / nEnd);
    b = b1 + (nLine * (b2-b1) / nEnd);
		a = a1 + (nLine * (a2-a1) / nEnd);
		
		const int nNegativeAplha = 255 - a;

		for( int x = nLeft; x < nRight; x++, pDest++ )
		{
      pDest->rgbRed = static_cast<unsigned char>( ( pDest->rgbRed * nNegativeAplha + r * a ) >>8 );
      pDest->rgbGreen = static_cast<unsigned char>( ( pDest->rgbGreen * nNegativeAplha + g * a ) >>8 );
      pDest->rgbBlue = static_cast<unsigned char>( ( pDest->rgbBlue * nNegativeAplha + b * a ) >>8 );
		}
	}
}
