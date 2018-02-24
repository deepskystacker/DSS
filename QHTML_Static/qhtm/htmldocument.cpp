/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLDocument.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML document.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include "HTMLParse.h"
#include "Defaults.h"
#include <reuse/MapIter.h>

CHTMLDocument::CHTMLDocument( CDefaults *pDefaults )
	: m_crLink( pDefaults->m_crLinkColour )
	, m_crLinkHover( pDefaults->m_crLinkHoverColour )
	, m_nLeftMargin( pDefaults->m_rcMargins.left )
	, m_nTopMargin( pDefaults->m_rcMargins.top )
	, m_nRightMargin( pDefaults->m_rcMargins.right )
	, m_nBottomMargin( pDefaults->m_rcMargins.bottom )
	, m_pimgBackground( NULL )
	, m_cCharSet( pDefaults->m_cCharSet )
	, m_pDefaults( pDefaults )
	, m_pCurrentPara( NULL )
	, m_uCurrentItemID( 0 )
	, m_mapFonts( 241 )
{

}


CHTMLDocument::~CHTMLDocument()
{
	size_t uChildCount = m_arrItems.GetSize();
	size_t n;
	for( n = 0; n < uChildCount; n++ )
	{
		delete m_arrItems[ n ];
	}

	// Delete the images
	for( MapIterClass<StringClass, CQHTMImageABC*> itr( m_mapImages ); !itr.EOL(); itr.Next() )
	{
#ifdef QHTM_BUILD_INTERNAL_IMAGING
		itr.GetValue()->Destroy();
#else
		if( itr.GetValue() )
		{
			itr.GetValue()->Destroy();
		}
#endif	//	QHTM_BUILD_INTERNAL_IMAGING
	}
	m_mapImages.RemoveAll();

	if( m_pimgBackground )
	{
		m_pimgBackground->Destroy();
		m_pimgBackground = NULL;
	}

	m_mapFonts.RemoveAll();

	size_t uFontCount = m_arrFontDefs.GetSize();
	for( n = 0; n < uFontCount; n++ )
	{
		delete m_arrFontDefs[ n ];
	}

	for( n = 0; n < m_arrForms.GetSize(); n++ )
	{
		delete m_arrForms[ n ];
	}
	
}

#ifdef _DEBUG
void CHTMLDocument::Dump() const
{
	TRACENL( _T("Document %d Items, %d Images, Margins( %d, %d, %d, %d )\n"), m_arrItems.GetSize(), m_mapImages.GetSize(), m_nLeftMargin, m_nTopMargin, m_nRightMargin, m_nBottomMargin );
	for( size_t n = 0; n < m_arrItems.GetSize(); n++ )
	{
		m_arrItems[ n ]->Dump();
	}
}
#endif	//	_DEBUG


void CHTMLDocument::AddItem( CHTMLDocumentObject *pdocobj )
{
	m_arrItems.Add( pdocobj );
}


void CHTMLDocument::AddParagraph( CHTMLParagraph *pPara )
{
	m_arrItems.Add( pPara );
	m_pCurrentPara = pPara;
}


CHTMLParagraph *CHTMLDocument::CurrentParagraph() const
{
	return m_pCurrentPara;
}


const HTMLFontDef * CHTMLDocument::GetFontDefIndex( const HTMLFontDef &def )
{
	size_t *pu = m_mapFonts.Lookup( def );
	if( !pu )
	{
		HTMLFontDef *pdef = new HTMLFontDef;
		*pdef = def;

		size_t u = m_arrFontDefs.GetSize();
		m_arrFontDefs.Add( pdef );
		m_mapFonts.SetAt( def, u );
		return pdef;
	}

	return m_arrFontDefs[ *pu ];
}


void CHTMLDocument::ResetMeasuringKludge()
{
	const size_t uDocumentSize = m_arrItems.GetSize();
	for( size_t n = 0; n < uDocumentSize; n++ )
	{
		CHTMLDocumentObject *pItem = m_arrItems[ n ];
		switch( pItem->GetType() )
		{
			case CHTMLDocumentObject::knParagraph:
			// A Table breaks the line like a <hr>
			// NewParagraph( 1, 0, CHTMLParse::algLeft );
			static_cast<CHTMLParagraph *>( pItem )->ResetMeasuringKludge();
			break;
		}
	}
}