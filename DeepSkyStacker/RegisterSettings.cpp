// RegisterSettings.cpp : implementation file
//

#include "stdafx.h"

#include <QSettings>

#include "deepskystacker.h"
#include "RegisterSettings.h"
#include "Registry.h"
#include "RawDDPSettings.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "Workspace.h"
#include "RecommandedSettings.h"

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CRegisterSettings dialog


CRegisterSettings::CRegisterSettings(CWnd* pParent /*=nullptr*/)
	: CDialog(CRegisterSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterSettings)
	//}}AFX_DATA_INIT

	m_bStack = false;
	m_fPercentStack = 80;

	m_bNoDark = false;
	m_bNoFlat = false;
	m_bNoOffset = false;
	m_pStackingTasks = nullptr;
	m_bForceRegister = false;
	m_bSettingsOnly	 = false;
    m_dwDetectionThreshold = 0;
    m_bMedianFilter = false;
}

/* ------------------------------------------------------------------- */

void CRegisterSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterSettings)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_SHEETRECT, m_Rect);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CRegisterSettings, CDialog)
	//{{AFX_MSG_MAP(CRegisterSettings)
	ON_BN_CLICKED(IDC_RAWDDPSETTINGS, OnRawddpsettings)
	ON_BN_CLICKED(IDC_STACKINGPARAMETERS, OnStackingParameters)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RECOMMANDEDSETTINGS, &CRegisterSettings::OnBnClickedRecommandedsettings)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CRegisterSettings message handlers

BOOL CRegisterSettings::OnInitDialog()
{
	CWorkspace			workspace;
	CString				strValue;
	DWORD				bValue;
	DWORD				bUseFileSettings = 1;


	CDialog::OnInitDialog();

 	CRect				rcSettings;

	m_Rect.GetWindowRect(&rcSettings);
	ScreenToClient(&rcSettings);
	rcSettings.left -= 5;
	rcSettings.top -= 11;

	if (!m_bSettingsOnly)
		m_Sheet.AddPage(&m_tabActions);

	m_Sheet.AddPage(&m_tabAdvanced);

	m_Sheet.EnableStackedTabs( false );
	m_Sheet.Create (this, WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0);

	m_Sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_Sheet.ModifyStyle( 0, WS_TABSTOP );

	// move to left upper corner

	m_Sheet.MoveWindow(&rcSettings, true);
	m_Sheet.ShowWindow(SW_SHOWNA);

	if (!m_bSettingsOnly)
	{
		m_tabActions.m_ForceRegister.SetCheck(m_bForceRegister);
		strValue.Empty();
		QString temp = workspace.value("Register/PercentStack").toString();
		strValue = CString((LPCTSTR)temp.utf16());
		if (!strValue.GetLength())
			strValue = _T("80");

		m_tabActions.m_Percent.SetWindowText(strValue);

		bValue = workspace.value("Register/StackAfter", false).toBool();

		m_tabActions.m_Stack.SetCheck(bValue);
		m_tabActions.m_Percent.EnableWindow(bValue);

		
		m_tabActions.m_HotPixels.SetCheck(workspace.value("Register/DetectHotPixels", false).toBool());
	}
	else
	{
		GetDlgItem(IDC_STACKINGPARAMETERS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECOMMANDEDSETTINGS)->ShowWindow(SW_HIDE);
		m_tabAdvanced.GetDlgItem(IDC_COMPUTEDETECTEDSTARS)->ShowWindow(SW_HIDE);
	};

	m_tabAdvanced.m_PercentSlider.SetRange(2, 98);
	m_tabAdvanced.m_PercentSlider.SetPos(workspace.value("Register/DetectionThreshold", 10).toUInt());
	m_tabAdvanced.m_strFirstLightFrame = m_strFirstLightFrame;
	
	m_tabAdvanced.m_MedianFilter.SetCheck(workspace.value("Register/ApplyMedianFilter", false).toBool());
	m_tabAdvanced.UpdateSliderText();

	if (!m_bSettingsOnly)
	{
		if (m_bNoDark || m_bNoFlat || m_bNoOffset)
		{
			if (m_bNoDark && m_bNoFlat && m_bNoOffset)
				strValue.LoadString(IDS_CHECK_DARKFLATOFFSET);
			else if (m_bNoDark && m_bNoFlat)
				strValue.LoadString(IDS_CHECK_DARKFLAT);
			else if (m_bNoDark && m_bNoOffset)
				strValue.LoadString(IDS_CHECK_DARKOFFSET);
			else if (m_bNoFlat && m_bNoOffset)
				strValue.LoadString(IDS_CHECK_FLATOFFSET);
			else if (m_bNoDark)
				strValue.LoadString(IDS_CHECK_DARK);
			else if (m_bNoFlat)
				strValue.LoadString(IDS_CHECK_FLAT);
			else if (m_bNoOffset)
				strValue.LoadString(IDS_CHECK_OFFSET);

			switch (m_bNoDark + m_bNoFlat + m_bNoOffset)
			{
			case 3 :
				m_tabActions.m_StackWarning.SetBkColor(RGB(252, 220, 221), RGB(255, 64, 64), CLabel::Gradient);
				break;
			case 2 :
				m_tabActions.m_StackWarning.SetBkColor(RGB(252, 220, 221), RGB(255, 171, 63), CLabel::Gradient);
				break;
			case 1 :
				m_tabActions.m_StackWarning.SetBkColor(RGB(252, 220, 221), RGB(255, 234, 63), CLabel::Gradient);
				break;
			};
			m_tabActions.m_StackWarning.SetText(strValue);
		}
		else
		{
			strValue.LoadString(IDS_CHECK_ALLOK);
			m_tabActions.m_StackWarning.SetBkColor(RGB(229, 255, 193), RGB(21, 223, 33), CLabel::Gradient);
			m_tabActions.m_StackWarning.SetText(strValue);
		};
	};

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

/* ------------------------------------------------------------------- */

void CRegisterSettings::OnOK()
{
	CString				strText;
	CWorkspace			workspace;
	bool				hotPixels;

	if (!m_bSettingsOnly)
	{
		m_bForceRegister = m_tabActions.m_ForceRegister.GetCheck();

		m_bStack = m_tabActions.m_Stack.GetCheck() ? true : false;
		workspace.setValue("Register/StackAfter", m_bStack);

		m_tabActions.m_Percent.GetWindowText(strText);
		workspace.setValue("Register/PercentStack", QString::fromWCharArray(strText.GetBuffer()));

		hotPixels = m_tabActions.m_HotPixels.GetCheck() ? true : false;
		workspace.setValue("Register/DetectHotPixels", hotPixels);

		m_fPercentStack = _ttof(strText);
	};

	m_dwDetectionThreshold = m_tabAdvanced.m_PercentSlider.GetPos();
	workspace.setValue("Register/DetectionThreshold", (uint)m_dwDetectionThreshold);

	m_bMedianFilter = m_tabAdvanced.m_MedianFilter.GetCheck() ? true : false;
	workspace.setValue("Register/ApplyMedianFilter", m_bMedianFilter);

	workspace.saveSettings();

	CDialog::OnOK();
}

/* ------------------------------------------------------------------- */

void CRegisterSettings::OnRawddpsettings()
{
	CRawDDPSettings			dlg;

	dlg.DoModal();
}

/* ------------------------------------------------------------------- */

void CRegisterSettings::OnStackingParameters()
{
	StackSettings			dlg;
	CRect					rcCustom;

	if (m_pStackingTasks)
	{
		if (m_pStackingTasks->GetCustomRectangle(rcCustom))
		{
			dlg.enableCustomRectangle(true);
			dlg.selectCustomRectangle(m_pStackingTasks->IsCustomRectangleUsed());
		}
		else
		{
			dlg.enableCustomRectangle(false);
			dlg.selectCustomRectangle(false);
		}
		dlg.setTabVisibility(m_pStackingTasks->AreDarkUsed(), m_pStackingTasks->AreFlatUsed(), m_pStackingTasks->AreBiasUsed());
	}
	else
	{
		dlg.enableCustomRectangle(false);
		dlg.selectCustomRectangle(false);
	}

	if (!m_tabActions.m_Stack.GetCheck())
		dlg.setRegisteringOnly(true);

	dlg.setStackingTasks(m_pStackingTasks);

	if ((dlg.exec()== QDialog::Accepted) && m_pStackingTasks)
		m_pStackingTasks->UpdateTasksMethods();

}

/* ------------------------------------------------------------------- */

void CRegisterSettings::OnBnClickedRecommandedsettings()
{
	CRecommendedSettings		dlg;

	dlg.setStackingTasks(m_pStackingTasks);

	if (dlg.DoModal()==IDOK)
	{
		if (m_pStackingTasks)
			m_pStackingTasks->UpdateTasksMethods();

		CWorkspace				workspace;

		bool bMedianFilter = workspace.value("Register/ApplyMedianFilter").toBool();
		m_tabAdvanced.m_MedianFilter.SetCheck(bMedianFilter);
	};
}

/* ------------------------------------------------------------------- */
