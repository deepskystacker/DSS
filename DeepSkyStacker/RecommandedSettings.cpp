// StackRecap.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "RecommandedSettings.h"
#include "Multitask.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "DeepStackerDlg.h"

// CRecommendedSettings dialog

/* ------------------------------------------------------------------- */

IMPLEMENT_DYNAMIC(CRecommendedSettings, CDialog)

CRecommendedSettings::CRecommendedSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CRecommendedSettings::IDD, pParent)
{
	m_pStackingTasks = NULL;
}

/* ------------------------------------------------------------------- */

CRecommendedSettings::~CRecommendedSettings()
{
}

/* ------------------------------------------------------------------- */

void CRecommendedSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECOMMANDEDSETTINGSHTML, m_RecommendedSettingsHTML);
	DDX_Control(pDX, IDC_SHOWALL, m_ShowAll);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CRecommendedSettings, CDialog)
	ON_BN_CLICKED(IDC_SHOWALL, &CRecommendedSettings::OnBnClickedShowAll)
	ON_NOTIFY( QHTMN_HYPERLINK, IDC_RECOMMANDEDSETTINGSHTML, OnQHTMHyperlink )
	ON_BN_CLICKED(IDOK, &CRecommendedSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRecommendedSettings::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CRecommendedSettings)
    EASYSIZE(IDC_SHOWALL,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_RECOMMANDEDSETTINGSHTML,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDOK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDCANCEL,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(AFX_IDW_SIZE_BOX,ES_KEEPSIZE,ES_KEEPSIZE, ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CStackSettings message handlers

#define GRIPPIE_SQUARE_SIZE 15

/* ------------------------------------------------------------------- */

BOOL CRecommendedSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWorkspace				workspace;

    CRect			rcClient;
    GetClientRect(&rcClient);
    
    CRect			rcGrip;


    rcGrip.right	= rcClient.right;
    rcGrip.bottom	= rcClient.bottom;
    rcGrip.left		= rcClient.right-GRIPPIE_SQUARE_SIZE;
    rcGrip.top		= rcClient.bottom-GRIPPIE_SQUARE_SIZE;

	m_Gripper.Create(WS_CHILD|WS_VISIBLE|SBS_SIZEGRIP|WS_CLIPSIBLINGS, rcGrip, this, AFX_IDW_SIZE_BOX);

	INIT_EASYSIZE;

	RestoreWindowPosition(this, REGENTRY_BASEKEY_DEEPSKYSTACKER_RECO_POSITION, true);

	workspace.Push();

	if (!m_pStackingTasks)
	{
		GetStackingDlg(this).FillTasks(m_StackingTasks);
		m_pStackingTasks = &m_StackingTasks;
	};

	m_RecommendedSettingsHTML.SetToolTips(FALSE);

	FillWithRecommendedSettings();
	return TRUE;
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	UPDATE_EASYSIZE;
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);

	EASYSIZE_MINSIZE(280,250,nSide,lpRect);
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::ClearText()
{
	CString					strText;
	COLORREF				crColor = ::GetSysColor(COLOR_WINDOW);

	strText.Format(_T("<body link=#0000ff bgcolor=#%02x%02x%02x></body>"),
		GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
	m_vRecommendations.clear();
	m_RecommendedSettingsHTML.SetWindowText(strText);
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::InsertHeader()
{
	CString					strText;
	CString					strHTML;

	strHTML = _T("<table border=1 align=center cellpadding=4 cellspacing=4 bgcolortop=#ececec bgcolorbottom=\"white\" width=\"100%%\"><tr>");
	strHTML += _T("<td>");

	strText.LoadString(IDS_RECO_DISCLAIMER);
	strText.Replace(_T("\n"), _T("<BR>"));
	strHTML += strText;
	strHTML += _T("<BR><BR>");
	strText.LoadString(IDS_RECO_CLICKTOSET);
	strText.Replace(_T("\n"), _T("<BR>"));
	strHTML += strText;
	strHTML += _T("<BR>");
	strText.LoadString(IDS_RECO_ALREADYSET);
	strText.Replace(_T("\n"), _T("<BR>"));

	strHTML += _T("<font color=\"86, 170, 86\">");
	strHTML += strText;;
	strHTML += _T("</font>");
	strHTML += _T("</td></tr></table><BR>");

	m_RecommendedSettingsHTML.AddHTML(strHTML, FALSE);
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::InsertText(LPCTSTR szText, COLORREF crColor, BOOL bBold, BOOL bItalic, LONG lLinkID)
{
	CString					strText;
	CString					strInputText = szText;

	strInputText.Replace(_T("\n"), _T("<BR>"));
	if (lLinkID)
	{
		strText.Format(_T("<a href = \"%ld\">%s</a>"), lLinkID, (LPCTSTR)strInputText);
		strInputText = strText;
	};
	if (bBold)
	{
		strText.Format(_T("<b>%s</b>"), (LPCTSTR)strInputText);
		strInputText = strText;
	};
	if (bItalic)
	{
		strText.Format(_T("<i>%s</i>"), (LPCTSTR)strInputText);
		strInputText = strText;
	};
	{
		strText.Format(_T("<font color = #%02x%02x%02x>%s</font>"),
			GetRValue(crColor),GetGValue(crColor),GetBValue(crColor),(LPCTSTR)strInputText);
		strInputText = strText;
	};

	m_RecommendedSettingsHTML.AddHTML(strInputText, FALSE);
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

static void AddRAWNarrowBandRecommendation(RECOMMANDATIONVECTOR & vRecommendations, BOOL bFITS)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.LoadString(IDS_RECO_RAWNARROWBAND_REASON);
	strText.Replace(_T(" Ha"), _T(" H<font face='Symbol'>a</font>"));

	rec.SetText(strText);
	ri.SetRecommendation(IDS_RECO_RAWNARROWBAND_TEXT);

	if (bFITS)
	{
		ri.AddSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), "SuperPixels");
	}
	else
	{
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("SuperPixels"), true);
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("RawBayer"), false);
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("AHD"), false);
	};
	rec.m_bImportant = false;
	rec.AddItem(ri);

	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddNarrowBandPerChannelBackgroundCalibration(RECOMMANDATIONVECTOR & vRecommendations)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.LoadString(IDS_RECO_RAWNARROWBAND_REASON);
	strText.Replace(_T(" Ha"), _T(" H<font face='Symbol'>a</font>"));

	rec.SetText(strText);
	ri.SetRecommendation(IDS_RECO_USEPERCHANNEL);

	rec.m_bImportant = false;

	ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibration"), false);
	ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PerChannelBackgroundCalibration"), true);

	rec.AddItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddRAWDebayering(RECOMMANDATIONVECTOR & vRecommendations, double fExposureTime, BOOL bFITS)
{
	CRecommendationItem			ri;
	CRecommendation				rec;

	if (fExposureTime > 4*60.0)
	{
		rec.SetText(IDS_RECO_RAWHIGHSNR_REASON);
		ri.SetRecommendation(IDS_RECO_RAWHIGHSNR_TEXT);
	}
	else
	{
		rec.SetText(IDS_RECO_RAWLOWSNR_REASON);
		ri.SetRecommendation(IDS_RECO_RAWLOWSNR_TEXT);
	};

	if (bFITS)
	{
		if (fExposureTime > 4*60.0)
			ri.AddSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), "AHD");
		else
			ri.AddSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), "Bilinear");
	}
	else
	{
		if (fExposureTime > 4*60.0)
		{
			ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("SuperPixels"), false);
			ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("RawBayer"), false);
			ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("Interpolation"), "AHD");
			ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("AHD"), true);
		}
		else
		{
			ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("SuperPixels"), false);
			ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("RawBayer"), false);
			ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("Interpolation"), "Bilinear");
			ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("AHD"), false);
		};
	};

	rec.AddItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddRAWBlackPoint(RECOMMANDATIONVECTOR & vRecommendations, BOOL bFlat, BOOL bBias)
{
	CRecommendationItem			ri;
	CRecommendation				rec;

	if (bBias)
	{
		rec.SetText(IDS_RECO_RAWSETBP_REASON);
		ri.SetRecommendation(IDS_RECO_RAWSETBP_TEXT);
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlackPointTo0"), true);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (bFlat)
	{
		rec.SetText(IDS_RECO_RAWCLEARBP_REASON);
		ri.SetRecommendation(IDS_RECO_RAWCLEARBP_TEXT);
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlackPointTo0"), false);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	};

};

/* ------------------------------------------------------------------- */

static void AddRegisterUseOfMedianFilter(RECOMMANDATIONVECTOR & vRecommendations)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CWorkspace					workspace;
	DWORD						dwThreshold;

	workspace.GetValue(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("DetectionThreshold"), dwThreshold);

	if (dwThreshold <= 5)
	{
		rec.SetText(IDS_RECO_MEDIANFILTER_REASON);
		ri.SetRecommendation(IDS_RECO_MEDIANFILTER_TEXT);
		ri.AddSetting(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("ApplyMedianFilter"), true);

		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddModdedDSLR(RECOMMANDATIONVECTOR & vRecommendations, BOOL bFITS)
{
	CRecommendationItem			ri;
	CRecommendation				rec;

	rec.SetText(IDS_RECO_MODDEDDSLR_REASON);
	ri.SetRecommendation(IDS_RECO_MODDEDDSLR_TEXT);

	if (bFITS)
	{
		ri.AddSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("Brighness"), 1.0);
		ri.AddSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("RedScale"), 1.0);
		ri.AddSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("BlueScale"), 1.0);
	}
	else
	{
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("AutoWB"), false);
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("CameraWB"), false);
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("Brighness"), 1.0);
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("RedScale"), 1.0);
		ri.AddSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlueScale"), 1.0);
	};

	rec.m_bImportant = false;

	rec.AddItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddCometStarTrails(RECOMMANDATIONVECTOR & vRecommendations, LONG lNrLightFrames)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CWorkspace					workspace;
	DWORD						dwCometMode;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CometStackingMode"), dwCometMode);

	if (dwCometMode == CSM_COMETONLY)
	{
		rec.SetText(IDS_RECO_COMETSTARTRAILS_REASON);
		ri.SetRecommendation(IDS_RECO_USEAVERAGECOMBINE);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)MBP_AVERAGE);

		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (dwCometMode == CSM_COMETSTAR)
	{
		CString					strText;

		strText.Format(IDS_RECO_COMETSTARSMANY_REASON, lNrLightFrames);
		if (lNrLightFrames>15)
		{
			rec.SetText(strText);
			ri.SetRecommendation(IDS_RECO_USESIGMACLIPPING);
			ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)MBP_SIGMACLIP);
		}
		else
		{
			rec.SetText(strText);
			ri.SetRecommendation(IDS_RECO_USEMEDIAN);
			ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)MBP_MEDIAN);
		};
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddLightMethod(RECOMMANDATIONVECTOR & vRecommendations, LONG lNrFrames)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.Format(IDS_RECO_LIGHT_REASON, lNrFrames);

	if (lNrFrames > 15)
	{
		rec.SetText(strText);
		ri.SetRecommendation(IDS_RECO_USESIGMACLIPPING);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)MBP_SIGMACLIP);
		rec.AddItem(ri);
		ri.Clear();
		ri.SetRecommendation(IDS_RECO_USEAUTOADAPTIVEAVERAGE);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)MBP_AUTOADAPTIVE);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (lNrFrames > 1)
	{
		rec.SetText(strText);
		ri.SetRecommendation(IDS_RECO_USEAVERAGECOMBINE);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)MBP_AVERAGE);
		rec.AddItem(ri);
		ri.Clear();
		ri.SetRecommendation(IDS_RECO_USEMEDIAN);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)MBP_MEDIAN);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddDarkMethod(RECOMMANDATIONVECTOR & vRecommendations, LONG lNrFrames)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.Format(IDS_RECO_DARK_REASON, lNrFrames);

	if (lNrFrames > 15)
	{
		rec.SetText(strText);
		ri.SetRecommendation(IDS_RECO_USESIGMAMEDIAN);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Method"), (DWORD)MBP_MEDIANSIGMACLIP);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (lNrFrames > 1)
	{
		rec.SetText(strText);
		ri.SetRecommendation(IDS_RECO_USEMEDIAN);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Method"), (DWORD)MBP_MEDIAN);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddBiasMethod(RECOMMANDATIONVECTOR & vRecommendations, LONG lNrFrames)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.Format(IDS_RECO_BIAS_REASON, lNrFrames);

	if (lNrFrames > 15)
	{
		rec.SetText(strText);
		ri.SetRecommendation(IDS_RECO_USESIGMAMEDIAN);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Method"), (DWORD)MBP_MEDIANSIGMACLIP);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (lNrFrames > 1)
	{
		rec.SetText(strText);
		ri.SetRecommendation(IDS_RECO_USEMEDIAN);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Method"), (DWORD)MBP_MEDIAN);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddFlatMethod(RECOMMANDATIONVECTOR & vRecommendations, LONG lNrFrames)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.Format(IDS_RECO_FLAT_REASON, lNrFrames);

	if (lNrFrames > 15)
	{
		rec.SetText(strText);
		ri.SetRecommendation(IDS_RECO_USESIGMAMEDIAN);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Method"), (DWORD)MBP_MEDIANSIGMACLIP);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (lNrFrames > 1)
	{
		rec.SetText(strText);
		ri.SetRecommendation(IDS_RECO_USEMEDIAN);
		ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Method"), (DWORD)MBP_MEDIAN);
		rec.AddItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddPerChannelBackgroundCalibration(RECOMMANDATIONVECTOR & vRecommendations)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.Format(IDS_RECO_PERCHANNELCALIBRATION_REASON);

	rec.SetText(strText);
	ri.SetRecommendation(IDS_RECO_USEPERCHANNEL);

	ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibration"), false);
	ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PerChannelBackgroundCalibration"), true);

	rec.m_bImportant = FALSE;

	rec.AddItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddRGBChannelBackgroundCalibration(RECOMMANDATIONVECTOR & vRecommendations)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.Format(IDS_RECO_RGBCALIBRATION_REASON);

	rec.SetText(strText);
	ri.SetRecommendation(IDS_RECO_USERGBCALIBRATION);

	ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibration"), true);
	ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PerChannelBackgroundCalibration"), false);

	rec.m_bImportant = FALSE;

	rec.AddItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddPerChannelBackgroundCalibrationGray(RECOMMANDATIONVECTOR & vRecommendations)
{
	CRecommendationItem			ri;
	CRecommendation				rec;
	CString						strText;

	strText.Format(IDS_RECO_PERCHANNELCALIBRATIONGRAY_REASON);

	rec.SetText(strText);
	ri.SetRecommendation(IDS_RECO_USEPERCHANNEL);

	ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibration"), false);
	ri.AddSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PerChannelBackgroundCalibration"), true);

	rec.AddItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::FillWithRecommendedSettings()
{
	int						nScrollPos;

	nScrollPos = QHTM_GetScrollPos(m_RecommendedSettingsHTML.GetSafeHwnd());

	m_RecommendedSettingsHTML.SetRedraw(FALSE);
	ClearText();
	if (m_pStackingTasks && m_pStackingTasks->GetNrLightFrames())
	{
		CString					strText;
		LONG					lPosition;

		InsertHeader();
		AddRegisterUseOfMedianFilter(m_vRecommendations);

		lPosition = (LONG)m_vRecommendations.size();

		if (m_pStackingTasks->AreBayerImageUsed())
		{
			AddRAWDebayering(m_vRecommendations, m_pStackingTasks->GetMaxExposureTime(), m_pStackingTasks->AreFITSImageUsed());
			AddModdedDSLR(m_vRecommendations, m_pStackingTasks->AreFITSImageUsed());
			AddRAWNarrowBandRecommendation(m_vRecommendations, m_pStackingTasks->AreFITSImageUsed());
			if (!m_pStackingTasks->AreFITSImageUsed())
				AddRAWBlackPoint(m_vRecommendations, m_pStackingTasks->AreFlatUsed(), m_pStackingTasks->AreBiasUsed());
		};

		if (m_pStackingTasks->AreColorImageUsed())
			AddNarrowBandPerChannelBackgroundCalibration(m_vRecommendations);

		if (m_vRecommendations.size()!=lPosition)
			m_vRecommendations[lPosition].m_bBreakBefore = true;
		lPosition = (LONG)m_vRecommendations.size();

		if (m_pStackingTasks->IsCometAvailable())
		{
			AddCometStarTrails(m_vRecommendations, m_pStackingTasks->GetNrLightFrames());
		};

		if (m_vRecommendations.size()!=lPosition)
			m_vRecommendations[lPosition].m_bBreakBefore = true;
		lPosition = (LONG)m_vRecommendations.size();

		AddLightMethod(m_vRecommendations, m_pStackingTasks->GetNrLightFrames());

		if (m_pStackingTasks->GetNrBiasFrames())
			AddBiasMethod(m_vRecommendations, m_pStackingTasks->GetNrBiasFrames());

		if (max(m_pStackingTasks->GetNrDarkFrames(), m_pStackingTasks->GetNrDarkFlatFrames()))
			AddDarkMethod(m_vRecommendations, max(m_pStackingTasks->GetNrDarkFrames(), m_pStackingTasks->GetNrDarkFlatFrames()));

		if (m_pStackingTasks->GetNrFlatFrames())
			AddFlatMethod(m_vRecommendations, m_pStackingTasks->GetNrFlatFrames());

		if (m_vRecommendations.size()!=lPosition)
			m_vRecommendations[lPosition].m_bBreakBefore = true;
		lPosition = (LONG)m_vRecommendations.size();

		if (m_pStackingTasks->AreColorImageUsed())
		{
			AddPerChannelBackgroundCalibration(m_vRecommendations);
			AddRGBChannelBackgroundCalibration(m_vRecommendations);
		}
		else
		{
			AddPerChannelBackgroundCalibrationGray(m_vRecommendations);
		};

		if (m_vRecommendations.size()!=lPosition)
			m_vRecommendations[lPosition].m_bBreakBefore = true;
		lPosition = (LONG)m_vRecommendations.size();

		LONG					lLastLinkID = 0;
		CString					strOr;

		strOr.LoadString(IDS_OR);

		for (LONG i = 0;i<m_vRecommendations.size();i++)
		{
			COLORREF			crColor;
			BOOL				bDifferent = FALSE;

			if (m_vRecommendations[i].IsBreakBefore())
				InsertText(_T("<hr>"));
			for (LONG j = 0;j<m_vRecommendations[i].m_vRecommendations.size() && !bDifferent;j++)
			{
				bDifferent = m_vRecommendations[i].m_vRecommendations[j].IsDifferent();
			};

			if (bDifferent)
			{
				if (m_vRecommendations[i].m_bImportant)
					crColor = RGB(128, 0, 0);
				else
					crColor = RGB(0, 0, 192);
			}
			else
				crColor = RGB(128, 128, 128);

			InsertText(m_vRecommendations[i].m_strText, crColor);
			InsertText(_T("\n"));

			for (LONG j = 0;j<m_vRecommendations[i].m_vRecommendations.size();j++)
			{
				CRecommendationItem &	ri = m_vRecommendations[i].m_vRecommendations[j];
				bool					bAlreadySet;
				LONG					lLinkID = 0;

				bAlreadySet = !ri.IsDifferent();

				if (j)
				{
					InsertText(_T("    "));
					InsertText(strOr+"\n", RGB(0, 0, 0), FALSE, TRUE);
				};

				InsertText(_T("->  "));
				if (bAlreadySet)
				{
					InsertText(ri.m_strRecommendation, RGB(86, 170, 86));
				}
				else
				{
					lLastLinkID++;
					lLinkID = lLastLinkID;
					InsertText(ri.m_strRecommendation, RGB(0, 0, 128), FALSE, FALSE, lLinkID);
				};
				ri.m_lLinkID = lLinkID;
			};
			InsertText(_T("\n\n"));
		};
	}
	else
	{
		CString				strText;

		strText.LoadString(IDS_RECO_PREREQUISITES);
		InsertText(strText, RGB(255, 0, 0), TRUE);
	};

	QHTM_SetScrollPos(m_RecommendedSettingsHTML.GetSafeHwnd(), nScrollPos);
	m_RecommendedSettingsHTML.SetRedraw(TRUE);
	m_RecommendedSettingsHTML.Invalidate(TRUE);
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::SetSetting(LONG lID)
{
	BOOL					bFound = FALSE;

	for (LONG i = 0;i<m_vRecommendations.size() && !bFound;i++)
	{
		for (LONG j = 0;j<m_vRecommendations[i].m_vRecommendations.size() && !bFound;j++)
		{
			CRecommendationItem &	ri = m_vRecommendations[i].m_vRecommendations[j];

			if (ri.m_lLinkID == lID)
			{
				bFound = TRUE;
				ri.ApplySettings();
			};
		};
	};

	if (bFound)
		FillWithRecommendedSettings();
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::OnQHTMHyperlink(NMHDR*nmh, LRESULT*)
{
	LPNMQHTM pnm = reinterpret_cast<LPNMQHTM>( nmh );
	if( pnm->pcszLinkText )
	{
		pnm->resReturnValue = FALSE;
		LONG				lLinkID;

		lLinkID = _ttol(pnm->pcszLinkText);
		SetSetting(lLinkID);
	}
}

/* ------------------------------------------------------------------- */

void CRecommendedSettings::OnBnClickedShowAll()
{
	FillWithRecommendedSettings();
};

/* ------------------------------------------------------------------- */

void CRecommendedSettings::OnBnClickedOk()
{
	SaveWindowPosition(this, REGENTRY_BASEKEY_DEEPSKYSTACKER_RECO_POSITION);

	CWorkspace				workspace;

	workspace.Pop(false);
	workspace.SaveToRegistry();
	OnOK();
}

/* ------------------------------------------------------------------- */

void CRecommendedSettings::OnBnClickedCancel()
{
	SaveWindowPosition(this, REGENTRY_BASEKEY_DEEPSKYSTACKER_RECO_POSITION);

	CWorkspace				workspace;

	workspace.Pop();
	OnCancel();
}

/* ------------------------------------------------------------------- */
