#include "stdafx.h"
#include "catch.h"

#include "RegisterEngine.h"


TEST_CASE("Register engine", "[Register][RegisterSubrect]")
{
	SECTION("Single pixel no star")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 10.0;

		pGray->m_vPixels[91 * W + 105] = 200.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 0);
	}

	SECTION("Cross of 3 pixels is a star")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 10.0;

		pGray->m_vPixels[91 * W + 105] = 200.0;
		pGray->m_vPixels[91 * W + 106] = 200.0;
		pGray->m_vPixels[91 * W + 107] = 200.0;
		pGray->m_vPixels[90 * W + 106] = 200.0;
		pGray->m_vPixels[92 * W + 106] = 200.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 1);
		REQUIRE(stars.cbegin()->m_fX == 106.0);
		REQUIRE(stars.cbegin()->m_fY == 91.0);
		REQUIRE(stars.cbegin()->m_fMeanRadius < 1.25);
	}

	SECTION("Cross of 3 pixels but below threshold is no star")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 50.0;

		pGray->m_vPixels[91 * W + 105] = 70.0;
		pGray->m_vPixels[91 * W + 106] = 70.0;
		pGray->m_vPixels[91 * W + 107] = 70.0;
		pGray->m_vPixels[90 * W + 106] = 70.0;
		pGray->m_vPixels[92 * W + 106] = 70.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 0);
	}

	SECTION("Block of 9 pixels")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 50.0;

		pGray->m_vPixels[96 * W + 115] = 180.0;
		pGray->m_vPixels[96 * W + 116] = 190.0;
		pGray->m_vPixels[96 * W + 117] = 180.0;
		pGray->m_vPixels[97 * W + 115] = 190.0;
		pGray->m_vPixels[97 * W + 116] = 200.0;
		pGray->m_vPixels[97 * W + 117] = 190.0;
		pGray->m_vPixels[98 * W + 115] = 180.0;
		pGray->m_vPixels[98 * W + 116] = 190.0;
		pGray->m_vPixels[98 * W + 117] = 180.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 1);
		REQUIRE(stars.cbegin()->m_fX == 116.0);
		REQUIRE(stars.cbegin()->m_fY == 97.0);
		REQUIRE(stars.cbegin()->m_fMeanRadius < 1.25);
	}

	SECTION("Block of 4 pixels, result is independent of background level")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 60.0;

		pGray->m_vPixels[100 * W + 93] = 150.0;
		pGray->m_vPixels[100 * W + 94] = 150.0;
		pGray->m_vPixels[101 * W + 93] = 150.0;
		pGray->m_vPixels[101 * W + 94] = 150.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 1);
		REQUIRE(stars.cbegin()->m_fX == 93.5);
		REQUIRE(stars.cbegin()->m_fY == 100.5);
		REQUIRE(stars.cbegin()->m_fMeanRadius == 0.75);
		REQUIRE(stars.cbegin()->m_fCircularity == Approx((150.0 - 60.0) / 1.1).epsilon(1e-2));
// ----------------------------------------------------------------------
		for (auto& v : pGray->m_vPixels)
			v = 0.0;

		pGray->m_vPixels[100 * W + 93] = 150.0;
		pGray->m_vPixels[100 * W + 94] = 150.0;
		pGray->m_vPixels[101 * W + 93] = 150.0;
		pGray->m_vPixels[101 * W + 94] = 150.0;

		stars.clear();
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 1);
		REQUIRE(stars.cbegin()->m_fX == 93.5);
		REQUIRE(stars.cbegin()->m_fY == 100.5);
		REQUIRE(stars.cbegin()->m_fMeanRadius == 0.75);
		REQUIRE(stars.cbegin()->m_fCircularity == Approx(150.0 / 1.1).epsilon(1e-2));
	}

	SECTION("Block of 13 pixels")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 30.0;

		pGray->m_vPixels[96 * W + 115] = 120.0;
		pGray->m_vPixels[96 * W + 116] = 120.0;
		pGray->m_vPixels[96 * W + 117] = 120.0;
		pGray->m_vPixels[97 * W + 115] = 120.0;
		pGray->m_vPixels[97 * W + 116] = 120.0;
		pGray->m_vPixels[97 * W + 117] = 120.0;
		pGray->m_vPixels[98 * W + 115] = 120.0;
		pGray->m_vPixels[98 * W + 116] = 120.0;
		pGray->m_vPixels[98 * W + 117] = 120.0;

		pGray->m_vPixels[95 * W + 116] = 120.0;
		pGray->m_vPixels[97 * W + 114] = 120.0;
		pGray->m_vPixels[97 * W + 118] = 120.0;
		pGray->m_vPixels[99 * W + 116] = 120.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 1);
		REQUIRE(stars.cbegin()->m_fX == 116.0);
		REQUIRE(stars.cbegin()->m_fY == 97.0);
		REQUIRE(stars.cbegin()->m_fMeanRadius < 2.2);
	}

	SECTION("Rectangle of 15 pixels no star")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 50.0;

		pGray->m_vPixels[96 * W + 115] = 180.0;
		pGray->m_vPixels[96 * W + 116] = 180.0;
		pGray->m_vPixels[96 * W + 117] = 180.0;
		pGray->m_vPixels[96 * W + 118] = 180.0;
		pGray->m_vPixels[96 * W + 119] = 180.0;
		pGray->m_vPixels[97 * W + 115] = 180.0;
		pGray->m_vPixels[97 * W + 116] = 180.0;
		pGray->m_vPixels[97 * W + 117] = 180.0;
		pGray->m_vPixels[97 * W + 118] = 180.0;
		pGray->m_vPixels[97 * W + 119] = 180.0;
		pGray->m_vPixels[98 * W + 115] = 180.0;
		pGray->m_vPixels[98 * W + 116] = 180.0;
		pGray->m_vPixels[98 * W + 117] = 180.0;
		pGray->m_vPixels[98 * W + 118] = 180.0;
		pGray->m_vPixels[98 * W + 119] = 180.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 0);
	}

	SECTION("2 x 3 pixels are two stars")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 20.0;

		pGray->m_vPixels[85 * W + 93] = 200.0;
		pGray->m_vPixels[85 * W + 94] = 200.0;
		pGray->m_vPixels[85 * W + 95] = 200.0;
		pGray->m_vPixels[84 * W + 94] = 200.0;
		pGray->m_vPixels[86 * W + 94] = 200.0;

		pGray->m_vPixels[111 * W + 116] = 100.0;
		pGray->m_vPixels[111 * W + 117] = 100.0;
		pGray->m_vPixels[111 * W + 118] = 100.0;
		pGray->m_vPixels[110 * W + 117] = 100.0;
		pGray->m_vPixels[112 * W + 117] = 100.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 2);
	}

	SECTION("Block of 9 and block of 4 pixels are 2 stars")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 50.0;

		pGray->m_vPixels[96 * W + 115] = 180.0;
		pGray->m_vPixels[96 * W + 116] = 180.0;
		pGray->m_vPixels[96 * W + 117] = 180.0;
		pGray->m_vPixels[97 * W + 115] = 180.0;
		pGray->m_vPixels[97 * W + 116] = 180.0;
		pGray->m_vPixels[97 * W + 117] = 180.0;
		pGray->m_vPixels[98 * W + 115] = 180.0;
		pGray->m_vPixels[98 * W + 116] = 180.0;
		pGray->m_vPixels[98 * W + 117] = 180.0;

		pGray->m_vPixels[115 * W + 82] = 180.0;
		pGray->m_vPixels[115 * W + 83] = 180.0;
		pGray->m_vPixels[116 * W + 82] = 180.0;
		pGray->m_vPixels[116 * W + 83] = 180.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 2);
	}

	SECTION("Block of 9 and block of 4 pixels close together is one star")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 50.0;

		pGray->m_vPixels[96 * W + 115] = 180.0;
		pGray->m_vPixels[96 * W + 116] = 180.0;
		pGray->m_vPixels[96 * W + 117] = 180.0;
		pGray->m_vPixels[97 * W + 115] = 180.0;
		pGray->m_vPixels[97 * W + 116] = 180.0;
		pGray->m_vPixels[97 * W + 117] = 180.0;
		pGray->m_vPixels[98 * W + 115] = 180.0;
		pGray->m_vPixels[98 * W + 116] = 180.0;
		pGray->m_vPixels[98 * W + 117] = 180.0;

		pGray->m_vPixels[94 * W + 118] = 180.0;
		pGray->m_vPixels[94 * W + 119] = 180.0;
		pGray->m_vPixels[95 * W + 118] = 180.0;
		pGray->m_vPixels[95 * W + 119] = 180.0;

		constexpr qreal iv = std::numeric_limits<qreal>::quiet_NaN();
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, nullptr, QPointF{ iv, iv });

		REQUIRE(stars.size() == 1);
		REQUIRE(stars.cbegin()->m_fX == Approx(117.7).epsilon(1e-2));
		REQUIRE(stars.cbegin()->m_fY == Approx(94.5).epsilon(1e-2));
	}

	SECTION("Calling registerSubrect twice with cache returns identical results")
	{
		constexpr int W = 200;
		constexpr int H = 180;

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmap>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmap*>(pBitmap.get());
		for (auto& v : pGray->m_vPixels)
			v = 30.0;

		pGray->m_vPixels[96 * W + 115] = 120.0;
		pGray->m_vPixels[96 * W + 116] = 120.0;
		pGray->m_vPixels[96 * W + 117] = 120.0;
		pGray->m_vPixels[97 * W + 115] = 120.0;
		pGray->m_vPixels[97 * W + 116] = 125.0;
		pGray->m_vPixels[97 * W + 117] = 120.0;
		pGray->m_vPixels[98 * W + 115] = 120.0;
		pGray->m_vPixels[98 * W + 116] = 120.0;
		pGray->m_vPixels[98 * W + 117] = 120.0;

		pGray->m_vPixels[95 * W + 116] = 120.0;
		pGray->m_vPixels[97 * W + 114] = 120.0;
		pGray->m_vPixels[97 * W + 118] = 120.0;
		pGray->m_vPixels[99 * W + 116] = 120.0;

		constexpr double iv = std::numeric_limits<qreal>::quiet_NaN();
		auto cache = std::make_pair(iv, iv);
		STARSET stars;
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, std::addressof(cache), QPointF{iv, iv});
		const auto star1 = *stars.cbegin();

		REQUIRE(std::isfinite(cache.first));
		REQUIRE(std::isfinite(cache.second));
		REQUIRE(cache.first == 125); // Max intensity
		REQUIRE(cache.second == 30.0 / 256.0); // Background level

		stars.clear();
		DSS::registerSubRect(*pGray, 0.2, DSSRect{ 70, 70, 130, 130 }, stars, std::addressof(cache), QPointF{ iv, iv });
		const auto star2 = *stars.cbegin();

		REQUIRE(star1.m_fCircularity == star2.m_fCircularity);
		REQUIRE(star1.m_fIntensity == star2.m_fIntensity);
		REQUIRE(star1.m_fMeanRadius == star2.m_fMeanRadius);
		REQUIRE(star1.m_fQuality == star2.m_fQuality);
		REQUIRE(star1 == star2);
		REQUIRE(star1.m_rcStar.left == star2.m_rcStar.left);
		REQUIRE(star1.m_rcStar.right == star2.m_rcStar.right);
		REQUIRE(star1.m_rcStar.top == star2.m_rcStar.top);
		REQUIRE(star1.m_rcStar.bottom == star2.m_rcStar.bottom);
	}
}
