/*
 *
 *	ChartGrid.cpp
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
#include "ChartGrid.h"
#include "ChartAxis.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartGrid::CChartGrid(CChartCtrl* pParent, CChartAxis* pAxis, bool bHoriz) : CChartObject(pParent)
{
	m_iTickCount = 0;
	m_bIsHorizontal = bHoriz;
	m_pParentAxis = pAxis;

	SetColor(RGB(128,128,128));
}

CChartGrid::~CChartGrid()
{

}

void CChartGrid::Draw(CDC *pDC)
{
	if (!m_bIsVisible)
		return;

	if (!pDC->GetSafeHdc() )
		return;

	CPen* pOldPen;
	CPen NewPen(PS_SOLID,0,m_ObjectColor);

	pOldPen = pDC->SelectObject(&NewPen);

	int ActuPosition = 0;
	for (int i=0;i<=m_iTickCount;i++)
	{
		ActuPosition = (int)m_pParentAxis->GetTickScreenPos(i);
		if (!m_bIsHorizontal)
		{
			int ActuX = m_ObjectRect.left;

			while (true)
			{
				if (!Clip(ActuX,ActuPosition))
					break;
				pDC->MoveTo(ActuX,ActuPosition);

				ActuX += 3;
				Clip(ActuX,ActuPosition);
				pDC->LineTo(ActuX,ActuPosition);

				ActuX += 3;
			}
		}
		else
		{
			int ActuY = m_ObjectRect.bottom;

			while (true)
			{
				if (!Clip(ActuPosition,ActuY))
					break;
				pDC->MoveTo(ActuPosition,ActuY);

				ActuY -= 3;
				Clip(ActuPosition,ActuY);
				pDC->LineTo(ActuPosition,ActuY);

				ActuY -= 3;
			}
		}

	}

	pDC->SelectObject(pOldPen);
	DeleteObject(NewPen);
}
