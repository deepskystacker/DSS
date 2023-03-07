/*
 *
 *	ChartTitle.cpp
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
#include "ChartTitle.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartTitle::CChartTitle(CChartCtrl* pParent) : CChartObject(pParent)
{
	CChartObject::SetColor(RGB(0,0,0));
	m_iFontSize = 100;
	m_strFontName = "Microsoft Sans Serif";
}

CChartTitle::~CChartTitle()
{

}

void CChartTitle::AddString(std::string const& NewString)
{
	m_StringArray.push_back(NewString);
}

size_t CChartTitle::GetStringCount() const
{
	return m_StringArray.size();
}

string CChartTitle::GetString(size_t Index) const
{
	if ( (Index<0) || (Index>=m_StringArray.size()) )
		return "";
	return m_StringArray[Index];
}

void CChartTitle::RemoveAll()
{
	m_StringArray.clear();
}

void CChartTitle::Draw(CDC *pDC)
{
	if (!pDC->GetSafeHdc())
		return;
	if (!m_bIsVisible)
		return;

	CPen SolidPen(PS_SOLID,0,m_ObjectColor);
	CPen* pOldPen;
	CFont* pOldFont;
	CFont NewFont;
	NewFont.CreatePointFont(m_iFontSize, CA2CT(m_strFontName.c_str()), pDC);

	COLORREF OldColor = pDC->SetTextColor(m_ObjectColor);
	pOldFont = pDC->SelectObject(&NewFont);
	pOldPen = pDC->SelectObject(&SolidPen);

	//Draw all entries
	int YPos = 4;
	size_t TitleCount = m_StringArray.size();
	for (size_t i=0;i<TitleCount;i++)
	{
		//Draw Text
		int TextWidth = pDC->GetTextExtent(m_StringArray[i].c_str()).cx;
		int TextHeigh = pDC->GetTextExtent(m_StringArray[i].c_str()).cy;

		int XPos = m_ObjectRect.left + (int)fabs((m_ObjectRect.left-m_ObjectRect.right)/2.0) - TextWidth/2;
		pDC->ExtTextOut(XPos,m_ObjectRect.top+YPos,ETO_CLIPPED,NULL,m_StringArray[i].c_str(),NULL);

		YPos += TextHeigh + 2;
	}

	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	pDC->SelectObject(pOldPen);
	DeleteObject(SolidPen);
	pDC->SetTextColor(OldColor);
}

CSize CChartTitle::GetSize(CDC *pDC)
{
	CSize TitleSize;

	if (!m_bIsVisible)
	{
		TitleSize.cx = TitleSize.cy = 0;
		return TitleSize;
	}

	int Height = 4;		//Upper space
	CSize TextSize = 0;
	int MaxTextWidth = 0;

	size_t TitleCount = m_StringArray.size();
	if (TitleCount==0)
	{
		TitleSize.cx = TitleSize.cy = 0;
		return TitleSize;
	}

	CFont* pOldFont;
	CFont NewFont;
	NewFont.CreatePointFont(m_iFontSize, CA2CT(m_strFontName.c_str()), pDC);
	pOldFont = pDC->SelectObject(&NewFont);

	for (size_t i=0;i<TitleCount;i++)
	{
		TextSize = pDC->GetTextExtent(m_StringArray[i].c_str());
		Height += TextSize.cy + 2;
		if (TextSize.cx > MaxTextWidth)
			MaxTextWidth = TextSize.cx;
	}

	TitleSize.cx = MaxTextWidth + 2;
	TitleSize.cy = Height;

	m_ObjectRect.bottom = m_ObjectRect.top + Height;

	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	return TitleSize;
}

