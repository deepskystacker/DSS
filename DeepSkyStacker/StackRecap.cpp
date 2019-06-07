// StackRecap.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "StackRecap.h"
#include "StackSettings.h"
#include "Multitask.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "RecommandedSettings.h"
#include "Registry.h"
#include "DeepStackerDlg.h"

// CStackRecap dialog

/* ------------------------------------------------------------------- */

IMPLEMENT_DYNAMIC(CStackRecap, CDialog)

CStackRecap::CStackRecap(CWnd* pParent /*=NULL*/)
	: CDialog(CStackRecap::IDD, pParent)
{
	m_pStackingTasks = NULL;
}

/* ------------------------------------------------------------------- */

CStackRecap::~CStackRecap()
{
}

/* ------------------------------------------------------------------- */

void CStackRecap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECAPHTML, m_RecapHTML);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CStackRecap, CDialog)
	ON_BN_CLICKED(IDC_STACKINGPARAMETERS, &CStackRecap::OnBnClickedStackingparameters)
	ON_BN_CLICKED(IDC_RECOMMANDEDSETTINGS, &CStackRecap::OnBnClickedRecommandedsettings)
	ON_NOTIFY( QHTMN_HYPERLINK, IDC_RECAPHTML, OnQHTMHyperlink )
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDOK, &CStackRecap::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CStackRecap::OnBnClickedCancel)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CStackRecap)
    EASYSIZE(IDC_RECAPHTML,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDOK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDCANCEL,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDC_STACKINGPARAMETERS,ES_BORDER, ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(IDC_RECOMMANDEDSETTINGS,ES_BORDER, ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
    EASYSIZE(AFX_IDW_SIZE_BOX,ES_KEEPSIZE,ES_KEEPSIZE, ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CStackSettings message handlers

#define GRIPPIE_SQUARE_SIZE 15

BOOL CStackRecap::OnInitDialog() 
{
	CDialog::OnInitDialog();

    CRect			rcClient;
    GetClientRect(&rcClient);
    
    CRect			rcGrip;
    rcGrip.right	= rcClient.right;
    rcGrip.bottom	= rcClient.bottom;
    rcGrip.left		= rcClient.right-GRIPPIE_SQUARE_SIZE;
    rcGrip.top		= rcClient.bottom-GRIPPIE_SQUARE_SIZE;

	m_Gripper.Create(WS_CHILD|WS_VISIBLE|SBS_SIZEGRIP|WS_CLIPSIBLINGS, rcGrip, this, AFX_IDW_SIZE_BOX);

	INIT_EASYSIZE;

	RestoreWindowPosition(this, REGENTRY_BASEKEY_DEEPSKYSTACKER_STEPS_POSITION, true);

	m_RecapHTML.SetToolTips(FALSE);
	SetWindowLong(m_RecapHTML.GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(m_RecapHTML.GetSafeHwnd(), GWL_STYLE ) | WS_EX_CLIENTEDGE );

	FillWithAllTasksHTML();
	return TRUE;
};

/* ------------------------------------------------------------------- */

void CStackRecap::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	UPDATE_EASYSIZE;
};

/* ------------------------------------------------------------------- */

void CStackRecap::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);

	EASYSIZE_MINSIZE(280,250,nSide,lpRect);
};

/* ------------------------------------------------------------------- */

void	CStackRecap::InsertHeaderHTML(CString & strHTML)
{
	CString					strText;
	COLORREF				crColor = ::GetSysColor(COLOR_WINDOW);

	strText.Format(_T("<body link=#0000ff bgcolor=#%02x%02x%02x></body>"),
		GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
	strHTML += strText;
};

/* ------------------------------------------------------------------- */

void	CStackRecap::InsertHTML(CString & strHTML, LPCTSTR szText, COLORREF crColor, BOOL bBold, BOOL bItalic, LONG lLinkID)
{
	CString					strText;
	CString					strInputText = szText;

	strInputText.Replace(_T("\n"), _T("<BR>"));
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
	if (lLinkID)
	{
		strText.Format(_T("<a href = \"%ld\">%s</a>"), lLinkID, (LPCTSTR)strInputText);
		strInputText = strText;
	};
	{
		strText.Format(_T("<font color = #%02x%02x%02x>%s</font>"),
			GetRValue(crColor),GetGValue(crColor),GetBValue(crColor),(LPCTSTR)strInputText);
		strInputText = strText;
	};
	strHTML += strInputText;
};

/* ------------------------------------------------------------------- */

void CStackRecap::FillWithAllTasksHTML()
{
	ZFUNCTRACE_RUNTIME();
	if (m_pStackingTasks)
	{
		CString				strHTML;

		InsertHeaderHTML(strHTML);

		CString				strText;
		CString				strExposure;
		CString				strISO;
		LONG				i, j;
		LONG				lTotalExposure = 0;
		__int64				ulNeededSpace;
		__int64				ulFreeSpace;
		CString				strDrive;
		CString				strFreeSpace;
		CString				strNeededSpace;
		CString				strDrizzle;
		CString				strDrizzleWarning;
		STACKINGMODE		ResultMode;
		BOOL				bSaveIntermediates;

		ulNeededSpace	= m_pStackingTasks->ComputeNecessaryDiskSpace();
		ulFreeSpace		= m_pStackingTasks->AvailableDiskSpace(strDrive);
		ResultMode		= m_pStackingTasks->GetStackingMode();
		bSaveIntermediates = m_pStackingTasks->GetCreateIntermediates();

		SpaceToString(ulFreeSpace, strFreeSpace);
		SpaceToString(ulNeededSpace, strNeededSpace);

		CString				strYesNo;
		CString				strBackgroundCalibration;
		CString				strPerChannelBackgroundCalibration;
		CString				strDarkOptimization;
		CString				strDarkFactor;
		CString				strHotPixels;
		BACKGROUNDCALIBRATIONMODE	CalibrationMode;

		CalibrationMode = m_pStackingTasks->GetBackgroundCalibrationMode();
		
		strYesNo.LoadString((CalibrationMode == BCM_RGB) ? IDS_YES : IDS_NO);
		strBackgroundCalibration.Format(IDS_RECAP_BACKGROUNDCALIBRATION, strYesNo);

		strYesNo.LoadString((CalibrationMode == BCM_PERCHANNEL) ? IDS_YES : IDS_NO);
		strPerChannelBackgroundCalibration.Format(IDS_RECAP_PERCHANNELBACKGROUNDCALIBRATION, strYesNo);

		strYesNo.LoadString(m_pStackingTasks->GetDarkOptimization() ? IDS_YES : IDS_NO);
		strDarkOptimization.Format(IDS_RECAP_DARKOPTIMIZATION, strYesNo);

		double				fDarkFactor = m_pStackingTasks->GetDarkFactor();
		if (fDarkFactor != 1.0)
			strDarkFactor.Format(IDS_RECAP_DARKMULTIPLICATIONFACTOR, fDarkFactor);

		strYesNo.LoadString(m_pStackingTasks->GetHotPixelsDetection() ? IDS_YES : IDS_NO);
		strHotPixels.Format(IDS_RECAP_HOTPIXELS, strYesNo);

		if (ulFreeSpace < ulNeededSpace)
		{
			// Warning about the available space on drive
			InsertHTML(strHTML,
				_T("<table border='1px' cellpadding='5' bordercolor=#ffc0c0 bordercolorlight=#ffc0c0 bgcolor=#ffffc0 bordercolordark=#ffffc0 cellspacing=0 width='100%'><tr><td>"));
			strText.Format(IDS_RECAP_WARNINGDISKSPACE, strNeededSpace, strDrive, strFreeSpace);
			InsertHTML(strHTML, strText, RGB(255, 0, 0), TRUE, FALSE);
			if (ResultMode == SM_MOSAIC)
			{
				InsertHTML(strHTML, _T("\n"));
				strText.Format(IDS_RECAP_MOSAICWARNING);
				InsertHTML(strHTML, strText);
			};
			InsertHTML(strHTML, _T("</td></tr></table>"));
		};

		if (m_pStackingTasks->AreCalibratingJPEGFiles())
		{
			InsertHTML(strHTML,
				_T("<table border='1px' cellpadding='5' bordercolor=#ffc0c0 bordercolorlight=#ffc0c0 bgcolor=#ffffc0 bordercolordark=#ffffc0 cellspacing=0 width='100%'><tr><td>"));
			strText.Format(IDS_RECAP_WARNINGJPEG);
			InsertHTML(strHTML, strText, RGB(255, 0, 0), TRUE, FALSE);
			InsertHTML(strHTML, _T("</td></tr></table>"));
		};

		InsertHTML(strHTML, _T("<table border=0 valign=middle cellspacing=0 width='100%'><tr>"));
		InsertHTML(strHTML, _T("<td width='48%'>"));
		strText.Format(IDS_RECAP_STACKINGMODE);
		InsertHTML(strHTML, strText, RGB(0, 0, 0), TRUE);
		switch (m_pStackingTasks->GetStackingMode())
		{
		case SM_NORMAL :
			strText.Format(IDS_RECAP_STACKINGMODE_NORMAL);
			break;
		case SM_MOSAIC :
			strText.Format(IDS_RECAP_STACKINGMODE_MOSAIC);
			break;
		case SM_CUSTOM :
			strText.Format(IDS_RECAP_STACKINGMODE_CUSTOM);
			break;
		case SM_INTERSECTION :
			strText.Format(IDS_RECAP_STACKINGMODE_INTERSECTION);
			break;
		};

		InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_RESULT);

		DWORD				dwAlignment;
		dwAlignment = m_pStackingTasks->GetAlignmentMethod();

		strText.Format(IDS_RECAP_ALIGNMENT);
		InsertHTML(strHTML, _T("</td><td width='48%'>"));
		InsertHTML(strHTML, strText, RGB(0, 0, 0), TRUE);

		switch (dwAlignment)
		{
		case 0 :
		case 1 :
			strText.Format(IDS_ALIGN_AUTO);
			break;
		case 2 :
			strText.Format(IDS_ALIGN_BILINEAR);
			break;
		case 3 :
			strText.Format(IDS_ALIGN_BISQUARED);
			break;
		case 4 :
			strText.Format(IDS_ALIGN_BICUBIC);
			break;
		case 5 :
			strText.Format(IDS_ALIGN_NONE);
			break;
		};

		InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_ALIGNMENT);
		InsertHTML(strHTML, _T("</td>"));
		InsertHTML(strHTML, _T("</tr></table>"));

		DWORD				dwDrizzle;

		dwDrizzle = m_pStackingTasks->GetPixelSizeMultiplier();
		if (dwDrizzle > 1)
		{
			strText.Format(IDS_RECAP_DRIZZLE, dwDrizzle);
			InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_RESULT);
			InsertHTML(strHTML, _T("\n"));
			if (IsRawBayer() || IsFITSRawBayer())
			{
				strText.Format(IDS_RECAP_WARNINGDRIZZLE);
				InsertHTML(strHTML, strText, RGB(255, 0, 0), TRUE);
				InsertHTML(strHTML, _T("\n"));
			};
		};

		LONG				lNrProcessors;

		lNrProcessors = CMultitask::GetNrProcessors(TRUE);
		if (lNrProcessors>1)
		{
			if (CMultitask::GetNrProcessors()>1)
				strText.Format(IDS_RECAP_DETECTEDANDUSEDPROCESSORS, lNrProcessors);
			else
				strText.Format(IDS_RECAP_DETECTEDNOTUSEDPROCESSORS, lNrProcessors);
			InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, TRUE);
			InsertHTML(strHTML, _T("\n"));
		};

		// Comet Info
		if (m_pStackingTasks->IsCometAvailable())
		{
			COMETSTACKINGMODE	CometStackingMode;

			CometStackingMode = m_pStackingTasks->GetCometStackingMode();
			strText.Format(IDS_RECAP_COMETSTACKING);
			InsertHTML(strHTML, strText, RGB(0, 0, 0), TRUE, FALSE);
			switch (CometStackingMode)
			{
			case CSM_STANDARD :
				strText.Format(IDS_RECAP_COMETSTACKING_NONE);
				break;
			case CSM_COMETONLY :
				strText.Format(IDS_RECAP_COMETSTACKING_COMET);
				break;
			case CSM_COMETSTAR :
				strText.Format(IDS_RECAP_COMETSTACKING_BOTH);
				break;
			};
			InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_COMET);
			InsertHTML(strHTML, _T("\n"));
		};
		InsertHTML(strHTML, _T("\n"));

		CPostCalibrationSettings		pcs;

		m_pStackingTasks->GetPostCalibrationSettings(pcs);
		if (pcs.m_bHot)
		{
			strText.Format(IDS_RECAP_COSMETICHOT, pcs.m_lHotFilter, pcs.m_fHotDetection);
			InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_POSTCALIBRATION);
		};
		if (pcs.m_bCold)
		{
			strText.Format(IDS_RECAP_COSMETICCOLD, pcs.m_lColdFilter, pcs.m_fColdDetection);
			InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_POSTCALIBRATION);
		};

		if (pcs.m_bHot || pcs.m_bCold)
			InsertHTML(strHTML, _T("\n"));

		for (i = 0;i<m_pStackingTasks->m_vStacks.size();i++)
		{
			CStackingInfo &			si = m_pStackingTasks->m_vStacks[i];

			if (si.m_pLightTask)
			{
				InsertHTML(strHTML,
					_T("<table border='1px' bgcolorleft=#fff9fa bgcolorright=#f9fbff bgcolor=#fafafa bordercolordark=#fafafa bordercolor=#c0c0c0 bordercolorlight=#c0c0c0  cellspacing=0 cellpadding=5 width='100%'><tr><td>"));
				LONG			lTaskExposure = 0;

				for (j = 0;j<si.m_pLightTask->m_vBitmaps.size();j++)
					lTaskExposure += si.m_pLightTask->m_vBitmaps[j].m_fExposure;

				lTotalExposure += lTaskExposure;

				ExposureToString(lTaskExposure, strExposure);
				ISOToString(si.m_pLightTask->m_lISOSpeed, strISO);

				strText.Format(IDS_RECAP_STEP, i+1, si.m_pLightTask->m_vBitmaps.size(), strISO);
				InsertHTML(strHTML, strText, RGB(0, 0, 0), TRUE);
				InsertHTML(strHTML, strExposure, RGB(0, 128, 0), TRUE);
				InsertHTML(strHTML, _T("\n"));
				InsertHTML(strHTML, _T("<ul>"));
				InsertHTML(strHTML, strBackgroundCalibration, RGB(0, 0, 128), FALSE, FALSE, SSTAB_LIGHT);
				InsertHTML(strHTML, strPerChannelBackgroundCalibration, RGB(0, 0, 128), FALSE, FALSE, SSTAB_LIGHT);
				InsertHTML(strHTML, _T("</ul>"));
				if (si.m_pLightTask->m_vBitmaps.size()>1)
				{
					InsertHTML(strHTML, _T("<ul>"));
					strText.Format(IDS_RECAP_METHOD);
					InsertHTML(strHTML, strText);
					FormatFromMethod(strText, si.m_pLightTask->m_Method, si.m_pLightTask->m_fKappa, si.m_pLightTask->m_lNrIterations);
					InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_LIGHT);
					InsertHTML(strHTML, _T("</ul>"));

					if ((si.m_pLightTask->m_Method != MBP_AVERAGE) && 
						(IsRawBayer() || IsFITSRawBayer()))
					{
						InsertHTML(strHTML, _T("\n"));
						strText.Format(IDS_RECAP_WARNINGBAYERDRIZZLE);
						InsertHTML(strHTML, strText, RGB(128, 0, 0), FALSE, TRUE);
					};
				};

				InsertHTML(strHTML, _T("<hr>"));
				if (si.m_pDarkTask || si.m_pOffsetTask || si.m_pFlatTask || si.m_pDarkFlatTask)
					InsertHTML(strHTML, _T("<ul>"));

				if (si.m_pOffsetTask)
				{
					ExposureToString(si.m_pOffsetTask->m_fExposure, strExposure);
					ISOToString(si.m_pOffsetTask->m_lISOSpeed, strISO);

					strText.Format(IDS_RECAP_OFFSET, si.m_pOffsetTask->m_vBitmaps.size(), strISO, strExposure);
					InsertHTML(strHTML, strText);

					if (si.m_pOffsetTask->m_vBitmaps.size()>1)
					{
						InsertHTML(strHTML, _T("<ul>"));
						strText.Format(IDS_RECAP_METHOD);
						InsertHTML(strHTML, strText);
						FormatFromMethod(strText, si.m_pOffsetTask->m_Method, si.m_pOffsetTask->m_fKappa, si.m_pOffsetTask->m_lNrIterations);
						InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_OFFSET);
						InsertHTML(strHTML, _T("</ul>"));
					};

					if (si.m_pOffsetTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
					{
						strText.Format(IDS_RECAP_ISOWARNING);
						InsertHTML(strHTML, strText, RGB(128, 0, 0), FALSE, TRUE);
					};
				}
				else
				{
					strText.Format(IDS_RECAP_NOOFFSET);
					InsertHTML(strHTML, strText, RGB(128, 0, 0));
				};
				if (si.m_pDarkTask)
				{
					ExposureToString(si.m_pDarkTask->m_fExposure, strExposure);
					ISOToString(si.m_pDarkTask->m_lISOSpeed, strISO);

					strText.Format(IDS_RECAP_DARK, si.m_pDarkTask->m_vBitmaps.size(), strISO, strExposure);
					InsertHTML(strHTML, strText);

					if (si.m_pDarkTask->m_vBitmaps.size()>1)
					{
						InsertHTML(strHTML, _T("<ul>"));
						strText.Format(IDS_RECAP_METHOD);
						InsertHTML(strHTML, strText);
						FormatFromMethod(strText, si.m_pDarkTask->m_Method, si.m_pDarkTask->m_fKappa, si.m_pDarkTask->m_lNrIterations);
						InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_DARK);
						InsertHTML(strHTML, _T("</ul>"));
					};

					InsertHTML(strHTML, _T("<ul>"));
					InsertHTML(strHTML, strDarkOptimization, RGB(0, 0, 128), FALSE, FALSE, SSTAB_DARK);
					InsertHTML(strHTML, strHotPixels, RGB(0, 0, 128), FALSE, FALSE, SSTAB_DARK);
					if (strDarkFactor.GetLength())
					{
						InsertHTML(strHTML, strDarkFactor, RGB(0, 0, 128), FALSE, FALSE, SSTAB_DARK);
						InsertHTML(strHTML, _T("\n"));
					};

					if (si.m_pDarkTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
					{
						strText.Format(IDS_RECAP_ISOWARNING);
						InsertHTML(strHTML, strText, RGB(128, 0, 0), FALSE, TRUE);
						InsertHTML(strHTML, _T("\n"));
					};
					if (!AreExposureEquals(si.m_pDarkTask->m_fExposure, si.m_pLightTask->m_fExposure))
					{
						strText.Format(IDS_RECAP_EXPOSUREWARNING);
						InsertHTML(strHTML, strText, RGB(128, 0, 0), FALSE, TRUE);
						InsertHTML(strHTML, _T("\n"));
					};
					InsertHTML(strHTML, _T("</ul>"));
				}
				else
				{
					strText.Format(IDS_RECAP_NODARK);
					InsertHTML(strHTML, strText, RGB(128, 0, 0));
				};
				if (si.m_pDarkFlatTask && si.m_pFlatTask)
				{
					ExposureToString(si.m_pDarkFlatTask->m_fExposure, strExposure);
					ISOToString(si.m_pDarkFlatTask->m_lISOSpeed, strISO);

					strText.Format(IDS_RECAP_DARKFLAT, si.m_pDarkFlatTask->m_vBitmaps.size(), strISO, strExposure);
					InsertHTML(strHTML, strText);

					if (si.m_pDarkFlatTask->m_vBitmaps.size()>1)
					{
						InsertHTML(strHTML, _T("<ul>"));
						strText.Format(IDS_RECAP_METHOD);
						InsertHTML(strHTML, strText+_T("\n"));
						FormatFromMethod(strText, si.m_pDarkFlatTask->m_Method, si.m_pDarkFlatTask->m_fKappa, si.m_pDarkFlatTask->m_lNrIterations);
						InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_DARK);
						InsertHTML(strHTML, _T("</ul>"));
					};

					if (si.m_pDarkFlatTask->m_lISOSpeed != si.m_pFlatTask->m_lISOSpeed)
					{
						strText.Format(IDS_RECAP_ISOWARNINGDARKFLAT);
						InsertHTML(strHTML, strText, RGB(128, 0, 0), FALSE, TRUE);
					};
					if (!AreExposureEquals(si.m_pDarkFlatTask->m_fExposure, si.m_pFlatTask->m_fExposure))
					{
						strText.Format(IDS_RECAP_EXPOSUREWARNINGDARKFLAT);
						InsertHTML(strHTML, strText, RGB(128, 0, 0), FALSE, TRUE);
					};
				};
				if (si.m_pFlatTask)
				{
					ExposureToString(si.m_pFlatTask->m_fExposure, strExposure);
					ISOToString(si.m_pFlatTask->m_lISOSpeed, strISO);

					strText.Format(IDS_RECAP_FLAT, si.m_pFlatTask->m_vBitmaps.size(), strISO, strExposure);
					InsertHTML(strHTML, strText);
					if (si.m_pFlatTask->m_vBitmaps.size()>1)
					{
						InsertHTML(strHTML, _T("<ul>"));
						strText.Format(IDS_RECAP_METHOD);
						InsertHTML(strHTML, strText);
						FormatFromMethod(strText, si.m_pFlatTask->m_Method, si.m_pFlatTask->m_fKappa, si.m_pFlatTask->m_lNrIterations);
						InsertHTML(strHTML, strText, RGB(0, 0, 128), FALSE, FALSE, SSTAB_FLAT);
						InsertHTML(strHTML, _T("</ul>"));
					};

					if (si.m_pFlatTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
					{
						strText.Format(IDS_RECAP_ISOWARNING);
						InsertHTML(strHTML, strText, RGB(128, 0, 0), FALSE, TRUE);
					};
				}
				else
				{
					strText.Format(IDS_RECAP_NOFLAT);
					InsertHTML(strHTML, strText, RGB(128, 0, 0));
				};

				if (si.m_pDarkTask || si.m_pOffsetTask || si.m_pFlatTask || si.m_pDarkFlatTask)
					InsertHTML(strHTML, _T("</ul>"));
				InsertHTML(strHTML, _T("</td></tr></table><br>"));
			};
		};

		ExposureToString(lTotalExposure, strExposure);
		strText.Format(_T("<font color=#008f00>%s</font>"), (LPCTSTR)strExposure);
		strExposure = strText;
		strText.Format(IDS_RECAP_TOTALEXPOSURETIME, (LPCTSTR)strExposure);
		InsertHTML(strHTML, strText, RGB(0, 0, 0), TRUE, TRUE);

		if (ulFreeSpace > ulNeededSpace)
		{
			InsertHTML(strHTML, _T("\n"));
			strText.Format(IDS_RECAP_INFODISKSPACE, strNeededSpace, strDrive, strFreeSpace);
			InsertHTML(strHTML, strText);
			if (ResultMode == SM_MOSAIC)
			{
				InsertHTML(strHTML, _T("\n"));
				strText.Format(IDS_RECAP_MOSAICWARNING);
				InsertHTML(strHTML, strText);
			};
			if (bSaveIntermediates)
			{
				InsertHTML(strHTML, _T("\n\n"));
				strText.Format(IDS_RECAP_WARNINGINTERMEDIATESAVE);
				InsertHTML(strHTML, strText, RGB(128, 0, 0));
			};
		};

		int						nScrollPos;

		nScrollPos = QHTM_GetScrollPos(m_RecapHTML.GetSafeHwnd());

		m_RecapHTML.SetRedraw(FALSE);

		m_RecapHTML.SetWindowText(strHTML);
		QHTM_SetScrollPos(m_RecapHTML.GetSafeHwnd(), nScrollPos);
		m_RecapHTML.SetRedraw(TRUE);
		m_RecapHTML.Invalidate(TRUE);
	};
};

/* ------------------------------------------------------------------- */

void CStackRecap::CallStackingParameters(LONG lID)
{
	ZFUNCTRACE_RUNTIME();

	CStackSettings			dlg;
	CRect					rcCustom;

	if (m_pStackingTasks->GetCustomRectangle(rcCustom))
		dlg.SetCustomRectangleAvailability(TRUE, m_pStackingTasks->IsCustomRectangleUsed());
	else
		dlg.SetCustomRectangleAvailability(FALSE);

	if (m_pStackingTasks->IsCometAvailable())
		dlg.EnableCometStacking(TRUE);

	dlg.SetDarkFlatBiasTabsVisibility(m_pStackingTasks->AreDarkUsed(), m_pStackingTasks->AreFlatUsed(), m_pStackingTasks->AreBiasUsed());

	dlg.SetStackingTasks(m_pStackingTasks);
	dlg.SetStartingTab(lID);
	if (dlg.DoModal() == IDOK)
	{
		m_pStackingTasks->UseCustomRectangle(dlg.IsCustomRectangleUsed());
		if (m_pStackingTasks)
		{
			m_pStackingTasks->UpdateTasksMethods();
			FillWithAllTasksHTML();
		};
	};
};

/* ------------------------------------------------------------------- */

void CStackRecap::OnBnClickedStackingparameters()
{
	CallStackingParameters();
}

/* ------------------------------------------------------------------- */

void CStackRecap::OnQHTMHyperlink(NMHDR*nmh, LRESULT*)
{
	LPNMQHTM pnm = reinterpret_cast<LPNMQHTM>( nmh );
	if( pnm->pcszLinkText )
	{
		pnm->resReturnValue = FALSE;
		LONG				lLinkID;

		lLinkID = _ttol(pnm->pcszLinkText);
		CallStackingParameters(lLinkID);
	}
}

/* ------------------------------------------------------------------- */

void CStackRecap::OnBnClickedRecommandedsettings()
{
	CRecommendedSettings		dlg;

	dlg.SetStackingTasks(m_pStackingTasks);
	if ((dlg.DoModal()==IDOK) && m_pStackingTasks)
	{
		m_pStackingTasks->UpdateTasksMethods();
		FillWithAllTasksHTML();
	};
}

/* ------------------------------------------------------------------- */

void CStackRecap::OnBnClickedOk()
{
	SaveWindowPosition(this, REGENTRY_BASEKEY_DEEPSKYSTACKER_STEPS_POSITION);

	OnOK();
}

/* ------------------------------------------------------------------- */

void CStackRecap::OnBnClickedCancel()
{
	SaveWindowPosition(this, REGENTRY_BASEKEY_DEEPSKYSTACKER_STEPS_POSITION);

	OnCancel();
}

/* ------------------------------------------------------------------- */
