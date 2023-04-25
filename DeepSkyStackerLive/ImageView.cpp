// ExplorerBar.cpp : implementation file
//

#include "stdafx.h"
#include "ImageView.h"
#include "LiveSettings.h"
#include "DeepSkyStackerLiveDlg.h"

/* ------------------------------------------------------------------- */
// CImageViewTab dialog

IMPLEMENT_DYNAMIC(CImageViewTab, CDialog)

CImageViewTab::CImageViewTab(CWnd* pParent /*=nullptr*/, bool bDarkMode /*=false*/)
	: CDialog(CImageViewTab::IDD, pParent),
	m_bDarkMode(bDarkMode),
	m_Picture(bDarkMode)
{
	m_bStackedImage = FALSE;
}

/* ------------------------------------------------------------------- */

CImageViewTab::~CImageViewTab()
{
}

/* ------------------------------------------------------------------- */

void CImageViewTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILENAME_DSSLIVE, m_FileName);
	DDX_Control(pDX, IDC_COPYTOCLIPBOARD, m_CopyToClipboard);
	DDX_Control(pDX, IDC_PICTURE, m_PictureStatic);
	DDX_Control(pDX, IDC_GAMMA, m_Gamma);
	DDX_Control(pDX, IDC_BACKGROUND, m_Background);
	DDX_Control(pDX, IDC_4CORNERS, m_4Corners);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CImageViewTab, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(GC_PEGMOVE, IDC_GAMMA, OnChangeGamma)
	ON_NOTIFY(GC_PEGMOVED, IDC_GAMMA, OnChangeGamma)
	ON_NOTIFY(NM_LINKCLICK, IDC_FILENAME_DSSLIVE, OnFileName)
	ON_NOTIFY(NM_LINKCLICK, IDC_COPYTOCLIPBOARD, OnCopyToClipboard)
	ON_BN_CLICKED(IDC_4CORNERS, OnBnClicked4corners)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CImageViewTab::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_ControlPos.MoveControls();
}

/* ------------------------------------------------------------------- */

BOOL CImageViewTab::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ControlPos.SetParent(this);

	m_Picture.CreateFromStatic(&m_PictureStatic);

	if(m_bDarkMode)
		m_Background.SetBkColor(RGB(112, 122, 126), RGB(69, 93, 121), CLabel::Gradient);
	else
		m_Background.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

	m_ControlPos.AddControl(IDC_BACKGROUND, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_FILENAME_DSSLIVE, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_PICTURE, CP_RESIZE_HORIZONTAL | CP_RESIZE_VERTICAL);
	m_ControlPos.AddControl(IDC_GAMMA, CP_MOVE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_4CORNERS, CP_MOVE_HORIZONTAL);

	m_Picture.SetBltMode(CWndImage::bltFitXY);
	m_Picture.SetAlign(CWndImage::bltCenter, CWndImage::bltCenter);
	m_Picture.EnableZoom(TRUE);

	m_FileName.SetTransparent(TRUE);

	m_CopyToClipboard.ShowWindow(SW_HIDE);
	m_CopyToClipboard.SetTransparent(TRUE);
	m_CopyToClipboard.SetLinkCursor(LoadCursor(nullptr,IDC_HAND));
	m_CopyToClipboard.SetLink(TRUE, TRUE);
	m_CopyToClipboard.SetTextColor(m_bDarkMode ? RGB(0, 0, 64) : RGB(0, 0, 128));

	m_Gamma.SetBackgroundColor(m_bDarkMode ? COLORREF(RGB(80,80,80)) : GetSysColor(COLOR_3DFACE));
	m_Gamma.ShowTooltips(FALSE);
	m_Gamma.SetOrientation(CGradientCtrl::ForceHorizontal);
	m_Gamma.SetPegSide(TRUE, FALSE);
	m_Gamma.SetPegSide(FALSE, TRUE);
	m_Gamma.GetGradient().SetStartPegColour(RGB(0, 0, 0));
	m_Gamma.GetGradient().AddPeg(RGB(0, 0, 0), 0.0, 0);
	m_Gamma.GetGradient().AddPeg(RGB(128, 128, 128), sqrt(0.5), 1);
	m_Gamma.GetGradient().AddPeg(RGB(255, 255, 255), 1.0, 2);
	m_Gamma.GetGradient().SetEndPegColour(RGB(255, 255, 255));
	if(m_bDarkMode)
		m_Gamma.GetGradient().SetBackgroundColour(RGB(128, 128, 128));
	else
		m_Gamma.GetGradient().SetBackgroundColour(RGB(255, 255, 255));
	m_Gamma.GetGradient().SetInterpolationMethod(CGradient::Linear);

	m_4Corners.SetBitmaps(IDB_4CORNERS, RGB(255,0, 255));
	if (m_bDarkMode)
	{
		m_4Corners.SetColor(CButtonST::BTNST_COLOR_BK_IN, COLORREF(RGB(80, 80, 80)));
		m_4Corners.SetColor(CButtonST::BTNST_COLOR_BK_OUT, COLORREF(RGB(80, 80, 80)));
		m_4Corners.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, COLORREF(RGB(80, 80, 80)));
	}
	m_4Corners.SetFlat(TRUE);

	CString				strText;

	if (m_bStackedImage)
	{
		strText.LoadString(IDS_NOSTACKEDIMAGE);
		m_Picture.SetImageSink(&m_StackedSink);
	}
	else
		strText.LoadString(IDS_NOIMAGELOADED);
	m_FileName.SetText(strText);

	return TRUE;
}

/* ------------------------------------------------------------------- */

void CImageViewTab::OnChangeGamma(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
		*pResult = 1;

	PegNMHDR *			pPegNMHDR = (PegNMHDR*)pNMHDR;
	double				fBlackPoint { 0.0 },
						fGrayPoint { 0.0 },
						fWhitePoint{ 0.0 };

	if ((pPegNMHDR->nmhdr.code == GC_PEGMOVE) ||
		(pPegNMHDR->nmhdr.code == GC_PEGMOVED))
	{
		// Adjust
		CGradient &			Gradient = m_Gamma.GetGradient();
		fBlackPoint = Gradient.GetPeg(Gradient.IndexFromId(0)).position;
		fGrayPoint  = Gradient.GetPeg(Gradient.IndexFromId(1)).position;
		fWhitePoint = Gradient.GetPeg(Gradient.IndexFromId(2)).position;
		BOOL				bAdjust = FALSE;

		switch (pPegNMHDR->peg.id)
		{
		case 0 :
			// Black point moving
			if (fBlackPoint>fWhitePoint-0.02)
			{
				fBlackPoint = fWhitePoint-0.02;
				bAdjust = TRUE;
			};
			if (fBlackPoint>fGrayPoint-0.01)
			{
				fGrayPoint = fBlackPoint+0.01;
				bAdjust = TRUE;
			};
			break;
		case 1 :
			// Gray point moving
			if (fGrayPoint<fBlackPoint+0.01)
			{
				fGrayPoint = fBlackPoint+0.01;
				bAdjust = TRUE;
			};
			if (fGrayPoint>fWhitePoint-0.01)
			{
				fGrayPoint = fWhitePoint-0.01;
				bAdjust = TRUE;
			};
			break;
		case 2 :
			// White point moving
			if (fWhitePoint<fBlackPoint+0.02)
			{
				fWhitePoint = fBlackPoint+0.02;
				bAdjust = TRUE;
			};
			if (fWhitePoint < fGrayPoint+0.01)
			{
				fGrayPoint = fWhitePoint-0.01;
				bAdjust = TRUE;
			};
			break;
		};
		if (bAdjust)
		{
			Gradient.SetPeg(Gradient.IndexFromId(0), (float)fBlackPoint);
			Gradient.SetPeg(Gradient.IndexFromId(1), (float)fGrayPoint);
			Gradient.SetPeg(Gradient.IndexFromId(2), (float)fWhitePoint);
			m_Gamma.InvalidateRect(nullptr);
		};
	};

	if (pPegNMHDR->nmhdr.code == GC_PEGMOVED)
	{
		// Adjust Gamma
		m_GammaTransformation.initTransformation(fBlackPoint*fBlackPoint, fGrayPoint*fGrayPoint, fWhitePoint*fWhitePoint);

		if (m_pWndImage)
		{
			ApplyGammaTransformation(m_pWndImage.get(), m_pBitmap.get(), m_GammaTransformation);
			// Refresh
			m_Picture.Invalidate(TRUE);
		};
	};
};

/* ------------------------------------------------------------------- */

void CImageViewTab::SetImage(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap, LPCTSTR szFileName)
{
	CString						strText;

	strText = szFileName;
	if(m_bDarkMode)
		m_FileName.SetBkColor(RGB(112, 122, 126), RGB(69, 93, 121), CLabel::Gradient);
	else
		m_FileName.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

	if (pBitmap)
	{
		m_Picture.SetImg(pWndBitmap->GetHBITMAP(), true);
		m_pWndImage = pWndBitmap;
		m_pBitmap	= pBitmap;
		if (m_GammaTransformation.isInitialized())
			ApplyGammaTransformation(m_pWndImage.get(), m_pBitmap.get(), m_GammaTransformation);

		m_Picture.Invalidate(TRUE);
		if (m_bStackedImage)
		{
			strText.LoadString(IDS_SAVESTACKEDIMAGE);
			m_FileName.SetText(strText);
			m_FileName.SetLink(TRUE, TRUE);
		}
		else
			m_FileName.SetText(strText);
		m_CopyToClipboard.ShowWindow(SW_SHOW);
	}
	else
	{
		m_CopyToClipboard.ShowWindow(SW_HIDE);
		if (m_bStackedImage)
		{
			strText.LoadString(IDS_NOIMAGELOADED);
			m_FileName.SetLink(FALSE, FALSE);
		}
		else
			strText.LoadString(IDS_NOSTACKEDIMAGE);
		m_StackedSink.ClearFootprint();
		m_Picture.SetImg((HBITMAP)nullptr);
		m_FileName.SetText(strText);
		m_pWndImage.reset();
		m_pBitmap.reset();
	};
};

/* ------------------------------------------------------------------- */

void CImageViewTab::OnFileName([[maybe_unused]] NMHDR* pNMHDR, [[maybe_unused]] LRESULT* pResult)
{
	CLiveSettings		LiveSettings;
	CString				strOutputFolder;

	LiveSettings.LoadFromRegistry();

	LiveSettings.GetStackedOutputFolder(strOutputFolder);
	if (strOutputFolder.GetLength())
	{
		PostSaveStackedImage();
		UpdateLiveSettings();

		CString			strText;

		strText.LoadString(IDS_STACKEDIMAGEWILLBESAVED);
		m_FileName.SetText(strText);
		m_FileName.SetLink(FALSE, FALSE);
		if(m_bDarkMode)
			m_FileName.SetBkColor(RGB(126, 126, 111), RGB(128, 76, 77), CLabel::Gradient);
		else
			m_FileName.SetBkColor(RGB(252, 251, 222), RGB(255, 151, 154), CLabel::Gradient);
	}
	else
	{
		CString			strText;

		strText.LoadString(IDS_NOSTACKEDIMAGEFOLDER);
		AfxMessageBox(strText, MB_OK | MB_ICONINFORMATION);
	};
};

/* ------------------------------------------------------------------- */

void CImageViewTab::OnStackedImageSaved()
{
	CString				strText;
	if (m_bDarkMode)
		m_FileName.SetBkColor(RGB(112, 122, 126), RGB(69, 92, 121), CLabel::Gradient);
	else
		m_FileName.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

	strText.LoadString(IDS_STACKEDIMAGESAVED);
	m_FileName.SetText(strText);
	m_FileName.SetLink(FALSE, FALSE);
};

/* ------------------------------------------------------------------- */

void CImageViewTab::OnCopyToClipboard([[maybe_unused]] NMHDR* pNMHDR, [[maybe_unused]] LRESULT* pResult)
{
	if (m_pWndImage.get())
		m_pWndImage->CopyToClipboard();
};

/* ------------------------------------------------------------------- */

void CImageViewTab::OnSetFootprint(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4)
{
	m_StackedSink.SetFootprint(pt1, pt2, pt3, pt4);
	m_Picture.Invalidate();
};

/* ------------------------------------------------------------------- */

void CImageViewTab::OnBnClicked4corners()
{
	m_Picture.Set4CornersMode(!m_Picture.Get4CornersMode());
}

/* ------------------------------------------------------------------- */

BOOL CImageViewTab::OnEraseBkgnd(CDC* pDC)
{
	if (!m_bDarkMode)
		return CDialog::OnEraseBkgnd(pDC);

	CRect rect;
	GetClientRect(&rect);
	CBrush myBrush(RGB(80, 80, 80));    // dialog background color
	CBrush *pOld = pDC->SelectObject(&myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOld);    // restore old brush
	return bRes;                       // CDialog::OnEraseBkgnd(pDC);
}
