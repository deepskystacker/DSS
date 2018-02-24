/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodeBMP.cpp
Owner:	rich@woodbridgeinternalmed.com
Purpose:	Decode an BMP file into a frame array.
			The only exported function is:
			bool DecodeBMP( CDataSourceABC &ds, CFrameArray &arrFrames, int &nWidth, int &nHeight )
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <datasource/DataSourceABC.h>
#include "ImgLib.h"
#include "ImgLibInt.h"
#include "DibLoader.h"
#include "Config.h"
#include "CompatibleDC.h"

#ifdef TRACING
	#define ITRACE TRACE
#else	//	TRACING
	#define ITRACE 
#endif	//	TRACING

#ifdef IMGLIB_BMP

static void ConvertCoreHeader(BITMAPCOREHEADER* coreHeader, BITMAPINFOHEADER* infoHeader)
{
   infoHeader->biWidth = coreHeader->bcWidth;
   infoHeader->biHeight = coreHeader->bcHeight;
   infoHeader->biPlanes = coreHeader->bcPlanes;
   infoHeader->biBitCount = coreHeader->bcBitCount;
   infoHeader->biCompression = BI_RGB;
   infoHeader->biSizeImage = 0;
   infoHeader->biXPelsPerMeter = 0;
   infoHeader->biYPelsPerMeter = 0;
   infoHeader->biClrUsed = 0;
   infoHeader->biClrImportant = 0;
}

static void ConvertCoreColors(RGBQUAD* col, int n)
{
   for (int i = n - 1; i >= 0; i--)
   {
      col[i].rgbRed = ((RGBTRIPLE*)col)[i].rgbtRed;
      col[i].rgbGreen = ((RGBTRIPLE*)col)[i].rgbtGreen;
      col[i].rgbBlue = ((RGBTRIPLE*)col)[i].rgbtBlue;
      col[i].rgbReserved = 0xFF;
   }
}


bool DecodeBMP(CDataSourceABC &ds, CFrameArray &arrFrames, SIZE &size )
{
	// Attempt to read and verify the signature by reading the file header
	BITMAPFILEHEADER bmf;
	BITMAPINFOHEADER infoHeader;
	DWORD offBits = 0;
	DWORD headerSize;
	bool isCoreFile;
	GS::CDIB *pDib = NULL;
	
	if ( !ds.ReadBytes((BYTE*)&bmf, sizeof(BITMAPFILEHEADER)) )
		return false;
		
	// Determine if it has a BITMAPFILEHEADER
	if (bmf.bfType != 0x4d42)
	{
		ds.Reset();
	}
	else
	{
		// Save offset from file header, if it makes sense
		if (bmf.bfOffBits)
			offBits = bmf.bfOffBits - sizeof(BITMAPFILEHEADER);			
	}

	// Read the header size, and validate
	if ( !ds.ReadBytes((BYTE*)&headerSize, sizeof(DWORD)) || 
			(headerSize != sizeof(BITMAPINFOHEADER) &&
			headerSize != sizeof(BITMAPCOREHEADER) ) )
		return false;
		
	isCoreFile = (headerSize == sizeof(BITMAPCOREHEADER));
	
	// Prepare the header structure, correcting possible errors in the file
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	
	// If its a core bitmap, convert the header
	if( isCoreFile )
	{
		BITMAPCOREHEADER coreHeader;
		if ( !ds.ReadBytes((BYTE*)&coreHeader.bcWidth, sizeof(BITMAPCOREHEADER) - sizeof(DWORD)) )
			return false;

		ConvertCoreHeader( &coreHeader, &infoHeader );
	}
	else
	{
		if( !ds.ReadBytes((BYTE*)&infoHeader.biWidth, sizeof(BITMAPINFOHEADER) - sizeof(DWORD)) )
			return false;
	}

	CDibLoader* pDibLoader = new CDibLoader( &infoHeader );
	
	// Read the masks
	if (pDibLoader->GetMaskTable())
	{
		if ( !ds.ReadBytes((BYTE*)pDibLoader->GetMaskTable(), 3 * sizeof(DWORD)) )
		{
			delete pDibLoader;
			return false;
		}
	}
			
	// Calculate number of colors in color table
	if (pDibLoader->GetColorTable())
	{
		int colors = pDibLoader->GetColorsUsed();
		// Calculate size of color table to read
		int colorsize = colors * ( isCoreFile ? sizeof(RGBTRIPLE) : sizeof(RGBQUAD));
		
		if ( !ds.ReadBytes((BYTE*)pDibLoader->GetColorTable(), colorsize) )
		{
			delete pDibLoader;
			return false;
		}

		if (isCoreFile)
			ConvertCoreColors(pDibLoader->GetColorTable(), colors);
	}			

	// Skip bits if necessary
	DWORD curPos = ds.GetCurrentPos();

	if (offBits && (DWORD)offBits > curPos)
		ds.SetRelativePos(offBits - curPos);

	if ( !ds.ReadBytes(pDibLoader->GetBits(), pDibLoader->GetAllocatedImageSize()) )
	{
		delete pDibLoader;
		return false;
	}

	size.cx = pDibLoader->GetWidth();
	size.cy = pDibLoader->GetHeight();
	HDC hdc = GetDC( NULL );

	pDib = new GS::CDIB( hdc, size.cx, size.cy );
	if( !pDib->IsValid() )
	{
		delete pDib;
		ReleaseDC( NULL, hdc );
		delete pDibLoader;
		return false;
	}

	{
		CCompatibleDC dc( hdc );
		HGDIOBJ hOld = SelectObject( dc, pDib->GetBitmap() );
		VERIFY( pDibLoader->Draw( dc, 0, 0 ) );
		SelectObject( dc, hOld );
	}
	ReleaseDC( NULL, hdc );
	delete pDibLoader;

#if ( _WIN32_WCE == 300 ||  _WIN32_WCE == 200 ||  _WIN32_WCE == 201 || _WIN32_WCE == 211 )
		//
		//	Needs to be in BGR
		BYTE bTemp;
		const GS::CDIB::CLineArray &arr = pDib->GetLineArray();
		for( PI_uint32 uRow = 0; uRow < pDib->GetHeight(); uRow++ )
		{
			RGBQUAD *p = (RGBQUAD *)arr[ uRow ];
			for( PI_uint32 uCol = 0; uCol < pDib->GetWidth(); uCol++ )
			{
				bTemp = p[ uCol ].rgbRed;
				p[ uCol ].rgbRed = p[ uCol ].rgbGreen;

				p[ uCol ].rgbBlue = 0;
				p[ uCol ].rgbGreen = 0;
				p[ uCol ].rgbBlue = 0;
				p[ uCol ].rgbReserved = 255;
			}
		}

#endif	//	
	CFrame *pFrame = new CFrame( pDib, 0 );

	arrFrames.Add( pFrame );

	if( pDib )
		return true;
	return false;
}

#endif	//	IMGLIB_BMP