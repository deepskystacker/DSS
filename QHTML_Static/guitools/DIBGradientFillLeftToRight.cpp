/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBGradientFillLeftToRight.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;


void CDIB::GradientFillLeftToRight( const RECT &rc, COLORREF crTop, COLORREF crBottom )
{
	const int nTop = max( rc.top, 0 );
	const int nBottom = min( rc.bottom, static_cast< int >( GetHeight() ) );
	const int nLeft = max( rc.left, 0 );
	const int nRight = min( rc.right, static_cast< int >( GetWidth() ) );

  const int r1 = GetRValue( crTop ), g1 = GetGValue( crTop ), b1 = GetBValue( crTop ), a1 = GetAValue( crTop );
  const int r2 = GetRValue( crBottom ), g2 = GetGValue( crBottom ), b2 = GetBValue( crBottom ), a2 = GetAValue( crBottom );

	int nEnd = rc.right - rc.left;

	int nCol = 0;
	for( int x = nLeft; x < nRight; x++, nCol++)
	{
    const int r = r1 + (nCol * (r2-r1) / nEnd); 
    const int g = g1 + (nCol * (g2-g1) / nEnd);
    const int b = b1 + (nCol * (b2-b1) / nEnd);
		const int a = a1 + (nCol * (a2-a1) / nEnd);
		const COLORREFA cr = DIB_RGBA( r, g, b, a );
		DrawLine( x, nTop, x, nBottom, cr );
	}
}


