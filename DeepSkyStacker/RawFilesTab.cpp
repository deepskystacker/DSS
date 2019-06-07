// RawFilesTab.cpp : implementation file
//

#include "stdafx.h"
#include "RawFilesTab.h"
#include "Registry.h"
#include "Workspace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRawFilesTab property page

IMPLEMENT_DYNCREATE(CRawFilesTab, CPropertyPage)

CRawFilesTab::CRawFilesTab() : CPropertyPage(CRawFilesTab::IDD)
{
	//{{AFX_DATA_INIT(CRawFilesTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_PREMATURE;
	m_bFirstActivation = TRUE;
}

CRawFilesTab::~CRawFilesTab()
{
}

void CRawFilesTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRawFilesTab)
	DDX_Control(pDX, IDC_REDSCALE, m_RedScale);
	DDX_Control(pDX, IDC_CAMERAWB, m_CameraWB);
	DDX_Control(pDX, IDC_BRIGHTNESS, m_Brightness);
	DDX_Control(pDX, IDC_BLUESCALE, m_BlueScale);
	DDX_Control(pDX, IDC_BILINEAR, m_Bilinear);
	DDX_Control(pDX, IDC_AUTOWB, m_AutoWB);
	DDX_Control(pDX, IDC_AHD, m_AHD);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SUPERPIXELS, m_SuperPixels);
	DDX_Control(pDX, IDC_RAWBAYER, m_RawBayer);
	DDX_Control(pDX, IDC_BLACKPOINTTO0, m_BlackPoint);
}


BEGIN_MESSAGE_MAP(CRawFilesTab, CPropertyPage)
	//{{AFX_MSG_MAP(CRawFilesTab)
	ON_EN_CHANGE(IDC_BLUESCALE, OnChangeBluescale)
	ON_EN_CHANGE(IDC_BRIGHTNESS, OnChangeBrightness)
	ON_EN_CHANGE(IDC_REDSCALE, OnChangeRedscale)
	ON_BN_CLICKED(IDC_BILINEAR, OnBilinear)
	ON_BN_CLICKED(IDC_AHD, OnAhd)
	ON_BN_CLICKED(IDC_SUPERPIXELS, &OnBnClickedSuperpixels)
	ON_BN_CLICKED(IDC_RAWBAYER, &OnBnClickedRawbayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CRawFilesTab message handlers

BOOL CRawFilesTab::OnSetActive() 
{
	if (m_bFirstActivation)
	{
		CWorkspace			workspace;
		CString				strValue;
		DWORD				bValue;

		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("Brighness"), strValue);
		if (!strValue.GetLength())
			strValue = _T("1.0");
		m_Brightness.SetWindowText(strValue);
		
		strValue.Empty();
		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("RedScale"), strValue);
		if (!strValue.GetLength())
			strValue = _T("1.0");
		m_RedScale.SetWindowText(strValue);

		strValue.Empty();
		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlueScale"), strValue);
		if (!strValue.GetLength())
			strValue = _T("1.0");
		m_BlueScale.SetWindowText(strValue);

		bValue = FALSE;
		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("AutoWB"), bValue);
		m_AutoWB.SetCheck(bValue);

		bValue = FALSE;
		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("CameraWB"), bValue);
		m_CameraWB.SetCheck(bValue);

		bValue = FALSE;
		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlackPointTo0"), bValue);
		m_BlackPoint.SetCheck(bValue);

		DWORD				bSuperPixel;

		bValue = FALSE;
		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("SuperPixels"), bSuperPixel);

		DWORD				bRawBayer;

		bValue = FALSE;
		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("RawBayer"), bRawBayer);

		strValue.Empty();
		workspace.GetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("Interpolation"), strValue);
		if (!strValue.GetLength())
			strValue = _T("Bilinear");

		if (bRawBayer)
			m_RawBayer.SetCheck(TRUE);
		else if (bSuperPixel)
			m_SuperPixels.SetCheck(TRUE);
		else if (strValue == _T("Bilinear"))
			m_Bilinear.SetCheck(TRUE);
		else if (strValue == _T("AHD") || strValue == _T("VNG"))
			m_AHD.SetCheck(TRUE);

		UpdateControls();
		m_bFirstActivation = FALSE;
	};

	return CPropertyPage::OnSetActive();
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::UpdateControls()
{
	CString				strValue;
	BOOL				bOk = TRUE;
	BOOL				bInterpolation = TRUE;
	double				fValue;

	m_Brightness.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = FALSE;

	m_RedScale.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = FALSE;

	m_BlueScale.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = FALSE;

//	m_OK.EnableWindow(bOk);
};

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnChangeBluescale() 
{
	UpdateControls();	
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnChangeBrightness() 
{
	UpdateControls();	
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnChangeRedscale() 
{
	UpdateControls();	
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnBilinear() 
{
	m_AHD.SetCheck(FALSE);
	m_RawBayer.SetCheck(FALSE);
	m_SuperPixels.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnAhd() 
{
	m_Bilinear.SetCheck(FALSE);
	m_RawBayer.SetCheck(FALSE);
	m_SuperPixels.SetCheck(FALSE);
}
/* ------------------------------------------------------------------- */

void CRawFilesTab::OnBnClickedSuperpixels()
{
	m_AHD.SetCheck(FALSE);
	m_Bilinear.SetCheck(FALSE);
	m_RawBayer.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnBnClickedRawbayer()
{
	m_AHD.SetCheck(FALSE);
	m_Bilinear.SetCheck(FALSE);
	m_SuperPixels.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::SaveValues()
{
	// Save settings to registry	
	CWorkspace			workspace;
	CString				strValue;

	m_Brightness.GetWindowText(strValue);
	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("Brighness"), strValue);
	
	m_RedScale.GetWindowText(strValue);
	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("RedScale"), strValue);

	m_BlueScale.GetWindowText(strValue);
	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlueScale"), strValue);

	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("AutoWB"), m_AutoWB.GetCheck() ? true : false);

	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("CameraWB"), m_CameraWB.GetCheck() ? true : false);

	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlackPointTo0"), m_BlackPoint.GetCheck() ? true : false);

	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("SuperPixels"), m_SuperPixels.GetCheck() ? true : false);

	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("RawBayer"), m_RawBayer.GetCheck() ? true : false);

	strValue.Empty();
	if (m_Bilinear.GetCheck())
		strValue = _T("Bilinear");
	else if (m_AHD.GetCheck())
		strValue = _T("AHD");

	workspace.SetValue(REGENTRY_BASEKEY_RAWSETTINGS, _T("Interpolation"), strValue);
}

/* ------------------------------------------------------------------- */

