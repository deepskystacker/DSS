// =======================================================
//
//                    CWndImage.cpp
//             WndImage class implementation
//
//           Copyright (C) 2000 Peter Hauptmann
//
//  Can be used and distributed freely
//          as long as the source copyright stays intact
//
//  updated version can be found under
//          http://buerger.metropolis.de/bitbucket/
//
// -------------------------------------------------------

#include "stdafx.h"
#include "WndImage.h"

const DWORD					TIMERID_TRACKLEAVE = 1;

static HBITMAP	GetSubImage(HBITMAP hBitmap, CRect & rcSource)
{
	HBITMAP				hBitmapResult;
	HDC					hScreenDC;
	HDC					hTgtDC;
	HDC					hSrcDC;

	hScreenDC = GetDC(nullptr);
	hTgtDC = CreateCompatibleDC(hScreenDC);
	hSrcDC = CreateCompatibleDC(hScreenDC);

	hBitmapResult = CreateCompatibleBitmap(hScreenDC, rcSource.Width(), rcSource.Height());
	HBITMAP				hOldSrcBitmap;
	HBITMAP				hOldTgtBitmap;

	hOldSrcBitmap = (HBITMAP)SelectObject(hSrcDC, hBitmap);
	hOldTgtBitmap = (HBITMAP)SelectObject(hTgtDC, hBitmapResult);

	BitBlt(hTgtDC, 0, 0, rcSource.Width(), rcSource.Height(), hSrcDC, rcSource.left, rcSource.top, SRCCOPY);

	// Add the crosshair
	SelectObject(hSrcDC, hOldSrcBitmap);
	SelectObject(hTgtDC, hOldTgtBitmap);

	ReleaseDC(nullptr, hScreenDC);
	DeleteDC(hTgtDC);
	DeleteDC(hSrcDC);

	return hBitmapResult;
};

// ==================================================================
//  CTor / DTor
// ------------------------------------------------------------------

CWndImage::CWndImage(bool bDarkMode /*=false*/) :
	m_bDarkMode(bDarkMode)
{
	m_shared = false;
	m_bmpSize = CSize(0,0);
	m_bltMode = bltNormal;
	m_alignX = bltLeft;
	m_alignY = bltTop;
	m_srcRect.SetRectEmpty();
	m_dstRect.SetRectEmpty();

	m_bCaptured = FALSE;

	m_zoomX   = 1.0;
	m_zoomY   = 1.0;
	m_origin.x = 0;
	m_origin.y = 0;

	m_bEnableZoom		= FALSE;
	m_bTrackTimerSet	= FALSE;

	m_bInvalidateInternalBitmap = TRUE;
	m_bInvalidateToolbarBitmap  = TRUE;
	m_bInvalidateZoomBitmap		= TRUE;
	m_bInvalidateOverlayBitmap  = TRUE;

	m_pButtonToolbar = nullptr;
	m_pToolbarImage  = nullptr;
	m_bToolbarTop	 = FALSE;
	m_bOnToolbar	 = FALSE;

	m_pBaseImage	 = nullptr;
	m_pOverlayImage	 = nullptr;
	m_pZoomImage	 = nullptr;
	m_pBufferImage	 = nullptr;

	m_pImageSink	 = nullptr;

	m_4Corners		 = false;
}

void CWndImage::OnDestroy()
{
	SetImg((HBITMAP)0);
	if (m_pToolbarImage)
		delete m_pToolbarImage;

	if (m_pBaseImage)
		delete m_pBaseImage;

	if (m_pOverlayImage)
		delete m_pOverlayImage;

	if (m_pZoomImage)
		delete m_pZoomImage;

	if (m_pBufferImage)
		delete m_pBufferImage;
}

CWndImage::~CWndImage()
{

}

BEGIN_MESSAGE_MAP(CWndImage, CWnd)
	//{{AFX_MSG_MAP(CWndImage)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// ==================================================================
//  OnPaint
// ------------------------------------------------------------------
#pragma warning (disable:4389)

static Bitmap * GetBitmap(CRect & rcOut, HBITMAP hBitmap, CRect & rcSrc, CRect & rcDst, BOOL bInterpolate, Bitmap * pInBitmap = nullptr, bool bDarkMode=false)
{
	Bitmap *		pBitmap = new Bitmap(hBitmap, nullptr);
	Bitmap *		pOutBitmap;

	if (pInBitmap)
	{
		// Check the size of the bitmap and if it's the same keep it
		if (pInBitmap->GetWidth()!=rcOut.Width() ||
			pInBitmap->GetHeight()!=rcOut.Height())
		{
			delete pInBitmap;
			pInBitmap = nullptr;
		};
	};
	if (!pInBitmap)
		pOutBitmap = new Bitmap(rcOut.Width(), rcOut.Height(), PixelFormat24bppRGB);
	else
		pOutBitmap = pInBitmap;

	Graphics *		pGraphics = new Graphics(pOutBitmap);

	if (pBitmap && pGraphics && pOutBitmap)
	{
		RectF				rcfSrc(rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height());
		RectF				rcfDst(rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height());
		ImageAttributes *	pAttr = nullptr;

		SolidBrush			brush(bDarkMode ? Color(80, 80, 80) : Color(200, 200, 200));

		pGraphics->FillRectangle(&brush, 0, 0, rcOut.Width(), rcOut.Height());

		if (bInterpolate)
			pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		else
			pGraphics->SetInterpolationMode(InterpolationModeNearestNeighbor);
		if (pBitmap)
			pGraphics->DrawImage(pBitmap, rcfDst, rcfSrc.X, rcfSrc.Y, rcfSrc.Width, rcfSrc.Height, UnitPixel, pAttr, nullptr, nullptr);

		if (pAttr)
			delete pAttr;
		delete pGraphics;
	}
	else
	{
		if (pOutBitmap)
			delete pOutBitmap;
		pOutBitmap = nullptr;
	};
	if (pBitmap)
		delete pBitmap;

	return pOutBitmap;
};

/* ------------------------------------------------------------------- */

BOOL CWndImage::CreateInternalBitmap()
{
	BOOL				bResult = FALSE;

	if (m_bInvalidateInternalBitmap)
	{
		CRect r;
		GetClientRect(&r);

		//if (m_pBaseImage)
		//	delete m_pBaseImage;

		CRect & src = m_srcRect;
		CRect & dst = m_dstRect;

		if (!src.IsRectEmpty() && !dst.IsRectEmpty())
		{
			m_pBaseImage = ::GetBitmap(r, (HBITMAP)m_bmp.GetSafeHandle(), src, dst, (m_zoomX < 1), m_pBaseImage, m_bDarkMode);
			bResult = TRUE;
		}
	};
	m_bInvalidateInternalBitmap = FALSE;

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CWndImage::CreateToolbarBitmap()
{
	BOOL			bResult = FALSE;

	if (m_pButtonToolbar && m_bInvalidateToolbarBitmap)
	{
		if (m_pToolbarImage)
			delete m_pToolbarImage;
		m_pToolbarImage = m_pButtonToolbar->GetImage();
		m_bInvalidateToolbarBitmap = FALSE;
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CWndImage::CreateZoomBitmap()
{
	BOOL			bResult = FALSE;

	if (m_bEnableZoom && m_bInvalidateZoomBitmap)
	{
		BOOL			bNeedRefresh = FALSE;

		if (m_pZoomImage)
			bNeedRefresh = TRUE;

		if (m_pZoomImage)
			delete m_pZoomImage;
		m_pZoomImage = nullptr;

		if ((m_zoomX < 3) && m_bmp.m_hObject)
		{
			CPoint			ptBitmap = m_ptZoomPos;
			CRect			rcClient;

			GetClientRect(&rcClient);

			if (rcClient.PtInRect(ptBitmap))
			{
				ScreenToBitmap(ptBitmap);

				CRect			src;
				CRect			dst;

				dst.left  = dst.top    = 0;
				dst.right = dst.bottom = 90;
				if (ptBitmap.x >= 15)
				{
					src.right = min(m_bmpSize.cx, ptBitmap.x + 15);
					src.left  = src.right-30;
				}
				else
				{
					src.left  = 0;
					src.right = 30;
				};
				if (ptBitmap.y >= 15)
				{
					src.bottom= min(m_bmpSize.cy, ptBitmap.y + 15);
					src.top   = src.bottom-30;;
				}
				else
				{
					src.top = 0;
					src.bottom = 30;
				};

				HBITMAP			hSubImage;
				CRect			rcOut;

				hSubImage = GetSubImage((HBITMAP)m_bmp.GetSafeHandle(), src);

				src.left = src.top = 0;
				src.right = src.bottom = 30;
				rcOut.left = rcOut.top = 0;
				rcOut.right = rcOut.bottom = 90;
				m_pZoomImage = ::GetBitmap(rcOut, hSubImage, src, dst, FALSE);

				DeleteObject(hSubImage);

				if (m_pZoomImage)
				{
					Graphics		graphics(m_pZoomImage);
					Pen				pen(Color(255, 255, 255), 1.0);

					graphics.DrawRectangle(&pen, dst.left, dst.top, dst.Width(), dst.Height());

					pen.SetColor(Color(170, 255, 0, 0));

					graphics.DrawLine(&pen, dst.left+dst.Width()/2-10, dst.top+dst.Height()/2, dst.left+dst.Width()/2-3, dst.Height()/2);
					graphics.DrawLine(&pen, dst.left+dst.Width()/2+10, dst.top+dst.Height()/2, dst.left+dst.Width()/2+3, dst.Height()/2);

					graphics.DrawLine(&pen, dst.left+dst.Width()/2, dst.top+dst.Height()/2-10, dst.left+dst.Width()/2, dst.Height()/2-3);
					graphics.DrawLine(&pen, dst.left+dst.Width()/2, dst.top+dst.Height()/2+10, dst.left+dst.Width()/2, dst.Height()/2+3);
				};
			};

			bResult = bNeedRefresh;
		}
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CWndImage::Draw4Corners(Graphics * pGraphics)
{
	BOOL			bResult = FALSE;
	CRect			rcClient;
	LONG			lWidth,
					lHeight;

	GetClientRect(&rcClient);

	lWidth = (rcClient.right-rcClient.left)/2.7;
	lHeight = (rcClient.bottom-rcClient.top)/2.7;

	if (lWidth > 100 && lHeight > 70)
	{
		CRect		rcOut, rcSrc, rcDst;
		LONG		lZoomWidth  = lWidth / m_zoomX,
					lZoomHeight = lHeight / m_zoomY;


		rcOut.left = rcOut.top = 0;
		rcOut.right = lWidth;
		rcOut.bottom = lHeight;

		// Top left corner
		rcSrc.left = rcSrc.top = 0;
		rcSrc.right = lZoomWidth;
		rcSrc.bottom = lZoomHeight;

		rcDst.left = rcDst.top = 0;
		rcDst.right = lWidth;
		rcDst.bottom = lHeight;

		Pen				pen(Color(255, 255, 255), 1.0);
		Bitmap *		pCorner;

		pCorner = ::GetBitmap(rcOut, (HBITMAP)m_bmp.GetSafeHandle(), rcSrc, rcDst, (m_zoomX<1));

		if (pCorner)
		{
			pGraphics->DrawImage(pCorner, 0, 0);
			pGraphics->DrawRectangle(&pen, 0, 0, lWidth, lHeight);

			delete pCorner;
		};

		// Top right corner
		rcSrc.left = m_bmpSize.cx - lZoomWidth-1;
		rcSrc.top = 0;
		rcSrc.right = m_bmpSize.cx-1;
		rcSrc.bottom = lZoomHeight;

		rcDst.left = rcDst.top = 0;
		rcDst.right = lWidth;
		rcDst.bottom = lHeight;

		pCorner = ::GetBitmap(rcOut, (HBITMAP)m_bmp.GetSafeHandle(), rcSrc, rcDst, (m_zoomX<1));

		if (pCorner)
		{
			pGraphics->DrawImage(pCorner, rcClient.right-lWidth, 0);
			pGraphics->DrawRectangle(&pen, rcClient.right-lWidth, 0, lWidth, lHeight);

			delete pCorner;
		};

		// bottom left corner
		rcSrc.left = 0;
		rcSrc.top = m_bmpSize.cy - lZoomHeight - 1;
		rcSrc.right = lZoomWidth;
		rcSrc.bottom = m_bmpSize.cy-1;

		rcDst.left = rcDst.top = 0;
		rcDst.right = lWidth;
		rcDst.bottom = lHeight;

		pCorner = ::GetBitmap(rcOut, (HBITMAP)m_bmp.GetSafeHandle(), rcSrc, rcDst, (m_zoomX<1));

		if (pCorner)
		{
			pGraphics->DrawImage(pCorner, 0, rcClient.bottom-lHeight);
			pGraphics->DrawRectangle(&pen, 0, rcClient.bottom-lHeight, lWidth, lHeight);

			delete pCorner;
		};

		// bottom right corner
		rcSrc.left = m_bmpSize.cx - lZoomWidth-1;
		rcSrc.top = m_bmpSize.cy - lZoomHeight - 1;
		rcSrc.right = m_bmpSize.cx-1;
		rcSrc.bottom = m_bmpSize.cy-1;

		rcDst.left = rcDst.top = 0;
		rcDst.right = lWidth;
		rcDst.bottom = lHeight;

		pCorner = ::GetBitmap(rcOut, (HBITMAP)m_bmp.GetSafeHandle(), rcSrc, rcDst, (m_zoomX<1));

		if (pCorner)
		{
			pGraphics->DrawImage(pCorner, (INT)(rcClient.right-lWidth), (INT)(rcClient.bottom-lHeight));
			pGraphics->DrawRectangle(&pen, (INT)(rcClient.right-lWidth), (INT)(rcClient.bottom-lHeight), (INT)lWidth, (INT)lHeight);

			delete pCorner;
		};

		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CWndImage::CreateBufferBitmap()
{
	BOOL			bResult = FALSE;
	BOOL			bNeedRefresh = FALSE;

	bNeedRefresh = m_bInvalidateOverlayBitmap;
	bNeedRefresh = CreateInternalBitmap() || bNeedRefresh;
	bNeedRefresh = CreateToolbarBitmap() || bNeedRefresh;
	bNeedRefresh = CreateZoomBitmap() || bNeedRefresh;

	m_bInvalidateZoomBitmap		= FALSE;
	m_bInvalidateInternalBitmap = FALSE;
	m_bInvalidateToolbarBitmap	= FALSE;

	if (bNeedRefresh || !m_pBufferImage)
	{
		CRect		rcClient;

		GetClientRect(&rcClient);

		if (m_pBufferImage)
		{
			// Check the size
			if ((m_pBufferImage->GetWidth() != rcClient.Width()) ||
				(m_pBufferImage->GetHeight() != rcClient.Height()))
			{
				delete m_pBufferImage;
				m_pBufferImage = nullptr;
			};
		};

		if (!m_pBufferImage)
		{
			m_pBufferImage = new Bitmap(rcClient.Width(), rcClient.Height(), PixelFormat24bppRGB);

			if (m_pBufferImage->GetLastStatus()!=Ok)
			{
				delete m_pBufferImage;
				m_pBufferImage = nullptr;
			};
		};

		if (m_pBufferImage)
		{
			Graphics		graphics(m_pBufferImage);

			REAL left(rcClient.left), top(rcClient.top), width(rcClient.Width()), height(rcClient.Height());
			
			const RectF destRect(left, top, rcClient.Width(), rcClient.Height());


			// Draw the internal Bitmap
			//graphics.DrawImage(m_pBaseImage, 0, 0);
			graphics.DrawImage(
				m_pBaseImage,
				destRect,
				left,		// X
				top,		// Y
				width,
				height,
				UnitPixel,
				nullptr,
				nullptr,
				nullptr
			);


			if (m_4Corners)
				Draw4Corners(&graphics);
			else
			{
				// Then the overlay layer is available
				if (m_bInvalidateOverlayBitmap)
				{
					m_bInvalidateOverlayBitmap  = FALSE;
					if (m_pOverlayImage)
						delete m_pOverlayImage;
					m_pOverlayImage = nullptr;
					if (m_pImageSink && m_bmp.m_hObject)
						m_pOverlayImage = m_pImageSink->GetOverlayImage(rcClient);
				};
				if (m_pOverlayImage && m_bmp.m_hObject)
					graphics.DrawImage(m_pOverlayImage, RectF(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()));

				// Then the zoom if available
				if (m_pZoomImage && m_bmp.m_hObject)
					graphics.DrawImage(m_pZoomImage, (INT)m_rcZoom.left, m_rcZoom.top);

				// then the toolbar
				if (m_pToolbarImage && m_bmp.m_hObject)
				{
					CRect			rcToolbar;

					GetToolbarRect(rcToolbar);

					// Check the cursor position against the toolbar position
					CPoint			pt;
					float			fTransparency = 0.50;

					GetCursorPos(&pt);
					ScreenToClient(&pt);

					m_bOnToolbar = FALSE;
					if (rcToolbar.PtInRect(pt) && !m_bCaptured)
					{
						fTransparency = 1.0;
						m_bOnToolbar = TRUE;
					};

					ImageAttributes		imgAttr;
					ColorMatrix			TransMatrix[] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
														0.0f, 0.0f, 0.0f, fTransparency, 0.0f,
														0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
					Rect				rcfDst(rcToolbar.left, rcToolbar.top, rcToolbar.Width(), rcToolbar.Height());

					imgAttr.SetColorMatrix(TransMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

					graphics.DrawImage(m_pToolbarImage, rcfDst, 0, 0, rcToolbar.Width(), rcToolbar.Height(), UnitPixel, &imgAttr, nullptr, nullptr);

					if (m_bOnToolbar && m_pButtonToolbar)
					{
						Image *			pTooltipImage;
						CRect			rcTooltip;
						CPoint			ptTooltip;

						pTooltipImage = m_pButtonToolbar->GetTooltipImage(rcTooltip);
						if (pTooltipImage)
						{
							// Move the rectangle
							ptTooltip.x = rcToolbar.left - rcTooltip.Width();
							ptTooltip.y = rcToolbar.top + rcTooltip.top;
							graphics.DrawImage(pTooltipImage, (INT)ptTooltip.x, ptTooltip.y);
							delete pTooltipImage;
						};
					};
				};
			};
			graphics.Flush(FlushIntentionSync);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CWndImage::OnEraseBkgnd(CDC* pDC)
{
	CRect			r;
	CreateBufferBitmap();

	GetClientRect(&r);

	if (m_pBufferImage && m_pBufferImage->GetLastStatus()==Ok)
	{
		Graphics		graphics(pDC->GetSafeHdc());

		if (graphics.GetLastStatus()==Ok)
			graphics.DrawImage(m_pBufferImage, 0, 0);
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

void CWndImage::OnPaint()
{
	DWORD style = GetStyle();
	if (!(style & WS_VISIBLE)) return;

	CRect			r;
	CreateBufferBitmap();

	GetClientRect(&r);

	CPaintDC dc(this);
	if (m_pBufferImage  && m_pBufferImage->GetLastStatus()==Ok)
	{
		Graphics		graphics(dc.GetSafeHdc());

		if (graphics.GetLastStatus()==Ok)
			graphics.DrawImage(m_pBufferImage, 0, 0);
	};
}

// ==================================================================
//  Create
// ------------------------------------------------------------------

BOOL CWndImage::Create(RECT const & r, CWnd * parent, UINT id, DWORD dwStyle)
{
	BOOL ok = CWnd::Create(nullptr, nullptr, dwStyle, r, parent, id, nullptr);
	return ok;
}


// ==================================================================
//  CreateFromStatic
// ------------------------------------------------------------------

BOOL CWndImage::CreateFromStatic(CWnd * sc)
{
	_ASSERTE(!::IsWindow(m_hWnd));          // image control already created
	if (!sc || !::IsWindow(sc->m_hWnd)) return false;
	CWnd * dlg = sc->GetParent();
	if (!dlg || !::IsWindow(dlg->m_hWnd)) return false;

	CRect r;
	sc->GetWindowRect(&r);
	dlg->ScreenToClient(&r);
	CString s;
	DWORD style   = sc->GetStyle() | WS_TABSTOP;
	DWORD exstyle = sc->GetExStyle();
	sc->GetWindowText(s);
	UINT dlgID = sc->GetDlgCtrlID();

	HBITMAP bmp = (HBITMAP) sc->SendMessage(STM_GETIMAGE, IMAGE_BITMAP, 0);
	if (bmp)
		sc->SendMessage(STM_SETIMAGE, IMAGE_BITMAP, 0);

	sc->DestroyWindow();
	CreateEx(exstyle, nullptr, s,  style, r, dlg, dlgID);
	SetFocus();

	if (bmp)
	{
		SetImg(bmp, false);
		SetBltMode(bltFitXY);
	}
	return true;
}


// ==================================================================
//  SetImage
// ------------------------------------------------------------------

void CWndImage::SetImg(HBITMAP bmp, bool shared)
{
	if ((HBITMAP)m_bmp.m_hObject != bmp)
	{
		HGDIOBJ prev = m_bmp.Detach();

		if (prev && !m_shared)
			DeleteObject(prev);

		m_bmp.Attach(bmp);
		m_shared = shared;

		if (bmp != 0)
		{
#pragma warning (push, 3)
			BITMAP bmp;
#pragma warning (pop)
			m_bmp.GetBitmap(&bmp);
			m_bmpSize.cx = bmp.bmWidth;
			m_bmpSize.cy = bmp.bmHeight;
		}
		else
		{
			m_bmpSize = CSize(0, 0);
			if (m_pBaseImage)
			{
				delete m_pBaseImage;
				m_pBaseImage = nullptr;
			}

			if (m_pBufferImage)
			{
				delete m_pBufferImage;
				m_pBufferImage = nullptr;
			}
		}

		SetSourceRect();      // use entire new image
		Recalc();
	};
	return;
}

// ==================================================================
//  SetImage Clones
// ------------------------------------------------------------------

void CWndImage::SetImg(CBitmap * bmp)
{
	SetImg(bmp ? (HBITMAP) (bmp->m_hObject) : 0);
}

/* ------------------------------------------------------------------- */

bool CWndImage::SetImg(LPCTSTR resID, HINSTANCE instance)
{
	if (!instance)
		instance = AfxGetResourceHandle();

	HBITMAP bmp = ::LoadBitmap(instance, resID);
	SetImg(bmp, false);
	return bmp != 0;
}

/* ------------------------------------------------------------------- */

bool CWndImage::SetImg(UINT resID, HINSTANCE instance)
{
	return SetImg(MAKEINTRESOURCE(resID), instance);
}

/* ------------------------------------------------------------------- */

bool CWndImage::SetImgFile(LPCTSTR fileName)
{
	HBITMAP bmp = (HBITMAP) ::LoadImage(nullptr, fileName, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	SetImg(bmp);
	return bmp != 0;
}

// ==================================================================
//  SetBltMode
// ------------------------------------------------------------------

void CWndImage::SetBltMode(int mode)
{
	if (mode <=0 || mode >  blt_MaxMode) mode = bltNormal;
	if (mode == m_bltMode) return;
	m_bltMode = mode;
	Recalc();
}

// ==================================================================
//  SetAlign
// ------------------------------------------------------------------

void CWndImage::SetAlign(int alignX, int alignY)
{
	if (alignX < 0 || alignX > blt_MaxAlign || alignX == m_alignX) alignX = 0;
	if (alignY < 0 || alignY > blt_MaxAlign || alignY == m_alignY) alignY = 0;

	if (alignX == 0 && alignY == 0) return;  // no change

	if (alignX) m_alignX = alignX;
	if (alignY) m_alignY = alignY;
	Recalc();
}

// ==================================================================
//  SetSourceRect
// ------------------------------------------------------------------

void CWndImage::SetSourceRect(RECT const & r)
{
	CRect bmpRect(0,0, m_bmpSize.cx, m_bmpSize.cy);
	m_srcRect.IntersectRect(&r, &bmpRect);
	Recalc();
}

// ==================================================================
//  SetSourceRect
// ------------------------------------------------------------------

void CWndImage::SetSourceRect()
{
	m_srcRect.SetRect(0,0, m_bmpSize.cx, m_bmpSize.cy);
	Recalc();
}


// ==================================================================
//  Recalc
// ------------------------------------------------------------------

void CWndImage::Recalc(bool invalidate)
{
	if (!::IsWindow(m_hWnd))
		return;

	CRect & src = m_srcRect;
	CRect & dst = m_dstRect;
	CRect r;

	GetClientRect(&r);
	CSize wndSize = r.Size();

	if (wndSize.cx == 0 || wndSize.cy == 0 || src.IsRectEmpty())
		dst.SetRectEmpty();
	else
	{
		dst.left = dst.top = 0;
		switch (m_bltMode)
		{
		default           :
		case bltNormal    :  dst.right = src.Width(); dst.bottom = src.Height(); break;
		case bltStretch   :  dst.right = wndSize.cx; dst.bottom = wndSize.cy; break;
		case bltCustom    :  dst.right  = (int)(src.Width() * m_zoomX);
			dst.bottom = (int)(src.Height() * m_zoomY);
			break;

		case bltFitX      :
		case bltFitY      :
		case bltFitXY     :
		case bltFitSm     : {
			double zoom = 1;
			double zoomX = ((double) wndSize.cx) / src.Width();
			double zoomY = ((double) wndSize.cy) / src.Height();

			if (m_bltMode == bltFitX)
				zoom = zoomX;
			else if (m_bltMode == bltFitY)
				zoom = zoomY;
			else    // for stretchXY take smaller, for stretchSm take larger:
				zoom = ((m_bltMode == bltFitXY) ^ (zoomX > zoomY)) ? zoomX : zoomY;

			m_zoomX = m_zoomY = zoom;       // so user can query these values

			dst.right  = (int)(src.Width() * zoom);
			dst.bottom = (int)(src.Height() * zoom);
			break;
							}
		case bltTile      :   break;

		}

		switch (m_alignX)
		{
		case bltCenter :
			m_origin.x = (wndSize.cx-dst.Width()) / 2;
			break;
		case bltRight  :
			m_origin.x =  wndSize.cx-dst.Width();
			break;
		case bltLeft   :
			m_origin.x =  0;
			break;
		}
		dst.left += m_origin.x;
		dst.right += m_origin.x;

		switch (m_alignY)
		{
		case bltCenter :
			m_origin.y = (wndSize.cy-dst.Height()) / 2;
			break;
		case bltRight  :
			m_origin.y =  wndSize.cy-dst.Height();
			break;
		case bltTop    :
			m_origin.y = 0;
			break;
		}
		dst.top += m_origin.y;
		dst.bottom += m_origin.y;
	}

	if (m_bltMode == bltTile)
	{
		dst.SetRect(0,0, wndSize.cx, wndSize.cy);
	}

	if (invalidate)
	{
		m_bInvalidateInternalBitmap = TRUE;
		m_bInvalidateOverlayBitmap	= TRUE;
		Invalidate();
	};
}

// ------------------------------------------------------------------

void CWndImage::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Recalc();
}

// ==================================================================
//  SetZoom, origin
// ------------------------------------------------------------------

void CWndImage::SetZoom(double zoomX, double zoomY)
{
	m_zoomX = zoomX;
	m_zoomY = zoomY;
	m_bltMode = bltCustom;
	Recalc();
}

void CWndImage::SetZoom(double zoom)
{
	SetZoom(zoom, zoom);
}

void CWndImage::SetOrigin(int origX, int origY)
{
	SetOriginX(origX);
	SetOriginY(origY);
}

void CWndImage::SetOriginX(int origX)
{
	m_origin.x = origX;
	m_alignX = bltCustom;
	Recalc();
}

void CWndImage::SetOriginY(int origY)
{
	m_origin.y = origY;
	m_alignY = bltCustom;
	Recalc();

}

// ==================================================================
//  GetBitmap
// ------------------------------------------------------------------

HBITMAP CWndImage::GetBitmap(bool detach)
{
	HBITMAP ret = (HBITMAP) m_bmp.m_hObject;
	if (detach)
	{
		m_shared = true;
		//SetImg((HBITMAP)0);
	}
	return ret;
}

// ==================================================================
//  SetbackBrush
// ------------------------------------------------------------------

void CWndImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL				bPass = TRUE;

	SetFocus();
	if (m_pButtonToolbar && m_bmp.m_hObject && !m_4Corners)
	{
		CRect			rcToolbar;
		// BOOL			bRefresh = FALSE;

		GetToolbarRect(rcToolbar);
		if (rcToolbar.PtInRect(point))
		{
			if (m_pButtonToolbar->OnLButtonDown(point.x-rcToolbar.left, point.y-rcToolbar.top))
				RefreshToolbar();
			bPass = FALSE;
		};
	};

	if (bPass && m_pImageSink && m_bmp.m_hObject && !m_4Corners)
	{
		if (m_pImageSink->Image_OnLButtonDown(point.x, point.y))
		{
			RefreshOverlay();
			m_bCaptured = TRUE;
			SetCapture();
		};
	};

	CWnd::OnLButtonDown(nFlags, point);
}

// ------------------------------------------------------------------

void CWndImage::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL				bPass = TRUE;

	ReleaseCapture();

	if (!m_bCaptured && m_pButtonToolbar && m_bmp.m_hObject && !m_4Corners)
	{
		CRect			rcToolbar;

		GetToolbarRect(rcToolbar);
		if (rcToolbar.PtInRect(point))
		{
			if (m_pButtonToolbar->OnLButtonUp(point.x-rcToolbar.left, point.y-rcToolbar.top))
				RefreshToolbar();
			bPass = FALSE;
		};
	};

	if (bPass && m_pImageSink && m_bmp.m_hObject && !m_4Corners)
	{
		if (m_pImageSink->Image_OnLButtonUp(point.x, point.y))
		{
			RefreshOverlay();
			m_bCaptured = FALSE;
			ReleaseCapture();
		};
	};

	CWnd::OnLButtonUp(nFlags, point);
}

// ------------------------------------------------------------------

void CWndImage::OnMouseMove(UINT nFlags, CPoint point)
{
	BOOL				bPass = TRUE;
	CWnd *				pFocus = GetFocus();

	if (!pFocus || pFocus->m_hWnd != m_hWnd)
		SetFocus();

	if (!m_bTrackTimerSet)
	{
		SetTimer(TIMERID_TRACKLEAVE, 100, nullptr);
		m_bTrackTimerSet = TRUE;
	};

	if (!m_bCaptured && m_pButtonToolbar && m_bmp.m_hObject && !m_4Corners)
	{
		CRect			rcToolbar;

		GetToolbarRect(rcToolbar);
		if (rcToolbar.PtInRect(point))
		{
			if (m_pButtonToolbar->OnMouseMove(point.x-rcToolbar.left, point.y-rcToolbar.top))
				RefreshToolbar();
			bPass = FALSE;
		}
		else if (m_bOnToolbar)
		{
			if (m_pButtonToolbar->OnMouseLeave())
				RefreshToolbar();
		};
	};

	if (bPass && m_pImageSink && m_bmp.m_hObject && !m_4Corners)
	{
		if (m_pImageSink->Image_OnMouseMove(point.x, point.y))
			RefreshOverlay();
	};

	if (bPass && m_bmp.m_hObject && !m_4Corners)
	{
		// Update the small zoom position
		UpdateZoomPosition(point);
	};

	CWnd::OnMouseMove(nFlags, point);
}

// ------------------------------------------------------------------

void CWndImage::OnRButtonDown(UINT nFlags, CPoint point)
{
	//BOOL				bPass = TRUE;

	if (m_pButtonToolbar && m_bmp.m_hObject && !m_4Corners)
	{
		CRect			rcToolbar;
		// BOOL			bRefresh = FALSE;

		GetToolbarRect(rcToolbar);
		if (rcToolbar.PtInRect(point))
		{
			if (m_pButtonToolbar->OnRButtonDown(point.x-rcToolbar.left, point.y-rcToolbar.top))
				RefreshToolbar();
		};
	};

	CWnd::OnRButtonDown(nFlags, point);
}

// ------------------------------------------------------------------

void CWndImage::RefreshZoom()
{
	if (m_bEnableZoom && m_bmp.m_hObject)
	{
		m_bInvalidateZoomBitmap = TRUE;
		CreateZoomBitmap();

		BOOL				bNeedRefresh = FALSE;
		CDC *				pdc;
		CRect				rcOldZoom;
		CRect				rcClient;

		GetClientRect(&rcClient);

		pdc= GetDC();
		{
			Graphics	graphics(pdc->GetSafeHdc());

			rcOldZoom = m_rcZoom;

			if (m_pZoomImage)
			{
				if ((m_ptZoomPos.x > 150) || (m_ptZoomPos.y > 150))
				{
					graphics.DrawImage(m_pZoomImage, 0, 0);
					m_rcZoom.top = 0;
				}
				else
				{
					m_rcZoom.top = rcClient.Height()-90;
					graphics.DrawImage(m_pZoomImage, 0, m_rcZoom.top);
				};
			}
			else
				bNeedRefresh = TRUE;

			m_rcZoom.left  = 0;
			m_rcZoom.right = 90;
			m_rcZoom.bottom= m_rcZoom.top+90;

			if (bNeedRefresh || (m_rcZoom != rcOldZoom))
			{
				InvalidateRect(&rcOldZoom, TRUE);
			};
		}
		ReleaseDC(pdc);
	}
};

// ------------------------------------------------------------------

void CWndImage::RefreshToolbar()
{
	if (m_pButtonToolbar && m_bmp.m_hObject)
	{
		m_bInvalidateToolbarBitmap = TRUE;
		CreateBufferBitmap();

		BOOL				bNeedRefresh = FALSE;
		CDC *				pdc;
		CRect				rcToolbar;

		GetToolbarRect(rcToolbar);

		pdc= GetDC();
		{
			Graphics	graphics(pdc->GetSafeHdc());

			if (m_pToolbarImage)
			{
				//if (m_bOnToolbar)
					graphics.DrawImage(m_pBufferImage, 0, 0);
				//else
					//graphics.DrawImage(m_pBufferImage, rcToolbar.left, rcToolbar.top, rcToolbar.left, rcToolbar.top, rcToolbar.Width(), rcToolbar.Height(), UnitPixel);
			}
			else
				bNeedRefresh = TRUE;

			if (bNeedRefresh)
				InvalidateRect(&rcToolbar, TRUE);
		}
		ReleaseDC(pdc);
	};
};

// ------------------------------------------------------------------

void CWndImage::RefreshOverlay()
{
	m_bInvalidateOverlayBitmap = TRUE;

	if (m_bmp.m_hObject)
	{
		CreateBufferBitmap();

		{
			CDC *				pdc;

			pdc= GetDC();
			{
				Graphics	graphics(pdc->GetSafeHdc());

				graphics.DrawImage(m_pBufferImage, 0, 0);
			}
			ReleaseDC(pdc);
		};
	};
};

// ------------------------------------------------------------------

void CWndImage::ReleaseZoom()
{
	if (m_pZoomImage)
	{
		m_ptZoomPos.x = m_ptZoomPos.y = -10;
		m_bInvalidateZoomBitmap = TRUE;
		InvalidateRect(&m_rcZoom, TRUE);
	};
};

// ------------------------------------------------------------------

void CWndImage::OnTimer([[maybe_unused]] UINT_PTR nIDEvent)
{
	CRect			rcWindow;
	CPoint			ptMouse;

	GetWindowRect(&rcWindow);
	GetCursorPos(&ptMouse);

	if (!rcWindow.PtInRect(ptMouse))
	{
		ReleaseZoom();
		KillTimer(TIMERID_TRACKLEAVE);
		m_bTrackTimerSet = FALSE;

		if (m_pImageSink)
		{
			if (m_pImageSink->Image_OnMouseLeave())
				RefreshOverlay();
		};
		if (m_pButtonToolbar)
		{
			if (m_bOnToolbar && m_pButtonToolbar->OnMouseLeave())
				RefreshToolbar();
		};
	};
};

// ------------------------------------------------------------------

void CWndImage::OnKeyDown(UINT nChar, [[maybe_unused]] UINT nRepCnt, [[maybe_unused]] UINT nFlags)
{
	POINT				ptCursor;

	switch (nChar)
	{
	case VK_UP:
		GetCursorPos(&ptCursor);
		SetCursorPos(ptCursor.x, ptCursor.y-1);
		break;
	case VK_DOWN:
		GetCursorPos(&ptCursor);
		SetCursorPos(ptCursor.x, ptCursor.y+1);
		break;
	case VK_LEFT:
		GetCursorPos(&ptCursor);
		SetCursorPos(ptCursor.x-1, ptCursor.y);
		break;
	case VK_RIGHT:
		GetCursorPos(&ptCursor);
		SetCursorPos(ptCursor.x+1, ptCursor.y);
		break;
	case VK_PRIOR :
		// Zoom In
		GetCursorPos(&ptCursor);
		OnMouseWheel(0, 10, ptCursor);
		break;
	case VK_NEXT :
		// Zoom Out
		GetCursorPos(&ptCursor);
		OnMouseWheel(0, -10, ptCursor);
		break;
	};
};

// ------------------------------------------------------------------

BOOL CWndImage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		OnKeyDown(pMsg->wParam, 0, 0);
	};
	return CWnd::PreTranslateMessage(pMsg);
}

// ------------------------------------------------------------------

void CWndImage::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
}

// ------------------------------------------------------------------

BOOL CWndImage::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// Zoom in/out
	ScreenToClient(&pt);
	if (zDelta  < 0)
	{
		if (m_zoomX > 1)
		{
			// zoom out
			CPoint			ptCenter;
			CRect			rcClient;

			GetClientRect(&rcClient);

			// The point is the new center of the picture
			ptCenter.x = (pt.x - m_origin.x)/m_zoomX;
			ptCenter.y = (pt.y - m_origin.y)/m_zoomY;

			m_zoomX = m_zoomX / 1.5;
			m_zoomY = m_zoomY / 1.5;

			m_origin.x = rcClient.Width()/2.0-ptCenter.x * m_zoomX;
			m_origin.y = rcClient.Height()/2.0-ptCenter.y * m_zoomY;

			m_alignX  = bltCustom;
			m_alignY  = bltCustom;
			m_bltMode = bltCustom;
			Recalc();
			Invalidate();
		}
		else
		{
			// Return to normal mode
			SetBltMode(bltFitXY);
			SetAlign(bltCenter, bltCenter);
		};
	}
	else
	{
		if (m_zoomX < 10)
		{
			// zoom in
			CPoint			ptCenter;
			CRect			rcClient;

			GetClientRect(&rcClient);

			// The point is the new center of the picture
			ptCenter.x = (pt.x - m_origin.x)/m_zoomX;
			ptCenter.y = (pt.y - m_origin.y)/m_zoomY;

			m_zoomX = m_zoomX * 1.5;
			m_zoomY = m_zoomY * 1.5;

			// Compute the new origin
			m_origin.x = rcClient.Width()/2.0-ptCenter.x * m_zoomX;
			m_origin.y = rcClient.Height()/2.0-ptCenter.y * m_zoomY;

			m_alignX  = bltCustom;
			m_alignY  = bltCustom;
			m_bltMode = bltCustom;
			Recalc();
			Invalidate();
		};
	};

	// Notify parent
	CWnd* pwndParent = GetParent();

	if (pwndParent)
	{
		NMHDR		hdr;

		hdr.hwndFrom = GetSafeHwnd();
		hdr.idFrom   = GetDlgCtrlID();
		hdr.code	 = NM_ZOOM;
		pwndParent->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&hdr);
	}


	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

// ------------------------------------------------------------------
