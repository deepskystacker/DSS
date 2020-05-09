// RegisterSettings_Actions.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "RegisterSettings_Actions.h"


/* ------------------------------------------------------------------- */
// CRegisterSettings_Actions dialog

IMPLEMENT_DYNAMIC(CRegisterSettings_Actions, CPropertyPage)

CRegisterSettings_Actions::CRegisterSettings_Actions()
	: CPropertyPage(CRegisterSettings_Actions::IDD)
{
	m_psp.dwFlags |= PSP_PREMATURE;
	m_bFirstActivation = true;
}

/* ------------------------------------------------------------------- */

CRegisterSettings_Actions::~CRegisterSettings_Actions()
{
}

/* ------------------------------------------------------------------- */

void CRegisterSettings_Actions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STACK, m_Stack);
	DDX_Control(pDX, IDC_PERCENT, m_Percent);
	DDX_Control(pDX, IDC_HOTPIXELS, m_HotPixels);
	DDX_Control(pDX, IDC_FORCEREGISTER, m_ForceRegister);
	DDX_Control(pDX, IDC_STACKWARNING, m_StackWarning);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CRegisterSettings_Actions, CPropertyPage)
	ON_BN_CLICKED(IDC_STACK, OnStack)
	ON_BN_CLICKED(IDC_HOTPIXELS, OnHotPixels)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

BOOL CRegisterSettings_Actions::OnSetActive()
{
	if (m_bFirstActivation)
	{
		m_bFirstActivation = false;
	};

	return CPropertyPage::OnSetActive();
};

/* ------------------------------------------------------------------- */

void CRegisterSettings_Actions::OnStack()
{
	m_Percent.EnableWindow(m_Stack.GetCheck());
}

/* ------------------------------------------------------------------- */

void CRegisterSettings_Actions::OnHotPixels()
{
};

/* ------------------------------------------------------------------- */
// CRegisterSettings_Actions message handlers
