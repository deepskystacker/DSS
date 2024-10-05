/****************************************************************************
**
** Copyright (C) 2024 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
// StarMaskDlg.cpp : implementation file
//
#include "stdafx.h"
#include "StarMaskDlg.h"

#include "DSSCommon.h"

#define dssApp DeepSkyStacker::instance()
namespace DSS
{
	StarMaskDlg::StarMaskDlg(QWidget* parent)
		: BaseDialog(BaseDialog::Behaviour::PersistGeometry, parent)
	{
		setupUi(this);
		QSettings settings;

		//
		// Initialise the combo-box for the output star shape and
		// select the preview image
		//
		QStringList starShapes
		{
			tr("Bell"),
			tr("Truncated Bell"),
			tr("Cone"),
			tr("Truncated Cone"),
			tr("Cubic"),
			tr("Quadric")
		};
		starShape->addItems(starShapes);
		const auto currentShape{ settings.value("StarMask/StarShape", 0).toInt() };
		starShape->setCurrentIndex(currentShape);

		//
		// Now display a preview of the selected star shape
		//
		setStarShapePreview(currentShape);

		//
		// Set the other controls
		//
		uint value = 0;
		value = settings.value("StarMask/DetectionThreshold", 10).toUInt();
		thresholdSlider->setValue(value);
		thresholdChanged(value);

		detectHotPixels->setChecked(settings.value("StarMask/DetectHotPixels", false).toBool());

		value = settings.value("StarMask/MinSize", 2).toUInt();
		minSizeSlider->setValue(value);
		minSizeChanged(value);

		value = settings.value("StarMask/MaxSize", 25).toUInt();
		maxSizeSlider->setValue(value);
		maxSizeChanged(value);

		value = settings.value("StarMask/PercentRadius", 100).toUInt();
		percentSlider->setValue(value);
		percentChanged(value);

		value = settings.value("StarMask/PixelIncrease", 0).toUInt();
		pixelsSlider->setValue(value);
		pixelsChanged(value);
		
		connectSignalsToSlots();
	}

	void StarMaskDlg::connectSignalsToSlots()
	{
		connect(starShape, &QComboBox::currentIndexChanged, this, &StarMaskDlg::setStarShapePreview);
		connect(thresholdSlider, &QSlider::valueChanged, this, &StarMaskDlg::thresholdChanged);
		connect(minSizeSlider, &QSlider::valueChanged, this, &StarMaskDlg::minSizeChanged);
		connect(maxSizeSlider, &QSlider::valueChanged, this, &StarMaskDlg::maxSizeChanged);
		connect(percentSlider, &QSlider::valueChanged, this, &StarMaskDlg::percentChanged);
		connect(pixelsSlider, &QSlider::valueChanged, this, &StarMaskDlg::pixelsChanged);
	}

	//
	// Slots
	//

	void StarMaskDlg::setStarShapePreview(int index)
	{
		QPixmap pix;
		switch (index)
		{
		case 0:
			pix.load(":/starmask/StarShape_Bell.bmp");
			break;
		case 1:
			pix.load(":/starmask/StarShape_TruncatedBell.bmp");
			break;
		case 2:
			pix.load(":/starmask/StarShape_Cone.bmp");
			break;
		case 3:
			pix.load(":/starmask/StarShape_TruncatedCone.bmp");
			break;
		case 4:
			pix.load(":/starmask/StarShape_Cubic.bmp");
			break;
		case 5:
			pix.load(":/starmask/StarShape_Quadratic.bmp");
			break;
		}
		starShapePreview->setPixmap(pix);
	}

	void StarMaskDlg::thresholdChanged(int value)
	{
		threshold->setText(tr("%1%", "").arg(value));
	}

	void StarMaskDlg::minSizeChanged(int value)
	{
		minSize->setText(tr("%n pixel(s)", "", value));
	}

	void StarMaskDlg::maxSizeChanged(int value)
	{
		maxSize->setText(tr("%n pixel(s)", "", value));
	}

	void StarMaskDlg::percentChanged(int value)
	{
		percent->setText(tr("%1%", "").arg(value));
	}

	void StarMaskDlg::pixelsChanged(int value)
	{
		pixels->setText(tr("%n pixel(s)", "", value));
	}

}
#if (0)
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
#endif