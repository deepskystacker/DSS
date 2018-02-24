/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLTextSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	Simple drawn text object.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLTextSection.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTMLTextSection::CHTMLTextSection( CHTMLSection * pParent, LPCTSTR pcszText, size_t nLength, const GS::FontDef *pfdef, const CColor &crFore, const CColor &crBack )
	: CHTMLSectionABC( pParent )
	, m_str( pcszText, nLength )
	, m_pfdef( pfdef )
	, m_crFore( crFore )
	, m_crBack( crBack )
{
	m_pfdef->AddRef();
	Transparent( true );
	SetCharacterSet( m_pfdef->m_cCharSet );
}

CHTMLTextSection::~CHTMLTextSection()
{
	const_cast< GS::FontDef *>( m_pfdef )->Release();
}


void CHTMLTextSection::OnDraw( GS::CDrawContext &dc )
{
	if( m_crBack.IsSet() )
	{
		dc.FillRect( *this, m_crBack );
	}

	dc.SelectFont( *m_pfdef );
	if( IsLink() )
	{
		if( IsMouseInSection() )
		{
			dc.DrawText( left, top, m_str, m_str.GetLength(), LinkHoverColour() );
		}
		else
		{
			dc.DrawText( left, top, m_str, m_str.GetLength(), m_crFore );
		}
	}
	else
	{
		dc.DrawText( left, top, m_str, m_str.GetLength(), m_crFore );
		if( GetTipText() )
		{
			WinHelper::CRect rc( left, bottom - 1, right, bottom );
			dc.FillRect( rc, RGB( 190,190,190 ) );
		}
	}


	CHTMLSectionABC::OnDraw( dc );
}
