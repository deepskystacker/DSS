/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBConvertToGrayScale.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;

void GS::ConvertToGrayScale( GS::CDIB &dib )
{
	const GS::CDIB::CLineArray & arrDest = dib.GetLineArray();
	const int uSize = static_cast< int >( arrDest.GetSize() );

	for( register int u = 0; u < uSize; u++ )
	{
		DIB_PIXEL *pDest = arrDest[ u ];
		for( int c = 0; c < dib.GetWidth(); c++, pDest++ )
		{
			const BYTE bNew = (BYTE)( (pDest->rgbRed + pDest->rgbGreen + pDest->rgbBlue) / 3 );
			pDest->rgbRed = bNew;
			pDest->rgbGreen = bNew;
			pDest->rgbBlue = bNew;
		}
	}
	
}
