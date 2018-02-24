/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLImageSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Image section.
					NOTE: Currently ignores the width and height
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include "HTMLImageSection.h"
#include "Utils.h"
#include "Defaults.h"

extern int GetFontSizeAsPixels( HDC hdc, int nSize, UINT nZoomLevel );

CHTMLImageSection::CHTMLImageSection( CHTMLSection *pParent, CQHTMImageABC *pImage, int nBorder )
	: CHTMLSectionABC( pParent )
	, m_pImage( pImage )
	,	m_nBorder( nBorder )
	, m_nTimerID( knNullTimerId )
	, m_nFrame( 0 )
	, m_uLoopCount( 0 )
{
	SetParent( pParent );

	SetKeep( true );

#ifdef QHTM_BUILD_INTERNAL_IMAGING
	ASSERT( m_pImage );
	if( m_pImage->GetFrameCount() && m_pImage->GetFrameTime( 0 ) )
	{
		m_nTimerID = RegisterTimerEvent( this, m_pImage->GetFrameTime( 0 ) );
	}
#endif	//	QHTM_BUILD_INTERNAL_IMAGING
}

CHTMLImageSection::~CHTMLImageSection()
{
	if( m_nTimerID !=  knNullTimerId )
	{
		UnregisterTimerEvent( m_nTimerID );
		m_nTimerID = knNullTimerId;
	}
}


void CHTMLImageSection::OnDraw( GS::CDrawContext &dc )
{
	if( m_pImage )
	{
		if( m_nBorder )
		{
			COLORREF clr = RGB( 0, 0, 0 );
			
			//
			//	Top border
			WinHelper::CRect rc( left, top, right, top + m_nBorder );
			dc.FillRect( rc, clr );

			//
			//	Bottom border
			rc.bottom = bottom;
			rc.top = bottom - m_nBorder;
			dc.FillRect( rc, clr );

			//
			//	Left border
			rc.top = top;
			rc.right = rc.left + m_nBorder;
			dc.FillRect( rc, clr );

			//
			//	Right border
			rc.right = right;
			rc.left = right - m_nBorder;
			dc.FillRect( rc, clr );

			(void)m_pImage->StretchFrame( m_nFrame, dc.GetSafeHdc(), left + m_nBorder, top + m_nBorder, right - m_nBorder, bottom - m_nBorder );
		}
		else
		{
			(void)m_pImage->StretchFrame( m_nFrame, dc.GetSafeHdc(), left, top, right, bottom );
		}
	}
	else
	{
		static TCHAR sz[] = _T("Image missing");

		dc.SelectFont( g_defaults.m_strFontName, GetFontSizeAsPixels( dc.GetSafeHdc(), g_defaults.m_nFontSize, 0 ), FW_NORMAL, false, false, false, g_defaults.m_cCharSet );

		dc.DrawText( left, top, sz, countof( sz ) - 1, 0 );
		dc.Rectangle( *this, CHTMLSectionABC::LinkHoverColour() );
	}

	if( IsMouseInSection() && IsLink() )
	{
		dc.Rectangle( *this, CHTMLSectionABC::LinkHoverColour() );
	}

	CHTMLSectionABC::OnDraw( dc );
}


void CHTMLImageSection::OnTimer( int nTimerID )
{
	if( nTimerID == m_nTimerID )
	{
		m_nFrame++;
		if( m_nFrame == m_pImage->GetFrameCount() )
		{
			m_nFrame = 0;
			m_uLoopCount++;
		}

		UnregisterTimerEvent( m_nTimerID );
		m_nTimerID = knNullTimerId;
		if( m_pImage->GetImageLoopCount() == 0 || m_uLoopCount < m_pImage->GetImageLoopCount() )
		{
			m_nTimerID = RegisterTimerEvent( this, m_pImage->GetFrameTime( m_nFrame ) );
			ForceRedraw();
		}
		else
		{
			m_nFrame = 0;
			ForceRedraw();
		}

	}
	else
	{
		CHTMLSectionABC::OnTimer( nTimerID );
	}
}

