// ExplorerBar.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "DeepStackerDlg.h"
#include "RawDDPSettings.h"
#include "ExplorerBar.h"
#include "About.h"
#include <gdiplus.h>
#include "RegisterSettings.h"
#include "StackSettings.h"
#include "RecommandedSettings.h"
using namespace Gdiplus;

/* ------------------------------------------------------------------- */
// CExplorerBar dialog

IMPLEMENT_DYNAMIC(CExplorerBar, CScrollDialog)

CExplorerBar::CExplorerBar(CWnd* pParent /*=NULL*/)
	: CScrollDialog(CExplorerBar::IDD, pParent)
{
	m_MRUSettings.SetBasePath(REGENTRY_BASEKEY_SETTINGFILES);
	m_bHorz = FALSE;
}

/* ------------------------------------------------------------------- */

CExplorerBar::~CExplorerBar()
{
}

/* ------------------------------------------------------------------- */

void CExplorerBar::DoDataExchange(CDataExchange* pDX)
{
	CScrollDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESSING_RECT, m_ProcessingRect);
	DDX_Control(pDX, IDC_STACKING_RECT, m_StackingRect);
	DDX_Control(pDX, IDC_OPTIONS_RECT, m_OptionsRect);

	DDX_Control(pDX, IDC_STACKING_SUBRECT1, m_SubRect1);
	DDX_Control(pDX, IDC_STACKING_SUBRECT2, m_SubRect2);
	DDX_Control(pDX, IDC_STACKING_SUBRECT3, m_SubRect3);

	DDX_Control(pDX, IDC_REGISTERING_REGISTERCHECKED, m_Register_RegisterChecked);

	DDX_Control(pDX, IDC_STACKING_TITLE, m_Stacking_Title);
	DDX_Control(pDX, IDC_STACKING_OPENFILES, m_Stacking_OpenFiles);
	DDX_Control(pDX, IDC_STACKING_OPENDARKS, m_Stacking_OpenDarks);
	DDX_Control(pDX, IDC_STACKING_OPENDARKFLATS, m_Stacking_OpenDarkFlats);
	DDX_Control(pDX, IDC_STACKING_OPENFLATS, m_Stacking_OpenFlats);
	DDX_Control(pDX, IDC_STACKING_OPENOFFSETS, m_Stacking_OpenOffsets);
	DDX_Control(pDX, IDC_STACKING_CLEARLIST, m_Stacking_ClearList);
	DDX_Control(pDX, IDC_STACKING_CHECKALL, m_Stacking_CheckAll);
	DDX_Control(pDX, IDC_STACKING_CHECKABOVE, m_Stacking_CheckAbove);
	DDX_Control(pDX, IDC_STACKING_UNCHECKALL, m_Stacking_UncheckAll);
	DDX_Control(pDX, IDC_STACKING_COMPUTEOFFSETS, m_Stacking_ComputeOffsets);
	DDX_Control(pDX, IDC_STACKING_STACKCHECKED, m_Stacking_StackChecked);
	DDX_Control(pDX, IDC_STACKING_LOADLIST, m_Stacking_LoadList);
	DDX_Control(pDX, IDC_STACKING_SAVELIST, m_Stacking_SaveList);
	DDX_Control(pDX, IDC_STACKING_BATCHSTACK, m_Stacking_BatchStack);

	DDX_Control(pDX, IDC_PROCESSING_TITLE, m_Processing_Title);
	DDX_Control(pDX, IDC_PROCESSING_OPENFILE, m_Processing_OpenFile);
	DDX_Control(pDX, IDC_PROCESSING_COPYTOCLIPBOARD, m_Processing_CopyToClipboard);
	DDX_Control(pDX, IDC_PROCESSING_CREATESTARMASK, m_Processing_CreateStarMask);
	DDX_Control(pDX, IDC_PROCESSING_SAVEFILE, m_Processing_SaveFile);

	DDX_Control(pDX, IDC_OPTIONS_TITLE, m_Options_Title);
	DDX_Control(pDX, IDC_OPTIONS_RAWDDPSETTINGS, m_Options_RawDDPSettings);
	DDX_Control(pDX, IDC_OPTIONS_LOADSETTINGS, m_Options_LoadSettings);
	DDX_Control(pDX, IDC_OPTIONS_SAVESETTINGS, m_Options_SaveSettings);
	DDX_Control(pDX, IDC_OPTIONS_SETTINGS, m_Options_Settings);
	DDX_Control(pDX, IDC_OPTIONS_RECOMMANDEDSETTINGS, m_Options_RecommandedSettings);

	DDX_Control(pDX, IDC_ABOUT, m_About);
	DDX_Control(pDX, IDC_CONTEXTHELP, m_Help);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CExplorerBar, CScrollDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_TITLE, OnStackingTitle)
	ON_NOTIFY(NM_LINKCLICK, IDC_PROCESSING_TITLE, OnProcessingTitle)

	ON_NOTIFY(NM_LINKCLICK, IDC_REGISTERING_REGISTERCHECKED, OnRegisteringRegisterChecked)

	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_OPENFILES, OnStackingOpenFiles)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_OPENDARKS, OnStackingOpenDarks)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_OPENDARKFLATS, OnStackingOpenDarkFlats)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_OPENFLATS, OnStackingOpenFlats)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_OPENOFFSETS, OnStackingOpenOffsets)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_CLEARLIST, OnStackingClearList)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_CHECKALL, OnStackingCheckAll)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_CHECKABOVE, OnStackingCheckAbove)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_UNCHECKALL, OnStackingUncheckAll)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_COMPUTEOFFSETS, OnStackingComputeOffsets)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_STACKCHECKED, OnStackingStackChecked)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_BATCHSTACK, OnStackingBatchStack)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_LOADLIST, OnStackingLoadList)
	ON_NOTIFY(NM_LINKCLICK, IDC_STACKING_SAVELIST, OnStackingSaveList)

	ON_NOTIFY(NM_LINKCLICK, IDC_PROCESSING_OPENFILE, OnProcessingOpenFile)
	ON_NOTIFY(NM_LINKCLICK, IDC_PROCESSING_COPYTOCLIPBOARD, OnProcessingCopyToClipboard)
	ON_NOTIFY(NM_LINKCLICK, IDC_PROCESSING_CREATESTARMASK, OnProcessingCreateStarMask)
	ON_NOTIFY(NM_LINKCLICK, IDC_PROCESSING_SAVEFILE, OnProcessingSaveFile)

	ON_NOTIFY(NM_LINKCLICK, IDC_OPTIONS_RAWDDPSETTINGS, OnOptionsRawDDPSettings)
	ON_NOTIFY(NM_LINKCLICK, IDC_OPTIONS_SETTINGS, OnOptionsSettings)
	ON_NOTIFY(NM_LINKCLICK, IDC_OPTIONS_LOADSETTINGS, OnOptionsLoadSettings)
	ON_NOTIFY(NM_LINKCLICK, IDC_OPTIONS_SAVESETTINGS, OnOptionsSaveSettings)
	ON_NOTIFY(NM_LINKCLICK, IDC_OPTIONS_RECOMMANDEDSETTINGS, OnOptionsRecommandedSettings)
	ON_NOTIFY(NM_LINKCLICK, IDC_ABOUT, OnAbout)
	ON_NOTIFY(NM_LINKCLICK, IDC_CONTEXTHELP, OnHelp)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
// CExplorerBar message handlers

void CExplorerBar::DrawGradientRect(CDC * pDC, const CRect & rc, COLORREF crColor1, COLORREF crColor2, double fAlpha)
{
	// Use GDI+
	#ifndef NOGDIPLUS
	Graphics 				graphics(pDC->GetSafeHdc());
	LinearGradientBrush		brush(PointF(rc.left, rc.top), 
					PointF(rc.right, rc.bottom), 
					Color(fAlpha*255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 
					Color(fAlpha*255.0, GetRValue(crColor2), GetGValue(crColor2), GetBValue(crColor2)));
	
	graphics.FillRectangle(&brush, RectF(rc.left, rc.top, rc.Width(), rc.Height()));
	#endif // NOGDIPLUS
};

void CExplorerBar::DrawGradientBackgroundRect(CDC * pDC, const CRect & rc)
{
//	DrawGradientRect(pDC, rc, RGB(224, 244, 252), RGB(138, 185, 242));

	#ifndef NOGDIPLUS
	Graphics 				graphics(pDC->GetSafeHdc());


	COLORREF		crColor1 = RGB(90, 90, 90);
	COLORREF		crColor2 = RGB(90, 90, 90);
			
	LinearGradientBrush		brush(PointF(rc.right, rc.top), 
					PointF(rc.left, rc.bottom), 
					Color(GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 
					Color(GetRValue(crColor2), GetGValue(crColor2), GetBValue(crColor2)));
	
	graphics.FillRectangle(&brush, RectF(rc.left, rc.top, rc.Width(), rc.Height()));

	#endif // NOGDIPLUS
};

void CExplorerBar::DrawGradientFrameRect(CDC * pDC, const CRect & rc, BOOL bActive, BOOL bShadow)
{
	CRect			rcTop;
	CRect			rcBottom;
	COLORREF		crColor1 = RGB(167, 211, 241);
	COLORREF		crColor2 = RGB(167, 211, 241);
	double			fAlpha = bShadow ? 0.3 : 1.0;

	if (bShadow)
	{
		crColor1 = RGB(127, 127, 127);
		crColor2 = RGB(127, 127, 127);
	}
	else if (!bActive)
	{
		crColor1 = RGB(225, 225, 225);
		crColor2 = RGB(255, 255, 255);
	};

	rcTop = rc;
	rcTop.bottom = rcTop.top + 20;
	rcBottom = rc;
	rcBottom.top = rcTop.bottom;
	if (bShadow)
	{
		rcTop.OffsetRect(5, 5);
		rcBottom.OffsetRect(5, 5);
	};
	DrawGradientRect(pDC, rcBottom, crColor2, crColor1, fAlpha);

	#ifndef NOGDIPLUS
	Graphics 				graphics(pDC->GetSafeHdc());
	LinearGradientBrush		brush(PointF(rc.left, rc.top), 
					PointF(rc.right, rc.bottom), 
					Color(fAlpha*255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 
					Color(fAlpha*255.0, GetRValue(crColor2), GetGValue(crColor2), GetBValue(crColor2)));

	GraphicsPath 	path(FillModeWinding);
	
	CRect			rc1(rcTop.left, rcTop.top + 5, rcTop.right, rcTop.bottom), 
					rc2(rcTop.left+5, rcTop.top, rcTop.right-5, rcTop.bottom),
					rc3(rcTop.left, rcTop.top, rcTop.left+10, rcTop.top+10), 
					rc4(rcTop.right-10, rcTop.top, rcTop.right, rcTop.top+10);
	
	path.AddRectangle(RectF(rc1.left, rc1.top, rc1.Width(), rc1.Height()));
	path.AddRectangle(RectF(rc2.left, rc2.top, rc2.Width(), rc2.Height()));
	path.AddEllipse(RectF(rc3.left, rc3.top, rc3.Width(), rc3.Height()));
	path.AddEllipse(RectF(rc4.left, rc4.top, rc4.Width(), rc4.Height()));
	path.Flatten();

	graphics.FillPath(&brush, &path);
	path.AddRectangle(RectF(rcBottom.left, rcBottom.top, rcBottom.Width(), rcBottom.Height()));
	path.Flatten();

	Pen 			pen(Color(fAlpha*255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 1.0);

	path.Outline();
	graphics.DrawPath(&pen, &path);

	#endif // NOGDIPLUS
};

void CExplorerBar::DrawSubFrameRect(CDC * pDC, const CRect & rc)
{
	COLORREF		crColor1 = RGB(255, 255, 255);
	COLORREF		crColor2 = RGB(200, 200, 200);
	double			fAlpha = 0.8;

	#ifndef NOGDIPLUS
	Graphics 		graphics(pDC->GetSafeHdc());

	GraphicsPath 	path(FillModeWinding);
	
	CRect			rc1(rc.left+3, rc.top, rc.right-3, rc.bottom), 
					rc2(rc.left, rc.top+3, rc.right, rc.bottom-3);
	
	path.AddRectangle(RectF(rc1.left, rc1.top, rc1.Width(), rc1.Height()));
	path.AddRectangle(RectF(rc2.left, rc2.top, rc2.Width(), rc2.Height()));
	path.AddEllipse(RectF(rc.left, rc.top, 6, 6));
	path.AddEllipse(RectF(rc.right-6, rc.top, 6, 6));
	path.AddEllipse(RectF(rc.left, rc.bottom-6, 6, 6));
	path.AddEllipse(RectF(rc.right-6, rc.bottom-6, 6, 6));
	path.Flatten();

	Pen 			pen(Color(fAlpha*255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 1.0);
	path.Outline();
	graphics.DrawPath(&pen, &path);
	#endif // NOGDIPLUS
};
BOOL CExplorerBar::OnEraseBkgnd(CDC* pDC)
{
	BOOL				bResult = TRUE;//CDialog::OnEraseBkgnd(pDC);
	BOOL				bRedraw = TRUE;
	DWORD				dwTabID = 0;
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	CRect				rc;

	GetClientRect(&rc);
	if (pDlg)
		dwTabID = pDlg->GetCurrentTab();

	if (m_Background.m_hObject)
	{
		bRedraw = (dwTabID != m_dwCurrentTabID) ||
			      (rc != m_rcBackground) ||
				  (GetScrollPos(SB_VERT) != m_nScrollPos);
	};
	

	if (bRedraw)
	{
		CDC					MemDC;
		CBitmap *			pOldBitmap;

		if (m_Background.m_hObject)
			m_Background.DeleteObject();

		MemDC.CreateCompatibleDC(pDC);
		m_Background.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
		pOldBitmap = MemDC.SelectObject(&m_Background);

		DrawGradientBackgroundRect(&MemDC, rc);

		m_StackingRect.GetWindowRect(&rc);
		ScreenToClient(&rc);
		DrawGradientFrameRect(&MemDC, rc, (dwTabID == IDD_STACKING) || (dwTabID == IDD_REGISTERING), TRUE);
		DrawGradientFrameRect(&MemDC, rc, (dwTabID == IDD_STACKING) || (dwTabID == IDD_REGISTERING), FALSE);

		m_rcRegisterStack = rc;

		if ((dwTabID == IDD_STACKING) || (dwTabID == IDD_REGISTERING))
		{
			// Draw sub rects
			m_SubRect1.GetWindowRect(&rc);
			ScreenToClient(&rc);
			DrawSubFrameRect(&MemDC, rc);

			m_SubRect2.GetWindowRect(&rc);
			ScreenToClient(&rc);
			DrawSubFrameRect(&MemDC, rc);

			m_SubRect3.GetWindowRect(&rc);
			ScreenToClient(&rc);
			DrawSubFrameRect(&MemDC, rc);
		};

		m_ProcessingRect.GetWindowRect(&rc);
		ScreenToClient(&rc);
		DrawGradientFrameRect(&MemDC, rc, (dwTabID == IDD_PROCESSING), TRUE);
		DrawGradientFrameRect(&MemDC, rc, (dwTabID == IDD_PROCESSING), FALSE);

		m_rcProcessing = rc;

		m_OptionsRect.GetWindowRect(&rc);
		ScreenToClient(&rc);
		DrawGradientFrameRect(&MemDC, rc, TRUE, TRUE);
		DrawGradientFrameRect(&MemDC, rc, TRUE, FALSE);

		for (LONG i = 0;i<m_vLabels.size();i++)
		{
			m_vLabels[i]->GetWindowRect(&rc);
			ScreenToClient(&rc);
			MemDC.SetViewportOrg(rc.left, rc.top);
			m_vLabels[i]->Draw(&MemDC);
		};
		MemDC.SetViewportOrg(0, 0);

		GetClientRect(&rc);
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &MemDC, 0, 0, SRCCOPY);
		MemDC.SelectObject(pOldBitmap);
		MemDC.DeleteDC();

		m_dwCurrentTabID = dwTabID;
		m_rcBackground   = rc;
		m_nScrollPos	 = GetScrollPos(SB_VERT);

	}
	else
	{
		CDC					MemDC;
		CBitmap *			pOldBitmap;

		MemDC.CreateCompatibleDC(pDC);
		pOldBitmap = MemDC.SelectObject(&m_Background);
		GetClientRect(&rc);
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &MemDC, 0, 0, SRCCOPY);
		MemDC.SelectObject(pOldBitmap);
		MemDC.DeleteDC();
	};

	return bResult;
}

/* ------------------------------------------------------------------- */

void CExplorerBar::OnSize(UINT nType, int cx, int cy)
{
	CScrollDialog::OnSize(nType, cx, cy);

	// Move the controls
	CRect			rcDlg;
	CRect			rcControl;

	GetClientRect(&rcDlg);

	if (m_StackingRect.m_hWnd)
	{
		m_StackingRect.GetWindowRect(&rcControl);
		ScreenToClient(&rcControl);
		rcControl.left = rcDlg.left + 10;
		rcControl.right = rcDlg.right - 10;
		m_StackingRect.MoveWindow(&rcControl);

		m_SubRect1.GetWindowRect(&rcControl);
		ScreenToClient(&rcControl);
		rcControl.left = rcDlg.left + 13;
		rcControl.right = rcDlg.right - 13;
		m_SubRect1.MoveWindow(&rcControl);

		m_SubRect2.GetWindowRect(&rcControl);
		ScreenToClient(&rcControl);
		rcControl.left = rcDlg.left + 13;
		rcControl.right = rcDlg.right - 13;
		m_SubRect2.MoveWindow(&rcControl);

		m_SubRect3.GetWindowRect(&rcControl);
		ScreenToClient(&rcControl);
		rcControl.left = rcDlg.left + 13;
		rcControl.right = rcDlg.right - 13;
		m_SubRect3.MoveWindow(&rcControl);

		m_ProcessingRect.GetWindowRect(&rcControl);
		ScreenToClient(&rcControl);
		rcControl.left = rcDlg.left + 10;
		rcControl.right = rcDlg.right - 10;
		m_ProcessingRect.MoveWindow(&rcControl);

		m_OptionsRect.GetWindowRect(&rcControl);
		ScreenToClient(&rcControl);
		rcControl.left = rcDlg.left + 10;
		rcControl.right = rcDlg.right - 10;
		m_OptionsRect.MoveWindow(&rcControl);
	};

	InvalidateRect(NULL);
}

/* ------------------------------------------------------------------- */

void CExplorerBar::OnLButtonDown( UINT nFlags, CPoint pt)
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);
	DWORD				dwTabID = 0;

	if (pDlg)
		dwTabID = pDlg->GetCurrentTab();

	if (m_rcRegisterStack.PtInRect(pt) && (dwTabID != IDD_REGISTERING) && (dwTabID != IDD_STACKING))
	{
		// Change tab to stacking
		pDlg->ChangeTab(IDD_STACKING);
	}
	else if (m_rcProcessing.PtInRect(pt) && (dwTabID != IDD_PROCESSING))
	{
		// Change tab to processing
		pDlg->ChangeTab(IDD_PROCESSING);
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollDialog::OnVScroll(nSBCode, nPos, pScrollBar);
	InvalidateRect(NULL);
};

/* ------------------------------------------------------------------- */

void CExplorerBar::InitLabel(CLabel & label, BOOL bMain)
{
	m_vLabels.push_back(&label);
	label.SetLink(TRUE, TRUE);
	label.SetTransparent(TRUE);
	label.SetLinkCursor(LoadCursor(NULL, IDC_HAND));
	label.SetFont3D(FALSE);
	label.SetTextColor(RGB(42, 42, 42));
//	label.SetText3DHiliteColor(RGB(0, 0, 0));

	if (bMain)
	{
		//label.SetFontBold(TRUE);
		label.SetTextColor(RGB(109*1.5, 23*1.5, 7*1.5));
	};
};

/* ------------------------------------------------------------------- */

BOOL CExplorerBar::OnInitDialog()
{
	CScrollDialog::OnInitDialog();

	InitLabel(m_Register_RegisterChecked, TRUE);

	InitLabel(m_Stacking_OpenFiles, TRUE);

	InitLabel(m_Stacking_OpenDarks);
	InitLabel(m_Stacking_OpenDarkFlats);
	InitLabel(m_Stacking_OpenFlats);
	InitLabel(m_Stacking_OpenOffsets);
	InitLabel(m_Stacking_ClearList);
	InitLabel(m_Stacking_CheckAll);
	InitLabel(m_Stacking_CheckAbove);
	InitLabel(m_Stacking_UncheckAll);
	InitLabel(m_Stacking_ComputeOffsets);
	InitLabel(m_Stacking_StackChecked, TRUE);

	InitLabel(m_Stacking_BatchStack);
	InitLabel(m_Stacking_LoadList);
	InitLabel(m_Stacking_SaveList);

	InitLabel(m_Processing_OpenFile);
	InitLabel(m_Processing_CopyToClipboard);
	InitLabel(m_Processing_CreateStarMask);
	InitLabel(m_Processing_SaveFile);

	InitLabel(m_Options_RawDDPSettings);
	InitLabel(m_Options_Settings);
	InitLabel(m_Options_LoadSettings);
	InitLabel(m_Options_SaveSettings);
	InitLabel(m_Options_RecommandedSettings, TRUE);
	InitLabel(m_About);
	InitLabel(m_Help);

	InitLabel(m_Stacking_Title);
	m_Stacking_Title.SetFontBold(TRUE);

	InitLabel(m_Processing_Title);
	m_Processing_Title.SetFontBold(TRUE);

	InitLabel(m_Options_Title);
	m_Options_Title.SetFontBold(TRUE);
	m_Options_Title.SetLink(FALSE, FALSE);

	m_MRUSettings.InitFromRegistry();

	return TRUE;  
}

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingTitle( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
		pDlg->ChangeTab(IDD_STACKING);

	InvalidateRect(NULL);
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnProcessingTitle( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
		pDlg->ChangeTab(IDD_PROCESSING);

	InvalidateRect(NULL);
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnRegisteringRegisterChecked( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().RegisterCheckedImage();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingOpenFiles( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().OnAddpictures();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingOpenDarks( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().OnAdddarks();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingOpenDarkFlats( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().OnAddDarkFlats();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingOpenFlats( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().OnAddFlats();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingOpenOffsets( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().OnAddOffsets();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingClearList( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().ClearList();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingLoadList( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().LoadList();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingSaveList( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().SaveList();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingCheckAll( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().CheckAll();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingCheckAbove( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().CheckAbove();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingUncheckAll( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().UncheckAll();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingComputeOffsets( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().ComputeOffsets();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingStackChecked( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().StackCheckedImage();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnStackingBatchStack( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_STACKING);
		InvalidateRect(NULL);
		pDlg->GetStackingDlg().BatchStack();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnProcessingOpenFile( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->GetProcessingDlg().OnLoaddsi();
		pDlg->ChangeTab(IDD_PROCESSING);
		InvalidateRect(NULL);
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnProcessingCopyToClipboard( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->ChangeTab(IDD_PROCESSING);
		InvalidateRect(NULL);
		pDlg->GetProcessingDlg().CopyPictureToClipboard();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnProcessingCreateStarMask( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->GetProcessingDlg().CreateStarMask();
		pDlg->ChangeTab(IDD_PROCESSING);
		InvalidateRect(NULL);
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnProcessingSaveFile( NMHDR * pNotifyStruct, LRESULT * result )
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

	if (pDlg)
	{
		pDlg->GetProcessingDlg().SavePictureToFile();
		pDlg->ChangeTab(IDD_PROCESSING);
		InvalidateRect(NULL);
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnOptionsRawDDPSettings( NMHDR * pNotifyStruct, LRESULT * result )
{
	CRawDDPSettings		dlg;

	dlg.DoModal();
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnOptionsSettings( NMHDR * pNotifyStruct, LRESULT * result )
{
	CPoint				pt;
	CMenu				menu;
	CMenu *				popup;
	int					nResult;

	menu.LoadMenu(IDR_EDITSETTINGS);
	popup = menu.GetSubMenu(0);

	CRect				rc;

	m_Options_Settings.GetWindowRect(&rc);
	pt.x = rc.left;
	pt.y = rc.bottom;

	nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, NULL);;

	if (nResult == ID_EDITSETTINGS_REGISTERSETTINGS)
	{
		CRegisterSettings		dlg;

		dlg.SetSettingsOnly(TRUE);
		dlg.DoModal();
	}
	else if (nResult == ID_EDITSETTINGS_STACKINGSETTINGS)
	{
		CStackSettings			dlg;

		dlg.SetEnableAll(TRUE);
		dlg.DoModal();
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::LoadSettingFile()
{
	CString						strBaseDirectory;
	CString						strTitle;
	DWORD						dwFilterIndex = 0;
	CString						strBaseExtension;

	TCHAR						szPath[1+_MAX_PATH];

	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath);

	strBaseDirectory = szPath;
	strBaseDirectory += "\\DeepSkyStacker";

	// Check that the path exists, else create it
	CreateDirectory(strBaseDirectory, NULL);

	strBaseExtension = _T(".dsssettings");
	strTitle.LoadString(IDS_TITLE_LOADSETTINGS);

	CFileDialog			dlgOpen(TRUE, 
								strBaseExtension,
								NULL,
								OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
								SETTINGFILE_FILTERS,
								this);

	dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;
	dlgOpen.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

	TCHAR				szBigBuffer[20000] = _T("");

	dlgOpen.m_ofn.lpstrFile = szBigBuffer;
	dlgOpen.m_ofn.nMaxFile  = sizeof(szBigBuffer) / sizeof(szBigBuffer[0]);

	if (dlgOpen.DoModal() == IDOK)
	{
		POSITION		pos;
		CString			strDrive;
		CString			strDir;

		pos = dlgOpen.GetStartPosition();
		while (pos)
		{
			CString		strFile;
			CWorkspace	workspace;

			strFile = dlgOpen.GetNextPathName(pos);

			workspace.ReadFromFile(strFile);
			workspace.SaveToRegistry();

			m_MRUSettings.Add(strFile);
			m_MRUSettings.SaveToRegistry();
		};
	};	
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnOptionsLoadSettings( NMHDR * pNotifyStruct, LRESULT * result )
{
	BOOL			bOpenAnother = TRUE;

	{
		CPoint				pt;
		CMenu				menu;
		CMenu *				popup;
		int					nResult;
		UINT				lStartID;
		CWorkspace			workspace;

		bOpenAnother = FALSE;

		menu.LoadMenu(IDR_LOADSETTINGS);
		popup = menu.GetSubMenu(0);

		CRect				rc;

		m_Options_LoadSettings.GetWindowRect(&rc);
		pt.x = rc.left;
		pt.y = rc.bottom;

		lStartID = ID_LOADSETTINGS_FIRSTMRU+1;
		for (LONG i = 0;i<m_MRUSettings.m_vLists.size();i++)
		{
			TCHAR				szDrive[1+_MAX_DRIVE];
			TCHAR				szDir[1+_MAX_DIR];
			TCHAR				szName[1+_MAX_FNAME];
			CString				strItem;

			_tsplitpath((LPCTSTR)m_MRUSettings.m_vLists[i], szDrive, szDir, szName, NULL);
			strItem = szName;

			popup->InsertMenu(ID_LOADSETTINGS_FIRSTMRU, MF_BYCOMMAND, lStartID, (LPCTSTR)strItem);
			lStartID++;
		};

		popup->DeleteMenu(ID_LOADSETTINGS_FIRSTMRU, MF_BYCOMMAND);
		if (!m_MRUSettings.m_vLists.size())
		{
			// Remove the separator in first position
			popup->DeleteMenu(0, MF_BYPOSITION);
		};

		nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, NULL);;

		if (nResult == ID_LOADSETTINGS_LOAD)
			bOpenAnother = TRUE;
		else if (nResult == ID_LOADSETTINGS_LOADDEEPSKYSTACKERLIVESETTINGS)
		{
			// Load the DSSLive setting file
			TCHAR				szPath[1+_MAX_PATH];
			CString				strPath;

			SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath);

			strPath = szPath;
			strPath += "\\DeepSkyStacker\\DSSLive.settings";
			workspace.ReadFromFile(strPath);
			workspace.SaveToRegistry();
		}
		else if (nResult == ID_LOADSETTINGS_RESTOREDEFAULTSETTINGS)
		{
			workspace.ResetToDefault();
			workspace.SaveToRegistry();
		}
		else if (nResult > ID_LOADSETTINGS_FIRSTMRU)
		{
			CString			strFile;

			strFile = m_MRUSettings.m_vLists[nResult-ID_LOADSETTINGS_FIRSTMRU-1];

			workspace.ReadFromFile(strFile);
			workspace.SaveToRegistry();
			m_MRUSettings.Add(strFile);
			m_MRUSettings.SaveToRegistry();
		};
	};

	if (bOpenAnother)
		LoadSettingFile();
};

/* ------------------------------------------------------------------- */

void CExplorerBar::SaveSettingFile()
{
	CString						strBaseDirectory;
	CString						strBaseExtension;
	CString						strTitle;
	DWORD						dwFilterIndex = 0;

	TCHAR						szPath[1+_MAX_PATH];

	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath);

	strBaseDirectory = szPath;
	strBaseDirectory += "\\DeepSkyStacker";

	// Check that the path exists, else create it
	CreateDirectory(strBaseDirectory, NULL);

	strBaseExtension = _T(".dsssettings");
	strTitle.LoadString(IDS_TITLE_SAVESETTINGS);

	CFileDialog					dlgSave(FALSE, 
								strBaseExtension,
								NULL,
								OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_DONTADDTORECENT,
								SETTINGFILE_FILTERS,
								this);

	dlgSave.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgSave.m_ofn.nFilterIndex = dwFilterIndex;
	dlgSave.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

	if (dlgSave.DoModal() == IDOK)
	{
		POSITION		pos;

		pos = dlgSave.GetStartPosition();
		if (pos)
		{
			CString		strFile;
			CWorkspace	workspace;

			strFile = dlgSave.GetNextPathName(pos);

			workspace.SaveToFile(strFile);
			m_MRUSettings.Add(strFile);
		};
	};
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnOptionsSaveSettings( NMHDR * pNotifyStruct, LRESULT * result )
{
	BOOL			bSaveAnother = TRUE;

	{
		CPoint				pt;
		CMenu				menu;
		CMenu *				popup;
		int					nResult;
		UINT				lStartID;
		CWorkspace			workspace;

		bSaveAnother = FALSE;

		menu.LoadMenu(IDR_SAVESETTINGS);
		popup = menu.GetSubMenu(0);

		CRect				rc;

		m_Options_SaveSettings.GetWindowRect(&rc);
		pt.x = rc.left;
		pt.y = rc.bottom;

		lStartID = ID_SAVESETTINGS_FIRSTMRU+1;
		for (LONG i = 0;i<m_MRUSettings.m_vLists.size();i++)
		{
			TCHAR				szDrive[1+_MAX_DRIVE];
			TCHAR				szDir[1+_MAX_DIR];
			TCHAR				szName[1+_MAX_FNAME];
			CString				strItem;

			_tsplitpath((LPCTSTR)m_MRUSettings.m_vLists[i], szDrive, szDir, szName, NULL);
			strItem = szName;

			popup->InsertMenu(ID_SAVESETTINGS_FIRSTMRU, MF_BYCOMMAND, lStartID, (LPCTSTR)strItem);
			lStartID++;
		};

		popup->DeleteMenu(ID_SAVESETTINGS_FIRSTMRU, MF_BYCOMMAND);
		if (!m_MRUSettings.m_vLists.size())
		{
			// Remove the separator in first position
			popup->DeleteMenu(0, MF_BYPOSITION);
		};

		nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, NULL);;

		if (nResult == ID_SAVESETTINGS_SAVEAS)
			bSaveAnother = TRUE;
		else if (nResult == ID_SAVESETTINGS_SAVEASDEEPSKYSTACKERLIVESETTINGS)
		{
			// Sace the DSSLive setting file
			TCHAR				szPath[1+_MAX_PATH];
			CString				strPath;

			SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath);

			strPath = szPath;
			strPath += "\\DeepSkyStacker\\DSSLive.settings";
			workspace.SaveToFile(strPath);
		}
		else if (nResult > ID_SAVESETTINGS_FIRSTMRU)
		{
			CString			strFile;

			strFile = m_MRUSettings.m_vLists[nResult-ID_SAVESETTINGS_FIRSTMRU-1];

			workspace.SaveToFile(strFile);
			m_MRUSettings.Add(strFile);
		};
	};

	if (bSaveAnother)
		SaveSettingFile();
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnOptionsRecommandedSettings( NMHDR * pNotifyStruct, LRESULT * result )
{
	CRecommendedSettings		dlg;

	dlg.DoModal();
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnAbout( NMHDR * pNotifyStruct, LRESULT * result )
{
	CAbout				dlg;

	dlg.DoModal();
};

/* ------------------------------------------------------------------- */

void CExplorerBar::CallHelp()
{
	CString			strBase;
	CString			strFile;
	TCHAR			szFileName[1+_MAX_PATH];
	TCHAR			szDrive[1+_MAX_DRIVE];
	TCHAR			szDir[1+_MAX_DIR];
	CString			strHelpFile;

	strHelpFile.LoadString(IDS_HELPFILE);

	GetModuleFileName(NULL, szFileName, sizeof(szFileName)/sizeof(TCHAR));
	strBase = szFileName;
	_tsplitpath(strBase, szDrive, szDir, NULL, NULL);

	strFile = szDrive;
	strFile += szDir;
	strFile += strHelpFile;

	::HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
};

/* ------------------------------------------------------------------- */

void CExplorerBar::OnHelp( NMHDR * pNotifyStruct, LRESULT * result )
{
	CallHelp();
};

/* ------------------------------------------------------------------- */

