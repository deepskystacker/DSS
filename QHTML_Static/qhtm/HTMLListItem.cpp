/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLListItem.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"

CHTMLListItem::CHTMLListItem( CDefaults *pDefaults, bool bBullet, LPCTSTR pcszFont, int nSize, bool bBold, bool bItalic, bool bUnderline, bool bStrikeout, CColor crFore, BYTE cCharSet )
	: CHTMLDocument( pDefaults )
	, m_bBullet( bBullet )
	, m_strFont( pcszFont )
	, m_nSize( nSize )
	, m_bBold( bBold )
	, m_bItalic( bItalic )
	, m_bUnderline( bUnderline )
	, m_bStrikeout( bStrikeout )
	, m_crFore( crFore )
	, m_bHasValue( false )
{
	m_cCharSet = cCharSet;
}


// This method is used to determine if any substance has been added to the 
// list item. List items are always created with a sigle paragraph.
// If there isone item, and it is empty, then the list item is empty.
bool CHTMLListItem::IsEmpty() const
{
	if (!m_arrItems.GetSize())
		return true;
	if (m_arrItems.GetSize() == 1)
	{
		// The first object is always a paragraph...
		ASSERT(m_arrItems[0]->GetType() == CHTMLDocumentObject::knParagraph);
		CHTMLParagraph* pPara = (CHTMLParagraph*)m_arrItems[0];
		return ( !pPara->m_arrItems.GetSize() );
	}
	return false;
}


void CHTMLListItem::SetValue( UINT uValue)
{
	m_bHasValue = true;
	m_uValue = uValue;
}


#ifdef _DEBUG
void CHTMLListItem::Dump() const
{
	TRACENL( _T("Table List Item----------\n") );
	TRACENL( _T(" Bullet:  %s\n"), (m_bBullet ? _T("true") : _T("false") ));
	TRACENL( _T("\tFont(%s)\n"), (LPCTSTR)m_strFont );
	TRACENL( _T("\tSize(%d)\n"), m_nSize );
	TRACENL( _T("\t bBold(%d)\n"), m_bBold );
	TRACENL( _T("\t bItalic(%d)\n"), m_bItalic );
	TRACENL( _T("\t bUnderline(%d)\n"), m_bUnderline );
	TRACENL( _T("\t crFore(%d)\n"), m_crFore );
	if( m_bHasValue )
	{
		TRACENL( _T("\t uValue(%d)\n"), m_uValue );
	}
	CHTMLDocument::Dump();
}
#endif	//	_DEBUG

