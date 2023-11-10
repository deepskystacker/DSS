/*
 *
 *	ChartLegend.cpp
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
#include "ChartLegend.h"
#include "ChartSerie.h"
#include "ChartCtrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartLegend::CChartLegend(CChartCtrl* pParent):CChartObject(pParent)
{
	CChartObject::SetColor(RGB(255,255,255));
	m_iFontSize = 100;
	m_strFontName = "Microsoft Sans Serif";

	m_bIsVisible = false;
	m_iTextHeigh = 0;
}

CChartLegend::~CChartLegend()
{
}

CSize CChartLegend::GetSize(CDC* pDC) const
{
	CSize LegendSize;
	CSize TextSize;

	if (!m_bIsVisible)
	{
		LegendSize.cx = LegendSize.cy = 0;
		return LegendSize;
	}

	CFont* pOldFont;
	CFont NewFont;
	NewFont.CreatePointFont(m_iFontSize,CA2CT(m_strFontName.c_str()),pDC);
	pOldFont = pDC->SelectObject(&NewFont);

	int Height = 4;		//Upper space
	int Width = 0;		
	int MaxTextWidth = 0;

	size_t SeriesCount = m_pParent->GetSeriesCount();
	int Drawn = 0;
	for (size_t i=0;i<SeriesCount;i++)
	{
		CChartSerie* pSerie = m_pParent->GetSerie(i);
		if ( (pSerie->GetName() == "") || !pSerie->IsVisible() )
			continue;

		Drawn++;
		CSize BitmapSize =pSerie->GetLegendSize();
		TextSize = pDC->GetTextExtent(pSerie->GetName().c_str());

		if (TextSize.cy>BitmapSize.cy)
			Height += TextSize.cy + 2;
		else
			Height += BitmapSize.cy + 2;

		if ( (TextSize.cx + BitmapSize.cx + 6)> MaxTextWidth)
			MaxTextWidth = TextSize.cx + BitmapSize.cx + 6;
	}

	if (!Drawn)
	{
		LegendSize.cx = LegendSize.cy = 0;
		return LegendSize;
	}

	Width += MaxTextWidth + 6;
	LegendSize.cx = Width;
	LegendSize.cy = Height+2;

	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	return LegendSize;
}

void CChartLegend::SetPosition(int LeftBorder, int TopBorder, CDC *pDC)
{
	CSize NewSize = GetSize(pDC);
	CRect NewRect;
	NewRect.top = TopBorder;
	NewRect.bottom = TopBorder + NewSize.cy;
	NewRect.left = LeftBorder;
	NewRect.right = LeftBorder + NewSize.cx;

	CChartObject::SetRect(NewRect);
}

void CChartLegend::Draw(CDC *pDC)
{
	if (!pDC->GetSafeHdc())
		return;
	if (!m_bIsVisible)
		return;

	// int YPos = 2;
	int MaxBitmapWidth = 0;
	bool bDraw = false;

	size_t SeriesCount = m_pParent->GetSeriesCount();
	size_t i=0;
	for (i=0;i<SeriesCount;i++)
	{
		CChartSerie* pSerie = m_pParent->GetSerie(i);
		if ( (pSerie->GetName() == "") || !pSerie->IsVisible() )
			continue;

		bDraw = true;
		CSize BitmapSize = pSerie->GetLegendSize();
		if (BitmapSize.cx > MaxBitmapWidth)
			MaxBitmapWidth = BitmapSize.cx;
	}

	if (!bDraw)
		return;

	CPen SolidPen(PS_SOLID,0,RGB(0,0,0));
	CPen* pOldPen;
	CFont* pOldFont;
	CFont NewFont;
	NewFont.CreatePointFont(m_iFontSize,CA2CT(m_strFontName.c_str()),pDC);

	//Fill back color
	CBrush m_BrushBack;
	m_BrushBack.CreateSolidBrush(m_ObjectColor) ;
	pDC->FillRect(m_ObjectRect,&m_BrushBack);
	COLORREF OldColor = pDC->SetBkColor(m_ObjectColor);

	pOldFont = pDC->SelectObject(&NewFont);
	pOldPen = pDC->SelectObject(&SolidPen);

	//Draw rectangle:
	pDC->MoveTo(m_ObjectRect.left,m_ObjectRect.top);
	pDC->LineTo(m_ObjectRect.right,m_ObjectRect.top);
	pDC->LineTo(m_ObjectRect.right,m_ObjectRect.bottom);
	pDC->LineTo(m_ObjectRect.left,m_ObjectRect.bottom);
	pDC->LineTo(m_ObjectRect.left,m_ObjectRect.top);

	CPoint UpperLeft;
	UpperLeft.x = m_ObjectRect.left;
	UpperLeft.y = m_ObjectRect.top + 4;
	for (i=0;i<SeriesCount;i++)
	{
		CChartSerie* pSerie = m_pParent->GetSerie(i);

		int NewHeight = pSerie->DrawLegend(pDC,UpperLeft,MaxBitmapWidth);
		UpperLeft.y += NewHeight+2;
	}

	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	pDC->SelectObject(pOldPen);
	DeleteObject(SolidPen);
	pDC->SetBkColor(OldColor);
}

