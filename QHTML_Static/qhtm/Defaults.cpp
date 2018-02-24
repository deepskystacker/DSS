/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Defaults.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM_Types.h"
#include "Defaults.h"
#include "QHTM.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDefaults g_defaults;

CDefaults::CDefaults()
	: m_strFontName( _T("Times New Roman") )
	, m_nFontSize( 3 )

	// richg - 19990621 - The indentation level used for Lists and BlockQuote
	// This is in 1000ths of an inch, not dependent on zoom level
	, m_nIndentSize( 250 )
	, m_nIndentSpaceSize( 125 )
	, m_nDefinitionListDescriptionIndentSize( 500 )
	, m_nHorizontalFormControlGap( 50 )

	, m_cCharSet( DEFAULT_CHARSET )


	, m_crBackground( RGB( 255, 255, 255 ) )
	, m_crDefaultForeColour( RGB( 0, 0, 0 ) )

	, m_crLinkColour( RGB( 141, 7, 102 ) )
	, m_crLinkHoverColour( RGB( 29, 49, 149 ) )

	, m_nCellPadding( 1 )
	, m_nCellSpacing( 1 )
	, m_crBorderLight( ::GetSysColor( COLOR_3DHILIGHT ) )
	, m_crBorderDark( ::GetSysColor( COLOR_3DSHADOW ) )
	, m_nAlignedTableMargin( 1 )

	, m_strDefaultPreFontName( _T("Courier New") )

	, m_nParagraphLinesAbove( 1 )
	, m_nParagraphLinesBelow( 1 )

	, m_nImageMargin( 5 )

	, m_rcMargins( 5, 5, 5, 5 )

	, m_nZoomLevel( QHTM_ZOOM_DEFAULT )

	, m_funcQHTMImageCallback( NULL )
	, m_funcBitmapCallback( NULL )
	, m_funcQHTMResourceCallback( NULL )
	, m_funcQHTMFORMCallback( NULL )
	, m_funcQHTMEventCallback( NULL )
	, m_lParam( 0 )
{
#ifdef UNDER_CE
	SetFont( (HFONT)GetStockObject( SYSTEM_FONT ) );
#else
	SetFont( (HFONT)GetStockObject( DEFAULT_GUI_FONT ) );
#endif
}


CDefaults::~CDefaults()
{

}


void CDefaults::SetFont( LOGFONT &lf )
{
	m_strFontName = lf.lfFaceName;
	HDC hdc = GetDC( NULL );
	m_nFontSize = WinHelper::MulDiv( 72, lf.lfHeight, GetDeviceCaps( hdc, LOGPIXELSY) );
	if( m_nFontSize > 0 )
		m_nFontSize = -m_nFontSize;
	ReleaseDC( NULL, hdc );
}


void CDefaults::SetFont( HFONT hFont )
{
	LOGFONT lf;
	VAPI( GetObject( hFont, sizeof( lf ), &lf ) );
	SetFont( lf );
}


bool CDefaults::Set( LPQHTM_DEFAULTS lpDefaults )
{
	if( lpDefaults && lpDefaults->cbSize == sizeof( QHTM_DEFAULTS ) )
	{
		if( lpDefaults->uFlags & QHTM_DEF_IMAGE_CALLBACK )
		{
			m_funcQHTMImageCallback = lpDefaults->funcQHTMImageCallback;
		}

		if( lpDefaults->uFlags & QHTM_DEF_IMAGE_BITMAP_CALLBACK )
		{
			m_funcBitmapCallback = lpDefaults->funcBitmapCallback;
		}

		if( lpDefaults->uFlags & QHTM_DEF_IMAGE_RESOURCE_CALLBACK )
		{
			m_funcQHTMResourceCallback = lpDefaults->funcQHTMResourceCallback;
		}

		if( lpDefaults->uFlags & QHTM_DEF_FORM_CALLBACK )
		{
			m_funcQHTMFORMCallback = lpDefaults->funcQHTMFORMCallback;
		}

		if( lpDefaults->uFlags & QHTM_DEF_EVENT_CALLBACK )
		{
			m_funcQHTMEventCallback = lpDefaults->funcQHTMEventCallback;
		}

		return true;
	}
	return false;
}