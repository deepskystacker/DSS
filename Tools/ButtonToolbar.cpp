#include <stdafx.h>

#include "ButtonToolbar.h"

/* ------------------------------------------------------------------- */

Bitmap *	GetBitmapFromIcon(int nBitmap, int nMask)
{
	HBITMAP				hBitmap = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(nBitmap));
	HBITMAP				hMask   = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(nMask));
	Bitmap	*			pResult;
	Bitmap				bmp(hBitmap, nullptr);
	Bitmap				bmpMask(hMask, nullptr);

	pResult = bmp.Clone(0, 0, BUTTONTOOLBARSIZE, BUTTONTOOLBARSIZE, PixelFormat32bppARGB);
	for (UINT i = 0;i<bmp.GetWidth();i++)
	{
		for (UINT j = 0;j<bmp.GetHeight();j++)
		{
			Color		color,
						tmpcolor;
			Color		alphaColor;
			pResult->GetPixel(i, j, &color);
			bmpMask.GetPixel(i, j, &alphaColor);

			tmpcolor.SetValue(color.MakeARGB(alphaColor.GetGreen(), color.GetRed(), color.GetGreen(), color.GetBlue()));
			pResult->SetPixel(i, j, tmpcolor);
		};
	};
	return pResult;
};

/* ------------------------------------------------------------------- */

void	CMaskedButton::Init(UINT nBitmapUp, UINT nBitmapDown, UINT nBitmapHot, UINT nBitmapDisabled, UINT nBitmapMask)
{
	m_pUp		= GetBitmapFromIcon(nBitmapUp, nBitmapMask);
	m_pDown		= GetBitmapFromIcon(nBitmapDown, nBitmapMask);
	m_pHot		= GetBitmapFromIcon(nBitmapHot, nBitmapMask);
	m_pDisabled = GetBitmapFromIcon(nBitmapDisabled, nBitmapMask);
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CButtonToolbar::ComputeTooltipSize(LPCTSTR szTooltip, CRect & rcTooltip)
{
	HDC					hDC = GetDC(nullptr);
	Graphics			graphics(hDC);
	Font				font(FontFamily::GenericSansSerif(), 10, FontStyleRegular);
	StringFormat		format;
	RectF				rcFBound;

	format.SetAlignment(StringAlignmentNear);
	format.SetLineAlignment(StringAlignmentNear);

	graphics.MeasureString(CComBSTR(szTooltip), -1, &font, PointF(0, 0), &format, &rcFBound);

	ReleaseDC(nullptr, hDC);

	rcTooltip.left  = rcFBound.X;
	rcTooltip.right = rcFBound.X+rcFBound.Width;

	rcTooltip.top	= rcFBound.Y;
	rcTooltip.bottom = rcFBound.Y+rcFBound.Height;
};

/* ------------------------------------------------------------------- */

BOOL CButtonToolbar::GetTooltipRect(CRect & rcTooltip, CString & strTooltip)
{
	BOOL					bResult = FALSE;

	rcTooltip.SetRectEmpty();
	for (LONG i = 0;i<m_vButtons.size() && !bResult;i++)
	{
		if (m_vButtons[i].m_bShowTooltip)
		{
			bResult = TRUE;
			if (m_vButtons[i].m_strTooltip.GetLength())
			{
				ComputeTooltipSize(m_vButtons[i].m_strTooltip, rcTooltip);
				strTooltip = m_vButtons[i].m_strTooltip;
				// Slightly enlarge the rectangle
				rcTooltip.right += 20;
				rcTooltip.bottom += 20;
				// Offset the rectangle so that it is centered on the button vertically
				rcTooltip.OffsetRect(0, m_vButtons[i].m_rcButton.top+(m_vButtons[i].m_rcButton.Height()-rcTooltip.Height())/2.0);
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

Image*	CButtonToolbar::GetImage()
{
	CRect					rcPosition;

	rcPosition = m_rcToolbar;

	m_ptTopLeft.x = rcPosition.left;
	m_ptTopLeft.y = rcPosition.top;

	Bitmap *				pbmp = new Bitmap(BUTTONTOOLBARSIZE+2, BUTTONTOOLBARSIZE*(LONG)m_vButtons.size()+2, PixelFormat32bppARGB);
	Graphics				graphics(pbmp);

	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	// First draw the outside frame
	{
		GraphicsPath		path(FillModeWinding);
		Rect				rc(rcPosition.left-1, rcPosition.top-1+BUTTONTOOLBARSIZE/2, rcPosition.Width(), rcPosition.Height()-BUTTONTOOLBARSIZE+2);

		path.AddRectangle(rc);
		path.AddEllipse((INT)rcPosition.left-1, (INT)rcPosition.top-1, (INT)BUTTONTOOLBARSIZE+2, (INT)BUTTONTOOLBARSIZE+2);
		path.AddEllipse((INT)rcPosition.left-1, (INT)rcPosition.bottom-BUTTONTOOLBARSIZE-1, (INT)BUTTONTOOLBARSIZE+2, (INT)BUTTONTOOLBARSIZE+2);

		path.Flatten();

		SolidBrush			brush(Color(255*0.9, 255, 255, 255));

		graphics.FillPath(&brush, &path);
	};

	for (LONG i = 0;i<m_vButtons.size();i++)
	{
		m_vButtons[i].m_rcButton.left  = 1;
		m_vButtons[i].m_rcButton.top   = 1+i*BUTTONTOOLBARSIZE;
		m_vButtons[i].m_rcButton.right = m_vButtons[i].m_rcButton.left+BUTTONTOOLBARSIZE;
		m_vButtons[i].m_rcButton.bottom= m_vButtons[i].m_rcButton.top+BUTTONTOOLBARSIZE;

		switch (m_vButtons[i].m_Status)
		{
		case MBS_UP :
			graphics.DrawImage(m_vButtons[i].m_pUp, (INT)rcPosition.left+1, (INT)rcPosition.top+1+i*BUTTONTOOLBARSIZE);
			break;
		case MBS_DOWN :
			graphics.DrawImage(m_vButtons[i].m_pDown, (INT)rcPosition.left+1, (INT)rcPosition.top+1+i*BUTTONTOOLBARSIZE);
			break;
		case MBS_DISABLED :
			graphics.DrawImage(m_vButtons[i].m_pDisabled, (INT)rcPosition.left+1, (INT)rcPosition.top+1+i*BUTTONTOOLBARSIZE);
			break;
		case MBS_HOT :
			graphics.DrawImage(m_vButtons[i].m_pHot, (INT)rcPosition.left+1, (INT)rcPosition.top+1+i*BUTTONTOOLBARSIZE);
			break;
		};
	};

	return pbmp;
};

/* ------------------------------------------------------------------- */

Image*	CButtonToolbar::GetTooltipImage(CRect & rcTooltip)
{
	Image *				pResult = nullptr;
	CString				strTooltip;

	if (GetTooltipRect(rcTooltip, strTooltip))
	{
		pResult = new Bitmap(rcTooltip.Width(), rcTooltip.Height(), PixelFormat32bppARGB);
		if (pResult)
		{
			Graphics			graphics(pResult);
			Font				font(FontFamily::GenericSansSerif(), 10, FontStyleRegular);
			StringFormat		format;
			SolidBrush			brush(Color(0, 0, 0));
			PointF				pt(10, 10);

			GraphicsPath		path;

			LONG				lWidth = rcTooltip.Width()*10;
			LONG				lHeight = rcTooltip.Height()*10;

			path.SetFillMode(FillModeWinding);
			path.AddRectangle(RectF(10*10, 0, lWidth-20*10, lHeight));
			path.AddRectangle(RectF(0, 10*10, lWidth, lHeight-20*10));

			path.AddEllipse(0, 0, 20*10, 20*10);
			path.AddEllipse(lWidth-20*10, 0, 20*10, 20*10);
			path.AddEllipse(0, lHeight-20*10, 20*10, 20*10);
			path.AddEllipse(lWidth-20*10, lHeight-20*10, 20*10, 20*10);
			//path.Flatten();
			path.Outline(nullptr, (REAL)0.01);

			Matrix				matrix;

			matrix.Scale((REAL)0.1, (REAL)0.1);
			path.Transform(&matrix);

			PathGradientBrush	pathbrush(&path);

			Color				colors(0, 255, 255, 255);
			INT					nrcolors = 1;
			pathbrush.SetCenterColor(Color(220, 255, 255, 255));
			pathbrush.SetSurroundColors(&colors, &nrcolors);
			pathbrush.SetFocusScales((REAL)(1.0-30.0/rcTooltip.Width()), (REAL)(1.0-30.0/rcTooltip.Height()));

			graphics.FillRectangle(&pathbrush, 0, 0, rcTooltip.Width(), rcTooltip.Height());

			format.SetAlignment(StringAlignmentNear);
			format.SetLineAlignment(StringAlignmentNear);

			graphics.DrawString(CComBSTR(strTooltip), -1, &font, pt, &format, &brush);
		};
	};

	return pResult;
};

/* ------------------------------------------------------------------- */

BOOL	CButtonToolbar::OnMouseEnter([[maybe_unused]] LONG lX, [[maybe_unused]] LONG lY)
{
	BOOL				bResult = FALSE;

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CButtonToolbar::OnMouseMove(LONG lX, LONG lY)
{
	BOOL				bResult = FALSE;

	if (!m_bMouseIn)
	{
		m_bMouseIn = TRUE;
		bResult = TRUE;
	};

	if (!m_bButtonDown)
	{
		LONG			lHotButton = -1;
		LONG			lNewHotButton  = -1;
		LONG			lTooltipButton = -1;
		LONG			lNewTooltipButton = -1;

		// Update Hot Status
		GetHotButton(lHotButton);
		GetTooltipButton(lTooltipButton);
		ResetTooltip();

		if (GetButton(lX, lY, lNewHotButton))
		{
			m_vButtons[lNewHotButton].m_bShowTooltip = TRUE;
			lNewTooltipButton = lNewHotButton;
			if ((m_vButtons[lNewHotButton].m_Status != MBS_UP) &&
				(m_vButtons[lNewHotButton].m_Status != MBS_HOT))
				lNewHotButton = -1;
		};

		if (lHotButton != lNewHotButton)
		{
			bResult = TRUE;
			ResetHot();
			if (lNewHotButton>=0)
				m_vButtons[lNewHotButton].m_Status = MBS_HOT;
		}
		else if (lTooltipButton != lNewTooltipButton)
			bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CButtonToolbar::OnMouseLeave()
{
	BOOL				bResult = FALSE;

	if (m_bMouseIn)
	{
		ResetHot();
		ResetTooltip();
		m_bMouseIn = FALSE;
		bResult    = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CButtonToolbar::OnLButtonDown(LONG lX, LONG lY)
{
	BOOL				bResult = FALSE;
	// BOOL				bFound = FALSE;
	LONG				lIndice;

	m_bButtonDown = TRUE;
	// Check the button under the cursor if possible
	if (GetButton(lX, lY, lIndice))
	{
		if ((m_vButtons[lIndice].m_Status == MBS_UP) || (m_vButtons[lIndice].m_Status == MBS_HOT))
		{
			if (m_vButtons[lIndice].m_Type == MBT_CHECKBOX)
				m_vButtons[lIndice].m_Status = MBS_DOWN;
			bResult = TRUE;
			// Here advice of the change...somehow
			if (m_pSink)
			{
				if (m_vButtons[lIndice].m_Type == MBT_CHECKBOX)
					m_pSink->ButtonToolbar_OnCheck(m_vButtons[lIndice].m_dwID, this);
				else
					m_pSink->ButtonToolbar_OnClick(m_vButtons[lIndice].m_dwID, this);
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CButtonToolbar::OnLButtonUp([[maybe_unused]] LONG lX, [[maybe_unused]] LONG lY)
{
	BOOL				bResult = FALSE;

	m_bButtonDown = FALSE;

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CButtonToolbar::OnRButtonDown(LONG lX, LONG lY)
{
	BOOL				bResult = FALSE;
	LONG				lIndice;

	// Check the button under the cursor if possible
	if (GetButton(lX, lY, lIndice))
	{
		bResult = TRUE;
		// Here advice of the change...somehow
		if (m_pSink)
			m_pSink->ButtonToolbar_OnRClick(m_vButtons[lIndice].m_dwID, this);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
