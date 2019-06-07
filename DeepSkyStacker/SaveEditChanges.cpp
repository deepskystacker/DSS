// RegisterSettings.cpp : implementation file
//

#include "stdafx.h"
#include "SaveEditChanges.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CSaveEditChanges dialog


CSaveEditChanges::CSaveEditChanges(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveEditChanges::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveEditChanges)
	//}}AFX_DATA_INIT

}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveEditChanges)
	DDX_Control(pDX, IDC_ASKAGAIN, m_AskAlways);
	DDX_Control(pDX, IDC_SAVEWITHOUTASKING, m_SaveDontAsk);
	DDX_Control(pDX, IDC_DONTSAVEWITHOUTASKING, m_DontSaveDontAsk);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CSaveEditChanges, CDialog)
	//{{AFX_MSG_MAP(CSaveEditChanges)
	ON_BN_CLICKED(IDC_ASKAGAIN, OnAskAlways)
	ON_BN_CLICKED(IDC_SAVEWITHOUTASKING, OnSaveDontAsk)
	ON_BN_CLICKED(IDC_DONTSAVEWITHOUTASKING, OnDontSaveDontAsk)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CSaveEditChanges message handlers

BOOL CSaveEditChanges::OnInitDialog() 
{
	CRegistry			reg;

	CDialog::OnInitDialog();
	
	switch (GetSaveEditMode())
	{
	case SECM_SAVEDONTASK :
		m_SaveDontAsk.SetCheck(TRUE);
		break;
	case SECM_DONTSAVEDONTASK :
		m_DontSaveDontAsk.SetCheck(TRUE);
		break;
	case SECM_ASKALWAYS :
		m_AskAlways.SetCheck(TRUE);
		break;
	};

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::SaveSettings()
{
	SAVEEDITCHANGESMODE	Mode = SECM_ASKALWAYS;

	if (m_SaveDontAsk.GetCheck())
		Mode = SECM_SAVEDONTASK;
	else if (m_DontSaveDontAsk.GetCheck())
		Mode = SECM_DONTSAVEDONTASK;

	SetSaveEditMode(Mode);
};

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnYes() 
{
	SaveSettings();
	EndDialog(IDYES);
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnNo() 
{
	SaveSettings();
	EndDialog(IDNO);
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnCancel() 
{
	CDialog::OnCancel();
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnAskAlways() 
{
	if (m_AskAlways.GetCheck())
	{
		m_SaveDontAsk.SetCheck(FALSE);
		m_DontSaveDontAsk.SetCheck(FALSE);
	};
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnSaveDontAsk() 
{
	if (m_SaveDontAsk.GetCheck())
	{
		m_AskAlways.SetCheck(FALSE);
		m_DontSaveDontAsk.SetCheck(FALSE);
	};
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnDontSaveDontAsk() 
{
	if (m_DontSaveDontAsk.GetCheck())
	{
		m_AskAlways.SetCheck(FALSE);
		m_SaveDontAsk.SetCheck(FALSE);
	};
}

/* ------------------------------------------------------------------- */

void	SetSaveEditMode(SAVEEDITCHANGESMODE Mode)
{
	CRegistry			reg;
	LONG				lValue = Mode;

	reg.SaveKey(REGENTRY_BASEKEY_EDITSTARS, _T("AutoSave"), lValue);
};

/* ------------------------------------------------------------------- */

SAVEEDITCHANGESMODE	GetSaveEditMode()
{
	CRegistry			reg;
	DWORD				dwValue = 0;

	reg.LoadKey(REGENTRY_BASEKEY_EDITSTARS, _T("AutoSave"), dwValue);

	return (SAVEEDITCHANGESMODE)dwValue;
};

/* ------------------------------------------------------------------- */

LONG	AskSaveEditChangesMode()
{
	CRegistry			reg;
	DWORD				dwValue = 0;

	reg.LoadKey(REGENTRY_BASEKEY_EDITSTARS, _T("AutoSave"), dwValue);

	if (dwValue == SECM_SAVEDONTASK)
		return IDYES;
	else if (dwValue == SECM_DONTSAVEDONTASK)
		return IDNO;
	else
	{
		CSaveEditChanges	dlg;

		return dlg.DoModal();
	};
};

/* ------------------------------------------------------------------- */

