#include "stdafx.h"
#include "catch.h"
#include "../DeepSkyStacker/DSSTools.h"

TEST_CASE("DSSRect", "[DSSRect]")
{
	constexpr int leftEdge = 1;
	constexpr int topEdge = 2;
	constexpr int rightEdge = 7;
	constexpr int bottomEdge = 10;

	SECTION("Width and height return correct values")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };

		REQUIRE(rect.width() == rightEdge - leftEdge);
		REQUIRE(rect.height() == bottomEdge - topEdge);
	}

	SECTION("Function contains() returns true for a pixel in the middle of the block")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPoint pixel{ (rightEdge - leftEdge) / 2, (bottomEdge - topEdge) / 2 };

		REQUIRE(rect.contains(pixel) == true);
	}

	SECTION("Function contains() returns true for a pixel in the last row and column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPoint pixel{ rightEdge - 1, bottomEdge - 1 };

		REQUIRE(rect.contains(pixel) == true);
	}

	SECTION("Function contains() returns false for a pixel in the rightEdge column")
	{
		const DSSRect rect{ leftEdge, topEdge, rightEdge, bottomEdge };
		const QPoint pixel{ rightEdge, bottomEdge - 1 }; // x-index of the pixel is 'rightEdge', so it's outside the block.

		REQUIRE(rect.contains(pixel) == false);
	}
}
