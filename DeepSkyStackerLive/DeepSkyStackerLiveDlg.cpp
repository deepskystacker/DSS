// DeepSkyStackerLiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "DeepSkyStackerLiveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeepSkyStackerLiveDlg dialog


/* ------------------------------------------------------------------- */

CDeepSkyStackerLiveDlg::CDeepSkyStackerLiveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeepSkyStackerLiveDlg::IDD, pParent)
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

BOOL CDeepSkyStackerLiveDlg::OnEraseBkgnd(CDC * pDC)
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
	m_MainBoard.InvalidateRect(NULL);
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
	m_Graphs.Create(IDD_GRAPHS, this);
	m_Settings.Create(IDD_SETTINGS, this);

	{
		DWORD		dwMaximized = 0;
		CString		strTop = "";
		CString		strLeft = "";
		DWORD		dwWidth = 0;
		DWORD		dwHeight = 0;

		CRegistry	reg;

		reg.LoadKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Maximized"), dwMaximized);
		reg.LoadKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Top"), strTop);
		reg.LoadKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Left"), strLeft);
		reg.LoadKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Width"), dwWidth);
		reg.LoadKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Height"), dwHeight);

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

	m_Log.AddToLog(strTitle, TRUE, TRUE);
	m_Log.AddToLog(_T("\n"));

	CString			strText;
	strText.LoadString(IDS_LOG_STARTING);
	m_Log.AddToLog(strText, FALSE, TRUE, FALSE);
	strText.LoadString(IDS_LOG_STARTING_1);
	m_Log.AddToLog(strText, FALSE, FALSE, FALSE);
	strText.LoadString(IDS_LOG_STARTING_2);
	m_Log.AddToLog(strText, FALSE, FALSE, FALSE);
	strText.LoadString(IDS_LOG_STARTING_3);
	m_Log.AddToLog(strText, FALSE, FALSE, FALSE);
	strText.LoadString(IDS_LOG_STARTING_4);
	m_Log.AddToLog(strText, FALSE, FALSE, FALSE);
	strText.LoadString(IDS_LOG_STARTING_5);
	m_Log.AddToLog(strText, FALSE, FALSE, FALSE);

	{
		CWorkspace			workspace;

		TCHAR				szPath[_MAX_PATH];
		CString				strPath;

		SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath);

		strPath = szPath;
		strPath += "\\DeepSkyStacker\\DSSLive.settings";
		workspace.ReadFromFile(strPath);
	};

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/* ------------------------------------------------------------------- */

void CDeepSkyStackerLiveDlg::OnClose( )
{
	m_ImageList.Close();

	{
		DWORD		dwMaximized = 0;
		CString		strTop = "";
		CString		strLeft = "";
		DWORD		dwWidth = 0;
		DWORD		dwHeight = 0;

		CRegistry	reg;

		WINDOWPLACEMENT		wp;

		memset(&wp, 0, sizeof(wp));
		wp.length  = sizeof(wp);

		GetWindowPlacement(&wp);
		dwMaximized = (wp.showCmd == SW_SHOWMAXIMIZED);
		strLeft.Format(_T("%ld"), wp.rcNormalPosition.left);
		strTop.Format(_T("%ld"), wp.rcNormalPosition.top);

		dwWidth  = wp.rcNormalPosition.right-wp.rcNormalPosition.left;
		dwHeight = wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;

		reg.SaveKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Maximized"), dwMaximized);
		reg.SaveKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Top"), strTop);
		reg.SaveKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Left"), strLeft);
		reg.SaveKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Width"), dwWidth);
		reg.SaveKey(REGENTRY_BASEKEY_LIVE_POSITION, _T("Height"), dwHeight);
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

	GetModuleFileName(NULL, szFileName, sizeof(szFileName));
	strBase = szFileName;
	_tsplitpath(strBase, szDrive, szDir, NULL, NULL);

	strFile = szDrive;
	strFile += szDir;
	strFile += strHelpFile;

	::HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
};

/* ------------------------------------------------------------------- */
