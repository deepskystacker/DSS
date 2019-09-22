#include <stdafx.h>
#include <WndImage.h>
#include "BitmapExt.h"
#include "StackedSink.h"

#define _USE_MATH_DEFINES
#include <math.h>


/* ------------------------------------------------------------------- */

Image *	CStackedSink::GetOverlayImage(CRect & rcClient)
{
	Image *				pResult = NULL;
	HDC					hDC = GetDC(NULL);

	//pResult = new Bitmap(rcClient.Width(), rcClient.Height(), PixelFormat32bppARGB);
	if (m_pt1!=m_pt2)
	{
		pResult = new Metafile(hDC, RectF(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()), MetafileFrameUnitPixel, EmfTypeEmfPlusOnly, NULL);

		if (pResult)
		{
			Graphics		graphics(pResult);
			Pen				pen(Color(180, 12, 248, 244), 1.0);
			CPointExt		pt1, pt2, pt3, pt4;

			pt1 = m_pt1;
			pt2 = m_pt2;
			pt3 = m_pt3;
			pt4 = m_pt4;

			m_pImage->BitmapToScreen(pt1.X, pt1.Y);
			m_pImage->BitmapToScreen(pt2.X, pt2.Y);
			m_pImage->BitmapToScreen(pt3.X, pt3.Y);
			m_pImage->BitmapToScreen(pt4.X, pt4.Y);

			graphics.DrawLine(&pen, (REAL)pt1.X, pt1.Y, pt2.X, pt2.Y);
			graphics.DrawLine(&pen, (REAL)pt2.X, pt2.Y, pt3.X, pt3.Y);
			graphics.DrawLine(&pen, (REAL)pt3.X, pt3.Y, pt4.X, pt4.Y);
			graphics.DrawLine(&pen, (REAL)pt4.X, pt4.Y, pt1.X, pt1.Y);
		};
	};

	return pResult;
};

/* ------------------------------------------------------------------- */

void	CStackedSink::SetFootprint(CPointExt pt1, CPointExt pt2, CPointExt pt3, CPointExt pt4)
{
	m_pt1 = pt1;
	m_pt2 = pt2;
	m_pt3 = pt3;
	m_pt4 = pt4;
};

/* ------------------------------------------------------------------- */

void	CStackedSink::ClearFootprint()
{
	m_pt4.X = m_pt4.Y = 0;
	m_pt1 = m_pt2 = m_pt3 = m_pt4;
};

/* ------------------------------------------------------------------- */

