// ResultParameters.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "ResultParameters.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"

// CIntermediateFiles dialog

IMPLEMENT_DYNAMIC(CIntermediateFiles, CChildPropertyPage)

/* ------------------------------------------------------------------- */

CIntermediateFiles::CIntermediateFiles()
	: CChildPropertyPage(CIntermediateFiles::IDD)
{
	m_bFirstActivation = TRUE;
	m_bRegisteringOnly = FALSE;
}

/* ------------------------------------------------------------------- */

CIntermediateFiles::~CIntermediateFiles()
{
}

/* ------------------------------------------------------------------- */

void CIntermediateFiles::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CREATEINTERMEDIATE, m_CreateIntermediates);
	DDX_Control(pDX, IDC_SAVECALIBRATED, m_SaveCalibrated);
	DDX_Control(pDX, IDC_SAVEDEBAYERED, m_SaveDebayered);
	DDX_Control(pDX, IDC_SAVEASTIFF, m_SaveAsTIFF);
	DDX_Control(pDX, IDC_SAVEASFITS, m_SaveAsFITS);
	DDX_Control(pDX, IDC_TITLE, m_Title);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CIntermediateFiles, CChildPropertyPage)
	ON_BN_CLICKED(IDC_CREATEINTERMEDIATE, &CIntermediateFiles::OnBnClickedCreateIntermediates)
	ON_BN_CLICKED(IDC_SAVECALIBRATED, &CIntermediateFiles::OnBnClickedSaveCalibrated)
	ON_BN_CLICKED(IDC_SAVEDEBAYERED, &CIntermediateFiles::OnBnClickedSaveDebayered)
	ON_BN_CLICKED(IDC_SAVEASTIFF, &CIntermediateFiles::OnBnClickedSaveAsTIFF)
	ON_BN_CLICKED(IDC_SAVEASFITS, &CIntermediateFiles::OnBnClickedSaveAsFITS)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CIntermediateFiles::UpdateControls()
{
	CStackSettings *	pDialog = dynamic_cast<CStackSettings *>(GetParent()->GetParent());
	BOOL				bEnable;

	bEnable = m_SaveCalibrated.GetCheck() || m_CreateIntermediates.GetCheck();

	m_SaveAsTIFF.EnableWindow(TRUE);
	m_SaveAsFITS.EnableWindow(TRUE);
	m_SaveDebayered.EnableWindow(m_SaveCalibrated.GetCheck());

	if (pDialog)
		pDialog->UpdateControls();
};

/* ------------------------------------------------------------------- */

BOOL CIntermediateFiles::OnSetActive()
{
	if (m_bFirstActivation)
	{
		m_Title.SetTextColor(RGB(0, 0, 0));
		m_Title.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

		m_CreateIntermediates.SetCheck(m_bCreateIntermediates);
		m_SaveCalibrated.SetCheck(m_bSaveCalibrated);
		m_SaveDebayered.SetCheck(m_bSaveDebayered);

		m_CreateIntermediates.EnableWindow(!m_bRegisteringOnly);
		UpdateControls();
		m_bFirstActivation = FALSE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */
// CIntermediateFiles message handlers

void CIntermediateFiles::OnBnClickedCreateIntermediates()
{
	m_bCreateIntermediates = m_CreateIntermediates.GetCheck();
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CIntermediateFiles::OnBnClickedSaveCalibrated()
{
	m_bSaveCalibrated = m_SaveCalibrated.GetCheck();
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CIntermediateFiles::OnBnClickedSaveDebayered()
{
	m_bSaveDebayered = m_SaveDebayered.GetCheck();
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CIntermediateFiles::OnBnClickedSaveAsTIFF()
{
	if (m_SaveAsTIFF.GetCheck())
	{
		m_lSaveFormat = IFF_TIFF;
		m_SaveAsFITS.SetCheck(FALSE);
	};
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CIntermediateFiles::OnBnClickedSaveAsFITS()
{
	if (m_SaveAsFITS.GetCheck())
	{
		m_lSaveFormat = IFF_FITS;
		m_SaveAsTIFF.SetCheck(FALSE);
	};
	UpdateControls();
};

/* ------------------------------------------------------------------- */
