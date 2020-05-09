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
	m_bFirstActivation = true;
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
	DDX_Control(pDX, IDC_NOWB, m_NoWB);
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
	ON_BN_CLICKED(IDC_NOWB, OnNoWB)
	ON_BN_CLICKED(IDC_CAMERAWB, OnCameraWB)
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
		QString				strValue;

		strValue = workspace.value("RawDDP/Brightness", "1.0").toString();
		m_Brightness.SetWindowText((LPCTSTR)strValue.utf16());

		strValue = workspace.value("RawDDP/RedScale", "1.0").toString();
		m_RedScale.SetWindowText((LPCTSTR)strValue.utf16());

		strValue = workspace.value("RawDDP/BlueScale", "1.0").toString();
		m_BlueScale.SetWindowText((LPCTSTR)strValue.utf16());

		//
		// Replace Auto WB processing with NO WB processing
		//
		m_NoWB.SetCheck(workspace.value("RawDDP/NoWB", false).toBool());

		m_CameraWB.SetCheck(workspace.value("RawDDP/CameraWB", false).toBool());

		m_BlackPoint.SetCheck(workspace.value("RawDDP/BlackPointTo0", false).toBool());

		bool isSuperPixel;
		bool isRawBayer;

		isSuperPixel = workspace.value("RawDDP/SuperPixels", false).toBool();
		isRawBayer = workspace.value("RawDDP/RawBayer", false).toBool();

		strValue = workspace.value("RawDDP/Interpolation", "Bilinear").toString();

		if (isRawBayer)
			m_RawBayer.SetCheck(true);
		else if (isSuperPixel)
			m_SuperPixels.SetCheck(true);
		else if (strValue == _T("Bilinear"))
			m_Bilinear.SetCheck(true);
		else if (strValue == _T("AHD") || strValue == _T("VNG"))
			m_AHD.SetCheck(true);

		UpdateControls();
		m_bFirstActivation = false;
	};

	return CPropertyPage::OnSetActive();
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::UpdateControls()
{
	CString				strValue;
	bool				bOk = true;
	bool				bInterpolation = true;
	double				fValue;

	m_Brightness.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = false;

	m_RedScale.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = false;

	m_BlueScale.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = false;

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

void CRawFilesTab::OnNoWB()
{
	bool value = m_NoWB.GetCheck();
	if (value)
	{
		m_CameraWB.SetCheck(false);
	}
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnCameraWB()
{
	bool value = m_CameraWB.GetCheck();
	if (value)
	{
		m_NoWB.SetCheck(false);
	}
}

/* ------------------------------------------------------------------- */


void CRawFilesTab::OnBilinear()
{
	m_AHD.SetCheck(false);
	m_RawBayer.SetCheck(false);
	m_SuperPixels.SetCheck(false);
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnAhd()
{
	m_Bilinear.SetCheck(false);
	m_RawBayer.SetCheck(false);
	m_SuperPixels.SetCheck(false);
}
/* ------------------------------------------------------------------- */

void CRawFilesTab::OnBnClickedSuperpixels()
{
	m_AHD.SetCheck(false);
	m_Bilinear.SetCheck(false);
	m_RawBayer.SetCheck(false);
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::OnBnClickedRawbayer()
{
	m_AHD.SetCheck(false);
	m_Bilinear.SetCheck(false);
	m_SuperPixels.SetCheck(false);
}

/* ------------------------------------------------------------------- */

void CRawFilesTab::SaveValues()
{
	// Save settings to registry
	CWorkspace			workspace;
	CString				strValue;

	m_Brightness.GetWindowText(strValue);
	workspace.setValue("RawDDP/Brightness", QString((QChar *)strValue.GetBuffer()));

	m_RedScale.GetWindowText(strValue);
	workspace.setValue("RawDDP/RedScale", QString((QChar *)strValue.GetBuffer()));

	m_BlueScale.GetWindowText(strValue);
	workspace.setValue("RawDDP/BlueScale", QString((QChar *)strValue.GetBuffer()));

	workspace.setValue("RawDDP/NoWB", m_NoWB.GetCheck() ? true : false);

	workspace.setValue("RawDDP/CameraWB", m_CameraWB.GetCheck() ? true : false);

	workspace.setValue("RawDDP/BlackPointTo0", m_BlackPoint.GetCheck() ? true : false);

	workspace.setValue("RawDDP/SuperPixels", m_SuperPixels.GetCheck() ? true : false);

	workspace.setValue("RawDDP/RawBayer", m_RawBayer.GetCheck() ? true : false);

	strValue.Empty();
	if (m_Bilinear.GetCheck())
		strValue = _T("Bilinear");
	else if (m_AHD.GetCheck())
		strValue = _T("AHD");

	workspace.setValue("RawDDP/Interpolation", QString((QChar *)strValue.GetBuffer()));
}

/* ------------------------------------------------------------------- */

