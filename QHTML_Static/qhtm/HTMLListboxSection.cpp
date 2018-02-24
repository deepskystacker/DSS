/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLListboxSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"

#include "QHTM.h"
#include "HTMLListboxSection.h"
#include "HTMLSectionCreator.h"
#include "HTMLFormInput.h"
#include <reuse/WindowText.h>

#ifdef QHTM_ALLOW_FORMS

extern void SubclassControl( HWND hwnd, CHTMLSection *psectHTML, CFocusControlABC *pFocus );

CHTMLListboxSection::CHTMLListboxSection( CHTMLSection *pSectParent, GS::FontDef &fdef, CHTMLSelect *pFormObject, class CHTMLSectionCreator *psc )
	: CHTMLSectionABC( pSectParent )
	, m_fdef( fdef )
	, m_pFormObject( pFormObject )
{
	SetHTMLID( pFormObject->m_strID );
	SetKeep( true );

	if( !psc->IsMeasuring() && !psc->GetDC().IsPrinting() )
	{
		DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_MULTIPLESEL;

		HWND hwndParent = pSectParent->GetHwnd();
		ASSERT( hwndParent );
		m_hwnd = CreateWindowEx( WS_EX_STATICEDGE, _T("LISTBOX"), NULL,  dwStyle, 0, 0, 0, 0, hwndParent, NULL, g_hQHTMInstance, NULL );
		VAPI( m_hwnd );

		for( UINT u = 0; u < pFormObject->m_arrItems.GetSize(); u++ )
		{
			CHTMLOption *pOption = pFormObject->m_arrItems[ u ];
			int nItem = static_cast< int > ( SendMessage( m_hwnd, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)pOption->m_strText ) );
			if( pOption->m_bSelected )
			{
				SendMessage( m_hwnd, LB_SETSEL, TRUE, nItem );
			}
		}

		if( pFormObject->m_bDisabled )
		{
			EnableWindow( m_hwnd, FALSE );
		}

		SubclassControl( m_hwnd, pSectParent, this );
	}
}


CHTMLListboxSection::~CHTMLListboxSection()
{
	DestroyWindow( m_hwnd );
}


void CHTMLListboxSection::OnDraw( GS::CDrawContext &dc )
{
	if( dc.IsPrinting() )
	{
		dc.Rectangle( *this, RGB( 0, 0, 0 ) );
		dc.SelectFont( m_fdef );

		WinHelper::CRect rc( *this );
		int nScrollSize = dc.ScaleX( GetSystemMetrics( SM_CXVSCROLL ) );
		rc.left = rc.right - nScrollSize;
		rc.bottom = rc.top + nScrollSize;
		DrawFrameControl( dc.GetSafeHdc(), rc, DFC_SCROLL, DFCS_SCROLLUP );

		rc = *this;
		rc.left = rc.right - nScrollSize;
		rc.top = rc.bottom - nScrollSize;
		DrawFrameControl( dc.GetSafeHdc(), rc, DFC_SCROLL, DFCS_SCROLLDOWN );

		rc = *this;
		rc.right -= nScrollSize;
		dc.SetClipRect( rc );

		int nTop = top;
		for( UINT u = 0; u < m_pFormObject->m_arrItems.GetSize(); u++ )
		{
			CHTMLOption *pOption = m_pFormObject->m_arrItems[ u ];
			dc.DrawText( left, nTop, pOption->m_strText, pOption->m_strText.GetLength(), RGB( 0, 0, 0 ) );
			nTop += dc.GetCurrentFontHeight();
		}

	}
}


void CHTMLListboxSection::FigureOutSize( CHTMLSectionCreator *psc, WinHelper::CSize &size, int &nBaseline )
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

	//
	//	Measure the list
	UINT uWidth = 0;
	for( UINT u = 0; u < m_pFormObject->m_arrItems.GetSize(); u++ )
	{
		CHTMLOption *pOption = m_pFormObject->m_arrItems[ u ];
		UINT uItemWidth = dc.GetTextExtent( pOption->m_strText, pOption->m_strText.GetLength() );
		if( uItemWidth > uWidth )
		{
			uWidth = uItemWidth;
		}
	}
	
	size.cx = uWidth + 12;
	size.cx += GetSystemMetrics( SM_CXVSCROLL );
	size.cy = dc.GetCurrentFontHeight() * + m_pFormObject->m_uSize + 4;
	nBaseline = size.cy - 4;
}


void CHTMLListboxSection::OnLayout( const WinHelper::CRect &rc )
{
	SetWindowPos( m_hwnd, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );
}


void CHTMLListboxSection::MoveY( int nOffsetY )
{
	CHTMLSectionABC::MoveY( nOffsetY );
	OnLayout( *this );
}


void CHTMLListboxSection::MoveXY( int nOffsetX, int nOffsetY )
{
	CHTMLSectionABC::MoveXY( nOffsetX, nOffsetY );
	OnLayout( *this );
}


void CHTMLListboxSection::SetFocus( bool bHasFocus )
{
	CHTMLSectionABC::SetFocus( bHasFocus );

	if( bHasFocus )
	{
		::SetFocus( m_hwnd );
	}
}


bool CHTMLListboxSection::IsFocused() const
{
	return ::GetFocus() == m_hwnd;
}


void CHTMLListboxSection::ResetContent()
{
	for( UINT u = 0; u < m_pFormObject->m_arrItems.GetSize(); u++ )
	{
		CHTMLOption *pOption = m_pFormObject->m_arrItems[ u ];
		if( pOption->m_bSelected )
		{
			SendMessage( m_hwnd, LB_SETSEL, TRUE, u );
		}
		else
		{
			SendMessage( m_hwnd, LB_SETSEL, FALSE, u );
		}
	}
}


void CHTMLListboxSection::UpdateFormFromControls()
{
	for( UINT u = 0; u < m_pFormObject->m_arrItems.GetSize(); u++ )
	{
		CHTMLOption *pOption = m_pFormObject->m_arrItems[ u ];
		if( SendMessage( m_hwnd, LB_GETSEL, u, 0 ) )
		{
			pOption->m_bSelected = true;
		}
		else
		{
			pOption->m_bSelected = false;
		}
	}
}

#endif	//	QHTM_ALLOW_FORMS
