/*
 *
 *	ChartPointsSerie.cpp
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
#include "ChartPointsSerie.h"
#include "Math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartPointsSerie::CChartPointsSerie(CChartCtrl* pParent) 
 : CChartSerie(pParent,stPointsSerie), m_iPointType(ptEllipse), m_iXPointSize(5),
   m_iYPointSize(5)
{

}

CChartPointsSerie::~CChartPointsSerie()
{

}

void CChartPointsSerie::Draw(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	if (pDC->GetSafeHdc())
	{
		CBrush		NewBrush(m_ObjectColor);
		CBrush*		pOldBrush;
		CPen		NewPen(PS_NULL, 1, RGB(0, 0, 0));
		CPen *		pOldPen;

		pDC->SetBkMode(TRANSPARENT);
		//To have lines limited in the drawing rectangle :
		pDC->IntersectClipRect(m_ObjectRect);
		pOldBrush = pDC->SelectObject(&NewBrush);
		pOldPen   = pDC->SelectObject(&NewPen);

		//Draw all points that haven't been drawn yet
		for (m_iLastDrawnPoint;m_iLastDrawnPoint<(int)m_vPoints.size();m_iLastDrawnPoint++)
		{
			CPoint ScreenPoint;
			ValueToScreen(m_vPoints[m_iLastDrawnPoint].X,m_vPoints[m_iLastDrawnPoint].Y,ScreenPoint);

			CRect PointRect;
			PointRect.SetRect(ScreenPoint.x-m_iXPointSize/2,ScreenPoint.y-m_iYPointSize/2,ScreenPoint.x+m_iXPointSize/2,ScreenPoint.y+m_iYPointSize/2);

			switch(m_iPointType)
			{
			case ptEllipse:
				pDC->Ellipse(PointRect);
				break;

			case ptRectangle:
				pDC->Rectangle(PointRect);
				break;

			case ptTriangle:
				{
					CPoint TrPoints[3];
					TrPoints[0].x = PointRect.left;
					TrPoints[0].y = PointRect.bottom;
					TrPoints[1].x = PointRect.right;
					TrPoints[1].y = PointRect.bottom;
					TrPoints[2].x = PointRect.left + (int)fabs((PointRect.left-PointRect.right)/2.0);
					TrPoints[2].y = PointRect.top;

					pDC->Polygon(TrPoints,3);
				}
				break;
			}
		}

		pDC->SelectClipRgn(NULL);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		DeleteObject(NewBrush);
	}
}

void CChartPointsSerie::DrawAll(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	CBrush		NewBrush(m_ObjectColor);
	CBrush*		pOldBrush;
	CPen		NewPen(PS_NULL, 1, RGB(0, 0, 0));
	CPen *		pOldPen;

	if (pDC->GetSafeHdc())
	{
		pDC->SetBkMode(TRANSPARENT);
		//To have lines limited in the drawing rectangle :
		pDC->IntersectClipRect(m_ObjectRect);
		pOldBrush = pDC->SelectObject(&NewBrush);
		pOldPen	  = pDC->SelectObject(&NewPen);

		for (int i=0;i<(int)m_vPoints.size();i++)
		{
			CPoint ScreenPoint;
			ValueToScreen(m_vPoints[i].X,m_vPoints[i].Y,ScreenPoint);

			CRect PointRect;
			PointRect.SetRect(ScreenPoint.x-m_iXPointSize/2,ScreenPoint.y-m_iYPointSize/2,ScreenPoint.x+m_iXPointSize/2,ScreenPoint.y+m_iYPointSize/2);

			switch(m_iPointType)
			{
			case ptEllipse:
				pDC->Ellipse(PointRect);
				break;

			case ptRectangle:
				pDC->Rectangle(PointRect);
				break;

			case ptTriangle:
				{
					CPoint TrPoints[3];
					TrPoints[0].x = PointRect.left;
					TrPoints[0].y = PointRect.bottom;
					TrPoints[1].x = PointRect.right;
					TrPoints[1].y = PointRect.bottom;
					TrPoints[2].x = PointRect.left + (int)fabs((PointRect.left-PointRect.right)/2.0);
					TrPoints[2].y = PointRect.top;

					pDC->Polygon(TrPoints,3);
				}
				break;
			}
		}

		pDC->SelectClipRgn(NULL);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		DeleteObject(NewBrush);
	}
}

CSize CChartPointsSerie::GetLegendSize() const
{
	CSize LegendSize;
	LegendSize.cx = m_iXPointSize;
	LegendSize.cy = m_iYPointSize;

	return LegendSize;
}

int CChartPointsSerie::DrawLegend(CDC *pDC, CPoint UpperLeft, int BitmapWidth) const
{
	if (m_strSerieName== "")
		return 0;

	//Draw Text
	int TextHeigh = pDC->GetTextExtent(m_strSerieName.c_str()).cy;
	pDC->ExtTextOut(UpperLeft.x+BitmapWidth+6,UpperLeft.y,ETO_CLIPPED,NULL,m_strSerieName.c_str(),NULL);

	CRect PointRect;
	if (TextHeigh > m_iYPointSize)
	{
		int Offset = (TextHeigh - m_iYPointSize)/2;
		PointRect.SetRect(UpperLeft.x+4,UpperLeft.y+Offset,UpperLeft.x+m_iXPointSize+4,UpperLeft.y+Offset+m_iYPointSize);
	}
	else
		PointRect.SetRect(UpperLeft.x+4,UpperLeft.y,UpperLeft.x+m_iXPointSize+4,UpperLeft.y+m_iYPointSize);

	CBrush NewBrush(m_ObjectColor);
	CBrush* pOldBrush = pDC->SelectObject(&NewBrush);

	switch(m_iPointType)
	{
	case ptEllipse:
		pDC->Ellipse(PointRect);
		break;

	case ptRectangle:
		pDC->Rectangle(PointRect);
		break;

	case ptTriangle:
		{
			CPoint TrPoints[3];
			TrPoints[0].x = PointRect.left;
			TrPoints[0].y = PointRect.bottom;
			TrPoints[1].x = PointRect.right;
			TrPoints[1].y = PointRect.bottom;
			TrPoints[2].x = PointRect.left + (int)fabs((PointRect.left-PointRect.right)/2.0);
			TrPoints[2].y = PointRect.top;

			pDC->Polygon(TrPoints,3);
		}
		break;
	}

	pDC->SelectObject(pOldBrush);
	DeleteObject(NewBrush);

	if (TextHeigh>m_iYPointSize)
		return TextHeigh;
	else
		return m_iYPointSize;
}
