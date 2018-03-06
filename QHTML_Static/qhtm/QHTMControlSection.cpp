/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	QHTMControlSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	Main control section.

	This is basically a wrapper for the CHTMLSection.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include <stdio.h>
#include <SHELLAPI.H>	//	For ShellExecute
#include <stdlib.h>
#include "ResourceLoader.h"
#include "QHTMControlSection.h"
#include "Utils.h"
#include "TipWindow.h"
#include <reuse/MapIter.h>
#include "UnicodeHelpers.h"

//	: 'this' : used in base member initializer list
//	Ignored because we want to pass the 'this' pointer...
#pragma warning( disable: 4355 )

extern bool GotoURL( LPCTSTR url, int showcmd );

CSectionABC *g_pSectHighlight = NULL;

#ifdef UNDER_CE
static BOOL ShowScrollBar( HWND hwnd, int nBar, BOOL bShow )
{
  if( bShow )
  {
   SCROLLINFO si = { sizeof(si), 0};

   si.nMax = 2;
   si.nPage = 1;
   si.nPos = 1;
   return ::SetScrollInfo( hwnd, nBar, &si, TRUE );
  }
  else
  {
   SCROLLINFO si = { sizeof(si), SIF_RANGE, 0, 0};
   return ::SetScrollInfo( hwnd, nBar, &si, TRUE );
  }
}
#endif	//	



extern void CancelHighlight()
{
	if( g_pSectHighlight )
	{
		g_pSectHighlight->OnMouseLeave();
		g_pSectHighlight = NULL;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQHTMControlSection::CQHTMControlSection( HWND hwnd )
	: m_defaults( g_defaults )
	, m_htmlSection( this, &m_defaults )
	, m_bEnableTooltips( true )
	, m_bLayingOut( false )
	, m_bUseColorStatic( false )
	, m_bEnableScrollbars( true )
	,	m_hwnd( NULL )
	, m_nMouseMoveTimerID( knNullTimerId )
	,	m_pCaptureSection( NULL )
	, m_nNextTimerID( 0 )
	, m_bAlwaysShowTips( false )
{
	m_hwnd = hwnd;
	m_bHadTABSTOP =  GetWindowLong( m_hwnd, GWL_STYLE ) & WS_TABSTOP ? true : false;
}


CQHTMControlSection::~CQHTMControlSection()
{
	m_htmlSection.DestroyDocument();

//#ifdef _DEBUG
//	for( Container::CMapIter<int, CSectionABC *> itr( m_mapTimerEvents ); !itr.EOL(); itr.Next() )
//	{
//		TRACE( _T("Timer ID %d remaining after close\n"), itr.GetKey() );
//	}	
//	//	All timers must have been stopped
//	ASSERT( m_mapTimerEvents.GetSize() == 0 );
//#endif	//	_DEBUG
}


void CQHTMControlSection::OnLayout( const WinHelper::CRect &rc )
{
	CParentSection::OnLayout( rc );

	LayoutHTML();
}


void CQHTMControlSection::SetText( LPCTSTR pcszText )
{
	if( pcszText == NULL )
	{
		pcszText = _T("");
	}
	m_defaults.m_crBackground = ::GetSysColor( COLOR_3DFACE );
	m_htmlSection.SetHTML( pcszText, lstrlen( pcszText ), NULL );

	OnNewHTMLTextSet();
}


void CQHTMControlSection::OnNewHTMLTextSet()
{
	ResetScrollPos();
	LayoutHTML();
	ForceRedraw( *this );

	LONG lStyle = GetWindowLong( m_hwnd, GWL_STYLE );
	if( m_bHadTABSTOP )
	{
		if( HasControls() )
		{
			SetWindowLong( m_hwnd, GWL_STYLE, lStyle | WS_TABSTOP );
		}
		else
		{
			SetWindowLong( m_hwnd, GWL_STYLE, lStyle & ~WS_TABSTOP );
		}
	}
}


void CQHTMControlSection::OnExecuteHyperlink( CHTMLSectionLink *pHtmlLink )
{
	HWND hwndParent = ::GetParent( m_hwnd );
	if( hwndParent )
	{
		if( m_defaults.m_funcQHTMResourceCallback )
		{
			HGLOBAL h = m_defaults.m_funcQHTMResourceCallback( pHtmlLink->m_strLinkTarget, m_defaults.m_lParam );
			if( h )
			{
				DWORD dw = static_cast< DWORD > ( GlobalSize( h ) );
				LPCTSTR pcszText = reinterpret_cast<LPCTSTR>( GlobalLock( h ) );
				if( pcszText )
				{
					m_defaults.m_crBackground = ::GetSysColor( COLOR_3DFACE );
					m_htmlSection.SetHTML( pcszText, dw, NULL );
					OnNewHTMLTextSet();

					GlobalUnlock( h );
					GlobalFree( h );
					return;
				}
			}
		}

#ifndef UNDER_CE
		LRESULT lrFormat = ::SendMessage( hwndParent, WM_NOTIFYFORMAT, (LPARAM)GetHwnd(), NF_QUERY );
#else
		#define NFR_UNICODE	1
		LRESULT lrFormat = NFR_UNICODE;
#endif	//	UNDER_CE

		NMQHTM nm = { { m_hwnd,	(UINT_PTR)GetWindowLongPtr( m_hwnd, GWL_ID ), QHTMN_HYPERLINK } };
		nm.resReturnValue = TRUE;


#ifdef _UNICODE
		if( lrFormat == NFR_UNICODE )
		{
			nm.pcszLinkText = pHtmlLink->m_strLinkTarget;
			nm.pcszLinkID = pHtmlLink->m_strLinkID;
			::SendMessage( hwndParent, WM_NOTIFY, (WPARAM)nm.hdr.idFrom, (LPARAM)&nm );
		}
		else
		{
			//	unicode to ansi
			CUnicodeToAnsi cstrLinkTarget( pHtmlLink->m_strLinkTarget, pHtmlLink->m_strLinkTarget.GetLength() );
			nm.pcszLinkText = cstrLinkTarget.AsUnicode();

			CUnicodeToAnsi cstrLinkID( pHtmlLink->m_strLinkID, pHtmlLink->m_strLinkID.GetLength() );
			nm.pcszLinkID = cstrLinkID.AsUnicode();
			::SendMessage( hwndParent, WM_NOTIFY, (WPARAM)nm.hdr.idFrom, (LPARAM)&nm );
		}
#else _UNICODE
		if( lrFormat == NFR_ANSI )
		{
			nm.pcszLinkText = pHtmlLink->m_strLinkTarget;
			nm.pcszLinkID = pHtmlLink->m_strLinkID;
			::SendMessage( hwndParent, WM_NOTIFY, (WPARAM)nm.hdr.idFrom, (LPARAM)&nm );
		}
		else
		{
			//
			//	Convert ansi to unicode...
			CAnsiToUnicode cstrLinkTarget( pHtmlLink->m_strLinkTarget, pHtmlLink->m_strLinkTarget.GetLength() );
			nm.pcszLinkText = cstrLinkTarget.AsAnsi();

			CAnsiToUnicode cstrLinkID( pHtmlLink->m_strLinkID, pHtmlLink->m_strLinkID.GetLength() );
			nm.pcszLinkID = cstrLinkID.AsAnsi();
			::SendMessage( hwndParent, WM_NOTIFY, (WPARAM)nm.hdr.idFrom, (LPARAM)&nm );

		}

#endif _UNICODE
		if( nm.resReturnValue )
		{
			GotoURL( pHtmlLink->m_strLinkTarget, SW_SHOW );
		}
	}
	else
	{
		GotoURL( pHtmlLink->m_strLinkTarget, SW_SHOW );
	}
}


bool CQHTMControlSection::LoadFromResource( HINSTANCE hInst, LPCTSTR pcszName )
{
	m_defaults.m_crBackground = ::GetSysColor( COLOR_3DFACE );
	if( m_htmlSection.SetHTML( hInst, pcszName ) )
	{
		OnNewHTMLTextSet();
		return true;
	}
	return false;
}


void CQHTMControlSection::SetDefaultResourceHandle( HINSTANCE hInstance )
{
	m_htmlSection.SetDefaultResourceHandle( hInstance );
}


bool CQHTMControlSection::LoadFromFile( LPCTSTR pcszFilename )
{
	bool bRetVal = false;
	m_defaults.m_crBackground = ::GetSysColor( COLOR_3DFACE );
	if( m_htmlSection.SetHTMLFile( pcszFilename ) )
	{
		OnNewHTMLTextSet();
		bRetVal = true;
	}

	return bRetVal;
}


void CQHTMControlSection::LayoutHTML()
{
	m_bLayingOut = true;
	/*
		Get the measurements of the window without scrollbars.
		Deterine if the document will fit without scrollbars.
		if it does then simply remove any scrollbars and move on.

		If the doc. does not fit without scrollbars:
			Does the doc fit vertically?
				No, so add a vertical scrollbar.
			Does the doc fit horizontally?
				No, so add a horiziontal scrollbar.
		
		With new sizes layout the doc once more.
	*/

	m_htmlSection.ResetMeasuringKludge();

	GetClientRect( m_hwnd, this );

	//
	//	Get the width and height of the scroll bars
	const int nScrollWidth = GetSystemMetrics( SM_CXVSCROLL );
	const int nScrollHeight = GetSystemMetrics( SM_CXHSCROLL );
	int nHorizontalPos = 0;
	int nVerticalPos = 0;

	//
	//	If the window has scroll bars then remove them from our width and height
	if( GetWindowLong m_hwnd, GWL_STYLE ) & WS_VSCROLL )
	{
		right += nScrollWidth;
		nVerticalPos = ::GetScrollPos( m_hwnd, SB_VERT );
	}

	if( GetWindowLong m_hwnd, GWL_STYLE ) & WS_HSCROLL )
	{
		bottom += nScrollHeight;
		nHorizontalPos = ::GetScrollPos( m_hwnd, SB_HORZ );
	}

	GS::CDrawContext dc;
	m_htmlSection.OnLayout( *this,  dc );

	WinHelper::CSize size( m_htmlSection.GetSize() );
	if( m_bEnableScrollbars )
	{
		m_bShowHorizontal = false;
		m_bShowVertical = false;
		bool bFirst = true;
		bool bDoItAgain = false;
		while( bFirst || bDoItAgain )
		{
			//
			//	Create locals of the current width and height so we can test them. If we uses Width() and Height() then they
			//	could be changed by the tests below.
			const int nCurrentWidth = Width();
			const int nCurrentHeight = Height();
			bDoItAgain = false;
			if( size.cy > nCurrentHeight && ! m_bShowVertical )
			{
				m_bShowVertical = true;
				right -= nScrollWidth;
				bDoItAgain = true;
			}

			if( size.cx > nCurrentWidth && !m_bShowHorizontal )
			{
				m_bShowHorizontal = true;
				bottom -= nScrollHeight;
				bDoItAgain = true;
			}

			if( bDoItAgain )
			{
				m_htmlSection.OnLayout( *this,  dc );
				size = m_htmlSection.GetSize();
			}

			if( !bFirst )
				break;
			bFirst = false;
		}

		if( size.cx > Width() )
		{
			SCROLLINFO siNew = {0};
			siNew.cbSize = sizeof( siNew );
			siNew.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			siNew.nMax = size.cx;
			siNew.nPos = min( siNew.nMax, nHorizontalPos );
			siNew.nPage = Width();
			SetScrollInfo( m_hwnd, SB_HORZ, &siNew, TRUE );
			m_bShowHorizontal = TRUE;
		}

		if( size.cy > Height() )
		{
			SCROLLINFO siNew = {0};
			siNew.cbSize = sizeof( siNew );
			siNew.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
			siNew.nMax = size.cy;
			siNew.nPos = min( siNew.nMax, nVerticalPos );
			siNew.nPage = Height();
			SetScrollInfo( m_hwnd, SB_VERT, &siNew, TRUE );
			m_bShowVertical = TRUE;
		}

		ShowScrollBar( m_hwnd, SB_HORZ, m_bShowHorizontal );
		ShowScrollBar( m_hwnd, SB_VERT, m_bShowVertical );
	}
	m_bLayingOut = false;
}


void CQHTMControlSection::OnVScroll( int nScrollCode )
{
	WinHelper::CSize size( m_htmlSection.GetSize() );
	int	nNewPos = ::GetScrollPos( m_hwnd, SB_VERT );
	int nOldPos = nNewPos;

	const int nLineScrollAmount = m_htmlSection.GetLineHeight();

	switch( nScrollCode )
	{
	case SB_BOTTOM:
		nNewPos = size.cy - Height();
		break;

	case SB_TOP:
		nNewPos = 0;
		break;

	case SB_LINEDOWN:
		if( nNewPos < size.cy - Height() )
			nNewPos += nLineScrollAmount;
		break;


	case SB_LINEUP:
		if( nNewPos > 0)
			nNewPos -= nLineScrollAmount;
		break;


	case SB_PAGEDOWN:
		if( nNewPos < size.cy - Height() )
		{
			nNewPos += Height();
			if( nNewPos > size.cy - Height() )
				nNewPos = size.cy - Height();
		}
		break;


	case SB_PAGEUP:
		if( nNewPos > 0 )
		{
			nNewPos -= (Height() - 1);
			if( nNewPos < 0 )
				nNewPos = 0;
		}
		break;

	case SB_THUMBTRACK:
		{
			SCROLLINFO siNew;
			siNew.cbSize = sizeof( siNew );
			siNew.fMask = SIF_TRACKPOS;
			GetScrollInfo( m_hwnd, SB_VERT, &siNew );
			nNewPos = siNew.nTrackPos;
		}
		break;

	default:
		break;
	}

	if( nNewPos != nOldPos )
	{
		::SetScrollPos( m_hwnd, SB_VERT, nNewPos, TRUE );
		m_htmlSection.SetPos( nNewPos );
		ForceRedraw( *this );
	}
}


void CQHTMControlSection::OnHScroll( int nScrollCode )
{
	WinHelper::CSize size( m_htmlSection.GetSize() );
	int	nNewPos = ::GetScrollPos( m_hwnd, SB_HORZ );
	int nOldPos = nNewPos;

	const int nLineScrollAmount = m_htmlSection.GetLineHeight();

	switch( nScrollCode )
	{
	case SB_RIGHT:
		nNewPos = size.cx - Width();
		break;

	case SB_LEFT:
		nNewPos = 0;
		break;

	case SB_LINEDOWN:
		if( nNewPos < size.cx - Width() )
			nNewPos += nLineScrollAmount;
		break;


	case SB_LINEUP:
		if( nNewPos > 0)
			nNewPos -= nLineScrollAmount;
		break;


	case SB_PAGEDOWN:
		if( nNewPos < size.cx - Width() )
		{
			nNewPos += Width();
			if( nNewPos > size.cx - Width() )
				nNewPos = size.cx - Width();
		}
		break;


	case SB_PAGEUP:
		if( nNewPos > 0 )
		{
			nNewPos -= (Width() - 1);
			if( nNewPos < 0 )
				nNewPos = 0;
		}
		break;

	case SB_THUMBTRACK:
		{
			SCROLLINFO siNew;
			siNew.cbSize = sizeof( siNew );
			siNew.fMask = SIF_TRACKPOS;
			GetScrollInfo( m_hwnd, SB_HORZ, &siNew );
			nNewPos = siNew.nTrackPos;
		}
		break;

	default:
		break;
	}

	if( nNewPos != nOldPos )
	{
		::SetScrollPos( m_hwnd, SB_HORZ, nNewPos, TRUE );
		m_htmlSection.SetPosH( nNewPos );
		ForceRedraw( *this );
	}
}


int CQHTMControlSection::SetOption( UINT uOptionIndex, LPARAM lParam )
{
	switch( uOptionIndex )
	{
	case QHTM_OPT_SET_ALWAYS_SHOW_TIPS:
		m_bAlwaysShowTips = lParam ? true : false;
		break;

	case QHTM_OPT_TOOLTIPS:
		if( lParam )
			m_htmlSection.EnableTooltips( true );
		else
			m_htmlSection.EnableTooltips( false );
		return true;

	case QHTM_OPT_ZOOMLEVEL:
		if( lParam >= QHTM_ZOOM_MIN && lParam <= QHTM_ZOOM_MAX )
		{
			if( lParam != m_htmlSection.GetZoomLevel() )
			{
				m_htmlSection.SetZoomLevel( static_cast<int>( lParam ) );
//				TRACE( _T("Zoom level set to %d\n"), lParam );

				LayoutHTML();
				ForceRedraw( *this );
			}
			return true;
		}


	case QHTM_OPT_MARGINS:
		{
			LPRECT lprect = reinterpret_cast<WinHelper::CRect *>( lParam );
			if( !::IsBadReadPtr( lprect, sizeof( RECT ) ) )
			{
				m_htmlSection.SetDefaultMargins( lprect );
				LayoutHTML();
				return true;
			}
		}


	case QHTM_OPT_USE_COLOR_STATIC:
		if( lParam )
			m_bUseColorStatic = true;
		else
			m_bUseColorStatic = false;
		return true;


	case QHTM_OPT_ENABLE_SCROLLBARS:
		m_bEnableScrollbars = lParam ? true : false;
		break;

	case QHTM_OPT_SET_IMAGE_CALLBACK:
		m_htmlSection.SetImageCallback( reinterpret_cast<funcQHTMImageCallback>(lParam) );
		break;

	case QHTM_OPT_SET_BITMAP_CALLBACK:
		m_htmlSection.SetBitmapCallback( reinterpret_cast<funcQHTMBitmapCallback>(lParam) );
		break;

	case QHTM_OPT_SET_RESOURCE_CALLBACK:
		m_htmlSection.SetResourceCallback( reinterpret_cast<funcQHTMResourceCallback>(lParam) );
		break;

	case QHTM_OPT_SET_FORM_SUBMIT_CALLBACK:
		m_htmlSection.SetFormCallback( reinterpret_cast<funcQHTMFORMCallback>(lParam) );
		break;

	case QHTM_OPT_SET_EVENT_CALLBACK:
		m_htmlSection.SetEventCallback( reinterpret_cast<funcQHTMEventCallback>(lParam) );
		break;
		
	}
	return false;
}


LPARAM CQHTMControlSection::GetOption( UINT uOptionIndex, LPARAM lParam )
{
	switch( uOptionIndex )
	{
	case QHTM_OPT_TOOLTIPS:
		return m_htmlSection.IsTooltipsEnabled();

	case QHTM_OPT_SET_ALWAYS_SHOW_TIPS:
		return m_bAlwaysShowTips;

	case QHTM_OPT_ZOOMLEVEL:
		return m_htmlSection.GetZoomLevel();

	case QHTM_OPT_MARGINS:
		{
			LPRECT lprect = reinterpret_cast<WinHelper::CRect *>( lParam );
			if( !::IsBadWritePtr( lprect, sizeof( RECT ) ) )
			{
				m_htmlSection.GetDefaultMargins( lprect );
				LayoutHTML();
				return true;
			}
		}

	case QHTM_OPT_USE_COLOR_STATIC:
		return m_bUseColorStatic;
	}
	return 0;
}


void CQHTMControlSection::GotoLink( LPCTSTR pcszLinkName )
{
	m_htmlSection.GotoLink( pcszLinkName );
}


void CQHTMControlSection::ResetScrollPos()
{
	::SetScrollPos( m_hwnd, SB_HORZ, 0, TRUE );
	::SetScrollPos( m_hwnd, SB_VERT, 0, TRUE );

	m_htmlSection.SetPos( 0 );
	m_htmlSection.SetPosH( 0 );
}


bool CQHTMControlSection::OnNotify( const CSectionABC *pChild, const int nEvent )
{
	if( NotifyParent( nEvent, pChild ) )
		return true;

	if( pChild == &m_htmlSection && nEvent == 1 )
	{
		SCROLLINFO siOld = {0};
		siOld.cbSize = sizeof( siOld );
		siOld.fMask = SIF_POS | SIF_RANGE;
		GetScrollInfo( m_hwnd, SB_VERT, &siOld );
		siOld.nPos = min( m_htmlSection.GetScrollPos(), siOld.nMax );
		SetScrollInfo( m_hwnd, SB_VERT, &siOld, TRUE );

		siOld.cbSize = sizeof( siOld );
		siOld.fMask = SIF_POS | SIF_RANGE;
		GetScrollInfo( m_hwnd, SB_HORZ, &siOld );
		siOld.nPos = min( m_htmlSection.GetScrollPosH(), siOld.nMax );
		SetScrollInfo( m_hwnd, SB_HORZ, &siOld, TRUE );

		UpdateCursor();
	}
	return false;
}


size_t CQHTMControlSection::GetTitleLength() const
{
	return m_htmlSection.GetTitle().GetLength();
}


size_t CQHTMControlSection::GetTitle( size_t uBufferLength, LPTSTR pszBuffer ) const
{
	const StringClass & str = m_htmlSection.GetTitle();
	size_t uLength = min( str.GetLength(), uBufferLength - 1 );
	_tcsncpy( pszBuffer, m_htmlSection.GetTitle(), uLength );
	pszBuffer[ uLength ] = _T('\000');
	return uLength;
}


void CQHTMControlSection::SetFont( HFONT hFont )
{
	m_defaults.SetFont( hFont );
}


bool CQHTMControlSection::GetBackgroundColours( HDC hdc, HBRUSH &hbr ) const
{
	bool bReturnValue = false;
	if( m_bUseColorStatic )
	{
		HWND hwndParent = ::GetParent( m_hwnd );
		if( hwndParent )
		{
			BOOL bRetVal = static_cast<BOOL>( SendMessage( hwndParent, WM_CTLCOLORSTATIC, (WPARAM)hdc, (LPARAM)m_hwnd ) );
			if( bRetVal )
			{
				hbr = (HBRUSH)bRetVal;
				bReturnValue = true;
			}
		}
	}
	return bReturnValue;
}


void CQHTMControlSection::OnAddHTML( LPCTSTR pcsz, UINT uValue )
{
	m_htmlSection.AddHTML( pcsz );

	switch( uValue )
	{
	case 0:
		LayoutHTML();
		break;

	case 2:
		LayoutHTML();
		SetScrollPos( -1 );
		break;

	default:
		LayoutHTML();
	}
	UpdateCursor();

	ForceRedraw( *this );

}


bool CQHTMControlSection::UpdateCursor()
{
	WinHelper::CPoint pt;
	GetCursorPos( &pt );

	ScreenToClient( GetHwnd(), &pt );

	if( OnSetMouseCursor( pt ) )
	{
		return true;
	}

	return false;
}


void CQHTMControlSection::ForceRedraw( const WinHelper::CRect &rc )
{
	if( m_hwnd )
	{
		ASSERT_VALID_HWND( m_hwnd );
		::InvalidateRect( m_hwnd, &rc, TRUE );
	}
}


void CQHTMControlSection::DrawNow()
{
	ForceRedraw( *this );
	UpdateWindow( m_hwnd );
}


void CQHTMControlSection::OnTimer( int nTimerID )
{
	if( nTimerID == m_nMouseMoveTimerID )
	{
		WinHelper::CPoint pt;
		GetCursorPos( pt );
		HWND hwndCursor = WindowFromPoint( pt );
		if( hwndCursor != m_hwnd )
		{
			CancelHighlight();
			UnregisterTimerEvent( m_nMouseMoveTimerID );
			m_nMouseMoveTimerID = knNullTimerId;
		}
	}
	else
	{
		CSectionABC **pSect = m_mapTimerEvents.Lookup( nTimerID );
		if( pSect )
		{
			ASSERT_VALID_WRITEOBJPTR( pSect );
			(*pSect)->OnTimer( nTimerID );
		}
		else
		{
			::KillTimer( GetHwnd(), nTimerID );
		}
	}
}


int CQHTMControlSection::RegisterTimerEvent( CSectionABC *pSect, int nInterval )
{
	ASSERT_VALID_HWND( GetHwnd() );
	ASSERT_VALID_WRITEOBJPTR( pSect );
	ASSERT( nInterval > 0 );

	//TRACE( _T("Added timer id %d\n"), m_nNextTimerID );
	VAPI( ::SetTimer( GetHwnd(), m_nNextTimerID, nInterval, 0 ) );
	m_mapTimerEvents.SetAt( m_nNextTimerID, pSect );
	return m_nNextTimerID++;
}


void CQHTMControlSection::UnregisterTimerEvent( const int nTimerEventID )
{
	//TRACE( _T("Removed timer id %d\n"), nTimerEventID );
	ASSERT( nTimerEventID != knNullTimerId );
	if( m_mapTimerEvents.Lookup( nTimerEventID ) )
	{
		(void)::KillTimer( GetHwnd(), nTimerEventID );
		m_mapTimerEvents.RemoveAt( nTimerEventID );
	}
	else
	{
		//	Trying to unregister an event twice
		ASSERT( FALSE );
	}
}


void CQHTMControlSection::OnMouseMove( const WinHelper::CPoint &pt )
{
	if( m_pCaptureSection )
	{
		m_pCaptureSection->OnMouseMove( pt );
		return;
	}

	if( m_nMouseMoveTimerID == knNullTimerId )
	{
		m_nMouseMoveTimerID = RegisterTimerEvent( this, 20 );
		//TRACE( _T("Setting mouse move timer ID %d\n"), m_nMouseMoveTimerID );
	}
			

	//
	//	Find the section beneath the point, if there is no section then
	//	we simply use this section. Doing this simplifies the handling and we
	//	may want to do highlighting in this section in the future.
	CSectionABC *pSect = FindSectionFromPoint( pt );

#ifdef QHTM_ALLOW_TOOLTIPS
	if( CTipWindow::LastTipCreated() < (DWORD)GetMessageTime() )
	{
		if( CSectionABC::m_pTippedWindow && pSect != CSectionABC::m_pTippedWindow )
		{
			CSectionABC::KillTip();
		}
	}
#endif//	QHTM_ALLOW_TOOLTIPS


	if( !pSect )
	{
		pSect = this;
	}

	if( g_pSectHighlight != pSect )
	{
		if( g_pSectHighlight )
			g_pSectHighlight->OnMouseLeave();
		g_pSectHighlight = pSect;
		g_pSectHighlight->OnMouseEnter();
	}

	if( pSect != this )
	{
		pSect->OnMouseMove( pt );
	}
}


void CQHTMControlSection::OnDestroy()
{
	CParentSection::OnDestroy();

	if( m_nMouseMoveTimerID != knNullTimerId )
	{
		//TRACE( _T("Removing Mouse move timer %d\n"), m_nMouseMoveTimerID );
		UnregisterTimerEvent( m_nMouseMoveTimerID );
	}
	m_nMouseMoveTimerID = knNullTimerId;
	m_hwnd = NULL;
}


void CQHTMControlSection::SetFocus( bool bFocused )
{
	if( !m_htmlSection.IsFocused() )
	{
		m_htmlSection.SetFocus( bFocused );
	}
}


bool CQHTMControlSection::TabToNextControl()
{
	return m_htmlSection.TabToNextControl();
}

bool CQHTMControlSection::TabToPreviousControl()
{
	return m_htmlSection.TabToPreviousControl();
}


void CQHTMControlSection::ActivateFocusedItem()
{
	m_htmlSection.ActivateLink();
}


bool CQHTMControlSection::CanTabForward() const
{
	return m_htmlSection.CanTabForward();
}


bool CQHTMControlSection::CanTabBackward() const
{
	return m_htmlSection.CanTabBackward();
}


bool CQHTMControlSection::SelectPreviousInGroup()
{
	return m_htmlSection.SelectPreviousInGroup();
}

bool CQHTMControlSection::SelectNextInGroup()
{
	return m_htmlSection.SelectNextInGroup();
}

void CQHTMControlSection::SetFocusControl( bool bFirst )
{
	if( !m_htmlSection.HasFocusSection() )
	{
		m_htmlSection.SetFocusControl( bFirst );
	}
}

bool CQHTMControlSection::HasControls() const
{
	return m_htmlSection.HasControls();
}


bool CQHTMControlSection::ShouldShowTips() const
{
	if( m_bAlwaysShowTips || GetActiveWindow() )
	{
		return true;
	}
	return false;
}


BOOL CQHTMControlSection::GetLinkFromPoint( LPPOINT lpPoint, LPQHTM_LINK_INFO lpInfo ) const
{
	BOOL bRetVal = FALSE;

	if( lpPoint )
	{
		//
		//	See if there is a section below the point
		CSectionABC *pSect = m_htmlSection.FindSectionFromPoint( *lpPoint );
		if( pSect )
		{
			//
			//	We have a section. Now we see if it's a link
			const CHTMLSectionLink *pLink = m_htmlSection.GetLinkFromSection( pSect );
			if( pLink )
			{
				if( lpInfo && lpInfo->cbSize == sizeof( QHTM_LINK_INFO ) )
				{
					lpInfo->pszID = (LPTSTR)(LPCTSTR)pLink->m_strLinkID;
					lpInfo->pszURL = (LPTSTR)(LPCTSTR)pLink->m_strLinkTarget;
				}
				bRetVal = TRUE;
			}
		}
	}
	return bRetVal;
}


void CQHTMControlSection::SelectNearestLink( const POINT &pt )
{
	m_htmlSection.SelectNearestLink( pt );
}


bool CQHTMControlSection::SubmitForm( LPCTSTR pcszFormName, LPCTSTR pcszSubmitValue )
{
	return m_htmlSection.SubmitForm( pcszFormName, pcszSubmitValue );
}

bool CQHTMControlSection::ResetForm( LPCTSTR pcszFormName )
{
	return m_htmlSection.ResetForm( pcszFormName );
}



HRGN CQHTMControlSection::GetLinkRegion( LPCTSTR pcszLinkID ) const
{
	return m_htmlSection.GetLinkRegion( pcszLinkID );
}

void CQHTMControlSection::SetLParam( LPARAM lParam )
{
	m_defaults.m_lParam = lParam;
}


void CQHTMControlSection::OnMouseWheel( int nDelta )
{
	if( m_bShowVertical )
	{
		if( nDelta > 0 )
		{
			OnVScroll( SB_LINEUP );
		}
		else
		{
			OnVScroll( SB_LINEDOWN );
		}
	}
	else if( m_bShowHorizontal )
	{
		if( nDelta > 0 )
		{
			OnHScroll( SB_LINEUP );
		}
		else
		{
			OnHScroll( SB_LINEDOWN );
		}
	}
}


UINT CQHTMControlSection::OnParentEvent( const CSectionABC * pChild, const Event nEventType )
{
	if( m_defaults.m_funcQHTMEventCallback )
	{
		m_defaults.m_funcQHTMEventCallback( m_hwnd, pChild->GetElementID(), nEventType, 0, m_defaults.m_lParam );
	}
	return 0;
}

