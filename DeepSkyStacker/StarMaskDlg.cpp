// StarMaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "StarMaskDlg.h"
#include "Registry.h"


// CStarMaskDlg dialog
/* ------------------------------------------------------------------- */

class CSaveMaskDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CSaveMaskDlg)

public :
	CSaveMaskDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL):CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
	{
	};
	virtual ~CSaveMaskDlg()
	{
	};

protected:
	virtual void OnTypeChange()
	{
		CFileDialog::OnTypeChange();
		CString			strFileName = GetFileTitle();

		if (strFileName.GetLength())
		{
			if (m_ofn.nFilterIndex == 1)
				strFileName += ".tif";
			else
				strFileName += ".fits";
			SetControlText(0x047c, strFileName);
		};
	};
};
IMPLEMENT_DYNAMIC(CSaveMaskDlg, CFileDialog)

/* ------------------------------------------------------------------- */

IMPLEMENT_DYNAMIC(CStarMaskDlg, CDialog)

/* ------------------------------------------------------------------- */

CStarMaskDlg::CStarMaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStarMaskDlg::IDD, pParent)
{
	m_bOutputFITS = FALSE;
}

/* ------------------------------------------------------------------- */

CStarMaskDlg::~CStarMaskDlg()
{
}

/* ------------------------------------------------------------------- */

void CStarMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARSHAPE, m_StarShape);
	DDX_Control(pDX, IDC_STARSHAPEPREVIEW, m_StarShapePreview);
	DDX_Control(pDX, IDC_STARTHRESHOLDTEXT, m_StarThresholdText);
	DDX_Control(pDX, IDC_STARTHRESHOLD, m_StarThreshold);
	DDX_Control(pDX, IDC_HOTPIXELS, m_HotPixels);
	DDX_Control(pDX, IDC_MINSIZETEXT, m_MinSizeText);
	DDX_Control(pDX, IDC_MINSIZE, m_MinSize);
	DDX_Control(pDX, IDC_MAXSIZETEXT, m_MaxSizeText);
	DDX_Control(pDX, IDC_MAXSIZE, m_MaxSize);
	DDX_Control(pDX, IDC_PERCENTTEXT, m_PercentText);
	DDX_Control(pDX, IDC_PERCENT, m_Percent);
	DDX_Control(pDX, IDC_PIXELSTEXT, m_PixelsText);
	DDX_Control(pDX, IDC_PIXELS, m_Pixels);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CStarMaskDlg, CDialog)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_STARSHAPE, &OnStarShapeChange)
END_MESSAGE_MAP()


// CStarMaskDlg message handlers

/* ------------------------------------------------------------------- */

void CStarMaskDlg::UpdateStarShapePreview()
{
	switch (m_StarShape.GetCurSel())
	{
	case 0 :
		m_StarShapePreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_STARSHAPE_BELL)));
		break;
	case 1 :
		m_StarShapePreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_STARSHAPE_TRUNCATEDBELL)));
		break;
	case 2 :
		m_StarShapePreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_STARSHAPE_CONE)));
		break;
	case 3 :
		m_StarShapePreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_STARSHAPE_TRUNCATEDCONE)));
		break;
	case 4 :
		m_StarShapePreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_STARSHAPE_CUBIC)));
		break;
	case 5 :
		m_StarShapePreview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_STARSHAPE_QUADRIC)));
		break;
	};
};

/* ------------------------------------------------------------------- */

void CStarMaskDlg::UpdateTexts()
{
	// Update all the texts
	LONG				lPos;
	CString				strText;

	lPos = m_StarThreshold.GetPos();
	strText.Format(m_StarThresholdMask, lPos);
	m_StarThresholdText.SetWindowText(strText);

	lPos = m_MinSize.GetPos();
	strText.Format(m_MinSizeMask, lPos);
	m_MinSizeText.SetWindowText(strText);

	lPos = m_MaxSize.GetPos();
	strText.Format(m_MaxSizeMask, lPos);
	m_MaxSizeText.SetWindowText(strText);

	lPos = m_Percent.GetPos();
	strText.Format(m_PercentMask, lPos);
	m_PercentText.SetWindowText(strText);

	lPos = m_Pixels.GetPos();
	strText.Format(m_PixelsMask, lPos);
	m_PixelsText.SetWindowText(strText);
};

/* ------------------------------------------------------------------- */

BOOL CStarMaskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_StarThresholdText.GetWindowText(m_StarThresholdMask);
	m_MinSizeText.GetWindowText(m_MinSizeMask);
	m_MaxSizeText.GetWindowText(m_MaxSizeMask);
	m_PercentText.GetWindowText(m_PercentMask);
	m_PixelsText.GetWindowText(m_PixelsMask);

	m_StarThreshold.SetRange(2, 100);
	m_MinSize.SetRange(2, 10);
	m_MaxSize.SetRange(10, 40);
	m_Percent.SetRange(10, 200);
	m_Pixels.SetRange(0, 10);

	CRegistry			reg;
	DWORD				bHotPixels = 0;
	DWORD				dwThreshold = 10;
	DWORD				dwPercent = 100;
	DWORD				dwPixel = 0;
	DWORD				dwMinSize = 2;
	DWORD				dwMaxSize = 25;
	DWORD				dwStarShape = 0;

	reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("StarShape"), dwStarShape);
	m_StarShape.SetCurSel(dwStarShape);

	reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("DetectHotPixels"), bHotPixels);
	m_HotPixels.SetCheck(bHotPixels);

	reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("DetectionThreshold"), dwThreshold);
	m_StarThreshold.SetPos(dwThreshold);

	reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("PercentRadius"), dwPercent);
	m_Percent.SetPos(dwPercent);

	reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("PixelIncrease"), dwPixel);
	m_Pixels.SetPos(dwPixel);

	reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("MinSize"), dwMinSize);
	m_MinSize.SetPos(dwMinSize);

	reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("MaxSize"), dwMaxSize);
	m_MaxSize.SetPos(dwMaxSize);

	UpdateTexts();
	UpdateStarShapePreview();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

BOOL CStarMaskDlg::AskOutputFile()
{
	BOOL					bResult = FALSE;
	CString					strTitle;
	CRegistry				reg;
	DWORD					dwFileType = 0;

	reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("FileType"), dwFileType);

	strTitle.LoadString(IDS_TITLE_MASK);
	
	CSaveMaskDlg			dlgSave(FALSE, 
								NULL,
								NULL,
								OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
								STARMASKFILE_FILTERS,
								this);

	// Get Base directory from base output file
	CString					strBaseDirectory;
	TCHAR					szDrive[1+_MAX_DRIVE];
	TCHAR					szDir[1+_MAX_DIR];

	_tsplitpath(m_strOutputFile, szDrive, szDir, NULL, NULL);
	strBaseDirectory = szDrive;
	strBaseDirectory += szDir;

	if (strBaseDirectory.GetLength())
		dlgSave.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);

	TCHAR				szBigBuffer[20000];
	
	if (dwFileType==2)
		lstrcpy(szBigBuffer, _T("StarMask.fits"));
	else
		lstrcpy(szBigBuffer, _T("StarMask.tif"));

	dlgSave.GetOFN().lpstrFile = szBigBuffer;
	dlgSave.GetOFN().nMaxFile  = sizeof(szBigBuffer) / sizeof(szBigBuffer[0]);
	dlgSave.GetOFN().lpstrTitle = strTitle.GetBuffer(200);
	dlgSave.GetOFN().nFilterIndex = dwFileType;

	if (dlgSave.DoModal() == IDOK)
	{
		POSITION		pos;

		pos = dlgSave.GetStartPosition();
		if (pos)
		{
			m_strOutputFile = dlgSave.GetNextPathName(pos);

			dwFileType = dlgSave.GetOFN().nFilterIndex;
			reg.SaveKey(REGENTRY_BASEKEY_STARMASK, _T("FileType"), dwFileType);
			bResult = TRUE;
			m_bOutputFITS = (dwFileType == 2);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CStarMaskDlg::OnStarShapeChange( )
{
	UpdateStarShapePreview();
};

/* ------------------------------------------------------------------- */

void CStarMaskDlg::OnOK() 
{
	if (AskOutputFile())
	{
		CRegistry			reg;
		DWORD				dwStarShape = 0;
		DWORD				bHotPixels = 0;
		DWORD				dwThreshold = 10;
		DWORD				dwPercent = 100;
		DWORD				dwPixel = 0;
		DWORD				dwMinSize = 2;
		DWORD				dwMaxSize = 25;

		dwStarShape = m_StarShape.GetCurSel();
		reg.SaveKey(REGENTRY_BASEKEY_STARMASK, _T("StarShape"), dwStarShape);

		bHotPixels = m_HotPixels.GetCheck() ? 1 : 0;
		reg.SaveKey(REGENTRY_BASEKEY_STARMASK, _T("DetectHotPixels"), bHotPixels);

		dwThreshold = m_StarThreshold.GetPos();
		reg.SaveKey(REGENTRY_BASEKEY_STARMASK, _T("DetectionThreshold"), dwThreshold);

		dwPercent = m_Percent.GetPos();
		reg.SaveKey(REGENTRY_BASEKEY_STARMASK, _T("PercentRadius"), dwPercent);

		dwPixel = m_Pixels.GetPos();
		reg.SaveKey(REGENTRY_BASEKEY_STARMASK, _T("PixelIncrease"), dwPixel);

		dwMinSize = m_MinSize.GetPos();
		reg.SaveKey(REGENTRY_BASEKEY_STARMASK, _T("MinSize"), dwMinSize);

		dwMaxSize = m_MaxSize.GetPos();
		reg.SaveKey(REGENTRY_BASEKEY_STARMASK, _T("MaxSize"), dwMaxSize);

		CDialog::OnOK();
	};
}

/* ------------------------------------------------------------------- */

void CStarMaskDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateTexts();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/* ------------------------------------------------------------------- */
