#ifndef __GRAPHVIEWTAB_H__
#define __GRAPHVIEWTAB_H__

#pragma once

#include "afxwin.h"
#include "label.h"
#include <ControlPos.h>
#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "ChartCtrl.h"
#include "ChartPointsSerie.h"
// CGraphViewTab dialog

typedef enum tagPOINTTYPE
{
	PT_REFERENCE		= 1,
	PT_OK				= 2,
	PT_WRONG			= 3,
	PT_WARNING			= 4
}POINTTYPE;

typedef enum tagCHARTTYPE
{
	CT_SCORE			= 1,
	CT_FWHM				= 2,
	CT_STARS			= 3,
	CT_DX				= 4,
	CT_DY				= 5,
	CT_ANGLE			= 6,
	CT_SKYBACKGROUND	= 7
}CHARTTYPE;

class CChartSeries
{
public :
	CChartCtrl*			m_pChart;
	CChartSerie*		m_pMain;
	CChartSerie*		m_pReference;
	CChartSerie*		m_pOk;
	CChartSerie*		m_pWrong;
	CChartSerie*		m_pWarning;

private :
	BOOL		IsPointInSerie(double fX, double fY, CChartSerie * pSerie)
	{
		BOOL			bResult = FALSE;

		for (LONG i = (LONG)(pSerie->GetPointsCount())-1;i>=0 && !bResult;i--)
		{
			if (fX == pSerie->GetXPointValue(i) &&
				fY == pSerie->GetYPointValue(i))
				bResult = TRUE;
		};

		return bResult;
	};

public :
	CChartSeries()
	{
	};

	~CChartSeries()
	{
	};

	void	Init(CChartCtrl & Chart, COLORREF crColor)
	{
		m_pChart = &Chart;

		m_pMain = Chart.AddSerie(CChartSerie::stLineSerie);
		m_pMain->SetVerticalAxis(false);
		m_pMain->SetColor(crColor);

		m_pReference = Chart.AddSerie(CChartSerie::stPointsSerie);
		m_pWarning	 = Chart.AddSerie(CChartSerie::stPointsSerie);
		m_pOk		 = Chart.AddSerie(CChartSerie::stPointsSerie);
		m_pWrong	 = Chart.AddSerie(CChartSerie::stPointsSerie);

		m_pReference->SetColor(RGB(0, 180, 0));
		m_pOk->SetColor(RGB(0, 255, 0));
		m_pWrong->SetColor(RGB(255, 0, 0));
		m_pWarning->SetColor(RGB(255, 190, 75));

		CChartPointsSerie *		pPointSerie;

		pPointSerie = dynamic_cast<CChartPointsSerie *>(m_pReference);
		pPointSerie->SetVerticalAxis(false);
		pPointSerie->SetPointSize(11, 11);
		pPointSerie->SetPointType(CChartPointsSerie::ptRectangle);

		pPointSerie = dynamic_cast<CChartPointsSerie *>(m_pOk);
		pPointSerie->SetVerticalAxis(false);
		pPointSerie->SetPointSize(11, 11);
		pPointSerie->SetPointType(CChartPointsSerie::ptEllipse);

		pPointSerie = dynamic_cast<CChartPointsSerie *>(m_pWrong);
		pPointSerie->SetVerticalAxis(false);
		pPointSerie->SetPointSize(11, 11);
		pPointSerie->SetPointType(CChartPointsSerie::ptTriangle);

		pPointSerie = dynamic_cast<CChartPointsSerie *>(m_pWarning);
		pPointSerie->SetVerticalAxis(false);
		pPointSerie->SetPointSize(17, 17);
		pPointSerie->SetPointType(CChartPointsSerie::ptEllipse);
	};

	void	SetName(LPCTSTR szName)
	{
		if (m_pMain)
			m_pMain->SetName((LPCSTR)CT2CA(szName));
	};

	void	SetVisible(bool bShow)
	{
		m_pMain->SetVisible(bShow);
		m_pReference->SetVisible(bShow);
		m_pOk->SetVisible(bShow);
		m_pWrong->SetVisible(bShow);
		m_pWarning->SetVisible(bShow);
	};

	void	AddPoint(double fX, double fY)
	{
		m_pMain->AddPoint(fX, fY);
	};

	void	SetPoint(double fX, POINTTYPE ptType)
	{
		// First search the point in the Main serie
		BOOL				bFound = FALSE;
		double				fY;

		for (LONG i = (LONG)(m_pMain->GetPointsCount())-1;i>=0 && !bFound;i--)
		{
			if (m_pMain->GetXPointValue(i) == fX)
			{
				bFound = TRUE;
				fY = m_pMain->GetYPointValue(i);
				if (ptType == PT_REFERENCE)
				{
					if (!IsPointInSerie(fX, fY, m_pReference))
						m_pReference->AddPoint(fX, fY);
				}
				else if (ptType == PT_OK)
				{
					if (!IsPointInSerie(fX, fY, m_pOk))
						m_pOk->AddPoint(fX, fY);
				}
				else if (ptType == PT_WRONG)
				{
					if (!IsPointInSerie(fX, fY, m_pWrong))
						m_pWrong->AddPoint(fX, fY);
				}
				else if (ptType == PT_WARNING)
				{
					if (!IsPointInSerie(fX, fY, m_pWarning))
						m_pWarning->AddPoint(fX, fY);
				};
			};
		};
	};
};

class CGraphViewTab : public CDialog
{
	DECLARE_DYNAMIC(CGraphViewTab)
private :
	CControlPos			m_ControlPos;
	CButton				m_Score;
	CButton				m_FWHM;
	CButton				m_Stars;
	CButton				m_Offset;
	CButton				m_Angle;
	CButton				m_SkyBackground;
	CChartCtrl			m_Graph;

	CChartSeries		m_csScores;
	CChartSeries		m_csFWHM;
	CChartSeries		m_csStars;
	CChartSeries		m_csdX;
	CChartSeries		m_csdY;
	CChartSeries		m_csAngle;
	CChartSeries		m_csSkyBackground;

	std::vector<CString>	m_vFiles;

public:
	CGraphViewTab(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGraphViewTab();

	//void	CallHelp();

// Dialog Data
	enum { IDD = IDD_GRAPHS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected :
	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnScore();
	afx_msg void OnFWHM();
	afx_msg void OnStars();
	afx_msg void OnOffset();
	afx_msg void OnAngle();
	afx_msg void OnSkyBackground();

private :
	void ChangeVisibleGraph();

public :
	void	AddScoreFWHMStars(LPCTSTR szFileName, double fScore, double fFWHM, double fStars, double fSkyBackground);
	void	AddOffsetAngle(LPCTSTR szFileName, double fdX, double fdY, double fAngle);
	void	SetPoint(LPCTSTR szFileName, POINTTYPE ptType, CHARTTYPE ctType);
	void	ChangeImageInfo(LPCTSTR szFileName, STACKIMAGEINFO info);
};


#endif