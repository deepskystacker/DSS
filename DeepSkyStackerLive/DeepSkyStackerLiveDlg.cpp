// DeepSkyStackerLiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStackerLiveDlg.h"
#include "resource.h"
#include "DeepSkyStackerLive.h"
#include "Workspace.h"

// CDeepSkyStackerLiveDlg dialog


/* ------------------------------------------------------------------- */

CDeepSkyStackerLiveDlg::CDeepSkyStackerLiveDlg(bool bUseDarkTheme, CWnd* pParent /*=nullptr*/)
	: CDialog(CDeepSkyStackerLiveDlg::IDD, pParent),
	m_StackedImage(nullptr, bUseDarkTheme),
	m_LastImage(nullptr, bUseDarkTheme),
	m_Graphs(nullptr, bUseDarkTheme),
	m_MainBoard(nullptr, bUseDarkTheme),
	m_Log(nullptr, bUseDarkTheme),
	m_Settings(nullptr, bUseDarkTheme),
	m_bUseDarkTheme(bUseDarkTheme)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_CurrentTab	= DLT_LOG;
}

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CDeepSkyStackerLiveDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_HELP, OnHTMLHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

BOOL CDeepSkyStackerLiveDlg::OnEraseBkgnd([[maybe_unused]] CDC * pDC)
{
	return 1;
};

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::UpdateTab()
{
	m_StackedImage.ShowWindow((m_CurrentTab == DLT_STACKEDIMAGE) ? SW_SHOW : SW_HIDE);
	m_LastImage.ShowWindow((m_CurrentTab == DLT_LASTIMAGE) ? SW_SHOW : SW_HIDE);
	m_Log.ShowWindow((m_CurrentTab == DLT_LOG) ? SW_SHOW : SW_HIDE);
	m_ImageList.ShowWindow((m_CurrentTab == DLT_IMAGELIST) ? SW_SHOW : SW_HIDE);
	m_Graphs.ShowWindow((m_CurrentTab == DLT_GRAPHS) ? SW_SHOW : SW_HIDE);
	m_Settings.ShowWindow((m_CurrentTab == DLT_SETTINGS) ? SW_SHOW : SW_HIDE);
	m_MainBoard.InvalidateRect(nullptr);
};

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::UpdateSizes()
{
	// Resize the tab control
	CRect			rcDlg;
	CRect			rcMainBoard;

	GetClientRect(&rcDlg);

	if (m_MainBoard.m_hWnd)
	{
		CRect		rcTemp;

		m_MainBoard.GetClientRect(&rcTemp);

		rcMainBoard = rcDlg;
		rcDlg.top += rcTemp.Height();
		rcMainBoard.bottom = rcDlg.top;

		m_MainBoard.MoveWindow(&rcMainBoard);

		if (m_Log.m_hWnd)
			m_Log.MoveWindow(&rcDlg);
		if (m_StackedImage.m_hWnd)
			m_StackedImage.MoveWindow(&rcDlg);
		if (m_LastImage.m_hWnd)
			m_LastImage.MoveWindow(&rcDlg);
		if (m_ImageList.m_hWnd)
			m_ImageList.MoveWindow(&rcDlg);
		if (m_Graphs.m_hWnd)
			m_Graphs.MoveWindow(&rcDlg);
		if (m_Settings.m_hWnd)
			m_Settings.MoveWindow(&rcDlg);
	};
};

/* ------------------------------------------------------------------- */
// CDeepSkyStackerLiveDlg message handlers

BOOL CDeepSkyStackerLiveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString			strMask;
	CString			strTitle;

	GetWindowText(strMask);
	strTitle.Format(strMask, _T(VERSION_DEEPSKYSTACKER));
	SetWindowText(strTitle);

	m_MainBoard.Create(IDD_MAINBOARD, this);
	m_Log.Create(IDD_LOG, this);
	m_StackedImage.SetStackedImage(TRUE);
	m_StackedImage.Create(IDD_IMAGEVIEW, this);
	m_LastImage.SetStackedImage(FALSE);
	m_LastImage.Create(IDD_IMAGEVIEW, this);
	m_ImageList.Create(IDD_IMAGELIST, this);
	m_ImageList.SetToDarkMode(m_bUseDarkTheme);
	m_Graphs.Create(IDD_GRAPHS, this);
	m_Settings.Create(IDD_SETTINGS_LIVE, this);

	{
		QSettings settings;
		settings.beginGroup("DeepSkyStackerLive");
		std::uint32_t dwMaximized { settings.value("Maximized", 0).toUInt() };
		CString strLeft{ settings.value("Left", "").toString().toStdWString().c_str() };
		CString strTop{ settings.value("Top", "").toString().toStdWString().c_str() };
		std::uint32_t dwWidth { settings.value("Width", 0).toUInt() };
		std::uint32_t dwHeight{ settings.value("Height", 0).toUInt() };
		settings.endGroup();

		if (strTop.GetLength() && strLeft.GetLength() && dwWidth && dwHeight)
		{
			WINDOWPLACEMENT		wp;

			memset(&wp, 0, sizeof(wp));
			wp.length  = sizeof(wp);
			wp.flags   = 0;
			wp.showCmd = dwMaximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
			wp.rcNormalPosition.left   = _tstol(strLeft);
			wp.rcNormalPosition.top    = _tstol(strTop);
			wp.rcNormalPosition.right  = wp.rcNormalPosition.left+dwWidth;
			wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+dwHeight;

			SetWindowPlacement(&wp);
		};
	};


	m_CurrentTab = DLT_LOG;
	UpdateTab();
	UpdateSizes();

	m_Log.AddToLog(QString::fromStdWString(strTitle.GetString()), TRUE, TRUE);
	m_Log.AddToLog("\n");

	m_Log.AddToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"\nHow to use  DeepSkyStacker Live ? \n","IDS_LOG_STARTING"),
		FALSE, TRUE, FALSE);
	m_Log.AddToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"Step 1\nCheck the Settings tabs for all the stackingand warning settings\n\n", "IDS_LOG_STARTING_1"),
		FALSE, FALSE, FALSE);
	m_Log.AddToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"Step 2\nClick on the Monitor button to start monitoring the folder\n"
		"When the monitoring is on incoming images are only registered but not stacked.\n\n", "IDS_LOG_STARTING_2"),
		FALSE, FALSE, FALSE);
	m_Log.AddToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"Step 3\nTo start stacking the images click on the Stack button\n"
		"At this point all the incoming(and previously registered) images will be stacked.\n", "IDS_LOG_STARTING_3"),
		FALSE, FALSE, FALSE);
	m_Log.AddToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"You can pause/start again the stacking process by clicking on the Stack button.\n", "IDS_LOG_STARTING_4"),
		FALSE, FALSE, FALSE);
	m_Log.AddToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"To stop the monitoring and stacking click on the Stop button.\n\n", "IDS_LOG_STARTING_5"),
		FALSE, FALSE, FALSE);

	{
		Workspace			workspace;

		TCHAR				szPath[_MAX_PATH];
		CString				strPath;

		//
		// Read the DSSLive setting file from the folder %AppData%/DeepSkyStacker/DeepSkyStacker5
		//
		SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, szPath);

		strPath = szPath;
		strPath += "\\DeepSkyStacker\\DeepSkyStacker5\\DSSLive.settings";
		workspace.ReadFromFile(strPath);
	};

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::OnClose( )
{
	m_ImageList.Close();

	{
		std::uint32_t dwMaximized = 0;
		CString		strTop = "";
		CString		strLeft = "";
		std::uint32_t dwWidth = 0;
		std::uint32_t dwHeight = 0;

		QSettings settings;

		WINDOWPLACEMENT		wp;

		memset(&wp, 0, sizeof(wp));
		wp.length  = sizeof(wp);

		GetWindowPlacement(&wp);
		dwMaximized = (wp.showCmd == SW_SHOWMAXIMIZED);
		strLeft.Format(_T("%ld"), wp.rcNormalPosition.left);
		strTop.Format(_T("%ld"), wp.rcNormalPosition.top);

		dwWidth  = wp.rcNormalPosition.right-wp.rcNormalPosition.left;
		dwHeight = wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;

		settings.beginGroup("DeepSkyStackerLive");
		settings.setValue("Maximized", dwMaximized);
		settings.setValue("Left", QString::fromStdWString(strLeft.GetString()));
		settings.setValue("Top", QString::fromStdWString(strTop.GetString()));
		settings.setValue("Width", dwWidth);
		settings.setValue("Height", dwHeight);
		settings.endGroup();
	};


	CDialog::OnClose();
};

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UpdateSizes();
	// Resize all dialogs
}

/* ------------------------------------------------------------------- */

HCURSOR CDeepSkyStackerLiveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::SetCurrentTab(DSSLIVETAB Tab)
{
	m_CurrentTab = Tab;
	UpdateTab();
};

/* ------------------------------------------------------------------- */

LRESULT CDeepSkyStackerLiveDlg::OnHTMLHelp(WPARAM, LPARAM)
{
	OnHelp();
	return 1;
};

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::OnHelp()
{
	if (m_MainBoard.m_hWnd)
		CallHelp();
};

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::CallHelp()
{
	CString			strBase;
	CString			strFile;
	TCHAR			szFileName[_MAX_PATH];
	TCHAR			szDrive[_MAX_DRIVE];
	TCHAR			szDir[_MAX_DIR];
	CString			strHelpFile;

	strHelpFile.LoadString(IDS_HELPFILE);

	GetModuleFileName(nullptr, szFileName, sizeof(szFileName));
	strBase = szFileName;
	_tsplitpath(strBase, szDrive, szDir, nullptr, nullptr);

	strFile = szDrive;
	strFile += szDir;
	strFile += strHelpFile;

	::HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, 0);
};

/* ------------------------------------------------------------------- */


CDeepSkyStackerLiveDlg* GetDSSLiveDlg(CWnd* pDialog)
{
	if (pDialog)
	{
		CWnd* pParent = pDialog->GetParent();
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pParent);

		return pDlg;
	}
	else
	{
		CWnd* pWnd = GetDSSLiveApp()->m_pMainWnd;
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);

		return pDlg;
	};
}
void AddToLog(QString szString, BOOL bAddDateTime /* = FALSE */, BOOL bBold /* = FALSE */, BOOL bItalic /* = FALSE */, COLORREF crColor /* = RGB(0, 0, 0) */)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetLogTab().AddToLog(szString, bAddDateTime, bBold, bItalic, crColor);
	};
}

/* ------------------------------------------------------------------- */

void SetLastImage(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap, LPCTSTR szFileName /* = nullptr */)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetLastImageTab().SetImage(pBitmap, pWndBitmap, szFileName);
	};
}

/* ------------------------------------------------------------------- */

void SetStackedImage(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetStackedImageTab().SetImage(pBitmap, pWndBitmap, nullptr);
	};
}

/* ------------------------------------------------------------------- */

void AdviseStackedImageSaved()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetStackedImageTab().OnStackedImageSaved();
	};
}

/* ------------------------------------------------------------------- */

void AddImageToList(LPCTSTR szImage)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetImageListTab().AddImage(szImage);
	};
}

/* ------------------------------------------------------------------- */

void ChangeImageStatusInList(LPCTSTR szImage, IMAGESTATUS status)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetImageListTab().ChangeImageStatus(szImage, status);
	};
}

/* ------------------------------------------------------------------- */

void ChangeImageInfoInCharts(LPCTSTR szFileName, STACKIMAGEINFO info)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetGraphsTab().ChangeImageInfo(szFileName, info);
	};
}

/* ------------------------------------------------------------------- */

void UpdateImageOffsetsInList(LPCTSTR szImage, double fdX, double fdY, double fAngle)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetImageListTab().UpdateImageOffsets(szImage, fdX, fdY, fAngle);
	};
}

/* ------------------------------------------------------------------- */

void SetFootprintInStackedImage(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetStackedImageTab().OnSetFootprint(pt1, pt2, pt3, pt4);
	};
}

/* ------------------------------------------------------------------- */

void AddScoreFWHMStarsToGraph(LPCTSTR szFileName, double fScore, double fFWHM, double fStars, double fSkyBackground)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetGraphsTab().AddScoreFWHMStars(szFileName, fScore, fFWHM, fStars, fSkyBackground);
	};
}

/* ------------------------------------------------------------------- */

void AddOffsetsAngleToGraph(LPCTSTR szFileName, double fdX, double fdY, double fAngle)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetGraphsTab().AddOffsetAngle(szFileName, fdX, fdY, fAngle);
	};
}

/* ------------------------------------------------------------------- */

void UpdateLiveSettings()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetMainBoard().UpdateLiveSettings();
	};
}

/* ------------------------------------------------------------------- */

void ResetEmailCount()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetMainBoard().ResetEmailCount();
	};
}

/* ------------------------------------------------------------------- */

void PostSaveStackedImage()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetMainBoard().PostSaveStackedImage();
	};
}

/* ------------------------------------------------------------------- */

void ShowResetEmailCountButton()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
	{
		CDeepSkyStackerLiveDlg* pDlg = dynamic_cast<CDeepSkyStackerLiveDlg*>(pWnd);
		if (pDlg)
			pDlg->GetSettingsTab().ShowResetEmailCountButton();
	};
}

/* ------------------------------------------------------------------- */

void FlashMainWindow()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		pWnd->FlashWindowEx(FLASHW_ALL, 4, 0);
}