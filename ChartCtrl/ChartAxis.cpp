/*
 *
 *	ChartAxis.cpp
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
 *	History:
 *		- 16/05/2006: Bug fix in ScreenToValue function
 *		- 18/05/2006: Added support for panning
 *		- 11/08/2006: Changes done for the automatic axis (Un/RegisterSeries, ...)
 *		- 12/08/2006: Tick increment can be set manually also 
 *
 */

#include "stdafx.h"
#include "ChartAxis.h"
#include "ChartAxisLabel.h"
#include "ChartGrid.h"
#include "ChartCtrl.h"
#include "ChartSerie.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartAxis::CChartAxis(CChartCtrl* pParent,bool bHoriz):CChartObject(pParent)
{
	m_bIsHorizontal = bHoriz;
	m_bIsInverted = false;
	m_bIsAutomatic = false;
	m_bIsLogarithmic = false;

	m_bIsSecondary = false;

	m_MaxValue = m_UnzoomMax = 10;
	m_MinValue = m_UnzoomMin = 0;

	m_bAutoTicks = true;
	m_TickIncrement = 1;
	m_FirstTickVal = 0;
	m_DecCount = 0;

	m_StartPos = m_EndPos = 0;

	m_nFontSize = 80;
	m_strFontName = "Microsoft Sans Serif";

	m_pAxisGrid = new CChartGrid(pParent,this,bHoriz);
	m_pAxisLabel = new CChartAxisLabel(pParent,bHoriz);

	m_bAutoMargin = true;
	m_iMarginSize = 0;

	m_TextColor = m_ObjectColor;
}

CChartAxis::~CChartAxis()
{
	if (m_pAxisGrid)
	{
		delete m_pAxisGrid;
		m_pAxisGrid = NULL;
	}
	if (m_pAxisLabel)
	{
		delete m_pAxisLabel;
		m_pAxisLabel = NULL;
	}
}	


int CChartAxis::ClipMargin(CRect ControlRect,CRect& MarginRect,CDC* pDC)
{
	if (!m_bIsVisible)
		return 0;

	int Size = 0;	

	CSize LabelSize = m_pAxisLabel->GetSize(pDC);
	CFont* pOldFont;
	CFont NewFont;
	NewFont.CreatePointFont(m_nFontSize,CA2CT(m_strFontName.c_str()),pDC);
	pOldFont = pDC->SelectObject(&NewFont);

	CString Buffer;
	Buffer.Format(_T("%.*f"),m_DecCount,m_MaxValue);
	if (m_bIsHorizontal)
	{
		CSize TextSize = pDC->GetTextExtent(Buffer);

		if (!m_bAutoMargin)
			Size = m_iMarginSize;
		else
		{
			Size += 4 + 2;		//Space above and under the text

			Size += TextSize.cy;
			Size += LabelSize.cy;

			m_iMarginSize = Size;
		}

		if (!m_bIsSecondary)
		{
			ControlRect.bottom -= Size;
			ControlRect.right -= TextSize.cx/2+3;

			if (ControlRect.bottom < MarginRect.bottom)
				MarginRect.bottom = ControlRect.bottom;
			if (ControlRect.right < MarginRect.right)
				MarginRect.right = ControlRect.right;
		}
		else
		{
			ControlRect.top += Size;
			ControlRect.right -= TextSize.cx/2+3;

			if (ControlRect.top > MarginRect.top)
				MarginRect.top = ControlRect.top;
			if (ControlRect.right < MarginRect.right)
				MarginRect.right = ControlRect.right;
		}

	}
	else
	{
        char szBuffer[255];
        if (!m_bIsLogarithmic)
        {
  
    		int MaxChars = abs( (int)log10(fabs(m_MaxValue) )) + 1;
    		int MinChars = abs( (int)log10(fabs(m_MinValue) )) + 1;
    		if (m_MinValue<0)
    			MinChars++;
    		if (m_MaxValue<0)
    			MaxChars++;
    		if (MaxChars>MinChars)
    			sprintf(szBuffer,"%.*f",m_DecCount,m_MaxValue);
    		else
    			sprintf(szBuffer,"%.*f",m_DecCount,m_MinValue);
       }
       else
       {
           char BuffMax[255];
           char BuffMin[255];
           int MaxDecCount = (int)log10(m_MaxValue);
           if (MaxDecCount < 0)
               MaxDecCount = -MaxDecCount;
           else
               MaxDecCount = 0;
           sprintf(BuffMax,"%.*f",MaxDecCount,m_MaxValue);
           
           int MinDecCount = (int)log10(m_MinValue);
           if (MinDecCount < 0)
                MinDecCount = -MinDecCount;
           else
               MinDecCount = 0;
           sprintf(BuffMin,"%.*f",MinDecCount,m_MinValue);
           
           if (strlen(BuffMin) > strlen(BuffMax) )
               strcpy(szBuffer,BuffMin);
           else
               strcpy(szBuffer,BuffMax);
       }

 		CSize TextSize = pDC->GetTextExtent(szBuffer);

		if (!m_bAutoMargin)
			Size = m_iMarginSize;
		else
		{
			Size += 7 + 1;		//Space before and after the text + Tick

			Size += TextSize.cx;
			Size += LabelSize.cx + 2;
			m_iMarginSize = Size;
		}

		if (!m_bIsSecondary)
		{
			ControlRect.left += Size;
			ControlRect.top += TextSize.cy/2+3;

			if (ControlRect.top > MarginRect.top)
				MarginRect.top = ControlRect.top;
			if (ControlRect.left > MarginRect.left)
				MarginRect.left = ControlRect.left;
		}
		else
		{
			ControlRect.right -= Size;
			ControlRect.top += TextSize.cy/2+3;

			if (ControlRect.top > MarginRect.top)
				MarginRect.top = ControlRect.top;
			if (ControlRect.right < MarginRect.right)
				MarginRect.right = ControlRect.right;
		}
	}

	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	return Size;
}

void CChartAxis::SetAutomatic(bool bNewValue)  
{ 
	m_bIsAutomatic = bNewValue; 
	if (m_bIsAutomatic)
		m_MinValue = m_MaxValue = 0;

	RefreshAutoAxis();
}

void CChartAxis::SetTickIncrement(bool bAuto, double Increment)
{
	m_bAutoTicks = bAuto;
	if (!m_bAutoTicks)
		m_TickIncrement = Increment;
	else
		CalculateTicksIncrement();

	CalculateFirstTick();
	m_pParent->RefreshCtrl();
}

void CChartAxis::SetAxisSize(CRect const& ControlRect, CRect const& MarginRect)
{
	if (m_bIsHorizontal)
	{
		m_StartPos = MarginRect.left;	
		m_EndPos = MarginRect.right;

		if (!m_bIsSecondary)
		{
			CRect AxisSize = ControlRect;
			AxisSize.top = MarginRect.bottom;
			SetRect(AxisSize);	
		}
		else
		{
			CRect AxisSize = ControlRect;
			AxisSize.bottom = MarginRect.top;
			SetRect(AxisSize);	
		}
	}
	else
	{
		m_StartPos = MarginRect.bottom;
		m_EndPos = MarginRect.top;

		if (!m_bIsSecondary)
		{
			CRect AxisSize = ControlRect;
			AxisSize.right = MarginRect.left;
			SetRect(AxisSize);
		}
		else
		{
			CRect AxisSize = ControlRect;
			AxisSize.left = MarginRect.right;
			SetRect(AxisSize);
		}
	}
}

void CChartAxis::Draw(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	if (pDC->GetSafeHdc() == NULL)
		return;

	CPen SolidPen(PS_SOLID,0,m_ObjectColor);
	CPen* pOldPen;

	CFont NewFont;
	CFont* pOldFont;
	COLORREF OldTextColor;

	CSize LabelSize = m_pAxisLabel->GetSize(pDC);
	int HalfAxisPos = (int)fabs((m_EndPos + m_StartPos)/2.0);
	int XPos = 0;
	int YPos = 0;
	if (m_bIsHorizontal)
	{
		if (!m_bIsSecondary)
		{
			CString Buffer;
			Buffer.Format(_T("%.*f"),m_DecCount,m_MaxValue);
			CSize TextSize = pDC->GetTextExtent(Buffer);
			
			YPos = m_ObjectRect.top  + TextSize.cy + 2;
			XPos = HalfAxisPos - LabelSize.cx/2;
		}
		else
		{
			YPos = m_ObjectRect.top  + 0;
			XPos = HalfAxisPos - LabelSize.cx/2;
		}
	}
	else
	{
		if (!m_bIsSecondary)
		{
			YPos = HalfAxisPos + LabelSize.cy/2;
			XPos = m_ObjectRect.left + 0;
		}
		else
		{
			YPos = HalfAxisPos + LabelSize.cy/2;
			XPos = m_ObjectRect.right - LabelSize.cx - 2;
		}
	}
	m_pAxisLabel->SetPosition(XPos,YPos,pDC);
	m_pAxisLabel->Draw(pDC);

	NewFont.CreatePointFont(m_nFontSize,CA2CT(m_strFontName.c_str()),pDC) ;
	pOldPen = pDC->SelectObject(&SolidPen);
	pOldFont = pDC->SelectObject(&NewFont);
	OldTextColor = pDC->SetTextColor(m_TextColor);

	CalculateTicksIncrement();

	char szBuffer[255];
	int TickCount = 0;
	int FirstTickPos = ValueToScreen(m_FirstTickVal);
    double TickValue = m_FirstTickVal;
	if (m_bIsHorizontal)
	{
		if (!m_bIsSecondary)
		{
			pDC->MoveTo(m_StartPos,m_ObjectRect.top+1);
			pDC->LineTo(m_EndPos,m_ObjectRect.top+1);
		}
		else
		{
			pDC->MoveTo(m_StartPos,m_ObjectRect.bottom-1);
			pDC->LineTo(m_EndPos,m_ObjectRect.bottom-1);
		}

		if (m_MinValue == m_MaxValue)
		{
			int HorizPos = m_StartPos + (int)fabs((m_EndPos-m_StartPos)/2.0);
			if (!m_bIsSecondary)
			{
				pDC->MoveTo(HorizPos,m_ObjectRect.top+1);
				pDC->LineTo(HorizPos,m_ObjectRect.top+4);
			}
			else
			{
				pDC->MoveTo(HorizPos,m_ObjectRect.bottom-1);
				pDC->LineTo(HorizPos,m_ObjectRect.bottom-4);
			}

            if (!m_bIsLogarithmic)
			   sprintf(szBuffer,"%.*f",m_DecCount,m_MinValue);
            else
            {
                int LogDecCount = (int)log10(m_MinValue);
                sprintf(szBuffer,"%.*f",LogDecCount,m_MinValue);
            }

			CSize TextSize;
			TextSize = pDC->GetTextExtent(szBuffer);

			if (!m_bIsSecondary)
				pDC->ExtTextOut(HorizPos-TextSize.cx/2,m_ObjectRect.top+5,ETO_CLIPPED|ETO_OPAQUE,NULL,szBuffer,NULL);
			else
				pDC->ExtTextOut(HorizPos-TextSize.cx/2,m_ObjectRect.bottom-5,ETO_CLIPPED|ETO_OPAQUE,NULL,szBuffer,NULL);
		}
		else while ( (TickValue<=m_MaxValue) || (TickValue-m_MaxValue < 0.0000001) )
		{
			int HorizPos = ValueToScreen(TickValue);
			if (m_bIsInverted)
			   FirstTickPos = HorizPos;

			if (!m_bIsSecondary)
			{
				pDC->MoveTo(HorizPos,m_ObjectRect.top+1);
				pDC->LineTo(HorizPos,m_ObjectRect.top+4);
			}
			else
			{
				pDC->MoveTo(HorizPos,m_ObjectRect.bottom-1);
				pDC->LineTo(HorizPos,m_ObjectRect.bottom-4);
			}

            if (!m_bIsLogarithmic)
			   sprintf(szBuffer,"%.*f",m_DecCount,TickValue);
            else
            {
                int LogDecCount = (int)log10(TickValue);
                if (LogDecCount<0)
                    LogDecCount = -(LogDecCount)-1;
                else
                    LogDecCount = 0;
                sprintf(szBuffer,"%.*f",LogDecCount,TickValue);
            }
			CSize TextSize = pDC->GetTextExtent(szBuffer);

			if (!m_bIsSecondary)
				pDC->ExtTextOut(HorizPos-TextSize.cx/2,m_ObjectRect.top+5,ETO_CLIPPED|ETO_OPAQUE,NULL,szBuffer,NULL);
			else
				pDC->ExtTextOut(HorizPos-TextSize.cx/2,m_ObjectRect.bottom-5-TextSize.cy,ETO_CLIPPED|ETO_OPAQUE,NULL,szBuffer,NULL);

            if (!m_bIsLogarithmic)
			   TickValue += m_TickIncrement;
            else
			   TickValue *= m_TickIncrement;
			TickCount++;
		}
	}
	else
	{
		if (!m_bIsSecondary)
		{
			pDC->MoveTo(m_ObjectRect.right-1,m_StartPos);
			pDC->LineTo(m_ObjectRect.right-1,m_EndPos);
		}
		else
		{
			pDC->MoveTo(m_ObjectRect.left+1,m_StartPos);
			pDC->LineTo(m_ObjectRect.left+1,m_EndPos);
		}

		if (m_MinValue == m_MaxValue)
		{
			int VertPos = m_EndPos + (int)fabs((m_EndPos-m_StartPos)/2.0);

			if (!m_bIsSecondary)
			{
				pDC->MoveTo(m_ObjectRect.right-1,VertPos);
				pDC->LineTo(m_ObjectRect.right-4,VertPos);
			}
			else
			{
				pDC->MoveTo(m_ObjectRect.left+1,VertPos);
				pDC->LineTo(m_ObjectRect.left+4,VertPos);
			}

            if (!m_bIsLogarithmic)
			   sprintf(szBuffer,"%.*f",m_DecCount,TickValue);
            else
            {
                int LogDecCount = (int)(log10(TickValue));
                sprintf(szBuffer,"%.*f",LogDecCount,TickValue);
            }			
			CSize TextSize = pDC->GetTextExtent(szBuffer);

			if (!m_bIsSecondary)
				pDC->ExtTextOut(LabelSize.cx+m_ObjectRect.left+2,VertPos-TextSize.cy/2,ETO_CLIPPED|ETO_OPAQUE,NULL,szBuffer,NULL);
			else
				pDC->ExtTextOut(m_ObjectRect.left+6,VertPos-TextSize.cy/2,ETO_CLIPPED|ETO_OPAQUE,NULL,szBuffer,NULL);
		}
		else while ( (TickValue<=m_MaxValue) || (TickValue-m_MaxValue < 0.0000001) )
		{
			int VertPos = ValueToScreen(TickValue);
			if (!m_bIsInverted)
			   FirstTickPos = VertPos;

			if (!m_bIsSecondary)
			{
				pDC->MoveTo(m_ObjectRect.right-1,VertPos);
				pDC->LineTo(m_ObjectRect.right-4,VertPos);
			}
			else
			{
				pDC->MoveTo(m_ObjectRect.left+1,VertPos);
				pDC->LineTo(m_ObjectRect.left+4,VertPos);
			}

            if (!m_bIsLogarithmic)
			   sprintf(szBuffer,"%.*f",m_DecCount,TickValue);
            else
            {
                int LogDecCount = (int)(log10(TickValue));
                if (LogDecCount<0)
                    LogDecCount = (int)fabs(LogDecCount*1.0)+1;
                else
                    LogDecCount = 0;
                sprintf(szBuffer,"%.*f",LogDecCount,TickValue);
            }
			CSize TextSize = pDC->GetTextExtent(szBuffer);

			if(!m_bIsSecondary)
				pDC->ExtTextOut(m_ObjectRect.left+LabelSize.cx+4,VertPos-TextSize.cy/2,ETO_CLIPPED|ETO_OPAQUE,NULL,szBuffer,NULL);
			else
				pDC->ExtTextOut(m_ObjectRect.left+6,VertPos-TextSize.cy/2,ETO_CLIPPED|ETO_OPAQUE,NULL,szBuffer,NULL);

            if (!m_bIsLogarithmic)
			   TickValue += m_TickIncrement;
            else
			   TickValue *= m_TickIncrement;
			TickCount++;
		}
	}

	CRect Size = m_pParent->GetPlottingRect();
	m_pAxisGrid->SetRect(Size);
	double TickSpace = 0;
	if (m_bIsLogarithmic)
	{
       m_TickIncrement = 10;
	   TickSpace = log10(m_TickIncrement) * GetAxisLenght()/(log10(m_MaxValue)-log10(m_MinValue));
    }
    else
        TickSpace = m_TickIncrement * GetAxisLenght()/(m_MaxValue-m_MinValue);
    m_pAxisGrid->SetTicksCount(TickCount);
	m_pAxisGrid->Draw(pDC);

	pDC->SelectObject(pOldPen);
	DeleteObject(SolidPen);
	pDC->SelectObject(pOldFont);
	DeleteObject(NewFont);
	pDC->SetTextColor(OldTextColor);
}


void CChartAxis::SetMinMax(double Minimum, double Maximum)
{
	if (Minimum > Maximum)
	{
		TRACE("Maximum axis value must be > minimum axis value");
		return;
	}

	m_MinValue = m_UnzoomMin = Minimum;
	m_MaxValue = m_UnzoomMax = Maximum;
}


void CChartAxis::SetInverted(bool bNewValue)
{
	m_bIsInverted = bNewValue;
}

int CChartAxis::GetAxisLenght() const
{
    int Length = (int)fabs( (m_EndPos-m_StartPos) * 1.0);
    return Length;
}

void CChartAxis::CalculateTicksIncrement()
{
	if (!m_bAutoTicks)
	{
		CalculateFirstTick();
		return;
	}

	//Calculate the appropriate TickSpace (1 tick every 30 pixel +/-)
	if (m_bIsLogarithmic)
	   m_TickIncrement = 10;
    else
    {
    	int PixelSpace;
    	if (m_bIsHorizontal)
    		PixelSpace = 25;
    	else
    		PixelSpace = 20;
    
    	int MaxTickNumber = (int)fabs((m_EndPos-m_StartPos)/PixelSpace * 1.0);
    
    	//Best Tick Space
    	double TempTickSpace = (m_MaxValue-m_MinValue)/MaxTickNumber;
    
    	// Calculate appropriate tickSpace (not rounded on 'strange values' but 
    	// on something like 1, 2 or 5*10^X  where X is optimalized for showing the most
    	// significant digits)
    	int Zeros = (int)floor(log10(TempTickSpace));
    	double MinTickSpace = pow(10.0,Zeros);
    
    	int Digits = 0;
    	if (Zeros<0)		//must set digits
    	{
    		Digits = (int)fabs(Zeros*1.0);
    	}
    
    	if (MinTickSpace>=TempTickSpace)
    	{
    		m_TickIncrement = MinTickSpace;
    		SetDecimals(Digits);
    	}
    	else if (MinTickSpace*2>=TempTickSpace)
    	{
    		m_TickIncrement = MinTickSpace*2;
    		SetDecimals(Digits);
    	}
    	else if (MinTickSpace*5>=TempTickSpace)
    	{
    		m_TickIncrement = MinTickSpace*5;
    		SetDecimals(Digits);
    	}
    	else if (MinTickSpace*10>=TempTickSpace)
    	{
    		m_TickIncrement = MinTickSpace*10;
    		if (Digits)
    			SetDecimals(Digits-1);
    		else
    			SetDecimals(Digits);
    	}
    }

	CalculateFirstTick();
}
	
void CChartAxis::CalculateFirstTick()
{
	if (m_bIsLogarithmic)
	{
        int LogBase = (int)log10(m_MinValue);
        m_FirstTickVal = pow(10.0,LogBase);
     }
    else
    {
    	m_FirstTickVal = 0;
    	if (m_TickIncrement!=0)
    	{
    		if (m_MinValue == 0)
    			m_FirstTickVal = 0;
    		else if (m_MinValue>0)
    		{
    			m_FirstTickVal = (int)(m_MinValue/m_TickIncrement) * m_TickIncrement;
    			while (m_FirstTickVal<m_MinValue)
    				m_FirstTickVal += m_TickIncrement;
    		}
    		else
    		{
    			m_FirstTickVal = (int)(m_MinValue/m_TickIncrement) * m_TickIncrement;
    			while (m_FirstTickVal>m_MinValue)
    				m_FirstTickVal -= m_TickIncrement;
    			if (!(m_FirstTickVal == m_MinValue))
    				m_FirstTickVal += m_TickIncrement;
    		}
    	}
     }
}

long CChartAxis::GetTickScreenPos(int TickIndex) const
{
	double TickVal = 0;
    if (!m_bIsLogarithmic)
		TickVal = m_FirstTickVal + TickIndex * m_TickIncrement;
	else
	   TickVal = m_FirstTickVal * pow(m_TickIncrement,TickIndex);
	return ValueToScreen(TickVal);
}

long CChartAxis::ValueToScreen(double Value) const
{
    long Offset = 0;
    if (m_bIsLogarithmic)
        Offset = (int)floor((log10(Value)-log10(m_MinValue)) * GetAxisLenght()/(log10(m_MaxValue)-log10(m_MinValue)) );
    else
    {
        Offset = (int)floor( (Value - m_MinValue) * GetAxisLenght()/(m_MaxValue-m_MinValue) );
    }
 
    if (m_bIsHorizontal)
    {
	   if (!m_bIsInverted)
		  return (m_StartPos + Offset);
	   else
		   return (m_EndPos - Offset);
     }
     else
     {
    	if (!m_bIsInverted)
    		return (m_StartPos - Offset);
    	else
    		return (m_EndPos + Offset);
     }
}

double CChartAxis::ScreenToValue(long ScreenVal) const
{
    int AxisOffset = 0;
    if (!m_bIsHorizontal)
    {
        if (m_bIsInverted)
			AxisOffset = ScreenVal - m_EndPos;      
        else
			AxisOffset = m_StartPos - ScreenVal;
            
    }
    else
    {
        if (!m_bIsInverted)
           AxisOffset = ScreenVal - m_StartPos;
        else
            AxisOffset = m_EndPos - ScreenVal;
    }

	if (!m_bIsLogarithmic)
       return ( (AxisOffset * 1.0 / GetAxisLenght()*(m_MaxValue-m_MinValue)) + m_MinValue);
    else
        return (pow(10.0,(AxisOffset *1.0 / GetAxisLenght()*(log10(m_MaxValue)-log10(m_MinValue)) ) + log10(m_MinValue)) );

}
     
void CChartAxis::PanAxis(long PanStart, long PanEnd)
{
	double StartVal = ScreenToValue(PanStart);
	double EndVal = ScreenToValue(PanEnd);

	if (!m_bIsLogarithmic)
	{
		double Shift = StartVal - EndVal;
		SetZoomMinMax(m_MinValue+Shift,m_MaxValue+Shift);
	}
	else
	{
		double Factor = StartVal/EndVal;
		SetZoomMinMax(m_MinValue*Factor,m_MaxValue*Factor);
	}
}

void CChartAxis::SetZoomMinMax(double Minimum, double Maximum)
{
	if (Minimum > Maximum)
	{
		TRACE("Maximum axis value must be > minimum axis value");
		return;
	}

	m_MinValue = Minimum;
	m_MaxValue = Maximum;
}

void CChartAxis::UndoZoom()
{
	SetMinMax(m_UnzoomMin,m_UnzoomMax);
}

void CChartAxis::RegisterSeries(CChartSerie* pSeries)
{
	// First check if the series is already present in the list
	SeriesList::iterator iter = m_pRelatedSeries.begin();
	for (iter; iter!=m_pRelatedSeries.end(); iter++)
	{
		if ( (*iter) == pSeries)
			return;
	}

	m_pRelatedSeries.push_back(pSeries);
}

void CChartAxis::UnregisterSeries(CChartSerie* pSeries)
{
	SeriesList::iterator iter = m_pRelatedSeries.begin();
	for (iter; iter!=m_pRelatedSeries.end(); iter++)
	{
		if ( (*iter) == pSeries)
		{
			m_pRelatedSeries.erase(iter);
			return;
		}
	}
}


void CChartAxis::RefreshAutoAxis()
{
	if (!m_bIsAutomatic)
		return;

	double	SeriesMin = 0;
	double	SeriesMax = 0;
	double	TempMin = 0;
	double	TempMax = 0;
	bool	bSet = false;

	
	SeriesList::iterator iter;

	for (iter = m_pRelatedSeries.begin(); iter!=m_pRelatedSeries.end(); iter++)
	{
		if ((*iter)->IsVisible())
		{
			if (m_bIsHorizontal)
				(*iter)->GetSerieXMinMax(TempMin,TempMax);
			else
				(*iter)->GetSerieYMinMax(TempMin,TempMax);

			if (!bSet)
			{
				SeriesMin = TempMin;
				SeriesMax = TempMax;
				bSet = true;
			}
			else
			{
				if (TempMin < SeriesMin)
					SeriesMin = TempMin;
				if (TempMax > SeriesMax)
					SeriesMax = TempMax;
			};
		};
	}

	if ( (SeriesMax>m_MaxValue) || (SeriesMin<m_MinValue) )
	{
		if (!m_bIsHorizontal)
		{
			double		Span = SeriesMax-SeriesMin;

			SeriesMax += Span * 0.10;
			SeriesMin -= Span * 0.10;
		};
		SetMinMax(SeriesMin,SeriesMax);
		m_pParent->RefreshCtrl();
	}

}