/*
 *
 *	ChartGrid.h
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

#if !defined(AFX_CHARTGRID_H__ECCBEFF4_2365_49CD_A865_F1B4DD8CA138__INCLUDED_)
#define AFX_CHARTGRID_H__ECCBEFF4_2365_49CD_A865_F1B4DD8CA138__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartObject.h"


class CChartAxis;

class CChartGrid : public CChartObject  
{
	friend CChartAxis;
public:
	CChartGrid(CChartCtrl* pParent,CChartAxis* pAxis, bool bHoriz);
	virtual ~CChartGrid();

private:
	void Draw(CDC* pDC);
	void SetTicksCount(int Count)  { m_iTickCount = Count; } 

	CChartAxis* m_pParentAxis;
	int			 m_iTickCount;
	bool         m_bIsHorizontal;
};

#endif // !defined(AFX_CHARTGRID_H__ECCBEFF4_2365_49CD_A865_F1B4DD8CA138__INCLUDED_)
