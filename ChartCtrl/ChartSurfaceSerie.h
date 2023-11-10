// ChartSurfaceSerie.h: interface for the CChartSurfaceSerie class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ChartSerie.h"

class CChartSurfaceSerie : public CChartSerie  
{
public:
	CChartSurfaceSerie(CChartCtrl* pParent);
	virtual ~CChartSurfaceSerie();

	enum FillStyle
	{
		fsSolid = 0,
		fsHatchDownDiag,
		fsHatchUpDiag,
		fsHatchCross,
		fsHatchDiagCross,
		fsHatchHorizontal,
		fsHatchVertical
	};

	void SetFillStyle(FillStyle NewStyle)  { m_FillStyle = NewStyle; }
	FillStyle GetFillStyle() const		   { return m_FillStyle; }

	void SetHorizontal(bool bHoriz) { m_bHorizontal = bHoriz; }
	bool GetHorizontal() const		{ return m_bHorizontal;   }

	virtual void AddPoint(double X, double Y);
	virtual void SetPoints(double *X, double *Y, int Count);

	virtual void SetYPointValue(int PointIndex, double NewVal);
	virtual void SetXPointValue(int PointIndex, double NewVal);	

private:

	struct SPointSort
	{
		bool operator()(SChartPoint& ptStart, SChartPoint& ptEnd)
		{
			if (m_bHorizontal)
				return ptStart.X < ptEnd.X;
			else
				return ptStart.Y < ptEnd.Y;
		}

		bool m_bHorizontal;
	};

	void ReorderPoints();

	int DrawLegend(CDC* pDC, CPoint UpperLeft, int BitmapWidth) const;
	CSize GetLegendSize() const;

	void DrawAll(CDC *pDC);
	void Draw(CDC* pDC);

	FillStyle m_FillStyle;
	// The surface serie can be either horizontal (the fill will be done between the curve 
	// and the corresponding horizontal axis) or vertical (the fill will be done between 
	// the curve and the corresponding vertical axis)
	bool m_bHorizontal;
};
