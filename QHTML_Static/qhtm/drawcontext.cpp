/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
File:	drawcontext.cpp
Owner:	russf@gipsysoft.com
Purpose:	Drawing context.
					All drawing operations, brushes, colours, fonts etc. are managed
					here.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DrawContext.h"
#include "QHTM_Trace.h"
#include <reuse/MapIter.h>

#ifdef _UNICODE
	//
	//	The system measuring functions are too slow to use in ansi builds so we only use
	//	them in unicode builds.
	#define USE_WIN32_MEASURING
#endif	//	_UNICODE

//	Ugly one...
//128 64 32 16  8  4  2  1
//  0  0  0  1  0  0  0  1
//  0  0  1  0  0  0  1  0
//  0  1  0  0  0  1  0  0
//  1  0  0  0  1  0  0  0
//  0  0  0  1  0  0  0  1
//  0  0  1  0  0  0  1  0
//  0  1  0  0  0  1  0  0
//  1  0  0  0  1  0  0  0
//WORD HatchBits[8] = { 0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88 };

//	Nice one...
//128 64 32 16  8  4  2  1
//  1  0  1  0  1  0  1  0
//  0  1  0  1  0  1  0  1
static const WORD g_FocusBitmap[] = { 0xAA,0x55 };

/*
	The object cache is only used for display purposes, it is shared between all QHTM windows.
	
	During printing we throw it away for each print. We do this because fonts are different between printers
	and we might print on one printer and then to another later!
*/
class CObjectCache
{
public:
	CObjectCache()
		: m_mapFonts( 397 )
		{};

#ifdef _DEBUG
	~CObjectCache()
	{
		//	There shouldn't be anything left!
		ASSERT( m_arrObjToDelete.GetSize() == 0 );
	}
#endif	//	_DEBUG

	void Clear()
	{
		QHTM_TRACE( _T("There were %d GDI objects cached.\n"), m_arrObjToDelete.GetSize());
		const size_t uSize = m_arrObjToDelete.GetSize();
		for( size_t n = 0; n < uSize; n++ )
		{
			VAPI( ::DeleteObject( m_arrObjToDelete[ n ] ) );
		}
		m_arrObjToDelete.RemoveAll();

		QHTM_TRACE( _T("There were %d fonts cached.\n"), m_mapFonts.GetSize());
		for( Container::CMapIter<FontDef, CDrawContext::FontInfo*> itr( m_mapFonts ); !itr.EOL(); itr.Next() )
		{
			delete itr.GetValue();
		}
		m_mapFonts.RemoveAll();
		m_mapBrushes.RemoveAll();
		m_mapPens.RemoveAll();
	}
	HBRUSH GetFocusBrush()
	{
		if( !m_hbrFocus )
		{
			HBITMAP hbmpFocus = CreateBitmap( 8, countof( g_FocusBitmap ), 1, 1, g_FocusBitmap );
			m_hbrFocus = CreatePatternBrush( hbmpFocus );
			m_arrObjToDelete.Add( m_hbrFocus );
			VAPI( ::DeleteObject( hbmpFocus ) );
		}
		return m_hbrFocus;
	}

	//	This must be first
	Container::CArray<HGDIOBJ> m_arrObjToDelete;

	Container::CMap<FontDef, CDrawContext::FontInfo*> m_mapFonts;
	Container::CMap< COLORREF, HBRUSH> m_mapBrushes;
	Container::CMap< COLORREF, HPEN> m_mapPens;
private:
	HBRUSH m_hbrFocus;

	CObjectCache( const CObjectCache & );
	CObjectCache& operator = ( const CObjectCache & );
};


static CObjectCache m_ocPrint;
static CObjectCache m_ocDisplay;

//
//	We can force QHTM to clear all objects. This is done when the last QHTM window is closed.
void CDrawContext::ClearAllObjects()
{
	m_ocDisplay.Clear();
}

namespace Container {
	inline BOOL ElementsTheSame( FontDef n1, FontDef n2 )
	{
		//
		//	Don't optimise this unless you have empirical evidence to proove that you can
		//	increase it' speed.
		return n1.m_bItalic == n2.m_bItalic
			&& n1.m_nSizePixels == n2.m_nSizePixels
			&& n1.m_nWeight == n2.m_nWeight
			&& n1.m_bUnderline == n2.m_bUnderline
			&& n1.m_bStrike == n2.m_bStrike
			&& n1.m_cCharSet == n2.m_cCharSet
			&& !_tcscmp( n1.m_szFontName, n2.m_szFontName );
	}

	inline UINT HashIt( const FontDef &key )
	{
		return key.m_nHasValue;
	}
}


CDrawContext::CDrawContext( const WinHelper::CRect *prcClip, HDC hdc, bool bIsPrinting /*= false */ )
	: m_bWeCreatedDC( false )
	, m_hdc( hdc )
	, m_pFontInfo( NULL )
	, m_bBrushSelected( false )
	, m_bPenSelected( false )
	, m_cxDeviceScaleNumer( 1 )
	, m_cxDeviceScaleDenom( 1 )
	, m_cyDeviceScaleNumer( 1 )
	, m_cyDeviceScaleDenom( 1 )
	, m_crCurrentText( 0 )
	, m_bIsPrinting( bIsPrinting )
	, m_poc( NULL )
{
	
	if( prcClip )
		m_rcClip = *prcClip;
	else
		m_rcClip.Set(0,0,0,0);

	if( !m_hdc )
	{
		m_hdc = ::GetDC( NULL );
		m_bWeCreatedDC = true;
	}

	// Set the device scale if needed
	if( IsPrinting() )
	{
		m_poc = &m_ocPrint;
		HDC screenDC = ::GetDC(NULL);
		m_cxDeviceScaleNumer = ::GetDeviceCaps(m_hdc, LOGPIXELSX);
		m_cxDeviceScaleDenom = ::GetDeviceCaps(screenDC, LOGPIXELSX);
		m_cyDeviceScaleNumer = ::GetDeviceCaps(m_hdc, LOGPIXELSY);
		m_cyDeviceScaleDenom = ::GetDeviceCaps(screenDC, LOGPIXELSY);
		::ReleaseDC(NULL, screenDC);
	}
	else
	{
		m_poc = &m_ocDisplay;
	}

	VAPI( ::SaveDC( m_hdc ) );

	::SetBkMode( m_hdc, TRANSPARENT );
}


CDrawContext::~CDrawContext()
{
	if( m_hdc )
		VAPI( ::RestoreDC( m_hdc, -1 ) );

	if( IsPrinting() )
	{
		m_poc->Clear();
	}

	if( m_bWeCreatedDC )
	{
		::ReleaseDC( NULL, m_hdc );
		m_hdc = NULL;
	}

}


void CDrawContext::DrawFocus( const WinHelper::CRect &rc )
{ VAPI( FrameRect( m_hdc, &rc, m_poc->GetFocusBrush() ) ); }


void CDrawContext::DrawFocus( HRGN rgn )
{ FrameRgn( m_hdc, rgn, m_poc->GetFocusBrush(), 1, 1 ); }



HPEN CDrawContext::GetPen( COLORREF cr )
{
	HPEN *pPen = m_poc->m_mapPens.Lookup( cr );
	if( pPen )
	{
		return *pPen;
	}

	HPEN hPen = CreatePen( PS_SOLID, 1, 0x02000000 | cr );
	VAPI( hPen );
	m_poc->m_arrObjToDelete.Add( hPen );
	m_poc->m_mapPens.SetAt( cr, hPen );
	return hPen;
}


HBRUSH CDrawContext::GetBrush( COLORREF cr )
{
	HBRUSH *pBrush = m_poc->m_mapBrushes.Lookup( cr );
	if( pBrush )
	{
		return *pBrush;
	}

	HBRUSH hbr = ::CreateSolidBrush( 0x02000000 | cr );
	VAPI( hbr );
	m_poc->m_arrObjToDelete.Add( hbr );
	m_poc->m_mapBrushes.SetAt( cr, hbr );
	return hbr;
}

#ifdef USE_WIN32_MEASURING
int CDrawContext::GetTextExtent( LPCTSTR pcszText, int nTextLength )
{
	ASSERT_VALID_STR_LEN( pcszText, nTextLength );
	ASSERT( m_pFontInfo );

	SIZE sizeTemp;
	INT nMaxChars = 0;
	GetTextExtentExPoint( m_hdc, pcszText, nTextLength, 32768, &nMaxChars, NULL, &sizeTemp );

	return sizeTemp.cx;
}

bool CDrawContext::GetTextFitWithWordWrap( int nMaxWidth, LPCTSTR &pcszText, WinHelper::CSize &size, bool bStartOfLine ) const
{
	LPCTSTR pcszStart = pcszText;
	int nStringlen = static_cast< int >( _tcslen( pcszText ) );
	if( !nStringlen )
		return true;

	if( nMaxWidth < 1 )
	{
		nMaxWidth = 1;
	}

	SIZE sizeTemp;
	INT nMaxChars = 0;
	GetTextExtentExPoint( m_hdc, pcszText, nStringlen, nMaxWidth, &nMaxChars, NULL, &sizeTemp );
	size = sizeTemp;
	if( nStringlen != nMaxChars )
	{
		LPCTSTR pcszPos = pcszText + nMaxChars;
		while( pcszPos && !_istspace( *pcszPos ) )
		{
			pcszPos = _tcsdec( pcszStart, pcszPos );
			nMaxChars--;
		}

		if( !pcszPos )
		{
			pcszPos = pcszText;
		}

		if( bStartOfLine && pcszPos == pcszText )
		{
			nMaxChars ++;
			while( *pcszPos && !_istspace( *pcszPos ) )
			{
				nMaxChars++;
				pcszPos = _tcsinc( pcszPos );
			}
			nMaxWidth = 32768;
		}
		GetTextExtentExPoint( m_hdc, pcszText, nMaxChars, nMaxWidth, &nMaxChars, NULL, &sizeTemp );
		pcszText += nMaxChars;
		size = sizeTemp;
		return false;
	}
	pcszText += nMaxChars;

	return true;
}

#else	//	USE_WIN32_MEASURING

int CDrawContext::GetTextExtent( LPCTSTR pcszText, int nTextLength )
{
	ASSERT_VALID_STR_LEN( pcszText, nTextLength );
	ASSERT( m_pFontInfo );

	int nLength = 0;
	while( nTextLength )
	{
		nLength += m_pFontInfo->m_nWidths[ (TBYTE)*pcszText ];
		pcszText++;
		nTextLength--;
	}
	if( nLength )
	{
		pcszText--;
		nLength += m_pFontInfo->m_nOverhang[ (TBYTE)*pcszText ];
	}
	return nLength;
}

bool CDrawContext::GetTextFitWithWordWrap( int nMaxWidth, LPCTSTR &pcszText, WinHelper::CSize &size, bool bStartOfLine ) const
//	Get the text that fits within the length given, return the 'size' of the text object
//	needed.
//	Returns true if all of the text fits. If this is the case then pcszText points
//	to the next text needed to be rendered.
//  richg - 19990224 - This should break only at an acceptable boundary! If the text does not
//	fit into the space provided, continue anyway!
{
	size.cx = 0;
	size.cy = GetCurrentFontHeight();
	int nLength;
	const int *parrWidths = m_pFontInfo->m_nWidths;
	const int *parrOverhang = m_pFontInfo->m_nOverhang;
	LPCTSTR pcszPreviousSpace = pcszText;
	int nSpaceLength;
	nLength = nSpaceLength = 0;

	TBYTE ch;
	//	richg - 19990224 - The way this was originally written, it can break a word
	//	in the middle, if it is the first word on the line! Words should not break,
	//	and are able to exceed the length of the space provided.
	//	A better way is to locate one word at a time, and see it it continues to fit.
	//	Stop when a word will not fit. If it is the first word, get its length.
	//	This should NOT break on &nbsp;!
	//	Unfortunately, there is no way yet to know here if we are at the beginning of a line!
	
	//	If we are at the start of a line, look past the first word to get the
	//	length. There shouldn't be any leading spaces.
	TBYTE cLast = 0;
	if (bStartOfLine)
	{
		while ((ch = *pcszText) != 0 && ch != '\r' && ch != 32 )
		{
			nLength += parrWidths[ ch ];
			cLast = ch;
			pcszText++;
		}
	}

	//
	//	We calculate the maximum width that will fit, we take into account the overhang for this character too.
	while( (ch = *pcszText) != 0 && ch != '\r' && nLength + ( parrWidths[ ch ] + ( ( ch == 32 ) ? 0 : parrOverhang[ ch ] ) ) <= nMaxWidth )
	{
		if( ch == 32 )
		{
			pcszPreviousSpace = pcszText;
			nSpaceLength = nLength;
		}
		nLength += parrWidths[ ch ];
		cLast = ch;
		pcszText++;
	}

	//
	//	Spaces as the last character don't need overhang.
	if( cLast != 32 && cLast != 160 )
		nLength -= parrOverhang[ cLast ];

	//
	//	If we did not get to the ned of the text then use our *word* wrap stuff
	if( *pcszText )
	{
		if( *pcszText == '\r' )
		{
			size.cx = nLength;
		}
		else
		{
			if( nSpaceLength && *pcszText != ' ' )
			{
				pcszText = pcszPreviousSpace;
				//
				//	If we had to break on a space then we need to add the overhang onto the length
				if( cLast )
					nSpaceLength -= parrOverhang[ cLast ];
				size.cx = nSpaceLength;
			}
			else
			{
				size.cx = nLength;
			}
		}
		return false;
	}
	else
	{
		size.cx = nLength;
	}

	return true;
}
#endif	//	USE_WIN32_MEASURING



CDrawContext::FontInfo * CDrawContext::GetFont( const FontDef &fdef )
//
//	Create a font. If the font already exists in our map of fonts then we use that
//	if the font does not exist then we create it.
{
	FontInfo **ppInfo = m_poc->m_mapFonts.Lookup( fdef );
	if( ppInfo )
	{
		return (*ppInfo);
	}

	LOGFONT lf = {0};
	(void)_tcscpy( lf.lfFaceName, fdef.m_szFontName );
	lf.lfWeight = fdef.m_nWeight;
	lf.lfHeight = fdef.m_nSizePixels;
	lf.lfItalic = fdef.m_bItalic;
	lf.lfUnderline = fdef.m_bUnderline;
	lf.lfStrikeOut = fdef.m_bStrike;
	lf.lfQuality = DRAFT_QUALITY;
	lf.lfCharSet = fdef.m_cCharSet;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	if( fdef.m_bFixedPitchFont )
		lf.lfPitchAndFamily = FIXED_PITCH;

	FontInfo *pInfo = new FontInfo;
	pInfo->hFont = CreateFontIndirect( &lf );


	//
	//	Get and store the widths of the individual characters, these are used
	//	later when calculating the lengths of text in pixels.
	HGDIOBJ hFontOld = ::SelectObject( m_hdc, pInfo->hFont );

	//
	//	Store some extra bits and pieces.
	TEXTMETRIC tm;
	GetTextMetrics( m_hdc, &tm );
	pInfo->m_nBaseline = tm.tmAscent;
	pInfo->m_nLineSpace = tm.tmHeight + tm.tmExternalLeading;
	pInfo->m_nAverageWidth = tm.tmAveCharWidth;
	if( fdef.m_bUnderline )
		pInfo->m_nLineSpace++;

	ABC abcWids[ countof( pInfo->m_nWidths ) ];
	memset( abcWids, 0, sizeof( abcWids ) );
	if( GetCharABCWidths( m_hdc, 0, countof( pInfo->m_nWidths ) - 1, abcWids ) )
	{
		ABC *pABC = &abcWids[0];
		int *parrWidths = pInfo->m_nWidths;
		if( tm.tmOverhang )
		{
			int *parrOverhang = pInfo->m_nOverhang;

			for( UINT n = 0; n < countof( pInfo->m_nWidths ) ; n++, pABC++ )
			{
				*parrWidths++ = ( pABC->abcA + pABC->abcB + pABC->abcC );
				*parrOverhang++ = pABC->abcC;
			}
		}
		else
		{
			for( UINT n = 0; n < countof( pInfo->m_nWidths ) ; n++, pABC++ )
			{
				*parrWidths++ = ( pABC->abcA + pABC->abcB + pABC->abcC );
			}
			memset( pInfo->m_nOverhang, 0, sizeof( pInfo->m_nOverhang ) );
		}
	}
	else
	{
		int *parrWidths = pInfo->m_nWidths;
		if( !GetCharWidth32( m_hdc, 0, 255, parrWidths ) )
		{
			SIZE size;
			TCHAR ch = 0;
			for( UINT n = 0; n < 255; n++ )
			{
				ch = (TCHAR)n;
				GetTextExtentPoint32( m_hdc, &ch, 1, &size );
				*parrWidths++ = size.cx;
			}
		}
		memset( pInfo->m_nOverhang, 0, sizeof( pInfo->m_nOverhang ) );
	}

	::SelectObject( m_hdc, hFontOld );

	//
	//	And finally add the font to our cache.
	if( pInfo->hFont )
		m_poc->m_arrObjToDelete.Add( pInfo->hFont );
	m_poc->m_mapFonts.SetAt( fdef, pInfo );

	return pInfo;
}
