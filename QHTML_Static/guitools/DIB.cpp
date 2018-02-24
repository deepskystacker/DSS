/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIB.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DIB.h"
#include "CompatibleDC.h"

using namespace GS;

static void InitialiseBitmapInfo( BITMAPINFO &bmi, int cx, int cy )
{
	memset( &bmi, 0, sizeof( bmi ) );

	bmi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = static_cast< LONG >( cx );
	bmi.bmiHeader.biHeight = -static_cast<LONG>( cy );
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = CDIB::g_knBPP;
#if ( _WIN32_WCE == 300 ||  _WIN32_WCE == 200 ||  _WIN32_WCE == 201 || _WIN32_WCE == 211 )
	bmi.bmiHeader.biCompression = BI_BITFIELDS;
#else
	bmi.bmiHeader.biCompression = BI_RGB;
#endif
	bmi.bmiHeader.biSizeImage = static_cast< DWORD >( cx * (( cy * CDIB::g_knBPP / 8 + 3 ) & ~ 3 ) ) ;
}

CDIB::CDIB( HDC hdc, int cx, int cy )
{
	InitialiseBitmapInfo( m_bi, cx, cy );

	m_hBitmap = CreateDIBSection( hdc, &m_bi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0 );
	ASSERT( m_hBitmap );

	Initialise();
}


CDIB::CDIB( HDC hdc, int x, int y, int cx, int cy )
{
	InitialiseBitmapInfo( m_bi, cx, cy );

	m_hBitmap = CreateDIBSection( hdc, &m_bi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0 );
	ASSERT( m_hBitmap );

	Initialise();

	//
	//	Copy the DC into the DIB.
	CCompatibleDC hdcMem( hdc );

	HGDIOBJ hOld = SelectObject( hdcMem, m_hBitmap );
	VERIFY( BitBlt( hdcMem, 0, 0, static_cast< int >( cx ), static_cast< int >( cy ), hdc, static_cast< int >( x ), static_cast< int >( y ), SRCCOPY ) );
	//TextOut( hdcMem, 0, 0, "hello", 5 );
	SelectObject( hdcMem, hOld );
}


CDIB::CDIB( HBITMAP hbm )
{
	BITMAP bm;
	GetObject( hbm, sizeof( bm ), &bm );

	InitialiseBitmapInfo( m_bi, bm.bmWidth, bm.bmHeight );

	HDC hdc = GetDC( NULL );
	ASSERT( hdc );
	m_hBitmap = CreateDIBSection( hdc, &m_bi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0 );

	ASSERT( m_hBitmap );
	Initialise();

	//
	//	Copy the DC into the DIB.
	{
		CCompatibleDC hdcMemDest( hdc );
		CCompatibleDC hdcMemSource( hdc );

		HGDIOBJ hOld = SelectObject( hdcMemDest, m_hBitmap );
		HGDIOBJ hOld2 = SelectObject( hdcMemSource, hbm );
		VERIFY( BitBlt( hdcMemDest, 0, 0, bm.bmWidth, bm.bmHeight, hdcMemSource, 0, 0, SRCCOPY ) );
		SelectObject( hdcMemDest, hOld );
		SelectObject( hdcMemSource, hOld2 );
	}

	VERIFY( ReleaseDC( NULL, hdc ) );
}


CDIB::CDIB( CDIB &dib, int x, int y, int cx, int cy )
{
	InitialiseBitmapInfo( m_bi, cx, cy );

	HDC hdc = GetDC( NULL );
	ASSERT( hdc );
	m_hBitmap = CreateDIBSection( hdc, &m_bi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0 );

	ASSERT( m_hBitmap );

	Initialise();

	if( dib.m_bHasAlpha )
	{
		SetHasAlphaChannel( true );
	}

	if( dib.m_bTransparentColorSet )
	{
		m_bTransparentColorSet = dib.m_bTransparentColorSet;
		m_crTransparent = dib.m_crTransparent;
	}

	//
	//	Copy the bits over...

	const int nLeft = max( x, 0 );
	const int nTop = max( y, 0 );
	const int nRight = min( x + cx, dib.GetWidth() );
	const int nBottom = min( y + cy, dib.GetHeight() );

	const CLineArray & arrDest = GetLineArray();
	const CLineArray & arrSrc = dib.GetLineArray();

	int nDestLine = 0;
	for( int nLine = nTop; nLine < nBottom; nLine++, nDestLine++ )
	{
		DIB_PIXEL *pDest = arrDest[ nDestLine ];
		const DIB_PIXEL *pSrc = arrSrc[ nLine ];
		pSrc += nLeft;

		for( int col = nLeft; col < nRight; col++, pDest++, pSrc++ )
		{
			*pDest = *pSrc;
		}
	}
}


CDIB::CDIB( HICON hIcon )
{
#if !defined (_WIN32_WCE)
	ICONINFO iconinfo;
	GetIconInfo( hIcon, &iconinfo );
	CDIB dibMask( iconinfo.hbmMask );

	BITMAP bm;
	GetObject( iconinfo.hbmColor, sizeof( bm ), &bm );

	InitialiseBitmapInfo( m_bi, bm.bmWidth, bm.bmHeight );


	HDC hdc = GetDC( NULL );
	ASSERT( hdc );
	m_hBitmap = CreateDIBSection( hdc, &m_bi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0 );

	ASSERT( m_hBitmap );
	Initialise();

	//
	//	Copy the DC into the DIB.
	{
		CCompatibleDC hdcMemDest( hdc );
		CCompatibleDC hdcMemSource( hdc );

		HGDIOBJ hOld = SelectObject( hdcMemDest, m_hBitmap );
		HGDIOBJ hOld2 = SelectObject( hdcMemSource, iconinfo.hbmColor );
		VERIFY( BitBlt( hdcMemDest, 0, 0, bm.bmWidth, bm.bmHeight, hdcMemSource, 0, 0, SRCCOPY ) );
		SelectObject( hdcMemDest, hOld );
		SelectObject( hdcMemSource, hOld2 );
	}


	VERIFY( ReleaseDC( NULL, hdc ) );

	const CLineArray & arrDest = GetLineArray();
	const int uSize = static_cast< int >( arrDest.GetSize() );
	const CLineArray & arrSrc = GetLineArray();
	for( register int u = 0; u < uSize; u++ )
	{
		DIB_PIXEL *pDest = arrDest[ u ];
		DIB_PIXEL *pSrc = arrSrc[ u ];
		for( int c = 0; c < GetWidth(); c++, pDest++, pSrc++ )
		{
      if( pSrc->rgbBlue != 0 )
			{
				pDest->rgbReserved = 255;
			}
		}
	}
	SetHasAlphaChannel( true );

#endif	//	#if !defined (_WIN32_WCE)
}


CDIB::~CDIB()
{
	VERIFY( DeleteObject( m_hBitmap ) );
}


void CDIB::Initialise()
//
//	Initialise various attributes of the DIB,mostly for performance reasons.
//	
//	Sets the elements of the line array to point to the start of each line in the
//	DIB data.
{
	m_size.cy = abs( m_bi.bmiHeader.biHeight );
	m_size.cx = m_bi.bmiHeader.biWidth;

	m_arrLine.SetSize( GetHeight() );

	for( int i= GetHeight() - 1; i >=  0; i--)
	{
		m_arrLine[i] = m_pBits + i * GetWidth();
	}

	m_crTransparent = 0;
	m_bTransparentColorSet = false;
	m_bHasAlpha = false;
}


bool CDIB::Draw( HDC hdc, int nX, int nY )
{
	if( m_bTransparentColorSet )
	{
		return DrawTransparent( hdc, nX, nY, m_crTransparent );
	}
	else if( m_bHasAlpha )
	{
		return DrawWithAlphaChannel( hdc, nX, nY );
	}	
	else if( BasicDraw( hdc, nX, nY ) )
	{
		return true;
	}

	return false;
}


bool CDIB::BasicDraw( HDC hdc, int nX, int nY )
{
#if !defined (_WIN32_WCE)
	return ::SetDIBitsToDevice( hdc, nX, nY, GetWidth(), GetHeight(), 0, 0, 0, GetHeight(), m_pBits, &m_bi, DIB_RGB_COLORS ) != GDI_ERROR; 
#else	//	#if !defined (_WIN32_WCE)
	CCompatibleDC dcSource( hdc );
	HGDIOBJ hOld = SelectObject( dcSource, GetBitmap() );
	BitBlt( hdc, nX, nY, GetWidth(), GetHeight(), dcSource, 0, 0, SRCCOPY );
	SelectObject( dcSource, hOld );

	return true;
#endif	//	#if !defined (_WIN32_WCE)

}


bool CDIB::BasicStretch( HDC hdc, int nX, int nY, int nWidth, int nHeight )
{
#if !defined (_WIN32_WCE)
	return ::StretchDIBits(hdc, nX, nY, nWidth, nHeight, 0, 0, GetWidth(), GetHeight(), m_pBits, &m_bi, DIB_RGB_COLORS, SRCCOPY ) != GDI_ERROR;
#else	//	#if !defined (_WIN32_WCE)
	CCompatibleDC dcSource( hdc );
	HGDIOBJ hOld = SelectObject( dcSource, GetBitmap() );
	StretchBlt( hdc, nX, nY, nWidth, nHeight, dcSource, 0, 0, GetWidth(), GetHeight(), SRCCOPY );
	SelectObject( dcSource, hOld );
	return true;
#endif	//	#if !defined (_WIN32_WCE)

}


void CDIB::StretchAlphaBlend( HDC hdc, int nX, int nY, int nCX, int nCY, unsigned char nAlpha )
{
#if !defined (_WIN32_WCE)
	CDIB dib( hdc, nCX, nCY );
	CCompatibleDC hdcMem( hdc );
	SetStretchBltMode( hdcMem, STRETCH_HALFTONE );

	HGDIOBJ hOld = SelectObject( hdcMem, dib.GetBitmap() );
	::StretchDIBits( hdcMem,
              0,
              0,
              nCX,
              nCY,
              0,
              0,
              GetWidth(),
              GetHeight(),
              m_pBits,
              (BITMAPINFO *) &m_bi,
              DIB_RGB_COLORS,
              SRCCOPY );

	SelectObject( hdcMem, hOld );

	dib.AlphaBlend( hdc, nX, nY, nAlpha );
#else
	::ExtTextOut(hdc, nX, nY, 0, NULL, _T("1"), 1, NULL );
#endif	//	#if !defined (_WIN32_WCE)
}


bool CDIB::StretchDrawTransparent( HDC hdc, int nX, int nY, int nCX, int nCY, COLORREF crTransparent )
{
#if !defined (_WIN32_WCE)
	CDIB dib( hdc, nCX, nCY );
	CCompatibleDC hdcMem( hdc );

	HGDIOBJ hOld = SelectObject( hdcMem, dib.GetBitmap() );
	SetStretchBltMode( hdcMem, STRETCH_HALFTONE );
	::StretchDIBits( hdcMem,
              0,                        // Destination x
              0,                        // Destination y
              nCX,	                // Destination width
              nCY,                // Destination height
              0,                        // Source x
              0,                        // Source y
              GetWidth(),									// Source width
              GetHeight(),	  						// Source height
              m_pBits,								// Pointer to bits
              (BITMAPINFO *) &m_bi,    // BITMAPINFO
              DIB_RGB_COLORS,           // Options
              SRCCOPY );

	SelectObject( hdcMem, hOld );

	return dib.DrawTransparent( hdc, nX, nY, crTransparent );
#else		//	#if !defined (_WIN32_WCE)
	::ExtTextOut(hdc, nX, nY, 0, NULL, _T("2"), 1, NULL );
	return false;
#endif	//	#if !defined (_WIN32_WCE)
}


void CDIB::AddImage( int nLeft, int nTop, CDIB *pDib )
{
	const CLineArray & arrThis = GetLineArray();
	const CLineArray & arr = pDib->GetLineArray();

	const int uDibHeight = pDib->GetHeight();
	const int uDibWidth = pDib->GetWidth();

	if( pDib->m_bTransparentColorSet )
	{
		BYTE bRed, bGreen, bBlue;
		bRed = GetRValue( pDib->m_crTransparent );
		bGreen = GetGValue( pDib->m_crTransparent );
		bBlue = GetBValue( pDib->m_crTransparent );

		for( int uRow = 0; uRow < uDibHeight; uRow++ )
		{
			const DIB_PIXEL *pSourceLine = (RGBQUAD *)arr[ uRow ];
			DIB_PIXEL *pDestLine = (RGBQUAD *)arrThis[ uRow + nTop ];
			for( int uCol = 0; uCol < uDibWidth; uCol++ )
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
		for( int uRow = 0; uRow < uDibHeight; uRow++ )
		{
			const DIB_PIXEL *pSourceLine = (RGBQUAD *)arr[ uRow ];
			DIB_PIXEL *pDestLine = (RGBQUAD *)arrThis[ uRow + nTop ];
			for( int uCol = 0; uCol < uDibWidth; uCol++ )
			{
				const DIB_PIXEL *pSource = pSourceLine + uCol;
				RGBQUAD *pDest = pDestLine + (uCol + nLeft);
				*pDest = *pSource;
			}
		}
	}
}


bool CDIB::Draw( HDC hdc, int nX, int nY, int nRight, int nBottom )
{
	const int nWidth = nRight - nX;
	const int nHeight = nBottom - nY;
	if( nWidth && nHeight )
	{

#if !defined (_WIN32_WCE)
		SetStretchBltMode( hdc, STRETCH_HALFTONE );
#endif	//	#if !defined (_WIN32_WCE)
		if( m_bTransparentColorSet )
		{
			if( nWidth != GetWidth() || nHeight != GetHeight() )
			{
				return StretchDrawTransparent( hdc, nX, nY, nWidth, nHeight, m_crTransparent );
			}
			return DrawTransparent( hdc, nX, nY, m_crTransparent );
		}
		else if( m_bHasAlpha )
		{
			if( nWidth != GetWidth() || nHeight != GetHeight() )
			{
				CDIB *pDib = CreateResized( nRight - nX, nBottom - nY );
				pDib->DrawWithAlphaChannel( hdc, nX, nY );
				delete pDib;
				return true;
			}
			return DrawWithAlphaChannel( hdc, nX, nY );
		}
		else
		{
			if( nWidth != GetWidth() || nHeight != GetHeight() )
			{
#if !defined (_WIN32_WCE)
				if( ::StretchDIBits(hdc, nX, nY, nWidth, nHeight, 0, 0, GetWidth(), GetHeight(), m_pBits, &m_bi, DIB_RGB_COLORS, SRCCOPY ) != GDI_ERROR )
				{
					return true;
				}
#else
				BasicStretch( hdc, nX, nY, nWidth, nHeight );
				return true;
#endif	//	#if !defined (_WIN32_WCE)
			}
			else
			{
				BasicDraw( hdc, nX, nY );
			}
		}

	}
	return false;
}

