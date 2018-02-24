/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLTableSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Table or Cell section.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLTableSection.h"

CHTMLTableSection::CHTMLTableSection( CHTMLSection * pParent, int border, COLORREF crBorderDark, COLORREF crBorderLight, CBackground &grad )
	: CHTMLSectionABC( pParent )
	, m_back( grad )
	, m_crBorderDark( crBorderDark )
	, m_crBorderLight ( crBorderLight )
	, m_nBorder( border )
{
}

CHTMLTableSection::~CHTMLTableSection()
{
}


void CHTMLTableSection::OnDraw( GS::CDrawContext &dc )
{
	WinHelper::CRect internalRect = *this;
	const int nScaledXBorder = dc.ScaleX(m_nBorder);
	// Leave room for the border
	internalRect.Inflate(-nScaledXBorder, dc.ScaleY(-m_nBorder));

	// Fill in all but the border
	if( m_back.IsSet() )
	{
//		internalRect.right++;
//		internalRect.bottom++;
		m_back.Draw( dc, internalRect );
	}

	//	A it of debugging code commented out on purpose!
	//dc.Rectangle( internalRect, RGB( 255, 0, 0 ) );

	// Now, draw the border, if applicable.
	// Light border is top/left
	// Dark border is bottom/right
	if (nScaledXBorder > 1)
	{
		WinHelper::CPoint p[6];
		p[0] = WinHelper::CPoint(left, bottom);
		p[1] = TopLeft();
		p[2] = WinHelper::CPoint(right, top);
		p[3] = WinHelper::CPoint(internalRect.right, internalRect.top);
		p[4] = internalRect.TopLeft();
		p[5] = WinHelper::CPoint(internalRect.left, internalRect.bottom);

		dc.PolyFillOutlined(p, 6, m_crBorderLight, m_crBorderLight);

		p[0] = WinHelper::CPoint(right, top);
		p[1] = BottomRight();
		p[2] = WinHelper::CPoint(left, bottom);
		p[3] = WinHelper::CPoint(internalRect.left, internalRect.bottom);
		p[4] = internalRect.BottomRight();
		p[5] = WinHelper::CPoint(internalRect.right, internalRect.top);

		dc.PolyFillOutlined(p, 6, m_crBorderDark, m_crBorderDark);
	}
	else if( nScaledXBorder == 1 )
	{
		WinHelper::CPoint p[3];
		p[0] = WinHelper::CPoint(left, bottom);
		p[1] = TopLeft();
		p[2] = WinHelper::CPoint(right, top);
		dc.PolyLine(p, 3, m_crBorderLight);

		p[0] = WinHelper::CPoint(right, top);;
		p[1] = BottomRight();
		p[2] = WinHelper::CPoint(left, bottom);
		dc.PolyLine(p, 3, m_crBorderDark);
	}			

#ifdef _DEBUG
	//Draw last in case the table has a background colour
	CHTMLSectionABC::OnDraw( dc );
#endif	//	_DEBUG
}
