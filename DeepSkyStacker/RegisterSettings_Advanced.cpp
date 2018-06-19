// RegisterSettings_Advanced.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "RegisterSettings_Advanced.h"
#include "ProgressDlg.h"


/* ------------------------------------------------------------------- */
// CRegisterSettings_Advanced dialog

IMPLEMENT_DYNAMIC(CRegisterSettings_Advanced, CPropertyPage)

CRegisterSettings_Advanced::CRegisterSettings_Advanced()
	: CPropertyPage(CRegisterSettings_Advanced::IDD)
{
	m_psp.dwFlags |= PSP_PREMATURE;
	m_bFirstActivation = TRUE;
}

/* ------------------------------------------------------------------- */

CRegisterSettings_Advanced::~CRegisterSettings_Advanced()
{
}

/* ------------------------------------------------------------------- */

void CRegisterSettings_Advanced::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LUMINANCETHRESHOLDTEXT, m_PercentText);
	DDX_Control(pDX, IDC_LUMINANCETHRESHOLD, m_PercentSlider);
	DDX_Control(pDX, IDC_NRSTARS, m_NrStars);
	DDX_Control(pDX, IDC_MEDIANFILTER, m_MedianFilter);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CRegisterSettings_Advanced, CPropertyPage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_COMPUTEDETECTEDSTARS, OnComputeStars)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

BOOL CRegisterSettings_Advanced::OnSetActive()
{
	if (m_bFirstActivation)
	{
		UpdateSliderText();

		m_NrStars.GetWindowText(m_strMask);

		m_NrStars.SetWindowText(_T(""));
	};

	m_bFirstActivation = FALSE;

	return CPropertyPage::OnSetActive();
};

/* ------------------------------------------------------------------- */
// CRegisterSettings_Advanced message handlers
/* ------------------------------------------------------------------- */

void	CRegisterSettings_Advanced::UpdateSliderText()
{
	LONG				lPos;
	CString				strText;

	lPos = m_PercentSlider.GetPos();
	strText.Format(_T("%ld %%"), lPos);
	m_PercentText.SetWindowText(strText);
};

void CRegisterSettings_Advanced::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateSliderText();

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

/* ------------------------------------------------------------------- */

void CRegisterSettings_Advanced::OnComputeStars() 
{
	// Retrieve the first checked light frame of the list
	CDSSProgressDlg				dlg;
	CLightFrameInfo				fi;
	LONG						lPos;
	CString						strText;

	lPos = m_PercentSlider.GetPos();

	// Register the image with hot pixel detection and not flat/dark...
	TCHAR						szFileName[1+_MAX_FNAME];
	TCHAR						szExt[1+_MAX_EXT];
	CString						strFileName;

	_tsplitpath(m_strFirstLightFrame, NULL, NULL, szFileName, szExt);
	strFileName.Format(_T("%s%s"), szFileName, szExt);
	strText.Format(IDS_REGISTERINGNAME, (LPCTSTR)strFileName);
	dlg.Start(strText, 0);
	dlg.SetJointProgress(TRUE);
	fi.RegisterPicture(m_strFirstLightFrame, (double)lPos/100.0, TRUE, m_MedianFilter.GetCheck(), &dlg);
	dlg.SetJointProgress(FALSE);

	strText.Format(m_strMask, fi.m_vStars.size());
	m_NrStars.SetWindowText(strText);
};

/* ------------------------------------------------------------------- */
