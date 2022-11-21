#include "stdafx.h"
#include "catch.h"
#include "../DeepSkyStacker/dssrect_utils.h"


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


constexpr int leftEdge = 1;
constexpr int topEdge = 2;
constexpr int rightEdge = 7;
constexpr int bottomEdge = 10;

TEST_CASE("DSSRect", "[DSSRect]")
{
	SECTION("Width and height return correct values")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };

		REQUIRE(rect.width() == rightEdge - leftEdge);
		REQUIRE(rect.height() == bottomEdge - topEdge);
	}

	SECTION("setEmpty sets the DSSRect empty")
	{
		DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		REQUIRE(rect.getRight() == rightEdge);
		rect.setEmpty();
		REQUIRE(rect.getLeft() == 0);
		REQUIRE(rect.getTop() == 0);
		REQUIRE(rect.getRight() == 0);
		REQUIRE(rect.getBottom() == 0);
	}

	SECTION("isEmpty returns true for an empty rect")
	{
		DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		rect.setCoords(leftEdge, 47, rightEdge, 47);
		REQUIRE(rect.isEmpty() == true);
	}

	SECTION("Function blockContainsPixel() returns true for a pixel in the middle of the block")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPoint pixel{ (rightEdge - leftEdge) / 2, (bottomEdge - topEdge) / 2 };

		REQUIRE(DSS::blockContainsPixel(rect, pixel) == true);
	}

	SECTION("Function pointIsInRect() returns true for a pixel in the middle of the block")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPointF point{ (rightEdge - leftEdge) / 2, (bottomEdge - topEdge) / 2 };

		REQUIRE(DSS::pointIsInRect(point, rect.getLeft(), rect.getTop(), rect.getRight(), rect.getBottom()) == true);
	}

	SECTION("Function blockContainsPixel() returns true for a pixel in the last row and column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPoint pixel{ rightEdge - 1, bottomEdge - 1 };

		REQUIRE(DSS::blockContainsPixel(rect, pixel) == true);
	}

	SECTION("Function blockContainsPixel() returns true for a pixel in the rightEdge column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPoint pixel{ rightEdge, bottomEdge - 1 }; // x-index of the pixel is 'rightEdge', so it's outside the block.

		REQUIRE(DSS::blockContainsPixel(rect, pixel) == true);
	}

	SECTION("Function pointIsInRect() returns true for a pixel in the rightEdge column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPointF pixel{ rightEdge, bottomEdge - 1 }; // x-index of the pixel is 'rightEdge', so it's outside the block.

		REQUIRE(DSS::pointIsInRect(pixel, rect.getLeft(), rect.getTop(), rect.getRight(), rect.getBottom()) == true);
	}

	SECTION("pointIsInRect and CPointExt::IsInRect are identical (true) for a point on the border")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPointF point{ rightEdge, bottomEdge };
		CPointExt ptExt{ rightEdge, bottomEdge }; // Cannot be const as IsInRect is non-const :-(

		REQUIRE(DSS::pointIsInRect(point, rect.getLeft(), rect.getTop(), rect.getRight(), rect.getBottom()) == true);
		REQUIRE(static_cast<bool>(ptExt.IsInRect(rect.getLeft(), rect.getTop(), rect.getRight(), rect.getBottom())) == true);
	}

	SECTION("pointIsInRect and CPointExt::IsInRect are identical (false) for a point just outside the border")
	{
		constexpr double offset = 1e-10;
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPointF point{ rightEdge, bottomEdge + offset };
		CPointExt ptExt{ rightEdge, bottomEdge + offset }; // Cannot be const as IsInRect is non-const :-(

		REQUIRE(DSS::pointIsInRect(point, rect.getLeft(), rect.getTop(), rect.getRight(), rect.getBottom()) == false);
		REQUIRE(static_cast<bool>(ptExt.IsInRect(rect.getLeft(), rect.getTop(), rect.getRight(), rect.getBottom())) == false);
	}
}

#include <atltypes.h>

TEST_CASE("CRect", "[CRect]")
{
	SECTION("CRect::PtInRect returns false for a pixel on the column outside the rect")
	{
		CRect rect{ 0, 0, 5, 5 };
		CPoint point{ 5, 5 };

		REQUIRE(rect.PtInRect(point) == FALSE);
	}
}

/*
#include <iostream>

class XC
{
	int i{ -1 };
	std::string s;
public:
	XC(const int x, std::string y) : i{ x }, s{ std::move(y) } {}
	XC(const XC& rhs) : i{ rhs.i }, s{ rhs.s } {
		std::cout << s << " copy constructor" << std::endl;
	}
	XC(XC&& rhs) : i{ rhs.i }, s{ rhs.s } {
		std::cout << s << " move constructor" << std::endl;
	}
	int iget() const { return this->i; }
};

XC xc_factory1(const std::string s) {
	const XC xc{ 1, s + " 1 const"};
	return xc;
}

XC xc_factory2(const std::string s) {
	XC xc{ 2, s + " 2 non-const" };
	return xc;
}

XC xc_factory3(const std::string s) {
	return XC{ 3, s + " 3 temp" };
}

TEST_CASE("RValue", "[rvalue]")
{
	XC x1 = xc_factory1("");
	XC x2 = xc_factory2("");
	XC x3 = xc_factory3("");
	const XC xc1 = xc_factory1("CONST");
	const XC xc2 = xc_factory2("CONST");
	const XC xc3 = xc_factory3("CONST");
	REQUIRE(x1.iget() == 1);
	REQUIRE(x2.iget() == 2);
	REQUIRE(x3.iget() == 3);
	REQUIRE(xc1.iget() == 1);
	REQUIRE(xc2.iget() == 2);
	REQUIRE(xc3.iget() == 3);
}
*/
