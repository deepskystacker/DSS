/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Dib.cpp
Owner:	russf@gipsysoft.com
Purpose:	Device indepenedent bitmap.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "CompatibleDC.h"
#include "DIB.h"

const size_t g_knBPP = 32;

CDib::CDib( HDC hdc, size_t cx, size_t cy )
{
	ZeroMemory( &m_bi, sizeof( m_bi ) );
	m_bi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	m_bi.bmiHeader.biWidth = static_cast< LONG >( cx );
	m_bi.bmiHeader.biHeight = static_cast< LONG >( cy );
	m_bi.bmiHeader.biPlanes = 1;
	m_bi.bmiHeader.biBitCount = g_knBPP;
	m_bi.bmiHeader.biCompression = BI_RGB;

	m_hBitmap = CreateDIBSection( hdc, &m_bi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0 );
	Initialise();
}


CDib::CDib( HDC hdc, int x, int y, size_t cx, size_t cy )
{
	ZeroMemory( &m_bi, sizeof( m_bi ) );
	m_bi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	m_bi.bmiHeader.biWidth = static_cast< LONG >( cx );
	m_bi.bmiHeader.biHeight = static_cast< LONG >( cy );
	m_bi.bmiHeader.biPlanes = 1;
	m_bi.bmiHeader.biBitCount = g_knBPP;
	m_bi.bmiHeader.biCompression = BI_RGB;

	m_hBitmap = CreateDIBSection( hdc, &m_bi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0 );

	Initialise();

	//
	//	Copy the DC into the DIB.
	CCompatibleDC hdcMem( hdc );

	HGDIOBJ hOld = SelectObject( hdcMem, m_hBitmap );
	BitBlt( hdcMem, 0, 0, static_cast< int >( cx ), static_cast< int >( cy ), hdc, x, y, SRCCOPY );
	SelectObject( hdcMem, hOld );
}


CDib::CDib( HBITMAP hbm )
{
	BITMAP bm;
	GetObject( hbm, sizeof( bm ), &bm );

	ZeroMemory( &m_bi, sizeof( m_bi ) );
	m_bi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	m_bi.bmiHeader.biWidth = bm.bmWidth;
	m_bi.bmiHeader.biHeight = bm.bmHeight;
	m_bi.bmiHeader.biPlanes = 1;
	m_bi.bmiHeader.biBitCount = g_knBPP;
	m_bi.bmiHeader.biCompression = BI_RGB;

	m_hBitmap = CreateDIBSection( NULL, &m_bi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0 );

	Initialise();

	//
	//	Copy the DC into the DIB.
	HDC hdc = GetDC( NULL );
	{
		CCompatibleDC hdcMemDest( hdc );
		CCompatibleDC hdcMemSource( hdc );

		HGDIOBJ hOld = SelectObject( hdcMemDest, m_hBitmap );
		HGDIOBJ hOld2 = SelectObject( hdcMemSource, hbm );
		BitBlt( hdcMemDest, 0, 0, bm.bmWidth, bm.bmHeight, hdcMemSource, 0, 0, SRCCOPY );
		SelectObject( hdcMemDest, hOld );
		SelectObject( hdcMemSource, hOld2 );
	}
	ReleaseDC( NULL, hdc );
}


CDib::~CDib()
{
	DeleteObject( m_hBitmap );
}


void CDib::SetTransparentColour( COLORREF cr )
{
	m_bTransparent = true;
	m_crTransparent = cr;	
}


bool CDib::GetTransparentColour( COLORREF &cr ) const
{
	cr = m_crTransparent;
	return m_bTransparent;
}


void CDib::Initialise()
//
//	Initialise various attributes of the DIB,mostly for performance reasons.
//	
//	Sets the elements of the line array to point to the start of each line in the
//	DIB data.
{
	m_nHeight = m_bi.bmiHeader.biHeight;
	m_nWidth = m_bi.bmiHeader.biWidth;
	m_bTransparent = false;
	m_bHasAlphaChannel = false;

	m_arrLine.SetSize( m_nHeight );

	//
	// The following loop used to read:
	//	 for (size_t i = m_nHeight - 1; i >=  0; i--)
	//
	// Unfortunately VS2017 15.9.3 compiled that as an infinite loop (arguably correctly)
	// which caused lots of C4072 "Code unreachable" warnings!
	//
	// I've now changed the code to iterate m_arrline in more normal way which should
	// prevent any further problems.
	//
	for (size_t i = 0; i < m_nHeight; i++)
	{
		m_arrLine[i] = m_pBits + i * m_nWidth;
	}
}


bool CDib::Draw( HDC hdc, int nX, int nY )
{
	if( m_bTransparent )
	{
		return DrawTransparent( hdc, nX, nY, m_crTransparent );
	}
	else if( m_bHasAlphaChannel )
	{
		return CDib::AlphaBlendChannel( hdc, nX, nY );
	}	
	else if( ::SetDIBitsToDevice( hdc, nX, nY, static_cast< DWORD >( m_nWidth ), static_cast< DWORD >( m_nHeight ), 0, 0, 0, static_cast< UINT >( m_nHeight ), m_pBits, &m_bi, DIB_RGB_COLORS ) != GDI_ERROR )
	{
		return true;
	}
	return false;
}


bool CDib::Draw( HDC hdc, int nX, int nY, int nRight, int nBottom )
{
	const size_t nWidth = nRight - nX;
	const size_t nHeight = nBottom - nY;

	SetStretchBltMode( hdc, STRETCH_HALFTONE );
	if( m_bTransparent )
	{
		if( nWidth != m_nWidth && nHeight != m_nHeight )
		{
			return StretchDrawTransparent( hdc, nX, nY, nWidth, nHeight, m_crTransparent );
		}
		return DrawTransparent( hdc, nX, nY, m_crTransparent );
	}
	else if( m_bHasAlphaChannel )
	{
		return CDib::AlphaBlendChannel( hdc, nX, nY );
	}	
	else if( ::StretchDIBits(hdc, nX, nY, static_cast< int >( nWidth ), static_cast< int >( nHeight ), 0, 0, static_cast< DWORD >( m_nWidth ), static_cast< DWORD >( m_nHeight ), m_pBits, &m_bi, DIB_RGB_COLORS, SRCCOPY ) != GDI_ERROR )
	{
		return true;
	}
	return false;
}


bool CDib::DrawTransparent( HDC hdc, int nX, int nY, COLORREF crTransparent )
{
	//
	//	Grab a copy of what's there already...
	CDib dib( hdc, nX, nY, m_nWidth, m_nHeight );

	//
	//	Now copy our stuff into the copy of the current screen...

	const CLineArray & arrScreen = dib.GetLineArray();
	const CLineArray & arrThis = GetLineArray();

	BYTE bRed, bGreen, bBlue;
	bRed = GetRValue( crTransparent );
	bGreen = GetGValue( crTransparent );
	bBlue = GetBValue( crTransparent );

	for( size_t u = 0; u < arrScreen.GetSize(); u++ )
	{
		DIB_PIXEL *pLine = arrScreen[ u ];
		DIB_PIXEL *pThis = arrThis[ u ];
		for( size_t c = 0; c < m_nWidth; c++, pLine++, pThis++ )
		{
			if( pThis->rgbRed != bRed || pThis->rgbGreen != bGreen || pThis->rgbBlue != bBlue )
			{
				pLine->rgbRed = pThis->rgbRed;
				pLine->rgbGreen = pThis->rgbGreen;
				pLine->rgbBlue = pThis->rgbBlue;
			}
		}
	}
	return dib.Draw( hdc, nX, nY );
}


bool CDib::AlphaBlend( HDC hdc, int nX, int nY, unsigned char nAlpha )
{
	//
	//	Grab a copy of what's there already...
	CDib dib( hdc, nX, nY, m_nWidth, m_nHeight );

	//
	//	Now copy our stuff into the copy of the current screen...
	const CLineArray & arrScreen = dib.GetLineArray();
	const CLineArray & arrThis = GetLineArray();

	unsigned char nNegativeAplha;
	for( size_t u = 0; u < arrScreen.GetSize(); u++ )
	{
		DIB_PIXEL *pLine = arrScreen[ u ];
		DIB_PIXEL *pThis = arrThis[ u ];
		for( size_t c = 0; c < m_nWidth; c++, pLine++, pThis++ )
		{
      nNegativeAplha = (unsigned char)( 255 - nAlpha );
      pLine->rgbRed = (unsigned char)( ( pLine->rgbRed * nNegativeAplha + pThis->rgbRed * nAlpha ) >>8 );
      pLine->rgbGreen = (unsigned char)( ( pLine->rgbGreen * nNegativeAplha + pThis->rgbGreen * nAlpha ) >> 8 );
      pLine->rgbBlue = (unsigned char)( ( pLine->rgbBlue * nNegativeAplha + pThis->rgbBlue * nAlpha ) >> 8 );
		}
	}
	return dib.Draw( hdc, nX, nY );
}


bool CDib::AlphaBlendChannel( HDC hdc, int nX, int nY )
{
	//
	//	Grab a copy of what's there already...
	CDib dib( hdc, nX, nY, m_nWidth, m_nHeight );

	//
	//	Now copy our stuff into the copy of the current screen...
	const CLineArray & arrScreen = dib.GetLineArray();
	const CLineArray & arrThis = GetLineArray();

	unsigned char nNegativeAplha;
	for( size_t u = 0; u < arrScreen.GetSize(); u++ )
	{
		DIB_PIXEL *pLine = arrScreen[ u ];
		DIB_PIXEL *pThis = arrThis[ u ];
		for( size_t c = 0; c < m_nWidth; c++, pLine++, pThis++ )
		{
			unsigned char nAlpha = pThis->rgbReserved;
			if( nAlpha )
			{
				nNegativeAplha = (unsigned char)( 255 - nAlpha );
				pLine->rgbRed = (unsigned char)( ( pLine->rgbRed * nNegativeAplha + pThis->rgbRed * nAlpha ) >>8 );
				pLine->rgbGreen = (unsigned char)( ( pLine->rgbGreen * nNegativeAplha + pThis->rgbGreen * nAlpha ) >> 8 );
				pLine->rgbBlue = (unsigned char)( ( pLine->rgbBlue * nNegativeAplha + pThis->rgbBlue * nAlpha ) >> 8 );
			}
		}
	}
	return dib.Draw( hdc, nX, nY );
}


bool CDib::StretchAlphaBlend( HDC hdc, int nX, int nY, size_t nCX, size_t nCY, unsigned char nAlpha )
{
	CDib dib( hdc, nCX, nCY );
	CCompatibleDC hdcMem( hdc );

	HGDIOBJ hOld = SelectObject( hdcMem, dib.GetBitmap() );
	::StretchDIBits( hdcMem,
              0,
              0,
              static_cast< int >( nCX ),
              static_cast< int >( nCY ),
              0,
              0,
              static_cast< int >( m_nWidth ),
              static_cast< int >( m_nHeight ),
              m_pBits,
              &m_bi,
              DIB_RGB_COLORS,
              SRCCOPY );

	SelectObject( hdcMem, hOld );

	return dib.AlphaBlend( hdc, nX, nY, nAlpha );
}


bool CDib::StretchDrawTransparent( HDC hdc, int nX, int nY, size_t nCX, size_t nCY, COLORREF crTransparent )
{
	CDib dib( hdc, nCX, nCY );
	CCompatibleDC hdcMem( hdc );

	HGDIOBJ hOld = SelectObject( hdcMem, dib.GetBitmap() );
	::StretchDIBits( hdcMem,
              0,													// Destination x
              0,													// Destination y
              static_cast< int >( nCX ),	// Destination width
              static_cast< int >( nCY ),	// Destination height
              0,													// Source x
              0,													// Source y
              static_cast< int >( m_nWidth ),		// Source width
              static_cast< int >( m_nHeight ),	// Source height
              m_pBits,										// Pointer to bits
              &m_bi,    // BITMAPINFO
              DIB_RGB_COLORS,           // Options
              SRCCOPY );

	SelectObject( hdcMem, hOld );

	return dib.DrawTransparent( hdc, nX, nY, crTransparent );
}


CDib *CDib::CreateCopy( size_t nCX, size_t nCY ) const
{
	HDC hdc = GetDC( NULL );

	CDib *pdib = new CDib( hdc, nCX, nCY );
	CCompatibleDC hdcMem( hdc );

	HGDIOBJ hOld = SelectObject( hdcMem, pdib->GetBitmap() );

	::StretchDIBits( hdcMem,
              0,
              0,
              static_cast< int >( nCX ),
              static_cast< int >( nCY ),
              0,
              0,
              static_cast< int >( m_nWidth ),
              static_cast< int >( m_nHeight ),
              m_pBits,
              &m_bi,
              DIB_RGB_COLORS,
              SRCCOPY );

	SelectObject( hdcMem, hOld );

	ReleaseDC( NULL, hdc );
	return pdib;
}

void CDib::AddImage( size_t nLeft, size_t nTop, CDib *pDib )
{
	const CLineArray & arrThis = GetLineArray();
	const CLineArray & arr = pDib->GetLineArray();

	const size_t uDibHeight = pDib->GetHeight();
	const size_t uDibWidth = pDib->GetWidth();

	if( pDib->m_bTransparent )
	{
		BYTE bRed, bGreen, bBlue;
		bRed = GetRValue( pDib->m_crTransparent );
		bGreen = GetGValue( pDib->m_crTransparent );
		bBlue = GetBValue( pDib->m_crTransparent );

		for( size_t uRow = 0; uRow < uDibHeight; uRow++ )
		{
			const DIB_PIXEL *pSourceLine = (RGBQUAD *)arr[ uRow ];
			DIB_PIXEL *pDestLine = (RGBQUAD *)arrThis[ uRow + nTop ];
			for( size_t uCol = 0; uCol < uDibWidth; uCol++ )
			{
				const DIB_PIXEL *pSource = pSourceLine + uCol;
				RGBQUAD *pDest = pDestLine + (uCol + nLeft);
				if( pSource->rgbRed != bRed || pSource->rgbGreen != bGreen || pSource->rgbBlue != bBlue )
				{
					*pDest = *pSource;
				}
			}
		}
	}
	else
	{
		for( size_t uRow = 0; uRow < uDibHeight; uRow++ )
		{
			const DIB_PIXEL *pSourceLine = (RGBQUAD *)arr[ uRow ];
			DIB_PIXEL *pDestLine = (RGBQUAD *)arrThis[ uRow + nTop ];
			for( size_t uCol = 0; uCol < uDibWidth; uCol++ )
			{
				const DIB_PIXEL *pSource = pSourceLine + uCol;
				RGBQUAD *pDest = pDestLine + (uCol + nLeft);
				*pDest = *pSource;
			}
		}
	}
}


void CDib::SetHasAlphaChannel( bool bHasAlphaChannel )
{
	m_bHasAlphaChannel = bHasAlphaChannel;
}