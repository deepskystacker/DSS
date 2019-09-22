// DeepSkyStackerLiveDlg.h : header file
//

#pragma once

#include "MainBoard.h"
#include "LogTab.h"
#include "ImageView.h"
#include "ImageList.h"
#include "GraphView.h"
#include "Settings.h"

typedef enum tabDSSLIVETAB
{
	DLT_NONE				= 0,
	DLT_STACKEDIMAGE		= 1,
	DLT_LASTIMAGE			= 2,
	DLT_GRAPHS				= 3,
	DLT_IMAGELIST			= 4,
	DLT_LOG					= 5,
	DLT_SETTINGS			= 6
}DSSLIVETAB;

/* ------------------------------------------------------------------- */

// CDeepSkyStackerLiveDlg dialog
class CDeepSkyStackerLiveDlg : public CDialog
{
// Construction
public:
	CDeepSkyStackerLiveDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DEEPSKYSTACKERLIVE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private :
	DSSLIVETAB			m_CurrentTab;
	CMainBoard			m_MainBoard;
	CLogTab				m_Log;
	CImageViewTab		m_StackedImage;
	CImageViewTab		m_LastImage;
	CImageListTab		m_ImageList;
	CGraphViewTab		m_Graphs;
	CSettingsTab		m_Settings;

	void	UpdateTab();
	void	UpdateSizes();

public :
	void		SetCurrentTab(DSSLIVETAB Tab);

	CMainBoard & GetMainBoard()
	{
		return m_MainBoard;
	};

	DSSLIVETAB	GetCurrentTab()
	{
		return m_CurrentTab;
	};

	CLogTab &	GetLogTab()
	{
		return m_Log;
	};

	CImageViewTab &	GetLastImageTab()
	{
		return m_LastImage;
	};

	CImageViewTab &	GetStackedImageTab()
	{
		return m_StackedImage;
	};

	CImageListTab &	GetImageListTab()
	{
		return m_ImageList;
	};

	CGraphViewTab &	GetGraphsTab()
	{
		return m_Graphs;
	};

	CSettingsTab &	GetSettingsTab()
	{
		return m_Settings;
	};

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL	OnInitDialog();
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL	OnEraseBkgnd(CDC * pDC);
	afx_msg void	OnClose( );
	afx_msg	LRESULT OnHTMLHelp(WPARAM, LPARAM);

	void			OnHelp();
	void			CallHelp();

	DECLARE_MESSAGE_MAP()
};

/* ------------------------------------------------------------------- */

inline CDeepSkyStackerLiveDlg *	GetDSSLiveDlg(CWnd * pDialog)
{
	if (pDialog)
	{
		CWnd *						pParent = pDialog->GetParent();
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pParent);

		return pDlg;
	}
	else
	{
		CWnd *						pWnd = GetDSSLiveApp()->m_pMainWnd;
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);

		return pDlg;
	};
};

/* ------------------------------------------------------------------- */

inline	void	AddToLog(LPCTSTR szString, BOOL bAddDateTime = FALSE, BOOL bBold = FALSE, BOOL bItalic = FALSE, COLORREF crColor = RGB(0, 0, 0))
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetLogTab().AddToLog(szString, bAddDateTime, bBold, bItalic, crColor);
	};
};

/* ------------------------------------------------------------------- */

inline	void	SetLastImage(CMemoryBitmap * pBitmap, C32BitsBitmap * pWndBitmap, LPCTSTR szFileName = nullptr)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetLastImageTab().SetImage(pBitmap, pWndBitmap, szFileName);
	};
};

/* ------------------------------------------------------------------- */

inline	void	SetStackedImage(CMemoryBitmap * pBitmap, C32BitsBitmap * pWndBitmap)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetStackedImageTab().SetImage(pBitmap, pWndBitmap, nullptr);
	};
};

/* ------------------------------------------------------------------- */

inline	void	AdviseStackedImageSaved()
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetStackedImageTab().OnStackedImageSaved();
	};
};

/* ------------------------------------------------------------------- */

inline	void	AddImageToList(LPCTSTR szImage)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetImageListTab().AddImage(szImage);
	};
};

/* ------------------------------------------------------------------- */

inline	void	ChangeImageStatusInList(LPCTSTR szImage, IMAGESTATUS status)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetImageListTab().ChangeImageStatus(szImage, status);
	};
};

/* ------------------------------------------------------------------- */

inline void		ChangeImageInfoInCharts(LPCTSTR szFileName, STACKIMAGEINFO info)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetGraphsTab().ChangeImageInfo(szFileName, info);
	};
};

/* ------------------------------------------------------------------- */

inline	void	UpdateImageOffsetsInList(LPCTSTR szImage, double fdX, double fdY, double fAngle)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetImageListTab().UpdateImageOffsets(szImage, fdX, fdY, fAngle);
	};
};

/* ------------------------------------------------------------------- */

inline void		SetFootprintInStackedImage(CPointExt const& pt1, CPointExt const& pt2, CPointExt const& pt3, CPointExt const& pt4)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetStackedImageTab().OnSetFootprint(pt1, pt2, pt3, pt4);
	};
};

/* ------------------------------------------------------------------- */

inline void		AddScoreFWHMStarsToGraph(LPCTSTR szFileName, double fScore, double fFWHM, double fStars, double fSkyBackground)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetGraphsTab().AddScoreFWHMStars(szFileName, fScore, fFWHM, fStars, fSkyBackground);
	};
};

/* ------------------------------------------------------------------- */

inline void		AddOffsetsAngleToGraph(LPCTSTR szFileName, double fdX, double fdY, double fAngle)
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetGraphsTab().AddOffsetAngle(szFileName, fdX, fdY, fAngle);
	};
};

/* ------------------------------------------------------------------- */

inline void		UpdateLiveSettings()
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetMainBoard().UpdateLiveSettings();
	};
};

/* ------------------------------------------------------------------- */

inline void		ResetEmailCount()
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetMainBoard().ResetEmailCount();
	};
};

/* ------------------------------------------------------------------- */

inline void		PostSaveStackedImage()
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetMainBoard().PostSaveStackedImage();
	};
};

/* ------------------------------------------------------------------- */

inline void		ShowResetEmailCountButton()
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg *	pDlg = dynamic_cast<CDeepSkyStackerLiveDlg *>(pWnd);
		if (pDlg)
			pDlg->GetSettingsTab().ShowResetEmailCountButton();
	};
};

/* ------------------------------------------------------------------- */

inline void		FlashMainWindow()
{
	CWnd *			pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		pWnd->FlashWindowEx(FLASHW_ALL, 4, 0);
};

/* ------------------------------------------------------------------- */
