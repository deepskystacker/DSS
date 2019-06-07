// LibraryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "LibraryDlg.h"


// CLibraryDlg dialog

IMPLEMENT_DYNAMIC(CLibraryDlg, CDialog)

/* ------------------------------------------------------------------- */

CLibraryDlg::CLibraryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLibraryDlg::IDD, pParent)
{

}

/* ------------------------------------------------------------------- */

CLibraryDlg::~CLibraryDlg()
{
}

/* ------------------------------------------------------------------- */

void CLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HEADER, m_HeaderHTML);
	DDX_Control(pDX, IDC_LIBRARY, m_LibraryHTML);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CLibraryDlg, CDialog)
	ON_NOTIFY( QHTMN_HYPERLINK, IDC_HEADER, OnQHTMHyperlink )
	ON_NOTIFY( QHTMN_HYPERLINK, IDC_LIBRARY, OnQHTMHyperlink )

	ON_WM_SIZE()
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
// CLibraryDlg message handlers

BOOL CLibraryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ControlPos.SetParent(this);

	m_ControlPos.AddControl(IDC_HEADER, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_LIBRARY, CP_RESIZE_VERTICAL | CP_RESIZE_HORIZONTAL);

	m_HeaderHTML.SetToolTips(FALSE);
	m_LibraryHTML.SetToolTips(FALSE);

	m_HeaderHTML.SetWindowText(_T("<b><font size='6' color='#6699FF'>Library</font><font size='5' color='#0000FF'><br>Use&nbsp;&nbsp;&nbsp; Manage</font></b>"));
	m_LibraryHTML.SetWindowText(_T("Library Content"));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CLibraryDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	m_ControlPos.MoveControls();
}

/* ------------------------------------------------------------------- */

void CLibraryDlg::OnQHTMHyperlink(NMHDR*nmh, LRESULT*)
{
	LPNMQHTM pnm = reinterpret_cast<LPNMQHTM>( nmh );
	if( pnm->pcszLinkText )
	{
		pnm->resReturnValue = FALSE;
/*		LONG				lLinkID;

		lLinkID = _ttol(pnm->pcszLinkText);
		CallStackingParameters(lLinkID);*/
	}
}

/* ------------------------------------------------------------------- */
