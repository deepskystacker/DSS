/*
 *
 *	ChartObject.h
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

#if !defined(AFX_CHARTOBJECT_H__6ED024F2_00D9_45D5_AB83_258EF0075288__INCLUDED_)
#define AFX_CHARTOBJECT_H__6ED024F2_00D9_45D5_AB83_258EF0075288__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>			
#include <afxwin.h>

class CChartCtrl;

class CChartObject  
{
	friend CChartCtrl;

public:
	CChartObject(CChartCtrl* pOwner)
	{
		m_ObjectRect.left = m_ObjectRect.right = 0;
		m_ObjectRect.top = m_ObjectRect.bottom = 0;
		m_ObjectColor = RGB(0,0,0);
		m_pParent = pOwner;
		m_bIsVisible = true;
	}
	virtual ~CChartObject()
	{
	}

	void SetVisible(bool bVisible)   { m_bIsVisible = bVisible; }
	bool IsVisible()  const         { return m_bIsVisible; }

	COLORREF GetColor() const        { return m_ObjectColor; }
	void SetColor(COLORREF NewColor) { m_ObjectColor = NewColor; }

	bool Clip(int& dX, int& dY)  const
	{
		bool bResult = true;

		if (dX>m_ObjectRect.right) 
		{
			dX = m_ObjectRect.right;
			bResult = false;
		}
		if (dX<m_ObjectRect.left)
		{
			dX = m_ObjectRect.left;
			bResult = false;
		}
		if (dY>m_ObjectRect.bottom)
		{
			dY = m_ObjectRect.bottom;
			bResult = false;
		}
		if (dY<m_ObjectRect.top)
		{
			dY = m_ObjectRect.top;
			bResult = false;
		}

		return bResult;
	}



protected:
    void SetRect(CRect NewTect)  { m_ObjectRect = NewTect; }
    
	CRect		m_ObjectRect;		// Size of the object
	COLORREF	m_ObjectColor;		// Color of the objet

	CChartCtrl*	m_pParent;			// Owner of the object
	bool        m_bIsVisible;

private:
	virtual void Draw(CDC* pDC) = 0;

};

#endif // !defined(AFX_CHARTOBJECT_H__6ED024F2_00D9_45D5_AB83_258EF0075288__INCLUDED_)
