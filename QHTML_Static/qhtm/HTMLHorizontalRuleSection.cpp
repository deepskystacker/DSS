/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLHorizontalRuleSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	Horizontal rule HTML section.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLHorizontalRuleSection.h"


CHTMLHorizontalRuleSection::CHTMLHorizontalRuleSection( CHTMLSection *pSectParent, bool bNoShade, COLORREF crColor )
	: CHTMLSectionABC( pSectParent )
	, m_bNoShade( bNoShade )
	, m_crColor( crColor )
{
	if( !bNoShade )
	{
		Transparent( true );
	}
}

CHTMLHorizontalRuleSection::~CHTMLHorizontalRuleSection()
{

}


void CHTMLHorizontalRuleSection::OnDraw( GS::CDrawContext &dc )
{
#ifdef DRAW_DEBUG
	CHTMLSectionABC::OnDraw( dc );
#endif	//	DRAW_DEBUG

	if( m_bNoShade )
	{
		dc.FillRect( *this, m_crColor );
	}
	else
	{
		//	REVIEW - russf - bogus bit of OS specific drawing *not* in the right place.
		if( Height() == 2 )
			::DrawEdge( dc.GetSafeHdc(), *this, BDR_SUNKENOUTER, BF_TOP | BF_BOTTOM );
		else
			::DrawEdge( dc.GetSafeHdc(), *this, BDR_SUNKENOUTER, BF_RECT );
	}
}
