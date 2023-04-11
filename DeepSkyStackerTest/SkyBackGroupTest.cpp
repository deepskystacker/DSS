#include "stdafx.h"
#include "catch.h"
#include "SkyBackground.h"

TEST_CASE("SkyBackground", "[SkyBackground]")
{
	SECTION("Reset sets the background to zero")
	{
		CSkyBackground sb;
		sb.m_fBlue = 1;
		sb.m_fGreen = 2;
		sb.m_fRed = 3;
		sb.m_fLight = 4;

		sb.Reset();

		REQUIRE(sb.m_fBlue == 0);
		REQUIRE(sb.m_fGreen == 0);
		REQUIRE(sb.m_fRed == 0);
		REQUIRE(sb.m_fLight == 0);
	}
}