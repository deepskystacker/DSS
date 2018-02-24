/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLControlSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include "HTMLControlSection.h"
#include "HTMLSectionCreator.h"
#include <reuse/WindowText.h>

#ifdef QHTM_WINDOWS_CONTROL

extern void SubclassControl( HWND hwnd, CHTMLSection *psectHTML, CFocusControlABC *pFocus );

CHTMLControlSection::CHTMLControlSection( CHTMLSection *pSectParent, GS::FontDef &fdef, const StringClass &strWindowClass, UINT uStyle, UINT uStyleEx, UINT uWidth, UINT uHeight )
	: CHTMLSectionABC( pSectParent )
	, m_fdef( fdef )
{
	SetKeep( true );

	m_hwnd = CreateWindowEx( uStyleEx, strWindowClass, _T(""),  uStyle, 0, 0, uWidth, uHeight, pSectParent->GetHwnd(), NULL, g_hQHTMInstance, NULL );
	VAPI( m_hwnd );

	SubclassControl( m_hwnd, pSectParent, this );
}


CHTMLControlSection::~CHTMLControlSection()
{
	DestroyWindow( m_hwnd );
}


void CHTMLControlSection::OnDraw( GS::CDrawContext &dc )
{
	if( dc.IsPrinting() )
	{
		dc.FillRect( *this, RGB( 255, 0, 0 ) );
	}
}


void CHTMLControlSection::SetFont( CHTMLSectionCreator *psc )
{
	GS::CDrawContext &dc = psc->GetDC();
	dc.SelectFont( m_fdef );

	SendMessage( m_hwnd, WM_SETFONT, (WPARAM)psc->GetDC().GetCurrentHFONT(), FALSE );
}


void CHTMLControlSection::OnLayout( const WinHelper::CRect &rc )
{
	SetWindowPos( m_hwnd, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );
}


void CHTMLControlSection::MoveY( int nOffsetY )
{
	CHTMLSectionABC::MoveY( nOffsetY );
	OnLayout( *this );
}


void CHTMLControlSection::MoveXY( int nOffsetX, int nOffsetY )
{
	CHTMLSectionABC::MoveXY( nOffsetX, nOffsetY );
	OnLayout( *this );
}


void CHTMLControlSection::SetFocus( bool bHasFocus )
{
	CHTMLSectionABC::SetFocus( bHasFocus );

	if( bHasFocus )
	{
		::SetFocus( m_hwnd );
	}
}


bool CHTMLControlSection::IsFocused() const
{
	return ::GetFocus() == m_hwnd;
}


void CHTMLControlSection::ResetContent()
{

}


void CHTMLControlSection::UpdateFormFromControls()
{
}

#endif	//	QHTM_WINDOWS_CONTROL