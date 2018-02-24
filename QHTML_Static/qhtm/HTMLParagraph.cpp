/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLParagraph.cpp
Owner:	russf@gipsysoft.com
Purpose:	A paragraph
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"

CHTMLParagraph::CHTMLParagraph( int nSpaceAbove, int nSpaceBelow, CStyle::Align alg )
	: CHTMLDocumentObject( CHTMLDocumentObject::knParagraph )
	, m_nSpaceAbove( nSpaceAbove )
	, m_nSpaceBelow( nSpaceBelow )
	, m_alg( alg )
{

}


CHTMLParagraph::~CHTMLParagraph()
{
	for( UINT n = 0; n < m_arrItems.GetSize(); n++ )
	{
		delete m_arrItems[ n ];
	}
	m_arrItems.RemoveAll();
}


void CHTMLParagraph::AddItem( CHTMLParagraphObject *pItem )
{
	pItem->SetParagraph( this );
	m_arrItems.Add( pItem );
}


void CHTMLParagraph::Reset( int nSpaceAbove, int nSpaceBelow, CStyle::Align alg )
{
	m_nSpaceAbove = nSpaceAbove;
	m_nSpaceBelow = nSpaceBelow;
	m_alg = alg;

	for( UINT n = 0; n < m_arrItems.GetSize(); n++ )
	{
		delete m_arrItems[ n ];
	}
	m_arrItems.RemoveAll();

}


bool CHTMLParagraph::IsEmpty() const
{
	if( m_arrItems.GetSize() == 0 )
		return true;

	if( m_arrItems.GetSize() == 1 )
	{
		const CHTMLParagraphObject *pItem = m_arrItems[ 0 ];
		return pItem->IsEmpty();
	}
	return false;
}


void CHTMLParagraph::ResetMeasuringKludge()
{
	const size_t uParaSize = m_arrItems.GetSize();
	for( size_t n = 0; n < uParaSize; n++ )
	{
		CHTMLParagraphObject *pItem = m_arrItems[ n ];
		pItem->ResetMeasuringKludge();
	}
}


void CHTMLParagraph::AddDisplayElements( class CHTMLSectionCreator * )
{
	//	We don't do anything here
}

#ifdef _DEBUG
void CHTMLParagraph::Dump() const
{
	TRACENL( _T("Paragraph ------------------------------------\n" ));
	TRACENL( _T("  LinesAbove(%d) LineBelow(%d)\n"), m_nSpaceAbove, m_nSpaceBelow );
	TRACENL( _T("  %s Aligment\n"), GetStringFromAlignment( m_alg ) );
	TRACENL( _T("  %d Items\n"), m_arrItems.GetSize() );
	for( size_t n = 0; n < m_arrItems.GetSize(); n++ )
	{
		m_arrItems[ n ]->Dump();
	}
}
#endif	//	_DEBUG
