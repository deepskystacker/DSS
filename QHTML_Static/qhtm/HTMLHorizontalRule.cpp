/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLHorizontalRule.cpp
Owner:	russf@gipsysoft.com
Purpose:	Horizontal rule between paragraphs.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"
#include "HTMLHorizontalRuleSection.h"

CHTMLHorizontalRule::CHTMLHorizontalRule( CStyle::Align alg, int nSize, int nWidth, bool bNoShade, COLORREF crColor )
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
	, m_alg( alg )
	,	m_nSize( nSize )
	, m_nWidth( nWidth )
	, m_bNoShade( bNoShade )
	, m_crColor( crColor )
{
}

#ifdef _DEBUG
void CHTMLHorizontalRule::Dump() const
{
	TRACENL( _T("Horizontal Rule\n") );
	TRACENL( _T("\tAlignment (%s)\n"), GetStringFromAlignment( m_alg ) );
	TRACENL( _T("\tSize (%d)\n"), m_nSize );
	TRACENL( _T("\tWidth (%d)\n"), m_nWidth );
	TRACENL( _T("\tNo Shade (%d)\n"), m_bNoShade );
	TRACENL( _T("\tColor (%d)\n"), m_crColor );
}
#endif	//	_DEBUG


void CHTMLHorizontalRule::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	const int nDisplayWidth = psc->GetCurrentWidth();
	int nWidth = m_nWidth;

	if( nWidth < 0 )
	{
		// If we are measuring, and looking at the largest possible size,
		// return some small number, as it is irrelevant!
		if( psc->IsMeasuring() && nDisplayWidth == knFindMaximumWidth )
			nWidth = 1;
		else
			nWidth = int( float( nDisplayWidth ) / 100 * abs(nWidth) );
	}

	int nLeft = psc->GetLeftMargin();
	if( !psc->IsMeasuring() && nWidth != nDisplayWidth )
	{
		switch( m_alg )
		{
		case CStyle::algRight:
			nLeft = psc->GetRightMargin() - nWidth;
			break;

		case CStyle::algCentre:
			nLeft = psc->GetLeftMargin() + ( nDisplayWidth - nWidth ) / 2;
			break;
		}
	}

	// Size may need to be scaled, if we are not on a screen
	int nSize = psc->GetDC().ScaleY( m_nSize );

	CHTMLHorizontalRuleSection *psect = new CHTMLHorizontalRuleSection( psc->GetHTMLSection(), m_bNoShade, m_crColor );
	psc->AddSection( psect );

	psect->Set( nLeft, psc->GetCurrentYPos(), nLeft + nWidth, psc->GetCurrentYPos() + nSize );

	psc->CarriageReturn( false );

	psc->SetCurrentYPos( psc->GetNextYPos() + nSize );
}