/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DibLoader.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "ImgLibInt.h"
#include "DibLoader.h"
#include "Config.h"
#include <guitools/CompatibleDC.h>

inline static long GetBytesPerLine( long width, long bpp )
//
//	returns the number of bytes required for a single line.
{
	return ((((width * bpp) + 31) & ~31) >> 3);
}


inline static long GetBytesMemNeeded( long width, long height, long bpp )
//
//	Return the number of bytes need for the total image
{
  return GetBytesPerLine( width, bpp ) * height;
}


static LPBITMAPINFO DibCreate( int bits, int dx, int dy, int compression = BI_RGB )
//
//	Given a bit depth and size create a BITMAPINFO structure complete with color table
//	Color table is only created and not initialised.
//	Also, the color tabel exists at the end of the structure.
//	Corrected to handle 16 and 32 bit images
{

	int nColorCount = 0;
	if( bits <= 8 )
		nColorCount = 1 << bits;

  DWORD dwSizeImage;
  dwSizeImage = dy * (( dx * bits / 8 + 3 ) & ~ 3 );

  // Calculate size of mask table, if needed
  DWORD dwMaskSpace = (compression == BI_BITFIELDS ? 3 * sizeof(DWORD) : 0);
  
  LPBITMAPINFO lpbi = (BITMAPINFO*)new char[ sizeof( BITMAPINFO )+ dwSizeImage + ( nColorCount * sizeof( RGBQUAD ) ) + dwMaskSpace ];

  if (lpbi == NULL)
      return NULL;

  lpbi->bmiHeader.biSize            = sizeof(BITMAPINFOHEADER) ;
  lpbi->bmiHeader.biWidth           = dx;
  lpbi->bmiHeader.biHeight          = dy;
  lpbi->bmiHeader.biPlanes          = 1;
  lpbi->bmiHeader.biBitCount        = static_cast<WORD>( bits );
  lpbi->bmiHeader.biCompression     = compression ;
  lpbi->bmiHeader.biSizeImage       = dwSizeImage;
  lpbi->bmiHeader.biXPelsPerMeter   = 0 ;
  lpbi->bmiHeader.biYPelsPerMeter   = 0 ;
  lpbi->bmiHeader.biClrUsed         = 0 ;
  lpbi->bmiHeader.biClrImportant    = 0 ;
  return lpbi;
}

CDibLoader::CDibLoader( int nWidth, int nHeight, int nBPP )
	: m_nWidth( nWidth )
	, m_nHeight( nHeight )
	, m_nBPP( nBPP )
	, m_pBMI( DibCreate( nBPP, nWidth, nHeight ) )
	, m_pClrTab( NULL )
	, m_pClrMasks( NULL )
	, m_pBits( NULL )
{
	if( nBPP <= 8 )
	{
    m_pClrTab = (RGBQUAD *)(m_pBMI->bmiColors);
	}

	m_dwImageSize = GetBytesMemNeeded( m_nWidth, m_nHeight, m_nBPP );
	m_pBits = new BYTE [ m_dwImageSize ];
}

// Create a Dib directly from a BITMAPINFO structure. 
// This makes a copy of the passed structure
// This is generally used when loading BMP/RLE files
CDibLoader::CDibLoader( LPBITMAPINFOHEADER lpbhi)
	: m_nWidth( 0 )
	, m_nHeight( 0 )
	, m_nBPP( 0 )
	, m_pBMI( NULL )
	, m_pClrTab( NULL )
	, m_pClrMasks( NULL )
	, m_pBits( NULL )
{
   DWORD dwMaskSpace = lpbhi->biCompression == BI_BITFIELDS ? 3 * sizeof(DWORD) : 0;

   int nColors = lpbhi->biClrUsed;
   if( !nColors )
	   nColors = (lpbhi->biBitCount <= 8) ? ( 1 << lpbhi->biBitCount ) : 0;

   m_dwImageSize = lpbhi->biSizeImage;

   // Don't recalculate for RLE compressed images!
   if (lpbhi->biCompression == BI_RGB || lpbhi->biCompression == BI_BITFIELDS)
      m_dwImageSize = GetBytesMemNeeded( lpbhi->biWidth, lpbhi->biHeight, lpbhi->biBitCount );
	
   // Allocate the space, and copy the data, fill in the blanks...
   m_pBMI = DibCreate( lpbhi->biBitCount, lpbhi->biWidth, lpbhi->biHeight, lpbhi->biCompression);

   // Replace fields that may have us fooled, such as RLE compression
   m_pBMI->bmiHeader.biSizeImage = m_dwImageSize;
   m_pBMI->bmiHeader.biClrUsed = nColors;
   
   m_nWidth = lpbhi->biWidth;
   m_nHeight = lpbhi->biHeight;
   m_nBPP = lpbhi->biBitCount;

   // Locate the color table
   if (m_nBPP <= 8)
	   m_pClrTab = (RGBQUAD*)((BYTE*)m_pBMI->bmiColors + dwMaskSpace);

   // Locate the mask table
   if (m_pBMI->bmiHeader.biCompression == BI_BITFIELDS)
	   m_pClrMasks = (DWORD*)(m_pBMI->bmiColors);
   
   // Since the bytes needed are not necessarily predicted by the
   // image size, use what we found in the header
   m_pBits = new BYTE [ m_dwImageSize ];
}


CDibLoader::~CDibLoader()
{
	delete[] m_pBMI;
	delete[] m_pBits;
}


void CDibLoader::GetLineArray( CLineArray &arr )
//
//	Sets the elements of the line array to point to the start of each line in the
//	DIB data.
//
//	This will NOT work for RLE encoded images.
{
	arr.SetSize( m_nHeight );

	const long bpl = GetBytesPerLine( m_nWidth, m_nBPP );
	for( int i= m_nHeight - 1; i >=  0; i--)
	{
		arr[i] = GetBits() + ( ((m_nHeight - 1) - i ) * bpl );
	}
}

void CDibLoader::GetLineArray( CConstLineArray &arr ) const
//
//	Sets the elements of the line array to point to the start of each line in the
//	DIB data.
//
//	This will NOT work for RLE encoded images.
{
	arr.SetSize( m_nHeight );

	const long bpl = GetBytesPerLine( m_nWidth, m_nBPP );
	for( int i= m_nHeight - 1; i >=  0; i--)
	{
		arr[i] = GetBits() + ( ((m_nHeight - 1) - i ) * bpl );
	}
}



/////////////////////////////////////////////////////////////////////
// Static functions


bool CDibLoader::Draw( HDC hdc, int x, int y ) const
{
#if !defined (_WIN32_WCE)
	if( ::StretchDIBits(hdc,
              x,                        // Destination x
              y,                        // Destination y
              m_nWidth,	                // Destination width
              m_nHeight,                // Destination height
              0,                        // Source x
              0,                        // Source y
              m_nWidth,									// Source width
              m_nHeight,	  						// Source height
              GetBits(),								// Pointer to bits
              (BITMAPINFO *) m_pBMI,    // BITMAPINFO
              DIB_RGB_COLORS,           // Options
              SRCCOPY ) == GDI_ERROR )								// Raster operator code
		return false;
	return true;
#else	//	#if !defined (_WIN32_WCE)
	LPVOID pData = NULL;
	HBITMAP hbmp = ::CreateDIBSection( hdc, (BITMAPINFO *)m_pBMI, DIB_RGB_COLORS, &pData, NULL, 0 );
	memcpy( pData, GetBits(), m_dwImageSize );
	GS::CCompatibleDC dcSource( hdc );
	HGDIOBJ hOld = SelectObject( dcSource, hbmp );
	BitBlt( hdc, x, y, m_nWidth, m_nHeight, dcSource, 0, 0, SRCCOPY );
	SelectObject( dcSource, hOld );
	DeleteObject( hbmp );

	return true;
#endif	//	#if !defined (_WIN32_WCE)

}
