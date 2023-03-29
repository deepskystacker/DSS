/*
 *
 *	ChartSerie.cpp
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
 *	History:
 *		- 11/08/2006: Management of the auto axis now done in the axis. Series Register
 *					  Unregister themselves to their respective axes	
 *
 */

#include "stdafx.h"
#include "ChartSerie.h"
#include "ChartAxis.h"
#include "ChartCtrl.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartSerie::CChartSerie(CChartCtrl* pParent, int Type) : CChartObject(pParent)
{
	m_iLastDrawnPoint = 0;

	m_pHorizontalAxis = pParent->GetBottomAxis();
	m_pHorizontalAxis->RegisterSeries(this);
	m_pVerticalAxis = pParent->GetLeftAxis();
	m_pVerticalAxis->RegisterSeries(this);

	m_strSerieName = "";
	m_iSerieType = Type;
}

CChartSerie::~CChartSerie()
{
	m_pHorizontalAxis->UnregisterSeries(this);
	m_pVerticalAxis->UnregisterSeries(this);
}

void CChartSerie::AddPoint(double X, double Y)
{
	SChartPoint NewPoint;
	NewPoint.X = X;
	NewPoint.Y = Y;
	m_vPoints.push_back(NewPoint);

	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();

	CDC* pDC = m_pParent->GetDC();
	Draw(pDC);
	m_pParent->ReleaseDC(pDC);
}

void CChartSerie::ClearSerie()
{
	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();

	m_vPoints.clear();
	m_pParent->RefreshCtrl();
}


double CChartSerie::GetXPointValue(int PointIndex) const
{
	if (PointIndex >= (int)m_vPoints.size() )
		return 0;

	return m_vPoints[PointIndex].X;
}

double CChartSerie::GetYPointValue(int PointIndex) const
{
	if (PointIndex >= (int)m_vPoints.size())
		return 0;

	return m_vPoints[PointIndex].Y;
}

void CChartSerie::SetYPointValue(int PointIndex, double NewVal)
{
	if (PointIndex >= (int)m_vPoints.size())
		return;

	m_vPoints[PointIndex].Y = NewVal;

	m_pVerticalAxis->RefreshAutoAxis();
	m_pParent->RefreshCtrl();
}
	
void CChartSerie::SetXPointValue(int PointIndex, double NewVal)
{
	if (PointIndex >= (int)m_vPoints.size())
		return;

	m_vPoints[PointIndex].X = NewVal;

	m_pHorizontalAxis->RefreshAutoAxis();
	m_pParent->RefreshCtrl();
}



bool CChartSerie::GetSerieXMinMax(double &Min, double &Max) const
{
	if ( (m_vPoints.size()==0) || !IsVisible() ) 
		return false;

	Min = m_vPoints[0].X;
	Max = m_vPoints[0].X;
	for (int i=0;i<(int)m_vPoints.size();i++)
	{
		if (m_vPoints[i].X<Min)
			Min = m_vPoints[i].X;
		if (m_vPoints[i].X>Max)
			Max = m_vPoints[i].X;
	}
	return true;
}

bool CChartSerie::GetSerieYMinMax(double &Min, double &Max) const
{
	if (m_vPoints.size() == 0)
		return false;

	Min = m_vPoints[0].Y;
	Max = m_vPoints[0].Y;
	for (int i=0;i<(int)m_vPoints.size();i++)
	{
		if (m_vPoints[i].Y<Min)
			Min = m_vPoints[i].Y;
		if (m_vPoints[i].Y>Max)
			Max = m_vPoints[i].Y;
	}
	return true;
}

void CChartSerie::SetPoints(double *X, double *Y, int Count)
{
	m_vPoints.clear();
	for (int i=0;i<Count;i++)
	{
		SChartPoint NewPoint;
		NewPoint.X = X[i];
		NewPoint.Y = Y[i];
		m_vPoints.push_back(NewPoint);
	}

	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();

	//Do not refresh parent so if more series are added, we refresh just once and gain some
	// speed
//	m_pParent->RefreshCtrl();
}


bool CChartSerie::SetVerticalAxis(bool bSecond)
{
	if (bSecond)
	{		
		CChartAxis* pAxis = m_pParent->GetRightAxis();
		if (pAxis)
		{
			m_pVerticalAxis->UnregisterSeries(this);
			m_pVerticalAxis->RefreshAutoAxis();

			m_pVerticalAxis = pAxis;
			m_pVerticalAxis->RegisterSeries(this);
			m_pVerticalAxis->RefreshAutoAxis();

			return true;
		}
		else
			return false;
	}
	else
	{
		CChartAxis* pAxis = m_pParent->GetLeftAxis();
		if (pAxis)
		{
			m_pVerticalAxis->UnregisterSeries(this);
			m_pVerticalAxis->RefreshAutoAxis();

			m_pVerticalAxis = pAxis;
			m_pVerticalAxis->RegisterSeries(this);
			m_pVerticalAxis->RefreshAutoAxis();

			return true;
		}
		else
			return false;
	}
}

bool CChartSerie::SetHorizontalAxis(bool bSecond)
{
	if (bSecond)
	{
		CChartAxis* pAxis = m_pParent->GetTopAxis();
		if (pAxis)
		{
			m_pHorizontalAxis->UnregisterSeries(this);
			m_pHorizontalAxis->RefreshAutoAxis();

			m_pHorizontalAxis = pAxis;
			m_pHorizontalAxis->RegisterSeries(this);
			m_pHorizontalAxis->RefreshAutoAxis();

			return true;
		}
		else
			return false;
	}
	else
	{
		CChartAxis* pAxis = m_pParent->GetBottomAxis();
		if (pAxis)
		{
			m_pHorizontalAxis->UnregisterSeries(this);
			m_pHorizontalAxis->RefreshAutoAxis();

			m_pHorizontalAxis = pAxis;
			m_pHorizontalAxis->RegisterSeries(this);
			m_pHorizontalAxis->RefreshAutoAxis();

			return true;
		}
		else
			return false;
	}
}


double CChartSerie::XScreenToValue(long XScreenCoord) const
{
	return m_pHorizontalAxis->ScreenToValue(XScreenCoord);
}

double CChartSerie::YScreenToValue(long YScreenCoord) const
{
	return m_pVerticalAxis->ScreenToValue(YScreenCoord);
}

void CChartSerie::ValueToScreen(double XValue, double YValue, CPoint &ScreenPoint) const
{
	ScreenPoint.x = m_pHorizontalAxis->ValueToScreen(XValue);
	ScreenPoint.y = m_pVerticalAxis->ValueToScreen(YValue);
}

CChartLineSerie* CChartSerie::GetAsLine()
{
	ASSERT(m_iSerieType==stLineSerie);
	return reinterpret_cast<CChartLineSerie*>(this);
}

CChartPointsSerie* CChartSerie::GetAsPoints()
{
	ASSERT(m_iSerieType==stPointsSerie);
	return reinterpret_cast<CChartPointsSerie*>(this);
}
