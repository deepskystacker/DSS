// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"
#include "SettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
	//}}AFX_DATA_INIT
	m_bLoadSettings = FALSE;
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Control(pDX, IDC_SETTINGSNAME, m_Name);
	DDX_Control(pDX, IDC_SETTINGSLIST, m_List);
	DDX_Control(pDX, IDC_LOAD, m_Load);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Control(pDX, IDC_ADD, m_Add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingsDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_LBN_SELCHANGE(IDC_SETTINGSLIST, OnSelchangeSettingslist)
	ON_LBN_DBLCLK(IDC_SETTINGSLIST, OnDblclkSettingslist)
	ON_EN_CHANGE(IDC_SETTINGSNAME, OnChangeSettingsname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

void CSettingsDlg::UpdateControls()
{
	CString				strText;
	BOOL				bAdd = FALSE;
	BOOL				bLoad = FALSE;

	m_Name.GetWindowText(strText);

	if (strText.GetLength())
	{
		// Check that the name is not already in the list
		LONG			lNrSettings;
		LONG			i;

		lNrSettings = m_pSettings->Count();

		bAdd = TRUE;

		for (i = 0;i<lNrSettings && bAdd;i++)
		{
			CDSSSetting		cds;

			m_pSettings->GetItem(i, cds);

			if (!cds.m_strName.CompareNoCase(strText))
				bAdd = FALSE;
		};
	};

	if (m_List.GetCurSel() >= 0)
		bLoad = TRUE;

	m_Load.EnableWindow(bLoad);
	m_Delete.EnableWindow(bLoad);
	m_Add.EnableWindow(bAdd);
};

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnAdd()
{
	CString				strText;

	m_Name.GetWindowText(strText);
	m_CurrentSetting.m_strName = strText;

	m_pSettings->Add(m_CurrentSetting);
	m_List.AddString(strText);

	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnDelete()
{
	LONG				lCurSel;

	lCurSel = m_List.GetCurSel();
	if (lCurSel >= 0)
	{
		CString			strText;
		CString			strSettings;

		m_List.GetText(lCurSel, strSettings);

		strText.Format(IDS_ASK_DELETESETTINGS, strSettings);
		if (AfxMessageBox(strText, MB_YESNO | MB_DEFBUTTON2, 0)== IDYES)
		{
			m_pSettings->Remove(lCurSel);
			m_List.DeleteString(lCurSel);
		};
	};

	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnLoad()
{
	LONG				lCurSel;

	lCurSel = m_List.GetCurSel();
	if (lCurSel >= 0)
	{
		m_pSettings->GetItem(lCurSel, m_CurrentSetting);
		m_bLoadSettings = TRUE;
	};

	OnOK();
}

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnSelchangeSettingslist()
{
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnDblclkSettingslist()
{
	OnLoad();
}

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnChangeSettingsname()
{
	UpdateControls();
}

/* ------------------------------------------------------------------- */

BOOL CSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pSettings)
	{
		LONG			lNrSettings;
		LONG			i;

		lNrSettings = m_pSettings->Count();

		for (i = 0;i<lNrSettings;i++)
		{
			CDSSSetting		cds;

			if (m_pSettings->GetItem(i, cds))
				m_List.AddString(cds.m_strName);
		};
	};

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnOK()
{
	if (m_pSettings)
		m_pSettings->Save();
	CDialog::OnOK();
}

/* ------------------------------------------------------------------- */
