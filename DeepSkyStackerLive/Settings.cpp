// ExplorerBar.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "DeepSkyStackerLiveDlg.h"
#include "Settings.h"
#include "RegisterEngine.h"
#include <FolderDlg.h>
#include "EmailSettings.h"

/* ------------------------------------------------------------------- */
// CSettingsTab dialog

IMPLEMENT_DYNAMIC(CSettingsTab, CDialog)

CSettingsTab::CSettingsTab(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsTab::IDD, pParent)
{
}

/* ------------------------------------------------------------------- */

CSettingsTab::~CSettingsTab()
{
}

/* ------------------------------------------------------------------- */

void CSettingsTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STACKING, m_Stacking);
	DDX_Control(pDX, IDC_DONTSTACK, m_DontStack);
	DDX_Control(pDX, IDC_MINIMAGES, m_MinImages);
	DDX_Control(pDX, IDC_WARN_SCORE, m_Warn_Score);
	DDX_Control(pDX, IDC_WARN_STARS, m_Warn_Stars);
	DDX_Control(pDX, IDC_WARN_FWHM, m_Warn_FWHM);
	DDX_Control(pDX, IDC_WARN_OFFSET, m_Warn_Offset);
	DDX_Control(pDX, IDC_WARN_ANGLE, m_Warn_Angle);
	DDX_Control(pDX, IDC_WARN_SKYBACKGROUND, m_Warn_SkyBackground);
	DDX_Control(pDX, IDC_DONTSTACK_SCORE, m_DontStack_Score);
	DDX_Control(pDX, IDC_DONTSTACK_STARS, m_DontStack_Stars);
	DDX_Control(pDX, IDC_DONTSTACK_FWHM, m_DontStack_FWHM);
	DDX_Control(pDX, IDC_DONTSTACK_OFFSET, m_DontStack_Offset);
	DDX_Control(pDX, IDC_DONTSTACK_ANGLE, m_DontStack_Angle);
	DDX_Control(pDX, IDC_DONTSTACK_SKYBACKGROUND, m_DontStack_SkyBackground);
	DDX_Control(pDX, IDC_MOVENONSTACKABLE, m_MoveNonStackable);
	DDX_Control(pDX, IDC_SCORE, m_Score);
	DDX_Control(pDX, IDC_STARCOUNT, m_Stars);
	DDX_Control(pDX, IDC_FWHM, m_FWHM);
	DDX_Control(pDX, IDC_OFFSET, m_Offset);
	DDX_Control(pDX, IDC_ANGLE, m_Angle);
	DDX_Control(pDX, IDC_MAX_SKYBACKGROUND, m_SkyBackground);

	DDX_Control(pDX, IDC_WARNINGS, m_Warnings);
	DDX_Control(pDX, IDC_WARN_SOUND, m_Warn_Sound);
	DDX_Control(pDX, IDC_WARN_FLASH, m_Warn_Flash);
	DDX_Control(pDX, IDC_WARN_EMAIL, m_Warn_Email);
	DDX_Control(pDX, IDC_WARN_FILE, m_Warn_File);
	DDX_Control(pDX, IDC_WARNINGFILEFOLDER, m_Warn_FileFolder);
	DDX_Control(pDX, IDC_EMAIL, m_Warn_EmailAddress);
	DDX_Control(pDX, IDC_RESETEMAILCOUNT, m_Warn_ResetEmailCount);

	DDX_Control(pDX, IDC_OPTIONS, m_Options);
	DDX_Control(pDX, IDC_SAVESTACKEDIMAGE, m_SaveStackedImage);
	DDX_Control(pDX, IDC_IMAGECOUNT, m_ImageCount);
	DDX_Control(pDX, IDC_STACKEDOUTPUTFOLDER, m_StackedOutputFolder);

	DDX_Control(pDX, IDC_APPLYCHANGES, m_ApplyChanges);
	DDX_Control(pDX, IDC_CANCELCHANGES, m_CancelChanges);

	DDX_Control(pDX, IDC_FILTERS, m_Filters);
	DDX_Control(pDX, IDC_PROCESS_RAW, m_Process_RAW);
	DDX_Control(pDX, IDC_PROCESS_FITS, m_Process_FITS);
	DDX_Control(pDX, IDC_PROCESS_TIFF, m_Process_TIFF);
	DDX_Control(pDX, IDC_PROCESS_OTHERS, m_Process_Others);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CSettingsTab, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_APPLYCHANGES, OnApplyChanges)
	ON_BN_CLICKED(IDC_CANCELCHANGES, OnCancelChanges)
	ON_NOTIFY(NM_LINKCLICK, IDC_WARNINGFILEFOLDER, OnWarningFileFolder)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKEDOUTPUTFOLDER, OnStackedOutputFolder)
	ON_NOTIFY(NM_LINKCLICK, IDC_EMAIL, OnEmailAddress)

	ON_BN_CLICKED(IDC_DONTSTACK, OnChangeSetting)
	ON_EN_CHANGE(IDC_MINIMAGES, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_SCORE, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_STARS, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_FWHM, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_OFFSET, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_ANGLE, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_SKYBACKGROUND, OnChangeSetting)
	ON_BN_CLICKED(IDC_DONTSTACK_SCORE, OnChangeSetting)
	ON_BN_CLICKED(IDC_DONTSTACK_STARS, OnChangeSetting)
	ON_BN_CLICKED(IDC_DONTSTACK_FWHM, OnChangeSetting)
	ON_BN_CLICKED(IDC_DONTSTACK_OFFSET, OnChangeSetting)
	ON_BN_CLICKED(IDC_DONTSTACK_ANGLE, OnChangeSetting)
	ON_BN_CLICKED(IDC_DONTSTACK_SKYBACKGROUND, OnChangeSetting)
	ON_BN_CLICKED(IDC_MOVENONSTACKABLE, OnChangeSetting)
	ON_EN_CHANGE(IDC_SCORE, OnChangeSetting)
	ON_EN_CHANGE(IDC_STARCOUNT, OnChangeSetting)
	ON_EN_CHANGE(IDC_FWHM, OnChangeSetting)
	ON_EN_CHANGE(IDC_OFFSET, OnChangeSetting)
	ON_EN_CHANGE(IDC_ANGLE, OnChangeSetting)
	ON_EN_CHANGE(IDC_MAX_SKYBACKGROUND, OnChangeSetting)

	ON_BN_CLICKED(IDC_WARN_SOUND, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_FLASH, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_EMAIL, OnChangeSetting)
	ON_BN_CLICKED(IDC_WARN_FILE, OnChangeSetting)

	ON_BN_CLICKED(IDC_SAVESTACKEDIMAGE, OnChangeSetting)
	ON_EN_CHANGE(IDC_IMAGECOUNT, OnChangeSetting)
	ON_BN_CLICKED(IDC_RESETEMAILCOUNT, OnResetEmailCount)

	ON_BN_CLICKED(IDC_PROCESS_RAW, OnChangeSetting)
	ON_BN_CLICKED(IDC_PROCESS_FITS, OnChangeSetting)
	ON_BN_CLICKED(IDC_PROCESS_TIFF, OnChangeSetting)
	ON_BN_CLICKED(IDC_PROCESS_OTHERS, OnChangeSetting)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CSettingsTab::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_ControlPos.MoveControls();
}

/* ------------------------------------------------------------------- */

void CSettingsTab::OnChangeSetting()
{
	m_bDirty = TRUE;
	UpdateControls();
};

/* ------------------------------------------------------------------- */

inline	void	SetEditValue(CEdit & edit, double fValue)
{
	CString			strFormat;

	strFormat.Format(_T("%.1f"), fValue);
	edit.SetWindowText(strFormat);
};

inline	void	SetEditValue(CEdit & edit, DWORD dwValue)
{
	CString			strFormat;

	strFormat.Format(_T("%ld"), dwValue);
	edit.SetWindowText(strFormat);
};

inline	double	GetEditDoubleValue(CEdit & edit)
{
	CString			strValue;

	edit.GetWindowText(strValue);
	return _tstof(strValue);
};

inline	DWORD	GetEditDWORDValue(CEdit & edit)
{
	CString			strValue;

	edit.GetWindowText(strValue);
	return _tstol(strValue);
};

void CSettingsTab::UpdateFromRegistry()
{
	m_LiveSettings.LoadFromRegistry();

	m_Warn_Score.SetCheck(m_LiveSettings.IsWarning_Score());
	m_Warn_Stars.SetCheck(m_LiveSettings.IsWarning_Stars());
	m_Warn_FWHM.SetCheck(m_LiveSettings.IsWarning_FWHM());
	m_Warn_Offset.SetCheck(m_LiveSettings.IsWarning_Offset());
	m_Warn_Angle.SetCheck(m_LiveSettings.IsWarning_Angle());
	m_Warn_SkyBackground.SetCheck(m_LiveSettings.IsWarning_SkyBackground());

	m_DontStack_Score.SetCheck(m_LiveSettings.IsDontStack_Score());
	m_DontStack_Stars.SetCheck(m_LiveSettings.IsDontStack_Stars());
	m_DontStack_FWHM.SetCheck(m_LiveSettings.IsDontStack_FWHM());
	m_DontStack_Offset.SetCheck(m_LiveSettings.IsDontStack_Offset());
	m_DontStack_Angle.SetCheck(m_LiveSettings.IsDontStack_Angle());
	m_DontStack_SkyBackground.SetCheck(m_LiveSettings.IsDontStack_SkyBackground());

	m_DontStack.SetCheck(m_LiveSettings.IsDontStack_Delayed());

	m_SaveStackedImage.SetCheck(m_LiveSettings.IsStack_Save());
	m_MoveNonStackable.SetCheck(m_LiveSettings.IsStack_Move());

	m_Warn_Flash.SetCheck(m_LiveSettings.IsWarning_Flash());
	m_Warn_Sound.SetCheck(m_LiveSettings.IsWarning_Sound());
	m_Warn_Email.SetCheck(m_LiveSettings.IsWarning_Email());
	m_Warn_File.SetCheck(m_LiveSettings.IsWarning_File());

	m_Process_RAW.SetCheck(m_LiveSettings.IsProcess_RAW());
	m_Process_FITS.SetCheck(m_LiveSettings.IsProcess_FITS());
	m_Process_TIFF.SetCheck(m_LiveSettings.IsProcess_TIFF());
	m_Process_Others.SetCheck(m_LiveSettings.IsProcess_Others());

	m_LiveSettings.GetWarning_FileFolder(m_strWarnFileFolder);
	if (m_strWarnFileFolder.GetLength())
		m_Warn_FileFolder.SetWindowText(m_strWarnFileFolder);

	m_LiveSettings.GetStackedOutputFolder(m_strStackedOutputFolder);
	if (m_strStackedOutputFolder.GetLength())
		m_StackedOutputFolder.SetWindowText(m_strStackedOutputFolder);

	SetEditValue(m_Score, m_LiveSettings.GetScore());
	SetEditValue(m_Stars, m_LiveSettings.GetStars());
	SetEditValue(m_FWHM, m_LiveSettings.GetFWHM());
	SetEditValue(m_Offset, m_LiveSettings.GetOffset());
	SetEditValue(m_Angle, m_LiveSettings.GetAngle());
	SetEditValue(m_SkyBackground, m_LiveSettings.GetSkyBackground());

	SetEditValue(m_MinImages, m_LiveSettings.GetMinImages());
	SetEditValue(m_ImageCount, m_LiveSettings.GetSaveCount());

	CString				strEmail;

	m_LiveSettings.GetEmailSettings(strEmail);
	if (strEmail.GetLength())
	{
		m_Warn_EmailAddress.SetWindowText(strEmail);
		m_Warn_Email.EnableWindow(TRUE);
	}
	else
	{
		m_Warn_EmailAddress.SetWindowText(m_strEmail);
		m_Warn_Email.SetCheck(FALSE);
		m_Warn_Email.EnableWindow(FALSE);
	};

	m_bDirty = FALSE;
};

/* ------------------------------------------------------------------- */

void CSettingsTab::SaveToRegistry()
{
	m_LiveSettings.SetWarning_Score(m_Warn_Score.GetCheck());
	m_LiveSettings.SetWarning_Stars(m_Warn_Stars.GetCheck());
	m_LiveSettings.SetWarning_FWHM(m_Warn_FWHM.GetCheck());
	m_LiveSettings.SetWarning_Offset(m_Warn_Offset.GetCheck());
	m_LiveSettings.SetWarning_Angle(m_Warn_Angle.GetCheck());
	m_LiveSettings.SetWarning_SkyBackground(m_Warn_SkyBackground.GetCheck());

	m_LiveSettings.SetDontStack_Score(m_DontStack_Score.GetCheck());
	m_LiveSettings.SetDontStack_Stars(m_DontStack_Stars.GetCheck());
	m_LiveSettings.SetDontStack_FWHM(m_DontStack_FWHM.GetCheck());
	m_LiveSettings.SetDontStack_Offset(m_DontStack_Offset.GetCheck());
	m_LiveSettings.SetDontStack_Angle(m_DontStack_Angle.GetCheck());
	m_LiveSettings.SetDontStack_SkyBackground(m_DontStack_SkyBackground.GetCheck());

	m_LiveSettings.SetDontStack_Delayed(m_DontStack.GetCheck());

	m_LiveSettings.SetStack_Save(m_SaveStackedImage.GetCheck());
	m_LiveSettings.SetStack_Move(m_MoveNonStackable.GetCheck());

	m_LiveSettings.SetWarning_Flash(m_Warn_Flash.GetCheck());
	m_LiveSettings.SetWarning_Sound(m_Warn_Sound.GetCheck());
	m_LiveSettings.SetWarning_Email(m_Warn_Email.GetCheck());
	m_LiveSettings.SetWarning_File(m_Warn_File.GetCheck());

	m_LiveSettings.SetProcess_RAW(m_Process_RAW.GetCheck());
	m_LiveSettings.SetProcess_FITS(m_Process_FITS.GetCheck());
	m_LiveSettings.SetProcess_TIFF(m_Process_TIFF.GetCheck());
	m_LiveSettings.SetProcess_Others(m_Process_Others.GetCheck());

	m_LiveSettings.SetScore(GetEditDoubleValue(m_Score));
	m_LiveSettings.SetStars(GetEditDWORDValue(m_Stars));
	m_LiveSettings.SetSkyBackground(GetEditDWORDValue(m_SkyBackground));
	m_LiveSettings.SetFWHM(GetEditDoubleValue(m_FWHM));
	m_LiveSettings.SetOffset(GetEditDoubleValue(m_Offset));
	m_LiveSettings.SetAngle(GetEditDoubleValue(m_Angle));

	m_LiveSettings.SetMinImages(GetEditDWORDValue(m_MinImages));
	m_LiveSettings.SetSaveCount(GetEditDWORDValue(m_ImageCount));

	m_LiveSettings.SetWarning_FileFolder(m_strWarnFileFolder);
	m_LiveSettings.SetStackedOutputFolder(m_strStackedOutputFolder);

	m_LiveSettings.SaveToRegistry();
	m_bDirty = FALSE;
};

/* ------------------------------------------------------------------- */

static void InitLabel(CLabel & label, BOOL bMain = FALSE)
{
	label.SetLink(TRUE, TRUE);
	label.SetTransparent(TRUE);
	label.SetLinkCursor(LoadCursor(NULL,IDC_HAND));
	label.SetFont3D(FALSE);
	//label.SetTextColor(RGB(255, 255, 255));
//	label.SetText3DHiliteColor(RGB(0, 0, 0));

	if (bMain)
	{
		//label.SetFontBold(TRUE);
		label.SetTextColor(RGB(109, 23, 7));
	};
};

/* ------------------------------------------------------------------- */

BOOL CSettingsTab::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ControlPos.SetParent(this);

	m_Warn_EmailAddress.GetWindowText(m_strEmail);

	m_ControlPos.AddControl(IDC_STACKING, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_WARNINGS, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_OPTIONS, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_FILTERS, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_EMAIL, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_WARNINGFILEFOLDER, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_STACKEDOUTPUTFOLDER, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_APPLYCHANGES, CP_MOVE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_CANCELCHANGES, CP_MOVE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_RESETEMAILCOUNT, CP_MOVE_HORIZONTAL);

	InitLabel(m_Warn_FileFolder);
	InitLabel(m_StackedOutputFolder);
	InitLabel(m_Warn_EmailAddress);

	UpdateFromRegistry();
	UpdateControls();

	return TRUE;  
}

/* ------------------------------------------------------------------- */

BOOL CSettingsTab::Close()
{

	return TRUE;
};

/* ------------------------------------------------------------------- */

void CSettingsTab::OnApplyChanges()
{
	SaveToRegistry();
	UpdateLiveSettings();
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CSettingsTab::OnCancelChanges()
{
	UpdateFromRegistry();
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CSettingsTab::UpdateControls()
{
	m_CancelChanges.EnableWindow(m_bDirty);
	m_ApplyChanges.EnableWindow(m_bDirty);
};

/* ------------------------------------------------------------------- */

void CSettingsTab::OnWarningFileFolder( NMHDR * pNotifyStruct, LRESULT * result )
{
	BOOL					bResult = FALSE;
	CString					strFolder;
	CString					strTitle;
	
	strFolder = m_strWarnFileFolder;

	CFolderDlg				dlg(FALSE, strFolder, this);

	strTitle.LoadString(IDS_SELECTWARNINGFOLDER);
	dlg.SetTitle(strTitle);

	if (dlg.DoModal() == IDOK)
	{
		strFolder = dlg.GetFolderName();
		m_Warn_FileFolder.SetWindowText(strFolder);
		m_strWarnFileFolder = strFolder;
		bResult = TRUE;
		m_bDirty = TRUE;
		UpdateControls();
	};
};

/* ------------------------------------------------------------------- */

void CSettingsTab::OnStackedOutputFolder( NMHDR * pNotifyStruct, LRESULT * result )
{
	BOOL					bResult = FALSE;
	CString					strFolder;
	CString					strTitle;
	
	strFolder = m_strStackedOutputFolder;

	CFolderDlg				dlg(FALSE, strFolder, this);

	strTitle.LoadString(IDS_SELECTSTACKEDFOLDER);
	dlg.SetTitle(strTitle);

	if (dlg.DoModal() == IDOK)
	{
		strFolder = dlg.GetFolderName();
		m_StackedOutputFolder.SetWindowText(strFolder);
		m_strStackedOutputFolder = strFolder;
		bResult = TRUE;
		m_bDirty = TRUE;
		UpdateControls();
	};
};

/* ------------------------------------------------------------------- */

void CSettingsTab::OnEmailAddress( NMHDR * pNotifyStruct, LRESULT * result )
{
	CEmailSettings			dlg;

	dlg.SetLiveSettings(&m_LiveSettings);

	if (dlg.DoModal()==IDOK)
	{
		CString				strEmail;

		m_LiveSettings.GetEmailSettings(strEmail);
		if (strEmail.GetLength())
		{
			m_Warn_EmailAddress.SetWindowText(strEmail);
			m_Warn_Email.SetCheck(TRUE);
			m_Warn_Email.EnableWindow(TRUE);
		}
		else
		{
			m_Warn_EmailAddress.SetWindowText(m_strEmail);
			m_Warn_Email.SetCheck(FALSE);
			m_Warn_Email.EnableWindow(FALSE);
		};
		m_bDirty = TRUE;
		UpdateControls();
	};
};

/* ------------------------------------------------------------------- */

void CSettingsTab::OnResetEmailCount()
{
	::ResetEmailCount();
	m_Warn_ResetEmailCount.ShowWindow(SW_HIDE);
};

/* ------------------------------------------------------------------- */
