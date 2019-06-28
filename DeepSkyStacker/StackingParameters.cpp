// StackingParameters.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "StackingParameters.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "BackgroundOptions.h"

// CStackingParameters dialog

IMPLEMENT_DYNAMIC(CStackingParameters, CChildPropertyPage)

/* ------------------------------------------------------------------- */

CStackingParameters::CStackingParameters()
	: CChildPropertyPage(CStackingParameters::IDD)
{
	m_bFirstActivation = TRUE;
}

/* ------------------------------------------------------------------- */

CStackingParameters::~CStackingParameters()
{
}

/* ------------------------------------------------------------------- */

void CStackingParameters::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVERAGE, m_Average);
	DDX_Control(pDX, IDC_MEDIAN, m_Median);
	DDX_Control(pDX, IDC_MAXIMUM, m_Maximum);
	DDX_Control(pDX, IDC_SIGMACLIPPING, m_SigmaClipping);
	DDX_Control(pDX, IDC_MEDIANSIGMACLIPPING, m_MedianSigmaClipping);
	DDX_Control(pDX, IDC_ENTROPYAVERAGE, m_EntropyAverage);
	DDX_Control(pDX, IDC_AUTOADAPTIVEAVERAGE, m_WeightedAverage);
	DDX_Control(pDX, IDC_KAPPA, m_Kappa);
	DDX_Control(pDX, IDC_ITERATION, m_Iteration);
	DDX_Control(pDX, IDC_STATICKAPPA, m_KappaStatic);
	DDX_Control(pDX, IDC_KAPPAFRAME, m_KappaFrame);
	DDX_Control(pDX, IDC_AUTOADAPTIVEFRAME, m_WeightedFrame);
	DDX_Control(pDX, IDC_STATICITERATION, m_IterationStatic);
	DDX_Control(pDX, IDC_TITLE, m_Title);
	DDX_Control(pDX, IDC_BACKGROUNDCALIBRATION, m_BackgroundCalibration);
	DDX_Control(pDX, IDC_DARKOPTIMIZATION, m_DarkOptimization);
	DDX_Control(pDX, IDC_HOTPIXELS, m_HotPixels);
	DDX_Control(pDX, IDC_BADCOLUMNREMOVAL, m_BadColumns);
	DDX_Control(pDX, IDC_USEDARKFACTOR, m_UseDarkFactor);
	DDX_Control(pDX, IDC_DARKMULTIPLICATIONFACTOR, m_DarkFactor);
	DDX_Control(pDX, IDC_DEBLOOM, m_Debloom);
//	DDX_Control(pDX, IDC_DEBLOOMSETTINGS, m_DebloomSettings);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CStackingParameters, CChildPropertyPage)
	ON_BN_CLICKED(IDC_AVERAGE, &CStackingParameters::OnBnClickedAverage)
	ON_BN_CLICKED(IDC_MEDIAN, &CStackingParameters::OnBnClickedMedian)
	ON_BN_CLICKED(IDC_MAXIMUM, &CStackingParameters::OnBnClickedMaximum)
	ON_BN_CLICKED(IDC_SIGMACLIPPING, &CStackingParameters::OnBnClickedSigmaclipping)
	ON_BN_CLICKED(IDC_MEDIANSIGMACLIPPING, &CStackingParameters::OnBnClickedMedianSigmaclipping)
	ON_EN_CHANGE(IDC_KAPPA, &CStackingParameters::OnEnChangeKappa)
	ON_EN_CHANGE(IDC_ITERATION, &CStackingParameters::OnEnChangeIteration)
	ON_BN_CLICKED(IDC_ENTROPYAVERAGE, &CStackingParameters::OnBnClickedEntropyaverage)
	ON_BN_CLICKED(IDC_AUTOADAPTIVEAVERAGE, &CStackingParameters::OnBnClickedAutoadaptiveaverage)
	ON_BN_CLICKED(IDC_USEDARKFACTOR, &CStackingParameters::OnBnClickedUseDarkFactor)
	ON_BN_CLICKED(IDC_DARKOPTIMIZATION, &CStackingParameters::OnBnClickedDarkOptimization)
	ON_BN_CLICKED(IDC_DEBLOOM, &CStackingParameters::OnBnClickedDebloom)
	ON_BN_CLICKED(IDC_DEBLOOMSETTINGS, &CStackingParameters::OnBnClickedDebloomSettings)
	ON_EN_CHANGE(IDC_DARKMULTIPLICATIONFACTOR, &CStackingParameters::OnEnChangeDarkFactor)
	ON_NOTIFY(NM_LINKCLICK, IDC_BACKGROUNDCALIBRATION, OnBackgroundCalibration)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CStackingParameters::UpdateControls()
{
	CStackSettings *	pDialog = dynamic_cast<CStackSettings *>(GetParent()->GetParent());
	
	if (m_WeightedAverage.GetCheck())
	{
		m_Kappa.EnableWindow(FALSE);
		m_KappaStatic.EnableWindow(FALSE);
		m_Iteration.EnableWindow(TRUE);
		m_IterationStatic.EnableWindow(TRUE);
		m_WeightedFrame.ShowWindow(SW_SHOW);
		m_KappaFrame.ShowWindow(SW_HIDE);
	}
	else if (m_SigmaClipping.GetCheck() || m_MedianSigmaClipping.GetCheck())
	{
		m_Kappa.EnableWindow(TRUE);
		m_KappaStatic.EnableWindow(TRUE);
		m_Iteration.EnableWindow(TRUE);
		m_IterationStatic.EnableWindow(TRUE);
		m_WeightedFrame.ShowWindow(SW_HIDE);
		m_KappaFrame.ShowWindow(SW_SHOW);
	}
	else
	{
		m_Kappa.EnableWindow(FALSE);
		m_KappaStatic.EnableWindow(FALSE);
		m_Iteration.EnableWindow(FALSE);
		m_IterationStatic.EnableWindow(FALSE);
		m_WeightedFrame.ShowWindow(SW_HIDE);
		m_KappaFrame.ShowWindow(SW_HIDE);
	};

	if (pDialog)
		pDialog->UpdateControls();
};

/* ------------------------------------------------------------------- */

BOOL CStackingParameters::OnSetActive()
{
	if (m_bFirstActivation)
	{
		m_Tooltips.Create(this);
		m_Title.SetTextColor(RGB(0, 0, 0));
		m_Title.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
		UpdateControls();
		m_bFirstActivation = FALSE;

		CString			strText;

		strText.LoadString(IDS_TOOLTIP_KAPPASIGMA);
		strText.Replace(_T("\n"), _T("<br>"));
		strText.Replace(_T("[sigma]"), _T("<font face='Symbol'>s</font>"));
		strText.Replace(_T("[kappa]"), _T("<font face='Symbol'>k</font>"));
		m_Tooltips.AddTool(GetDlgItem(IDC_SIGMACLIPPING), strText);
		m_Tooltips.AddTool(GetDlgItem(IDC_KAPPA), strText);
		m_Tooltips.AddTool(GetDlgItem(IDC_STATICKAPPA), strText);

		strText.LoadString(IDS_TOOLTIP_MEDIANKAPPASIGMA);
		strText.Replace(_T("\n"), _T("<br>"));
		strText.Replace(_T("[sigma]"), _T("<font face='Symbol'>s</font>"));
		strText.Replace(_T("[kappa]"), _T("<font face='Symbol'>k</font>"));
		m_Tooltips.AddTool(GetDlgItem(IDC_MEDIANSIGMACLIPPING), strText);

		strText.LoadString(IDS_TOOLTIP_AUTOADAPTIVE);
		strText.Replace(_T("\n"), _T("<br>"));
		strText.Replace(_T("[sigma]"), _T("<font face='Symbol'>s</font>"));
		strText.Replace(_T("[kappa]"), _T("<font face='Symbol'>k</font>"));
		m_Tooltips.AddTool(GetDlgItem(IDC_AUTOADAPTIVEAVERAGE), strText);

		m_Tooltips.Activate(TRUE);

		m_BackgroundCalibration.SetLink(TRUE, TRUE);
		m_BackgroundCalibration.SetTransparent(TRUE);
		m_BackgroundCalibration.SetTextColor(RGB(0, 0, 128));

		UpdateCalibrationMode();
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

void CStackingParameters::SetControls(MULTIBITMAPPROCESSMETHOD Method, double fKappa, LONG lIteration)
{
	switch (Method)
	{
	case MBP_AVERAGE :
		m_Average.SetCheck(TRUE);
		break;
	case MBP_MEDIAN :
		m_Median.SetCheck(TRUE);
		break;
	case MBP_MAXIMUM :
		m_Maximum.SetCheck(TRUE);
		break;
	case MBP_SIGMACLIP :
		m_SigmaClipping.SetCheck(TRUE);
		break;
	case MBP_MEDIANSIGMACLIP :
		m_MedianSigmaClipping.SetCheck(TRUE);
		break;
	case MBP_AUTOADAPTIVE :
		m_WeightedAverage.SetCheck(TRUE);
		break;
	case MBP_ENTROPYAVERAGE :
		m_EntropyAverage.SetCheck(TRUE);
		break;
	};

	CString			strValue;

	strValue.Format(_T("%.2f"), fKappa);
	m_Kappa.SetWindowText(strValue);

	strValue.Format(_T("%ld"), lIteration);
	m_Iteration.SetWindowText(strValue);
};

/* ------------------------------------------------------------------- */

void CStackingParameters::GetControls(MULTIBITMAPPROCESSMETHOD & Method, double & fKappa, LONG & lIteration)
{
	if (m_Average.GetCheck())
		Method = MBP_AVERAGE;
	else if (m_Median.GetCheck())
		Method = MBP_MEDIAN;
	else if (m_Maximum.GetCheck())
		Method = MBP_MAXIMUM;
	else if (m_SigmaClipping.GetCheck())
		Method = MBP_SIGMACLIP;
	else if (m_MedianSigmaClipping.GetCheck())
		Method = MBP_MEDIANSIGMACLIP;
	else if (m_EntropyAverage.GetCheck())
		Method = MBP_ENTROPYAVERAGE;
	else if (m_WeightedAverage.GetCheck())
		Method = MBP_AUTOADAPTIVE;

	CString			strValue;

	m_Kappa.GetWindowText(strValue);
	fKappa = _ttof(strValue);

	m_Iteration.GetWindowText(strValue);
	lIteration = _ttol(strValue);
};

/* ------------------------------------------------------------------- */
// CStackingParameters message handlers

void CStackingParameters::OnBnClickedAverage()
{
	if (m_Average.GetCheck())
	{
		m_Median.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		m_SigmaClipping.SetCheck(FALSE);
		m_MedianSigmaClipping.SetCheck(FALSE);
		m_WeightedAverage.SetCheck(FALSE);
		m_EntropyAverage.SetCheck(FALSE);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedMedian()
{
	if (m_Median.GetCheck())
	{
		m_Average.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		m_SigmaClipping.SetCheck(FALSE);
		m_MedianSigmaClipping.SetCheck(FALSE);
		m_WeightedAverage.SetCheck(FALSE);
		m_EntropyAverage.SetCheck(FALSE);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedMaximum()
{
	if (m_Maximum.GetCheck())
	{
		m_Average.SetCheck(FALSE);
		m_Median.SetCheck(FALSE);
		m_SigmaClipping.SetCheck(FALSE);
		m_MedianSigmaClipping.SetCheck(FALSE);
		m_WeightedAverage.SetCheck(FALSE);
		m_EntropyAverage.SetCheck(FALSE);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedSigmaclipping()
{
	if (m_SigmaClipping.GetCheck())
	{
		m_Average.SetCheck(FALSE);
		m_Median.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		m_MedianSigmaClipping.SetCheck(FALSE);
		m_WeightedAverage.SetCheck(FALSE);
		m_EntropyAverage.SetCheck(FALSE);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedMedianSigmaclipping()
{
	if (m_MedianSigmaClipping.GetCheck())
	{
		m_Average.SetCheck(FALSE);
		m_Median.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		m_SigmaClipping.SetCheck(FALSE);
		m_WeightedAverage.SetCheck(FALSE);
		m_EntropyAverage.SetCheck(FALSE);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnEnChangeKappa()
{
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnEnChangeIteration()
{
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnEnChangeDarkFactor()
{
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedEntropyaverage()
{
	if (m_EntropyAverage.GetCheck())
	{
		m_SigmaClipping.SetCheck(FALSE);
		m_MedianSigmaClipping.SetCheck(FALSE);
		m_Average.SetCheck(FALSE);
		m_Median.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		m_WeightedAverage.SetCheck(FALSE);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedAutoadaptiveaverage()
{
	if (m_WeightedAverage.GetCheck())
	{
		m_SigmaClipping.SetCheck(FALSE);
		m_MedianSigmaClipping.SetCheck(FALSE);
		m_Average.SetCheck(FALSE);
		m_Median.SetCheck(FALSE);
		m_Maximum.SetCheck(FALSE);
		m_EntropyAverage.SetCheck(FALSE);
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedUseDarkFactor()
{
	if (m_UseDarkFactor.GetCheck())
		m_DarkOptimization.SetCheck(FALSE);
};

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedDarkOptimization()
{
	if (m_DarkOptimization.GetCheck())
		m_UseDarkFactor.SetCheck(FALSE);
};

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedDebloom()
{
/*	if (m_Debloom.GetCheck())
		m_DebloomSettings.ShowWindow(SW_SHOW);
	else
		m_DebloomSettings.ShowWindow(SW_HIDE);*/
};

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBnClickedDebloomSettings()
{
};

/* ------------------------------------------------------------------- */

void CStackingParameters::OnBackgroundCalibration( NMHDR * pNotifyStruct, LRESULT * result )
{
	CPoint				pt;
	CMenu				menu;
	CMenu *				popup;
	int					nResult;

	menu.LoadMenu(IDR_CALIBRATION);
	popup = menu.GetSubMenu(0);
	
	CRect				rc;
	CString				strText;

	m_BackgroundCalibration.GetWindowRect(&rc);
	pt.x = rc.left;
	pt.y = rc.bottom;

	if (m_BackgroundCalibrationMode == BCM_RGB)
		popup->CheckMenuItem(ID_CALIBRATIONMENU_RGBBACKGROUNDCALIBRATION, MF_BYCOMMAND | MF_CHECKED);
	else if (m_BackgroundCalibrationMode == BCM_PERCHANNEL)
		popup->CheckMenuItem(ID_CALIBRATIONMENU_PERCHANNELBACKGROUNDCALIBRATION, MF_BYCOMMAND | MF_CHECKED);
	else
		popup->CheckMenuItem(ID_CALIBRATIONMENU_NOBACKGROUNDCALIBRATION, MF_BYCOMMAND | MF_CHECKED);

	nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, NULL);

	switch (nResult)
	{
	case ID_CALIBRATIONMENU_NOBACKGROUNDCALIBRATION :
		m_BackgroundCalibrationMode = BCM_NONE;
		UpdateCalibrationMode();
		break;
	case ID_CALIBRATIONMENU_PERCHANNELBACKGROUNDCALIBRATION :
		m_BackgroundCalibrationMode = BCM_PERCHANNEL;
		UpdateCalibrationMode();
		break;
	case ID_CALIBRATIONMENU_RGBBACKGROUNDCALIBRATION :
		m_BackgroundCalibrationMode = BCM_RGB;
		UpdateCalibrationMode();
		break;
	case ID_CALIBRATIONMENU_OPTIONS :
		{
			CBackgroundOptions		dlg;

			dlg.SetBackgroundCalibrationMode(m_BackgroundCalibrationMode);
			if (dlg.DoModal() == IDOK)
			{
				m_BackgroundCalibrationMode = dlg.GetBackgroundCalibrationMode();
				UpdateCalibrationMode();
			};
		};
		break;
	};
};

/* ------------------------------------------------------------------- */

void CStackingParameters::UpdateCalibrationMode()
{
	CMenu				menu;
	CMenu *				popup;
	CString				strText;

	menu.LoadMenu(IDR_CALIBRATION);
	popup = menu.GetSubMenu(0);

	switch (m_BackgroundCalibrationMode)
	{
	case BCM_NONE :
		popup->GetMenuString(ID_CALIBRATIONMENU_NOBACKGROUNDCALIBRATION, strText, MF_BYCOMMAND);
		break;
	case BCM_PERCHANNEL :
		popup->GetMenuString(ID_CALIBRATIONMENU_PERCHANNELBACKGROUNDCALIBRATION, strText, MF_BYCOMMAND);
		break;
	default :
		popup->GetMenuString(ID_CALIBRATIONMENU_RGBBACKGROUNDCALIBRATION, strText, MF_BYCOMMAND);
		break;
	};

	m_BackgroundCalibration.SetText(strText);
};

/* ------------------------------------------------------------------- */

void CStackingParameters::SetBackgroundCalibrationMode(BACKGROUNDCALIBRATIONMODE Mode)
{
	m_BackgroundCalibrationMode	= Mode;

	if (m_BackgroundCalibration.m_hWnd)
		UpdateCalibrationMode();
};

/* ------------------------------------------------------------------- */
