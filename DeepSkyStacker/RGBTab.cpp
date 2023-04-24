// RGBTab.cpp : implementation file
//
#include "stdafx.h"
#include "RGBTab.h"
#include "ProcessingDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CRGBTab property page

IMPLEMENT_DYNCREATE(CRGBTab, CChildPropertyPage)

CRGBTab::CRGBTab() : CChildPropertyPage(CRGBTab::IDD)
{
	//{{AFX_DATA_INIT(CRGBTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_PREMATURE;
	m_bFirstActivation = true;
    m_RedAdjustMethod = HISTOADJUSTTYPE(0);
    m_GreenAdjustMethod = HISTOADJUSTTYPE(0);
    m_BlueAdjustMethod = HISTOADJUSTTYPE(0);
}

CRGBTab::~CRGBTab()
{
}

void CRGBTab::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRGBTab)
	DDX_Control(pDX, IDC_RED_HAT, m_RedHAT);
	DDX_Control(pDX, IDC_GREEN_HAT, m_GreenHAT);
	DDX_Control(pDX, IDC_BLUE_HAT, m_BlueHAT);
	DDX_Control(pDX, IDC_SETTINGS, m_Settings);
	DDX_Control(pDX, IDC_REDO, m_Redo);
	DDX_Control(pDX, IDC_UNDO, m_Undo);
	DDX_Control(pDX, IDC_LINK, m_LinkSettings);
	DDX_Control(pDX, IDC_REDGRADIENT, m_RedGradient);
	DDX_Control(pDX, IDC_GREENGRADIENT, m_GreenGradient);
	DDX_Control(pDX, IDC_BLUEGRADIENT, m_BlueGradient);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRGBTab, CChildPropertyPage)
	//{{AFX_MSG_MAP(CRGBTab)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_PROCESS, OnProcess)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_BN_CLICKED(IDC_REDO, OnRedo)
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_BN_CLICKED(IDC_RED_HAT, OnRedHat)
	ON_BN_CLICKED(IDC_BLUE_HAT, OnBlueHat)
	ON_BN_CLICKED(IDC_GREEN_HAT, OnGreenHat)
	//}}AFX_MSG_MAP
	ON_NOTIFY(GC_SELCHANGE, IDC_REDGRADIENT, OnNotifyRedChangeSelPeg)
	ON_NOTIFY(GC_PEGMOVE, IDC_REDGRADIENT, OnNotifyRedPegMove)
	ON_NOTIFY(GC_PEGMOVED, IDC_REDGRADIENT, OnNotifyRedPegMove)
	ON_NOTIFY(GC_SELCHANGE, IDC_GREENGRADIENT, OnNotifyGreenChangeSelPeg)
	ON_NOTIFY(GC_PEGMOVE, IDC_GREENGRADIENT, OnNotifyGreenPegMove)
	ON_NOTIFY(GC_PEGMOVED, IDC_GREENGRADIENT, OnNotifyGreenPegMove)
	ON_NOTIFY(GC_SELCHANGE, IDC_BLUEGRADIENT, OnNotifyBlueChangeSelPeg)
	ON_NOTIFY(GC_PEGMOVE, IDC_BLUEGRADIENT, OnNotifyBluePegMove)
	ON_NOTIFY(GC_PEGMOVED, IDC_BLUEGRADIENT, OnNotifyBluePegMove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRGBTab message handlers

/* ------------------------------------------------------------------- */

BOOL CRGBTab::OnSetActive()
{
	constexpr auto IconNumber = static_cast<int>(reinterpret_cast<std::uintptr_t>(BTNST_AUTO_GRAY));

	if (m_bFirstActivation)
	{
		m_RedGradient.ShowTooltips(false);
		m_GreenGradient.ShowTooltips(false);
		m_BlueGradient.ShowTooltips(false);

		m_RedHAT.SetFlat(true);
		m_GreenHAT.SetFlat(true);
		m_BlueHAT.SetFlat(true);

		m_RedHAT.DrawTransparent(true);
		m_GreenHAT.DrawTransparent(true);
		m_BlueHAT.DrawTransparent(true);

		m_RedHAT.SetTooltipText(IDS_TT_REDADJUST);
		m_GreenHAT.SetTooltipText(IDS_TT_GREENADJUST);
		m_BlueHAT.SetTooltipText(IDS_TT_BLUEADJUST);

		SetRedAdjustMethod(HAT_LINEAR);
		SetGreenAdjustMethod(HAT_LINEAR);
		SetBlueAdjustMethod(HAT_LINEAR);

		m_Redo.SetIcon(IDI_REDO, IconNumber);
		m_Undo.SetIcon(IDI_UNDO, IconNumber);
		m_Redo.SetFlat(true);
		m_Undo.SetFlat(true);
		m_Redo.DrawTransparent(true);
		m_Undo.DrawTransparent(true);

		m_Redo.SetTooltipText(IDS_REDOLASTSETTINGS);
		m_Undo.SetTooltipText(IDS_UNDOLASTSETTINGS);

		m_Settings.DrawTransparent(true);
		m_Settings.SetFlat(true);
		m_Settings.SetIcon(IDI_SETTINGS, IconNumber);
		m_Settings.SetTooltipText(IDS_MANAGESETTINGS);

		m_bFirstActivation = false;
	}

	return CChildPropertyPage::OnSetActive();
}

/* ------------------------------------------------------------------- */

void CRGBTab::OnReset()
{
	GetParentProcessingDlg(this)->OnReset();
}

/* ------------------------------------------------------------------- */

void CRGBTab::OnNotifyRedChangeSelPeg(NMHDR *, LRESULT *)
{
}

void CRGBTab::OnNotifyRedPegMove(NMHDR * pNotifyStruct, LRESULT *)
{
	if (m_LinkSettings.GetCheck())
	{
		// Update green and blue pegs
		PegNMHDR *			pHDR = (PegNMHDR *)pNotifyStruct;
		int					nID;
		float				Position;

		nID = pHDR->peg.GetID();
		Position = pHDR->peg.position;

		CGradient &			GreenGradient = m_GreenGradient.GetGradient();
		GreenGradient.SetPeg(GreenGradient.IndexFromId(nID), Position);
		m_GreenGradient.Invalidate(true);

		CGradient &			BlueGradient = m_BlueGradient.GetGradient();
		BlueGradient.SetPeg(BlueGradient.IndexFromId(nID), Position);
		m_BlueGradient.Invalidate(true);
	}
	GetParentProcessingDlg(this)->UpdateBezierCurve();
}

void CRGBTab::OnNotifyRedPegMoved(NMHDR * pNotifyStruct, LRESULT *result)
{
	OnNotifyRedPegMove(pNotifyStruct, result);
}

void CRGBTab::OnNotifyGreenChangeSelPeg(NMHDR *, LRESULT *)
{
}

void CRGBTab::OnNotifyGreenPegMove(NMHDR * pNotifyStruct, LRESULT *)
{
	if (m_LinkSettings.GetCheck())
	{
		// Update red and blue pegs
		PegNMHDR *			pHDR = (PegNMHDR *)pNotifyStruct;
		int					nID;
		float				Position;

		nID = pHDR->peg.GetID();
		Position = pHDR->peg.position;

		CGradient &			RedGradient = m_RedGradient.GetGradient();
		RedGradient.SetPeg(RedGradient.IndexFromId(nID), Position);
		m_RedGradient.Invalidate(true);

		CGradient &			BlueGradient = m_BlueGradient.GetGradient();
		BlueGradient.SetPeg(BlueGradient.IndexFromId(nID), Position);
		m_BlueGradient.Invalidate(true);
	}

	GetParentProcessingDlg(this)->UpdateBezierCurve();
}

void CRGBTab::OnNotifyGreenPegMoved(NMHDR * pNotifyStruct, LRESULT *result)
{
	OnNotifyGreenPegMove(pNotifyStruct, result);
}

void CRGBTab::OnNotifyBlueChangeSelPeg(NMHDR *, LRESULT *)
{
}

void CRGBTab::OnNotifyBluePegMove(NMHDR * pNotifyStruct, LRESULT *)
{
	if (m_LinkSettings.GetCheck())
	{
		// Update red and green pegs
		PegNMHDR *			pHDR = (PegNMHDR *)pNotifyStruct;
		int					nID;
		float				Position;

		nID = pHDR->peg.GetID();
		Position = pHDR->peg.position;

		CGradient &			RedGradient = m_RedGradient.GetGradient();
		RedGradient.SetPeg(RedGradient.IndexFromId(nID), Position);
		m_RedGradient.Invalidate(true);

		CGradient &			GreenGradient = m_GreenGradient.GetGradient();
		GreenGradient.SetPeg(GreenGradient.IndexFromId(nID), Position);
		m_GreenGradient.Invalidate(true);
	}

	GetParentProcessingDlg(this)->UpdateBezierCurve();
}

void CRGBTab::OnNotifyBluePegMoved(NMHDR * pNotifyStruct, LRESULT *result)
{
	OnNotifyBluePegMove(pNotifyStruct, result);
}

/* ------------------------------------------------------------------- */

void CRGBTab::OnProcess()
{
	GetParentProcessingDlg(this)->OnProcess();
}

/* ------------------------------------------------------------------- */

void CRGBTab::OnUndo()
{
	GetParentProcessingDlg(this)->OnUndo();
}

/* ------------------------------------------------------------------- */

void CRGBTab::OnRedo()
{
	GetParentProcessingDlg(this)->OnRedo();
}

/* ------------------------------------------------------------------- */

void CRGBTab::OnSettings()
{
	GetParentProcessingDlg(this)->OnSettings();
}

/* ------------------------------------------------------------------- */

static void PopulateHistoAdjustMenu(CMenu & menu)
{
	CString			strText;

	menu.CreatePopupMenu();

	menu.AppendMenu(MF_STRING, HAT_LINEAR, HistoAdjustTypeText(HAT_LINEAR).toStdWString().c_str());
	menu.AppendMenu(MF_STRING, HAT_CUBEROOT, HistoAdjustTypeText(HAT_CUBEROOT).toStdWString().c_str());
	menu.AppendMenu(MF_STRING, HAT_SQUAREROOT, HistoAdjustTypeText(HAT_SQUAREROOT).toStdWString().c_str());
	menu.AppendMenu(MF_STRING, HAT_LOG, HistoAdjustTypeText(HAT_LOG).toStdWString().c_str());
	menu.AppendMenu(MF_STRING, HAT_LOGLOG, HistoAdjustTypeText(HAT_LOGLOG).toStdWString().c_str());
	menu.AppendMenu(MF_STRING, HAT_LOGSQUAREROOT, HistoAdjustTypeText(HAT_LOGSQUAREROOT).toStdWString().c_str());
	menu.AppendMenu(MF_STRING, HAT_ASINH, HistoAdjustTypeText(HAT_ASINH).toStdWString().c_str());
}

void CRGBTab::OnRedHat()
{
	CMenu				menu;
	CRect				rcButton;
	int					nAction;

	m_RedHAT.GetWindowRect(&rcButton);

	PopulateHistoAdjustMenu(menu);

	nAction = TrackPopupMenuEx(menu.GetSafeHmenu(), TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN, rcButton.right, rcButton.top, GetSafeHwnd(), nullptr);
	if (nAction > 0)
	{
		HISTOADJUSTTYPE		hat = (HISTOADJUSTTYPE)nAction;

		SetRedAdjustMethod(hat);
		if (m_LinkSettings.GetCheck())
		{
			SetGreenAdjustMethod(hat);
			SetBlueAdjustMethod(hat);
		}
		GetParentProcessingDlg(this)->UpdateBezierCurve();
	}
}

/* ------------------------------------------------------------------- */

void CRGBTab::OnGreenHat()
{
	CMenu				menu;
	CRect				rcButton;
	int					nAction;

	m_GreenHAT.GetWindowRect(&rcButton);

	PopulateHistoAdjustMenu(menu);

	nAction = TrackPopupMenuEx(menu.GetSafeHmenu(), TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN, rcButton.right, rcButton.top, GetSafeHwnd(), nullptr);
	if (nAction > 0)
	{
		HISTOADJUSTTYPE		hat = (HISTOADJUSTTYPE)nAction;

		SetGreenAdjustMethod(hat);
		if (m_LinkSettings.GetCheck())
		{
			SetRedAdjustMethod(hat);
			SetBlueAdjustMethod(hat);
		}
		GetParentProcessingDlg(this)->UpdateBezierCurve();
	}
}

/* ------------------------------------------------------------------- */

void CRGBTab::OnBlueHat()
{
	CMenu				menu;
	CRect				rcButton;
	int					nAction;

	m_BlueHAT.GetWindowRect(&rcButton);

	PopulateHistoAdjustMenu(menu);

	nAction = TrackPopupMenuEx(menu.GetSafeHmenu(), TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN, rcButton.right, rcButton.top, GetSafeHwnd(), nullptr);
	if (nAction > 0)
	{
		HISTOADJUSTTYPE		hat = (HISTOADJUSTTYPE)nAction;

		SetBlueAdjustMethod(hat);
		if (m_LinkSettings.GetCheck())
		{
			SetRedAdjustMethod(hat);
			SetGreenAdjustMethod(hat);
		}

		GetParentProcessingDlg(this)->UpdateBezierCurve();
	}
}

/* ------------------------------------------------------------------- */
