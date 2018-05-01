// StackSettings.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "StackSettings.h"
#include "Registry.h"
#include "StackingTasks.h"
#include <shlobj.h>
#include <FolderDlg.h>
#include "Multitask.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "Workspace.h"

// CStackSettings dialog

IMPLEMENT_DYNAMIC(CStackSettings, CDialog)

/* ------------------------------------------------------------------- */

CStackSettings::CStackSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CStackSettings::IDD, pParent)
{
	m_lStartingTab			= 0;
	m_bRegisteringOnly		= FALSE;
	m_bUseCustomRectangle	= FALSE;
	m_bEnableCustomRectangle= FALSE;
	m_bEnableCometStacking	= FALSE;
	m_pStackingTasks		= NULL;

	m_bEnableDark			= TRUE;
	m_bEnableFlat			= TRUE;
	m_bEnableBias			= TRUE;
	m_bEnableAll			= FALSE;
}

/* ------------------------------------------------------------------- */

CStackSettings::~CStackSettings()
{
}

/* ------------------------------------------------------------------- */

void CStackSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABRECT, m_TabRect);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_USEALLPROCESSORS, m_UseAllProcessors);
	DDX_Control(pDX, IDC_REDUCEWORKERTHREADS, m_ReducePriority);
	DDX_Control(pDX, IDC_TEMPFOLDER, m_TempFolder);
}

/* ------------------------------------------------------------------- */

BOOL CStackSettings::OnInitDialog()
{
	CRect				rcTabs;

	CDialog::OnInitDialog();

	m_TabRect.GetWindowRect(&rcTabs);
	ScreenToClient(&rcTabs);
	rcTabs.left = 0;
	rcTabs.top -= 11;

	m_tabResult.m_psp.dwFlags |= /*PSP_USETITLE | */PSP_PREMATURE;
	//m_tabResult.m_psp.pszTitle = MAKEINTRESOURCE(IDS_TYPE_LIGHT);

	m_tabLightFrames.m_psp.dwFlags |= PSP_USETITLE | PSP_PREMATURE;
	m_tabLightFrames.m_psp.pszTitle = MAKEINTRESOURCE(IDS_TYPE_LIGHT);

	m_tabDarkFrames.m_psp.dwFlags |= PSP_USETITLE | PSP_PREMATURE;
	m_tabDarkFrames.m_psp.pszTitle = MAKEINTRESOURCE(IDS_TYPE_DARK);


	m_tabFlatFrames.m_psp.dwFlags |= PSP_USETITLE | PSP_PREMATURE;
	m_tabFlatFrames.m_psp.pszTitle = MAKEINTRESOURCE(IDS_TYPE_FLAT);

	m_tabOffsetFrames.m_psp.dwFlags |= PSP_USETITLE | PSP_PREMATURE;
	m_tabOffsetFrames.m_psp.pszTitle = MAKEINTRESOURCE(IDS_TYPE_OFFSET);

	m_tabAlignment.m_psp.dwFlags |= PSP_PREMATURE;
	m_tabIntermediate.m_psp.dwFlags |= PSP_PREMATURE;
	m_tabComet.m_psp.dwFlags |= PSP_PREMATURE;
	m_tabPostCalibration.m_psp.dwFlags |= PSP_PREMATURE;
	m_tabOutput.m_psp.dwFlags |= PSP_PREMATURE;

	m_tabIntermediate.SetRegisteringOnly(m_bRegisteringOnly);
	if (m_bRegisteringOnly)
	{
		if (m_bEnableDark || m_bEnableAll)
			m_Sheet.AddPage(&m_tabDarkFrames);
		if (m_bEnableFlat  || m_bEnableAll)
			m_Sheet.AddPage(&m_tabFlatFrames);
		if (m_bEnableBias || m_bEnableAll)
			m_Sheet.AddPage(&m_tabOffsetFrames);
		m_Sheet.AddPage(&m_tabIntermediate);
	}
	else
	{
		m_Sheet.AddPage(&m_tabResult);
		if (m_bEnableCometStacking || m_bEnableAll)
			m_Sheet.AddPage(&m_tabComet);
		m_Sheet.AddPage(&m_tabLightFrames);
		if (m_bEnableDark || m_bEnableAll)
			m_Sheet.AddPage(&m_tabDarkFrames);
		if (m_bEnableFlat || m_bEnableAll)
			m_Sheet.AddPage(&m_tabFlatFrames);
		if (m_bEnableBias || m_bEnableAll)
			m_Sheet.AddPage(&m_tabOffsetFrames);
		m_Sheet.AddPage(&m_tabAlignment);
		m_Sheet.AddPage(&m_tabIntermediate);
		m_Sheet.AddPage(&m_tabPostCalibration);
		m_Sheet.AddPage(&m_tabOutput);
	};

	m_Sheet.EnableStackedTabs( FALSE );
	m_Sheet.Create (this, WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0);

	// Init controls
	CWorkspace			workspace;
	DWORD				dwMethod;
	DWORD				lIteration;
	DWORD				dwBackgroundCalibration = 1;
	DWORD				dwPerChannelBackgroundCalibration = 0;
	DWORD				dwDarkOptimization = 0;
	DWORD				dwDarkFactor = 0;
	CString				strDarkFactor = "1.0";
	DWORD				dwHotPixels = 1;
	DWORD				dwBadColumns = 0;
	DWORD				dwMosaic = 0;
	DWORD				dwCreateIntermediates = 0;
	DWORD				dwSaveCalibrated = 0;
	DWORD				dwSaveDebayered = 0;
	DWORD				dwSaveFormat = 1;
	CString				strKappa;
	DWORD				dwAlignment = 0;
	DWORD				dwDrizzle = 1;
	DWORD				dwAlignChannels = 0;
	DWORD				dwCometStackingMode = 0;
	DWORD				dwDebloom = 0;
	double				fKappa;

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Mosaic"), dwMosaic);
		if (dwMosaic==2)
			m_tabResult.SetResultMode(SM_INTERSECTION);
		else if (dwMosaic==1)
			m_tabResult.SetResultMode(SM_MOSAIC);
		else 
			m_tabResult.SetResultMode(SM_NORMAL);

		m_tabResult.SetCustom(m_bEnableCustomRectangle, m_bUseCustomRectangle);

		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PixelSizeMultiplier"), dwDrizzle);
		m_tabResult.SetDrizzle(dwDrizzle);

		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignChannels"), dwAlignChannels);
		m_tabResult.SetAlignChannels(dwAlignChannels);

		dwMethod = MBP_AVERAGE;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), dwMethod);
		lIteration = 5;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Iteration"), lIteration);
		strKappa ="2.0";
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Kappa"), strKappa);
		fKappa = _ttof(strKappa);
		m_tabLightFrames.SetControls((MULTIBITMAPPROCESSMETHOD)dwMethod, fKappa, lIteration);

		if (!m_pStackingTasks ||
			(!m_pStackingTasks->AreBayerImageUsed() &&
			!m_pStackingTasks->AreColorImageUsed()))
		{
			m_tabLightFrames.m_Debloom.ShowWindow(SW_SHOW);
			workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Debloom"), dwDebloom);
			m_tabLightFrames.m_Debloom.SetCheck(dwDebloom);
			//m_tabLightFrames.m_DebloomSettings.ShowWindow(dwDebloom ? SW_SHOW : SW_HIDE);
		};

		BACKGROUNDCALIBRATIONMODE		CalibrationMode;

		CalibrationMode = CAllStackingTasks::GetBackgroundCalibrationMode();

		m_tabLightFrames.m_BackgroundCalibration.ShowWindow(SW_SHOW);
		m_tabLightFrames.SetBackgroundCalibrationMode(CalibrationMode);

		if (m_bEnableCometStacking || m_bEnableAll)
		{
			workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CometStackingMode"), dwCometStackingMode);
			m_tabComet.SetCometStackingMode((COMETSTACKINGMODE)dwCometStackingMode);
		};

		CPostCalibrationSettings		PCSettings;

		CAllStackingTasks::GetPostCalibrationSettings(PCSettings);
		m_tabPostCalibration.SetPostCalibration(PCSettings);
		m_tabPostCalibration.SetStackingTasks(m_pStackingTasks);

		COutputSettings					OutputSettings;

		CAllStackingTasks::GetOutputSettings(OutputSettings);
		m_tabOutput.SetOutputSettings(OutputSettings);
	};

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("IntermediateFileFormat"), dwSaveFormat);
	m_tabIntermediate.SetFileFormat((INTERMEDIATEFILEFORMAT)dwSaveFormat);

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CreateIntermediates"), dwCreateIntermediates);
	m_tabIntermediate.SetCreateIntermediates(dwCreateIntermediates);

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCalibrated"), dwSaveCalibrated);
	m_tabIntermediate.SetSaveCalibrated(dwSaveCalibrated);

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCalibratedDebayered"), dwSaveDebayered);
	m_tabIntermediate.SetSaveDebayered(dwSaveDebayered);

	if (m_bEnableDark || m_bEnableAll)
	{
		dwMethod = MBP_MEDIAN;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Method"), dwMethod);
		lIteration = 5;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Iteration"), lIteration);
		strKappa ="2.0";
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Kappa"), strKappa);
		fKappa = _ttof(strKappa);
		m_tabDarkFrames.SetControls((MULTIBITMAPPROCESSMETHOD)dwMethod, fKappa, lIteration);
		m_tabDarkFrames.m_EntropyAverage.EnableWindow(FALSE);

		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("DarkOptimization"), dwDarkOptimization);
		m_tabDarkFrames.m_DarkOptimization.ShowWindow(SW_SHOW);
		m_tabDarkFrames.m_DarkOptimization.SetCheck(dwDarkOptimization);
		
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("UseDarkFactor"), dwDarkFactor);
		m_tabDarkFrames.m_UseDarkFactor.ShowWindow(SW_SHOW);
		m_tabDarkFrames.m_UseDarkFactor.SetCheck(dwDarkFactor);

		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("DarkFactor"), strDarkFactor);
		m_tabDarkFrames.m_DarkFactor.ShowWindow(SW_SHOW);
		m_tabDarkFrames.m_DarkFactor.SetWindowText(strDarkFactor);

		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("HotPixelsDetection"), dwHotPixels);
		m_tabDarkFrames.m_HotPixels.ShowWindow(SW_SHOW);
		m_tabDarkFrames.m_HotPixels.SetCheck(dwHotPixels);

		if (!m_pStackingTasks ||
			(!m_pStackingTasks->AreBayerImageUsed() &&
			!m_pStackingTasks->AreColorImageUsed()))
		{
			workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BadLinesDetection"), dwBadColumns);
			m_tabDarkFrames.m_BadColumns.ShowWindow(SW_SHOW);
			m_tabDarkFrames.m_BadColumns.SetCheck(dwBadColumns);
		};
	};

	if (m_bEnableFlat || m_bEnableAll)
	{
		dwMethod = MBP_MEDIAN;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Method"), dwMethod);
		lIteration = 5;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Iteration"), lIteration);
		strKappa ="2.0";
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Kappa"), strKappa);
		fKappa = _ttof(strKappa);
		m_tabFlatFrames.SetControls((MULTIBITMAPPROCESSMETHOD)dwMethod, fKappa, lIteration);
		m_tabFlatFrames.m_EntropyAverage.EnableWindow(FALSE);
	};

	if (m_bEnableBias || m_bEnableAll)
	{
		dwMethod = MBP_MEDIAN;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Method"), dwMethod);
		lIteration = 5;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Iteration"), lIteration);
		strKappa ="2.0";
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Kappa"), strKappa);
		fKappa = _ttof(strKappa);
		m_tabOffsetFrames.SetControls((MULTIBITMAPPROCESSMETHOD)dwMethod, fKappa, lIteration);
		m_tabOffsetFrames.m_EntropyAverage.EnableWindow(FALSE);
	};

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		dwAlignment = 0;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignmentTransformation"), dwAlignment);
		m_tabAlignment.SetAlignment(dwAlignment);
	};

	m_Sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_Sheet.ModifyStyle( 0, WS_TABSTOP );

	m_Sheet.MoveWindow(&rcTabs, TRUE);
	m_Sheet.ShowWindow(SW_SHOWNA);

	if (m_lStartingTab)
	{
		if (m_lStartingTab == SSTAB_LIGHT)
			m_Sheet.SetActivePage(&m_tabLightFrames);
		else if ((m_lStartingTab == SSTAB_DARK) && m_bEnableDark)
			m_Sheet.SetActivePage(&m_tabDarkFrames);
		else if ((m_lStartingTab == SSTAB_FLAT) && m_bEnableFlat)
			m_Sheet.SetActivePage(&m_tabFlatFrames);
		else if ((m_lStartingTab == SSTAB_OFFSET) && m_bEnableBias)
			m_Sheet.SetActivePage(&m_tabOffsetFrames);
		else if (m_lStartingTab == SSTAB_ALIGNMENT)
			m_Sheet.SetActivePage(&m_tabAlignment);
		else if (m_lStartingTab == SSTAB_COMET)
			m_Sheet.SetActivePage(&m_tabComet);
		else if (m_lStartingTab == SSTAB_POSTCALIBRATION)
			m_Sheet.SetActivePage(&m_tabPostCalibration);
		else if (m_lStartingTab == SSTAB_OUTPUT)
			m_Sheet.SetActivePage(&m_tabOutput);
	};

	CString				strTempFolder;

	CAllStackingTasks::GetTemporaryFilesFolder(strTempFolder);
	m_TempFolder.SetWindowText(strTempFolder);

	UpdateControls();

	if (CMultitask::GetReducedThreadsPriority())
		m_ReducePriority.SetCheck(TRUE);

	if (CMultitask::GetNrProcessors(TRUE)>1)
		m_UseAllProcessors.SetCheck(CMultitask::GetNrProcessors()>1);
	else
		m_UseAllProcessors.ShowWindow(SW_HIDE);

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL CStackSettings::CheckTabControls(CStackingParameters * pTab)
{
	BOOL			bResult = FALSE;

	if (NULL != pTab->m_SigmaClipping.GetSafeHwnd() &&
		pTab->m_SigmaClipping.GetCheck())
	{
		CString		strKappa;
		CString		strIteration;
		double		fKappa;
		LONG		lIteration;

		pTab->m_Kappa.GetWindowText(strKappa);
		pTab->m_Iteration.GetWindowText(strIteration);

		fKappa = _ttof(strKappa);
		lIteration = _ttol(strIteration);

		if ((fKappa > 0) && (fKappa < 5) && (lIteration > 0) && (lIteration < 50))
			bResult = TRUE;
	}
	else
		bResult = TRUE;

	if (NULL != pTab->m_DarkFactor.GetSafeHwnd() && 
		pTab->m_DarkFactor.IsWindowVisible())
	{
		CString		strFactor;
		double		fFactor;

		pTab->m_DarkFactor.GetWindowText(strFactor);
		fFactor = _ttof(strFactor);

		if (fFactor<=0)
			bResult = FALSE;
	};

	return bResult;
}

/* ------------------------------------------------------------------- */

void CStackSettings::UpdateControls()
{
	BOOL			bOk;

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		// Here check that the comet options are compatible with the stacking
		// options
		if (m_bEnableCometStacking)
		{
			BOOL			bEnable;
			
			bEnable = !m_tabComet.m_AdvancedStacking.GetCheck();
			m_tabLightFrames.m_Maximum.EnableWindow(bEnable);
			m_tabLightFrames.m_EntropyAverage.EnableWindow(bEnable);

			if (!bEnable && 
				(m_tabLightFrames.m_Maximum.GetCheck() || 
				 m_tabLightFrames.m_EntropyAverage.GetCheck()))
			{
				// Revert to Median stacking
				m_tabLightFrames.m_Maximum.SetCheck(FALSE);
				m_tabLightFrames.m_EntropyAverage.SetCheck(FALSE);
				m_tabLightFrames.m_Median.SetCheck(TRUE);
			};
		};

		bOk =	CheckTabControls(&m_tabLightFrames);
		if (m_bEnableDark)
			bOk = bOk && CheckTabControls(&m_tabDarkFrames);
		if (m_bEnableFlat)
			bOk = bOk && CheckTabControls(&m_tabFlatFrames);
		if (m_bEnableBias)
			bOk = bOk && CheckTabControls(&m_tabOffsetFrames);
	}
	else
	{
		bOk = TRUE;
		if (m_bEnableDark)
			bOk = bOk && CheckTabControls(&m_tabDarkFrames);
		if (m_bEnableFlat)
			bOk = bOk && CheckTabControls(&m_tabFlatFrames);
		if (m_bEnableBias)
			bOk = bOk && CheckTabControls(&m_tabOffsetFrames);
	};

	m_OK.EnableWindow(bOk);
};

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CStackSettings, CDialog)
	ON_BN_CLICKED(IDOK, &CStackSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHANGETEMPFOLDER, &CStackSettings::OnBnClickedChangetempfolder)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
// CStackSettings message handlers

void CStackSettings::OnBnClickedOk()
{
	// Save settings
	CWorkspace					workspace;
	MULTIBITMAPPROCESSMETHOD	dwMethod;
	LONG						lIteration;
	CString						strKappa;
	double						fKappa;
	STACKINGMODE				ResultMode;
	DWORD						dwDrizzle;
	DWORD						dwAlignChannels;
	DWORD						dwCreateIntermediates;
	DWORD						dwSaveCalibrated;
	DWORD						dwSaveDebayered;
	DWORD						dwSaveFormat;

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		m_bUseCustomRectangle = m_tabResult.GetCustom();

		ResultMode = m_tabResult.GetResultMode();
		if (ResultMode == SM_MOSAIC)
			workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Mosaic"), (DWORD)1);
		else if (ResultMode == SM_INTERSECTION)
			workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Mosaic"), (DWORD)2);
		else
			workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Mosaic"), (DWORD)0);

		dwDrizzle = m_tabResult.GetDrizzle();
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PixelSizeMultiplier"), (DWORD)dwDrizzle);

		dwAlignChannels = m_tabResult.GetAlignChannels();
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignChannels"), (DWORD)dwAlignChannels);

		m_tabLightFrames.GetControls(dwMethod, fKappa, lIteration);
		strKappa.Format(_T("%.4f"), fKappa);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)dwMethod);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Iteration"), (DWORD)lIteration);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Kappa"), strKappa);
	};

	dwSaveFormat= m_tabIntermediate.GetFileFormat();
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("IntermediateFileFormat"), (DWORD)dwSaveFormat);

	dwCreateIntermediates = m_tabIntermediate.GetCreateIntermediates();
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CreateIntermediates"), (DWORD)dwCreateIntermediates);

	dwSaveCalibrated = m_tabIntermediate.GetSaveCalibrated();
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCalibrated"), (DWORD)dwSaveCalibrated);

	dwSaveDebayered= m_tabIntermediate.GetSaveDebayered();
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCalibratedDebayered"), (DWORD)dwSaveDebayered);

	if (m_bEnableDark || m_bEnableAll)
	{
		m_tabDarkFrames.GetControls(dwMethod, fKappa, lIteration);
		strKappa.Format(_T("%.4f"), fKappa);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Method"), (DWORD)dwMethod);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Iteration"), (DWORD)lIteration);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Kappa"), strKappa);

		DWORD						dwDarkOptimization;

		dwDarkOptimization = m_tabDarkFrames.m_DarkOptimization.GetCheck();
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("DarkOptimization"), dwDarkOptimization);

		DWORD						dwDarkFactor;
		CString						strDarkFactor;

		dwDarkFactor = m_tabDarkFrames.m_UseDarkFactor.GetCheck();
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("UseDarkFactor"), dwDarkFactor);

		m_tabDarkFrames.m_DarkFactor.GetWindowText(strDarkFactor);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("DarkFactor"), strDarkFactor);

		DWORD						dwHotPixels;

		dwHotPixels = m_tabDarkFrames.m_HotPixels.GetCheck();
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("HotPixelsDetection"), dwHotPixels);

		DWORD						dwBadColumns;

		dwBadColumns = m_tabDarkFrames.m_BadColumns.GetCheck();
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BadLinesDetection"), dwBadColumns);
	};

	if (m_bEnableFlat || m_bEnableAll)
	{
		m_tabFlatFrames.GetControls(dwMethod, fKappa, lIteration);
		strKappa.Format(_T("%.4f"), fKappa);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Method"), (DWORD)dwMethod);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Iteration"), (DWORD)lIteration);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Kappa"), strKappa);
	};

	if (m_bEnableBias || m_bEnableAll)
	{
		m_tabOffsetFrames.GetControls(dwMethod, fKappa, lIteration);
		strKappa.Format(_T("%.4f"), fKappa);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Method"), (DWORD)dwMethod);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Iteration"), (DWORD)lIteration);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Kappa"), strKappa);
	};

	CString						strFolder;

	m_TempFolder.GetWindowText(strFolder);
	CAllStackingTasks::SetTemporaryFilesFolder(strFolder);

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		DWORD						dwDebloom;
		DWORD						dwBackgroundCalibration;
		DWORD						dwPerChannelBackgroundCalibration;

		dwDebloom = m_tabLightFrames.m_Debloom.GetCheck();
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Debloom"), dwDebloom);

		dwBackgroundCalibration = (m_tabLightFrames.m_BackgroundCalibrationMode == BCM_RGB);
		dwPerChannelBackgroundCalibration = (m_tabLightFrames.m_BackgroundCalibrationMode == BCM_PERCHANNEL);

		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibration"), dwBackgroundCalibration);
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PerChannelBackgroundCalibration"), dwPerChannelBackgroundCalibration);
	};


	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		DWORD						dwAlignment;
		dwAlignment = m_tabAlignment.GetAlignment();

		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignmentTransformation"), dwAlignment);

		CPostCalibrationSettings	PCSettings;

		m_tabPostCalibration.GetPostCalibration(PCSettings);
		CAllStackingTasks::SetPostCalibrationSettings(PCSettings);

		COutputSettings				OutputSettings;

		m_tabOutput.GetOutputSettings(OutputSettings);
		CAllStackingTasks::SetOutputSettings(OutputSettings);
	};

	if (m_bEnableCometStacking || m_bEnableAll)
	{
		COMETSTACKINGMODE			CometStackingMode;

		CometStackingMode = m_tabComet.GetCometStackingMode();
		workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CometStackingMode"), (DWORD)CometStackingMode);
	};

	if (CMultitask::GetNrProcessors(TRUE)>1)
		CMultitask::SetUseAllProcessors(m_UseAllProcessors.GetCheck());
	CMultitask::SetReducedThreadsPriority(m_ReducePriority.GetCheck());

	workspace.SaveToRegistry();

	OnOK();
}

/* ------------------------------------------------------------------- */

void CStackSettings::OnBnClickedChangetempfolder()
{
	CString					strFolder;
	CString					strTitle;
	
	m_TempFolder.GetWindowText(strFolder);

	CFolderDlg				dlg(FALSE, strFolder, this);

	strTitle.LoadString(IDS_RECAP_SELECTTEMPFOLDER);
	dlg.SetTitle(strTitle);

	if (dlg.DoModal() == IDOK)
	{
		strFolder = dlg.GetFolderName();
		m_TempFolder.SetWindowText(strFolder);
	};
}

/* ------------------------------------------------------------------- */

