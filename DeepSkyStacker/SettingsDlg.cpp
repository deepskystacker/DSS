// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"
#include "SettingsDlg.h"
#include "dss_settings.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
	//}}AFX_DATA_INIT
	m_bLoadSettings = false;
    m_pSettings = NULL;
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
	bool				bAdd = false;
	bool				bLoad = false;

	m_Name.GetWindowText(strText);

	if (strText.GetLength())
	{
		// Check that the name is not already in the list
		const int lNrSettings = m_pSettings->Count();

		bAdd = true;

		for (int i = 0; i < lNrSettings && bAdd; i++)
		{
			CDSSSetting cds;
			m_pSettings->GetItem(i, cds);

			const QString text(QString::fromWCharArray(strText));
			if (!cds.m_strName.compare(text, Qt::CaseInsensitive))
				bAdd = false;
		}
	}

	if (m_List.GetCurSel() >= 0)
		bLoad = true;

	m_Load.EnableWindow(bLoad);
	m_Delete.EnableWindow(bLoad);
	m_Add.EnableWindow(bAdd);
};

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnAdd()
{
	CString				strText;

	m_Name.GetWindowText(strText);
	m_CurrentSetting.m_strName = QString::fromWCharArray(strText);

	m_pSettings->Add(m_CurrentSetting);
	m_List.AddString(strText);

	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnDelete()
{
	const int lCurSel = m_List.GetCurSel();
	if (lCurSel >= 0)
	{
		CString			strText;
		CString			strSettings;

		m_List.GetText(lCurSel, strSettings);

		strText.Format(IDS_ASK_DELETESETTINGS, strSettings.GetString());
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
	const int lCurSel = m_List.GetCurSel();
	if (lCurSel >= 0)
	{
		m_pSettings->GetItem(lCurSel, m_CurrentSetting);
		m_bLoadSettings = true;
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

	if (m_pSettings != nullptr)
	{
		const int lNrSettings = m_pSettings->Count();

		for (int i = 0; i < lNrSettings; i++)
		{
			CDSSSetting cds;
			if (m_pSettings->GetItem(i, cds))
				m_List.AddString(cds.m_strName.toStdWString().c_str());
		}
	}

	UpdateControls();

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

/* ------------------------------------------------------------------- */

void CSettingsDlg::OnOK()
{
	if (m_pSettings)
		m_pSettings->Save();
	CDialog::OnOK();
}

/* ------------------------------------------------------------------- */
