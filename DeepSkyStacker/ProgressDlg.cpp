// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog


CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	//}}AFX_DATA_INIT
	m_bCancelled = FALSE;
}


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
	m_bCancelled	= TRUE;
	//CDialog::OnCancel();
}

void CProgressDlg::OnStop() 
{
	m_bCancelled	= TRUE;
	m_Cancel.EnableWindow(FALSE);
}


void CProgressDlg::OnStnClickedTimeremaining()
{
	// TODO: Add your control notification handler code here
}
