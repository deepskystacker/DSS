// ChartSurfaceSerie.cpp: implementation of the CChartSurfaceSerie class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChartSurfaceSerie.h"
#include "ChartCtrl.h"
#include "ChartAxis.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartSurfaceSerie::CChartSurfaceSerie(CChartCtrl* pParent) 
 : CChartSerie(pParent,stSurfaceSerie), m_FillStyle(fsHatchDownDiag), m_bHorizontal(true)
{

}

CChartSurfaceSerie::~CChartSurfaceSerie()
{

}

void CChartSurfaceSerie::Draw(CDC* pDC)
{
	DrawAll(pDC);
}

void CChartSurfaceSerie::DrawAll(CDC* pDC)
{
	size_t count = m_vPoints.size();
	CPoint* pPoints = new CPoint[count+2];

	CBrush NewBrush;
	if (m_FillStyle == fsSolid)
		NewBrush.CreateSolidBrush(m_ObjectColor);
	else
	{
		int nIndex = 0;
		switch (m_FillStyle)
		{
		case fsHatchDownDiag:
			nIndex = HS_FDIAGONAL;
			break;
		case fsHatchUpDiag:
			nIndex = HS_BDIAGONAL;
			break;
		case fsHatchCross:
			nIndex = HS_CROSS;
			break;
		case fsHatchDiagCross:
			nIndex = HS_DIAGCROSS;
			break;
		case fsHatchHorizontal:
			nIndex = HS_HORIZONTAL;
			break;
		case fsHatchVertical:
			nIndex = HS_VERTICAL;
			break;
		}
		NewBrush.CreateHatchBrush(nIndex,m_ObjectColor);
	}

	CBrush* pOldBrush = pDC->SelectObject(&NewBrush);

	for (size_t i=0; i<count; i++)
	{
		ValueToScreen(m_vPoints[i].X,m_vPoints[i].Y,pPoints[i+1]);
	}

	if (m_bHorizontal)
	{
		pPoints[0].x = pPoints[1].x;
		pPoints[count+1].x = pPoints[count].x;

		float Position = m_pHorizontalAxis->GetPosition()/100.00;
		int AxisPos = m_ObjectRect.top + (int)(Position * (m_ObjectRect.bottom-m_ObjectRect.top));

		pPoints[0].y = AxisPos;
		pPoints[count+1].y = AxisPos;
	}
	else
	{
		pPoints[0].y = pPoints[1].y;
		pPoints[count+1].y = pPoints[count].y;

		float Position = m_pVerticalAxis->GetPosition()/100.00;
		int AxisPos = m_ObjectRect.left + (int)(Position * (m_ObjectRect.right-m_ObjectRect.left));

		pPoints[0].x = AxisPos;
		pPoints[count+1].x = AxisPos;
	}

	pDC->SetBkMode(TRANSPARENT);
	//To have lines limited in the drawing rectangle :
	pDC->IntersectClipRect(m_ObjectRect);

	pDC->Polygon(pPoints, (int)count+2);
	pDC->SelectClipRgn(NULL);
	pDC->SelectObject(pOldBrush);
	DeleteObject(NewBrush);

	delete[] pPoints;
}

int CChartSurfaceSerie::DrawLegend(CDC* pDC, CPoint UpperLeft, int BitmapWidth) const
{
	if (m_strSerieName== "")
		return 0;

	//Draw Text
	// int TextHeigh = pDC->GetTextExtent(m_strSerieName.c_str()).cy;
	pDC->ExtTextOut(UpperLeft.x+BitmapWidth+6,
					UpperLeft.y+1,
					ETO_CLIPPED,
					NULL,
					m_strSerieName.c_str(),
					NULL);

	// Draw the bitmap
	CBrush NewBrush;
	if (m_FillStyle == fsSolid)
		NewBrush.CreateSolidBrush(m_ObjectColor);
	else
	{
		int nIndex = 0;
		switch (m_FillStyle)
		{
		case fsHatchDownDiag:
			nIndex = HS_FDIAGONAL;
			break;
		case fsHatchUpDiag:
			nIndex = HS_BDIAGONAL;
			break;
		case fsHatchCross:
			nIndex = HS_CROSS;
			break;
		case fsHatchDiagCross:
			nIndex = HS_DIAGCROSS;
			break;
		case fsHatchHorizontal:
			nIndex = HS_HORIZONTAL;
			break;
		case fsHatchVertical:
			nIndex = HS_VERTICAL;
			break;
		}
		NewBrush.CreateHatchBrush(nIndex,m_ObjectColor);
	}

	CBrush* pOldBrush = pDC->SelectObject(&NewBrush);

	pDC->Rectangle(UpperLeft.x+2,UpperLeft.y+2,UpperLeft.x+17,UpperLeft.y+17);

	pDC->SelectObject(pOldBrush);
	DeleteObject(NewBrush);

	return 15;
}

CSize CChartSurfaceSerie::GetLegendSize() const
{
	CSize LegendSize;
	LegendSize.cx = 17;
	LegendSize.cy = 17;

	return LegendSize;
}

void CChartSurfaceSerie::AddPoint(double X, double Y)
{
	CChartSerie::AddPoint(X,Y);
	ReorderPoints();
}

void CChartSurfaceSerie::SetPoints(double *X, double *Y, int Count)
{
	CChartSerie::SetPoints(X,Y,Count);
	ReorderPoints();
}

void CChartSurfaceSerie::SetYPointValue(int PointIndex, double NewVal)
{
	CChartSerie::SetYPointValue(PointIndex,NewVal);
	ReorderPoints();
}

void CChartSurfaceSerie::SetXPointValue(int PointIndex, double NewVal)
{
	CChartSerie::SetXPointValue(PointIndex,NewVal);
	ReorderPoints();
}

void CChartSurfaceSerie::ReorderPoints()
{
	SPointSort NewSort;
	NewSort.m_bHorizontal = m_bHorizontal;
	std::sort(m_vPoints.begin(),m_vPoints.end(),NewSort);
}