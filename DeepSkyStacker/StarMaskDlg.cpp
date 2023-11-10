// StarMaskDlg.cpp : implementation file
//
#include "stdafx.h"
#include "StarMaskDlg.h"
#include "DSSCommon.h"

extern CString STARMASKFILE_FILTERS;
// CStarMaskDlg dialog
/* ------------------------------------------------------------------- */

class CSaveMaskDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CSaveMaskDlg)

public :
	CSaveMaskDlg(bool bOpenFileDialog, // true for FileOpen, false for FileSaveAs
		LPCTSTR lpszDefExt = nullptr,
		LPCTSTR lpszFileName = nullptr,
		std::uint32_t dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = nullptr,
		CWnd* pParentWnd = nullptr):CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
	{
	};
	virtual ~CSaveMaskDlg()
	{
	};

//protected:
//	virtual void OnTypeChange()
//	{
//		CFileDialog::OnTypeChange();
//		CString			strFileName = GetFileTitle();
//
//		if (strFileName.GetLength())
//		{
//			if (m_ofn.nFilterIndex == 1)
//				strFileName += ".tif";
//			else
//				strFileName += ".fits";
//			SetControlText(FILE_DIALOG_NAME, strFileName);
//		};
//	};
};
IMPLEMENT_DYNAMIC(CSaveMaskDlg, CFileDialog)

/* ------------------------------------------------------------------- */

IMPLEMENT_DYNAMIC(CStarMaskDlg, CDialog)

/* ------------------------------------------------------------------- */

CStarMaskDlg::CStarMaskDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CStarMaskDlg::IDD, pParent)
{
	m_bOutputFITS = false;
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
	int				lPos;
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
	m_MaxSize.SetRange(10, 2*STARMAXSIZE);
	m_Percent.SetRange(10, 200);
	m_Pixels.SetRange(0, 10);

	QSettings	settings;

	const auto dwStarShape = settings.value("StarMask/StarShape", 0).toUInt();
	m_StarShape.SetCurSel(dwStarShape);

	bool bHotPixels = settings.value("StarMask/DetectHotPixels", false).toBool();
	m_HotPixels.SetCheck(bHotPixels);

	const auto dwThreshold = settings.value("StarMask/DetectionThreshold", 10).toUInt();
	m_StarThreshold.SetPos(dwThreshold);

	const auto dwPercent = settings.value("StarMask/PercentRadius", 100).toUInt();
	m_Percent.SetPos(dwPercent);

	const auto dwPixel = settings.value("StarMask/PixelIncrease", 0).toUInt();
	m_Pixels.SetPos(dwPixel);

	const auto dwMinSize = settings.value("StarMask/MinSize", 2).toUInt();
	m_MinSize.SetPos(dwMinSize);

	const auto dwMaxSize = settings.value("StarMask/MaxSize", 25).toUInt();
	m_MaxSize.SetPos(dwMaxSize);

	UpdateTexts();
	UpdateStarShapePreview();

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

/* ------------------------------------------------------------------- */

bool CStarMaskDlg::AskOutputFile()
{
	bool					bResult = false;
	CString					strTitle;
	QSettings				settings;

	auto dwFileType = settings.value("StarMask/FileType", 0).toUInt();

	strTitle.LoadString(IDS_TITLE_MASK);

	CSaveMaskDlg			dlgSave(false,
								nullptr,
								nullptr,
								OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
								STARMASKFILE_FILTERS,
								this);

	// Get Base directory from base output file
	CString					strBaseDirectory;
	TCHAR					szDrive[1+_MAX_DRIVE];
	TCHAR					szDir[1+_MAX_DIR];

	_tsplitpath(m_strOutputFile, szDrive, szDir, nullptr, nullptr);
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
			settings.setValue("StarMask/FileType", (uint)dwFileType);
			bResult = true;
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
		QSettings			settings;

		const auto dwStarShape = m_StarShape.GetCurSel();
		settings.setValue("StarMask/StarShape", static_cast<uint>(dwStarShape));

		const bool bHotPixels = m_HotPixels.GetCheck() ? 1 : 0;
		settings.setValue("StarMask/DetectHotPixels", bHotPixels);

		const auto dwThreshold = m_StarThreshold.GetPos();
		settings.setValue("StarMask/DetectionThreshold", static_cast<uint>(dwThreshold));

		const auto dwPercent = m_Percent.GetPos();
		settings.setValue("StarMask/PercentRadius", static_cast<uint>(dwPercent));

		const auto dwPixel = m_Pixels.GetPos();
		settings.setValue("StarMask/PixelIncrease", static_cast<uint>(dwPixel));

		const auto dwMinSize = m_MinSize.GetPos();
		settings.setValue("StarMask/MinSize", static_cast<uint>(dwMinSize));

		const auto dwMaxSize = m_MaxSize.GetPos();
		settings.setValue("StarMask/MaxSize", static_cast<uint>(dwMaxSize));

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
