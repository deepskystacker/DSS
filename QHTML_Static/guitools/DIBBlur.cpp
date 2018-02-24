/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBBlur.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;

void CDIB::Blur( int nTimes )
{
#define BLUR( color, bytes )  \
		    (BYTE)((bPrev[nCol - bytes].color + bPrev[nCol].color + bPrev[nCol + bytes].color \
 		   + pbyte[nCol - bytes].color + pbyte[nCol].color + pbyte[nCol + bytes].color	\
		   + bNext[nCol - bytes].color + bNext[nCol].color + bNext[nCol + bytes].color) / 9)\

	//
	//	REVIEW - russf - There is a problem when blurring a mostly white image in that it gives some
	//										artifact sliding down the display.
	const CLineArray arr = GetLineArray();

	#define bytes 1

	while( nTimes )
	{
		for( int nRow = bytes; nRow < static_cast< int >( arr.GetSize() ) - bytes; nRow++ )
		{
			const DIB_PIXEL * bPrev = arr[ nRow - 1];
			DIB_PIXEL * pbyte = arr[ nRow ];
			const DIB_PIXEL * bNext = arr[ nRow + 1];
			for( int nCol = bytes; nCol < GetWidth() - bytes ; nCol++ )
			{
				pbyte[nCol].rgbBlue = BLUR( rgbBlue, bytes );
				pbyte[nCol].rgbRed = BLUR( rgbRed, bytes );
				pbyte[nCol].rgbGreen = BLUR( rgbGreen, bytes );

				//
				//	Don't forget to also blur th alpha channel
				pbyte[nCol].rgbReserved = BLUR( rgbReserved, bytes );
			}
		}
		nTimes--;
	}
}


