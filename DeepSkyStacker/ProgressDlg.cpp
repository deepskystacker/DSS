// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"
#include "ProgressDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowPos(&this->wndTop, 0, 0, 0, 0, SWP_NOSIZE);
	return true;
}

CProgressDlg::CProgressDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	//}}AFX_DATA_INIT
	m_bCancelled = false;
}

bool CDSSProgressDlg::Close()
{
	m_dlg.PeekAndPump();
	// Prevent failure if mdlg is no longer a valid window
	if (nullptr != m_dlg.m_hWnd)
		m_dlg.EndDialog(true);

	DeepSkyStacker::instance()->enableSubDialogs();

	return true;
}

void CDSSProgressDlg::CreateProgressDialog()
{

	if (!m_dlg.m_hWnd)
	{
		CWnd* pMainWnd = AfxGetMainWnd();

		m_dlg.Create(IDD_PROGRESS);

		// Centre on main window
		if (pMainWnd)
			m_dlg.CenterWindow(pMainWnd);

		// Disable child dialogs of DeepSkyStackerDlg

		DeepSkyStacker::instance()->disableSubDialogs();

		// Re-enable this window
		m_dlg.EnableWindow(true);
		m_dlg.ShowWindow(SW_SHOW);
	};
};

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_TIMEREMAINING, m_TimeRemaining);
	DDX_Control(pDX, IDC_STOP, m_Cancel);
	DDX_Control(pDX, IDC_PROGRESS2, m_Progress2);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress1);
	DDX_Control(pDX, IDC_PROCESS_TEXT2, m_Text2);
	DDX_Control(pDX, IDC_PROCESS_TEXT1, m_Text1);
	DDX_Control(pDX, IDC_NRPROCESSORS, m_NrProcessors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_TIMEREMAINING, &CProgressDlg::OnStnClickedTimeremaining)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

void CProgressDlg::OnCancel()
{
	m_bCancelled	= true;
	//CDialog::OnCancel();
}

void CProgressDlg::OnStop()
{
	m_bCancelled	= true;
	m_Cancel.EnableWindow(false);
}


void CProgressDlg::OnStnClickedTimeremaining()
{
	// TODO: Add your control notification handler code here
}
