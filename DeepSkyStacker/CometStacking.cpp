// ResultParameters.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "CometStacking.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"

// CCometStacking dialog

IMPLEMENT_DYNAMIC(CCometStacking, CChildPropertyPage)

/* ------------------------------------------------------------------- */

CCometStacking::CCometStacking()
	: CChildPropertyPage(CCometStacking::IDD)
{
	m_bFirstActivation = true;
	m_CometStackingMode = CSM_STANDARD;
}

/* ------------------------------------------------------------------- */

CCometStacking::~CCometStacking()
{
}

/* ------------------------------------------------------------------- */

void CCometStacking::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STANDARDSTACKING, m_StandardStacking);
	DDX_Control(pDX, IDC_COMETSTACKING, m_CometStacking);
	DDX_Control(pDX, IDC_ADVANCEDSTACKING, m_AdvancedStacking);
	DDX_Control(pDX, IDC_COMETSAMPLE, m_Preview);
	DDX_Control(pDX, IDC_TITLE, m_Title);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CCometStacking, CChildPropertyPage)
	ON_BN_CLICKED(IDC_STANDARDSTACKING, &CCometStacking::OnBnClickedStandardStacking)
	ON_BN_CLICKED(IDC_COMETSTACKING, &CCometStacking::OnBnClickedCometStacking)
	ON_BN_CLICKED(IDC_ADVANCEDSTACKING, &CCometStacking::OnBnClickedAdvancedStacking)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CCometStacking::UpdateControls()
{
	CStackSettings *	pDialog = dynamic_cast<CStackSettings *>(GetParent()->GetParent());

	/*
	if (m_bUseCustom)
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_CUSTOMMODE)));
	else if (!m_bMosaic)
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_NORMALMODE)));
	else
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_MOSAICMODE)));
	*/

	m_StandardStacking.SetCheck(m_CometStackingMode == CSM_STANDARD);
	m_CometStacking.SetCheck(m_CometStackingMode == CSM_COMETONLY);
	m_AdvancedStacking.SetCheck(m_CometStackingMode == CSM_COMETSTAR);

	switch (m_CometStackingMode)
	{
	case CSM_STANDARD:
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_COMETNORMAL)));
		break;
	case CSM_COMETONLY:
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_COMETTRAILS)));
		break;
	case CSM_COMETSTAR:
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_COMETFREEZE)));
		break;
	};

	if (pDialog)
		pDialog->UpdateControls();
};

/* ------------------------------------------------------------------- */

BOOL CCometStacking::OnSetActive()
{
	if (m_bFirstActivation)
	{
		m_Title.SetTextColor(RGB(0, 0, 0));
		m_Title.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

		UpdateControls();
		m_bFirstActivation = false;
	};

	return true;
};

/* ------------------------------------------------------------------- */
// CCometStacking message handlers

void CCometStacking::OnBnClickedStandardStacking()
{
	if (m_StandardStacking.GetCheck())
	{
		m_CometStackingMode = CSM_STANDARD;
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CCometStacking::OnBnClickedCometStacking()
{
	if (m_CometStacking.GetCheck())
	{
		m_CometStackingMode = CSM_COMETONLY;
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CCometStacking::OnBnClickedAdvancedStacking()
{
	if (m_AdvancedStacking.GetCheck())
	{
		m_CometStackingMode = CSM_COMETSTAR;
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */
