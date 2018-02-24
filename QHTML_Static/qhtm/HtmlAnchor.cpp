/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLAnchor.cpp
Owner:	russf@gipsysoft.com
Author: rich@woodbridgeinternalmed.com
Purpose:	Hidden element that represents anchor tag data
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"


CHTMLAnchor::CHTMLAnchor()
	: CHTMLParagraphObject( CHTMLParagraphObject::knAnchor )
	, m_crLink( RGB(0,0,0) )
	, m_crHover( RGB(0,0,0) )
	, m_bTitleSet( false )
{
}

#ifdef _DEBUG
void CHTMLAnchor::Dump() const
{
	TRACENL( _T("Anchor\n") );
	if (m_strLinkName.GetLength())
	{
		TRACENL( _T("\tName: \t%s\n"), (LPCTSTR)m_strLinkName);
	}
	if (m_strLinkTarget.GetLength())
	{
		TRACENL( _T("\tTarget: \t%s\n"), (LPCTSTR)m_strLinkTarget);
	}
	if (m_strLinkTitle.GetLength() && m_bTitleSet )
	{
		TRACENL( _T("\tTitle: \t%s\n"), (LPCTSTR)m_strLinkTitle);
	}
	TRACENL( _T("\tColors not shown.\n" ) );
}
#endif	//	_DEBUG


void CHTMLAnchor::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	//
	// Add an entry to the parent section
	if( m_strLinkTarget.GetLength() )
	{
		CHTMLSectionLink *pSectionLink = psc->GetHTMLSection()->AddLink( m_strLinkTarget, m_strLinkTitle, m_strID, m_crLink, m_crHover, m_bTitleSet );
		psc->SetCurrentLink( pSectionLink );
	}
	else
	{
		psc->SetCurrentLink( NULL );
	}

	//
	// Handle named section...
	if( m_strLinkName.GetLength() )
	{
		psc->GetHTMLSection()->AddNamedSection( m_strLinkName, WinHelper::CPoint( psc->GetCurrentXPos(), psc->GetCurrentYPos() ) );
	}
}
