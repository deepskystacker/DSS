// ExplorerBar.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "DeepSkyStackerLiveDlg.h"
#include "MainBoard.h"
#include "Registry.h"
#include <gdiplus.h>
#include "FolderDlg.h"
#include "RestartMonitoring.h"

#include <algorithm>
#include <FrameInfo.h>
#include <..\SMTP\PJNSMTP.h>

const	DWORD			WM_FOLDERCHANGE	= WM_USER+100;

/* ------------------------------------------------------------------- */
// CMainBoard dialog

IMPLEMENT_DYNAMIC(CMainBoard, CDialog)

CMainBoard::CMainBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CMainBoard::IDD, pParent)
{
	m_ulSHRegister = 0;
	m_bProgressing = FALSE;
	m_bMonitoring  = FALSE;
	m_bStacking	   = FALSE;
	m_bStopping	   = FALSE;

	m_lNrPending	= 0;
	m_lNrRegistered = 0;
	m_lNrStacked	= 0;
	m_fTotalExposureTime = 0;
	m_lNrEmails		= 0;

	m_LiveSettings.LoadFromRegistry();
}

/* ------------------------------------------------------------------- */

CMainBoard::~CMainBoard()
{
}

/* ------------------------------------------------------------------- */

void CMainBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MONITORINGRECT, m_MonitoringRect);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressRect);

	DDX_Control(pDX, IDC_STACKEDIMAGE, m_StackedImage);
	DDX_Control(pDX, IDC_LASTIMAGE, m_LastImage);
	DDX_Control(pDX, IDC_GRAPHS, m_Graphs);
	DDX_Control(pDX, IDC_IMAGELIST, m_ImageList);
	DDX_Control(pDX, IDC_LOGLIST, m_Log);
	DDX_Control(pDX, IDC_WARNINGS, m_Warnings);
	DDX_Control(pDX, IDC_MONITOREDFOLDER, m_MonitoredFolder);

	DDX_Control(pDX, IDC_MONITOR, m_Monitor);
	DDX_Control(pDX, IDC_STACK, m_Stack);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_STATS, m_Stats);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CMainBoard, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()

	ON_NOTIFY(NM_LINKCLICK, IDC_MONITOREDFOLDER, OnMonitoredFolder)
	ON_MESSAGE(WM_FOLDERCHANGE, OnFolderChange)
	ON_MESSAGE(WM_LIVEENGINE, OnLiveEngine)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
// CMainBoard message handlers

void CMainBoard::DrawGradientRect(CDC * pDC, const CRect & rc, COLORREF crColor1, COLORREF crColor2, double fAlpha)
{
	// Use GDI+
	#ifndef NOGDIPLUS
	Graphics *		pGraphics;
	Brush *			pBrush;
	
	pGraphics = new Graphics(pDC->GetSafeHdc());
	pBrush = new LinearGradientBrush(PointF(rc.left, rc.top), 
					PointF(rc.right, rc.bottom), 
					Color(fAlpha*255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 
					Color(fAlpha*255.0, GetRValue(crColor2), GetGValue(crColor2), GetBValue(crColor2)));
	pGraphics->FillRectangle(pBrush, RectF(rc.left, rc.top, rc.Width(), rc.Height()));
	delete pBrush;
	delete pGraphics;
	#endif // NOGDIPLUS
};

void CMainBoard::DrawGradientBackgroundRect(CDC * pDC, const CRect & rc)
{
//	DrawGradientRect(pDC, rc, RGB(224, 244, 252), RGB(138, 185, 242));

	#ifndef NOGDIPLUS
	Graphics *		pGraphics;
	Brush *			pBrush;
	COLORREF		crColor1 = RGB(70, 70, 70);
	COLORREF		crColor2 = RGB(230, 230, 230);
			
	
	pGraphics = new Graphics(pDC->GetSafeHdc());
	pBrush = new LinearGradientBrush(PointF(rc.right, rc.top), 
					PointF(rc.left, rc.bottom), 
					Color(GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 
					Color(GetRValue(crColor2), GetGValue(crColor2), GetBValue(crColor2)));
	pGraphics->FillRectangle(pBrush, RectF(rc.left, rc.top, rc.Width(), rc.Height()));

	delete pBrush;
	delete pGraphics;
	#endif // NOGDIPLUS
};

void CMainBoard::DrawGradientFrameRect(CDC * pDC, LPCTSTR szTitle, const CRect & rc, BOOL bActive, BOOL bShadow)
{
	CRect			rcTop;
	CRect			rcBottom;
	COLORREF		crColor1 = RGB(65, 153, 247);
	COLORREF		crColor2 = RGB(129, 13, 201);
	double			fAlpha = bShadow ? 0.8 : 1.0;

	if (bShadow)
	{
		crColor1 = RGB(127, 127, 127);
		crColor2 = RGB(127, 127, 127);
	}
	else if (!bActive)
	{
		crColor1 = RGB(65, 153, 247);
		crColor2 = RGB(129, 13, 201);
	};

	rcTop = rc;
	rcTop.bottom = rcTop.top + 20;
	rcBottom = rc;
	rcBottom.top = rcTop.bottom;
	if (bShadow)
	{
		rcTop.OffsetRect(5, 5);
		rcBottom.OffsetRect(5, 5);
	};
	DrawGradientRect(pDC, rcBottom, crColor2, crColor1, fAlpha);

	#ifndef NOGDIPLUS
	Graphics *		pGraphics;
	Brush *			pBrush;
	Pen *			pPen;
	GraphicsPath *	pPath;
	
	pGraphics = new Graphics(pDC->GetSafeHdc());
	pBrush = new LinearGradientBrush(PointF(rc.left, rc.top), 
					PointF(rc.right, rc.bottom), 
					Color(fAlpha*255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 
					Color(fAlpha*255.0, GetRValue(crColor2), GetGValue(crColor2), GetBValue(crColor2)));

	pPath = new GraphicsPath(FillModeWinding);
	CRect			rc1(rcTop.left, rcTop.top + 5, rcTop.right, rcTop.bottom), 
					rc2(rcTop.left+5, rcTop.top, rcTop.right-5, rcTop.bottom),
					rc3(rcTop.left, rcTop.top, rcTop.left+10, rcTop.top+10), 
					rc4(rcTop.right-10, rcTop.top, rcTop.right, rcTop.top+10);
	
	pPath->AddRectangle(RectF(rc1.left, rc1.top, rc1.Width(), rc1.Height()));
	pPath->AddRectangle(RectF(rc2.left, rc2.top, rc2.Width(), rc2.Height()));
	pPath->AddEllipse(RectF(rc3.left, rc3.top, rc3.Width(), rc3.Height()));
	pPath->AddEllipse(RectF(rc4.left, rc4.top, rc4.Width(), rc4.Height()));
	pPath->Flatten();

	pGraphics->FillPath(pBrush, pPath);
	pPath->AddRectangle(RectF(rcBottom.left, rcBottom.top, rcBottom.Width(), rcBottom.Height()));
	pPath->Flatten();

	pPen = new Pen(Color(fAlpha*255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 1.0);
	pPath->Outline();
	pGraphics->DrawPath(pPen, pPath);

	delete pPen;
	delete pPath;
	delete pBrush;

	if (!bShadow)
	{
		Font				font(FontFamily::GenericSansSerif(), 10, FontStyleRegular);
		StringFormat		format;
		PointF				pf(rc.left+6, rc.top+2);
		CString				strText = szTitle;
		SolidBrush			brush(Color(255, 255, 255));

		format.SetAlignment(StringAlignmentNear);
		format.SetLineAlignment(StringAlignmentNear);

		pGraphics->DrawString(CComBSTR(strText), -1, &font, pf, &format, &brush);
	};

	delete pGraphics;
	#endif // NOGDIPLUS
};

void CMainBoard::DrawSubFrameRect(CDC * pDC, const CRect & rc)
{
	COLORREF		crColor1 = RGB(255, 255, 255);
	COLORREF		crColor2 = RGB(200, 200, 200);
	double			fAlpha = 0.8;

	#ifndef NOGDIPLUS
	Graphics *		pGraphics;
	Pen *			pPen;
	GraphicsPath *	pPath;
	
	pGraphics = new Graphics(pDC->GetSafeHdc());

	pPath = new GraphicsPath(FillModeWinding);
	CRect			rc1(rc.left+3, rc.top, rc.right-3, rc.bottom), 
					rc2(rc.left, rc.top+3, rc.right, rc.bottom-3);
	
	pPath->AddRectangle(RectF(rc1.left, rc1.top, rc1.Width(), rc1.Height()));
	pPath->AddRectangle(RectF(rc2.left, rc2.top, rc2.Width(), rc2.Height()));
	pPath->AddEllipse(RectF(rc.left, rc.top, 6, 6));
	pPath->AddEllipse(RectF(rc.right-6, rc.top, 6, 6));
	pPath->AddEllipse(RectF(rc.left, rc.bottom-6, 6, 6));
	pPath->AddEllipse(RectF(rc.right-6, rc.bottom-6, 6, 6));
	pPath->Flatten();

	pPen = new Pen(Color(fAlpha*255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 1.0);
	pPath->Outline();
	pGraphics->DrawPath(pPen, pPath);

	delete pPen;
	delete pPath;
	delete pGraphics;
	#endif // NOGDIPLUS
};

void CMainBoard::DrawTab(CDC * pDC, LPCTSTR szText, const CRect & rcTab, BOOL bActive)
{
	COLORREF		crColor1 = RGB(200, 200, 200);
	COLORREF		crColor2 = RGB(255, 255, 255);
	CRect			rc = rcTab;

	if (bActive)
	{
		crColor1 = RGB(65, 153, 247);
		crColor2 = RGB(129, 13, 201);
	};

	rc.left	 += 3;
	rc.right -= 3;

	if (!bActive)
		rc.top+=3;

	#ifndef NOGDIPLUS
	Graphics *		pGraphics;
	Pen *			pPen;
	GraphicsPath *	pPath;
	Brush *			pBrush;
	
	pGraphics = new Graphics(pDC->GetSafeHdc());

	pPath = new GraphicsPath(FillModeWinding);
	CRect			rc1(rc.left+12, rc.top, rc.right-12, rc.bottom), 
					rc2(rc.left, rc.top+12, rc.right, rc.bottom);
	
	pPath->AddRectangle(RectF(rc1.left, rc1.top, rc1.Width(), rc1.Height()));
	pPath->AddRectangle(RectF(rc2.left, rc2.top, rc2.Width(), rc2.Height()));
	pPath->AddEllipse(RectF(rc.left, rc.top, 24, 24));
	pPath->AddEllipse(RectF(rc.right-24, rc.top, 24, 24));
	pPath->Flatten();

	pBrush = new LinearGradientBrush(PointF(rc.right, rc.top), 
					PointF(rc.right, rc.bottom), 
					Color(GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 
					Color(GetRValue(crColor2), GetGValue(crColor2), GetBValue(crColor2)));

	pPen = new Pen(Color(255.0, GetRValue(crColor1), GetGValue(crColor1), GetBValue(crColor1)), 1.0);
	pPath->Outline();
	pGraphics->FillPath(pBrush, pPath);
	pGraphics->DrawPath(pPen, pPath);

	{
		Font *				pFont;
		StringFormat		format;
		PointF				pf((rc.left+rc.right)/2, rc.top+3);
		CString				strText = szText;
		SolidBrush			brush(Color(0, 0, 0));

		if (bActive)
		{
			pFont = new Font(FontFamily::GenericSansSerif(), 9, FontStyleBold);
			brush.SetColor(Color(255, 255, 255));
		}
		else
		{
			pFont = new Font(FontFamily::GenericSansSerif(), 9, FontStyleRegular);
		};

		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentNear);

		pGraphics->DrawString(CComBSTR(strText), -1, pFont, pf, &format, &brush);

		delete pFont;
	};

	delete pBrush;
	delete pPen;
	delete pPath;
	delete pGraphics;
	#endif // NOGDIPLUS
};

void CMainBoard::DrawProgress(CDC * pDC)
{
	if (m_bProgressing)
	{
		CRect					rcProgress;

		m_ProgressRect.GetClientRect(rcProgress);
		m_ProgressRect.ClientToScreen(&rcProgress);
		ScreenToClient(&rcProgress);

		Graphics *				pGraphics;
		Pen *					pPen;
		Brush *					pBrush;
		double					fPercent = 0.0;
		double					fPosition;

		//m_lProgressTotal = 100;
		//m_lProgressAchieved = 50;
		if (m_lProgressTotal)
			fPercent = (double)m_lProgressAchieved/(double)m_lProgressTotal;

		fPosition = rcProgress.Width()*fPercent;
		
		pGraphics = new Graphics(pDC->GetSafeHdc());
		pPen = new Pen(Color(255.0, 255.0, 255.0), 2.0);
		
		pGraphics->SetSmoothingMode(SmoothingModeHighQuality);


		GraphicsPath *			pOutlinePath;

		pOutlinePath = new GraphicsPath(FillModeWinding);
		pOutlinePath->AddRectangle(RectF(rcProgress.left+5, rcProgress.top, rcProgress.Width()-10, rcProgress.Height()));
		pOutlinePath->AddRectangle(RectF(rcProgress.left, rcProgress.top+5, rcProgress.Width(), rcProgress.Height()-10));
		pOutlinePath->AddEllipse(RectF(rcProgress.left, rcProgress.top, 10, 10));
		pOutlinePath->AddEllipse(RectF(rcProgress.right-10, rcProgress.top, 10, 10));
		pOutlinePath->AddEllipse(RectF(rcProgress.left, rcProgress.bottom-10, 10, 10));
		pOutlinePath->AddEllipse(RectF(rcProgress.right-10, rcProgress.bottom-10, 10, 10));
		pOutlinePath->Outline(NULL, (REAL)0.01);

		pBrush  = new SolidBrush(Color(255.0*0.7, 255.0, 255.0, 255.0));
		pGraphics->FillPath(pBrush, pOutlinePath);
		delete pBrush;

		GraphicsPath *			pProgressPath;

		pProgressPath = new GraphicsPath(FillModeWinding);
		pProgressPath->AddRectangle(RectF(rcProgress.left+5, rcProgress.top, fPosition-5, rcProgress.Height()));
		pProgressPath->AddRectangle(RectF(rcProgress.left, rcProgress.top+5, fPosition, rcProgress.Height()-10));
		pProgressPath->AddEllipse(RectF(rcProgress.left, rcProgress.top, 10, 10));
		pProgressPath->AddEllipse(RectF(rcProgress.left, rcProgress.bottom-10, 10, 10));
		pProgressPath->Outline(NULL, (REAL)0.01);

		pBrush  = new SolidBrush(Color(255.0*0.7, 255.0, 255.0, 255.0));
		pGraphics->FillPath(pBrush, pOutlinePath);
		delete pBrush;


		pBrush = new LinearGradientBrush(PointF(rcProgress.left, (rcProgress.top+rcProgress.bottom)/2.0), 
			               PointF(rcProgress.left+fPosition, (rcProgress.top+rcProgress.bottom)/2.0), 
						   Color(0.0, 128.0, 0.0), 
						   Color(0.0, 255.0, 0.0));
		pGraphics->FillPath(pBrush, pProgressPath);
		delete pBrush;

		pGraphics->DrawPath(pPen, pOutlinePath);
		delete pOutlinePath;

		if (m_strProgress.GetLength())
		{
			Font *				pFont;
			StringFormat		format;
			PointF				pf((rcProgress.left+rcProgress.right)/2, (rcProgress.top+rcProgress.bottom)/2);
			SolidBrush			brush(Color(0, 0, 0));

			pFont = new Font(FontFamily::GenericSansSerif(), 9, FontStyleRegular);
			format.SetAlignment(StringAlignmentCenter);
			format.SetLineAlignment(StringAlignmentCenter);

			pGraphics->DrawString(CComBSTR(m_strProgress), -1, pFont, pf, &format, &brush);

			delete pFont;
		};

		delete pPen;
		delete pGraphics;
	};

	// Draw Statistics
	if (m_lNrPending || m_lNrRegistered || m_lNrStacked)
	{
		Graphics				graphics(pDC->GetSafeHdc());
		CRect					rcStats;
		CString					strText;
		CString					strTime;

		ExposureToString(m_fTotalExposureTime, strTime);
		strText.Format(m_strStatsMask, m_lNrPending, m_lNrRegistered, m_lNrStacked, (LPCTSTR)strTime);

		m_Stats.GetWindowRect(&rcStats);
		ScreenToClient(&rcStats);

		Font 				font(FontFamily::GenericSansSerif(), 8, FontStyleRegular);
		StringFormat		format;
		PointF				pf(rcStats.left, rcStats.top);
		SolidBrush			brush(Color(255, 255, 255));

		format.SetAlignment(StringAlignmentNear);
		format.SetLineAlignment(StringAlignmentNear);

		graphics.DrawString(CComBSTR(strText), -1, &font, pf, &format, &brush);
	};
};

void	CMainBoard::DrawMonitorButton(CDC * pDC)
{
	CRect						rcButton;
	CString						strText;

	m_Monitor.GetWindowRect(&rcButton);
	m_Monitor.GetWindowText(strText);
	ScreenToClient(&rcButton);

	Graphics					graphics(pDC->GetSafeHdc());
	double						fHeight = rcButton.Height()*0.8;
	Point						pt[4];
	GraphicsPath				path;

	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	pt[0].X = (rcButton.left+rcButton.right)/2 - fHeight/3;
	pt[0].Y = rcButton.top;
	pt[2] = pt[1] = pt[0];
	pt[1].Y = pt[0].Y + fHeight/2;
	pt[2].Y = pt[0].Y + fHeight;
	pt[3].X = pt[0].X + fHeight/2+fHeight/4;
	pt[3].Y = pt[0].Y + fHeight/2;

	path.AddPolygon(pt, 4);

	if (m_bMonitoring)
	{
		// Black outline triangle
		// Green inside triangle
		// With vertical green/white/green gradient
		Pen				pen(Color(0, 0, 0), 2.5);

		graphics.DrawPath(&pen, &path);

		Color			colors[4];
		INT				nColors = 4;

		colors[0].SetValue(Color::MakeARGB(255*0.8, 0, 255, 0));
		colors[1].SetValue(Color::MakeARGB(255*0.9, 255, 255, 255));
		colors[2].SetValue(Color::MakeARGB(255*0.8, 0, 255, 0));
		colors[3].SetValue(Color::MakeARGB(255*0.9, 255, 255, 255));
		PathGradientBrush	brush(&path);
		brush.SetCenterColor(Color(255*0.80, 255, 255, 255));
		brush.SetSurroundColors(colors, &nColors);

		graphics.FillPath(&brush, &path);

		pen.SetColor(Color(0, 255.0, 0));
		pen.SetWidth((float)1.7);
		graphics.DrawPath(&pen, &path);
	}
	else
	{
		// Blue inside triangle

		// With white translucent background
		Pen					pen(Color(0, 220, 255), 2.0);
		SolidBrush			brush(Color(255*0.8, 255.0, 255.0, 255.0));

		graphics.FillPath(&brush, &path);
		graphics.DrawPath(&pen, &path);
	};

	{
		Font 				font(FontFamily::GenericSansSerif(), 8, FontStyleRegular);
		StringFormat		format;
		PointF				pf((rcButton.left+rcButton.right)/2, rcButton.bottom);
		SolidBrush			brush(Color(255, 255, 255));

		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentNear);

		graphics.DrawString(CComBSTR(strText), -1, &font, pf, &format, &brush);
	};
};

void	CMainBoard::DrawStackButton(CDC * pDC)
{
	CRect						rcButton;
	CString						strText;

	m_Stack.GetWindowRect(&rcButton);
	m_Stack.GetWindowText(strText);
	ScreenToClient(&rcButton);

	Graphics					graphics(pDC->GetSafeHdc());
	float						fHeight = rcButton.Height()*0.7;
	Point						ptCenter;
	GraphicsPath				path;

	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	ptCenter.X = (rcButton.left+rcButton.right)/2;
	ptCenter.Y = rcButton.top+fHeight/2+0.05*fHeight;

	path.AddEllipse((float)ptCenter.X-fHeight/2, ptCenter.Y-fHeight/2, fHeight, fHeight);

	if (m_bStacking)
	{
		// Black outline triangle
		// Green inside triangle
		// With vertical green/white/green gradient
		Pen				pen(Color(0, 0, 0), 2.5);

		graphics.DrawPath(&pen, &path);

		Color			colors[1];
		INT				nColors = 1;

		colors[0].SetValue(Color::MakeARGB(255*0.8, 255, 0, 0));
		PathGradientBrush	brush(&path);
		brush.SetCenterColor(Color(255*0.80, 255, 240, 240));
		brush.SetSurroundColors(colors, &nColors);

		graphics.FillPath(&brush, &path);

		pen.SetColor(Color(255.0, 0.0, 0));
		pen.SetWidth((float)1.7);
		graphics.DrawPath(&pen, &path);
	}
	else
	{
		// Blue inside triangle

		// With white translucent background
		Color			colors[1];
		INT				nColors = 1;

		colors[0].SetValue(Color::MakeARGB(255*0.8, 255, 255, 255));

		Pen					pen(Color(0, 220, 255), 2.0);
		PathGradientBrush	brush(&path);
		brush.SetCenterColor(Color(255*0.7, 255.0, 255.0, 255.0));
		brush.SetSurroundColors(colors, &nColors);

		graphics.FillPath(&brush, &path);
		graphics.DrawPath(&pen, &path);
	};

	{
		Font 				font(FontFamily::GenericSansSerif(), 8, FontStyleRegular);
		StringFormat		format;
		PointF				pf((rcButton.left+rcButton.right)/2, rcButton.bottom);
		SolidBrush			brush(Color(255, 255, 255));

		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentNear);

		graphics.DrawString(CComBSTR(strText), -1, &font, pf, &format, &brush);
	};
};

void	CMainBoard::DrawStopButton(CDC * pDC)
{
	CRect						rcButton;
	CString						strText;

	m_Stop.GetWindowRect(&rcButton);
	m_Stop.GetWindowText(strText);
	ScreenToClient(&rcButton);

	Graphics					graphics(pDC->GetSafeHdc());
	float						fHeight = rcButton.Height()*0.6;
	Point						ptCenter;
	GraphicsPath				path;

	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	ptCenter.X = (rcButton.left+rcButton.right)/2;
	ptCenter.Y = rcButton.top+fHeight/2+0.15*fHeight;

	path.AddRectangle(RectF((float)ptCenter.X-fHeight/2, ptCenter.Y-fHeight/2, fHeight, fHeight));

	if (m_bStopping)
	{
		// Black outline triangle
		// Green inside triangle
		// With vertical green/white/green gradient
		Pen				pen(Color(0, 0, 0), 2.5);

		graphics.DrawPath(&pen, &path);

		Color			colors[1];
		INT				nColors = 1;

		colors[0].SetValue(Color::MakeARGB(255*0.8, 255, 0, 0));
		PathGradientBrush	brush(&path);
		brush.SetCenterColor(Color(255*0.80, 255, 240, 240));
		brush.SetSurroundColors(colors, &nColors);

		graphics.FillPath(&brush, &path);

		pen.SetColor(Color(255.0, 0.0, 0));
		pen.SetWidth((float)1.7);
		graphics.DrawPath(&pen, &path);
	}
	else
	{
		// Blue inside triangle

		// With white translucent background
		Color			colors[1];
		INT				nColors = 1;

		colors[0].SetValue(Color::MakeARGB(255*0.8, 255, 255, 255));

		Pen					pen(Color(0, 220, 255), 2.0);
		PathGradientBrush	brush(&path);
		brush.SetCenterColor(Color(255*0.7, 255.0, 255.0, 255.0));
		brush.SetSurroundColors(colors, &nColors);

		graphics.FillPath(&brush, &path);
		graphics.DrawPath(&pen, &path);
	};

	{
		Font 				font(FontFamily::GenericSansSerif(), 8, FontStyleRegular);
		StringFormat		format;
		PointF				pf((rcButton.left+rcButton.right)/2, rcButton.bottom);
		SolidBrush			brush(Color(255, 255, 255));

		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentNear);

		graphics.DrawString(CComBSTR(strText), -1, &font, pf, &format, &brush);
	};
};

BOOL CMainBoard::OnEraseBkgnd(CDC* pDC)
{
	BOOL						bResult = TRUE;//CDialog::OnEraseBkgnd(pDC);
	CRect						rc;
	CDC							MemDC;
	CBitmap						Bitmap;
	CBitmap *					pOldBitmap;
	CString						strText;
	CDeepSkyStackerLiveDlg *	pDlg = GetDSSLiveDlg(this);
	DSSLIVETAB					TabID;

	GetClientRect(&rc);
	MemDC.CreateCompatibleDC(pDC);
	Bitmap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	pOldBitmap = MemDC.SelectObject(&Bitmap);

	if (pDlg)
		TabID = pDlg->GetCurrentTab();

	DrawGradientBackgroundRect(&MemDC, rc);

	m_MonitoringRect.GetWindowRect(&rc);
	m_MonitoringRect.GetWindowText(strText);
	ScreenToClient(&rc);
	DrawGradientFrameRect(&MemDC, strText, rc, TRUE, TRUE);
	DrawGradientFrameRect(&MemDC, strText, rc, TRUE, FALSE);

	/*
	m_SettingsRect.GetWindowRect(&rc);
	m_SettingsRect.GetWindowText(strText);
	ScreenToClient(&rc);
	DrawGradientFrameRect(&MemDC, strText, rc, TRUE, TRUE);
	DrawGradientFrameRect(&MemDC, strText, rc, TRUE, FALSE);
	*/

	m_StackedImage.GetWindowRect(&rc);
	m_StackedImage.GetWindowText(strText);
	ScreenToClient(&rc);
	DrawTab(&MemDC, strText, rc, (TabID == DLT_STACKEDIMAGE));

	m_LastImage.GetWindowRect(&rc);
	m_LastImage.GetWindowText(strText);
	ScreenToClient(&rc);
	DrawTab(&MemDC, strText, rc, (TabID == DLT_LASTIMAGE));

	m_Graphs.GetWindowRect(&rc);
	m_Graphs.GetWindowText(strText);
	ScreenToClient(&rc);
	DrawTab(&MemDC, strText, rc, (TabID == DLT_GRAPHS));

	m_Log.GetWindowRect(&rc);
	m_Log.GetWindowText(strText);
	ScreenToClient(&rc);
	DrawTab(&MemDC, strText, rc, (TabID == DLT_LOG));

	m_ImageList.GetWindowRect(&rc);
	m_ImageList.GetWindowText(strText);
	ScreenToClient(&rc);
	DrawTab(&MemDC, strText, rc, (TabID == DLT_IMAGELIST));

	m_Warnings.GetWindowRect(&rc);
	m_Warnings.GetWindowText(strText);
	ScreenToClient(&rc);
	DrawTab(&MemDC, strText, rc, (TabID == DLT_SETTINGS));

	DrawMonitorButton(&MemDC);
	DrawStackButton(&MemDC);
	DrawStopButton(&MemDC);

	DrawProgress(&MemDC);

	GetClientRect(&rc);
	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC();

	return bResult;
}

/* ------------------------------------------------------------------- */

static void InitLabel(CLabel & label, BOOL bMain = FALSE)
{
	label.SetLink(TRUE, TRUE);
	label.SetTransparent(TRUE);
	label.SetLinkCursor(LoadCursor(NULL,IDC_HAND));
	label.SetFont3D(FALSE);
	label.SetTextColor(RGB(255, 255, 255));
//	label.SetText3DHiliteColor(RGB(0, 0, 0));

	if (bMain)
	{
		//label.SetFontBold(TRUE);
		label.SetTextColor(RGB(109, 23, 7));
	};
};

/* ------------------------------------------------------------------- */

inline BOOL	IsInRect(CStatic & ctrl, CPoint pt)
{
	CRect			rc;

	ctrl.GetWindowRect(&rc);
	return (rc.PtInRect(pt));
};

inline BOOL	IsInRect(CButton & ctrl, CPoint pt)
{
	CRect			rc;

	ctrl.GetWindowRect(&rc);
	return (rc.PtInRect(pt));
};

void CMainBoard::OnLButtonDown( UINT, CPoint pt)
{
	CDeepSkyStackerLiveDlg *	pDlg = GetDSSLiveDlg(this);
	DSSLIVETAB					TabID = DLT_NONE;
	DSSLIVETAB					NewTabID = DLT_NONE;

	if (pDlg)
		TabID = pDlg->GetCurrentTab();

	ClientToScreen(&pt);

	if (IsInRect(m_StackedImage, pt))
		NewTabID = DLT_STACKEDIMAGE;
	else if (IsInRect(m_LastImage, pt))
		NewTabID = DLT_LASTIMAGE;
	else if (IsInRect(m_Graphs, pt))
		NewTabID = DLT_GRAPHS;
	else if (IsInRect(m_ImageList, pt))
		NewTabID = DLT_IMAGELIST;
	else if (IsInRect(m_Log, pt))
		NewTabID = DLT_LOG;
	else if (IsInRect(m_Warnings, pt))
		NewTabID = DLT_SETTINGS;
	else if (IsInRect(m_Monitor, pt))
	{
		if (!m_bMonitoring)
		{
			if (IsMonitoredFolderOk() || ChangeMonitoredFolder())
			{
				if (CheckRestartMonitoring())
				{
					m_bMonitoring = TRUE;
					OnMonitor();
					InvalidateButtons();
				};
			};
		};
	}
	else if (IsInRect(m_Stack, pt))
	{
		BOOL			bProceed = TRUE;
		if (m_bStacking)
			m_bStacking = FALSE;
		else
		{
			if (IsMonitoredFolderOk() || ChangeMonitoredFolder())
			{
				if (!m_bMonitoring)
				{
					m_bMonitoring = CheckRestartMonitoring();
					if (m_bMonitoring)
						OnMonitor();
				};
				if (m_bMonitoring)
					m_bStacking = TRUE;
			}
			else
				bProceed = FALSE;
		};
		if (bProceed)
		{
			OnStack();
			InvalidateButtons();
		};
	}
	else if (IsInRect(m_Stop, pt))
	{
		OnStop();
		m_bStacking = m_bMonitoring = FALSE;
		OnStack();
		InvalidateButtons();
	};

	if ((TabID != NewTabID) && (NewTabID != DLT_NONE))
		pDlg->SetCurrentTab(NewTabID);
};

/* ------------------------------------------------------------------- */

BOOL	CMainBoard::CheckRestartMonitoring()
{
	BOOL					bResult = TRUE;

	if (!m_bMonitoring)
	{
		if (m_lNrStacked || m_lNrPending)
		{
			CRestartMonitoring		dlg;
			bResult = FALSE;

			if (dlg.DoModal() == IDOK)
			{
				if (dlg.IsDropPending())
					m_LiveEngine.ClearPendingImages();
				if (dlg.IsClearStackedImage())
					m_LiveEngine.ClearStackedImage();
				bResult = TRUE;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CMainBoard::IsMonitoredFolderOk()
{
	BOOL					bResult = FALSE;
	CString					strFolder;
	CRegistry				reg;

	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("MonitoredFolder"), strFolder);

	if (strFolder.GetLength())
	{
		// Check that the folder exists
		CString			strFile;

		strFile.Format(_T("%s/testfolder.dsslive.txt"), (LPCTSTR)strFolder);
		FILE *			hFile;

		hFile = _tfopen(strFile, _T("wt"));
		if (hFile)
		{
			fclose(hFile);
			DeleteFile(strFile);
			bResult = TRUE;
		}
		else
			strFolder.Empty();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CMainBoard::ChangeMonitoredFolder()
{
	BOOL					bResult = FALSE;

	if (m_bMonitoring)
	{
		CString					strText;

		strText.LoadString(IDS_CANTCHANGEMONITOREDFOLDER);
		AfxMessageBox(strText, MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		CString					strFolder;
		CString					strTitle;
		
		m_MonitoredFolder.GetWindowText(strFolder);

		CFolderDlg				dlg(FALSE, strFolder, this);

		strTitle.LoadString(IDS_SELECTMONITOREDFOLDER);
		dlg.SetTitle(strTitle);

		if (dlg.DoModal() == IDOK)
		{
			strFolder = dlg.GetFolderName();
			m_MonitoredFolder.SetWindowText(strFolder);
			InvalidateRect(NULL);

			CRegistry			reg;

			reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("MonitoredFolder"), strFolder);
			bResult = TRUE;
		};	};


	return bResult;
};

/* ------------------------------------------------------------------- */

void CMainBoard::OnMonitoredFolder( NMHDR * pNotifyStruct, LRESULT * result )
{
	ChangeMonitoredFolder();
};

/* ------------------------------------------------------------------- */

BOOL CMainBoard::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRegistry			reg;
	CString				strFolder;

	m_Stats.GetWindowText(m_strStatsMask);
	m_LiveEngine.SetWindow(m_hWnd);

	if (IsMonitoredFolderOk())
	{
		reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("MonitoredFolder"), strFolder);
		m_MonitoredFolder.SetWindowText(strFolder);
	};

	InitLabel(m_MonitoredFolder);

	m_ControlPos.SetParent(this);
	m_ControlPos.AddControl(IDC_MONITORINGRECT, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_PROGRESS, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_MONITOREDFOLDER, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_STATS, CP_RESIZE_HORIZONTAL);

	return TRUE;  
}

/* ------------------------------------------------------------------- */

void CMainBoard::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_ControlPos.MoveControls();
	InvalidateRect(NULL);
}

/* ------------------------------------------------------------------- */

void	CMainBoard::GetNewFilesInMonitoredFolder(std::vector<CString> & vFiles)
{
	CString					strFolder;
	WIN32_FIND_DATA			FindData;
	CString					strFileMask;
	HANDLE					hFindFiles;
	CRegistry				reg;
	std::vector<CString>	vDelayedFiles;
	CString					strExcluded;

	vFiles.clear();
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("MonitoredFolder"), strFolder);
	if (!reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Excluded"), strExcluded))
		strExcluded = _T(".TMP;.BAK;.TEMP;.TXT");
	strExcluded.MakeUpper();

	strFolder += _T("\\");
	strFileMask = strFolder;
	strFileMask += _T("*.*");

	hFindFiles = FindFirstFile(strFileMask, &FindData);
	if (hFindFiles != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString			strFile;

			strFile = strFolder;
			strFile += FindData.cFileName;

			// Check that it is not a txt file
			TCHAR			szExt[_MAX_EXT];
			CString			strExt;

			_tsplitpath(strFile, NULL, NULL, NULL, szExt);
			strExt = szExt;
			strExt.MakeUpper();
			if (strExt.GetLength() && (strExcluded.Find(strExt, 0) == -1))
			{
				// Check that the file is not already in the all list
				if (!std::binary_search(m_vAllFiles.begin(), m_vAllFiles.end(), strFile))
				{
					// Check that it is an image file which is to
					// be processed
					if (m_LiveEngine.IsFileAvailable(strFile))
					{
						CBitmapInfo			bmpInfo;

						if (GetPictureInfo(strFile, bmpInfo))
						{
							BOOL			bAdd = FALSE;

							if (bmpInfo.m_strFileType=="RAW")
								bAdd = m_LiveSettings.IsProcess_RAW();
							else if (bmpInfo.m_strFileType.Left(4)=="FITS")
								bAdd = m_LiveSettings.IsProcess_FITS();
							else if (bmpInfo.m_strFileType.Left(4)=="TIFF")
								bAdd = m_LiveSettings.IsProcess_TIFF();
							else
								bAdd = m_LiveSettings.IsProcess_Others();

							if (bAdd)
								vFiles.push_back(strFile);
						};
					}
					else
					{
						// Remove the file from the all files list and 
						// put the timer to try again in 10 seconds
						vDelayedFiles.push_back(strFile);
					};
				};
			};
		}
		while (FindNextFile(hFindFiles, &FindData));

		FindClose(hFindFiles);

		std::sort(vFiles.begin(), vFiles.end());

		if (vDelayedFiles.size() && m_ulSHRegister)
		{
			// Use and alternate method
			DWORD			dwPollingTime = 10;

			reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("PollingTime"), dwPollingTime);

			SetTimer(1, dwPollingTime*1000, NULL);
		};
	};
};

/* ------------------------------------------------------------------- */
// The lParam value contains the event SHCNE_xxxx
// The wParam value supplies the SHNOTIFYSTRUCT

typedef struct 
{
    DWORD dwItem1;    // dwItem1 contains the previous PIDL or name of the folder. 
    DWORD dwItem2;    // dwItem2 contains the new PIDL or name of the folder. 
} SHNOTIFYSTRUCT;

LRESULT CMainBoard::OnFolderChange(WPARAM wParam, LPARAM lParam)
{
	std::vector<CString>		vFiles;
	std::vector<CString>		vNewFiles;

	GetNewFilesInMonitoredFolder(vFiles);

	// Find the new files
	for (LONG i = 0;i<vFiles.size();i++)
	{
		if (!std::binary_search(m_vAllFiles.begin(), m_vAllFiles.end(), vFiles[i]))
			vNewFiles.push_back(vFiles[i]);
	};

	if (vNewFiles.size())
	{
		CString						strNewFiles;
		std::vector<CBitmapInfo>	vBitmapInfos;

		// Sort the new files by date/time
		for (LONG i = 0;i<vNewFiles.size();i++)
		{
			CBitmapInfo				BitmapInfo;

			if (GetPictureInfo(vNewFiles[i], BitmapInfo))
				vBitmapInfos.push_back(BitmapInfo);
		};

		std::sort(vBitmapInfos.begin(), vBitmapInfos.end(), CompareBitmapInfoDateTime);
		vNewFiles.clear();

		for (LONG i = 0;i<vBitmapInfos.size();i++)
			vNewFiles.push_back(vBitmapInfos[i].m_strFileName);

		for (LONG i = 0;i<vNewFiles.size();i++)
			m_vAllFiles.push_back(vNewFiles[i]);
		std::sort(m_vAllFiles.begin(), m_vAllFiles.end());

		strNewFiles.Format(IDS_LOG_NEWFILESFOUND, vNewFiles.size());
		AddToLog(strNewFiles, TRUE, FALSE, FALSE, RGB(0, 128, 0));
		for (LONG i = 0;i<vNewFiles.size();i++)
		{
			strNewFiles.Format(IDS_LOG_NEWFILE, (LPCTSTR)vNewFiles[i]);
			AddToLog(strNewFiles, FALSE, FALSE, FALSE, RGB(0, 128, 0));
			m_LiveEngine.AddFileToProcess(vNewFiles[i]);
		};
	};

	return 1;
};

/* ------------------------------------------------------------------- */

void CMainBoard::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent)
	{
		if (m_ulSHRegister)
			KillTimer(nIDEvent);
		OnFolderChange(0, 0);
	};
};

/* ------------------------------------------------------------------- */

void CMainBoard::OnMonitor()
{
	HRESULT					hResult;
	CComPtr<IShellFolder>	pIShellFolder;
	CComPtr<IBindCtx>		pIBindCtx;
	SHChangeNotifyEntry		shCNE;
	LPITEMIDLIST			ppidl;
	BOOL					bUseExisting = FALSE;

	if (m_ulSHRegister)
	{
		SHChangeNotifyDeregister(m_ulSHRegister);
		m_ulSHRegister = 0;
	};
	KillTimer(1);
	try
	{
		CRegistry				reg;
		CString					strFolder;
		std::vector<CString>	vNewFiles;

		reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("MonitoredFolder"), strFolder);

		m_vAllFiles.clear();
		GetNewFilesInMonitoredFolder(vNewFiles);

		if (!m_lNrStacked && !m_lNrPending && vNewFiles.size())
		{
			// Ask if the user want to use existing images
			CString			strText;
			int				nResult;

			strText.Format(IDS_USEEXISTINGIMAGES, vNewFiles.size());

			nResult = AfxMessageBox(strText, MB_YESNO | MB_ICONQUESTION);
			if (nResult == IDYES)
			{
				m_vAllFiles.clear();
				bUseExisting = TRUE;
			};
		}
		else
			m_vAllFiles = vNewFiles;

		hResult = SHGetDesktopFolder(&pIShellFolder);
		if (hResult != S_OK)
			throw(hResult);
		hResult = CreateBindCtx(0, &pIBindCtx);
		if (hResult != S_OK)
			throw(hResult);

		hResult = pIShellFolder->ParseDisplayName(NULL, pIBindCtx, CComBSTR(strFolder), NULL, &ppidl, NULL);
		if (hResult != S_OK)
			throw(hResult);

		shCNE.fRecursive = FALSE;
		shCNE.pidl		 = ppidl;
		m_ulSHRegister = SHChangeNotifyRegister(m_hWnd, SHCNE_DISKEVENTS, SHCNE_ALLEVENTS, WM_FOLDERCHANGE, 1, &shCNE);

		if (!m_ulSHRegister)
		{
			// Use and alternate method
			DWORD			dwPollingTime = 10;

			reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("PollingTime"), dwPollingTime);

			SetTimer(1, dwPollingTime*1000, NULL);
		};

		//if (m_ulSHRegister)
		{
			CString			strText;

			strText.Format(IDS_LOG_STARTMONITORING, (LPCTSTR)strFolder);
			AddToLog(strText, TRUE, TRUE, FALSE, RGB(0, 127, 0));
		}
		/*else
		{
			CString			strText;

			strText.Format(IDS_LOG_ERRORSTARTMONITORING, (LPCTSTR)strFolder);
			AddToLog(strText, TRUE, TRUE, FALSE, RGB(0, 127, 0));
		};*/

		if (bUseExisting)
			OnFolderChange(0, 0);
	}
	catch(HRESULT )
	{
	};
};

/* ------------------------------------------------------------------- */

void CMainBoard::OnStop()
{
	KillTimer(1);
	if (m_bMonitoring/*m_ulSHRegister*/)
	{
		CRegistry			reg;
		CString				strFolder;
		CString				strText;

		reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("MonitoredFolder"), strFolder);

		strText.Format(IDS_LOG_STOPMONITORING, (LPCTSTR)strFolder);
		AddToLog(strText, TRUE, TRUE, FALSE, RGB(128, 0, 0));

		if (m_ulSHRegister)
			SHChangeNotifyDeregister(m_ulSHRegister);
		m_ulSHRegister = 0;
	};
};

/* ------------------------------------------------------------------- */

void CMainBoard::OnStack()
{
	if (m_bMonitoring/*m_ulSHRegister*/)
	{
		CRegistry			reg;
		CString				strFolder;
		CString				strText;

		reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("MonitoredFolder"), strFolder);

		if (m_bStacking)
			strText.Format(IDS_LOG_STARTSTACKING, (LPCTSTR)strFolder);
		else
			strText.Format(IDS_LOG_STOPSTACKING, (LPCTSTR)strFolder);
		AddToLog(strText, TRUE, TRUE, FALSE, RGB(128, 128, 0));

	};
	m_LiveEngine.EnableRegistering(m_bMonitoring);
	m_LiveEngine.EnableStacking(m_bStacking);
};

/* ------------------------------------------------------------------- */

void CMainBoard::InvalidateProgress()
{
	CRect				rcProgress;

	m_ProgressRect.GetClientRect(rcProgress);
	m_ProgressRect.ClientToScreen(&rcProgress);
	ScreenToClient(&rcProgress);
	rcProgress.InflateRect(2, 2);
	InvalidateRect(&rcProgress, TRUE);
};

/* ------------------------------------------------------------------- */

void CMainBoard::InvalidateButtons()
{
	CRect				rcButtons;
	CRect				rcMonitor;
	CRect				rcStack;
	CRect				rcStop;

	m_Monitor.GetWindowRect(&rcMonitor);
	ScreenToClient(&rcMonitor);
	m_Stack.GetWindowRect(&rcStack);
	ScreenToClient(&rcStack);
	m_Stop.GetWindowRect(&rcStop);
	ScreenToClient(&rcStop);

	rcButtons.UnionRect(&rcMonitor, &rcStack);
	rcButtons.UnionRect(&rcButtons, &rcStop);
	// Then add the text below
	rcButtons.bottom += 20;
	InvalidateRect(&rcButtons, TRUE);
};

/* ------------------------------------------------------------------- */

void	CMainBoard::InvalidateStats()
{
	CRect				rcStats;

	m_Stats.GetWindowRect(rcStats);
	ScreenToClient(&rcStats);
	rcStats.InflateRect(2, 2);
	InvalidateRect(&rcStats, TRUE);
};

/* ------------------------------------------------------------------- */

LRESULT CMainBoard::OnLiveEngine(WPARAM, LPARAM)
{
	CSmartPtr<CLiveEngineMsg>		pMsg;

	if (m_LiveEngine.GetMessage(&pMsg))
	{
		switch (pMsg->GetMessage())
		{
		case LEM_ADDTOLOG:
			{
				CString				strText;
				BOOL				bDateTime,
									bBold,
									bItalic;
				COLORREF			crColor;

				if (pMsg->GetLog(strText, bDateTime, bBold, bItalic, crColor))
					AddToLog(strText, bDateTime, bBold, bItalic, crColor);
			};
			break;
		case LEM_STARTPROGRESS :
		case LEM_PROGRESSING :
			{
				CString				strProgress;
				LONG				lAchieved,
									lTotal;

				if (pMsg->GetProgress(strProgress, lAchieved, lTotal))
				{
					if (strProgress.GetLength())
						m_strProgress = strProgress;
					if (lTotal)
						m_lProgressTotal = lTotal;
					m_lProgressAchieved = lAchieved;
					m_bProgressing = TRUE;
					InvalidateProgress();
				};
			};
			break;
		case LEM_ENDPROGRESS :
			m_bProgressing = FALSE;
			InvalidateProgress();
			break;
		case LEM_UPDATEPENDING :
			{
				if (pMsg->GetPending(m_lNrPending))
					InvalidateStats();
			}
			break;
		case LEM_FILELOADED :
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;
				CSmartPtr<C32BitsBitmap>	pWndBitmap;
				CString						strFileName;

				if (pMsg->GetImage(&pBitmap, &pWndBitmap, strFileName))
					SetLastImage(pBitmap, pWndBitmap, strFileName);
			}
			break;
		case LEM_SETSTACKEDIMAGE :
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;
				CSmartPtr<C32BitsBitmap>	pWndBitmap;
				LONG						lNrStacked;
				double						fExposure;

				if (pMsg->GetStackedImage(&pBitmap, &pWndBitmap, lNrStacked, fExposure))
				{
					SetStackedImage(pBitmap, pWndBitmap);
					m_lNrStacked = lNrStacked;
					m_fTotalExposureTime = fExposure;
					InvalidateStats();
				};
			};
			break;
		case LEM_SETFOOTPRINT :
			{
				CPointExt					pt1, pt2, pt3, pt4;

				if (pMsg->GetFootprint(pt1, pt2, pt3, pt4))
					SetFootprintInStackedImage(pt1, pt2, pt3, pt4);
			};
			break;
		case LEM_STACKEDIMAGESAVED :
			{
				AdviseStackedImageSaved();
			}
			break;
		case LEM_FILEREGISTERED :
			{
				CString						strFileName;

				if (pMsg->GetRegisteredImage(strFileName))
				{
					AddImageToList(strFileName);
					m_lNrRegistered++;
					InvalidateStats();
				};
			}
			break;
		case LEM_UPDATEIMAGEOFFSETS :
			{
				CString						strFileName;
				double						fdX, fdY, fAngle;

				if (pMsg->GetImageOffsets(strFileName, fdX, fdY, fAngle))
				{
					UpdateImageOffsetsInList(strFileName, fdX, fdY, fAngle);
					AddOffsetsAngleToGraph(strFileName, fdX, fdY, fAngle);
				};
			};
			break;
		case LEM_CHANGEIMAGESTATUS :
			{
				CString						strFileName;
				IMAGESTATUS					status;
				if (pMsg->GetImageStatus(strFileName, status))
					ChangeImageStatusInList(strFileName, status);
			};
			break;
		case LEM_UPDATEIMAGEINFO :
			{
				CString						strFileName;
				STACKIMAGEINFO				info;

				if (pMsg->GetImageInfo(strFileName, info))
					ChangeImageInfoInCharts(strFileName, info);
			};
		case LEM_WARNING :
			{
				CString						strWarning;

				if (pMsg->GetWarning(strWarning))
				{

					if (m_LiveSettings.IsWarning_Flash())
						FlashMainWindow();
					if (m_LiveSettings.IsWarning_Sound())
						MessageBeep(MB_ICONEXCLAMATION);
					if (m_LiveSettings.IsWarning_File())
					{
						CString				strFolder;
						CString				strFile;

						m_LiveSettings.GetWarning_FileFolder(strFolder);

						strFile.Format(_T("%s\\DSSLiveWarning.txt"), (LPCTSTR)strFolder);
						FILE *				hFile;

						hFile = _tfopen(strFile, _T("at"));
						if (hFile)
						{
							fprintf(hFile, "%s\n", (LPCSTR)CT2CA(strWarning));
							fclose(hFile);
						};
					};
					if (m_LiveSettings.IsWarning_Email())
					{
						BOOL				bSendEmail = FALSE;

						if (m_LiveSettings.IsWarning_SendMultipleEmails())
							bSendEmail = TRUE;
						else if (!m_lNrEmails)
							bSendEmail = TRUE;

						if (bSendEmail)
						{
							CString			strEmail;
							CString			strSMTP;
							CString			strAccount;
							CString			strObject;

							m_LiveSettings.GetEmailSettings(strEmail, strAccount, strSMTP, strObject);

							try
							{
								CPJNSMTPConnection smtp;
								smtp.Connect(strSMTP);

								CPJNSMTPMessage m;
								m.m_To.Add(CPJNSMTPAddress(strEmail));
								m.m_From = CPJNSMTPAddress(strAccount);
								m.m_sSubject = strObject;
								m.AddTextBody(strWarning);
								smtp.SendMessage(m);

								m_lNrEmails++;
								ShowResetEmailCountButton();
							}
							catch (...)
							{
								CString		strError;

								strError.LoadString(IDS_ERRORSENDINGEMAIL);
								strError+="\n";
								AddToLog(strError, TRUE, TRUE, FALSE, RGB(255, 0, 0));
							};
						};
					};
				};
			};
			break;
		};
	};

	return 1;
};

/* ------------------------------------------------------------------- */

