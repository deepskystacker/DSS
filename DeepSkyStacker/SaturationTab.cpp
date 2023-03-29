// SaturationTab.cpp : implementation file
//
#include "stdafx.h"
#include "SaturationTab.h"
#include "ProcessingDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSaturationTab property page

/* ------------------------------------------------------------------- */

IMPLEMENT_DYNCREATE(CSaturationTab, CChildPropertyPage)

CSaturationTab::CSaturationTab() : CChildPropertyPage(CSaturationTab::IDD)
{
	//{{AFX_DATA_INIT(CSaturationTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_PREMATURE;
	m_bFirstActivation = true;
}

CSaturationTab::~CSaturationTab()
{
}

/* ------------------------------------------------------------------- */

void CSaturationTab::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaturationTab)
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_SETTINGS, m_Settings);
	DDX_Control(pDX, IDC_REDO, m_Redo);
	DDX_Control(pDX, IDC_SATURATION, m_Saturation);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ST_SATURATION, m_ST_Saturation);
	DDX_Control(pDX, IDC_TEXT_SATURATION, m_SaturationTitle);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CSaturationTab, CChildPropertyPage)
	//{{AFX_MSG_MAP(CSaturationTab)
	ON_BN_CLICKED(IDC_PROCESS, OnProcess)
	ON_BN_CLICKED(IDC_REDO, OnRedo)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CSaturationTab message handlers

void CSaturationTab::OnProcess()
{
	GetParentProcessingDlg(this)->OnProcess();
}

/* ------------------------------------------------------------------- */

void CSaturationTab::OnRedo()
{
	GetParentProcessingDlg(this)->OnRedo();
}

/* ------------------------------------------------------------------- */

void CSaturationTab::OnReset()
{
	GetParentProcessingDlg(this)->OnReset();
}

/* ------------------------------------------------------------------- */

void CSaturationTab::OnSettings()
{
	GetParentProcessingDlg(this)->OnSettings();
}

/* ------------------------------------------------------------------- */

void CSaturationTab::OnUndo()
{
	GetParentProcessingDlg(this)->OnUndo();
}

/* ------------------------------------------------------------------- */

BOOL CSaturationTab::OnSetActive()
{
	constexpr auto IconNumber = static_cast<int>(reinterpret_cast<std::uintptr_t>(BTNST_AUTO_GRAY));

	if (m_bFirstActivation)
	{
		m_Redo.DrawTransparent(true);
		m_Undo.DrawTransparent(true);
		m_Redo.SetFlat(true);
		m_Undo.SetFlat(true);
		m_Redo.SetIcon(IDI_REDO, IconNumber);
		m_Undo.SetIcon(IDI_UNDO, IconNumber);

		m_Redo.SetTooltipText(IDS_REDOLASTSETTINGS);
		m_Undo.SetTooltipText(IDS_UNDOLASTSETTINGS);

		m_Settings.DrawTransparent(true);
		m_Settings.SetFlat(true);
		m_Settings.SetIcon(IDI_SETTINGS, IconNumber);
		m_Settings.SetTooltipText(IDS_MANAGESETTINGS);

//		m_ST_Saturation.SetBorder(true);
		m_SaturationTitle.SetTextColor(RGB(0, 0, 0));
		m_SaturationTitle.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

		m_bFirstActivation = false;
	};

	return CChildPropertyPage::OnSetActive();
}

/* ------------------------------------------------------------------- */

void CSaturationTab::UpdateTexts()
{
	CString				strText;

	auto lPos = m_Saturation.GetPos();
	strText.Format(_T("%ld %%"), lPos-50);

	m_ST_Saturation.SetText(strText);
};

/* ------------------------------------------------------------------- */


void CSaturationTab::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
//	GetParentProcessingDlg(this)->OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateTexts();
	CChildPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

/* ------------------------------------------------------------------- */
