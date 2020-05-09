// ResultParameters.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "AlignmentParameters.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"

// CAlignmentParameters dialog

IMPLEMENT_DYNAMIC(CAlignmentParameters, CChildPropertyPage)

/* ------------------------------------------------------------------- */

CAlignmentParameters::CAlignmentParameters()
	: CChildPropertyPage(CAlignmentParameters::IDD)
{
	m_bFirstActivation = true;
	m_Alignment = 0;
}

/* ------------------------------------------------------------------- */

CAlignmentParameters::~CAlignmentParameters()
{
}

/* ------------------------------------------------------------------- */

void CAlignmentParameters::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AUTO, m_Automatic);
	DDX_Control(pDX, IDC_BISQUARED, m_Bisquared);
	DDX_Control(pDX, IDC_BICUBIC, m_Bicubic);
	DDX_Control(pDX, IDC_BILINEAR, m_Bilinear);
	DDX_Control(pDX, IDC_NOALIGNMENT, m_NoAlignment);
	DDX_Control(pDX, IDC_ALIGNMENTTEXT, m_Explanation);
	DDX_Control(pDX, IDC_TITLE, m_Title);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CAlignmentParameters, CChildPropertyPage)
	ON_BN_CLICKED(IDC_AUTO, &CAlignmentParameters::OnBnClickedAutomatic)
	ON_BN_CLICKED(IDC_BILINEAR, &CAlignmentParameters::OnBnClickedBilinear)
	ON_BN_CLICKED(IDC_BISQUARED, &CAlignmentParameters::OnBnClickedBisquared)
	ON_BN_CLICKED(IDC_BICUBIC, &CAlignmentParameters::OnBnClickedBicubic)
	ON_BN_CLICKED(IDC_NOALIGNMENT, &CAlignmentParameters::OnBnClickedNoAlignment)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CAlignmentParameters::UpdateControls()
{
	CStackSettings *	pDialog = dynamic_cast<CStackSettings *>(GetParent()->GetParent());

	if (m_Automatic.GetCheck())
		m_Alignment = 0;
	else if (m_Bilinear.GetCheck())
		m_Alignment = 2;
	else if (m_Bisquared.GetCheck())
		m_Alignment = 3;
	else if (m_Bicubic.GetCheck())
		m_Alignment = 4;
	else if (m_NoAlignment.GetCheck())
		m_Alignment = 5;

	CString				strText;

	switch (m_Alignment)
	{
	case 0 :
	case 1 :
		strText.LoadString(IDS_ALIGNMENT_AUTO);
		break;
	case 2 :
		strText.LoadString(IDS_ALIGNMENT_BILINEAR);
		break;
	case 3 :
		strText.LoadString(IDS_ALIGNMENT_BISQUARED);
		break;
	case 4 :
		strText.LoadString(IDS_ALIGNMENT_BICUBIC);
		break;
	};

	m_Explanation.SetWindowText(strText);

	if (pDialog)
		pDialog->UpdateControls();
};

/* ------------------------------------------------------------------- */

BOOL CAlignmentParameters::OnSetActive()
{
	if (m_bFirstActivation)
	{
		m_Automatic.SetCheck(m_Alignment == 0);
		m_Bilinear.SetCheck(m_Alignment == 2);
		m_Bisquared.SetCheck(m_Alignment == 3);
		m_Bicubic.SetCheck(m_Alignment == 4);
		m_NoAlignment.SetCheck(m_Alignment == 5);
		UpdateControls();
		m_bFirstActivation = false;

		m_Title.SetTextColor(RGB(0, 0, 0));
		m_Title.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
	};

	return true;
};

/* ------------------------------------------------------------------- */
// CAlignmentParameters message handlers

void CAlignmentParameters::OnBnClickedAutomatic()
{
	if (m_Automatic.GetCheck())
	{
		m_Bilinear.SetCheck(false);
		m_Bisquared.SetCheck(false);
		m_Bicubic.SetCheck(false);
		m_NoAlignment.SetCheck(false);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CAlignmentParameters::OnBnClickedBilinear()
{
	if (m_Bilinear.GetCheck())
	{
		m_Automatic.SetCheck(false);
		m_Bisquared.SetCheck(false);
		m_Bicubic.SetCheck(false);
		m_NoAlignment.SetCheck(false);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CAlignmentParameters::OnBnClickedBisquared()
{
	if (m_Bisquared.GetCheck())
	{
		m_Automatic.SetCheck(false);
		m_Bilinear.SetCheck(false);
		m_Bicubic.SetCheck(false);
		m_NoAlignment.SetCheck(false);
		UpdateControls();
	};
};

/* ------------------------------------------------------------------- */

void CAlignmentParameters::OnBnClickedBicubic()
{
	if (m_Bicubic.GetCheck())
	{
		m_Automatic.SetCheck(false);
		m_Bilinear.SetCheck(false);
		m_Bisquared.SetCheck(false);
		m_NoAlignment.SetCheck(false);
		UpdateControls();
	};
};

/* ------------------------------------------------------------------- */

void CAlignmentParameters::OnBnClickedNoAlignment()
{
	if (m_NoAlignment.GetCheck())
	{
		m_Automatic.SetCheck(false);
		m_Bilinear.SetCheck(false);
		m_Bisquared.SetCheck(false);
		m_Bicubic.SetCheck(false);
		UpdateControls();
	};
};

/* ------------------------------------------------------------------- */
