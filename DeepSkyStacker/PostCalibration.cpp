// ResultParameters.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "PostCalibration.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "ProgressDlg.h"
#include "MasterFrames.h"
#include "CosmeticEngine.h"

// CPostCalibration dialog

IMPLEMENT_DYNAMIC(CPostCalibration, CChildPropertyPage)

/* ------------------------------------------------------------------- */

CPostCalibration::CPostCalibration()
	: CChildPropertyPage(CPostCalibration::IDD)
{
	m_bFirstActivation = TRUE;
    m_pStackingTasks = NULL;
}

/* ------------------------------------------------------------------- */

CPostCalibration::~CPostCalibration()
{
}

/* ------------------------------------------------------------------- */

void CPostCalibration::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TITLE, m_Title);

	DDX_Control(pDX, IDC_DETECTCLEANHOT, m_DetectCleanHot);
	DDX_Control(pDX, IDC_HOTFILTERSTATIC, m_HotFilterText);
	DDX_Control(pDX, IDC_HOTFILTER, m_HotFilter);
	DDX_Control(pDX, IDC_HOTDETECTIONSTATIC, m_HotDetectionText);
	DDX_Control(pDX, IDC_HOTDETECTION, m_HotDetection);

	DDX_Control(pDX, IDC_DETECTCLEANCOLD, m_DetectCleanCold);
	DDX_Control(pDX, IDC_COLDFILTERSTATIC, m_ColdFilterText);
	DDX_Control(pDX, IDC_COLDFILTER, m_ColdFilter);
	DDX_Control(pDX, IDC_COLDDETECTIONSTATIC, m_ColdDetectionText);
	DDX_Control(pDX, IDC_COLDDETECTION, m_ColdDetection);

	DDX_Control(pDX, IDC_STRONG1, m_Strong1);
	DDX_Control(pDX, IDC_STRONG2, m_Strong2);
	DDX_Control(pDX, IDC_WEAK1, m_Weak1);
	DDX_Control(pDX, IDC_WEAK2, m_Weak2);

	DDX_Control(pDX, IDC_SAVEDELTAIMAGE, m_SaveDelta);

	DDX_Control(pDX, IDC_REPLACETEXT, m_ReplaceText);
	DDX_Control(pDX, IDC_REPLACEMETHOD, m_ReplaceMethod);
	DDX_Control(pDX, IDC_TESTCOSMETIC, m_Test);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CPostCalibration, CChildPropertyPage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_DETECTCLEANHOT, &CPostCalibration::OnBnClickedDetectCleanHotCold)
	ON_BN_CLICKED(IDC_DETECTCLEANCOLD, &CPostCalibration::OnBnClickedDetectCleanHotCold)
	ON_BN_CLICKED(IDC_SAVEDELTAIMAGE, &CPostCalibration::OnBnClickedDetectCleanHotCold)
	ON_NOTIFY(NM_LINKCLICK, IDC_REPLACEMETHOD, OnCosmeticMethod)
	ON_NOTIFY(NM_LINKCLICK, IDC_TESTCOSMETIC, OnTest)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

static void	MakeSmallLabel(CLabel & label)
{
	CFont *				pFont;
	LOGFONT				lf;

	pFont = label.GetFont();
	pFont->GetLogFont(&lf);

	lf.lfHeight += 3;
	label.SetFont(lf);

	label.SetTransparent(TRUE);
};

/* ------------------------------------------------------------------- */

static void	GetMethodText(COSMETICREPLACE cr, CString & strText)
{
	CMenu				menu;
	CMenu *				popup;

	menu.LoadMenu(IDR_COSMETICMETHOD);
	popup = menu.GetSubMenu(0);

	if (cr == CR_GAUSSIAN)
		popup->GetMenuString(ID_COSMETICMETHOD_GAUSSIAN, strText, MF_BYCOMMAND);
	else
		popup->GetMenuString(ID_COSMETICMETHOD_MEDIAN, strText, MF_BYCOMMAND);
};

/* ------------------------------------------------------------------- */

void CPostCalibration::UpdateControls()
{
	CStackSettings *	pDialog = dynamic_cast<CStackSettings *>(GetParent()->GetParent());
	BOOL				bEnableHot  = m_DetectCleanHot.GetCheck(),
						bEnableCold = m_DetectCleanCold.GetCheck();

	m_HotFilterText.EnableWindow(bEnableHot);
	m_HotFilter.EnableWindow(bEnableHot);
	m_HotDetectionText.EnableWindow(bEnableHot);
	m_HotDetection.EnableWindow(bEnableHot);
	m_Weak1.SetTextColor(GetSysColor(bEnableHot ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
	m_Strong1.SetTextColor(GetSysColor(bEnableHot ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

	m_ColdFilterText.EnableWindow(bEnableCold);
	m_ColdFilter.EnableWindow(bEnableCold);
	m_ColdDetectionText.EnableWindow(bEnableCold);
	m_ColdDetection.EnableWindow(bEnableCold);
	m_Weak2.SetTextColor(GetSysColor(bEnableCold ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
	m_Strong2.SetTextColor(GetSysColor(bEnableCold ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

	m_ReplaceText.EnableWindow(bEnableHot || bEnableCold);
	m_ReplaceMethod.EnableWindow(bEnableHot || bEnableCold);
	m_Test.EnableWindow(bEnableHot || bEnableCold);
	m_SaveDelta.EnableWindow(bEnableHot || bEnableCold);

	m_ReplaceMethod.SetTextColor(bEnableHot || bEnableCold ? RGB(0, 0, 128) : GetSysColor(COLOR_GRAYTEXT));
	m_Test.SetTextColor(bEnableHot || bEnableCold ? RGB(0, 0, 128) : GetSysColor(COLOR_GRAYTEXT));


	if (pDialog)
		pDialog->UpdateControls();
};

/* ------------------------------------------------------------------- */

void CPostCalibration::UpdateSettingsTexts()
{
	CString			strText;
	LONG			lValue;
	double			fValue;

	lValue = m_Settings.m_lHotFilter;
	strText.Format(m_strPixelMask, lValue);
	m_HotFilterText.SetWindowText(strText);

	fValue = m_Settings.m_fHotDetection;
	strText.Format(m_strPercentMask, fValue);
	m_HotDetectionText.SetWindowText(strText);

	lValue = m_Settings.m_lColdFilter;
	strText.Format(m_strPixelMask, lValue);
	m_ColdFilterText.SetWindowText(strText);

	fValue = m_Settings.m_fColdDetection;
	strText.Format(m_strPercentMask, fValue);
	m_ColdDetectionText.SetWindowText(strText);
};

/* ------------------------------------------------------------------- */

void CPostCalibration::UpdateControlsFromSettings()
{
	m_DetectCleanHot.SetCheck(m_Settings.m_bHot);
	m_HotFilter.SetPos(m_Settings.m_lHotFilter);
	m_HotDetection.SetPos(1000-m_Settings.m_fHotDetection*10.0);

	m_DetectCleanCold.SetCheck(m_Settings.m_bCold);
	m_ColdFilter.SetPos(m_Settings.m_lColdFilter);
	m_ColdDetection.SetPos(1000-m_Settings.m_fColdDetection*10.0);

	m_SaveDelta.SetCheck(m_Settings.m_bSaveDeltaImage);

	CString				strText;

	GetMethodText(m_Settings.m_Replace, strText);
	m_ReplaceMethod.SetText(strText);

	UpdateSettingsTexts();
};

/* ------------------------------------------------------------------- */

void CPostCalibration::UpdateSettingsFromControls()
{
	m_Settings.m_bHot			= m_DetectCleanHot.GetCheck();
	m_Settings.m_lHotFilter		= m_HotFilter.GetPos();
	m_Settings.m_fHotDetection	= 100.0-(double)m_HotDetection.GetPos()/10.0;

	m_Settings.m_bCold			= m_DetectCleanCold.GetCheck();
	m_Settings.m_lColdFilter	= m_ColdFilter.GetPos();
	m_Settings.m_fColdDetection	= 100.0-(double)m_ColdDetection.GetPos()/10.0;

	m_Settings.m_bSaveDeltaImage = m_SaveDelta.GetCheck();

	UpdateSettingsTexts();
};

/* ------------------------------------------------------------------- */

BOOL CPostCalibration::OnSetActive()
{
	if (m_bFirstActivation)
	{
		m_Title.SetTextColor(RGB(0, 0, 0));
		m_Title.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

		m_HotFilterText.GetWindowText(m_strPixelMask);
		m_HotDetectionText.GetWindowText(m_strPercentMask);

		m_HotFilter.SetRange(1, 6);
		m_HotDetection.SetRange(10, 990);

		m_ColdFilter.SetRange(1, 6);
		m_ColdDetection.SetRange(10, 990);

		MakeSmallLabel(m_Strong1);
		MakeSmallLabel(m_Strong2);
		MakeSmallLabel(m_Weak1);
		MakeSmallLabel(m_Weak2);

		m_ReplaceText.SetTransparent(TRUE);
		m_ReplaceMethod.SetTransparent(TRUE);
		m_Test.SetTransparent(TRUE);

		m_ReplaceMethod.SetLink(TRUE, TRUE);

		if (m_pStackingTasks)
			m_Test.SetLink(TRUE, TRUE);
		else
			m_Test.ShowWindow(SW_HIDE);

		UpdateControlsFromSettings();
		UpdateControls();
		m_bFirstActivation = FALSE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */
// CPostCalibration message handlers

void CPostCalibration::OnBnClickedDetectCleanHotCold()
{
	UpdateSettingsFromControls();
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CPostCalibration::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateSettingsFromControls();
	CChildPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
};

/* ------------------------------------------------------------------- */

void CPostCalibration::OnTest( NMHDR * pNotifyStruct, LRESULT * result )
{
	ZFUNCTRACE_RUNTIME();
	// Load the reference light frame
	if (m_pStackingTasks)
	{
		CAllStackingTasks			tasks = *(m_pStackingTasks);

		// Retrieve the first light frame
		tasks.ResolveTasks();
		if (tasks.m_vStacks.size())
		{
			CDSSProgressDlg				dlg;
			CStackingInfo &				StackingInfo = tasks.m_vStacks[0];

			if (StackingInfo.m_pLightTask &&
				StackingInfo.m_pLightTask->m_vBitmaps.size())
			{
				// Keep Only the first light frame
				CString				strFileName;
				CString				strText;

				StackingInfo.m_pLightTask->m_vBitmaps.resize(1);
				strFileName = StackingInfo.m_pLightTask->m_vBitmaps[0].m_strFileName;

				CMasterFrames	MasterFrames;

				// Disable all the tasks except the one used by StackingInfo
				for (LONG i = 0;i<tasks.m_vTasks.size();i++)
					tasks.m_vTasks[i].m_bDone = TRUE;
				if (StackingInfo.m_pDarkFlatTask)
					StackingInfo.m_pDarkFlatTask->m_bDone = FALSE;
				if (StackingInfo.m_pOffsetTask)
					StackingInfo.m_pOffsetTask->m_bDone = FALSE;
				if (StackingInfo.m_pDarkTask)
					StackingInfo.m_pDarkTask->m_bDone = FALSE;
				if (StackingInfo.m_pFlatTask)
					StackingInfo.m_pFlatTask->m_bDone = FALSE;
				if (StackingInfo.m_pLightTask)
					StackingInfo.m_pLightTask->m_bDone = FALSE;

				strText.LoadString(IDS_COMPUTINGCOSMETICSTATS);
				dlg.Start(strText, 0, FALSE);

				dlg.SetJointProgress(TRUE);
				tasks.DoAllPreTasks(&dlg);
				MasterFrames.LoadMasters(&StackingInfo, &dlg);

				// Load the image
				CBitmapInfo		bmpInfo;
				// Load the bitmap
				if (GetPictureInfo(strFileName, bmpInfo) && bmpInfo.CanLoad())
				{
					CSmartPtr<CMemoryBitmap>	pBitmap;
					CString						strDescription;

					bmpInfo.GetDescription(strDescription);

					if (bmpInfo.m_lNrChannels==3)
						strText.Format(IDS_LOADRGBLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)strFileName);
					else
						strText.Format(IDS_LOADGRAYLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)strFileName);
					dlg.Start2(strText, 0);

					if (::LoadPicture(strFileName, &pBitmap, &dlg))
					{
						// Apply offset, dark and flat to lightframe
						MasterFrames.ApplyAllMasters(pBitmap, nullptr, &dlg);

						// Then simulate the cosmetic on this image
						CCosmeticStats			Stats;

						SimulateCosmetic(pBitmap, m_Settings, Stats, &dlg);

						// Show the results
						double					fHotPct = (double)Stats.m_lNrDetectedHotPixels/Stats.m_lNrTotalPixels * 100.0,
												fColdPct= (double)Stats.m_lNrDetectedColdPixels/Stats.m_lNrTotalPixels * 100.0;

						CString					strCosmeticStat;

						strCosmeticStat.Format(IDS_COSMETICSTATS, Stats.m_lNrDetectedHotPixels, fHotPct, Stats.m_lNrDetectedColdPixels, fColdPct);

						AfxMessageBox(strCosmeticStat, MB_ICONINFORMATION | MB_OK);
					};

					dlg.End2();
				};
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CPostCalibration::OnCosmeticMethod( NMHDR * pNotifyStruct, LRESULT * result )
{
	ZFUNCTRACE_RUNTIME();
	CPoint				pt;
	CMenu				menu;
	CMenu *				popup;
	int					nResult;

	menu.LoadMenu(IDR_COSMETICMETHOD);
	popup = menu.GetSubMenu(0);

	CRect				rc;
	CString				strText;

	m_ReplaceMethod.GetWindowRect(&rc);
	pt.x = rc.left;
	pt.y = rc.bottom;

	if (m_Settings.m_Replace == CR_MEDIAN)
		popup->CheckMenuItem(ID_COSMETICMETHOD_MEDIAN, MF_BYCOMMAND | MF_CHECKED);
	else
		popup->CheckMenuItem(ID_COSMETICMETHOD_GAUSSIAN, MF_BYCOMMAND | MF_CHECKED);

	nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, nullptr);

	if (nResult == ID_COSMETICMETHOD_MEDIAN)
	{
		popup->GetMenuString(ID_COSMETICMETHOD_MEDIAN, strText, MF_BYCOMMAND);
		m_ReplaceMethod.SetText(strText);
		m_Settings.m_Replace = CR_MEDIAN;
	}
	else if (nResult == ID_COSMETICMETHOD_GAUSSIAN)
	{
		popup->GetMenuString(ID_COSMETICMETHOD_GAUSSIAN, strText, MF_BYCOMMAND);
		m_ReplaceMethod.SetText(strText);
		m_Settings.m_Replace = CR_GAUSSIAN;
	};
};

/* ------------------------------------------------------------------- */
