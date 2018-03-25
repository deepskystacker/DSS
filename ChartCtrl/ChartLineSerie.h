/*
 *
 *	ChartLineSerie.h
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

#if !defined(AFX_CHARTLINESERIE_H__792C2F20_9650_42FA_B13D_E63911C98CE5__INCLUDED_)
#define AFX_CHARTLINESERIE_H__792C2F20_9650_42FA_B13D_E63911C98CE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartSerie.h"


class CChartLineSerie : public CChartSerie  
{
public:
	int  GetPenStyle() const        { return m_iPenStyle; }
	void SetPenStyle(int NewStyle)  { m_iPenStyle = NewStyle; }

	int	 GetWidtth() const       { return m_iLineWidth; }
	void SetWidth(int NewValue)  { m_iLineWidth = NewValue; }

	CChartLineSerie(CChartCtrl* pParent);
	virtual ~CChartLineSerie();

private:
	int DrawLegend(CDC* pDC, CPoint UpperLeft, int BitmapWidth) const;
	CSize GetLegendSize() const;

	void Draw(CDC* pDC);
	void DrawAll(CDC *pDC);

	int m_iLineWidth;
	int m_iPenStyle;
};

#endif // !defined(AFX_CHARTLINESERIE_H__792C2F20_9650_42FA_B13D_E63911C98CE5__INCLUDED_)
