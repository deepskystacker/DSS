/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	RadialGradient.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "GuiTools.h"
#include <math.h>

void GS::RadialGradient( GS::CDIB &dib, const WinHelper::CRect &rc, GS::COLORREFA crFrom, GS::COLORREFA crTo, int nGradientX, int nGradientY, int nGradientRadius )
{
	const int nTop = max( rc.top, 0 );
	register const int nBottom = min( rc.bottom, static_cast< int >( dib.GetHeight() ) );
	const int nLeft = max( rc.left, 0 );
	register const int nRight = min( rc.right, static_cast< int >( dib.GetWidth() ) );

  int nR1 = GetRValue(crTo);
  int nG1 = GetGValue(crTo);
  int nB1 = GetBValue(crTo);
  int nA1 = GetAValue(crTo);

  int nR2 = GetRValue(crFrom);
  int nG2 = GetGValue(crFrom);
  int nB2 = GetBValue(crFrom);
	int nA2 = GetAValue(crFrom);

  int nR;
  int nG;
  int nB;
	int nA;
  int nGradient;

	const GS::CDIB::CLineArray & arrDest = dib.GetLineArray();

  for ( int nY = nTop; nY < nBottom; nY++)
	{
		DIB_PIXEL *pDest = arrDest[ nY ];
		double dPreY = pow( (double)nY - nTop - nGradientY, 2);
		for ( int nX = nLeft; nX < nRight; nX++)
		{
      nGradient = ((nGradientRadius - (int)sqrt( pow((double)nX - nLeft - nGradientX, 2) + dPreY )) * 100) / nGradientRadius;

      if( nGradient < 0 )
			{
          nGradient = 0;
      }

      nR = nR1 + ((nR2 - nR1) * nGradient / 100);
      nG = nG1 + ((nG2 - nG1) * nGradient / 100);
      nB = nB1 + ((nB2 - nB1) * nGradient / 100);
			nA = nA1 + ((nA2 - nA1) * nGradient / 100);
			const int nNegativeAplha = 255 - nA;

      //dibEntireDisplay.PutPixelBlend( nX, nY, nR, nG, nB, nA );
			DIB_PIXEL *pPixel = pDest + nX;
			pPixel->rgbRed = static_cast<unsigned char>( ( pPixel->rgbRed * nNegativeAplha + nR * nA ) >>8 );
			pPixel->rgbGreen = static_cast<unsigned char>( ( pPixel->rgbGreen * nNegativeAplha + nG * nA ) >>8 );
			pPixel->rgbBlue = static_cast<unsigned char>( ( pPixel->rgbBlue * nNegativeAplha + nB * nA ) >>8 );
    }
  }
}
