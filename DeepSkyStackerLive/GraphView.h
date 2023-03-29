#pragma once
#include "ControlPos.h"
#include "ChartCtrl.h"
#include "Resource.h"
#include "LiveEngine.h"

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

class CChartCtrl;
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
	BOOL IsPointInSerie(double fX, double fY, CChartSerie* pSerie);

public :
    CChartSeries()
    {
        m_pChart = nullptr;
        m_pMain = nullptr;;
        m_pReference = nullptr;;
        m_pOk = nullptr;;
        m_pWrong = nullptr;;
        m_pWarning = nullptr;;
    }

	~CChartSeries()
	{
	};

	void Init(CChartCtrl& Chart, COLORREF crColor);
	void SetName(LPCTSTR szName);
	void SetVisible(bool bShow);
	void AddPoint(double fX, double fY);
	void SetPoint(double fX, POINTTYPE ptType);
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

	bool m_bDarkMode;
public:
	CGraphViewTab(CWnd* pParent = nullptr, bool bDarkMode = false);   // standard constructor
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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private :
	void ChangeVisibleGraph();

public :
	void	AddScoreFWHMStars(LPCTSTR szFileName, double fScore, double fFWHM, double fStars, double fSkyBackground);
	void	AddOffsetAngle(LPCTSTR szFileName, double fdX, double fdY, double fAngle);
	void	SetPoint(LPCTSTR szFileName, POINTTYPE ptType, CHARTTYPE ctType);
	void	ChangeImageInfo(LPCTSTR szFileName, STACKIMAGEINFO info);
};
