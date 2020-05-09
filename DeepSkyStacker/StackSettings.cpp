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

CStackSettings::CStackSettings(CWnd* pParent /*=nullptr*/)
	: CDialog(CStackSettings::IDD, pParent)
{
	m_lStartingTab			= 0;
	m_bRegisteringOnly		= false;
	m_bUseCustomRectangle	= false;
	m_bEnableCustomRectangle= false;
	m_bEnableCometStacking	= false;
	m_pStackingTasks		= nullptr;

	m_bEnableDark			= true;
	m_bEnableFlat			= true;
	m_bEnableBias			= true;
	m_bEnableAll			= false;
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

	m_Sheet.EnableStackedTabs( false );
	m_Sheet.Create (this, WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0);

	// Init controls
	CWorkspace			workspace;
	DWORD				dwMethod;
	DWORD				lIteration;
	DWORD				dwBackgroundCalibration = 1;
	DWORD				dwPerChannelBackgroundCalibration = 0;
	bool				fDarkOptimization;
	bool				fDarkFactor;
	QString				strDarkFactor = "1.0";
	bool				fHotPixels;
	bool				fBadColumns = 0;
	DWORD				dwMosaic = 0;
	bool				fCreateIntermediates;
	bool				fSaveCalibrated;
	bool				fSaveDebayered = 0;
	DWORD				dwSaveFormat = 1;
	DWORD				dwAlignment = 0;
	DWORD				dwDrizzle = 1;
	bool				fAlignChannels = 0;
	DWORD				dwCometStackingMode = 0;
	bool				fDebloom = false;
	double				fKappa;

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		dwMosaic = workspace.value("Stacking/Mosaic", (uint)0).toUInt();
		if (dwMosaic==2)
			m_tabResult.SetResultMode(SM_INTERSECTION);
		else if (dwMosaic==1)
			m_tabResult.SetResultMode(SM_MOSAIC);
		else
			m_tabResult.SetResultMode(SM_NORMAL);

		m_tabResult.SetCustom(m_bEnableCustomRectangle, m_bUseCustomRectangle);

		dwDrizzle = workspace.value("Stacking/PixelSizeMultiplier", 1).toUInt();
		m_tabResult.SetDrizzle(dwDrizzle);

		fAlignChannels = workspace.value("Stacking/AlignChannels", false).toUInt();
		m_tabResult.SetAlignChannels(fAlignChannels);

		dwMethod = MBP_AVERAGE;
		dwMethod = workspace.value("Stacking/Light_Method", (uint)MBP_AVERAGE).toUInt();
		lIteration = workspace.value("Stacking/Light_Iteration", (uint)5).toUInt();

		fKappa = workspace.value("Stacking/Light_Kappa", "2.0").toDouble();

		m_tabLightFrames.SetControls((MULTIBITMAPPROCESSMETHOD)dwMethod, fKappa, lIteration);

		if (!m_pStackingTasks ||
			(!m_pStackingTasks->AreBayerImageUsed() &&
			!m_pStackingTasks->AreColorImageUsed()))
		{
			m_tabLightFrames.m_Debloom.ShowWindow(SW_SHOW);
			fDebloom = workspace.value("Stacking/Debloom", false).toBool();
			m_tabLightFrames.m_Debloom.SetCheck(fDebloom);
			//m_tabLightFrames.m_DebloomSettings.ShowWindow(fDeBloom ? SW_SHOW : SW_HIDE);
		};

		BACKGROUNDCALIBRATIONMODE		CalibrationMode;

		CalibrationMode = CAllStackingTasks::GetBackgroundCalibrationMode();

		m_tabLightFrames.m_BackgroundCalibration.ShowWindow(SW_SHOW);
		m_tabLightFrames.SetBackgroundCalibrationMode(CalibrationMode);

		if (m_bEnableCometStacking || m_bEnableAll)
		{
			dwCometStackingMode = workspace.value("Stacking/CometStackingMode", (uint)CSM_STANDARD).toUInt();
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

	dwSaveFormat = workspace.value("Stacking/IntermediateFileFormat", (uint)IFF_TIFF).toUInt();
	m_tabIntermediate.SetFileFormat((INTERMEDIATEFILEFORMAT)dwSaveFormat);

	fCreateIntermediates = workspace.value("Stacking/CreateIntermediates", false).toBool();
	m_tabIntermediate.SetCreateIntermediates(fCreateIntermediates);

	fSaveCalibrated = workspace.value("Stacking/SaveCalibrated", false).toBool();
	m_tabIntermediate.SetSaveCalibrated(fSaveCalibrated);

	fSaveDebayered = workspace.value("Stacking/SaveCalibratedDebayered", false).toBool();
	m_tabIntermediate.SetSaveDebayered(fSaveDebayered);

	if (m_bEnableDark || m_bEnableAll)
	{
		dwMethod = workspace.value("Stacking/Dark_Method", (uint)MBP_MEDIAN).toUInt();
		lIteration = workspace.value("Stacking/Dark_Iteration", (uint)5).toUInt();
		fKappa = workspace.value("Stacking/Dark_Kappa", "2.0").toDouble();

		m_tabDarkFrames.SetControls((MULTIBITMAPPROCESSMETHOD)dwMethod, fKappa, lIteration);
		m_tabDarkFrames.m_EntropyAverage.EnableWindow(false);

		fDarkOptimization = workspace.value("Stacking/DarkOptimization", false).toBool();
		m_tabDarkFrames.m_DarkOptimization.ShowWindow(SW_SHOW);
		m_tabDarkFrames.m_DarkOptimization.SetCheck(fDarkOptimization);

		fDarkFactor = workspace.value("Stacking/UseDarkFactor", false).toBool();
		m_tabDarkFrames.m_UseDarkFactor.ShowWindow(SW_SHOW);
		m_tabDarkFrames.m_UseDarkFactor.SetCheck(fDarkFactor);

		strDarkFactor = workspace.value("Stacking/DarkFactor", "1.0").toString();
		m_tabDarkFrames.m_DarkFactor.ShowWindow(SW_SHOW);
		m_tabDarkFrames.m_DarkFactor.SetWindowText((LPCTSTR)strDarkFactor.utf16());

		fHotPixels = workspace.value("Stacking/HotPixelsDetection", true).toBool();
		m_tabDarkFrames.m_HotPixels.ShowWindow(SW_SHOW);
		m_tabDarkFrames.m_HotPixels.SetCheck(fHotPixels);

		if (!m_pStackingTasks ||
			(!m_pStackingTasks->AreBayerImageUsed() &&
			!m_pStackingTasks->AreColorImageUsed()))
		{
			fBadColumns = workspace.value("Stacking/BadLinesDetection").toBool();
			m_tabDarkFrames.m_BadColumns.ShowWindow(SW_SHOW);
			m_tabDarkFrames.m_BadColumns.SetCheck(fBadColumns);
		};
	};

	if (m_bEnableFlat || m_bEnableAll)
	{
		dwMethod = workspace.value("Stacking/Flat_Method", (uint)MBP_MEDIAN).toUInt();
		lIteration = 5;
		lIteration = workspace.value("Stacking/Flat_Iteration", (uint)5).toUInt();
		fKappa = workspace.value("Stacking/Flat_Kappa", "2.0").toDouble();
		m_tabFlatFrames.SetControls((MULTIBITMAPPROCESSMETHOD)dwMethod, fKappa, lIteration);
		m_tabFlatFrames.m_EntropyAverage.EnableWindow(false);
	};

	if (m_bEnableBias || m_bEnableAll)
	{
		dwMethod = workspace.value("Stacking/Offset_Method", (uint)MBP_MEDIAN).toUInt();
		lIteration = workspace.value("Stacking/Offset_Iteration").toUInt();
		fKappa = workspace.value("Stacking/Offset_Kappa", "2.0").toDouble();
		m_tabOffsetFrames.SetControls((MULTIBITMAPPROCESSMETHOD)dwMethod, fKappa, lIteration);
		m_tabOffsetFrames.m_EntropyAverage.EnableWindow(false);
	};

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		dwAlignment = workspace.value("Stacking/AlignmentTransformation", 0).toUInt();
		m_tabAlignment.SetAlignment(dwAlignment);
	};

	m_Sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_Sheet.ModifyStyle( 0, WS_TABSTOP );

	m_Sheet.MoveWindow(&rcTabs, true);
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

	QString				strTempFolder;

	CAllStackingTasks::GetTemporaryFilesFolder(strTempFolder);
	m_TempFolder.SetWindowText(CString((LPCTSTR)strTempFolder.utf16()));

	UpdateControls();

	if (CMultitask::GetReducedThreadsPriority())
		m_ReducePriority.SetCheck(true);

	if (CMultitask::GetNrProcessors(true)>1)
		m_UseAllProcessors.SetCheck(CMultitask::GetNrProcessors()>1);
	else
		m_UseAllProcessors.ShowWindow(SW_HIDE);

	return true;
};

/* ------------------------------------------------------------------- */

bool CStackSettings::CheckTabControls(CStackingParameters * pTab)
{
	bool			bResult = false;

	if (nullptr != pTab->m_SigmaClipping.GetSafeHwnd() &&
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
			bResult = true;
	}
	else
		bResult = true;

	if (nullptr != pTab->m_DarkFactor.GetSafeHwnd() &&
		pTab->m_DarkFactor.IsWindowVisible())
	{
		CString		strFactor;
		double		fFactor;

		pTab->m_DarkFactor.GetWindowText(strFactor);
		fFactor = _ttof(strFactor);

		if (fFactor<=0)
			bResult = false;
	};

	return bResult;
}

/* ------------------------------------------------------------------- */

void CStackSettings::UpdateControls()
{
	bool			bOk;

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		// Here check that the comet options are compatible with the stacking
		// options
		if (m_bEnableCometStacking)
		{
			bool			bEnable;

			bEnable = !m_tabComet.m_AdvancedStacking.GetCheck();
			m_tabLightFrames.m_Maximum.EnableWindow(bEnable);
			m_tabLightFrames.m_EntropyAverage.EnableWindow(bEnable);

			if (!bEnable &&
				(m_tabLightFrames.m_Maximum.GetCheck() ||
				 m_tabLightFrames.m_EntropyAverage.GetCheck()))
			{
				// Revert to Median stacking
				m_tabLightFrames.m_Maximum.SetCheck(false);
				m_tabLightFrames.m_EntropyAverage.SetCheck(false);
				m_tabLightFrames.m_Median.SetCheck(true);
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
		bOk = true;
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
	QString						strKappa;
	double						fKappa;
	STACKINGMODE				ResultMode;
	DWORD						dwDrizzle;
	bool						fAlignChannels;
	bool						fCreateIntermediates;
	bool						fSaveCalibrated;
	bool						fSaveDebayered;
	DWORD						dwSaveFormat;

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		m_bUseCustomRectangle = m_tabResult.GetCustom();

		ResultMode = m_tabResult.GetResultMode();
		if (ResultMode == SM_MOSAIC)
			workspace.setValue("Stacking/Mosaic", (uint)1);
		else if (ResultMode == SM_INTERSECTION)
			workspace.setValue("Stacking/Mosaic", (uint)2);
		else
			workspace.setValue("Stacking/Mosaic", (uint)0);

		dwDrizzle = m_tabResult.GetDrizzle();
		workspace.setValue("Stacking/PixelSizeMultiplier", (uint)dwDrizzle);

		fAlignChannels = m_tabResult.GetAlignChannels();
		workspace.setValue("Stacking/AlignChannels", fAlignChannels);

		m_tabLightFrames.GetControls(dwMethod, fKappa, lIteration);
		strKappa = QString::asprintf("%.4f", fKappa);
		workspace.setValue("Stacking/Light_Method", (uint)dwMethod);
		workspace.setValue("Stacking/Light_Iteration", (uint)lIteration);
		workspace.setValue("Stacking/Light_Kappa", strKappa);
	};

	dwSaveFormat= m_tabIntermediate.GetFileFormat();
	workspace.setValue("Stacking/IntermediateFileFormat", (uint)dwSaveFormat);

	fCreateIntermediates = m_tabIntermediate.GetCreateIntermediates();
	workspace.setValue("Stacking/CreateIntermediates", fCreateIntermediates);

	fSaveCalibrated = m_tabIntermediate.GetSaveCalibrated();
	workspace.setValue("Stacking/SaveCalibrated", fSaveCalibrated);

	fSaveDebayered= m_tabIntermediate.GetSaveDebayered();
	workspace.setValue("Stacking/SaveCalibratedDebayered", fSaveDebayered);

	if (m_bEnableDark || m_bEnableAll)
	{
		m_tabDarkFrames.GetControls(dwMethod, fKappa, lIteration);
		strKappa = QString::asprintf("%.4f", fKappa);
		workspace.setValue("Stacking/Dark_Method", (uint)dwMethod);
		workspace.setValue("Stacking/Dark_Iteration", (uint)lIteration);
		workspace.setValue("Stacking/Dark_Kappa", strKappa);

		bool						fDarkOptimization;

		fDarkOptimization = m_tabDarkFrames.m_DarkOptimization.GetCheck();
		workspace.setValue("Stacking/DarkOptimization", fDarkOptimization);

		bool						fDarkFactor;
		CString						strDarkFactor;

		fDarkFactor = (bool)m_tabDarkFrames.m_UseDarkFactor.GetCheck();
		workspace.setValue("Stacking/UseDarkFactor", fDarkFactor);

		m_tabDarkFrames.m_DarkFactor.GetWindowText(strDarkFactor);
		workspace.setValue("Stacking/DarkFactor", QString((QChar *)strDarkFactor.GetBuffer()));

		bool						fHotPixels;

		fHotPixels = m_tabDarkFrames.m_HotPixels.GetCheck();
		workspace.setValue("Stacking/HotPixelsDetection", fHotPixels);

		bool						fBadColumns;

		fBadColumns = m_tabDarkFrames.m_BadColumns.GetCheck();
		workspace.setValue("Stacking/BadLinesDetection", fBadColumns);
	};

	if (m_bEnableFlat || m_bEnableAll)
	{
		m_tabFlatFrames.GetControls(dwMethod, fKappa, lIteration);
		strKappa = QString::asprintf("%.4f", fKappa);
		workspace.setValue("Stacking/Flat_Method", (uint)dwMethod);
		workspace.setValue("Stacking/Flat_Iteration", (uint)lIteration);
		workspace.setValue("Stacking/Flat_Kappa", strKappa);
	};

	if (m_bEnableBias || m_bEnableAll)
	{
		m_tabOffsetFrames.GetControls(dwMethod, fKappa, lIteration);
		strKappa = QString::asprintf("%.4f", fKappa);
		workspace.setValue("Stacking/Offset_Method", (uint)dwMethod);
		workspace.setValue("Stacking/Offset_Iteration", (uint)lIteration);
		workspace.setValue("Stacking/Offset_Kappa", strKappa);
	};

	CString						strFolder;

	m_TempFolder.GetWindowText(strFolder);
	CAllStackingTasks::SetTemporaryFilesFolder(strFolder);

	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		bool						fDeBloom;
		bool						fBackgroundCalibration;
		bool						fPerChannelBackgroundCalibration;

		fDeBloom = m_tabLightFrames.m_Debloom.GetCheck();
		workspace.setValue("Stacking/Debloom", fDeBloom);

		fBackgroundCalibration = (m_tabLightFrames.m_BackgroundCalibrationMode == BCM_RGB);
		fPerChannelBackgroundCalibration = (m_tabLightFrames.m_BackgroundCalibrationMode == BCM_PERCHANNEL);

		workspace.setValue("Stacking/BackgroundCalibration", fBackgroundCalibration);
		workspace.setValue("Stacking/PerChannelBackgroundCalibration", fPerChannelBackgroundCalibration);
	};


	if (m_bEnableAll || !m_bRegisteringOnly)
	{
		DWORD						dwAlignment;
		dwAlignment = m_tabAlignment.GetAlignment();

		workspace.setValue("Stacking/AlignmentTransformation", (uint)dwAlignment);

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
		workspace.setValue("Stacking/CometStackingMode", (uint)CometStackingMode);
	};

	if (CMultitask::GetNrProcessors(true)>1)
		CMultitask::SetUseAllProcessors(m_UseAllProcessors.GetCheck());
	CMultitask::SetReducedThreadsPriority(m_ReducePriority.GetCheck());

	workspace.saveSettings();

	OnOK();
}

/* ------------------------------------------------------------------- */

void CStackSettings::OnBnClickedChangetempfolder()
{
	CString					strFolder;
	CString					strTitle;

	m_TempFolder.GetWindowText(strFolder);

	CFolderDlg				dlg(false, strFolder, this);

	strTitle.LoadString(IDS_RECAP_SELECTTEMPFOLDER);
	dlg.SetTitle(strTitle);

	if (dlg.DoModal() == IDOK)
	{
		strFolder = dlg.GetFolderName();
		m_TempFolder.SetWindowText(strFolder);
	};
}

/* ------------------------------------------------------------------- */

