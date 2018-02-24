/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIBCreateResized.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"

using namespace GS;


CDIB *CDIB::CreateResized( int newx, int newy ) const
{
	/*

		Most of this code was taken from CxImage and then heavily modified
		to allow the use of RGBQUAD and inline alpha channel (the way we do things)

	*/
	HDC hdc = GetDC( NULL );
	CDIB * pDib = new CDIB( hdc, newx, newy );
	pDib->ClearToColour( DIB_RGBA( 255, 0, 0, 255 ) );
	::ReleaseDC( NULL, hdc );

	float xScale, yScale, fX, fY;
	xScale = (float)GetWidth()  / (float)newx;
	yScale = (float)GetHeight() / (float)newy;

	LPBYTE pSourceImageData = (LPBYTE)m_pBits;

	DWORD dwEffectiveWidth = ((((32 * GetWidth()) + 31) / 32) * BYTES_PER_PIXEL);
	DWORD dwDestinationEffectiveWidth = ((((32 * pDib->GetWidth()) + 31) / 32) * BYTES_PER_PIXEL);

	if( !( GetWidth() >newx && GetHeight()>newy ) )
	{
		//© 1999 Steve McMahon (steve@dogma.demon.co.uk)
		long ifX, ifY, ifX1, ifY1, xmax, ymax;
		float ir1, ir2, ig1, ig2, ib1, ib2, ia1, ia2, dx, dy;

		BYTE r,g,b,a;

		RGBQUAD rgb1, rgb2, rgb3, rgb4;
		xmax = GetWidth()-1;
		ymax = GetHeight()-1;

		const CLineArray & arrDest = pDib->GetLineArray();

		for(int y=0; y<newy; y++)
		{
			fY = y * yScale;
			ifY = (int)fY;
			ifY1 = min(ymax, ifY+1);
			dy = fY - ifY;
			float DYMinus = 1-dy;
			
			DIB_PIXEL *pPixelRow = arrDest[ y ];
			const long lYOffset = ifY * dwEffectiveWidth;
			const long lYOffset1 = ifY1 * dwEffectiveWidth;
			
			for(int x=0; x<newx; x++)
			{
				fX = x * xScale;
				ifX = (int)fX;
				ifX1 = min(xmax, ifX+1);
				dx = fX - ifX;
				float DXMinus = 1-dx;

				BYTE* pCurrentSource;
				const long lXOffset = ifX*BYTES_PER_PIXEL;
				const long lXOffset1 = ifX1*BYTES_PER_PIXEL;

				pCurrentSource = pSourceImageData + lYOffset + lXOffset;
				rgb1.rgbBlue = *pCurrentSource++;
				rgb1.rgbGreen= *pCurrentSource++;
				rgb1.rgbRed =*pCurrentSource++;
				rgb1.rgbReserved =*pCurrentSource;

				pCurrentSource = pSourceImageData + lYOffset + lXOffset1;
				rgb2.rgbBlue = *pCurrentSource++;
				rgb2.rgbGreen= *pCurrentSource++;
				rgb2.rgbRed =*pCurrentSource++;
				rgb2.rgbReserved =*pCurrentSource++;

				pCurrentSource = pSourceImageData + lYOffset1 + lXOffset;
				rgb3.rgbBlue = *pCurrentSource++;
				rgb3.rgbGreen= *pCurrentSource++;
				rgb3.rgbRed =*pCurrentSource++;
				rgb3.rgbReserved =*pCurrentSource++;

				pCurrentSource = pSourceImageData + lYOffset1 + lXOffset1;
				rgb4.rgbBlue = *pCurrentSource++;
				rgb4.rgbGreen= *pCurrentSource++;
				rgb4.rgbRed =*pCurrentSource++;
				rgb4.rgbReserved =*pCurrentSource++;

				// Interplate in x direction:
				ir1 = rgb1.rgbRed   * DYMinus + rgb3.rgbRed   * dy;
				ig1 = rgb1.rgbGreen * DYMinus + rgb3.rgbGreen * dy;
				ib1 = rgb1.rgbBlue  * DYMinus + rgb3.rgbBlue  * dy;
				ia1 = rgb1.rgbReserved * DYMinus + rgb3.rgbReserved * dy;

				ir2 = rgb2.rgbRed   * DYMinus + rgb4.rgbRed   * dy;
				ig2 = rgb2.rgbGreen * DYMinus + rgb4.rgbGreen * dy;
				ib2 = rgb2.rgbBlue  * DYMinus + rgb4.rgbBlue  * dy;
				ia2 = rgb2.rgbReserved * DYMinus + rgb4.rgbReserved * dy;

				// Interpolate in y:
				r = (BYTE)(ir1 * DXMinus + ir2 * dx);
				g = (BYTE)(ig1 * DXMinus + ig2 * dx);
				b = (BYTE)(ib1 * DXMinus + ib2 * dx);
				a = (BYTE)(ia1 * DXMinus + ia2 * dx);

				// Set output
				DIB_PIXEL *pPixel = pPixelRow + x;
				pPixel->rgbRed = r;
				pPixel->rgbGreen = g;
				pPixel->rgbBlue = b;
				pPixel->rgbReserved = a;
			}
		} 
	}
	else
	{
		//high resolution shrink, thanks to Henrik Stellmann <henrik.stellmann@volleynet.de>
		const long ACCURACY = 1000;

		int i,j; // index for faValue
		int x,y; // coordinates in  source image
		BYTE* pSource;
		BYTE* pDest = (BYTE* )pDib->m_pBits;
		long* naAccu  = new long[BYTES_PER_PIXEL * newx + BYTES_PER_PIXEL];
		long* naCarry = new long[BYTES_PER_PIXEL * newx + BYTES_PER_PIXEL];
		long* naTemp;
		long  nWeightX,nWeightY;
		float fEndX;
		long nScale = (long)(ACCURACY * xScale * yScale);

		memset(naAccu,  0, sizeof(long) * BYTES_PER_PIXEL * newx);
		memset(naCarry, 0, sizeof(long) * BYTES_PER_PIXEL * newx);

		int u, v = 0; // coordinates in dest image
		float fEndY = yScale - 1.0f;

		for (y = 0; y < GetHeight(); y++)
		{
			pSource = pSourceImageData + y * dwEffectiveWidth;
			u = i = 0;
			fEndX = xScale - 1.0f;
			if ((float)y < fEndY)
			{       // complete source row goes into dest row
				for (x = 0; x < GetWidth(); x++)
				{
					if ((float)x < fEndX)
					{       // complete source pixel goes into dest pixel
						for (j = 0; j < BYTES_PER_PIXEL; j++)
							naAccu[i + j] += (*pSource++) * ACCURACY;

					}
					else
					{       // source pixel is splitted for 2 dest pixels
						nWeightX = (long)(((float)x - fEndX) * ACCURACY);
						for (j = 0; j < BYTES_PER_PIXEL; j++)
						{
							naAccu[i] += (ACCURACY - nWeightX) * (*pSource);
							naAccu[BYTES_PER_PIXEL + i++] += nWeightX * (*pSource++);
						}
						fEndX += xScale;
						u++;
					}
				}
			}
			else
			{       // source row is splitted for 2 dest rows       
				nWeightY = (long)(((float)y - fEndY) * ACCURACY);

				for (x = 0; x < GetWidth(); x++)
				{
					if ((float)x < fEndX)
					{       // complete source pixel goes into 2 pixel
						for (j = 0; j < BYTES_PER_PIXEL; j++)
						{
							naAccu[i + j] += ((ACCURACY - nWeightY) * (*pSource));
							naCarry[i + j] += nWeightY * (*pSource++);
						}
					}
					else
					{       // source pixel is splitted for 4 dest pixels
						nWeightX = (int)(((float)x - fEndX) * ACCURACY);
						for (j = 0; j < BYTES_PER_PIXEL; j++)
						{
							naAccu[i] += ((ACCURACY - nWeightY) * (ACCURACY - nWeightX)) * (*pSource) / ACCURACY;
							*pDest++ = (BYTE)(naAccu[i] / nScale);
							naCarry[i] += (nWeightY * (ACCURACY - nWeightX) * (*pSource)) / ACCURACY;
							naAccu[i + BYTES_PER_PIXEL] += ((ACCURACY - nWeightY) * nWeightX * (*pSource)) / ACCURACY;
							naCarry[i + BYTES_PER_PIXEL] = (nWeightY * nWeightX * (*pSource)) / ACCURACY;
							i++;
							pSource++;
						}
						fEndX += xScale;
						u++;
					}
				}


				if( u < newx )
				{
					// possibly not completed due to rounding errors
					for (j = 0; j < BYTES_PER_PIXEL; j++)
						*pDest++ = (BYTE)(naAccu[i++] / nScale);
				}

				naTemp = naCarry;
				naCarry = naAccu;
				naAccu = naTemp;
				memset(naCarry, 0, sizeof(int) * BYTES_PER_PIXEL);    // need only to set first pixel zero
				pDest = (BYTE* )pDib->m_pBits + (++v * dwDestinationEffectiveWidth );
				fEndY += yScale;
			}
		}


		if (v < newy)
		{	// possibly not completed due to rounding errors
			for (i = 0; i < BYTES_PER_PIXEL * newx; i++)
				*pDest++ = (BYTE)(naAccu[i] / nScale);
		}
		delete [] naAccu;
		delete [] naCarry;
	}

	return pDib;
}