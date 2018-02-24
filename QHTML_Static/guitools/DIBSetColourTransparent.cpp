/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBSetColourTransparent.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;


void CDIB::SetColourTransparent( COLORREF crTransparent )
{
	m_bTransparentColorSet = true;
	m_crTransparent = crTransparent;
	SetHasAlphaChannel( true );

	BYTE bRed, bGreen, bBlue;
	bRed = GetRValue( crTransparent );
	bGreen = GetGValue( crTransparent );
	bBlue = GetBValue( crTransparent );

	const CLineArray & arrSrc = GetLineArray();
	for( int u = 0; u < static_cast< int >( arrSrc.GetSize() ); u++ )
	{
		DIB_PIXEL *pSrc = arrSrc[ u ];
		for( int c = 0; c < GetWidth(); c++, pSrc++ )
		{
			if( pSrc->rgbRed == bRed && pSrc->rgbGreen == bGreen && pSrc->rgbBlue == bBlue )
			{
				pSrc->rgbReserved = 0;
			}
			else
			{
				pSrc->rgbReserved = 255;
			}
		}
	}
}


