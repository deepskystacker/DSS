/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
File:	sectionabc.cpp
Owner:	russf@gipsysoft.com
Purpose:	base class for all sections
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "SectionABC.h"
#include "ParentSection.h"
#include "Utils.h"
#include "TipWindow.h"

//REVIEW - russf - bit of a kludge
extern CSectionABC *g_pSectMouseDowned;
extern void CancelMouseDowns();		
extern CSectionABC *g_pSectHighlight;
extern void CancelHighlight();
extern bool IsMouseDown();

//
//	Uncomment this to put a red rectangle around everything that gets drawn
//#define DRAWING_TESTING

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

enum { g_knTipWaitTime = 400 };

#ifdef QHTM_ALLOW_TOOLTIPS
CTipWindow *CSectionABC::m_pCurrentTip = NULL;
#endif	//	QHTM_ALLOW_TOOLTIPS

CSectionABC *CSectionABC::m_pTippedWindow = NULL;

static LONG g_lLastMouseMove = 0;

CSectionABC::CSectionABC( )
	: m_psectParent( NULL )
	, m_bMouseInSection( false )
	, m_bLeftMouseDown( false )
	,	m_nTipTimerID( knNullTimerId )
	, m_bTransparent( false )
	, m_bKeep( false )
	, m_uID( 0 )
	, m_cCharSet( DEFAULT_CHARSET )
{
	Empty();
}


CSectionABC::~CSectionABC()
{
	if( g_pSectHighlight == this )
		CancelHighlight();

	if( SectionHasToolTip() )
	{
		KillTip();
		m_pTippedWindow = NULL;
	}

	//
	//	If we are the section that was mousedowned on then we cancel it
	if( g_pSectMouseDowned == this )
		CancelMouseDowns();
}


void CSectionABC::OnMouseMove( const WinHelper::CPoint & )
{
	g_lLastMouseMove = GetMessageTime();
}


void CSectionABC::OnDestroy()
{
	if( SectionHasToolTip() )
	{
		KillTip();
	}
	RestTipTimer( false );
}


bool CSectionABC::OnSetMouseCursor( const WinHelper::CPoint &pt )
{
	//
	//	Find a section the point falls in.
	//	If we succeed at that then pass the event onto the section, if
	//		it gets handled in the section then do nothing otherwise we set our own section cursor.
	CSectionABC *pSect = FindSectionFromPoint( pt );
	bool bRetVal = false;
	if( pSect )
	{
		pSect->SetCursor( pt );
		bRetVal = true;
	}
	return bRetVal;
}


void CSectionABC::OnDraw( GS::CDrawContext & dc )
{
#ifdef DRAWING_TESTING
	dc.Rectangle( *this, RGB( 255,0,0 ) );
#else
	UNREF( dc );
#endif	//	DRAWING_TESTING
}


void CSectionABC::OnLayout( const WinHelper::CRect &rc )
{
	CRect::operator = ( rc );
}


CSectionABC * CSectionABC::FindSectionFromPoint(const WinHelper::CPoint & /*pt*/) const
//
//	Given a point return pointer to a section if the pointer is within a section
//	Can return NULL if there is no section at the point.
{
	return NULL;
}


void CSectionABC::SetCursor( const WinHelper::CPoint & )
{
	m_cursor.Set();
}


void CSectionABC::OnMouseLeftDown( const WinHelper::CPoint &pt )
{
	CSectionABC *pSect = FindSectionFromPoint( pt );
	if( pSect )
	{
		pSect->OnMouseLeftDown( pt );
	}
	else
	{
		m_bLeftMouseDown = true;
	}
}

void CSectionABC::OnMouseRightDown( const WinHelper::CPoint &pt )
{
	CSectionABC *pSect = FindSectionFromPoint( pt );
	if( pSect )
	{
		pSect->OnMouseRightDown( pt );
	}
}

void CSectionABC::OnMouseRightUp( const WinHelper::CPoint &pt )
{
	CSectionABC *pSect = FindSectionFromPoint( pt );
	if( pSect )
	{
		pSect->OnMouseRightUp( pt );
	}
}


void CSectionABC::OnMouseLeftUp( const WinHelper::CPoint & )
{
	m_bLeftMouseDown = false;
}
	

void CSectionABC::OnStopMouseCapture()
{
	//
	//	Do nothing
}


void CSectionABC::OnStopMouseDown()
{
	m_bLeftMouseDown = false;
}


void CSectionABC::ForceRedraw()
//
//	Force the section to be re-drawn.
{
	ForceRedraw( *this );
}


void CSectionABC::ForceRedraw( const WinHelper::CRect &rc )
//
//	Force the section to be re-drawn.
{
	if( m_psectParent )
	{
		m_psectParent->ForceRedraw( rc );
	}
}


void CSectionABC::DrawNow()
{
	if( m_psectParent )
	{
		m_psectParent->DrawNow();
	}
}


void CSectionABC::Scroll( int cx, int cy, const WinHelper::CRect &rc )
{
#ifdef _WINDOWS_
	WinHelper::CRect rcUpdate;
	(void)::ScrollWindowEx( GetHwnd(), cx, cy, &rc, &rc, NULL, &rcUpdate, SW_INVALIDATE );
	(void)InvalidateRect( GetHwnd(), rcUpdate, FALSE );
#endif	//	_WINDOWS_
}

void CSectionABC::Transparent( bool bTransparent )
{
	m_bTransparent = bTransparent;
}



bool CSectionABC::NotifyParent( int nEvent, const CSectionABC *pChild /*= NULL*/ )
//
//	Send our parent a notify event. If pChild is NULL then this is used as child
{
	if( m_psectParent )
	{
		return m_psectParent->OnNotify( pChild ? pChild : this, nEvent );
	}
	else
	{
		//TRACE( _T("CSectionABC::NotifyParent not sent because no parent\n") );
	}

	return false;
}

bool CSectionABC::OnNotify( const CSectionABC * pChild, const int nEvent )
{
	if( GetParent() )
	{
		return GetParent()->OnNotify( pChild, nEvent );
	}
	return false;
}

void CSectionABC::OnMouseEnter()
{
	m_bMouseInSection = true;

	if( GetParent() )
	{
		GetParent()->OnParentEvent( this, knEventMouseEnter );
	}

	RestTipTimer( true );
}



UINT CSectionABC::OnParentEvent( const CSectionABC * pChild, const Event nEventType )
{
	if( GetParent() )
	{
		GetParent()->OnParentEvent( pChild, nEventType );
	}

	//
	//	Currently we are not interested in the return value but the future may need something
	return 0;
}


void CSectionABC::OnMouseLeave()
{
	if( GetParent() )
	{
		GetParent()->OnParentEvent( this, knEventMouseLeave );
	}

	RestTipTimer( false );
	m_bMouseInSection = false;
}


void CSectionABC::KillTip()
{
#ifdef QHTM_ALLOW_TOOLTIPS
	if( m_pCurrentTip )
	{
		if( !WinHelper::IsControlPressed() )
		{
			//
			//	We copy into temporary just in case we get a recursive call.
			CTipWindow *pWndTip = m_pCurrentTip;
			m_pCurrentTip = NULL;
			pWndTip -> DestroyWindow();
		}
	}	
#endif	//QHTM_ALLOW_TOOLTIPS
}


void	CSectionABC::KillTip( TipKillReason /*nKillReason*/ )
{
	CSectionABC::KillTip();
}


void CSectionABC::RestTipTimer( bool bReStart )
{
#ifdef QHTM_ALLOW_TOOLTIPS
	if( m_pTippedWindow )
	{
		KillTip();
	}

	if( m_nTipTimerID != knNullTimerId )
	{
		const int nTipTimerID = m_nTipTimerID;
		m_nTipTimerID = knNullTimerId;

		UnregisterTimerEvent( nTipTimerID );
		//TRACE( _T("Removing tip timer ID %d\n"), nTipTimerID );
	}

	if( bReStart && GetTipText().GetLength() )
	{
		m_nTipTimerID = RegisterTimerEvent( this, g_knTipWaitTime );
		//TRACE( _T("Setting tip timer ID %d\n"), m_nTipTimerID );
	}
#endif	//	QHTM_ALLOW_TOOLTIPS
}


void CSectionABC::OnTimer( int nTimerID )
{
#ifdef QHTM_ALLOW_TOOLTIPS
	if( m_nTipTimerID == nTimerID && !IsMouseDown() && IsMouseInSection() && GetMessageTime() - g_lLastMouseMove >= g_knTipWaitTime && !m_pCurrentTip )
	{
		RestTipTimer( false );
		StringClass strTip = GetTipText();
		if( !WinHelper::IsControlPressed() && strTip.GetLength() && ShouldShowTips() )
		{
			WinHelper::CPoint pt;
			GetMousePoint( pt );
			m_pCurrentTip = CreateTipWindow( strTip, pt );
			if( m_pCurrentTip )
			{
				m_pTippedWindow = this;
			}
		}
	}
#endif	//	QHTM_ALLOW_TOOLTIPS
}


bool CSectionABC::ShouldShowTips() const
{
	if( GetParent() )
	{
		return GetParent()->ShouldShowTips();
	}
	return true;
}


CTipWindow *CSectionABC::CreateTipWindow(  LPCTSTR pcszTip, WinHelper::CPoint &pt  )
{
#ifdef QHTM_ALLOW_TOOLTIPS
	return new CTipWindow( this, pcszTip, pt, GetCharacterSet() );
#else//	QHTM_ALLOW_TOOLTIPS
	return NULL;
#endif	//	QHTM_ALLOW_TOOLTIPS
}


StringClass CSectionABC::GetTipText() const
{
	return m_strTipText;
}


void CSectionABC::SetTipText( const StringClass &strTipText )
{
	m_strTipText = strTipText;
}


void CSectionABC::OnMouseWheel( int nDelta )
{
	//
	//	Default simply calls it's parent
	if( m_psectParent )
	{
		m_psectParent->OnMouseWheel( nDelta );
	}
}


int CSectionABC::RegisterTimerEvent( CSectionABC *pSect, int nInterval )
{
	if( GetParent() )
		return GetParent()->RegisterTimerEvent( pSect, nInterval );
	return 0;
}


void CSectionABC::UnregisterTimerEvent( const int nTimerEventID )
{
	if( GetParent() )
		GetParent()->UnregisterTimerEvent( nTimerEventID );
}


HWND CSectionABC::GetHwnd() const
{
	if( GetParent() )
		return GetParent()->GetHwnd();
	return NULL;
}
