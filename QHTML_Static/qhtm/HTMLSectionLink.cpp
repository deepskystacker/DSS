/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLSectionLink.cpp
Owner:	russf@gipsysoft.com
Author: rich@woodbridgeinternalmed.com
Purpose:	Hyperlink 'link' object, links all of the hyperlink sections
					together. Ensure they all highlight at the same time if the
					hyperlink is made up of multiple sections etc.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLSectionLink.h"
#include "HTMLSection.h"

void CHTMLSectionLink::OnMouseEnter()
{
	for (UINT i = 0; i < m_arrSections.GetSize(); ++i)
	{
		m_arrSections[i]->SetMouseInSection(true);
		m_arrSections[i]->ForceRedraw();
	}
}


void CHTMLSectionLink::OnMouseLeave()
{
	for (UINT i = 0; i < m_arrSections.GetSize(); ++i)
	{
		m_arrSections[i]->SetMouseInSection(false);
		m_arrSections[i]->ForceRedraw();
	}
}


void CHTMLSectionLink::Update()
{
	for (UINT i = 0; i < m_arrSections.GetSize(); ++i)
	{
		m_arrSections[i]->ForceRedraw();
	}
}


bool CHTMLSectionLink::GetRegion( HRGN &rgn ) const
{
	if( m_arrSections.GetSize() )
	{
		//
		//	Create it...
		rgn = CreateRectRgnIndirect( m_arrSections[ 0 ] );

		//
		//	Iterate over the rest of teh sections and combine them together
		for (UINT i = 1; i < m_arrSections.GetSize(); ++i)
		{
			WinHelper::CRect rc( *m_arrSections[ i ] );
			HRGN hrgn = CreateRectRgnIndirect( rc );
			CombineRgn( rgn, rgn, hrgn, RGN_OR );
			DeleteObject( hrgn );				
		}
		return true;
	}

	return false;
}


void CHTMLSectionLink::DrawFocus( GS::CDrawContext &dc )
{
	HRGN rgn;
	if( m_pParent->IsFocused() && GetRegion( rgn ) )
	{
		dc.DrawFocus( rgn );

		VAPI( DeleteObject( rgn ) );
	}
}


void CHTMLSectionLink::SetFocus( bool bHasFocus )
{
	m_bFocused = bHasFocus;
	for (UINT i = 0; i < m_arrSections.GetSize(); ++i)
	{
		m_arrSections[i]->SetMouseInSection( false );
		m_arrSections[i]->ForceRedraw();
	}
}


void CHTMLSectionLink::Activate()
{
	if( m_pParent )
	{
		m_pParent->GotoLink( this );
	}	
}


void CHTMLSectionLink::GetObjectRect( WinHelper::CRect &rcBounds ) const
{
	HRGN rgn;
	if( GetRegion( rgn ) )
	{
		GetRgnBox( rgn, rcBounds );
		VAPI( DeleteObject( rgn ) );
	}
	else
	{
		ASSERT( FALSE );
	}
}


void CHTMLSectionLink::GetPointDistance( const POINT &pt, int &nX, int &nY ) const
{
	for (UINT i = 0; i < m_arrSections.GetSize(); ++i)
	{
		const CSectionABC *pSect = m_arrSections[ i ];
		nY = min( abs( pSect->top - pt.y ), abs( pSect->bottom - pt.y ) );
		nX = min( abs( pSect->left - pt.x ), abs( pSect->right - pt.x ) );
	}

}