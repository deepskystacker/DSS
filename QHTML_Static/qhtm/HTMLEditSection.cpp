/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLEditSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Image section.
					NOTE: Currently ignores the width and height
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include "HTMLEditSection.h"
#include "HTMLSectionCreator.h"
#include "HTMLFormInput.h"
#include <reuse/WindowText.h>

#ifdef QHTM_ALLOW_FORMS


extern void SubclassControl( HWND hwnd, CHTMLSection *psectHTML, CFocusControlABC *pFocus );

CHTMLEditSection::CHTMLEditSection( CHTMLSection *pSectParent, GS::FontDef &fdef, CHTMLFormInput *pFormObject, class CHTMLSectionCreator *psc )
	: CHTMLSectionABC( pSectParent )
	, m_fdef( fdef )
	, m_pFormObject( pFormObject )
	, m_strOriginalSelection( pFormObject->m_strValue )
{
	SetHTMLID( pFormObject->m_strID );
	SetKeep( true );

	if( !psc->IsMeasuring() && !psc->GetDC().IsPrinting() )
	{
		DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE;

		if( pFormObject->GetFormType() == CHTMLFormInput::knTextArea )
		{
			dwStyle |= (ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL);
		}
		else
		{
			dwStyle |= ES_AUTOHSCROLL;
		}

		if( pFormObject->m_bReadonly )
		{
			dwStyle |= ES_READONLY;
		}

		if( pFormObject->GetFormType() == CHTMLFormInput::knPassword )
		{
			dwStyle |= ES_PASSWORD;
		}

		LPTSTR endptr;
		UINT uID = pFormObject->m_strID.GetLength() ? _tcstol( pFormObject->m_strID, &endptr, 10 ) : 0;
		m_hwnd = CreateWindowEx( WS_EX_STATICEDGE, _T("EDIT"), pFormObject->m_strValue,  dwStyle, 0, 0, 0, 0, pSectParent->GetHwnd(), (HMENU)uID, g_hQHTMInstance, NULL );
		ASSERT( m_hwnd );
		if( pFormObject->m_bDisabled )
		{
			EnableWindow( m_hwnd, FALSE );
		}
		SubclassControl( m_hwnd, pSectParent, this );

		if( pFormObject->m_uMaxLength )
		{
			SendMessage( m_hwnd, EM_LIMITTEXT, pFormObject->m_uMaxLength, 0 );
		}
		SendMessage( m_hwnd, EM_SETMARGINS, EC_LEFTMARGIN, 0 );
		SendMessage( m_hwnd, EM_SETMARGINS, EC_RIGHTMARGIN, 0 );
	}
}


CHTMLEditSection::~CHTMLEditSection()
{
	if( m_hwnd )
	{
		DestroyWindow( m_hwnd );
	}
}


void CHTMLEditSection::OnDraw( GS::CDrawContext &dc )
{
	if( dc.IsPrinting() )
	{
		dc.Rectangle( *this, RGB( 0, 0, 0 ) );
		dc.SelectFont( m_fdef );
		dc.SetClipRect( *this );
		if( m_pFormObject->GetFormType() == CHTMLFormInput::knTextArea )
		{
			//	Need to wrap this text...
			dc.DrawText( left, top, m_pFormObject->m_strValue, m_pFormObject->m_strValue.GetLength(), RGB( 0, 0, 0 ) );
		}
		else
		{
			dc.DrawText( left, top, m_pFormObject->m_strValue, m_pFormObject->m_strValue.GetLength(), RGB( 0, 0, 0 ) );
		}

		dc.RemoveClip();
	}
}


void CHTMLEditSection::FigureOutSize( CHTMLSectionCreator *psc, WinHelper::CSize &size, int &nBaseline )
{
	GS::CDrawContext &dc = psc->GetDC();
	dc.SelectFont( m_fdef );

	#if defined (_WIN32_WCE)
	// value of all f's causes exception error in wince.
	// occurs before combo is created and sizing is in progress?
	if (m_hwnd == (HWND)0xFFFFFFFF) 
		m_hwnd = 0;
	#endif

	SendMessage( m_hwnd, WM_SETFONT, (WPARAM)psc->GetDC().GetCurrentHFONT(), FALSE );

	if( m_pFormObject->GetFormType() == CHTMLFormInput::knTextArea )
	{
		if( m_pFormObject->m_nCols > 0 )
		{
			int nWidth = dc.GetCurrentFontAverageWidth() * m_pFormObject->m_nCols;
			size.cx = nWidth + 12;
			size.cx += GetSystemMetrics( SM_CXVSCROLL );
		}
		else
		{
			size.cx = psc->GetCurrentWidth();
		}

		size.cy = dc.GetCurrentFontHeight() * m_pFormObject->m_uRows + 4;
	}
	else
	{
		int nWidth = dc.GetCurrentFontAverageWidth() * m_pFormObject->m_nCols;
		size.cx = nWidth + 12;
		size.cy = dc.GetCurrentFontHeight() + 6;
	}
	nBaseline = size.cy - 4;
}


void CHTMLEditSection::OnLayout( const WinHelper::CRect &rc )
{
	if( m_hwnd )
	{
		SetWindowPos( m_hwnd, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );
	}
}


void CHTMLEditSection::MoveY( int nOffsetY )
{
	CHTMLSectionABC::MoveY( nOffsetY );
	OnLayout( *this );
}


void CHTMLEditSection::MoveXY( int nOffsetX, int nOffsetY )
{
	CHTMLSectionABC::MoveXY( nOffsetX, nOffsetY );
	OnLayout( *this );
}


void CHTMLEditSection::SetFocus( bool bHasFocus )
{
	CHTMLSectionABC::SetFocus( bHasFocus );

	if( bHasFocus )
	{
		::SetFocus( m_hwnd );
	}
}


bool CHTMLEditSection::IsFocused() const
{
	return ::GetFocus() == m_hwnd;
}


void CHTMLEditSection::ResetContent()
{
	SetWindowText( m_hwnd, m_strOriginalSelection  );
}


void CHTMLEditSection::UpdateFormFromControls()
{
	CWindowText str( m_hwnd );

	m_pFormObject->m_strValue.Set( str, str.GetLength() );
}

#endif	//	QHTM_ALLOW_FORMS
