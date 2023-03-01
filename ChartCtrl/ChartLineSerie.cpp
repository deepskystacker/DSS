/*
 *
 *	ChartLineSerie.cpp
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
#include "ChartLineSerie.h"
#include "ChartCtrl.h"

#include "Math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartLineSerie::CChartLineSerie(CChartCtrl* pParent) : CChartSerie(pParent,stLineSerie)
{
	m_iLineWidth = 1;
	m_iPenStyle = PS_SOLID;
}

CChartLineSerie::~CChartLineSerie()
{

}


void CChartLineSerie::DrawAll(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	if (pDC->GetSafeHdc())
	{
		CPen NewPen(m_iPenStyle,m_iLineWidth,m_ObjectColor);
		CPen* pOldPen;

		pDC->SetBkMode(TRANSPARENT);
		//To have lines limited in the drawing rectangle :
		pDC->IntersectClipRect(m_ObjectRect);
		pOldPen = pDC->SelectObject(&NewPen);

		for (int i=0;i<(int)m_vPoints.size()-1;i++)
		{
			//We don't draw a line between the origin and the first point -> we must have
			// a least 2 points before begining drawing
			
			CPoint ScreenPoint;
			ValueToScreen(m_vPoints[i].X,m_vPoints[i].Y,ScreenPoint);
			pDC->MoveTo(ScreenPoint.x,ScreenPoint.y);

			ValueToScreen(m_vPoints[i+1].X,m_vPoints[i+1].Y,ScreenPoint);
			pDC->LineTo(ScreenPoint.x,ScreenPoint.y);
		}

		pDC->SelectClipRgn(NULL);
		pDC->SelectObject(pOldPen);
		DeleteObject(NewPen);
	}
}

void CChartLineSerie::Draw(CDC* pDC)
{
	if (!m_bIsVisible)
		return;

	if (pDC->GetSafeHdc())
	{
		CPen NewPen(m_iPenStyle,m_iLineWidth,m_ObjectColor);
		CPen* pOldPen;

		pDC->SetBkMode(TRANSPARENT);
		//To have lines limited in the drawing rectangle :
		pDC->IntersectClipRect(m_ObjectRect);
		pOldPen = pDC->SelectObject(&NewPen);

		//Draw all points that haven't been drawn yet
		for (m_iLastDrawnPoint;m_iLastDrawnPoint<(int)m_vPoints.size()-1;m_iLastDrawnPoint++)
		{
			//We don't draw a line between the origin and the first point -> we must have
			// a least 2 points before begining drawing
		//	if (m_vPoints<1)
		//		break;

			CPoint ScreenPoint;
			ValueToScreen(m_vPoints[m_iLastDrawnPoint].X,m_vPoints[m_iLastDrawnPoint].Y,ScreenPoint);
			pDC->MoveTo(ScreenPoint.x,ScreenPoint.y);

			ValueToScreen(m_vPoints[m_iLastDrawnPoint+1].X,m_vPoints[m_iLastDrawnPoint+1].Y,ScreenPoint);
			pDC->LineTo(ScreenPoint.x,ScreenPoint.y);
		}

		pDC->SelectClipRgn(NULL);
		pDC->SelectObject(pOldPen);
		DeleteObject(NewPen);
	}
}

CSize CChartLineSerie::GetLegendSize() const
{
	CSize LegendSize;
	LegendSize.cx = 9;
	LegendSize.cy = m_iLineWidth;

	return LegendSize;
}

int CChartLineSerie::DrawLegend(CDC *pDC, CPoint UpperLeft, int BitmapWidth) const
{
	if (m_strSerieName== "")
		return 0;

	//Draw Text
	int TextHeigh = pDC->GetTextExtent(m_strSerieName.c_str()).cy;
	pDC->ExtTextOut(UpperLeft.x+BitmapWidth+6,UpperLeft.y,ETO_CLIPPED,NULL,m_strSerieName.c_str(),NULL);

	//Draw line:
	CPen NewPen(m_iPenStyle,m_iLineWidth,m_ObjectColor);
	CPen* pOldPen = pDC->SelectObject(&NewPen);
	pDC->MoveTo(UpperLeft.x+3,UpperLeft.y+TextHeigh/2);
	pDC->LineTo(UpperLeft.x+BitmapWidth,UpperLeft.y+TextHeigh/2);
	pDC->SelectObject(pOldPen);
	DeleteObject(NewPen);

	return TextHeigh;
}
