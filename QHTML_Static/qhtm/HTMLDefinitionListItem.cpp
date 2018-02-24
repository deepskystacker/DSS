/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLListItem.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"

CHTMLDefinitionListItem::CHTMLDefinitionListItem( CDefaults *pDefaults, bool bTerm )
	: CHTMLDocument( pDefaults )
	, m_bTerm( bTerm )
{
}


bool CHTMLDefinitionListItem::IsEmpty() const
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


#ifdef _DEBUG
void CHTMLDefinitionListItem::Dump() const
{
	if( m_bTerm )
	{
		TRACENL( _T("Definition List TERM----------\n") );
	}
	else
	{
		TRACENL( _T("Definition List DECRIPTION----------\n") );
	}
	CHTMLDocument::Dump();
}
#endif	//	_DEBUG

