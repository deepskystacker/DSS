// ProcessingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "DeepStackerDlg.h"
#include "ProcessingDlg.h"
#include "ProgressDlg.h"
#include <algorithm>
#include "SettingsDlg.h"
#include "Registry.h"
#include "SavePicture.h"
#include "StarMask.h"
#include "FITSUtil.h"
#include "TIFFUtil.h"
#include "StarMaskDlg.h"

#define _USE_MATH_DEFINES
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const DWORD			WM_INITNEWPICTURE = WM_USER+1;

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CProcessingDlg dialog


CProcessingDlg::CProcessingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessingDlg::IDD, pParent), m_Settings(_T(""))
{
	//{{AFX_DATA_INIT(CProcessingDlg)
	//}}AFX_DATA_INIT
	m_bDirty		 = FALSE;
}

/* ------------------------------------------------------------------- */

void CProcessingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessingDlg)
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_PROCESSING_PROGRESS, m_ProcessingProgress);
	DDX_Control(pDX, IDC_SETTINGS, m_SettingsRect);
	DDX_Control(pDX, IDC_ORIGINAL_HISTOGRAM, m_OriginalHistogramStatic);
	DDX_Control(pDX, IDC_PICTURE, m_PictureStatic);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CProcessingDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessingDlg)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PROCESS, OnProcess)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_INITNEWPICTURE, OnInitNewPicture)
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

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CProcessingDlg message handlers

BOOL CProcessingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

 	CRect			rcSettings;

	m_Info.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

	m_SettingsRect.GetWindowRect(&rcSettings);
	ScreenToClient(&rcSettings);
	rcSettings.left = 0;
	rcSettings.top -= 11;

	m_Settings.AddPage(&m_tabRGB);
	m_Settings.AddPage(&m_tabLuminance);
	m_Settings.AddPage(&m_tabSaturation);

	m_Settings.EnableStackedTabs( FALSE );
	m_Settings.Create (this, WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0);

	m_Settings.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_Settings.ModifyStyle( 0, WS_TABSTOP );

	// move to left upper corner

	m_Settings.MoveWindow(&rcSettings, TRUE);
	m_Settings.ShowWindow(SW_SHOWNA);

	m_Picture.CreateFromStatic(&m_PictureStatic);
	m_ControlPos.SetParent(this);

	m_tabLuminance.m_MidTone.SetRange(0, 1000, TRUE);
	m_tabLuminance.m_MidTone.SetPos(200);
	m_tabLuminance.m_MidAngle.SetRange(0, 45, TRUE);
	m_tabLuminance.m_MidAngle.SetPos(20);

	m_tabLuminance.m_DarkAngle.SetRange(0, 45, TRUE);
	m_tabLuminance.m_DarkAngle.SetPos(0);
	m_tabLuminance.m_DarkPower.SetRange(0, 1000, TRUE);
	m_tabLuminance.m_DarkPower.SetPos(500);

	m_tabLuminance.m_HighAngle.SetRange(0, 45, TRUE);
	m_tabLuminance.m_HighAngle.SetPos(0);
	m_tabLuminance.m_HighPower.SetRange(0, 1000, TRUE);
	m_tabLuminance.m_HighPower.SetPos(500);

	m_tabLuminance.UpdateTexts();

	m_tabSaturation.m_Saturation.SetRange(0, 100, TRUE);
	m_tabSaturation.m_Saturation.SetPos(50);
	m_tabSaturation.UpdateTexts();

	m_OriginalHistogram.CreateFromStatic(&m_OriginalHistogramStatic);
	m_OriginalHistogram.SetBltMode(CWndImage::bltFitXY);
	m_OriginalHistogram.SetAlign(CWndImage::bltCenter, CWndImage::bltCenter);

	m_ControlPos.AddControl(IDC_PICTURE, CP_RESIZE_VERTICAL | CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(&m_Settings, CP_MOVE_VERTICAL);
	m_ControlPos.AddControl(&m_OriginalHistogram, CP_MOVE_VERTICAL);
	m_ControlPos.AddControl(&m_Info, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(&m_ProcessingProgress, CP_MOVE_VERTICAL | CP_RESIZE_HORIZONTAL);

	m_Picture.EnableZoom(TRUE);
	m_Picture.SetImageSink(&m_SelectRectSink);
	m_Picture.SetBltMode(CWndImage::bltFitXY);
	m_Picture.SetAlign(CWndImage::bltCenter, CWndImage::bltCenter);

	COLORREF		crReds[3] = {RGB(0, 0, 0), RGB(128, 0, 0), RGB(255, 0, 0)};
	COLORREF		crGreens[3] = {RGB(0, 0, 0), RGB(0, 128, 0), RGB(0, 255, 0)};
	COLORREF		crBlues[3] = {RGB(0, 0, 0), RGB(0, 0, 128), RGB(0, 0, 255)};

	SetTimer(1, 100, NULL);

	m_fGradientOffset = 0.0;
	m_fGradientRange  = 65535.0;

	m_tabRGB.m_RedGradient.SetOrientation(CGradientCtrl::ForceHorizontal);
	m_tabRGB.m_RedGradient.SetPegSide(TRUE, FALSE);
	m_tabRGB.m_RedGradient.SetPegSide(FALSE, TRUE);
	m_tabRGB.m_RedGradient.GetGradient().SetStartPegColour(RGB(0, 0, 0));
	m_tabRGB.m_RedGradient.GetGradient().AddPeg(crReds[0], 0.0, 0);
	m_tabRGB.m_RedGradient.GetGradient().AddPeg(crReds[1], 0.5, 1);
	m_tabRGB.m_RedGradient.GetGradient().AddPeg(crReds[2], 1.0, 2);
	m_tabRGB.m_RedGradient.GetGradient().SetEndPegColour(RGB(255, 0, 0));
	m_tabRGB.m_RedGradient.GetGradient().SetBackgroundColour(RGB(255, 255, 255));
	m_tabRGB.m_RedGradient.GetGradient().SetInterpolationMethod(CGradient::Linear);
	m_tabRGB.SetRedAdjustMethod(HAT_LINEAR);

	m_tabRGB.m_GreenGradient.SetOrientation(CGradientCtrl::ForceHorizontal);
	m_tabRGB.m_GreenGradient.SetPegSide(TRUE, FALSE);
	m_tabRGB.m_GreenGradient.SetPegSide(FALSE, TRUE);
	m_tabRGB.m_GreenGradient.GetGradient().SetStartPegColour(RGB(0, 0, 0));
	m_tabRGB.m_GreenGradient.GetGradient().AddPeg(crGreens[0], 0.0, 0);
	m_tabRGB.m_GreenGradient.GetGradient().AddPeg(crGreens[1], 0.5, 1);
	m_tabRGB.m_GreenGradient.GetGradient().AddPeg(crGreens[2], 1.0, 2);
	m_tabRGB.m_GreenGradient.GetGradient().SetEndPegColour(RGB(0, 255, 0));
	m_tabRGB.m_GreenGradient.GetGradient().SetBackgroundColour(RGB(255, 255, 255));
	m_tabRGB.m_GreenGradient.GetGradient().SetInterpolationMethod(CGradient::Linear);
	m_tabRGB.SetGreenAdjustMethod(HAT_LINEAR);

	m_tabRGB.m_BlueGradient.SetOrientation(CGradientCtrl::ForceHorizontal);
	m_tabRGB.m_BlueGradient.SetPegSide(TRUE, FALSE);
	m_tabRGB.m_BlueGradient.SetPegSide(FALSE, TRUE);
	m_tabRGB.m_BlueGradient.GetGradient().SetStartPegColour(RGB(0, 0, 0));
	m_tabRGB.m_BlueGradient.GetGradient().AddPeg(crBlues[0], 0.0, 0);
	m_tabRGB.m_BlueGradient.GetGradient().AddPeg(crBlues[1], 0.5, 1);
	m_tabRGB.m_BlueGradient.GetGradient().AddPeg(crBlues[2], 1.0, 2);
	m_tabRGB.m_BlueGradient.GetGradient().SetEndPegColour(RGB(0, 0, 255));
	m_tabRGB.m_BlueGradient.GetGradient().SetBackgroundColour(RGB(255, 255, 255));
	m_tabRGB.m_BlueGradient.GetGradient().SetInterpolationMethod(CGradient::Linear);
	m_tabRGB.SetBlueAdjustMethod(HAT_LINEAR);

	m_ProcessingProgress.SetRange(0, 100);
	m_ProcessingProgress.SetPos(100);

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CProcessingDlg::UpdateControls()
{
	BOOL			bUndo = FALSE,
					bRedo = FALSE;

	if (GetDeepStack(this).IsLoaded())
	{
		m_tabRGB.EnableWindow(TRUE);
		m_tabLuminance.EnableWindow(TRUE);
		m_tabSaturation.EnableWindow(TRUE);

		bUndo = m_lProcessParams.IsBackwardAvailable();
		bRedo = m_lProcessParams.IsForwardAvailable();

		m_tabRGB.m_Undo.EnableWindow(bUndo);
		m_tabRGB.m_Redo.EnableWindow(bRedo);
		m_tabLuminance.m_Undo.EnableWindow(bUndo);
		m_tabLuminance.m_Redo.EnableWindow(bRedo);
		m_tabSaturation.m_Undo.EnableWindow(bUndo);
		m_tabSaturation.m_Redo.EnableWindow(bRedo);
	}
	else
	{
		m_tabRGB.EnableWindow(FALSE);
		m_tabLuminance.EnableWindow(FALSE);
		m_tabSaturation.EnableWindow(FALSE);
	};
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::UpdateMonochromeControls()
{
	if (GetDeepStack(this).IsLoaded())
	{
		BOOL			bMonochrome;

		bMonochrome = GetDeepStack(this).GetStackedBitmap().IsMonochrome();

		m_tabRGB.m_GreenGradient.ShowWindow(bMonochrome ? SW_HIDE : SW_SHOW);
		m_tabRGB.m_BlueGradient.ShowWindow(bMonochrome ? SW_HIDE : SW_SHOW);
		m_tabRGB.m_GreenHAT.ShowWindow(bMonochrome ? SW_HIDE : SW_SHOW);
		m_tabRGB.m_BlueHAT.ShowWindow(bMonochrome ? SW_HIDE : SW_SHOW);
		m_tabRGB.m_LinkSettings.ShowWindow(bMonochrome ? SW_HIDE : SW_SHOW);

		CGradient &		RedGradient = m_tabRGB.m_RedGradient.GetGradient();

		if (bMonochrome)
		{
			// Change the colors of the red gradient
			m_tabRGB.m_LinkSettings.SetCheck(TRUE);
			RedGradient.SetStartPegColour(RGB(0, 0, 0));
			RedGradient.SetPeg(RedGradient.IndexFromId(0), RGB(0, 0, 0));
			RedGradient.SetPeg(RedGradient.IndexFromId(1), RGB(128, 128, 128));
			RedGradient.SetPeg(RedGradient.IndexFromId(2), RGB(255, 255, 255));
			RedGradient.SetEndPegColour(RGB(255, 255, 255));
			RedGradient.SetBackgroundColour(RGB(255, 255, 255));
		}
		else
		{
			// Change the colors of the red gradient
			RedGradient.SetStartPegColour(RGB(0, 0, 0));
			RedGradient.SetPeg(RedGradient.IndexFromId(0), RGB(0, 0, 0));
			RedGradient.SetPeg(RedGradient.IndexFromId(1), RGB(128, 0, 0));
			RedGradient.SetPeg(RedGradient.IndexFromId(2), RGB(255, 0, 0));
			RedGradient.SetEndPegColour(RGB(255, 0, 0));
			RedGradient.SetBackgroundColour(RGB(255, 255, 255));
		};
		m_tabRGB.m_RedGradient.Invalidate(TRUE);
	};
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::UpdateControlsFromParams()
{
	m_tabLuminance.m_MidTone.SetPos(m_ProcessParams.m_BezierAdjust.m_fMidtone*10);
	m_tabLuminance.m_MidAngle.SetPos(m_ProcessParams.m_BezierAdjust.m_fMidtoneAngle);

	m_tabLuminance.m_DarkAngle.SetPos(m_ProcessParams.m_BezierAdjust.m_fDarknessAngle);
	m_tabLuminance.m_DarkPower.SetPos(m_ProcessParams.m_BezierAdjust.m_fDarknessPower*10);

	m_tabLuminance.m_HighAngle.SetPos(m_ProcessParams.m_BezierAdjust.m_fHighlightAngle);
	m_tabLuminance.m_HighPower.SetPos(m_ProcessParams.m_BezierAdjust.m_fHighlightPower*10);

	m_tabLuminance.UpdateTexts();

	m_tabSaturation.m_Saturation.SetPos(m_ProcessParams.m_BezierAdjust.m_fSaturationShift + 50);
	m_tabSaturation.UpdateTexts();

	double					fMinRed,
							fMaxRed,
							fShiftRed;
	double					fMinGreen,
							fMaxGreen,
							fShiftGreen;
	double					fMinBlue,
							fMaxBlue,
							fShiftBlue;

	fMinRed = m_ProcessParams.m_HistoAdjust.GetRedAdjust().GetMin();
	fMaxRed = m_ProcessParams.m_HistoAdjust.GetRedAdjust().GetMax();
	fShiftRed = m_ProcessParams.m_HistoAdjust.GetRedAdjust().GetShift();

	fMinGreen = m_ProcessParams.m_HistoAdjust.GetGreenAdjust().GetMin();
	fMaxGreen = m_ProcessParams.m_HistoAdjust.GetGreenAdjust().GetMax();
	fShiftGreen = m_ProcessParams.m_HistoAdjust.GetGreenAdjust().GetShift();

	fMinBlue = m_ProcessParams.m_HistoAdjust.GetBlueAdjust().GetMin();
	fMaxBlue = m_ProcessParams.m_HistoAdjust.GetBlueAdjust().GetMax();
	fShiftBlue = m_ProcessParams.m_HistoAdjust.GetBlueAdjust().GetShift();

	double				fAbsMin,
						fAbsMax;
	double				fOffset;
	double				fRange;

	fAbsMin = min(fMinRed, min(fMinGreen, fMinBlue));
	fAbsMax = max(fMaxRed, min(fMaxGreen, fMaxBlue));

	fRange  = fAbsMax-fAbsMin;
	if (fRange * 1.10 <= 65535.0)
		fRange *= 1.10;
	
	fOffset = (fAbsMin+fAbsMax - fRange)/2.0;
	if (fOffset < 0)
		fOffset = 0.0;

	m_fGradientOffset = fOffset;
	m_fGradientRange  = fRange;

	CGradient &		RedGradient = m_tabRGB.m_RedGradient.GetGradient();
	RedGradient.SetPeg(RedGradient.IndexFromId(0), (float)((fMinRed - m_fGradientOffset)/m_fGradientRange));
	RedGradient.SetPeg(RedGradient.IndexFromId(1), (float)(fShiftRed/2.0 + 0.5));
	RedGradient.SetPeg(RedGradient.IndexFromId(2), (float)((fMaxRed - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_RedGradient.Invalidate(TRUE);
	m_tabRGB.SetRedAdjustMethod(m_ProcessParams.m_HistoAdjust.GetRedAdjust().GetAdjustMethod());

	CGradient &		GreenGradient = m_tabRGB.m_GreenGradient.GetGradient();
	GreenGradient.SetPeg(GreenGradient.IndexFromId(0), (float)((fMinGreen - m_fGradientOffset)/m_fGradientRange));
	GreenGradient.SetPeg(GreenGradient.IndexFromId(1), (float)(fShiftGreen/2.0 + 0.5));
	GreenGradient.SetPeg(GreenGradient.IndexFromId(2), (float)((fMaxGreen - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_GreenGradient.Invalidate(TRUE);
	m_tabRGB.SetGreenAdjustMethod(m_ProcessParams.m_HistoAdjust.GetGreenAdjust().GetAdjustMethod());

	CGradient &		BlueGradient = m_tabRGB.m_BlueGradient.GetGradient();
	BlueGradient.SetPeg(BlueGradient.IndexFromId(0), (float)((fMinBlue - m_fGradientOffset)/m_fGradientRange));
	BlueGradient.SetPeg(BlueGradient.IndexFromId(1), (float)(fShiftBlue/2.0 + 0.5));
	BlueGradient.SetPeg(BlueGradient.IndexFromId(2), (float)((fMaxBlue - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_BlueGradient.Invalidate(TRUE);
	m_tabRGB.SetBlueAdjustMethod(m_ProcessParams.m_HistoAdjust.GetBlueAdjust().GetAdjustMethod());
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnUndo()
{
	m_lProcessParams.MoveBackward();
	m_lProcessParams.GetCurrentParams(m_ProcessParams);
	UpdateControlsFromParams();
	ProcessAndShow(FALSE);
	ShowOriginalHistogram(FALSE);
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnRedo()
{
	m_lProcessParams.MoveForward();
	m_lProcessParams.GetCurrentParams(m_ProcessParams);
	UpdateControlsFromParams();
	ProcessAndShow(FALSE);
	ShowOriginalHistogram(FALSE);
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnSettings()
{
	CSettingsDlg			dlg;
	CDSSSettings &			Settings = GetDSSSettings(this);
	
	KillTimer(1);
	dlg.SetDSSSettings(&Settings, m_ProcessParams);
	dlg.DoModal();

	if (dlg.IsLoaded())
	{
		dlg.GetCurrentSettings(m_ProcessParams);
		UpdateControlsFromParams();
		ProcessAndShow(FALSE);
		ShowOriginalHistogram(FALSE);
		UpdateControls();
		m_bDirty = TRUE;
	};
	SetTimer(1, 100, NULL);
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	m_ControlPos.MoveControls();
}

/* ------------------------------------------------------------------- */

void CProcessingDlg::UpdateInfos()
{
	LONG		lISOSpeed;
	LONG		lTotalTime;
	LONG		lNrFrames;
	CString		strText;
	
	strText.LoadString(IDS_NOINFO);

	lISOSpeed	= GetDeepStack(this).GetStackedBitmap().GetISOSpeed();
	lTotalTime	= GetDeepStack(this).GetStackedBitmap().GetTotalTime();
	lNrFrames	= GetDeepStack(this).GetStackedBitmap().GetNrStackedFrames();

	if (lISOSpeed || lTotalTime || lNrFrames)
	{
		CString		strISO;
		CString		strTime;
		CString		strFrames;

		if (lISOSpeed)
		{
			strISO.Format(_T("%ld ISO - "), lISOSpeed);
		};

		if (lTotalTime)
		{
			DWORD			dwHour,
							dwMin,
							dwSec;

			dwHour = lTotalTime / 3600;
			lTotalTime -= dwHour * 3600;
			dwMin = lTotalTime / 60;
			lTotalTime -= dwMin * 60;
			dwSec = lTotalTime;

			if (dwHour)
				strTime.Format(IDS_EXPOSURE3, dwHour, dwMin, dwSec);
			else if (dwMin)
				strTime.Format(IDS_EXPOSURE2, dwMin, dwSec);
			else if (dwSec)
				strTime.Format(IDS_EXPOSURE1, dwSec);
			else
				strTime.Format(IDS_EXPOSURE0);
		};

		if (lNrFrames)
		{
			strFrames.Format(IDS_NRFRAMES, lNrFrames);
		};

		strText.Format(_T("%s\n%s%s%s"), m_strCurrentFile, strISO, strTime, strFrames);
	}
	else
		strText = m_strCurrentFile;

	m_Info.SetText(strText);
};

/* ------------------------------------------------------------------- */

LRESULT CProcessingDlg::OnInitNewPicture(WPARAM, LPARAM)
{
	ShowOriginalHistogram(FALSE);
	ResetSliders();
	m_ToProcess.Init(GetDeepStack(this).GetWidth(), GetDeepStack(this).GetHeight(), 500);

	m_lProcessParams.clear();
	m_Picture.SetImg((HBITMAP)NULL);
	ProcessAndShow(TRUE);

	UpdateInfos();

	return 1;
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow && GetDeepStack(this).IsNewStackedBitmap(TRUE))
		OnInitNewPicture(0, 0);//PostMessage(WM_INITNEWPICTURE);
}

/* ------------------------------------------------------------------- */

void	CProcessingDlg::LoadFile(LPCTSTR szFileName)
{
	ZFUNCTRACE_RUNTIME();
	CDSSProgressDlg		dlg;
	BOOL				bOk;

	BeginWaitCursor();
	GetDeepStack(this).Clear();
	GetDeepStack(this).SetProgress(&dlg);
	bOk = GetDeepStack(this).LoadStackedInfo(szFileName);
	GetDeepStack(this).SetProgress(NULL);
	EndWaitCursor();

	if (bOk)
	{
		m_strCurrentFile = szFileName;

		UpdateMonochromeControls();
		UpdateInfos();
		BeginWaitCursor();
		GetDeepStack(this).GetStackedBitmap().GetBezierAdjust(m_ProcessParams.m_BezierAdjust);
		GetDeepStack(this).GetStackedBitmap().GetHistogramAdjust(m_ProcessParams.m_HistoAdjust);
		
		UpdateControlsFromParams();

		ShowOriginalHistogram(FALSE);
		ResetSliders();
		m_ToProcess.Init(GetDeepStack(this).GetWidth(), GetDeepStack(this).GetHeight(), 500);

		m_lProcessParams.clear();
		m_Picture.SetImg((HBITMAP)NULL);
		ProcessAndShow(TRUE);
		EndWaitCursor();
		m_bDirty = FALSE;
	};

	SetTimer(1, 100, NULL);
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnLoaddsi() 
{
	if (AskToSave())
	{
		BOOL				bOk = FALSE;
		CString				strFilter;
		CRegistry			reg;
		CString				strBaseDirectory;

		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveDSIFolder"), strBaseDirectory);

		strFilter.LoadString(IDS_FILTER_DSIIMAGETIFF);
		CFileDialog			dlgOpen(TRUE, 
									_T(".DSImage"),
									NULL,
									OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST,
									strFilter,
									this);
		TCHAR				szBigBuffer[20000] = _T("");
		CDSSProgressDlg		dlg;

		if (strBaseDirectory.GetLength())
			dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);

		KillTimer(1);

		dlgOpen.m_ofn.lpstrFile = szBigBuffer;
		dlgOpen.m_ofn.nMaxFile  = sizeof(szBigBuffer) / sizeof(szBigBuffer[0]);

		if (dlgOpen.DoModal() == IDOK)
		{
			CString			strFile;
			POSITION		pos;

			pos = dlgOpen.GetStartPosition();
			while (pos && !bOk)
			{
				BeginWaitCursor();
				strFile = dlgOpen.GetNextPathName(pos);
				GetDeepStack(this).Clear();
				GetDeepStack(this).SetProgress(&dlg);
				bOk = GetDeepStack(this).LoadStackedInfo(strFile);
				GetDeepStack(this).SetProgress(NULL);
				EndWaitCursor();
			};

			if (bOk)
			{
				m_strCurrentFile = strFile;

				TCHAR		szDir[1+_MAX_DIR];
				TCHAR		szDrive[1+_MAX_DRIVE];

				_tsplitpath(strFile, szDrive, szDir, NULL, NULL);
				strBaseDirectory = szDrive;
				strBaseDirectory += szDir;

				reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveDSIFolder"), strBaseDirectory);

				UpdateMonochromeControls();
				UpdateInfos();
				BeginWaitCursor();
				GetDeepStack(this).GetStackedBitmap().GetBezierAdjust(m_ProcessParams.m_BezierAdjust);
				GetDeepStack(this).GetStackedBitmap().GetHistogramAdjust(m_ProcessParams.m_HistoAdjust);
				
				UpdateControlsFromParams();

				ShowOriginalHistogram(FALSE);
				// ResetSliders();
				m_ToProcess.Init(GetDeepStack(this).GetWidth(), GetDeepStack(this).GetHeight(), 500);

				m_lProcessParams.clear();
				m_Picture.SetImg((HBITMAP)NULL);
				ProcessAndShow(TRUE);
				EndWaitCursor();
				m_bDirty = FALSE;
			};
		};	

		SetTimer(1, 100, NULL);
	};
}

/* ------------------------------------------------------------------- */

BOOL CProcessingDlg::AskToSave()
{
	BOOL				bResult = FALSE;

	if (m_bDirty)
	{
		int				nResult;
		
		nResult = AfxMessageBox(IDS_MSG_SAVEMODIFICATIONS, MB_YESNOCANCEL | MB_ICONQUESTION);
		if (nResult == IDCANCEL)
			bResult = FALSE;
		else if (nResult == IDNO)
			bResult = TRUE;
		else
			bResult = SavePictureToFile();
	}
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::SaveDSImage()
{
	CRegistry			reg;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveDSIFolder"), strBaseDirectory);
	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveDSIExtension"), strBaseExtension);
	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveDSIIndex"), dwFilterIndex);

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".DSImage");

	CFileDialog			dlgOpen(FALSE, 
								_T(".DSImage"),
								NULL,
								OFN_EXPLORER | OFN_PATHMUSTEXIST,
								_T("DeepSkyStacker Image (.DSImage)|*.DSImage||"),
								this);

	if (strBaseDirectory.GetLength())
		dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;

	TCHAR				szBigBuffer[20000] = _T("");
	CDSSProgressDlg		dlg;

	dlgOpen.m_ofn.lpstrFile = szBigBuffer;
	dlgOpen.m_ofn.nMaxFile  = sizeof(szBigBuffer) / sizeof(szBigBuffer[0]);

	if (dlgOpen.DoModal() == IDOK)
	{
		POSITION		pos;

		pos = dlgOpen.GetStartPosition();
		if (pos)
		{
			CString		strFile;
			CRect		rcSelect;

			BeginWaitCursor();
			strFile = dlgOpen.GetNextPathName(pos);
			GetDeepStack(this).SetProgress(&dlg);

			if (m_SelectRectSink.GetSelectRect(rcSelect))
				GetDeepStack(this).SaveStackedInfo(strFile, &rcSelect);
			else
				GetDeepStack(this).SaveStackedInfo(strFile);
			GetDeepStack(this).SetProgress(NULL);

			TCHAR		szDir[1+_MAX_DIR];
			TCHAR		szDrive[1+_MAX_DRIVE];
			TCHAR		szExt[1+_MAX_EXT];

			_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;

			dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
			reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveDSIFolder"), strBaseDirectory);
			reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveDSIExtension"), strBaseExtension);
			reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveDSIIndex"), dwFilterIndex);

			EndWaitCursor();
		};
	};	
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::ProcessAndShow(BOOL bSaveUndo)
{
	UpdateHistogramAdjust();

	m_ProcessParams.m_BezierAdjust.m_fMidtone		= m_tabLuminance.m_MidTone.GetPos()/10.0;
	m_ProcessParams.m_BezierAdjust.m_fMidtoneAngle	= m_tabLuminance.m_MidAngle.GetPos();
	m_ProcessParams.m_BezierAdjust.m_fDarknessAngle = m_tabLuminance.m_DarkAngle.GetPos();
	m_ProcessParams.m_BezierAdjust.m_fHighlightAngle= m_tabLuminance.m_HighAngle.GetPos();
	m_ProcessParams.m_BezierAdjust.m_fHighlightPower= m_tabLuminance.m_HighPower.GetPos()/10.0;
	m_ProcessParams.m_BezierAdjust.m_fDarknessPower = m_tabLuminance.m_DarkPower.GetPos()/10.0;
	m_ProcessParams.m_BezierAdjust.m_fSaturationShift = m_tabSaturation.m_Saturation.GetPos()-50;
	m_ProcessParams.m_BezierAdjust.Clear();

	if (bSaveUndo)
		m_lProcessParams.AddParams(m_ProcessParams);

	UpdateControls();

	CRect			rcSelect;

	if (m_SelectRectSink.GetSelectRect(rcSelect))
		m_ToProcess.SetProcessRect(rcSelect);
	else
	{
		rcSelect.SetRectEmpty();
		m_ToProcess.SetProcessRect(rcSelect);
	};

	m_ToProcess.Reset();
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::CopyPictureToClipboard()
{
	HBITMAP			hBitmap;

	hBitmap = m_Picture.GetBitmap();

	if (hBitmap)
		CopyBitmapToClipboard(hBitmap);
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::CreateStarMask()
{
	BOOL				bResult = FALSE;

	if (GetDeepStack(this).IsLoaded())
	{
		KillTimer(1);

		CStarMaskDlg				dlgStarMask;

		dlgStarMask.SetBaseFileName(m_strCurrentFile);
		if (dlgStarMask.DoModal()==IDOK)
		{
			CDSSProgressDlg				dlg;
			CSmartPtr<CMemoryBitmap>	pBitmap;
			CStarMaskEngine				starmask;
			CSmartPtr<CMemoryBitmap>	pStarMask;

			dlg.SetJointProgress(TRUE);
			GetDeepStack(this).GetStackedBitmap().GetBitmap(&pBitmap, &dlg);
			if (starmask.CreateStarMask2(pBitmap, &pStarMask, &dlg))
			{
				// Save the star mask to a file
				CString					strFileName;
				CString					strText;
				BOOL					bFits;
				CString					strDescription;

				strDescription.LoadString(IDS_STARMASKDESCRIPTION);

				dlgStarMask.GetOutputFileName(strFileName, bFits);
				strText.Format(IDS_SAVINGSTARMASK, (LPCTSTR)strFileName);
				dlg.Start2((LPCTSTR)strText, 0);
				if (bFits)
					WriteFITS((LPCTSTR)strFileName, pStarMask, &dlg, strDescription);
				else
					WriteTIFF((LPCTSTR)strFileName, pStarMask, &dlg, strDescription);
			};
		};
		SetTimer(1, 100, NULL);
	}
	else
	{
		AfxMessageBox(IDS_MSG_NOPICTUREFORSTARMASK, MB_OK | MB_ICONSTOP);	
	};
};

/* ------------------------------------------------------------------- */

BOOL CProcessingDlg::SavePictureToFile()
{
	BOOL				bResult = FALSE;
	CRegistry			reg;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	DWORD				dwApplied = 0;
	DWORD				dwCompression = (DWORD)TC_NONE;
	CRect				rcSelect;

	if (GetDeepStack(this).IsLoaded())
	{
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SavePictureFolder"), strBaseDirectory);
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SavePictureExtension"), strBaseExtension);
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SavePictureIndex"), dwFilterIndex);
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveApplySetting"), dwApplied);
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveCompression"), dwCompression);

		if (!strBaseExtension.GetLength())
			strBaseExtension = _T(".tif");

		CSavePicture				dlgOpen(FALSE, 
									_T(".TIF"),
									NULL,
									OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
									OUTPUTFILE_FILTERS,
									this);

		if (m_SelectRectSink.GetSelectRect(rcSelect))
			dlgOpen.SetUseRect(TRUE, TRUE);
		if (dwApplied)
			dlgOpen.SetApplied(TRUE);

		dlgOpen.SetCompression((TIFFCOMPRESSION)dwCompression);

		if (strBaseDirectory.GetLength())
			dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
		dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;

		TCHAR				szBigBuffer[20000] = _T("");
		CDSSProgressDlg		dlg;

		dlgOpen.m_ofn.lpstrFile = szBigBuffer;
		dlgOpen.m_ofn.nMaxFile  = sizeof(szBigBuffer) / sizeof(szBigBuffer[0]);

		if (dlgOpen.DoModal() == IDOK)
		{
			POSITION		pos;

			pos = dlgOpen.GetStartPosition();
			if (pos)
			{
				CString			strFile;
				LPRECT			lpRect = NULL;
				BOOL			bApply;
				BOOL			bUseRect;
				TIFFCOMPRESSION	Compression;

				bApply   = dlgOpen.GetApplied();
				bUseRect = dlgOpen.GetUseRect();
				Compression = dlgOpen.GetCompression();

				if (bUseRect && m_SelectRectSink.GetSelectRect(rcSelect))
					lpRect = &rcSelect;

				BeginWaitCursor();
				strFile = dlgOpen.GetNextPathName(pos);
				if (dlgOpen.m_ofn.nFilterIndex == 1)
					GetDeepStack(this).GetStackedBitmap().SaveTIFF16Bitmap(strFile, lpRect, &dlg, bApply, Compression);
				else if (dlgOpen.m_ofn.nFilterIndex == 2)
					GetDeepStack(this).GetStackedBitmap().SaveTIFF32Bitmap(strFile, lpRect, &dlg, bApply, FALSE, Compression);
				else if (dlgOpen.m_ofn.nFilterIndex == 3)
					GetDeepStack(this).GetStackedBitmap().SaveTIFF32Bitmap(strFile, lpRect, &dlg, bApply, TRUE, Compression);
				else if (dlgOpen.m_ofn.nFilterIndex == 4)
					GetDeepStack(this).GetStackedBitmap().SaveFITS16Bitmap(strFile, lpRect, &dlg, bApply);
				else if (dlgOpen.m_ofn.nFilterIndex == 5)
					GetDeepStack(this).GetStackedBitmap().SaveFITS32Bitmap(strFile, lpRect, &dlg, bApply, FALSE);
				else if (dlgOpen.m_ofn.nFilterIndex == 6)
					GetDeepStack(this).GetStackedBitmap().SaveFITS32Bitmap(strFile, lpRect, &dlg, bApply, TRUE);

				TCHAR		szDir[1+_MAX_DIR];
				TCHAR		szDrive[1+_MAX_DRIVE];
				TCHAR		szExt[1+_MAX_EXT];

				_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
				strBaseDirectory = szDrive;
				strBaseDirectory += szDir;
				strBaseExtension = szExt;

				dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
				reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SavePictureFolder"), strBaseDirectory);
				reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SavePictureExtension"), strBaseExtension);
				reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SavePictureIndex"), dwFilterIndex);
				reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveApplySetting"), bApply);
				reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("SaveCompression"), (DWORD)Compression);

				EndWaitCursor();

				m_strCurrentFile = strFile;
				UpdateInfos();
				m_bDirty = FALSE;
				bResult = TRUE;
			};
		};
	}
	else
	{
		AfxMessageBox(IDS_MSG_NOPICTURETOSAVE, MB_OK | MB_ICONSTOP);	
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnProcess() 
{
	ProcessAndShow(TRUE);	
}

/* ------------------------------------------------------------------- */

void CProcessingDlg::ResetSliders()
{
	CRGBHistogram &		Histogram = GetDeepStack(this).GetOriginalHistogram();

	m_ProcessParams.m_BezierAdjust.Reset();

	float				RedMarks[3];
	float				GreenMarks[3];
	float				BlueMarks[3];

	m_fGradientOffset = 0.0;
	m_fGradientRange  = 65535.0;

	RedMarks[0]		= Histogram.GetRedHistogram().GetMin();
	GreenMarks[0]	= Histogram.GetGreenHistogram().GetMin();
	BlueMarks[0]	= Histogram.GetBlueHistogram().GetMin();

	RedMarks[2]		= Histogram.GetRedHistogram().GetMax();
	GreenMarks[2]	= Histogram.GetGreenHistogram().GetMax();
	BlueMarks[2]	= Histogram.GetBlueHistogram().GetMax();


	CGradient &		RedGradient = m_tabRGB.m_RedGradient.GetGradient();
	RedGradient.SetPeg(RedGradient.IndexFromId(0), (float)((RedMarks[0] - m_fGradientOffset)/m_fGradientRange));
	RedGradient.SetPeg(RedGradient.IndexFromId(1), (float)0.5);
	RedGradient.SetPeg(RedGradient.IndexFromId(2), (float)((RedMarks[2] - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_RedGradient.Invalidate(TRUE);
	m_tabRGB.SetRedAdjustMethod(m_ProcessParams.m_HistoAdjust.GetRedAdjust().GetAdjustMethod());

	CGradient &		GreenGradient = m_tabRGB.m_GreenGradient.GetGradient();
	GreenGradient.SetPeg(GreenGradient.IndexFromId(0), (float)((GreenMarks[0] - m_fGradientOffset)/m_fGradientRange));
	GreenGradient.SetPeg(GreenGradient.IndexFromId(1), (float)0.5);
	GreenGradient.SetPeg(GreenGradient.IndexFromId(2), (float)((GreenMarks[2] - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_GreenGradient.Invalidate(TRUE);
	m_tabRGB.SetGreenAdjustMethod(m_ProcessParams.m_HistoAdjust.GetGreenAdjust().GetAdjustMethod());

	CGradient &		BlueGradient = m_tabRGB.m_BlueGradient.GetGradient();
	BlueGradient.SetPeg(BlueGradient.IndexFromId(0), (float)((BlueMarks[0] - m_fGradientOffset)/m_fGradientRange));
	BlueGradient.SetPeg(BlueGradient.IndexFromId(1), (float)0.5);
	BlueGradient.SetPeg(BlueGradient.IndexFromId(2), (float)((BlueMarks[2] - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_BlueGradient.Invalidate(TRUE);
	m_tabRGB.SetBlueAdjustMethod(m_ProcessParams.m_HistoAdjust.GetBlueAdjust().GetAdjustMethod());

	m_tabLuminance.m_MidTone.SetPos(m_ProcessParams.m_BezierAdjust.m_fMidtone*10);
	m_tabLuminance.m_MidAngle.SetPos(m_ProcessParams.m_BezierAdjust.m_fMidtoneAngle);

	m_tabLuminance.m_DarkAngle.SetPos(m_ProcessParams.m_BezierAdjust.m_fDarknessAngle);
	m_tabLuminance.m_DarkPower.SetPos(m_ProcessParams.m_BezierAdjust.m_fDarknessPower*10);

	m_tabLuminance.m_HighAngle.SetPos(m_ProcessParams.m_BezierAdjust.m_fHighlightAngle);
	m_tabLuminance.m_HighPower.SetPos(m_ProcessParams.m_BezierAdjust.m_fHighlightPower*10);

	m_tabLuminance.UpdateTexts();

	m_tabSaturation.m_Saturation.SetPos(m_ProcessParams.m_BezierAdjust.m_fSaturationShift + 50);
	m_tabSaturation.UpdateTexts();

	ShowOriginalHistogram(FALSE);
};

/* ------------------------------------------------------------------- */

class CColorOrder
{
public :
	COLORREF		m_crColor;
	LONG			m_lSize;

private :
	void	CopyFrom(const CColorOrder & co)
	{
		m_crColor = co.m_crColor;
		m_lSize	  = co.m_lSize;
	};

public :
	CColorOrder() {};
	CColorOrder(COLORREF crColor, LONG lSize) 
	{
		m_crColor	= crColor;
		m_lSize		= lSize;
	};
	virtual ~CColorOrder() {};
	CColorOrder(const CColorOrder & co)
	{
		CopyFrom(co);
	};

	CColorOrder & operator = (const CColorOrder & co)
	{
		CopyFrom(co);
		return *this;
	};

	bool operator < (const CColorOrder & co) const
	{
		return m_lSize < co.m_lSize;
	};
};

void CProcessingDlg::DrawHistoBar(Graphics * pGraphics, LONG lNrReds, LONG lNrGreens, LONG lNrBlues, LONG X, LONG lHeight)
{
	HPEN						hOldPen	= NULL;
	std::vector<CColorOrder>	vColors;
	LONG						lLastHeight = 0;

	vColors.push_back(CColorOrder(RGB(255,0,0), lNrReds));
	vColors.push_back(CColorOrder(RGB(0,255,0), lNrGreens));
	vColors.push_back(CColorOrder(RGB(0,0,255), lNrBlues));
	
	std::sort(vColors.begin(), vColors.end());

	for (LONG i = 0;i<vColors.size();i++)
	{
		if (vColors[i].m_lSize > lLastHeight)
		{
			// Create a color from the remaining values
			double			fRed, fGreen, fBlue;
			LONG			lNrColors = 1;

			fRed   = GetRValue(vColors[i].m_crColor);
			fGreen = GetGValue(vColors[i].m_crColor);
			fBlue  = GetBValue(vColors[i].m_crColor);
			
			for (LONG j = i+1;j<vColors.size();j++)
			{
				fRed   += GetRValue(vColors[j].m_crColor);
				fGreen += GetGValue(vColors[j].m_crColor);
				fBlue  += GetBValue(vColors[j].m_crColor);
				lNrColors++;
			};

			Pen				ColorPen(Color(fRed/lNrColors, fGreen/lNrColors, fBlue/lNrColors));

			pGraphics->DrawLine(&ColorPen, X, lHeight-lLastHeight, X, lHeight-vColors[i].m_lSize);
/*
			HPEN			hPen,
							hOldPen;

			hPen = ::CreatePen(PS_SOLID, 1, RGB(fRed/lNrColors, fGreen/lNrColors, fBlue/lNrColors));
			hOldPen = (HPEN)::SelectObject(hDC, hPen);
			::MoveToEx(hDC, X, lHeight-lLastHeight, NULL);
			::LineTo(hDC, X, lHeight-vColors[i].m_lSize);
			::SelectObject(hDC, hOldPen);
			::DeleteObject(hPen);*/

			lLastHeight = vColors[i].m_lSize;
		};
	};
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::DrawBezierCurve(Graphics * pGraphics, LONG lWidth, LONG lHeight)
{
	CBezierAdjust		BezierAdjust;
	POINT				pt;
	
	BezierAdjust.m_fMidtone			= m_tabLuminance.m_MidTone.GetPos()/10.0;
	BezierAdjust.m_fMidtoneAngle	= m_tabLuminance.m_MidAngle.GetPos();
	BezierAdjust.m_fDarknessAngle	= m_tabLuminance.m_DarkAngle.GetPos();
	BezierAdjust.m_fHighlightAngle	= m_tabLuminance.m_HighAngle.GetPos();
	BezierAdjust.m_fHighlightPower	= m_tabLuminance.m_HighPower.GetPos()/10.0;
	BezierAdjust.m_fDarknessPower	= m_tabLuminance.m_DarkPower.GetPos()/10.0;

	BezierAdjust.Clear();

	Pen					BlackPen(Color(0,0,0));
	std::vector<PointF>	vPoints;

	BlackPen.SetDashStyle(DashStyleDash);

	for (double i = 0;i<=1.0;i+=0.01)
	{
		double	j;

		j = BezierAdjust.GetValue(i);
		pt.x = i * lWidth;
		pt.y = lHeight - j * lHeight;
		vPoints.push_back(PointF(pt.x, pt.y));
	};

	pGraphics->DrawLines(&BlackPen, &vPoints[0], (LONG)vPoints.size());
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::DrawGaussCurves(Graphics * pGraphics, CRGBHistogram & Histogram, LONG lWidth, LONG lHeight)
{
	LONG				lNrValues;
	double				fAverage[3] = {0, 0, 0};
	double				fStdDev[3] = {0, 0, 0};
	double				fSum[3] = {0, 0, 0};
	double				fSquareSum[3] = {0, 0, 0};
	double				fTotalPixels[3] = {0, 0, 0};
	LONG				i;

	lNrValues = Histogram.GetRedHistogram().GetNrValues();
	
	if (lNrValues)
	{
		for (i = 0;i<lNrValues;i++)
		{
			LONG			lNrReds;
			LONG			lNrGreens;
			LONG			lNrBlues;

			Histogram.GetValues(i, lNrReds, lNrGreens, lNrBlues);

			fSum[0] += lNrReds * i;
			fSum[1] += lNrGreens * i;
			fSum[2] += lNrBlues * i;
			fTotalPixels[0] += lNrReds;
			fTotalPixels[1] += lNrGreens;
			fTotalPixels[2] += lNrBlues;
		};

		fAverage[0] = fSum[0]/fTotalPixels[0];
		fAverage[1] = fSum[1]/fTotalPixels[1];
		fAverage[2] = fSum[2]/fTotalPixels[2];

		for (i = 0;i<lNrValues;i++)
		{
			LONG			lNrReds;
			LONG			lNrGreens;
			LONG			lNrBlues;

			Histogram.GetValues(i, lNrReds, lNrGreens, lNrBlues);

			fSquareSum[0] += pow(i - fAverage[0], 2)*lNrReds;
			fSquareSum[1] += pow(i - fAverage[1], 2)*lNrGreens;
			fSquareSum[2] += pow(i - fAverage[2], 2)*lNrBlues;
		};

		fStdDev[0] = sqrt(fSquareSum[0] /fTotalPixels[0]);
		fStdDev[1] = sqrt(fSquareSum[1] /fTotalPixels[1]);
		fStdDev[2] = sqrt(fSquareSum[2] /fTotalPixels[2]);


		std::vector<PointF>	vReds;
		std::vector<PointF>	vGreens;
		std::vector<PointF>	vBlues;

		BOOL				bShow = TRUE;

		for (i = 0;i<lNrValues;i++)
		{
			double		fX, 
						fY;
			fX = i;

			fY = exp(-(fX - fAverage[0])*(fX - fAverage[0])/(fStdDev[0]*fStdDev[0]*2))*lWidth/lNrValues;
			fY = lHeight - fY*lHeight;
			vReds.push_back(PointF(fX, fY));

			bShow = bShow && (fX < 1000 && fY < 1000);

			fY = exp(-(fX - fAverage[1])*(fX - fAverage[1])/(fStdDev[1]*fStdDev[1]*2))*lWidth/lNrValues;
			fY = lHeight - fY*lHeight;
			vGreens.push_back(PointF(fX, fY));

			bShow = bShow && (fX < 1000 && fY < 1000);

			fY = exp(-(fX - fAverage[2])*(fX - fAverage[2])/(fStdDev[2]*fStdDev[2]*2))*lWidth/lNrValues;
			fY = lHeight - fY*lHeight;
			vBlues.push_back(PointF(fX, fY));

			bShow = bShow && (fX < 1000 && fY < 1000);
		};

		Pen				RedPen(Color(128, 255, 0, 0));
		Pen				GreenPen(Color(128, 0, 255, 0));
		Pen				BluePen(Color(128, 0, 0, 255));

		RedPen.SetDashStyle(DashStyleDash);
		GreenPen.SetDashStyle(DashStyleDash);
		BluePen.SetDashStyle(DashStyleDash);

		if (bShow)
		{
			pGraphics->DrawLines(&RedPen, &vReds[0], (LONG)vReds.size());
			pGraphics->DrawLines(&GreenPen, &vGreens[0], (LONG)vGreens.size());
			pGraphics->DrawLines(&BluePen, &vBlues[0], (LONG)vBlues.size());
		};
	};
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::ShowHistogram(CWndImage & wndImage, CRGBHistogram & Histogram, BOOL bLog)
{
	CRect				rcClient;
	HBITMAP				hBitmap;
	HBITMAP				hOldBitmap;
	HDC					hScreenDC;
	HDC					hMemDC;
	double				fLog = 0;

	wndImage.GetClientRect(&rcClient);

	hScreenDC = ::GetDC(NULL);
	hMemDC = ::CreateCompatibleDC(hScreenDC);
	hBitmap = ::CreateCompatibleBitmap(hScreenDC, rcClient.Width(), rcClient.Height());

	hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	::FillRect(hMemDC, &rcClient, (HBRUSH)::GetStockObject(WHITE_BRUSH));

	Graphics *			pGraphics = new Graphics(hMemDC);
	if (pGraphics)
	{
		pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
		LONG				lNrValues;
		LONG				lMaxValue = 0;

		lMaxValue = max(lMaxValue, Histogram.GetRedHistogram().GetMaximumNrValues());
		lMaxValue = max(lMaxValue, Histogram.GetGreenHistogram().GetMaximumNrValues());
		lMaxValue = max(lMaxValue, Histogram.GetBlueHistogram().GetMaximumNrValues());

		lNrValues = Histogram.GetRedHistogram().GetNrValues();

		if (lNrValues)
		{
			if (bLog)
			{
				if (lMaxValue)
					fLog = exp(log((double)lMaxValue)/rcClient.Height());
				else
					bLog = FALSE;
			};

			for (LONG i = 0;i<lNrValues;i++)
			{
				LONG			lNrReds;
				LONG			lNrGreens;
				LONG			lNrBlues;

				Histogram.GetValues(i, lNrReds, lNrGreens, lNrBlues);

				if (bLog)
				{
					if (lNrReds)
						lNrReds = log((double)lNrReds)/log(fLog);
					if (lNrGreens)
						lNrGreens = log((double)lNrGreens)/log(fLog);
					if (lNrBlues)
						lNrBlues = log((double)lNrBlues)/log(fLog);
				}
				else
				{
					lNrReds = (double)lNrReds/(double)lMaxValue * rcClient.Height();
					lNrGreens = (double)lNrGreens/(double)lMaxValue * rcClient.Height();
					lNrBlues = (double)lNrBlues/(double)lMaxValue * rcClient.Height();
				};

				DrawHistoBar(pGraphics/*hMemDC*/, lNrReds, lNrGreens, lNrBlues, i, rcClient.Height());
			};

		};

		DrawGaussCurves(pGraphics /*hMemDC*/, Histogram, rcClient.Width(), rcClient.Height());
		DrawBezierCurve(pGraphics /*hMemDC*/, rcClient.Width(), rcClient.Height());
		delete pGraphics;
		::SelectObject(hMemDC, hOldBitmap);
	};

	::DeleteDC(hMemDC);
	::ReleaseDC(NULL, hScreenDC);

	wndImage.SetImg(hBitmap);
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::UpdateHistogramAdjust()
{
	CGradient &			RedGradient = m_tabRGB.m_RedGradient.GetGradient();
	double				fMinRed = m_fGradientOffset+RedGradient.GetPeg(RedGradient.IndexFromId(0)).position * m_fGradientRange,
						fShiftRed = (RedGradient.GetPeg(RedGradient.IndexFromId(1)).position - 0.5) * 2.0,
						fMaxRed = m_fGradientOffset+RedGradient.GetPeg(RedGradient.IndexFromId(2)).position * m_fGradientRange;

	CGradient &			GreenGradient = m_tabRGB.m_GreenGradient.GetGradient();
	double				fMinGreen = m_fGradientOffset+GreenGradient.GetPeg(GreenGradient.IndexFromId(0)).position * m_fGradientRange,
						fShiftGreen = (GreenGradient.GetPeg(GreenGradient.IndexFromId(1)).position - 0.5) * 2.0,
						fMaxGreen = m_fGradientOffset+GreenGradient.GetPeg(GreenGradient.IndexFromId(2)).position * m_fGradientRange;


	CGradient &			BlueGradient = m_tabRGB.m_BlueGradient.GetGradient();
	double				fMinBlue = m_fGradientOffset+BlueGradient.GetPeg(BlueGradient.IndexFromId(0)).position * m_fGradientRange,
						fShiftBlue = (BlueGradient.GetPeg(BlueGradient.IndexFromId(1)).position - 0.5) * 2.0,
						fMaxBlue = m_fGradientOffset+BlueGradient.GetPeg(BlueGradient.IndexFromId(2)).position * m_fGradientRange;


	m_ProcessParams.m_HistoAdjust.GetRedAdjust().SetNewValues(fMinRed, fMaxRed, fShiftRed);
	m_ProcessParams.m_HistoAdjust.GetGreenAdjust().SetNewValues(fMinGreen, fMaxGreen, fShiftGreen);
	m_ProcessParams.m_HistoAdjust.GetBlueAdjust().SetNewValues(fMinBlue, fMaxBlue, fShiftBlue);

	m_ProcessParams.m_HistoAdjust.GetRedAdjust().SetAdjustMethod(m_tabRGB.GetRedAdjustMethod());
	m_ProcessParams.m_HistoAdjust.GetGreenAdjust().SetAdjustMethod(m_tabRGB.GetGreenAdjustMethod());
	m_ProcessParams.m_HistoAdjust.GetBlueAdjust().SetAdjustMethod(m_tabRGB.GetBlueAdjustMethod());


	// Update gradient adjust values
	double				fAbsMin,
						fAbsMax;
	double				fOffset;
	double				fRange;

	fAbsMin = min(fMinRed, min(fMinGreen, fMinBlue));
	fAbsMax = max(fMaxRed, min(fMaxGreen, fMaxBlue));

	fRange  = fAbsMax-fAbsMin;
	if (fRange * 1.10 <= 65535.0)
		fRange *= 1.10;
	
	fOffset = (fAbsMin+fAbsMax - fRange)/2.0;
	if (fOffset < 0)
		fOffset = 0.0;

	m_fGradientOffset = fOffset;
	m_fGradientRange  = fRange;

	RedGradient.SetPeg(RedGradient.IndexFromId(0), (float)((fMinRed - m_fGradientOffset)/m_fGradientRange));
	RedGradient.SetPeg(RedGradient.IndexFromId(1), (float)(fShiftRed/2.0 + 0.5));
	RedGradient.SetPeg(RedGradient.IndexFromId(2), (float)((fMaxRed - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_RedGradient.Invalidate(TRUE);

	GreenGradient.SetPeg(GreenGradient.IndexFromId(0), (float)((fMinGreen - m_fGradientOffset)/m_fGradientRange));
	GreenGradient.SetPeg(GreenGradient.IndexFromId(1), (float)(fShiftGreen/2.0 + 0.5));
	GreenGradient.SetPeg(GreenGradient.IndexFromId(2), (float)((fMaxGreen - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_GreenGradient.Invalidate(TRUE);

	BlueGradient.SetPeg(BlueGradient.IndexFromId(0), (float)((fMinBlue - m_fGradientOffset)/m_fGradientRange));
	BlueGradient.SetPeg(BlueGradient.IndexFromId(1), (float)(fShiftBlue/2.0 + 0.5));
	BlueGradient.SetPeg(BlueGradient.IndexFromId(2), (float)((fMaxBlue - m_fGradientOffset)/m_fGradientRange));
	m_tabRGB.m_BlueGradient.Invalidate(TRUE);
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::ShowOriginalHistogram(BOOL bLog)
{
	// Adjust Histogram
	CRGBHistogram			Histo;
	CRGBHistogramAdjust		HistoAdjust;
	CRect					rcClient;

	m_OriginalHistogram.GetClientRect(&rcClient);

	Histo.SetSize(65535.0, (LONG)rcClient.Width());

	CGradient &			RedGradient = m_tabRGB.m_RedGradient.GetGradient();
	double				fMinRed = m_fGradientOffset+RedGradient.GetPeg(RedGradient.IndexFromId(0)).position * m_fGradientRange,
						fShiftRed = (RedGradient.GetPeg(RedGradient.IndexFromId(1)).position - 0.5)* 2.0,
						fMaxRed = m_fGradientOffset+RedGradient.GetPeg(RedGradient.IndexFromId(2)).position * m_fGradientRange;

	CGradient &			GreenGradient = m_tabRGB.m_GreenGradient.GetGradient();
	double				fMinGreen = m_fGradientOffset+GreenGradient.GetPeg(GreenGradient.IndexFromId(0)).position * m_fGradientRange,
						fShiftGreen = (GreenGradient.GetPeg(GreenGradient.IndexFromId(1)).position - 0.5)* 2.0,
						fMaxGreen = m_fGradientOffset+GreenGradient.GetPeg(GreenGradient.IndexFromId(2)).position * m_fGradientRange;

	CGradient &			BlueGradient = m_tabRGB.m_BlueGradient.GetGradient();
	double				fMinBlue = m_fGradientOffset+BlueGradient.GetPeg(BlueGradient.IndexFromId(0)).position * m_fGradientRange,
						fShiftBlue = (BlueGradient.GetPeg(BlueGradient.IndexFromId(1)).position - 0.5)* 2.0,
						fMaxBlue = m_fGradientOffset+BlueGradient.GetPeg(BlueGradient.IndexFromId(2)).position * m_fGradientRange;
	

	HistoAdjust.GetRedAdjust().SetAdjustMethod(m_tabRGB.GetRedAdjustMethod());
	HistoAdjust.GetRedAdjust().SetNewValues(fMinRed, fMaxRed, fShiftRed);
	HistoAdjust.GetGreenAdjust().SetAdjustMethod(m_tabRGB.GetGreenAdjustMethod());
	HistoAdjust.GetGreenAdjust().SetNewValues(fMinGreen, fMaxGreen, fShiftGreen);
	HistoAdjust.GetBlueAdjust().SetAdjustMethod(m_tabRGB.GetBlueAdjustMethod());
	HistoAdjust.GetBlueAdjust().SetNewValues(fMinBlue, fMaxBlue, fShiftBlue);

	GetDeepStack(this).AdjustOriginalHistogram(Histo, HistoAdjust);

	ShowHistogram(m_OriginalHistogram, Histo, bLog);
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnTimer(UINT_PTR nIDEvent) 
{
	CRect			rcCell;

	if (m_ToProcess.GetNextUnProcessedRect(rcCell))
	{
		HBITMAP			hBitmap = NULL;
		BOOL			bInitialized = TRUE;

		hBitmap = m_Picture.GetBitmap();
		if (!hBitmap)
			bInitialized = FALSE;

		hBitmap = GetDeepStack(this).PartialProcess(rcCell, m_ProcessParams.m_BezierAdjust, m_ProcessParams.m_HistoAdjust);

		if (!bInitialized)
			m_Picture.SetImg(hBitmap, true);

		m_Picture.Invalidate(TRUE);

		if (!m_OriginalHistogram.GetBitmap())
		{
			ShowOriginalHistogram(FALSE);
			ResetSliders();
		};
		const unsigned int nProgress = static_cast<unsigned int>(m_ToProcess.GetPercentageComplete());
		m_ProcessingProgress.SetPos(min(max(0, nProgress), 100));
	};

	CDialog::OnTimer(nIDEvent);
}

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnReset() 
{
	m_bDirty = TRUE;
	ResetSliders();
}

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnNotifyRedChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

void CProcessingDlg::OnNotifyRedPegMove(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

void CProcessingDlg::OnNotifyRedPegMoved(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

void CProcessingDlg::OnNotifyGreenChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

void CProcessingDlg::OnNotifyGreenPegMove(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

void CProcessingDlg::OnNotifyGreenPegMoved(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

void CProcessingDlg::OnNotifyBlueChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

void CProcessingDlg::OnNotifyBluePegMove(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

void CProcessingDlg::OnNotifyBluePegMoved(NMHDR * pNotifyStruct, LRESULT *result)
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::UpdateBezierCurve()
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
};

/* ------------------------------------------------------------------- */

void CProcessingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_bDirty = TRUE;
	ShowOriginalHistogram();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/* ------------------------------------------------------------------- */

BOOL CProcessingDlg::SaveOnClose()
{
	BOOL			bResult = TRUE;

	bResult = AskToSave();
	
	return bResult;
};

/* ------------------------------------------------------------------- */
