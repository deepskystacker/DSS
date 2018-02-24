/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLDefinitionList.cpp
Owner:	russf@gipsysoft.com
Purpose:	A List, and List Entries
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"
#include "defaults.h"

CHTMLDefinitionList::CHTMLDefinitionList()
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
{
}


CHTMLDefinitionList::~CHTMLDefinitionList()
{
	for( UINT nIndex = 0; nIndex < m_arrItems.GetSize(); nIndex++ )
	{
		delete m_arrItems[ nIndex ];
	}
	m_arrItems.RemoveAll();
}


void CHTMLDefinitionList::AddItem( CHTMLDefinitionListItem *pItem )
{
	m_arrItems.Add( pItem );
}


#ifdef _DEBUG
void CHTMLDefinitionList::Dump() const
{
	const size_t size = m_arrItems.GetSize();

	TRACENL( _T("Definition List----------------\n") );
	TRACENL( _T(" Size (%d)\n"), size );
	for( size_t nIndex = 0; nIndex < size; nIndex++ )
	{
		TRACENL( _T(" Item %d\n"), nIndex );
		m_arrItems[ nIndex ]->Dump();
	}
}
#endif	//	_DEBUG


void CHTMLDefinitionList::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	// Iterate the list items, creating the bullet and the subdocument.
	const size_t nItems = m_arrItems.GetSize();

	psc->SetNextYPos( psc->GetNextYPos() + psc->GetDC().GetCurrentFontHeight() );
	
	const int nIndentSpace = WinHelper::MulDiv( g_defaults.m_nDefinitionListDescriptionIndentSize, GetDeviceCaps( psc->GetDC().GetSafeHdc(), LOGPIXELSX), 1000); 

	for( size_t i = 0; i < nItems; ++i )
	{
		CHTMLDefinitionListItem* pItem = m_arrItems[i];

		//NewParagraph( 0,0 ,CHTMLParse::algLeft );
		int nLeft = psc->GetCurrentXPos();
		if( !pItem->m_bTerm )
			nLeft += nIndentSpace;
		

		// Now, create the sub-document
		CHTMLSectionCreator htCreate( psc->GetHTMLSection(), psc->GetDC(), psc->GetCurrentYPos(), nLeft, psc->GetRightMargin(), psc->GetBackgroundColor(), false, psc->GetZoomLevel(), psc->GetCurrentLink(), psc );
		htCreate.AddDocument( pItem );

		const WinHelper::CSize size( htCreate.GetSize() );

		// Adjust y-pos
		psc->SetCurrentYPos( size.cy + 1 );
	}

	psc->AddYPos( psc->GetDC().GetCurrentFontHeight() );
}