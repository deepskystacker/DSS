// DeepStackerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "DeepStackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------- */

static BOOL	GetDefaultSettingsFileName(CString & strFile)
{
	CString			strBase;
	TCHAR			szFileName[_MAX_PATH];
	TCHAR			szDrive[_MAX_DRIVE];
	TCHAR			szDir[_MAX_DIR];

	GetModuleFileName(NULL, szFileName, sizeof(szFileName));
	strBase = szFileName;
	_splitpath(strBase, szDrive, szDir, NULL, NULL);

	strFile = szDrive;
	strFile += szDir;
	strFile += "DSSSettings.DSSSettings";

	return TRUE;
};

/* ------------------------------------------------------------------- */

#pragma pack(push, HDSETTINGS, 2)

const DWORD			HDSSETTINGS_MAGIC = 0x7ABC6F10L;

typedef struct tagHDSETTINGSHEADER
{
	DWORD			dwMagic;		// Magic number (always HDSSETTINGS_MAGIC)
	DWORD			dwHeaderSize;	// Always sizeof(HDSETTINGSHEADER);
	LONG			lNrSettings;	// Number of settings
	DWORD			dwFlags;		// Flags
	char			Reserved[32];	// Reserved (set to 0)
}HDSETTINGSHEADER;

#pragma pack(pop, HDSETTINGS)

/* ------------------------------------------------------------------- */

BOOL	CDSSSettings::Load(LPCTSTR szFile)
{
	BOOL			bResult = FALSE;
	CString			strFile = szFile;
	FILE *			hFile = NULL;

	if (!strFile.GetLength())
		GetDefaultSettingsFileName(strFile);

	hFile = fopen(strFile, "rb");
	if (hFile)
	{
		HDSETTINGSHEADER		Header;
		LONG					i;

		fread(&Header, sizeof(Header), 1, hFile);
		if ((Header.dwMagic == HDSSETTINGS_MAGIC) && 
			(Header.dwHeaderSize == sizeof(Header)))
		{
			m_lSettings.clear();
			for (i = 0;i<Header.lNrSettings;i++)
			{
				CDSSSetting		cds;

				cds.Load(hFile);
				m_lSettings.push_back(cds);
			};

			bResult = TRUE;
			m_lSettings.sort();
		};

		fclose(hFile);
	};
	
	m_bLoaded = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CDSSSettings::Save(LPCTSTR szFile)
{
	BOOL			bResult = FALSE;
	CString			strFile = szFile;
	FILE *			hFile = NULL;

	if (!strFile.GetLength())
		GetDefaultSettingsFileName(strFile);

	hFile = fopen(strFile, "wb");
	if (hFile)
	{
		m_lSettings.sort();

		HDSETTINGSHEADER		Header;
		DSSSETTINGITERATOR		it;

		memset(&Header, 0, sizeof(Header));

		Header.dwMagic = HDSSETTINGS_MAGIC;
		Header.dwHeaderSize = sizeof(Header);
		Header.lNrSettings  = (LONG)m_lSettings.size();

		fwrite(&Header, sizeof(Header), 1, hFile);
		for (it = m_lSettings.begin(); it != m_lSettings.end();it++)
			(*it).Save(hFile);

		fclose(hFile);
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

/////////////////////////////////////////////////////////////////////////////
// CDeepStackerDlg dialog


CDeepStackerDlg::CDeepStackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeepStackerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeepStackerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_dwCurrentTab = 0;
}

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeepStackerDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BARSTATIC, m_BarStatic);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CDeepStackerDlg, CDialog)
	//{{AFX_MSG_MAP(CDeepStackerDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_MESSAGE(WM_HELP, OnHTMLHelp)
	ON_BN_CLICKED(IDCANCEL, &CDeepStackerDlg::OnBnClickedCancel)
	ON_WM_DROPFILES()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::UpdateTab()
{
	switch (m_dwCurrentTab)
	{
	case IDD_REGISTERING :
	case IDD_STACKING :
		m_dlgStacking.ShowWindow(SW_SHOW);
		m_dlgProcessing.ShowWindow(SW_HIDE);
		m_dlgLibrary.ShowWindow(SW_HIDE);
		break;
	case IDD_LIBRARY :
		m_dlgStacking.ShowWindow(SW_HIDE);
		m_dlgProcessing.ShowWindow(SW_HIDE);
		m_dlgLibrary.ShowWindow(SW_SHOW);
		break;
	case IDD_PROCESSING :
		m_dlgStacking.ShowWindow(SW_HIDE);
		m_dlgProcessing.ShowWindow(SW_SHOW);
		m_dlgLibrary.ShowWindow(SW_HIDE);
		break;
	};
	m_ExplorerBar.InvalidateRect(NULL);
};

/* ------------------------------------------------------------------- */

BOOL CDeepStackerDlg::OnEraseBkgnd(CDC * pDC)
{
	return 1;
};

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::UpdateSizes()
{
	// Resize the tab control
	CRect			rcDlg;
	CRect			rcExplorerBar;

	GetClientRect(&rcDlg);

	if (m_dlgStacking.m_hWnd)
	{
		rcExplorerBar = rcDlg;
		rcDlg.left += 220;
		rcExplorerBar.right = rcDlg.left;
		
		if (m_dlgStacking.m_hWnd)
			m_dlgStacking.MoveWindow(&rcDlg);
		if (m_dlgProcessing.m_hWnd)
			m_dlgProcessing.MoveWindow(&rcDlg);
		if (m_dlgLibrary.m_hWnd)
			m_dlgLibrary.MoveWindow(&rcDlg);
		if (m_ExplorerBar.m_hWnd)
			m_ExplorerBar.MoveWindow(&rcExplorerBar);
	};
};

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CDeepStackerDlg message handlers

void CDeepStackerDlg::ChangeTab(DWORD dwTabID)
{
	if (dwTabID == IDD_REGISTERING)
		dwTabID = IDD_STACKING;
#ifdef DSSBETA
	if (dwTabID == IDD_STACKING && 	(GetAsyncKeyState(VK_CONTROL) & 0x8000))
		dwTabID = IDD_LIBRARY;
#endif
	m_dwCurrentTab = dwTabID;
	UpdateTab();
};

/* ------------------------------------------------------------------- */

BOOL CDeepStackerDlg::OnInitDialog() 
{
	AddToLog("Initializing Main Dialog\n");
	CDialog::OnInitDialog();
	AddToLog("Initializing Main Dialog - ok\n");


	CString			strMask;
	CString			strTitle;

	this->DragAcceptFiles(TRUE);

	GetWindowText(strMask);
	strTitle.Format(strMask, VERSION_DEEPSKYSTACKER);
	SetWindowText(strTitle);
	m_strBaseTitle = strTitle;

	AddToLog("Creating Left Panel\n");
	m_ExplorerBar.Create(IDD_EXPLORERBAR, this);
	AddToLog("Creating Left Panel - ok\n");

	SetIcon(AfxGetApp()->LoadIcon(IDI_APP), TRUE);
	m_dlgStacking.SetStartingFileList(m_strStartFileList);
	AddToLog("Creating Stacking Panel\n");
	m_dlgStacking.Create(IDD_STACKING, this);
	AddToLog("Creating Stacking Panel - ok\n");
	AddToLog("Creating Processing Panel\n");
	m_dlgProcessing.Create(IDD_PROCESSING, this);
	AddToLog("Creating Processing Panel - ok\n");
	m_dlgLibrary.Create(IDD_LIBRARY, this);

	AddToLog("Restoring Window Position\n");
	RestoreWindowPosition(this, REGENTRY_BASEKEY_DEEPSKYSTACKER_POSITION);
	AddToLog("Restoring Window Position - ok\n");

	m_dwCurrentTab = IDD_REGISTERING;
	AddToLog("Updating All Panels\n");
	UpdateTab();
	AddToLog("Updating All Panels - ok\n");
	AddToLog("Updating Sizes\n");
	UpdateSizes();
	AddToLog("Updating Sizes - ok\n");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::SetCurrentFileInTitle(LPCTSTR szFile)
{
	CString					strFileName = szFile;
	if (strFileName.GetLength())
	{
		TCHAR				szFileName[_MAX_FNAME];
		TCHAR				szExt[_MAX_EXT];

		_splitpath(szFile, NULL, NULL, szFileName, szExt);

		CString				strTitle;

		strTitle.Format("%s - %s%s", (LPCTSTR)m_strBaseTitle, szFileName, szExt);
		SetWindowText(strTitle);
	}
	else
		SetWindowText(m_strBaseTitle);
};


/* ------------------------------------------------------------------- */

void CDeepStackerDlg::OnDropFiles(HDROP hDropInfo)
{
	if (hDropInfo && m_dlgStacking.m_hWnd)
	{
		SetForegroundWindow();
		BringWindowToTop();
		SetActiveWindow();
		m_dlgStacking.DropFiles(hDropInfo);
	};
};

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	UpdateSizes();
	// Resize all dialogs
}

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::OnClose()
{
	if (m_dlgStacking.SaveOnClose() &&
		m_dlgProcessing.SaveOnClose())
	{
		SaveWindowPosition(this, REGENTRY_BASEKEY_DEEPSKYSTACKER_POSITION);

		CDialog::OnClose();
	};
}

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::OnBnClickedCancel()
{
	if (!(GetKeyState(VK_ESCAPE) & 0x8000))
		OnCancel();
}

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::OnOK()
{

};

/* ------------------------------------------------------------------- */

LRESULT CDeepStackerDlg::OnHTMLHelp(WPARAM, LPARAM)
{
	OnHelp();
	return 1;
};

/* ------------------------------------------------------------------- */

void CDeepStackerDlg::OnHelp()
{
	if (m_ExplorerBar.m_hWnd)
		m_ExplorerBar.CallHelp();
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	SaveWindowPosition(CWnd * pWnd, LPCTSTR szRegistryPath)
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

	pWnd->GetWindowPlacement(&wp);
	dwMaximized = (wp.showCmd == SW_SHOWMAXIMIZED);
	strLeft.Format("%ld", wp.rcNormalPosition.left);
	strTop.Format("%ld", wp.rcNormalPosition.top);

	dwWidth  = wp.rcNormalPosition.right-wp.rcNormalPosition.left;
	dwHeight = wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;

	reg.SaveKey(szRegistryPath, "Maximized", dwMaximized);
	reg.SaveKey(szRegistryPath, "Top", strTop);
	reg.SaveKey(szRegistryPath, "Left", strLeft);
	reg.SaveKey(szRegistryPath, "Width", dwWidth);
	reg.SaveKey(szRegistryPath, "Height", dwHeight);
};

/* ------------------------------------------------------------------- */

void	RestoreWindowPosition(CWnd * pWnd, LPCTSTR szRegistryPath, bool bCenter)
{
	DWORD		dwMaximized = 0;
	CString		strTop = "";
	CString		strLeft = "";
	DWORD		dwWidth = 0;
	DWORD		dwHeight = 0;

	CRegistry	reg;

	reg.LoadKey(szRegistryPath, "Maximized", dwMaximized);
	reg.LoadKey(szRegistryPath, "Top", strTop);
	reg.LoadKey(szRegistryPath, "Left", strLeft);
	reg.LoadKey(szRegistryPath, "Width", dwWidth);
	reg.LoadKey(szRegistryPath, "Height", dwHeight);

	if (strTop.GetLength() && strLeft.GetLength() && dwWidth && dwHeight)
	{
		WINDOWPLACEMENT		wp;

		memset(&wp, 0, sizeof(wp));
		wp.length  = sizeof(wp);
		wp.flags   = 0;
		wp.showCmd = dwMaximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
		wp.rcNormalPosition.left   = atol(strLeft);
		wp.rcNormalPosition.top    = atol(strTop);
		wp.rcNormalPosition.right  = wp.rcNormalPosition.left+dwWidth;
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+dwHeight;

		pWnd->SetWindowPlacement(&wp);
		if (bCenter)
			pWnd->CenterWindow();
	};
};

/* ------------------------------------------------------------------- */
