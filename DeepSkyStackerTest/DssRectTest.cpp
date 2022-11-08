#include "stdafx.h"
#include "catch.h"
#include "../DeepSkyStacker/DssRectUtils.h"


// Old CPointExt class
class CPointExt
{
public:
	double			X;
	double			Y;
private:
	void	CopyFrom(const CPointExt& pt)
	{
		X = pt.X;
		Y = pt.Y;
	};
public:
	CPointExt(double x = 0, double y = 0)
	{
		X = x;
		Y = y;
	};
	CPointExt(const CPointExt& pt)
	{
		CopyFrom(pt);
	};
	//CPointExt(const CPoint& pt)
	//{
	//	X = pt.x;
	//	Y = pt.y;
	//};
	//CPointExt(const CRect& rc)
	//{
	//	X = (double)(rc.left + rc.right) / 2.0;
	//	Y = (double)(rc.top + rc.bottom) / 2.0;
	//};
	CPointExt& operator = (const CPointExt& pt)
	{
		CopyFrom(pt);
		return (*this);
	};
	void	Offset(const CPointExt& pt)
	{
		X -= pt.X;
		Y -= pt.Y;
	};
	//void	CopyTo(CPoint& pt)
	//{
	//	pt.x = X;
	//	pt.y = Y;
	//};
	bool operator != (const CPointExt& pt)
	{
		return (X != pt.X) || (Y != pt.Y);
	};
	bool operator == (const CPointExt& pt)
	{
		return (X == pt.X) && (Y == pt.Y);
	};
	bool operator < (const CPointExt& pt)
	{
		return (X < pt.X);
	};
	BOOL	IsInRect(double fLeft, double fTop, double fRight, double fBottom)
	{
		return (X >= fLeft) && (X <= fRight) && (Y >= fTop) && (Y <= fBottom);
	};
	BOOL	IsNear(const CPointExt& ptTest)
	{
		return (fabs(X - ptTest.X) <= 3) && (fabs(Y - ptTest.Y) <= 3);
	};
	void Rotate(double fAngle, const CPointExt& ptCenter)
	{
		CPointExt		ptResult;
		double			fX, fY;
		ptResult.X = X - ptCenter.X;
		ptResult.Y = Y - ptCenter.Y;
		fX = cos(fAngle) * ptResult.X - sin(fAngle) * ptResult.Y;
		fY = sin(fAngle) * ptResult.X + cos(fAngle) * ptResult.Y;
		X = fX + ptCenter.X;
		Y = fY + ptCenter.Y;
	};
};

template <class T>
class PointT
{
	T c0{ 0 }, c1{ 0 };
public:
	explicit PointT(const T x, const T y) : c0{ x }, c1{ y } {}
	T x() const { return c0; }
	T y() const { return c1; }
};

using Point = PointT<int>;
using PointF = PointT<double>;

constexpr int leftEdge = 1;
constexpr int topEdge = 2;
constexpr int rightEdge = 7;
constexpr int bottomEdge = 10;

TEST_CASE("DSSRect with pixel", "[DSSRect pixel]")
{
	SECTION("Width and height return correct values")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };

		REQUIRE(rect.width() == rightEdge - leftEdge);
		REQUIRE(rect.height() == bottomEdge - topEdge);
	}

	SECTION("Function rectContainsPoint() returns true for a pixel in the middle of the block")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const Point pixel{ (rightEdge - leftEdge) / 2, (bottomEdge - topEdge) / 2 };

		REQUIRE(rectContainsPoint(rect, pixel) == true);
	}

	SECTION("Function pointInRect() returns true for a pixel in the middle of the block")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const Point pixel{ (rightEdge - leftEdge) / 2, (bottomEdge - topEdge) / 2 };

		REQUIRE(pointInRect(pixel, rect) == true);
	}

	SECTION("Function rectContainsPoint() returns true for a pixel in the last row and column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const Point pixel{ rightEdge - 1, bottomEdge - 1 };

		REQUIRE(rectContainsPoint(rect, pixel) == true);
	}

	SECTION("Function rectContainsPoint() returns false for a pixel in the rightEdge column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const Point pixel{ rightEdge, bottomEdge - 1 }; // x-index of the pixel is 'rightEdge', so it's outside the block.

		REQUIRE(rectContainsPoint(rect, pixel) == false);
	}

	SECTION("Function pointInRect() returns false for a pixel in the rightEdge column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const Point pixel{ rightEdge, bottomEdge - 1 }; // x-index of the pixel is 'rightEdge', so it's outside the block.

		REQUIRE(pointInRect(pixel, rect) == false);
	}
}

TEST_CASE("DSSRect with point", "[DSSRect point]")
{

	SECTION("Function rectContainsPoint() returns true for a point in the last row and column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const PointF point{ rightEdge - 1, bottomEdge - 1 };

		REQUIRE(rectContainsPoint(rect, point) == true);
	}

	SECTION("Function rectContainsPoint() returns true for a point in the rightEdge column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const PointF point{ rightEdge, bottomEdge }; // x-index of the point is 'rightEdge'

		REQUIRE(rectContainsPoint(rect, point) == true); // We must reproduce this behaviour to be compatible with code prior to Qt.
	}

	SECTION("Function rectContainsPoint() returns false for a point just outside the rightEdge column")
	{
		constexpr double rightOffset = 1e-10;
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const PointF point{ rightEdge + rightOffset, bottomEdge }; // x-index of the point is 'rightEdge + 1e-10'

		REQUIRE(rectContainsPoint(rect, point) == false);
	}

	SECTION("rectContainsPoint, pointInRect, and CPointExt::IsInRect are identical (true) for a point on the border")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const PointF point{ rightEdge, bottomEdge };
		CPointExt ptExt{ rightEdge, bottomEdge }; // Cannot be const as IsInRect is non-const :-(

		REQUIRE(rectContainsPoint(rect, point) == true);
		REQUIRE(pointInRect(point, rect) == true);
		REQUIRE(static_cast<bool>(ptExt.IsInRect(leftEdge, topEdge, rightEdge, bottomEdge)) == true);
	}

	SECTION("rectContainsPoint, pointInRect, and CPointExt::IsInRect are identical (false) for a point just outside the border")
	{
		constexpr double offset = 1e-10;
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const PointF point{ rightEdge, bottomEdge + offset };
		CPointExt ptExt{ rightEdge, bottomEdge + offset }; // Cannot be const as IsInRect is non-const :-(

		REQUIRE(rectContainsPoint(rect, point) == false);
		REQUIRE(pointInRect(point, rect) == false);
		REQUIRE(static_cast<bool>(ptExt.IsInRect(leftEdge, topEdge, rightEdge, bottomEdge)) == false);
	}
}

#include <atltypes.h>

TEST_CASE("CRect", "[CRect]")
{
	SECTION("CRect")
	{
		CRect rect{ 0, 0, 5, 5 };
		CPoint point{ 5, 5 };

		REQUIRE(rect.PtInRect(point) == FALSE);
	}
}
