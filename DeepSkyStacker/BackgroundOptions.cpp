// CheckAbove.cpp : implementation file
//
#include "stdafx.h"
#include "deepskystacker.h"
#include "Registry.h"
#include "FrameList.h"
#include "BackgroundOptions.h"
#include "Workspace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CBackgroundOptions dialog


CBackgroundOptions::CBackgroundOptions(CWnd* pParent /*=nullptr*/)
	: CDialog(CBackgroundOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBackgroundOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_CalibrationMode		   = CAllStackingTasks::GetBackgroundCalibrationMode();
	m_CalibrationInterpolation = CAllStackingTasks::GetBackgroundCalibrationInterpolation();
	m_RGBCalibrationMethod	   = CAllStackingTasks::GetRGBBackgroundCalibrationMethod();
}

/* ------------------------------------------------------------------- */

void CBackgroundOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackgroundOptions)
	DDX_Control(pDX, IDC_LINEAR, m_Linear);
	DDX_Control(pDX, IDC_RATIONAL, m_Rational);
	DDX_Control(pDX, IDC_CALIBRATIONPREVIEW, m_CalibrationPreview);
	DDX_Control(pDX, IDC_NONE, m_None);
	DDX_Control(pDX, IDC_MINIMUM, m_Minimum);
	DDX_Control(pDX, IDC_MAXIMUM, m_Maximum);
	DDX_Control(pDX, IDC_MIDDLE, m_Middle);
	DDX_Control(pDX, IDC_RGBCALIBRATIONPREVIEW, m_RGBCalibrationPreview);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CBackgroundOptions, CDialog)
	//{{AFX_MSG_MAP(CBackgroundOptions)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LINEAR, &CBackgroundOptions::OnBnClickedLinear)
	ON_BN_CLICKED(IDC_RATIONAL, &CBackgroundOptions::OnBnClickedRational)
	ON_BN_CLICKED(IDC_NONE, &CBackgroundOptions::OnBnClickedNone)
	ON_BN_CLICKED(IDC_MINIMUM, &CBackgroundOptions::OnBnClickedMinimum)
	ON_BN_CLICKED(IDC_MIDDLE, &CBackgroundOptions::OnBnClickedMiddle)
	ON_BN_CLICKED(IDC_MAXIMUM, &CBackgroundOptions::OnBnClickedMaximum)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackgroundOptions message handlers

/* ------------------------------------------------------------------- */

BOOL CBackgroundOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ((m_CalibrationMode == BCM_NONE) || (m_CalibrationMode == BCM_PERCHANNEL))
		m_None.SetCheck(TRUE);
	else if (m_RGBCalibrationMethod == RBCM_MINIMUM)
		m_Minimum.SetCheck(TRUE);
	else if (m_RGBCalibrationMethod == RBCM_MIDDLE)
		m_Middle.SetCheck(TRUE);
	else
		m_Maximum.SetCheck(TRUE);

	if (m_CalibrationInterpolation == BCI_LINEAR)
		m_Linear.SetCheck(TRUE);
	else
		m_Rational.SetCheck(TRUE);

	UpdateCalibrationPreview();
	UpdateRGBCalibrationPreview();

	return TRUE;
};

/* ------------------------------------------------------------------- */

void CBackgroundOptions::OnOK()
{
	CWorkspace			workspace;

	if (m_Linear.GetCheck())
		m_CalibrationInterpolation = BCI_LINEAR;
	else
		m_CalibrationInterpolation = BCI_RATIONAL;

	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibrationInterpolation"), (DWORD)m_CalibrationInterpolation);

	if (!m_None.GetCheck())
	{
		m_CalibrationMode = BCM_RGB;
		if (m_Minimum.GetCheck())
			m_RGBCalibrationMethod = RBCM_MINIMUM;
		else if (m_Middle.GetCheck())
			m_RGBCalibrationMethod = RBCM_MIDDLE;
		else
			m_RGBCalibrationMethod = RBCM_MAXIMUM;


		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("RGBBackgroundCalibrationMethod"), (DWORD)m_RGBCalibrationMethod);
	}
	else if (m_CalibrationMode == BCM_RGB)
	{
		m_CalibrationMode = BCM_PERCHANNEL;
	};

	CDialog::OnOK();
}

/* ------------------------------------------------------------------- */

void CBackgroundOptions::UpdateCalibrationPreview()
{
	if (m_Linear.GetCheck())
		m_CalibrationPreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_CALIBRATION_LINEAR)));
	else
		m_CalibrationPreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_CALIBRATION_RATIONAL)));
};

/* ------------------------------------------------------------------- */

void CBackgroundOptions::UpdateRGBCalibrationPreview()
{
	if (m_None.GetCheck())
		m_RGBCalibrationPreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_RGBCALIBRATION_NONE)));
	else if (m_Minimum.GetCheck())
		m_RGBCalibrationPreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_RGBCALIBRATION_MINIMUM)));
	else if (m_Middle.GetCheck())
		m_RGBCalibrationPreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_RGBCALIBRATION_MIDDLE)));
	else if (m_Maximum.GetCheck())
		m_RGBCalibrationPreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_RGBCALIBRATION_MAXIMUM)));
};

/* ------------------------------------------------------------------- */

void CBackgroundOptions::OnBnClickedLinear()
{
	if (m_Linear.GetCheck())
	{
		m_Rational.SetCheck(FALSE);
		UpdateCalibrationPreview();
	};
}

/* ------------------------------------------------------------------- */

void CBackgroundOptions::OnBnClickedRational()
{
	if (m_Rational.GetCheck())
	{
		m_Linear.SetCheck(FALSE);
		UpdateCalibrationPreview();
	};
}

/* ------------------------------------------------------------------- */

void CBackgroundOptions::OnBnClickedNone()
{
	if (m_None.GetCheck())
	{
		m_Minimum.SetCheck(FALSE);
		m_Middle.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		UpdateRGBCalibrationPreview();
	};
}

/* ------------------------------------------------------------------- */

void CBackgroundOptions::OnBnClickedMinimum()
{
	if (m_Minimum.GetCheck())
	{
		m_None.SetCheck(FALSE);
		m_Middle.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		UpdateRGBCalibrationPreview();
	};
}

/* ------------------------------------------------------------------- */

void CBackgroundOptions::OnBnClickedMiddle()
{
	if (m_Middle.GetCheck())
	{
		m_None.SetCheck(FALSE);
		m_Minimum.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		UpdateRGBCalibrationPreview();
	};
}

/* ------------------------------------------------------------------- */

void CBackgroundOptions::OnBnClickedMaximum()
{
	if (m_Maximum.GetCheck())
	{
		m_None.SetCheck(FALSE);
		m_Minimum.SetCheck(FALSE);
		m_Middle.SetCheck(FALSE);
		UpdateRGBCalibrationPreview();
	};
}

/* ------------------------------------------------------------------- */
