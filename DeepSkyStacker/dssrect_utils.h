#pragma once
#include "dssrect.h"
#include <QPoint>

namespace DSS {

	//
	// This is a replacement for the old CPointExt::IsInRect(double fLeft, double fTop, double fRight, double fBottom)
	// The check has been copied literally.
	//
	inline bool pointIsInRect(const QPointF& point, const double rectleft, const double recttop, const double rectright, const double rectbottom) noexcept
	{
		return (point.x() >= rectleft) && (point.x() <= rectright) && (point.y() >= recttop) && (point.y() <= rectbottom);
	}

	//
	// This is a replacement for the old CRect::PtInRect(const CPoint& pt)
	// The check has been copied literally. Even if it seems to be a bug (as DSSRect right and bottom are exclusive), we leave it as it is for the moment.
	// CPoint had indices LONG x, LONG y.
	//
	inline bool blockContainsPixel(const DSSRect& block, const QPoint& pixel) noexcept
	{
		return pixel.x() >= block.getLeft() && pixel.x() <= block.getRight() && pixel.y() >= block.getTop() && pixel.y() <= block.getBottom();
	}

}
