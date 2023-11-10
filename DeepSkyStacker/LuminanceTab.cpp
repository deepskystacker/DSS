// LuminanceTab.cpp : implementation file
//
#include "stdafx.h"
#include "LuminanceTab.h"
#include "ProcessingDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CLuminanceTab property page

IMPLEMENT_DYNCREATE(CLuminanceTab, CChildPropertyPage)

CLuminanceTab::CLuminanceTab() : CChildPropertyPage(CLuminanceTab::IDD)
{
	//{{AFX_DATA_INIT(CLuminanceTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_PREMATURE;
	m_bFirstActivation = true;
}

CLuminanceTab::~CLuminanceTab()
{
}

void CLuminanceTab::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLuminanceTab)
	DDX_Control(pDX, IDC_SETTINGS, m_Settings);
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_REDO, m_Redo);
	DDX_Control(pDX, IDC_MIDTONE, m_MidTone);
	DDX_Control(pDX, IDC_MIDANGLE, m_MidAngle);
	DDX_Control(pDX, IDC_HIGHPOWER, m_HighPower);
	DDX_Control(pDX, IDC_HIGHANGLE, m_HighAngle);
	DDX_Control(pDX, IDC_DARKPOWER, m_DarkPower);
	DDX_Control(pDX, IDC_DARKANGLE, m_DarkAngle);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ST_DARKNESS, m_ST_Darkness);
	DDX_Control(pDX, IDC_ST_MIDTONE, m_ST_Midtone);
	DDX_Control(pDX, IDC_ST_HIGHLIGHT, m_ST_Highlight);
	DDX_Control(pDX, IDC_TEXT_DARKNESS, m_DarknessTitle);
	DDX_Control(pDX, IDC_TEXT_MIDTONE, m_MidtoneTitle);
	DDX_Control(pDX, IDC_TEXT_HIGHLIGHT, m_HighlightTitle);
}


BEGIN_MESSAGE_MAP(CLuminanceTab, CChildPropertyPage)
	//{{AFX_MSG_MAP(CLuminanceTab)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_PROCESS, OnProcess)
	ON_BN_CLICKED(IDC_REDO, OnRedo)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CLuminanceTab message handlers

BOOL CLuminanceTab::OnSetActive()
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

		m_bFirstActivation = false;

		m_DarknessTitle.SetTextColor(RGB(0, 0, 0));
		m_DarknessTitle.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

		m_MidtoneTitle.SetTextColor(RGB(0, 0, 0));
		m_MidtoneTitle.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

		m_HighlightTitle.SetTextColor(RGB(0, 0, 0));
		m_HighlightTitle.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
	};

	return CChildPropertyPage::OnSetActive();
}

/* ------------------------------------------------------------------- */

void CLuminanceTab::UpdateTexts()
{
	CString			strText;
	int			lPos1, lPos2;

	lPos1 = m_DarkAngle.GetPos();
	lPos2 = m_DarkPower.GetPos();

	strText.Format(_T(" %ld °\n %.1f"), lPos1, lPos2/10.0);
	m_ST_Darkness.SetText(strText);

	lPos1 = m_MidAngle.GetPos();
	lPos2 = m_MidTone.GetPos();

	strText.Format(_T(" %ld °\n %.1f"), lPos1, lPos2/10.0);
	m_ST_Midtone.SetText(strText);

	lPos1 = m_HighAngle.GetPos();
	lPos2 = m_HighPower.GetPos();

	strText.Format(_T(" %ld °\n %.1f"), lPos1, lPos2/10.0);
	m_ST_Highlight.SetText(strText);
};

/* ------------------------------------------------------------------- */

void CLuminanceTab::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	GetParentProcessingDlg(this)->OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateTexts();
	CChildPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

/* ------------------------------------------------------------------- */

void CLuminanceTab::OnProcess()
{
	GetParentProcessingDlg(this)->OnProcess();
}

/* ------------------------------------------------------------------- */

void CLuminanceTab::OnRedo()
{
	GetParentProcessingDlg(this)->OnRedo();
}

/* ------------------------------------------------------------------- */

void CLuminanceTab::OnUndo()
{
	GetParentProcessingDlg(this)->OnUndo();
}

/* ------------------------------------------------------------------- */

void CLuminanceTab::OnReset()
{
	GetParentProcessingDlg(this)->OnReset();
}

/* ------------------------------------------------------------------- */

void CLuminanceTab::OnSettings()
{
	GetParentProcessingDlg(this)->OnSettings();
}

/* ------------------------------------------------------------------- */

