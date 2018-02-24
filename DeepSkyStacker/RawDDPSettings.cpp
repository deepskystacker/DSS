// RawDDPSettings.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"
#include "RawDDPSettings.h"
#include "Registry.h"
#include "DeepStackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CRawDDPSettings dialog


CRawDDPSettings::CRawDDPSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CRawDDPSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRawDDPSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/* ------------------------------------------------------------------- */

void CRawDDPSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRawDDPSettings)
	DDX_Control(pDX, IDC_SHEETRECT, m_SheetStatic);
	DDX_Control(pDX, IDOK, m_OK);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CRawDDPSettings, CDialog)
	//{{AFX_MSG_MAP(CRawDDPSettings)
	ON_BN_CLICKED(IDC_APPLY, &OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CRawDDPSettings message handlers

BOOL CRawDDPSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

 	CRect			rcSettings;

	m_SheetStatic.GetWindowRect(&rcSettings);
	ScreenToClient(&rcSettings);
	rcSettings.left = 0;
	rcSettings.top -= 11;

	m_Sheet.AddPage(&m_tabRAWFiles);
	m_Sheet.AddPage(&m_tabFITSFiles);

	m_Sheet.EnableStackedTabs( FALSE );
	m_Sheet.Create (this, WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0);

	m_Sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_Sheet.ModifyStyle( 0, WS_TABSTOP );

	// move to left upper corner

	m_Sheet.MoveWindow(&rcSettings, TRUE);
	m_Sheet.ShowWindow(SW_SHOWNA);

	// UpdateControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CRawDDPSettings::UpdateControls()
{
//	m_OK.EnableWindow(bOk);
};

/* ------------------------------------------------------------------- */

void CRawDDPSettings::OnOK() 
{
	m_tabRAWFiles.SaveValues();
	m_tabFITSFiles.SaveValues();

	GetDeepStackerDlg(NULL)->GetStackingDlg().ReloadCurrentImage();
	CDialog::OnOK();
}

/* ------------------------------------------------------------------- */

void CRawDDPSettings::OnApply()
{
	m_tabRAWFiles.SaveValues();
	m_tabFITSFiles.SaveValues();

	GetDeepStackerDlg(NULL)->GetStackingDlg().ReloadCurrentImage();
};

/* ------------------------------------------------------------------- */
