/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLButtonSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Image section.
					NOTE: Currently ignores the width and height
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLButtonSection.h"
#include "HTMLSectionCreator.h"

#ifdef QHTM_ALLOW_FORMS

static const int knHorizontalBorder = 6;
static const int knVerticalBorder = 4;

#ifndef DFCS_TRANSPARENT
	#define DFCS_TRANSPARENT        0x0800
#endif	//	DFCS_TRANSPARENT

extern void SubclassControl( HWND hwnd, CHTMLSection *psectHTML, CFocusControlABC *pFocus );

CHTMLButtonSection::CHTMLButtonSection( CHTMLSection *pSectParent, GS::FontDef &fdef, CHTMLFormInput *pFormObject, class CHTMLSectionCreator *psc )
	: CHTMLSectionABC( pSectParent )
	, m_pFormObject( pFormObject )
	, m_fdef( fdef )
	, m_bOriginalCheckState( pFormObject->m_bChecked )
{
	SetHTMLID( pFormObject->m_strID );
	SetKeep( true );

	if( !psc->IsMeasuring() && !psc->GetDC().IsPrinting() )
	{

		LPCTSTR pcszValue = NULL;

		DWORD dwStyle = WS_CHILD | WS_VISIBLE;
		switch( m_pFormObject->GetFormType() )
		{
		case CHTMLFormInput::knReset:
		case CHTMLFormInput::knButton:
		case CHTMLFormInput::knSubmit:
			dwStyle |= BS_PUSHBUTTON | BS_TEXT;
			pcszValue = m_pFormObject->m_strValue;
			break;

		case CHTMLFormInput::knRadio:
			dwStyle |= BS_RADIOBUTTON;
			break;

		case CHTMLFormInput::knCheckbox:
			dwStyle |= BS_AUTOCHECKBOX;
			break;
		}

		HWND hwndParent = pSectParent->GetHwnd();
		ASSERT( hwndParent );
		LPTSTR endptr;
		INT ID = pFormObject->m_strID.GetLength() ? _tcstol( pFormObject->m_strID, &endptr, 10 ) : 0;
#pragma warning(disable : 4312)
		m_hwnd = CreateWindow( _T("BUTTON"), pcszValue, dwStyle, 0, 0, 0, 0, hwndParent, (HMENU)ID, g_hQHTMInstance, NULL );
#pragma warning(default : 4312)
		ASSERT( m_hwnd );
		if( m_pFormObject->m_bDisabled )
		{
			EnableWindow( m_hwnd, FALSE );
		}

		SubclassControl( m_hwnd, pSectParent, this );
		SetMessageReflector( m_hwnd );

		ResetContent();
	}
}


CHTMLButtonSection::~CHTMLButtonSection()
{
	if( m_hwnd )
	{
		RemoveMessageReflector( m_hwnd );
		DestroyWindow( m_hwnd );
	}
}


void CHTMLButtonSection::OnDraw( GS::CDrawContext &dc )
{
	if( dc.IsPrinting() )
	{
		dc.SelectFont( m_fdef );

		switch( m_pFormObject->GetFormType() )
		{
		case CHTMLFormInput::knButton:
		case CHTMLFormInput::knSubmit:
		case CHTMLFormInput::knReset:
			DrawFrameControl( dc.GetSafeHdc(), this, DFC_BUTTON, DFCS_BUTTONPUSH );
			dc.DrawText( left + knHorizontalBorder, top + knVerticalBorder, m_pFormObject->m_strValue, m_pFormObject->m_strValue.GetLength(), GetSysColor( COLOR_WINDOWTEXT ) );
			break;

		case CHTMLFormInput::knRadio:
			DrawFrameControl( dc.GetSafeHdc(), this, DFC_BUTTON, DFCS_BUTTONRADIO | (m_pFormObject->m_bChecked ? DFCS_CHECKED : 0) );
			break;

		case CHTMLFormInput::knCheckbox:
			DrawFrameControl( dc.GetSafeHdc(), this, DFC_BUTTON, DFCS_BUTTONCHECK | (m_pFormObject->m_bChecked ? DFCS_CHECKED : 0) );
			break;
		}		
	}
	else
	{
		if( IsFocused() )
		{
			dc.DrawFocus( *this );
		}
		CHTMLSectionABC::OnDraw( dc );
	}
}


void CHTMLButtonSection::FigureOutSize( CHTMLSectionCreator *psc, WinHelper::CSize &size, int &nBaseline )
{
	GS::CDrawContext &dc = psc->GetDC();
	dc.SelectFont( m_fdef );

	SendMessage( m_hwnd, WM_SETFONT, (WPARAM)psc->GetDC().GetCurrentHFONT(), FALSE );
	const int nFontHeight = dc.GetCurrentFontHeight();
	switch( m_pFormObject->GetFormType() )
	{
	case CHTMLFormInput::knButton:
	case CHTMLFormInput::knSubmit:
	case CHTMLFormInput::knReset:
		size.cx = dc.GetTextExtent( m_pFormObject->m_strValue, m_pFormObject->m_strValue.GetLength() ) + knHorizontalBorder * 2;
		size.cy = nFontHeight + knVerticalBorder * 2;
		nBaseline = size.cy - knVerticalBorder;
		break;

	case CHTMLFormInput::knRadio:
		size.cx = size.cy = ( nFontHeight - ( nFontHeight - dc.GetCurrentFontBaseline() ) ) + 2;
		nBaseline = dc.GetCurrentFontBaseline();
		break;

	case CHTMLFormInput::knCheckbox:
		size.cx = size.cy = ( nFontHeight - ( nFontHeight - dc.GetCurrentFontBaseline() ) ) + 2;
		nBaseline = dc.GetCurrentFontBaseline();
		break;
	}
}


void CHTMLButtonSection::OnLayout( const WinHelper::CRect &rc )
{
	if( m_pFormObject->GetFormType() == CHTMLFormInput::knRadio || m_pFormObject->GetFormType() == CHTMLFormInput::knCheckbox )
	{
		//
		//	We need to do this otehrwise we can't see the focus rectangle
		WinHelper::CRect rc2( rc );
		rc2.Inflate( -1, -1 );
		SetWindowPos( m_hwnd, NULL, rc2.left, rc2.top, rc2.Width(), rc2.Height(), SWP_NOZORDER );
	}
	else
	{
		SetWindowPos( m_hwnd, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );
	}
}


void CHTMLButtonSection::MoveY( int nOffsetY )
{
	CHTMLSectionABC::MoveY( nOffsetY );
	OnLayout( *this );
}


void CHTMLButtonSection::MoveXY( int nOffsetX, int nOffsetY )
{
	CHTMLSectionABC::MoveXY( nOffsetX, nOffsetY );
	OnLayout( *this );
}


void CHTMLButtonSection::SetFocus( bool bHasFocus )
{
	if( bHasFocus )
	{
		::SetFocus( m_hwnd );
	}

	CHTMLSectionABC::SetFocus( bHasFocus );
}


bool CHTMLButtonSection::IsFocused() const
{
	return ::GetFocus() == m_hwnd;
}


LRESULT CHTMLButtonSection::OnWindowMessage( UINT uMessage, WPARAM wParam, LPARAM )
{
	switch( uMessage )
	{
	case WM_COMMAND:
		switch( HIWORD( wParam ) )
		{
		case BN_CLICKED:
			switch( m_pFormObject->GetFormType() )
			{
			case CHTMLFormInput::knReset:
				if( m_pFormObject->m_pForm )
				{
					m_pFormObject->m_pForm->ResetContent();
				}
				break;


			case CHTMLFormInput::knSubmit:
				if( m_pFormObject->m_pForm )
				{
					m_psectHTMLParent->SubmitForm( m_pFormObject->m_pForm, m_pFormObject->m_strValue );
				}
				break;


			case CHTMLFormInput::knRadio:
				m_pFormObject->m_pForm->UncheckAllNamedInputs( m_pFormObject->m_strName );
				Check( true );
				break;
			}
		}
		break;


	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		switch( m_pFormObject->GetFormType() )
		{
		case CHTMLFormInput::knRadio:
		case CHTMLFormInput::knCheckbox:
			{
				//HDC hdc = (HDC)wParam;
				return (LRESULT) GetStockObject( HOLLOW_BRUSH );
			}
			break;
		}
		break;
	}
	return 0;
}


void CHTMLButtonSection::Check( bool bCheck )
{
	if( bCheck )
	{
		SendMessage( m_hwnd, BM_SETCHECK, BST_CHECKED, 0 );
	}
	else
	{
		SendMessage( m_hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	}
}


void CHTMLButtonSection::CheckIfNamed( LPCTSTR pcszName, bool bCheck )
{
	if( m_pFormObject && m_pFormObject->m_strName.GetLength() && !_tcsicmp( m_pFormObject->m_strName, pcszName ) )
	{
		Check( bCheck );
	}
}



void CHTMLButtonSection::ResetContent()
{
	m_pFormObject->m_bChecked = m_bOriginalCheckState;
	Check( m_pFormObject->m_bChecked );
}


void CHTMLButtonSection::UpdateFormFromControls()
{
	if( SendMessage( m_hwnd, BM_GETCHECK, 0, 0 ) )
	{
		m_pFormObject->m_bChecked = true;
	}
	else
	{
		m_pFormObject->m_bChecked = false;
	}	
}


bool CHTMLButtonSection::IsSameName( LPCTSTR pcszName ) const
{
	if( pcszName && m_pFormObject->m_strName.GetLength() && !_tcsicmp( m_pFormObject->m_strName, pcszName ) )
	{
		return true;
	}
	return false;
}


LPCTSTR CHTMLButtonSection::GetName() const
{
	return m_pFormObject->m_strName;
}


bool CHTMLButtonSection::IsSelected() const
{
	switch( m_pFormObject->GetFormType() )
	{
	case CHTMLFormInput::knRadio:
		if( SendMessage( m_hwnd, BM_GETCHECK, 0, 0 ) )
		{
			return true;
		}
		return false;		
	}
	
	return true;
}


void CHTMLButtonSection::Activate()
{
	switch( m_pFormObject->GetFormType() )
	{
	case CHTMLFormInput::knCheckbox:
		if( SendMessage( m_hwnd, BM_GETCHECK, 0, 0 ) )
		{
			Check( false );
		}
		else
		{
			Check( true );
		}
		
		break;
	}	
}

#endif	//	QHTM_ALLOW_FORMS
