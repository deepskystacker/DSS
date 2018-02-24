/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLSectionABC.cpp
Owner:	russf@gipsysoft.com
Purpose:	Base class used for the HTML display sections.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLSectionABC.h"
#include "HTMLSectionLink.h"
#include "Utils.h"

//#define DRAW_DEBUG

bool g_bShowingTooltips = true;

CHTMLSectionABC::CHTMLSectionABC( CHTMLSection *psectParent )
	: m_pHtmlLink( NULL )
	, m_psectHTMLParent( psectParent )
{

}


CHTMLSectionABC::~CHTMLSectionABC()
{
}


void CHTMLSectionABC::SetAsLink( CHTMLSectionLink* pLink )
{
	m_pHtmlLink = pLink;

	if( pLink )
	{
		m_cursor.Load( CCursor::knHand );
		if( pLink->m_bTitleSet )
		{
			SetTipText( pLink->m_strLinkTitle );
		}
		else
		{
			if( pLink->m_strLinkTarget.GetLength() )
			{
				SetTipText( pLink->m_strLinkTarget );
			}
		}
		pLink->AddSection( this );
	}
}


void CHTMLSectionABC::OnMouseEnter()
{
	CSectionABC::OnMouseEnter();

	if (m_pHtmlLink)
	{
		m_pHtmlLink->OnMouseEnter();
	}
}


#ifdef QHTM_ALLOW_FOCUS
void CHTMLSectionABC::OnDraw( GS::CDrawContext &dc )
{
	if( IsFocused() && m_pHtmlLink )
	{
		m_pHtmlLink->DrawFocus( dc );
	}

#ifdef DRAW_DEBUG
	dc.Rectangle( *this, RGB( 255, 0, 0 ) );
#endif	//	 DRAW_DEBUG
	CSectionABC::OnDraw( dc );
}
#endif	//	QHTM_ALLOW_FOCUS
		

void CHTMLSectionABC::OnMouseLeave()
{
	CSectionABC::OnMouseLeave();

	if( m_pHtmlLink )
		m_pHtmlLink->OnMouseLeave();

}


void CHTMLSectionABC::OnMouseLeftDown( const WinHelper::CPoint &pt )
{
	CSectionABC::	OnMouseLeftDown( pt );
	if( m_pHtmlLink )
	{
		m_psectHTMLParent->SetCurrentFocus( m_pHtmlLink, true );
		m_psectHTMLParent->ForceRedraw();
	}
	else if( m_strActivationTarget.GetLength() )
	{
		m_psectHTMLParent->SetFocusAndActivate( m_strActivationTarget );
	}
}


void CHTMLSectionABC::OnMouseLeftUp( const WinHelper::CPoint & )
{
	if( m_pHtmlLink )
	{
		m_psectHTMLParent->GotoLink( m_pHtmlLink );
	}
}


StringClass CHTMLSectionABC::GetTipText() const
{
	if( g_bShowingTooltips )
		return CSectionABC::GetTipText();

	return NULL;
}


void CHTMLSectionABC::EnableTooltips( bool bEnable )
{
	g_bShowingTooltips = bEnable;
}

bool CHTMLSectionABC::IsTooltipsEnabled()
{
	return g_bShowingTooltips;
}

COLORREF CHTMLSectionABC::LinkColour()
{
	if (m_pHtmlLink)
		return m_pHtmlLink->m_crLink;
	else
		return RGB( 141, 7, 102 );
}


COLORREF CHTMLSectionABC::LinkHoverColour()
{
	if (m_pHtmlLink)
		return m_pHtmlLink->m_crHover;
	else
		return RGB( 29, 49, 149 );
}


void CHTMLSectionABC::SetFocus( bool  )
{
	ForceRedraw();
}

bool CHTMLSectionABC::IsFocused() const
{
	return m_pHtmlLink && m_pHtmlLink->IsFocused();
}

void CHTMLSectionABC::GetObjectRect( WinHelper::CRect &rcBounds ) const
{
	rcBounds = *this;
}


void CHTMLSectionABC::Activate()
{
	if( m_pHtmlLink )
	{
		m_psectHTMLParent->GotoLink( m_pHtmlLink );
	}
}
