/*
 *
 *	ChartAxisLabel.cpp
 *
 *	Written by Cédric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *	This code may be used for any non-commercial and commercial purposes in a compiled form.
 *	The code may be redistributed as long as it remains unmodified and providing that the 
 *	author name and this disclaimer remain intact. The sources can be modified WITH the author 
 *	consent only.
 *	
 *	This code is provided without any garanties. I cannot be held responsible for the damage or
 *	the loss of time it causes. Use it at your own risks
 *
 *	An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */

#include "stdafx.h"
#include "ChartAxisLabel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartAxisLabel::CChartAxisLabel(CChartCtrl* pParent, bool bHorizontal):CChartObject(pParent)
{
	m_bIsHorizontal = bHorizontal;

	m_iFontSize = 100;
	m_strFontName = "Microsoft Sans Serif";

	m_strLabelText = "";
}

CChartAxisLabel::~CChartAxisLabel()
{

}

CSize CChartAxisLabel::GetSize(CDC *pDC) const
{
	CSize LabelSize;
	LabelSize.cx = 0;
	LabelSize.cy = 0;

	if (!m_bIsVisible)
		return LabelSize;
	if (!pDC->GetSafeHdc())
		return LabelSize;
	if (m_strLabelText == "")
		return LabelSize;

	COLORREF OldColor = pDC->SetTextColor(m_ObjectColor);
	CFont NewFont;
	CFont* pOldFont;
	NewFont.CreatePointFont(m_iFontSize,CA2CT(m_strFontName.c_str()),pDC);
	pOldFont = pDC->SelectObject(&NewFont);

	LabelSize = pDC->GetTextExtent(m_strLabelText.c_str());
	LabelSize.cx += 4;
	LabelSize.cy += 4;
	if (!m_bIsHorizontal)
	{
		int Width = LabelSize.cy;
		int Height = LabelSize.cx;
		LabelSize.cx = Width;
		LabelSize.cy = Height;
	}
	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	pDC->SetTextColor(OldColor);

	return LabelSize;
}

void CChartAxisLabel::Draw(CDC *pDC)
{
	if (!m_bIsVisible)
		return;
	if (!pDC->GetSafeHdc())
		return;
	if (m_strLabelText == "")
		return;

	CFont NewFont;
	NewFont.CreatePointFont(m_iFontSize,CA2CT(m_strFontName.c_str()),pDC);
	CFont* pOldFont;

	if (!m_bIsHorizontal)
	{
		LOGFONT LogFont;
		NewFont.GetLogFont(&LogFont);
		LogFont.lfOrientation = 900;
		LogFont.lfEscapement = 900;

		CFont VertFont;
		VertFont.CreateFontIndirect(&LogFont);
		pOldFont = pDC->SelectObject(&VertFont);

		pDC->ExtTextOut(m_ObjectRect.left + 2,m_ObjectRect.top,
					ETO_CLIPPED,NULL,m_strLabelText.c_str(),NULL);

		pDC->SelectObject(pOldFont);
		DeleteObject(VertFont);
		DeleteObject(NewFont);
	}
	else
	{		
		pOldFont = pDC->SelectObject(&NewFont);
		pDC->ExtTextOut(m_ObjectRect.left,m_ObjectRect.top + 2,
					ETO_CLIPPED,NULL,m_strLabelText.c_str(),NULL);
		pDC->SelectObject(pOldFont);
		DeleteObject(NewFont);
	}

}

void CChartAxisLabel::SetPosition(int LeftBorder, int TopBorder, CDC *pDC)
{
	CSize NewSize = GetSize(pDC);
	CRect NewRect;
	NewRect.top = TopBorder;
	NewRect.bottom = TopBorder + NewSize.cy;
	NewRect.left = LeftBorder;
	NewRect.right = LeftBorder + NewSize.cx;

	CChartObject::SetRect(NewRect);
}
