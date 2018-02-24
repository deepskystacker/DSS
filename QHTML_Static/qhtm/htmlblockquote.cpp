/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLBlockQuote.cpp
Owner:	rich@woodbridgeinternalmed.com
Purpose:	BlockQuote entity
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"
#include "defaults.h"

CHTMLBlockQuote::CHTMLBlockQuote( CDefaults *pDefaults )
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
	, m_pDoc( new CHTMLDocument( pDefaults ) )
{
}


CHTMLBlockQuote::~CHTMLBlockQuote()
{
	delete m_pDoc;
}

#ifdef _DEBUG
void CHTMLBlockQuote::Dump() const
{
	TRACENL( _T("BlockQuote----------------\n") );
	if (m_pDoc)
		m_pDoc->Dump();
}
#endif	//	_DEBUG


void CHTMLBlockQuote::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	/*
		A Blockquote contains a single element, a document. Just lay it out into
		smaller margins.
	*/
	const int nIndentWidth = WinHelper::MulDiv(g_defaults.m_nIndentSize + g_defaults.m_nIndentSpaceSize, GetDeviceCaps( psc->GetDC().GetSafeHdc(), LOGPIXELSX), 1000); 

	psc->NewParagraph( 1, 1, CStyle::algLeft );
	/* 
		Since there is the possibility that the margins exceed the page width,
		force the quote to be at least as wide as the margins!
	*/
	int nLeft = psc->GetCurrentXPos() + nIndentWidth;
	int nRight = psc->GetRightMargin() - nIndentWidth;
	if (nRight < nLeft)
		nRight = nLeft + nIndentWidth;

	CHTMLSectionCreator htCreate( psc->GetHTMLSection(), psc->GetDC(), psc->GetCurrentYPos(), nLeft, nRight, psc->GetBackgroundColor(), false, psc->GetZoomLevel(), psc->GetCurrentLink(), psc );
	htCreate.AddDocument( m_pDoc );
	const WinHelper::CSize size( htCreate.GetSize() );

	psc->SetCurrentYPos( size.cy + ( ( psc->GetDC().GetCurrentFontHeight() ) / 2) );

	psc->NewParagraph( 0, 0 );
}