/*
 *
 *	ChartAxis.h
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

class CChartGrid;
class CChartSerie;
class CChartAxisLabel;
    
class CChartAxis : public CChartObject  
{
	friend CChartCtrl;
	friend CChartGrid;
	friend CChartSerie;

public:

	int GetPosition()
	{
		if (m_bIsHorizontal)
		{
			if (m_bIsSecondary)
				return 0;
			else
				return 100;
		}
		else
		{
			if (m_bIsSecondary)
				return 100;
			else
				return 0;
		}
	}

	void SetInverted(bool bNewValue);
	bool IsInverted() const  { return m_bIsInverted; }
	void SetLogarithmic(bool bNewValue) { m_bIsLogarithmic = bNewValue; }
	bool IsLogarithmic() const          { return m_bIsLogarithmic; }
	void SetAutomatic(bool bNewValue);  
	bool IsAutomatic()  const  { return m_bIsAutomatic; }

	void SetMinMax(double Minimum, double Maximum);
	void GetMinMax(double& Minimum, double& Maximum) const
	{
		Minimum = m_MinValue;
		Maximum = m_MaxValue;
	}

    long   ValueToScreen(double Value) const;
    double ScreenToValue(long ScreenVal) const;
  
	void	 SetTextColor(COLORREF NewColor)  { m_TextColor = NewColor; }
	COLORREF GetTextColor() const			  { return m_TextColor;		}

	void SetFont(int nPointSize, std::string const& strFaceName)
	{
		m_nFontSize = nPointSize;
		m_strFontName = strFaceName;
	}

	void   SetTickIncrement(bool bAuto, double Increment);
	double GetTickIncrement() const  { return m_TickIncrement; }

	CChartAxisLabel* GetLabel() const  { return m_pAxisLabel; }
	CChartGrid*		 GetGrid()	const  { return m_pAxisGrid;  }

	CChartAxis(CChartCtrl* pParent,bool bHoriz);
	virtual ~CChartAxis();

	void SetAutoMargin(bool bAuto, int iNewSize)
	{
		m_bAutoMargin = bAuto;
		m_iMarginSize = iNewSize;
	}

private:
	void PanAxis(long PanStart, long PanEnd);
	void SetZoomMinMax(double Minimum, double Maximum);
	void UndoZoom();

	void SetDecimals(int NewValue)  { m_DecCount = NewValue; }
	bool IsHorizontal() const  { return m_bIsHorizontal; }

    int  GetAxisLenght() const;
    void CalculateTicksIncrement();
	void CalculateFirstTick();
	void SetSecondary(bool bNewVal)  { m_bIsSecondary = bNewVal; }
	bool GetSecondary() const		 { return m_bIsSecondary; }

	void RefreshAutoAxis();
	void FormatValue(char* outBuffer, int precision, double value, double prevValue, bool havePrevValue) const;

	void SetAxisSize(CRect const& ControlRect, CRect const& MarginRect);
	int ClipMargin(CRect ControlRect,CRect& MarginRect,CDC* pDC);	// Allows to calculate the margin required to displayys ticks and text

	long GetTickScreenPos(int TickIndex) const;

	void Draw(CDC* pDC);

	// To register/Unregister series related to this axis
	void RegisterSeries(CChartSerie* pSeries);
	void UnregisterSeries(CChartSerie* pSeries);


	bool m_bIsHorizontal;	  // Indicates if this is an horizontal or vertical axis
	bool m_bIsInverted;		  // Indicates if the axis is inverted
	bool m_bIsAutomatic;      // Indicates if the axis is automatic
	bool m_bIsLogarithmic;    // Indicates if the axis is logarithmic

	bool m_bIsSecondary;	// If the axis is secondary, it will be positioned to 
                            // the right (vertical) or to the top (horizontal)
	double m_MaxValue;		// Maximum value on the axis
	double m_MinValue;		
	double m_UnzoomMin;		// Min and max values of the axis before it has been zoomed
	double m_UnzoomMax;		// (used when we unzoom the chart -> go back to previous state)

	bool   m_bAutoTicks;		// Specify if the tick increment is manual or automatic
	double m_TickIncrement;		// Indicates the space between ticks (in axis value) or for log axis the mult base between two ticks
	double m_FirstTickVal;

	unsigned int m_DecCount;	// Number of decimals to display
	int m_StartPos;			    // Start position of the axis 
	int m_EndPos;

	int  m_nFontSize;			
	std::string m_strFontName;

	CChartGrid*			m_pAxisGrid;
	CChartAxisLabel*	m_pAxisLabel;

	typedef std::list<CChartSerie*> SeriesList;
	SeriesList m_pRelatedSeries;		// List containing pointers to series related to this axis

	// The user can specify the size of the margin, instead of
	// having it calculated automatically
	bool m_bAutoMargin;
	int m_iMarginSize;

	COLORREF m_TextColor;
};
