/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBClearToColour.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;


void CDIB::ClearToColour( COLORREFA cr )
{
	const CLineArray & arrDest = GetLineArray();
	const int uSize = static_cast< int >( arrDest.GetSize() );

	register DIB_PIXEL dp;
  dp.rgbBlue = GetBValue( cr );
  dp.rgbGreen = GetGValue( cr );
  dp.rgbRed = GetRValue( cr );
  dp.rgbReserved = GetAValue( cr );

	for( register int u = 0; u < uSize; u++ )
	{
		DIB_PIXEL *pDest = arrDest[ u ];
		for( register int c = 0; c < GetWidth(); c++, pDest++ )
		{
      *pDest = dp;
		}
	}
}


