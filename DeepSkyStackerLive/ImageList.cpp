// ExplorerBar.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "DeepSkyStackerLiveDlg.h"
#include "ImageList.h"
#include "RegisterEngine.h"

const DWORD		COLUMN_STACKED	= 0;
const DWORD 	COLUMN_FILE		= 1;
const DWORD		COLUMN_EXPOSURE = 2;
const DWORD 	COLUMN_SCORE	= 3;
const DWORD		COLUMN_STARS	= 4;
const DWORD		COLUMN_FWHM		= 5;
const DWORD 	COLUMN_DX		= 6;
const DWORD 	COLUMN_DY		= 7;
const DWORD 	COLUMN_ANGLE	= 8;
const DWORD		COLUMN_FILETIME = 9;
const DWORD		COLUMN_SIZES	= 10;
const DWORD		COLUMN_CFA		= 11;
const DWORD		COLUMN_DEPTH	= 12;
const DWORD		COLUMN_INFO		= 13;
const DWORD		COLUMN_ISO		= 14;
const DWORD		COLUMN_SKYBACKGROUND = 15;


/* ------------------------------------------------------------------- */
// CImageListTab dialog

IMPLEMENT_DYNAMIC(CImageListTab, CDialog)

CImageListTab::CImageListTab(CWnd* pParent /*=NULL*/)
	: CDialog(CImageListTab::IDD, pParent)
{
}

/* ------------------------------------------------------------------- */

CImageListTab::~CImageListTab()
{
}

/* ------------------------------------------------------------------- */

void CImageListTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGELIST, m_ImageList);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CImageListTab, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CImageListTab::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_ControlPos.MoveControls();
}

/* ------------------------------------------------------------------- */

void CImageListTab::InitList()
{
	CString				strColumn;

	m_ImageList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

	m_ImageList.InsertColumn(COLUMN_STACKED, _T(""), LVCFMT_LEFT, 50);

	strColumn.LoadString(IDS_COLUMN_FILE);
	m_ImageList.InsertColumn(COLUMN_FILE, strColumn, LVCFMT_LEFT, 200);
	strColumn.LoadString(IDS_COLUMN_EXPOSURE);
	m_ImageList.InsertColumn(COLUMN_EXPOSURE, strColumn, LVCFMT_RIGHT, 50);
	strColumn.LoadString(IDS_COLUMN_SCORE);
	m_ImageList.InsertColumn(COLUMN_SCORE, strColumn, LVCFMT_RIGHT, 80);
	strColumn.LoadString(IDS_COLUMN_STARS);
	m_ImageList.InsertColumn(COLUMN_STARS, strColumn, LVCFMT_RIGHT, 50);
	m_ImageList.InsertColumn(COLUMN_FWHM, _T("FWHM"), LVCFMT_RIGHT, 50);
	strColumn.LoadString(IDS_COLUMN_DX);
	m_ImageList.InsertColumn(COLUMN_DX, strColumn, LVCFMT_RIGHT, 60);
	strColumn.LoadString(IDS_COLUMN_DY);
	m_ImageList.InsertColumn(COLUMN_DY, strColumn, LVCFMT_RIGHT, 60);
	strColumn.LoadString(IDS_COLUMN_ANGLE);
	m_ImageList.InsertColumn(COLUMN_ANGLE, strColumn, LVCFMT_RIGHT, 60);
	strColumn.LoadString(IDS_COLUMN_DATETIME);
	m_ImageList.InsertColumn(COLUMN_FILETIME, strColumn, LVCFMT_RIGHT, 120);
	strColumn.LoadString(IDS_COLUMN_SIZES);
	m_ImageList.InsertColumn(COLUMN_SIZES, strColumn, LVCFMT_RIGHT, 100);
	strColumn.LoadString(IDS_COLUMN_CFA);
	m_ImageList.InsertColumn(COLUMN_CFA, strColumn, LVCFMT_CENTER, 30);
	strColumn.LoadString(IDS_COLUMN_DEPTH);
	m_ImageList.InsertColumn(COLUMN_DEPTH, strColumn, LVCFMT_LEFT, 50);
	strColumn.LoadString(IDS_COLUMN_INFOS);
	m_ImageList.InsertColumn(COLUMN_INFO, strColumn, LVCFMT_LEFT, 50);
	strColumn.LoadString(IDS_COLUMN_ISO);
	m_ImageList.InsertColumn(COLUMN_ISO, strColumn, LVCFMT_RIGHT, 50);
	strColumn.LoadString(IDS_COLUMN_SKYBACKGROUND);
	m_ImageList.InsertColumn(COLUMN_SKYBACKGROUND, strColumn, LVCFMT_RIGHT, 50);

	m_ImageList.RestoreState(_T("LivePictureList"), _T("Settings"));
};

/* ------------------------------------------------------------------- */

BOOL CImageListTab::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ControlPos.SetParent(this);

	m_ControlPos.AddControl(IDC_IMAGELIST, CP_RESIZE_HORIZONTAL | CP_RESIZE_VERTICAL);
	InitList();

	return TRUE;  
}

/* ------------------------------------------------------------------- */

BOOL CImageListTab::Close()
{
	m_ImageList.SaveState(_T("LivePictureList"), _T("Settings"));

	return TRUE;
};

/* ------------------------------------------------------------------- */

void CImageListTab::AddImage(LPCTSTR szImage)
{
	CLightFrameInfo			lfi;
	CBitmapInfo				bmpInfo;

	GetPictureInfo(szImage, bmpInfo);
	lfi.SetBitmap(szImage, FALSE, FALSE);
	if (lfi.IsRegistered() && bmpInfo.CanLoad())
	{
		// Add the file to the list
		int					nItem;
		CString				strImage;

		TCHAR				szName[_MAX_FNAME];
		TCHAR				szExt[_MAX_EXT];

		_tsplitpath(szImage, NULL, NULL, szName, szExt);
		strImage.Format(_T("%s%s"), szName, szExt);

		nItem = m_ImageList.InsertItem(m_ImageList.GetItemCount(), _T(""));
		m_ImageList.SetItemText(nItem, COLUMN_FILE, (LPCTSTR)strImage);

		CString				strSizes;
		CString				strDepth;
		CString				strInfos;

		strSizes.Format(_T("%ld x %ld"), bmpInfo.m_lWidth, bmpInfo.m_lHeight);
		m_ImageList.SetItemText(nItem, COLUMN_SIZES, (LPCTSTR)strSizes);

		if (lfi.m_lNrChannels == 3)
			strDepth.Format(IDS_FORMAT_RGB, bmpInfo.m_lBitPerChannel);
		else
			strDepth.Format(IDS_FORMAT_GRAY, bmpInfo.m_lBitPerChannel);
		m_ImageList.SetItemText(nItem, COLUMN_DEPTH, (LPCTSTR)strDepth);

		CString				strText;

		ISOToString(bmpInfo.m_lISOSpeed, strText);
		m_ImageList.SetItemText(nItem, COLUMN_ISO, (LPCTSTR)strText);

		ExposureToString(bmpInfo.m_fExposure, strText);
		m_ImageList.SetItemText(nItem, COLUMN_EXPOSURE, (LPCTSTR)strText);

		strText.Format(_T("%ld"), lfi.m_vStars.size());
		m_ImageList.SetItemText(nItem, COLUMN_STARS, (LPCTSTR)strText);

		strText.Format(_T("%.2f"), lfi.m_fOverallQuality);
		m_ImageList.SetItemText(nItem, COLUMN_SCORE, (LPCTSTR)strText);

		strText.Format(_T("%.2f"), lfi.m_fFWHM);
		m_ImageList.SetItemText(nItem, COLUMN_FWHM, (LPCTSTR)strText);

		m_ImageList.SetItemText(nItem, COLUMN_FILETIME, bmpInfo.m_strDateTime);

		strText.Format(_T("%.2f %%"), lfi.m_SkyBackground.m_fLight*100.0);
		m_ImageList.SetItemText(nItem, COLUMN_SKYBACKGROUND, (LPCTSTR)strText);

		strText = bmpInfo.m_strFileType;
		if (bmpInfo.m_strModel.GetLength())
			strText += " "+bmpInfo.m_strModel;
		m_ImageList.SetItemText(nItem, COLUMN_INFO, strText);

		if (bmpInfo.m_CFAType == CFATYPE_NONE)
			strText.LoadString(IDS_NO);
		else
			strText.LoadString(IDS_YES);
		m_ImageList.SetItemText(nItem, COLUMN_CFA, strText);

		AddScoreFWHMStarsToGraph(lfi.m_strFileName, lfi.m_fOverallQuality, lfi.m_fFWHM, lfi.m_vStars.size(), lfi.m_SkyBackground.m_fLight*100.0);
	};
};

/* ------------------------------------------------------------------- */

void	CImageListTab::ChangeImageStatus(LPCTSTR szImage, IMAGESTATUS status)
{
	CString				strImage;

	TCHAR				szName[_MAX_FNAME];
	TCHAR				szExt[_MAX_EXT];

	_tsplitpath(szImage, NULL, NULL, szName, szExt);
	strImage.Format(_T("%s%s"), szName, szExt);

	// Search the image in the list
	BOOL				bFound = FALSE;

	for (LONG i = 0;i<m_ImageList.GetItemCount() && !bFound;i++)
	{
		CString			strTemp;
		CString			strStatus;

		strTemp = m_ImageList.GetItemText(i, COLUMN_FILE);

		if (!strTemp.CompareNoCase(strImage))
		{
			bFound = TRUE;
			switch (status)
			{
			case IS_LOADED :
				strStatus.LoadString(IDS_STATUS_LOADED);
				break;
			case IS_REGISTERED :
				strStatus.LoadString(IDS_STATUS_REGISTERED);
				break;
			case IS_STACKDELAYED :
				strStatus.LoadString(IDS_STATUS_STACKDELAYED);
				break;
			case IS_NOTSTACKABLE :
				strStatus.LoadString(IDS_STATUS_NOTSTACKABLE);
				break;
			case IS_STACKED :
				strStatus.LoadString(IDS_STATUS_STACKED);
				break;
			};
			m_ImageList.SetItemText(i, COLUMN_STACKED, strStatus);
		};
	};
};

/* ------------------------------------------------------------------- */

void CImageListTab::UpdateImageOffsets(LPCTSTR szImage, double fdX, double fdY, double fAngle)
{
	CString				strImage;
	TCHAR				szName[_MAX_FNAME];
	TCHAR				szExt[_MAX_EXT];

	_tsplitpath(szImage, NULL, NULL, szName, szExt);
	strImage.Format(_T("%s%s"), szName, szExt);

	// Search the image in the list
	BOOL				bFound = FALSE;

	for (LONG i = 0;i<m_ImageList.GetItemCount() && !bFound;i++)
	{
		CString			strTemp;
		CString			strValue;

		strTemp = m_ImageList.GetItemText(i, COLUMN_FILE);

		if (!strTemp.CompareNoCase(strImage))
		{
			bFound = TRUE;
			strValue.Format(_T("%.2f"), fdX);
			m_ImageList.SetItemText(i, COLUMN_DX, strValue);
			strValue.Format(_T("%.2f"), fdY);
			m_ImageList.SetItemText(i, COLUMN_DY, strValue);
			strValue.Format(_T("%.2f °"), fAngle);
			m_ImageList.SetItemText(i, COLUMN_ANGLE, strValue);
		};
	};
};

/* ------------------------------------------------------------------- */

