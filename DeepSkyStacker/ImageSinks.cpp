#include <stdafx.h>
#include "ImageSinks.h"
#include "RegisterEngine.h"
#include "BackgroundCalibration.h"
#include "commonresource.h"

/* ------------------------------------------------------------------- */

HCURSOR	CSelectRectSink::GetCursorFromMode(SELECTRECTMODE Mode)
{
	HCURSOR			hResult = nullptr;

	switch (Mode)
	{
	case SRM_MOVE :
		hResult = ::LoadCursor(nullptr, IDC_SIZEALL);
		break;

	case SRM_MOVETOP :
	case SRM_MOVEBOTTOM :
		hResult = ::LoadCursor(nullptr, IDC_SIZENS);
		break;

	case SRM_MOVELEFT :
	case SRM_MOVERIGHT :
		hResult = ::LoadCursor(nullptr, IDC_SIZEWE);
		break;

	case SRM_MOVETOPLEFT :
	case SRM_MOVEBOTTOMRIGHT :
		hResult = ::LoadCursor(nullptr, IDC_SIZENWSE);
		break;

	case SRM_MOVETOPRIGHT :
	case SRM_MOVEBOTTOMLEFT :
		hResult = ::LoadCursor(nullptr, IDC_SIZENESW);
		break;
	};

	return hResult;
};

/* ------------------------------------------------------------------- */

SELECTRECTMODE	CSelectRectSink::GetModeFromPosition(long lX, long lY)
{
	SELECTRECTMODE	Result = SRM_NONE;
	CRect			rcScreen;

	// Transform the select rectangle coordinates to
	//
	rcScreen = m_rcSelect;
	m_pImage->BitmapToScreen(rcScreen);

	if (m_rcSelect.IsRectEmpty())
		Result = SRM_NONE;
	else
	{
		// Near the corners
		CRect		rcCorner;

		if (rcScreen.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVE;

		// Top
		rcCorner.left = rcScreen.left;		rcCorner.right  = rcScreen.right;
		rcCorner.top  = rcScreen.top-1;		rcCorner.bottom = rcScreen.top+1;

		if (rcCorner.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVETOP;

		// Bottom
		rcCorner.left = rcScreen.left;		rcCorner.right  = rcScreen.right;
		rcCorner.top  = rcScreen.bottom-1;	rcCorner.bottom = rcScreen.bottom+1;

		if (rcCorner.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVEBOTTOM;

		// Left
		rcCorner.left = rcScreen.left-1;	rcCorner.right  = rcScreen.left+1;
		rcCorner.top  = rcScreen.top;		rcCorner.bottom = rcScreen.bottom;

		if (rcCorner.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVELEFT;

		// Right
		rcCorner.left = rcScreen.right-1;	rcCorner.right  = rcScreen.right+1;
		rcCorner.top  = rcScreen.top;		rcCorner.bottom = rcScreen.bottom;

		if (rcCorner.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVERIGHT;

		// Top left corner
		rcCorner.left = rcScreen.left-1;	rcCorner.right  = rcScreen.left+3;
		rcCorner.top  = rcScreen.top-1;		rcCorner.bottom = rcScreen.top+3;

		if (rcCorner.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVETOPLEFT;

		// Top right corner
		rcCorner.left = rcScreen.right-3;	rcCorner.right  = rcScreen.right+1;
		rcCorner.top  = rcScreen.top-1;		rcCorner.bottom = rcScreen.top+3;

		if (rcCorner.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVETOPRIGHT;

		// Bottom left corner
		rcCorner.left = rcScreen.left-1;	rcCorner.right  = rcScreen.left+3;
		rcCorner.top  = rcScreen.bottom-3;	rcCorner.bottom = rcScreen.bottom+1;

		if (rcCorner.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVEBOTTOMLEFT;

		// bottom right corner
		rcCorner.left = rcScreen.right-3;	rcCorner.right  = rcScreen.right+1;
		rcCorner.top  = rcScreen.bottom-3;	rcCorner.bottom = rcScreen.bottom+1;

		if (rcCorner.PtInRect(CPoint(lX, lY)))
			Result = SRM_MOVEBOTTOMRIGHT;
	};

	return Result;
};

/* ------------------------------------------------------------------- */

void	CSelectRectSink::UpdateSelectRect()
{
	m_rcSelect = m_rcStart;
	double				fDeltaX = m_fXEnd-m_fXStart;
	double				fDeltaY = m_fYEnd-m_fYStart;

	switch (m_Mode)
	{
	case SRM_CREATE	:
		m_rcSelect.left  = std::max(std::min(m_fXStart, m_fXEnd), 0.0);
		m_rcSelect.right = std::min(std::max(m_fXStart, m_fXEnd), static_cast<double>(m_pImage->GetImgSizeX()));
		m_rcSelect.top   = std::max(std::min(m_fYStart, m_fYEnd), 0.0);
		m_rcSelect.bottom= std::min(std::max(m_fYStart, m_fYEnd), static_cast<double>(m_pImage->GetImgSizeY()));
		break;
	case SRM_MOVE :
		if (fDeltaX<0) // Move to the left - check left first
		{
			if (-fDeltaX > m_rcSelect.left)
				fDeltaX = -m_rcSelect.left;
		}
		else // Move to the left - check right first
		{
			if (fDeltaX > m_pImage->GetImgSizeX()-m_rcSelect.right)
				fDeltaX = m_pImage->GetImgSizeX()-m_rcSelect.right;
		};
		m_rcSelect.left  += fDeltaX;
		m_rcSelect.right += fDeltaX;

		if (fDeltaY<0) // Move to the top - check top first
		{
			if (-fDeltaY > m_rcSelect.top)
				fDeltaY = -m_rcSelect.top;
		}
		else // Move to the bottom - check bottom first
		{
			if (fDeltaY > m_pImage->GetImgSizeY()-m_rcSelect.bottom)
				fDeltaY = m_pImage->GetImgSizeY()-m_rcSelect.bottom;
		};
		m_rcSelect.top    += fDeltaY;
		m_rcSelect.bottom += fDeltaY;
		break;
	case SRM_MOVETOP :
		// Only fDeltaY is used
		if (fDeltaY > m_rcSelect.Height()-1)
			fDeltaY = m_rcSelect.Height()-1;
		m_rcSelect.top += fDeltaY;
		m_rcSelect.top = std::max(0L, m_rcSelect.top);
		break;
	case SRM_MOVEBOTTOM	:
		// Only fDeltaY is used
		if (-fDeltaY > m_rcSelect.Height()-1)
			fDeltaY = -m_rcSelect.Height()+1;
		m_rcSelect.bottom += fDeltaY;
		m_rcSelect.bottom = std::min(static_cast<decltype(m_rcSelect.bottom)>(m_pImage->GetImgSizeY()), m_rcSelect.bottom);
		break;
	case SRM_MOVELEFT :
		// Only fDeltaX is used
		if (fDeltaX > m_rcSelect.Width()-1)
			fDeltaX = m_rcSelect.Width()-1;
		m_rcSelect.left += fDeltaX;
		m_rcSelect.left = std::max(0L, m_rcSelect.left);
		break;
	case SRM_MOVERIGHT :
		// Only fDeltaX is used
		if (-fDeltaX > m_rcSelect.Width()-1)
			fDeltaX = -m_rcSelect.Width()+1;
		m_rcSelect.right += fDeltaX;
		m_rcSelect.right = std::min(static_cast<decltype(m_rcSelect.right)>(m_pImage->GetImgSizeX()), m_rcSelect.right);
		break;
	case SRM_MOVETOPLEFT :
		// Only fDeltaY is used
		if (fDeltaY > m_rcSelect.Height()-1)
			fDeltaY = m_rcSelect.Height()-1;
		m_rcSelect.top += fDeltaY;
		m_rcSelect.top = std::max(0L, m_rcSelect.top);
		// Only fDeltaX is used
		if (fDeltaX > m_rcSelect.Width()-1)
			fDeltaX = m_rcSelect.Width()-1;
		m_rcSelect.left += fDeltaX;
		m_rcSelect.left = std::max(0L, m_rcSelect.left);
		break;
	case SRM_MOVETOPRIGHT :
		// Only fDeltaY is used
		if (fDeltaY > m_rcSelect.Height()-1)
			fDeltaY = m_rcSelect.Height()-1;
		m_rcSelect.top += fDeltaY;
		m_rcSelect.top = std::max(0L, m_rcSelect.top);
		// Only fDeltaX is used
		if (-fDeltaX > m_rcSelect.Width()-1)
			fDeltaX = -m_rcSelect.Width()+1;
		m_rcSelect.right += fDeltaX;
		m_rcSelect.right = std::min(static_cast<decltype(m_rcSelect.right)>(m_pImage->GetImgSizeX()), m_rcSelect.right);
		break;
	case SRM_MOVEBOTTOMLEFT :
		// Only fDeltaX is used
		if (fDeltaX > m_rcSelect.Width()-1)
			fDeltaX = m_rcSelect.Width()-1;
		m_rcSelect.left += fDeltaX;
		m_rcSelect.left = std::max(0L, m_rcSelect.left);
		// Only fDeltaY is used
		if (-fDeltaY > m_rcSelect.Height()-1)
			fDeltaY = -m_rcSelect.Height()+1;
		m_rcSelect.bottom += fDeltaY;
		m_rcSelect.bottom = std::min(static_cast<decltype(m_rcSelect.bottom)>(m_pImage->GetImgSizeY()), m_rcSelect.bottom);
		break;
	case SRM_MOVEBOTTOMRIGHT :
		// Only fDeltaX is used
		if (-fDeltaX > m_rcSelect.Width()-1)
			fDeltaX = -m_rcSelect.Width()+1;
		m_rcSelect.right += fDeltaX;
		m_rcSelect.right = std::min(static_cast<decltype(m_rcSelect.right)>(m_pImage->GetImgSizeX()), m_rcSelect.right);
		// Only fDeltaY is used
		if (-fDeltaY > m_rcSelect.Height()-1)
			fDeltaY = -m_rcSelect.Height()+1;
		m_rcSelect.bottom += fDeltaY;
		m_rcSelect.bottom = std::min(static_cast<decltype(m_rcSelect.bottom)>(m_pImage->GetImgSizeY()), m_rcSelect.bottom);
		break;
	};

	CRect				rc2xDrizzle,
						rc3xDrizzle;

	GetDrizzleRectangles(rc2xDrizzle, rc3xDrizzle);

	if (!rc2xDrizzle.IsRectEmpty())
	{
		// If the width/height is within 5 pixels - make it match
		if (labs(rc2xDrizzle.Width()-m_rcSelect.Width()) < 5/m_pImage->GetZoomX())
		{
			m_rcSelect.left		= rc2xDrizzle.left;
			m_rcSelect.right	= rc2xDrizzle.right;
		};
		if (labs(rc2xDrizzle.Height()-m_rcSelect.Height()) < 5/m_pImage->GetZoomX())
		{
			m_rcSelect.top		= rc2xDrizzle.top;
			m_rcSelect.bottom	= rc2xDrizzle.bottom;
		};
	};

	if (!rc3xDrizzle.IsRectEmpty())
	{
		// If the width/height is within 5 pixels - make it match
		if (labs(rc3xDrizzle.Width()-m_rcSelect.Width()) < 5/m_pImage->GetZoomX())
		{
			m_rcSelect.left		= rc3xDrizzle.left;
			m_rcSelect.right	= rc3xDrizzle.right;
		};
		if (labs(rc3xDrizzle.Height()-m_rcSelect.Height()) < 5/m_pImage->GetZoomY())
		{
			m_rcSelect.top		= rc3xDrizzle.top;
			m_rcSelect.bottom	= rc3xDrizzle.bottom;
		};
	};
};

/* ------------------------------------------------------------------- */

void CSelectRectSink::GetDrizzleRectangles(CRect & rc2xDrizzle, CRect & rc3xDrizzle)
{
	if (m_bShowDrizzle)
	{
		rc2xDrizzle.left   = rc2xDrizzle.top = 0;
		rc2xDrizzle.right  = m_pImage->GetImgSizeX()/2;
		rc2xDrizzle.bottom = m_pImage->GetImgSizeY()/2;

		rc3xDrizzle.left   = rc3xDrizzle.top = 0;
		rc3xDrizzle.right  = m_pImage->GetImgSizeX()/3;
		rc3xDrizzle.bottom = m_pImage->GetImgSizeY()/3;

		switch (m_Mode)
		{
		case SRM_CREATE :
		case SRM_MOVEBOTTOM	:
		case SRM_MOVERIGHT :
		case SRM_MOVEBOTTOMRIGHT :
			// Upper left corner is used as anchor
			rc2xDrizzle.OffsetRect(m_rcSelect.left, m_rcSelect.top);
			rc3xDrizzle.OffsetRect(m_rcSelect.left, m_rcSelect.top);
			break;
		case SRM_MOVE :
			// No need to show the drizzle rectangles
			rc2xDrizzle.SetRectEmpty();
			rc3xDrizzle.SetRectEmpty();
			break;
		case SRM_MOVETOP :
		case SRM_MOVETOPRIGHT :
			// Lower left corner is used as anchor
			rc2xDrizzle.OffsetRect(m_rcSelect.left, 0);
			rc3xDrizzle.OffsetRect(m_rcSelect.left, 0);
			rc2xDrizzle.OffsetRect(0, m_rcSelect.bottom-rc2xDrizzle.Height());
			rc3xDrizzle.OffsetRect(0, m_rcSelect.bottom-rc3xDrizzle.Height());
			break;
		case SRM_MOVELEFT :
		case SRM_MOVEBOTTOMLEFT :
			// Upper right corner is used as anchor
			rc2xDrizzle.OffsetRect(0, m_rcSelect.top);
			rc3xDrizzle.OffsetRect(0, m_rcSelect.top);
			rc2xDrizzle.OffsetRect(m_rcSelect.right-rc2xDrizzle.Width(), 0);
			rc3xDrizzle.OffsetRect(m_rcSelect.right-rc3xDrizzle.Width(), 0);
			break;
		case SRM_MOVETOPLEFT :
			// Lower right corner is used as anchor
			rc2xDrizzle.OffsetRect(0, m_rcSelect.bottom-rc2xDrizzle.Height());
			rc3xDrizzle.OffsetRect(0, m_rcSelect.bottom-rc3xDrizzle.Height());
			rc2xDrizzle.OffsetRect(m_rcSelect.right-rc2xDrizzle.Width(), 0);
			rc3xDrizzle.OffsetRect(m_rcSelect.right-rc3xDrizzle.Width(), 0);
			break;
		};
	}
	else
	{
		rc2xDrizzle.SetRectEmpty();
		rc3xDrizzle.SetRectEmpty();
	};
};

/* ------------------------------------------------------------------- */

bool	CSelectRectSink::Image_OnMouseMove(long lX, long lY)
{
	bool			bResult = false;

	if (m_bInSelecting)
	{
		HCURSOR			hCursor = GetCursorFromMode(m_Mode);
		if (hCursor)
			SetCursor(hCursor);

		m_fXEnd = lX;	m_fYEnd = lY;
		m_pImage->ScreenToBitmap(m_fXEnd, m_fYEnd);
		UpdateSelectRect();
		bResult = true;
	}
	else
	{
		// Change the cursor if necessary
		SELECTRECTMODE		Mode;
		HCURSOR				hCursor;

		Mode = GetModeFromPosition(lX, lY);
		hCursor = GetCursorFromMode(Mode);
		if (hCursor)
			SetCursor(hCursor);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CSelectRectSink::Image_OnLButtonDown(long lX, long lY)
{
	bool			bResult = false;

	// Start drawing the rectangle
	m_Mode = GetModeFromPosition(lX, lY);
	if (m_Mode == SRM_NONE)
		m_Mode = SRM_CREATE;

	m_fXStart = lX;	m_fYStart = lY;
	m_pImage->ScreenToBitmap(m_fXStart, m_fYStart);

	m_fXEnd = m_fXStart ;	m_fYEnd = m_fYStart;
	m_rcStart = m_rcSelect;
	UpdateSelectRect();
	m_bInSelecting = true;
	bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CSelectRectSink::Image_OnLButtonUp(long lX, long lY)
{
	bool			bResult = false;

	if (m_bInSelecting)
	{
		m_fXEnd = lX;	m_fYEnd = lY;
		m_pImage->ScreenToBitmap(m_fXEnd, m_fYEnd);
		UpdateSelectRect();

		if (m_rcSelect.Width()<=2 || m_rcSelect.Height()<=2)
			m_rcSelect.SetRectEmpty();
		m_bInSelecting = false;
		m_Mode = SRM_NONE;
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

Image* CSelectRectSink::GetOverlayImage(CRect& rcClient)
{
	Image* pResult = nullptr;
	HDC hDC = GetDC(nullptr);

	//pResult = new Bitmap(rcClient.Width(), rcClient.Height(), PixelFormat32bppARGB);
	pResult = new Metafile(hDC, RectF(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()), MetafileFrameUnitPixel, EmfTypeEmfPlusOnly, nullptr);

	if (pResult != nullptr)
	{
		Graphics		graphics(pResult);
		CRect			rcScreen;

		// Transform the select rectangle coordinates to
		//
		if (!m_rcSelect.IsRectEmpty())
		{
			CRect		rc2xDrizzle;
			CRect		rc3xDrizzle;

			GetDrizzleRectangles(rc2xDrizzle, rc3xDrizzle);
			m_pImage->BitmapToScreen(rc2xDrizzle);
			m_pImage->BitmapToScreen(rc3xDrizzle);

			rcScreen = m_rcSelect;
			m_pImage->BitmapToScreen(rcScreen);

			graphics.SetPageUnit(UnitPixel);
			graphics.SetPageScale(1.0);

			if (m_bInSelecting && !rc2xDrizzle.IsRectEmpty() && !rc3xDrizzle.IsRectEmpty())
			{
				Font			font(FontFamily::GenericSansSerif(), 10, FontStyleBold);
				SolidBrush		brush(Color(125, 255, 0, 0));
				Pen				pen(Color(125, 255, 0, 0), 1.0);
				PointF			pt;
				StringFormat	format;

				graphics.DrawRectangle(&pen, rc2xDrizzle.left, rc2xDrizzle.top, rc2xDrizzle.Width(), rc2xDrizzle.Height());
				graphics.DrawRectangle(&pen, rc3xDrizzle.left, rc3xDrizzle.top, rc3xDrizzle.Width(), rc3xDrizzle.Height());

				format.SetAlignment(StringAlignmentFar);
				format.SetLineAlignment(StringAlignmentFar);

				if (rc2xDrizzle == rcScreen)
					brush.SetColor(Color(255, 0, 0));
				else
					brush.SetColor(Color(125, 255, 0, 0));
				pt.X = rc2xDrizzle.right; pt.Y = rc2xDrizzle.bottom;
				graphics.DrawString(L"2x", -1, &font, pt, &format, &brush);

				if (rc3xDrizzle == rcScreen)
					brush.SetColor(Color(255, 0, 0));
				else
					brush.SetColor(Color(125, 255, 0, 0));
				pt.X = rc3xDrizzle.right; pt.Y = rc3xDrizzle.bottom;
				graphics.DrawString(L"3x", -1, &font, pt, &format, &brush);
			}
			{
				Pen				pen(Color(255, 0, 0), 1.0);

				graphics.DrawRectangle(&pen, rcScreen.left, rcScreen.top, rcScreen.Width(), rcScreen.Height());
			};

			if (m_bInSelecting)
			{
				// Draw extra information
				Font			font(FontFamily::GenericSansSerif(), 10, FontStyleBold);
				SolidBrush		brush(Color(255, 0, 0));
				CString			strText;
				PointF			pt(rcScreen.left, rcScreen.top);
				StringFormat	format;

				strText.Format(_T("%ldx%ld"), m_rcSelect.Width(), m_rcSelect.Height());

				format.SetAlignment(StringAlignmentNear);
				format.SetLineAlignment(StringAlignmentFar);

				graphics.DrawString(CComBSTR(strText), -1, &font, pt, &format, &brush);

				if (rcScreen.Width() > 10 && rcScreen.Height() > 10)
				{
					// Draw a small cross in the center
					Pen			pen(Color(255, 0, 0), 1.0);
					PointF		ptCenter(((double)rcScreen.left+(double)rcScreen.right)/2.0, ((double)rcScreen.top+(double)rcScreen.bottom)/2.0);

					graphics.DrawLine(&pen, (REAL)(ptCenter.X-3.0), (REAL)(ptCenter.Y-3.0), (REAL)(ptCenter.X+3.0), (REAL)(ptCenter.Y+3.0));
					graphics.DrawLine(&pen, (REAL)(ptCenter.X-3.0), (REAL)(ptCenter.Y+3.0), (REAL)(ptCenter.X+3.0), (REAL)(ptCenter.Y-3.0));
				};
			};
		};
	};

	ReleaseDC(nullptr, hDC);

	return pResult;
}


void CEditStarsSink::SetLightFrame(LPCTSTR szFileName)
{
	CLightFrameInfo bmpInfo;

	m_QualityGrid.Clear();
	bmpInfo.SetBitmap(szFileName, false);
	if (bmpInfo.m_bInfoOk)
	{
		// Get the stars back
		m_vStars = bmpInfo.GetStars();
		std::sort(m_vStars.begin(), m_vStars.end());
		m_bComet = bmpInfo.m_bComet;
		m_fXComet = bmpInfo.m_fXComet;
		m_fYComet = bmpInfo.m_fYComet;
		m_fLightBkgd = bmpInfo.m_SkyBackground.m_fLight;
	}
	else
		m_vStars.clear();

	m_strFileName = szFileName;
	m_bDirty = false;
	ComputeOverallQuality();
}


void CEditStarsSink::ComputeBackgroundValue()
{
	double fResult = 0.0;

	if (static_cast<bool>(m_pBitmap))
	{
		CBackgroundCalibration	BackgroundCalibration;

		BackgroundCalibration.m_BackgroundCalibrationMode = BCM_PERCHANNEL;
		BackgroundCalibration.m_BackgroundInterpolation   = BCI_LINEAR;
		BackgroundCalibration.SetMultiplier(1.0);
		BackgroundCalibration.ComputeBackgroundCalibration(m_pBitmap.get(), true, nullptr);
		fResult = BackgroundCalibration.m_fTgtRedBk/256.0/256.0;

		m_fBackground = fResult;
	};
};

/* ------------------------------------------------------------------- */

void	CEditStarsSink::SaveRegisterSettings()
{
	if (m_strFileName.GetLength())
	{
		CRegisteredFrame	regFrame;
		STARVECTOR			vStars;

		regFrame.m_bComet = m_bComet;
		regFrame.m_fXComet = m_fXComet;
		regFrame.m_fYComet = m_fYComet;
		regFrame.m_SkyBackground.m_fLight = m_fLightBkgd;
		for (size_t i = 0; i < m_vStars.size(); i++)
		{
			if (!m_vStars[i].m_bRemoved)
				vStars.push_back(m_vStars[i]);
		};
		regFrame.SetStars(vStars);
		m_vStars = vStars;
		std::sort(m_vStars.begin(), m_vStars.end());

		TCHAR				szDrive[1+_MAX_DRIVE];
		TCHAR				szDir[1+_MAX_DIR];
		TCHAR				szFile[1+_MAX_FNAME];
		CString				strInfoFileName;

		_tsplitpath(m_strFileName, szDrive, szDir, szFile, nullptr);
		strInfoFileName = szDrive;
		strInfoFileName += szDir;
		strInfoFileName += szFile;
		strInfoFileName += _T(".info.txt");

		regFrame.SaveRegisteringInfo(strInfoFileName.GetString());

		m_bDirty = false;
	};
};

/* ------------------------------------------------------------------- */

void CEditStarsSink::InitGrayBitmap(CRect& rc)
{
	m_GrayBitmap.SetMultiplier(1.0);
	for (int i = rc.left; i <= rc.right; i++)
	{
		for (int j = rc.top; j <= rc.bottom; j++)
		{
			double fGray;
			m_pBitmap->GetPixel(i, j, fGray);
			m_GrayBitmap.SetPixel(i - rc.left, j - rc.top, fGray / 256.0);
		}
	}
}

void CEditStarsSink::DetectStars(const QPointF & pt, CRect & rcCheck, STARVECTOR & vStars)
{
	// Create a 3*STARMAXSIZE + 1 square rectangle centered on the point
	vStars.clear();
	if (m_pImage)
	{
		rcCheck.left	= pt.x() - RCCHECKSIZE/2;
		rcCheck.right	= pt.x() + RCCHECKSIZE/2;
		rcCheck.top		= pt.y() - RCCHECKSIZE/2;
		rcCheck.bottom	= pt.y() + RCCHECKSIZE/2;

		if (rcCheck.left < 0)
		{
			// Move the rectangle to the right
			rcCheck.left  = 0;
			rcCheck.right = RCCHECKSIZE;
		}
		else if (rcCheck.right >= m_pImage->GetImgSizeX())
		{
			rcCheck.right = m_pImage->GetImgSizeX()-1;
			rcCheck.left  = rcCheck.right-RCCHECKSIZE;
		};

		if (rcCheck.top < 0)
		{
			rcCheck.top		= 0;
			rcCheck.bottom	= RCCHECKSIZE;
		}
		else if (rcCheck.bottom >= m_pImage->GetImgSizeY())
		{
			rcCheck.bottom = m_pImage->GetImgSizeY()-1;
			rcCheck.top	   = rcCheck.bottom-RCCHECKSIZE;
		};

		InitGrayBitmap(rcCheck);

		CRegisteredFrame		regFrame;
		const DSSRect rcReg{ STARMAXSIZE, STARMAXSIZE, rcCheck.Width() - (STARMAXSIZE + 1), rcCheck.Height() - (STARMAXSIZE + 1) };
		STARSET stars;

		regFrame.m_fBackground = m_fBackground;
		regFrame.RegisterSubRect(&m_GrayBitmap, rcReg, stars);

		vStars.assign(stars.cbegin(), stars.cend());
	}
}


bool CEditStarsSink::Image_OnMouseMove(long lX, long lY)
{
	bool			bResult = false;

	if (static_cast<bool>(m_pBitmap))
	{
		CPoint		pt(lX, lY);

		m_pImage->ScreenToBitmap(pt);
		if (pt.x>=0 && pt.x<m_pImage->GetImgSizeX() &&
			pt.y>=0 && pt.y<m_pImage->GetImgSizeY())
		{
			// The point is in the image
			bResult = true; // Refresh the overlay
			m_ptCursor = QPointF(pt.x, pt.y);
		}
		else if (m_ptCursor.x() >=0)
		{
			m_ptCursor.rx() = m_ptCursor.ry() = -1;
			bResult = true;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CEditStarsSink::Image_OnMouseLeave()
{
	bool			bResult = false;

	if (m_ptCursor.x() >= 0)
	{
		m_ptCursor.rx() = m_ptCursor.ry() = -1;
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CEditStarsSink::Image_OnLButtonDown(long, long)
{
	bool			bResult = false;

	{
		if (m_Action != ESA_NONE)
		{
			if (m_Action == ESA_ADDSTAR)
			{
				m_vStars.push_back(m_AddedStar);
				std::sort(m_vStars.begin(), m_vStars.end());
				if (m_bRemoveComet)
					m_bComet = false;
				ComputeOverallQuality();
				m_QualityGrid.InitGrid(m_vStars);
			}
			else if (m_Action == ESA_REMOVESTAR)
			{
				m_vStars[m_lRemovedIndice].m_bRemoved = true;
				m_vStars.erase(m_vStars.begin()+m_lRemovedIndice);
				ComputeOverallQuality();
				m_QualityGrid.InitGrid(m_vStars);
			}
			else if (m_Action == ESA_SETCOMET)
			{
				if (m_lRemovedIndice >= 0)
				{
					m_vStars[m_lRemovedIndice].m_bRemoved = true;
					m_vStars.erase(m_vStars.begin()+m_lRemovedIndice);
					ComputeOverallQuality();
					m_QualityGrid.InitGrid(m_vStars);
				};
				m_fXComet = m_AddedStar.m_fX;
				m_fYComet = m_AddedStar.m_fY;
				m_bComet  = true;
			}
			else if (m_Action == ESA_RESETCOMET)
			{
				m_bComet = false;
			};

			bResult = true;
			m_bDirty = true;

			if (m_pImage)
				m_pImage->NotifyModeChange();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CEditStarsSink::Image_OnLButtonUp(long, long)
{
	return true;
}

/* ------------------------------------------------------------------- */

void	CEditStarsSink::DrawQualityGrid(Graphics * pGraphics, CRect & rcClient)
{
	// Find the first top/left point in the image
	bool					bDraw = true;
	decltype(tagPOINT::x)	x1{ 0 }, x2{ 0 };	// Initialise to zero as you get a warning in the CRect by the CDelaunayTriangle instance about
	decltype(tagPOINT::y)	y1{ 0 }, y2{ 0 };	// some of these values potentially not being initialised. I'm not convinced that is the case though!

	CPoint		pt(rcClient.left, rcClient.top);

	m_pImage->ScreenToBitmap(pt);
	if (pt.x < 0)
		x1 = 0;
	else if (pt.x>=m_pImage->GetImgSizeX())
		bDraw = false;
	else
		x1 = pt.x;

	if (pt.y < 0)
		y1 = 0;
	else if (pt.y >= m_pImage->GetImgSizeY())
		bDraw = false;
	else
		y1 = pt.y;

	pt.x = rcClient.right;	pt.y = rcClient.bottom;
	m_pImage->ScreenToBitmap(pt);

	if (pt.x >= m_pImage->GetImgSizeX())
		x2 = m_pImage->GetImgSizeX()-1;
	else if (pt.x < 0)
		bDraw = false;
	else
		x2 = pt.x;

	if (pt.y >= m_pImage->GetImgSizeY())
		y2 = m_pImage->GetImgSizeY()-1;
	else if (pt.y < 0)
		bDraw = false;
	else
		y2 = pt.y;

	if (bDraw)
	{
		pGraphics->SetSmoothingMode(SmoothingModeHighQuality);
		for (size_t i = 0; i < m_QualityGrid.m_vTriangles.size(); i++)
		{
			CDelaunayTriangle &		tr = m_QualityGrid.m_vTriangles[i];
			CRect					rc1(x1, y1, x2, y2),
									rc2,
									rc3;

			rc2.left    = std::min(tr.pt1.X, std::min(tr.pt2.X, tr.pt3.X));
			rc2.top     = std::min(tr.pt1.Y, std::min(tr.pt2.Y, tr.pt3.Y));
			rc2.right   = std::max(tr.pt1.X, std::max(tr.pt2.X, tr.pt3.X));
			rc2.bottom  = std::max(tr.pt1.Y, std::max(tr.pt2.Y, tr.pt3.Y));

			if (rc3.IntersectRect(&rc1, &rc2))
			{
				// Draw the triangle
#pragma warning (suppress:4456)
				PointF		pt[3];

				pt[0] = tr.pt1,
				pt[1] = tr.pt2,
				pt[2] = tr.pt3;

				m_pImage->BitmapToScreen(pt[0]);
				m_pImage->BitmapToScreen(pt[1]);
				m_pImage->BitmapToScreen(pt[2]);

				GraphicsPath				path;

				path.AddPolygon(pt, 3);

				Color			colors[3];
				int				nColors = 3;

				colors[0] = tr.cr1;
				colors[1] = tr.cr2;
				colors[2] = tr.cr3;

				PathGradientBrush	brush(&path);
				brush.SetCenterColor(Color(
					((double)tr.cr1.GetAlpha()+(double)tr.cr2.GetAlpha()+(double)tr.cr3.GetAlpha())/3.0,
					((double)tr.cr1.GetRed()+(double)tr.cr2.GetRed()+(double)tr.cr3.GetRed())/3.0,
					((double)tr.cr1.GetGreen()+(double)tr.cr2.GetGreen()+(double)tr.cr3.GetGreen())/3.0,
					((double)tr.cr1.GetBlue()+(double)tr.cr2.GetBlue()+(double)tr.cr3.GetBlue())/3.0));
				brush.SetSurroundColors(colors, &nColors);

				pGraphics->FillPolygon(&brush, pt, 3);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

Image *	CEditStarsSink::GetOverlayImage(CRect & rcClient)
{
	Image *				pResult = nullptr;

	//if (m_vStars.size())
	{
		HDC				hDC = GetDC(nullptr);

		pResult = new Metafile(hDC, RectF(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()), MetafileFrameUnitPixel, EmfTypeEmfPlusOnly, nullptr);

		if (pResult)
		{
			Graphics		graphics(pResult);

			graphics.SetSmoothingMode(SmoothingModeHighQuality);

			if (m_QualityGrid.Empty() && m_vStars.size())
				m_QualityGrid.InitGrid(m_vStars);

			//if (!m_QualityGrid.Empty())
			//	DrawQualityGrid(&graphics, rcClient);

			if (g_bShowRefStars && !m_bCometMode)
			{
				for (int i = 0; i < m_vRefStars.size(); i++)
				{
					if (IsRefStarVoted(i))
					{
						CPoint		pt;
						CRect		rc;

						double		fX, fY;

						fX   = m_vRefStars[i].m_fX - m_vRefStars[i].m_fMeanRadius + 0.5;
						fY   = m_vRefStars[i].m_fY - m_vRefStars[i].m_fMeanRadius + 0.5;
						m_pImage->BitmapToScreen(fX, fY);
						rc.left = fX;
						rc.top  = fY;

						fX   = m_vRefStars[i].m_fX + m_vRefStars[i].m_fMeanRadius + 0.5;
						fY   = m_vRefStars[i].m_fY + m_vRefStars[i].m_fMeanRadius + 0.5;
						m_pImage->BitmapToScreen(fX, fY);
						rc.right  = fX;
						rc.bottom = fY;

						Pen			pen(Color(255, 0, 0), 1.0);

						graphics.DrawEllipse(&pen, rc.left, rc.top, rc.Width(), rc.Height());

						if (rc.Width() > 10 && rc.Height() > 10)
						{
							fX = m_vRefStars[i].m_fX+0.5;
							fY = m_vRefStars[i].m_fY+0.5;
							m_pImage->BitmapToScreen(fX, fY);
							pt.x = fX; pt.y = fY;
							graphics.DrawLine(&pen, 
								static_cast<int>(pt.x) - 5, static_cast<int>(pt.y),
								static_cast<int>(pt.x) + 6, static_cast<int>(pt.y));
							graphics.DrawLine(&pen,
								static_cast<int>(pt.x), static_cast<int>(pt.y) - 5,
								static_cast<int>(pt.x), static_cast<int>(pt.y) + 6);
						};
					};
				};
			};

			for (int i = 0; i < m_vStars.size(); i++)
			{
				if (IsTgtStarVoted(i) && !m_vStars[i].m_bRemoved)
				{
					CPoint		pt;
					CRect		rc;

					double		fX, fY;

					fX   = m_vStars[i].m_fX - m_vStars[i].m_fMeanRadius + 0.5;
					fY   = m_vStars[i].m_fY - m_vStars[i].m_fMeanRadius + 0.5;
					m_pImage->BitmapToScreen(fX, fY);
					rc.left = fX;
					rc.top  = fY;

					fX   = m_vStars[i].m_fX + m_vStars[i].m_fMeanRadius + 0.5;
					fY   = m_vStars[i].m_fY + m_vStars[i].m_fMeanRadius + 0.5;
					m_pImage->BitmapToScreen(fX, fY);
					rc.right  = fX;
					rc.bottom = fY;

					Pen			pen(Color(255, 0, 0), 1.0);

					if (m_vStars[i].m_bAdded)
						pen.SetColor(Color(m_bCometMode ? 255*0.5 : 255, 0, 0, 255));
					else
						pen.SetColor(Color(m_bCometMode ? 255*0.5 : 255, 0, 190, 0));

					graphics.DrawEllipse(&pen, rc.left, rc.top, rc.Width(), rc.Height());

					if (rc.Width() > 10 && rc.Height() > 10)
					{
						fX = m_vStars[i].m_fX+0.5;
						fY = m_vStars[i].m_fY+0.5;
						m_pImage->BitmapToScreen(fX, fY);
						pt.x = fX; pt.y = fY;
						graphics.DrawLine(&pen,
							static_cast<int>(pt.x) - 5, static_cast<int>(pt.y),
							static_cast<int>(pt.x) + 6, static_cast<int>(pt.y));
						graphics.DrawLine(&pen,
							static_cast<int>(pt.x), static_cast<int>(pt.y) - 5,
							static_cast<int>(pt.x), static_cast<int>(pt.y) + 6);
					};

					if (g_bShowRefStars && m_vRefStars.size())
					{
						QPointF			ptOrg;
						QPointF			ptDst;

						ptOrg.rx() = m_vStars[i].m_fX;
						ptOrg.ry() = m_vStars[i].m_fY;

						ptDst = ptOrg;

						ptDst = m_Transformation.transform(ptOrg);

						CPoint				ptOrgBmp;
						CPoint				ptDstBmp;

						ptOrg.rx() += 0.5; ptOrg.ry() += 0.5;
						ptDst.rx() += 0.5; ptDst.ry() += 0.5;

						m_pImage->BitmapToScreen(ptOrg.rx(), ptOrg.ry());
						ptOrgBmp.x = ptOrg.x(); ptOrgBmp.y = ptOrg.y();
						m_pImage->BitmapToScreen(ptDst.rx(), ptDst.ry());
						ptDstBmp.x = ptDst.x(); ptDstBmp.y = ptDst.y();

						graphics.DrawLine(&pen,
							static_cast<int>(ptOrgBmp.x), static_cast<int>(ptOrgBmp.y),
							static_cast<int>(ptDstBmp.x), static_cast<int>(ptDstBmp.y));
					};

					if (g_bShowRefStars && m_vStars[i].m_fLargeMajorAxis > 0)
					{
						QPointF			ptOrg(m_vStars[i].m_fX+ 0.5, m_vStars[i].m_fY+ 0.5);
						QPointF			ptDst;

						ptDst = ptOrg;
						ptDst.rx() += m_vStars[i].m_fLargeMajorAxis * cos(m_vStars[i].m_fMajorAxisAngle/180.0*M_PI);
						ptDst.ry() += m_vStars[i].m_fLargeMajorAxis * sin(m_vStars[i].m_fMajorAxisAngle/180.0*M_PI);

						CPoint				ptOrgBmp;
						CPoint				ptDstBmp;

						m_pImage->BitmapToScreen(ptOrg.rx(), ptOrg.ry());
						m_pImage->BitmapToScreen(ptDst.rx(), ptDst.ry());
						ptOrgBmp.x = ptOrg.x(); ptOrgBmp.y = ptOrg.y();
						ptDstBmp.x = ptDst.x(); ptDstBmp.y = ptDst.y();

						graphics.DrawLine(&pen,
							static_cast<int>(ptOrgBmp.x), static_cast<int>(ptOrgBmp.y),
							static_cast<int>(ptDstBmp.x), static_cast<int>(ptDstBmp.y));
					};
				};
			};

			if (m_bComet)
			{
				double					fX = m_fXComet,
										fY = m_fYComet;
				double					fCometSize = 20;

				m_pImage->BitmapToScreen(fX, fY);

				if (m_pImage->GetZoomX() > 1)
					fCometSize *= m_pImage->GetZoomX();

				Pen						pen(Color(m_bComet ? 210 : 150, 255, 0, 255), 2.0);

				graphics.DrawEllipse(&pen, (REAL)fX-fCometSize/2, (REAL)fY-fCometSize/2, (REAL)fCometSize, (REAL)fCometSize);
			};

			m_Action = ESA_NONE;
			if (m_ptCursor.x() >= 0 && m_ptCursor.y() >= 0)
			{
				CRect					rcCheck;
				STARVECTOR				vStars;
				double					fNearestNewStarDistance;
				bool					bInNewStar;

				double					fNearestOldStarDistance;
				bool					bInOldStar;

				bool					bAdd = true;
				bool					bShowAction = false;
				CStar					star;
				bool					bForceHere = false;

				bForceHere = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

				DetectStars(m_ptCursor, rcCheck, vStars);

				auto lNearestNewStar = FindNearestStar(m_ptCursor.x() - rcCheck.left, m_ptCursor.y() - rcCheck.top, vStars, bInNewStar, fNearestNewStarDistance);

				fNearestOldStarDistance = 50;
				auto lNearestOldStar = FindNearestStarWithinDistance(m_ptCursor.x(), m_ptCursor.y(), m_vStars, bInOldStar, fNearestOldStarDistance);

				m_lRemovedIndice = -1;
				m_bRemoveComet	 = false;
				if (m_bCometMode)
				{
					if (lNearestNewStar>=0 || lNearestOldStar>=0)
					{
						bool		bRemoveComet = false;
						if (m_bComet)
						{
							// Check for comet removal
							if ((lNearestNewStar>=0) &&
								vStars[lNearestNewStar].IsInRadius(m_fXComet-rcCheck.left, m_fYComet-rcCheck.top))
								bRemoveComet = true;
							else if ((lNearestOldStar>0) &&
  								     m_vStars[lNearestOldStar].IsInRadius(m_fXComet, m_fYComet))
								bRemoveComet = true;
							if (bRemoveComet)
							{
								star.m_fX = m_fXComet;
								star.m_fY = m_fYComet;
								bShowAction = true;
								bAdd = false;
							};
						};
						if (!bRemoveComet)
						{
							// Check to add the comet
							if ((lNearestOldStar>=0) && (lNearestNewStar < 0))
							{
								bAdd = true;
								m_lRemovedIndice = lNearestOldStar;
								star = m_vStars[lNearestOldStar];
								bShowAction = true;
							}
							else if ((lNearestOldStar < 0) && (lNearestNewStar >= 0))
							{
								bAdd = true;
								star = vStars[lNearestNewStar];
								star.m_fX += rcCheck.left;
								star.m_fY += rcCheck.top;
								bShowAction = true;
							}
							else // Both new and old star nearest
							{
								if (fNearestNewStarDistance < fNearestOldStarDistance)
								{
									star = vStars[lNearestNewStar];
									star.m_fX += rcCheck.left;
									star.m_fY += rcCheck.top;
								}
								else
								{
									m_lRemovedIndice = lNearestOldStar;
									star = m_vStars[lNearestOldStar];
								};
								bAdd = true;
								bShowAction = true;
							};
							if (bForceHere)
							{
								bAdd = true;
								star.m_fX = m_ptCursor.x();
								star.m_fY = m_ptCursor.y();
								bShowAction = true;
							}
						}
					}
					else if (bForceHere)
					{
						bAdd = true;
						star.m_fX = m_ptCursor.x();
						star.m_fY = m_ptCursor.y();
						bShowAction = true;
					};
					if (bAdd && m_lRemovedIndice<0)
					{
						// Check that the comet is not in a existing star
						fNearestOldStarDistance = 50;
						lNearestOldStar = FindNearestStarWithinDistance(star.m_fX, star.m_fY, m_vStars, bInOldStar, fNearestOldStarDistance);
						if (bInOldStar)
							m_lRemovedIndice = lNearestOldStar;
					};
				}
				else
				{
					if (lNearestNewStar>=0 || lNearestOldStar>=0)
					{
						if (bInOldStar || lNearestNewStar < 0)
						{
							bAdd = false;
							bShowAction = true;
						}
						else if ((lNearestOldStar < 0) && (lNearestNewStar >= 0))
						{
							bAdd = true;
							bShowAction = true;
						}
						else // Both new and old star nearest
						{
							// If the new star is in an old star - remove
							QPoint		pt(vStars[lNearestNewStar].m_fX + rcCheck.left, vStars[lNearestNewStar].m_fY + rcCheck.top);
							if (m_vStars[lNearestOldStar].IsInRadius(pt))
								bAdd = false;
							else if (fNearestNewStarDistance >= fNearestOldStarDistance*1.10)
								bAdd = false;
							else
								bAdd = true;
							bShowAction = true;
						};
					};
					if (bShowAction)
					{
						if (bAdd)
						{
							star = vStars[lNearestNewStar];
							star.m_fX += rcCheck.left;
							star.m_fY += rcCheck.top;

							if (m_bComet && star.IsInRadius(m_fXComet, m_fYComet))
								m_bRemoveComet = true;
						}
						else
							star = m_vStars[lNearestOldStar];
					};
				};


				// Draw all the potentially registrable stars
				for (int i = 0; i < vStars.size(); i++)
				{
					CPoint		pt;
					CRect		rc;
#pragma warning (suppress:4456)
					CStar &		star = vStars[i];

					double		fX, fY;

					fX   = star.m_fX - star.m_fMeanRadius + 0.5 + rcCheck.left;
					fY   = star.m_fY - star.m_fMeanRadius + 0.5 + rcCheck.top;
					m_pImage->BitmapToScreen(fX, fY);
					rc.left = fX;
					rc.top  = fY;

					fX   = star.m_fX + star.m_fMeanRadius + 0.5 + rcCheck.left;
					fY   = star.m_fY + star.m_fMeanRadius + 0.5 + rcCheck.top;
					m_pImage->BitmapToScreen(fX, fY);
					rc.right  = fX;
					rc.bottom = fY;

					Pen			pen(Color(255*0.7, 255, 255, 255), 1.0);

					if (bAdd && bShowAction && lNearestNewStar == i)
						pen.SetColor(Color(255*0.9, 255, 255, 255));

					graphics.DrawEllipse(&pen, rc.left, rc.top, rc.Width(), rc.Height());

					if (rc.Width() > 10 && rc.Height() > 10)
					{
						fX = star.m_fX+0.5 + rcCheck.left;
						fY = star.m_fY+0.5 + rcCheck.top;
						m_pImage->BitmapToScreen(fX, fY);
						pt.x = fX; pt.y = fY;
						graphics.DrawLine(&pen,
							static_cast<int>(pt.x) - 5, static_cast<int>(pt.y),
							static_cast<int>(pt.x) + 6, static_cast<int>(pt.y));
						graphics.DrawLine(&pen,
							static_cast<int>(pt.x), static_cast<int>(pt.y) - 5,
							static_cast<int>(pt.x), static_cast<int>(pt.y) + 6);
					};
				};

				if (bShowAction)
				{
					double		fRectSize = 20;
					double		fDiameter = 10;
					Pen			penaction(Color(255*0.7, 255, 255, 255), 3.0);
					SolidBrush	brushaction(Color(255*0.5, 255, 255, 255));
					double		fX, fY;
					CString		strTip;

					if (m_pImage->GetZoomX() > 1)
					{
						fRectSize *= m_pImage->GetZoomX();
						fDiameter *= m_pImage->GetZoomX();
					};

					if (bAdd)
					{
						if (m_bCometMode)
							strTip.LoadString(IDS_TIP_SETCOMET);
						else
							strTip.LoadString(IDS_TIP_ADDSTAR);
						penaction.SetColor(Color(255*0.7, 0, 255, 0));
						brushaction.SetColor(Color(255*0.5, 0, 255, 0));
					}
					else
					{
						if (m_bCometMode)
							strTip.LoadString(IDS_TIP_REMOVECOMET);
						else
							strTip.LoadString(IDS_TIP_REMOVESTAR);
						penaction.SetColor(Color(255*0.7, 255, 255, 0));
						brushaction.SetColor(Color(255*0.5, 255, 255, 0));
					};

					fX = star.m_fX;		fY = star.m_fY;
					m_pImage->BitmapToScreen(fX, fY);

					graphics.DrawArc(&penaction, (REAL)(fX-fRectSize), (REAL)(fY-fRectSize), fDiameter, fDiameter, -90.0, -90.0);
					graphics.DrawArc(&penaction, (REAL)(fX+fRectSize-fDiameter), (REAL)(fY-fRectSize), fDiameter, fDiameter, 0.0, -90.0);
					graphics.DrawArc(&penaction, (REAL)(fX+fRectSize-fDiameter), (REAL)(fY+fRectSize-fDiameter), fDiameter, fDiameter, 0.0, 90.0);
					graphics.DrawArc(&penaction, (REAL)(fX-fRectSize), (REAL)(fY+fRectSize-fDiameter), fDiameter, fDiameter, -180.0, -90.0);

					Font				font(FontFamily::GenericSansSerif(), 10, FontStyleRegular);
					StringFormat		format;
					PointF				pf(fX, fY-fRectSize);

					format.SetAlignment(StringAlignmentCenter);
					format.SetLineAlignment(StringAlignmentFar);

					graphics.DrawString(CComBSTR(strTip), -1, &font, pf, &format, &brushaction);

					m_AddedStar = star;
					if (m_bCometMode)
						m_Action = bAdd ? ESA_SETCOMET : ESA_RESETCOMET;
					else
					{
						m_lRemovedIndice = lNearestOldStar;
						m_Action = bAdd ? ESA_ADDSTAR : ESA_REMOVESTAR;
					};
				};

				{
					CRect			rcText;
					QPointF		ptScreen{ m_ptCursor };

					m_pImage->BitmapToScreen(ptScreen.rx(), ptScreen.ry());

					if ((ptScreen.x() >= rcClient.right-150) &&
						(ptScreen.y() <= 150))
					{
						// Draw the rectangle at the left bottom
						rcText.left = 2;
						rcText.top  = rcClient.bottom-72;
					}
					else
					{
						rcText.left = rcClient.right-122;
						rcText.top  = 2;
					};

					rcText.right = rcText.left + 120;
					rcText.bottom = rcText.top + 70;

					SolidBrush				brush(Color(200, 255, 255, 255));

					graphics.FillRectangle(&brush, rcText.left, rcText.top, rcText.Width(), rcText.Height());

					Pen						pen(Color(200, 0, 0, 0), 1.0);
					graphics.DrawRectangle(&pen, rcText.left+2, rcText.top+2, rcText.Width()-4, rcText.Height()-4);

					brush.SetColor(Color(0, 0, 0));

					Font				font(FontFamily::GenericSansSerif(), 9, FontStyleRegular);
					StringFormat		format;
					PointF				pf(rcText.left+6, rcText.top+6);
					CString				strText;

					format.SetAlignment(StringAlignmentNear);
					format.SetLineAlignment(StringAlignmentNear);

					strText.Format(IDS_LIGHTFRAMEINFO, m_lNrStars, m_fScore, m_fFWHM);
					if (m_bComet)
					{
						CString			strComet;
						CString			strYes;

						strYes.LoadString(IDS_YES);
						strComet.Format(IDS_LIGHTFRAMEINFOCOMET, (LPCTSTR)strYes);
						strText += strComet;
					};

					graphics.DrawString(CComBSTR(strText), -1, &font, pf, &format, &brush);
				};
			};
		};
		ReleaseDC(nullptr, hDC);
	};

	return pResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

#include "Delaunay.h"

inline Color SolveColor(double fValue, double fMean, double fStdDev)
{
	double			red = 0, green = 0, blue = 0;

	fValue -= fMean;
	fValue /= fStdDev;

	if (fabs(fValue)<0.5)
		green = 255;
	else if (fValue<0)
	{
		red   = std::max(0.0, 255.0-std::max(0.0, 3.0-fabs(fValue))*255.0/3.0);
		green = std::max(0.0, std::max(0.0, 3.0-fabs(fValue))*255.0/3.0);
	}
	else
	{
		blue  = std::max(0.0, 255.0-std::max(0.0, 3.0-fabs(fValue))*255.0/3.0);
		green = std::max(0.0, std::max(0.0, 3.0-fabs(fValue))*255.0/3.0);
	};

	return Color(0.3*255, red, green, blue);
};

inline double StarValue(const CStar& star)
{
	return (star.m_fLargeMinorAxis + star.m_fSmallMinorAxis) / (star.m_fLargeMajorAxis + star.m_fSmallMajorAxis);
};

void CQualityGrid::InitGrid(STARVECTOR& vStars)
{
	double fPowSum = 0.0;
	vertexSet sVertices;

	m_fMean = 0;
	m_vTriangles.clear();

	if (!vStars.empty())
	{
		for (auto& star : vStars)
		{
			if (!star.m_bRemoved)
			{
				vertex v(static_cast<decltype(Gdiplus::PointF::X)>(star.m_fX), static_cast<decltype(Gdiplus::PointF::Y)>(star.m_fY));

				star.m_fX = v.GetX();
				star.m_fY = v.GetY();

				sVertices.insert(v);

				const double fValue = StarValue(star);

				m_fMean += fValue;
				fPowSum += fValue * fValue;
			}
		}

		if (!sVertices.empty())
		{
			m_fStdDev = sqrt(fPowSum / sVertices.size() - pow(m_fMean / sVertices.size(), 2));
			m_fMean /= sVertices.size();
		};

		Delaunay delaunay;
		triangleSet sTriangles;

		//
		// ### To Do
		// March 2023: Delaunay::Triangulate() does nothing. sTriangles will be empty.
		//
		delaunay.Triangulate(sVertices, sTriangles);

		m_vTriangles.reserve(sTriangles.size());

		for (ctIterator cit = sTriangles.cbegin(); cit != sTriangles.cend(); ++cit)
		{
			CDelaunayTriangle tr;
			
			QPointF temp;
			
			temp = cit->GetVertex(0)->GetPoint();
			tr.pt1 = PointF(temp.x(), temp.y());
			temp = cit->GetVertex(1)->GetPoint();
			tr.pt2 = PointF(temp.x(), temp.y());
			temp = cit->GetVertex(2)->GetPoint();
			tr.pt3 = PointF(temp.x(), temp.y());

			// Find the value for each point
			const auto solve = [mean = this->m_fMean, stdev = this->m_fStdDev, &vStars](const Gdiplus::REAL x, const Gdiplus::REAL y) -> Gdiplus::Color
			{
				const auto it = lower_bound(vStars.cbegin(), vStars.cend(), CStar(x, y));
				return SolveColor(it != vStars.cend() ? StarValue(*it) : mean, mean, stdev);
			};

			tr.cr1 = solve(tr.pt1.X, tr.pt1.Y);
			tr.cr2 = solve(tr.pt2.X, tr.pt2.Y);
			tr.cr3 = solve(tr.pt3.X, tr.pt3.Y);

			m_vTriangles.push_back(tr);
		}
	}
}

/* ------------------------------------------------------------------- */
