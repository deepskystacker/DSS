/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodeICO.cpp
Owner:	russf@gipsysoft.com
Purpose:

	Not yet finished

----------------------------------------------------------------------*/
#include "stdafx.h"
#include <datasource/DataSourceABC.h>
#include <datasource/ResourceDataSource.h>
#include <reuse/winhelper.h>
#include "ImgLib.h"
#include "ImgLibInt.h"
#include "DibLoader.h"
#include "Config.h"
#include "CompatibleDC.h"

#define TRACING	1

#ifdef TRACING
	#define ITRACE TRACE
#else	//	TRACING
	#define ITRACE 
#endif	//	TRACING

#ifdef IMGLIB_ICO


inline int GetColorsUsed( const int nBitCount )
{
  switch( nBitCount )
	{
  case 1:		return 2;
  case 4:		return 16;
  case 8:		return 256; 
  }
	return 0;
}

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



#pragma pack( push )
#pragma pack( 2 )
typedef struct tagIconDirectoryEntry {
    BYTE  bWidth;
    BYTE  bHeight;
    BYTE  bColorCount;
    BYTE  bReserved;
    WORD  wPlanes;
    WORD  wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
} ICONDIRENTRY;

typedef struct tagIconDir {
    WORD          idReserved;
    WORD          idType;
    WORD          idCount;
} ICONHEADER;

typedef struct
{
	BYTE	bWidth;               // Width of the image
	BYTE	bHeight;              // Height of the image (times 2)
	BYTE	bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;            // Reserved
	WORD	wPlanes;              // Color Planes
	WORD	wBitCount;            // Bits per pixel
	DWORD	dwBytesInRes;         // how many bytes in this resource?
	WORD	nID;                  // the ID
} MEMICONDIRENTRY, *LPMEMICONDIRENTRY;
#pragma pack( pop )

GS::CDIB *ReadIconImageFromResource( CDataSourceABC &ds, WinHelper::CSize &size )
{
	DWORD headerSize;
	// Read the header size, and validate
	if ( !ds.ReadBytes((BYTE*)&headerSize, sizeof(DWORD)) || 
			(headerSize != sizeof(BITMAPINFOHEADER) &&
			headerSize != sizeof(BITMAPCOREHEADER) ) )
	{
		return NULL;
	}


	bool isCoreFile = (headerSize == sizeof(BITMAPCOREHEADER));
	DWORD offBits = 0;

	BITMAPINFOHEADER infoHeader;			
	// Prepare the header structure, correcting possible errors in the file
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	
	// If its a core bitmap, convert the header
	if( isCoreFile )
	{
		BITMAPCOREHEADER coreHeader;
		if ( !ds.ReadBytes((BYTE*)&coreHeader.bcWidth, sizeof(BITMAPCOREHEADER) - sizeof(DWORD)) )
		{
			return NULL;
		}

		ConvertCoreHeader( &coreHeader, &infoHeader );
	}
	else
	{
		if( !ds.ReadBytes((BYTE*)&infoHeader.biWidth, sizeof(BITMAPINFOHEADER) - sizeof(DWORD)) )
		{
			return NULL;
		}
	}

	infoHeader.biHeight /= 2;

	CDibLoader* pDibLoader = new CDibLoader( &infoHeader );
	
	// Read the masks
	if (pDibLoader->GetMaskTable())
	{
		if ( !ds.ReadBytes((BYTE*)pDibLoader->GetMaskTable(), 3 * sizeof(DWORD)) )
		{
			delete pDibLoader;
			return NULL;
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
			return NULL;
		}

		if (isCoreFile)
			ConvertCoreColors(pDibLoader->GetColorTable(), colors);
	}			

	// Skip bits if necessary
	DWORD curPos = ds.GetCurrentPos();

	if (offBits && (DWORD)offBits > curPos)
		ds.SetRelativePos(offBits - curPos);


	//
	//	Read the image data...
	const int nBytesToRead = pDibLoader->GetAllocatedImageSize() ;
	if ( !ds.ReadBytes(pDibLoader->GetBits(), nBytesToRead ) )
	{
		delete pDibLoader;
		return NULL;
	}

	size.cx = pDibLoader->GetWidth();
	size.cy = pDibLoader->GetHeight();
	HDC hdc = GetDC( NULL );

	GS::CDIB *pDib = new GS::CDIB( hdc, size.cx, size.cy );
	{
		CCompatibleDC dc( hdc );
		HGDIOBJ hOld = SelectObject( dc, pDib->GetBitmap() );
		pDibLoader->Draw( dc, 0, 0 );
		SelectObject( dc, hOld );
	}
	ReleaseDC( NULL, hdc );
	delete pDibLoader;

	if( infoHeader.biBitCount <= 24 )
	{
		pDibLoader = new CDibLoader( infoHeader.biWidth, infoHeader.biHeight, 1 );
		if ( !ds.ReadBytes(pDibLoader->GetBits(), pDibLoader->GetAllocatedImageSize() ) )
		{
			delete pDibLoader;
			return NULL;
		}

		const BYTE *pSrcBytes = pDibLoader->GetBits();
		const int maskwdt = ((infoHeader.biWidth+31) / 32) * 4;	//line width of AND mask (always 1 Bpp)

		const GS::CDIB::CLineArray &la = pDib->GetLineArray();
		for (int y = 0; y < infoHeader.biHeight; y++)
		{
			DIB_PIXEL *p = la[ y ];
			for (int x = 0; x < infoHeader.biWidth; x++, p++)
			{
				if (((pSrcBytes[(infoHeader.biHeight - 1 - y)*maskwdt+(x>>3)]>>(7-x%8))&0x01))
				{
					p->rgbReserved = 0;
				}
				else
				{
					p->rgbReserved = 255;
				}
			}

		}
		delete pDibLoader;

		pDib->SetHasAlphaChannel( true );
	}
	else
	{
		pDib->SetHasAlphaChannel( true );
	}

	return pDib;
}


bool DecodeICO( CDataSourceABC &ds, CFrameArray &arrFrames, SIZE &size )
{
	size.cx = size.cy = 0;

	ICONHEADER icon_header;

	if( ds.ReadBytes( (LPBYTE)&icon_header, sizeof(icon_header) ) )
	{
		// check if it's an icon or a cursor
		if ((icon_header.idReserved == 0) && ((icon_header.idType == 1)||(icon_header.idType == 2)))
		{
			//ITRACE( _T("Looks like an icon with %d images\n"), icon_header.idCount );

			//
			//	Okay, if we have a resource to our datasource then we must be reading an icon from
			//	either a DLL or an EXE. So, for these we need to use the dwImageOffset as a resource
			//	ID and load from there instead.
			GS::CDIB *pDib = NULL;
			HINSTANCE hInst = ds.GetResourceHandle();
			if( hInst )
			{
				//ITRACE( _T("Loading an icon from resources") );

				MEMICONDIRENTRY *pIconList = new MEMICONDIRENTRY[ icon_header.idCount ];
				ZeroMemory( pIconList, sizeof( MEMICONDIRENTRY ) * icon_header.idCount );

				int c = 0;
				for( c = 0; c < icon_header.idCount; c++ )
				{
					if( !ds.ReadBytes( (LPBYTE)&pIconList[ c ], sizeof( MEMICONDIRENTRY ) ) )
					{
						//ITRACE( _T("Failed to read icon image directory\n") );
						delete pIconList;
						return false;
					}
				}

				WinHelper::CSize sizeThis( 0, 0 );
				for( c = 0; c < icon_header.idCount; c++ )
				{
					CResourceDataSource rs;
					if( rs.Open( hInst, MAKEINTRESOURCE( pIconList[ c ].nID ), RT_ICON ) )
					{
						pDib = ReadIconImageFromResource( rs, sizeThis );
					}
					if( sizeThis > size )
					{
						size = sizeThis;
					}

					CFrame *pFrame = new CFrame( pDib, 1000 );
					arrFrames.Add( pFrame );
				}

				delete pIconList;
			}
			else
			{
				ICONDIRENTRY *pIconList = new ICONDIRENTRY[ icon_header.idCount ];
				ZeroMemory( pIconList, sizeof( ICONDIRENTRY ) * icon_header.idCount );
				int c;
				for( c = 0; c < icon_header.idCount; c++ )
				{
					if( !ds.ReadBytes( (LPBYTE)&pIconList[ c ], sizeof( ICONDIRENTRY ) ) )
					{
						//ITRACE( _T("Failed to read icon image directory\n") );
						delete pIconList;
						return false;
					}
				}

	
				WinHelper::CSize sizeThis( 0, 0 );
				for( c = 0; c < icon_header.idCount; c++ )
				{
					ds.Reset();
					ds.SetRelativePos( pIconList[ c ].dwImageOffset );
					pDib = ReadIconImageFromResource( ds, sizeThis );
					if( sizeThis > size )
					{
						size = sizeThis;
					}

					CFrame *pFrame = new CFrame( pDib, 1000 );
					arrFrames.Add( pFrame );
				}

				delete pIconList;
			}


			if( arrFrames.GetSize() )
			{
				return true;
			}
		}
	}

	return false;
}

#endif	//	IMGLIB_ICO