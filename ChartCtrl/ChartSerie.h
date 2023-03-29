/*
 *
 *	ChartSerie.h
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
#pragma once

#include "ChartObject.h"

class CChartLineSerie;
class CChartPointsSerie;
class CChartLegend;
class CChartAxis;
class CChartSerie : public CChartObject  
{
	friend CChartCtrl;
	friend CChartLegend;

public:
	CChartPointsSerie* GetAsPoints();
	CChartLineSerie*   GetAsLine();

	virtual void AddPoint(double X, double Y);
	virtual void SetPoints(double *X, double *Y, int Count);
	void ClearSerie();

	size_t GetPointsCount() const  { return m_vPoints.size(); }
	double GetYPointValue(int PointIndex) const;
	double GetXPointValue(int PointIndex) const;
	virtual void   SetYPointValue(int PointIndex, double NewVal);
	virtual void   SetXPointValue(int PointIndex, double NewVal);	
	
	bool GetSerieYMinMax(double& Min, double& Max)  const;
	bool GetSerieXMinMax(double& Min, double& Max)  const;

	void		SetName(std::string const& NewName) { m_strSerieName = NewName; }
	std::string GetName() const              { return m_strSerieName; }

	void ValueToScreen(double XValue, double YValue, CPoint& ScreenPoint)  const;
	double YScreenToValue(long YScreenCoord)  const;
	double XScreenToValue(long XScreenCoord)  const;

//	void RefreshAutoAxes();
	bool SetHorizontalAxis(bool bSecond);
	bool SetVerticalAxis(bool bSecond);

	CChartSerie(CChartCtrl* pParent,int Type);
	virtual ~CChartSerie();

	enum
	{
		stLineSerie=0,
		stPointsSerie=1,
		stSurfaceSerie=2
	};

protected:
    struct SChartPoint
    {
    	double X;
    	double Y;
    };
	std::vector<SChartPoint>   m_vPoints; 
	
	int m_iLastDrawnPoint;			// Index of the last point drawn

	CChartAxis* m_pVerticalAxis;    // Related vertical axis
	CChartAxis* m_pHorizontalAxis;  // Related horizontal axis

	std::string m_strSerieName;		// Name displayed in the legend
	int m_iSerieType;			    // Type of the serie (ligne, point, surface, ...)


private:
	virtual CSize GetLegendSize() const =0;
    virtual int   DrawLegend(CDC* pDC, CPoint UpperLeft, int BitmapWidth) const =0;


	virtual void Draw(CDC* pDC) =0;
	virtual void DrawAll(CDC *pDC) =0;
};
