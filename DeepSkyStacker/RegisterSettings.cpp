// RegisterSettings.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"
#include "RegisterSettings.h"
#include "Registry.h"
#include "RawDDPSettings.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "Workspace.h"
#include "RecommandedSettings.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CRegisterSettings dialog


CRegisterSettings::CRegisterSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterSettings)
	//}}AFX_DATA_INIT

	m_bStack = FALSE;
	m_fPercentStack = 80;

	m_bNoDark = FALSE;
	m_bNoFlat = FALSE;
	m_bNoOffset = FALSE;
	m_pStackingTasks = NULL;
	m_bForceRegister = FALSE;
	m_bSettingsOnly	 = FALSE;
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
	DWORD				bHotPixels = 0;

	CDialog::OnInitDialog();

 	CRect				rcSettings;

	m_Rect.GetWindowRect(&rcSettings);
	ScreenToClient(&rcSettings);
	rcSettings.left -= 5;
	rcSettings.top -= 11;

	if (!m_bSettingsOnly)
		m_Sheet.AddPage(&m_tabActions);

	m_Sheet.AddPage(&m_tabAdvanced);

	m_Sheet.EnableStackedTabs( FALSE );
	m_Sheet.Create (this, WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0);

	m_Sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_Sheet.ModifyStyle( 0, WS_TABSTOP );

	// move to left upper corner

	m_Sheet.MoveWindow(&rcSettings, TRUE);
	m_Sheet.ShowWindow(SW_SHOWNA);

	if (!m_bSettingsOnly)
	{
		m_tabActions.m_ForceRegister.SetCheck(m_bForceRegister);
		strValue.Empty();
		workspace.GetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("PercentStack"), strValue);
		if (!strValue.GetLength())
			strValue = _T("80");

		m_tabActions.m_Percent.SetWindowText(strValue);

		bValue = FALSE;
		workspace.GetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("StackAfter"), bValue);
		m_tabActions.m_Stack.SetCheck(bValue);

		m_tabActions.m_Percent.EnableWindow(bValue);

		workspace.GetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("DetectHotPixels"), bHotPixels);
		m_tabActions.m_HotPixels.SetCheck(bHotPixels);
	}
	else
	{
		GetDlgItem(IDC_STACKINGPARAMETERS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECOMMANDEDSETTINGS)->ShowWindow(SW_HIDE);
		m_tabAdvanced.GetDlgItem(IDC_COMPUTEDETECTEDSTARS)->ShowWindow(SW_HIDE);
	};

	DWORD				dwDetectionThreshold = 10;
	DWORD				bMedianFilter = FALSE;

	workspace.GetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("DetectionThreshold"), dwDetectionThreshold);
	m_tabAdvanced.m_PercentSlider.SetRange(2, 98);
	m_tabAdvanced.m_PercentSlider.SetPos(dwDetectionThreshold);
	m_tabAdvanced.m_strFirstLightFrame = m_strFirstLightFrame;

	workspace.GetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("ApplyMedianFilter"), bMedianFilter);
	m_tabAdvanced.m_MedianFilter.SetCheck(bMedianFilter);
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
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CRegisterSettings::OnOK() 
{
	CString				strText;
	CWorkspace			workspace;
	DWORD				dwHotPixels;

	if (!m_bSettingsOnly)
	{
		m_bForceRegister = m_tabActions.m_ForceRegister.GetCheck();

		m_bStack = m_tabActions.m_Stack.GetCheck() ? true : false;
		workspace.SetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("StackAfter"), m_bStack);

		m_tabActions.m_Percent.GetWindowText(strText);
		workspace.SetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("PercentStack"), strText);

		dwHotPixels = m_tabActions.m_HotPixels.GetCheck();
		workspace.SetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("DetectHotPixels"), dwHotPixels);

		m_fPercentStack = _ttof(strText);
	};

	m_dwDetectionThreshold = m_tabAdvanced.m_PercentSlider.GetPos();
	workspace.SetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("DetectionThreshold"), m_dwDetectionThreshold);

	m_bMedianFilter = m_tabAdvanced.m_MedianFilter.GetCheck();
	workspace.SetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("ApplyMedianFilter"), m_bMedianFilter);
	
	workspace.SaveToRegistry();

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
	CStackSettings			dlg;
	CRect					rcCustom;

	if (m_pStackingTasks)
	{
		if (m_pStackingTasks->GetCustomRectangle(rcCustom))
			dlg.SetCustomRectangleAvailability(TRUE, m_pStackingTasks->IsCustomRectangleUsed());
		else
			dlg.SetCustomRectangleAvailability(FALSE);
		dlg.SetDarkFlatBiasTabsVisibility(m_pStackingTasks->AreDarkUsed(), m_pStackingTasks->AreFlatUsed(), m_pStackingTasks->AreBiasUsed());
	}
	else
		dlg.SetCustomRectangleAvailability(FALSE);

	if (!m_tabActions.m_Stack.GetCheck())
		dlg.SetRegisteringOnly(TRUE);

	dlg.SetStackingTasks(m_pStackingTasks);

	if ((dlg.DoModal()==IDOK) && m_pStackingTasks)
		m_pStackingTasks->UpdateTasksMethods();

}

/* ------------------------------------------------------------------- */

void CRegisterSettings::OnBnClickedRecommandedsettings()
{
	CRecommendedSettings		dlg;

	dlg.SetStackingTasks(m_pStackingTasks);

	if (dlg.DoModal()==IDOK)
	{
		if (m_pStackingTasks)
			m_pStackingTasks->UpdateTasksMethods();

		CWorkspace				workspace;
		bool					bMedianFilter;

		workspace.GetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("ApplyMedianFilter"), bMedianFilter);
		m_tabAdvanced.m_MedianFilter.SetCheck(bMedianFilter);
	};
}

/* ------------------------------------------------------------------- */
