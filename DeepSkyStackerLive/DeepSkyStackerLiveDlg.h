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
	CDeepSkyStackerLiveDlg(bool bUseDarkTheme, CWnd* pParent = nullptr);	// standard constructor

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
	bool			    m_bUseDarkTheme;

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

CDeepSkyStackerLiveDlg* GetDSSLiveDlg(CWnd* pDialog);
void AddToLog(QString szString, BOOL bAddDateTime = FALSE, BOOL bBold = FALSE, BOOL bItalic = FALSE, COLORREF crColor = RGB(0, 0, 0));
void SetLastImage(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap, LPCTSTR szFileName = nullptr);
void SetStackedImage(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap);
void AdviseStackedImageSaved();
void AddImageToList(LPCTSTR szImage);
void ChangeImageStatusInList(LPCTSTR szImage, IMAGESTATUS status);
void ChangeImageInfoInCharts(LPCTSTR szFileName, STACKIMAGEINFO info);
void UpdateImageOffsetsInList(LPCTSTR szImage, double fdX, double fdY, double fAngle);
void SetFootprintInStackedImage(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4);
void AddScoreFWHMStarsToGraph(LPCTSTR szFileName, double fScore, double fFWHM, double fStars, double fSkyBackground);
void AddOffsetsAngleToGraph(LPCTSTR szFileName, double fdX, double fdY, double fAngle);
void UpdateLiveSettings();
void ResetEmailCount();
void PostSaveStackedImage();
void ShowResetEmailCountButton();
void FlashMainWindow();
