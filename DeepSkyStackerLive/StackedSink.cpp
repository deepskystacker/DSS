#include <stdafx.h>
#include <WndImage.h>
#include "BitmapExt.h"
#include "StackedSink.h"

#define _MATH_DEFINES_DEFINED
#include <math.h>


/* ------------------------------------------------------------------- */

Image *	CStackedSink::GetOverlayImage(CRect & rcClient)
{
	Image *				pResult = nullptr;
	HDC					hDC = GetDC(nullptr);

	//pResult = new Bitmap(rcClient.Width(), rcClient.Height(), PixelFormat32bppARGB);
	if (m_pt1!=m_pt2)
	{
		pResult = new Metafile(hDC, RectF(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()), MetafileFrameUnitPixel, EmfTypeEmfPlusOnly, nullptr);

		if (pResult)
		{
			Graphics		graphics(pResult);
			Pen				pen(Color(180, 12, 248, 244), 1.0);
			QPointF		pt1, pt2, pt3, pt4;

			pt1 = m_pt1;
			pt2 = m_pt2;
			pt3 = m_pt3;
			pt4 = m_pt4;

			m_pImage->BitmapToScreen(pt1.rx(), pt1.ry());
			m_pImage->BitmapToScreen(pt2.rx(), pt2.ry());
			m_pImage->BitmapToScreen(pt3.rx(), pt3.ry());
			m_pImage->BitmapToScreen(pt4.rx(), pt4.ry());

			graphics.DrawLine(&pen, (REAL)pt1.x(), pt1.y(), pt2.x(), pt2.y());
			graphics.DrawLine(&pen, (REAL)pt2.x(), pt2.y(), pt3.x(), pt3.y());
			graphics.DrawLine(&pen, (REAL)pt3.x(), pt3.y(), pt4.x(), pt4.y());
			graphics.DrawLine(&pen, (REAL)pt4.x(), pt4.y(), pt1.x(), pt1.y());
		};
	};

	return pResult;
};

/* ------------------------------------------------------------------- */

void	CStackedSink::SetFootprint(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4)
{
	m_pt1 = pt1;
	m_pt2 = pt2;
	m_pt3 = pt3;
	m_pt4 = pt4;
};

/* ------------------------------------------------------------------- */

void	CStackedSink::ClearFootprint()
{
	m_pt4.rx() = m_pt4.ry() = 0;
	m_pt1 = m_pt2 = m_pt3 = m_pt4;
};

/* ------------------------------------------------------------------- */

