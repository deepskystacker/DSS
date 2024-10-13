#include "stdafx.h"
#include <QSettings>
#include "catch.h"
#include "avx_histogram.h"
#include "MedianFilterEngine.h"

TEST_CASE("AVX Histogram", "[AVX][Histogram]")
{
	SECTION("Gray pixels same value")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set to 100
		pGray->m_vPixels.assign(256 * 32, 100);

		REQUIRE(avxHistogram.calcHistogram(0, 32, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expected(65536, 0);
		expected[100] = 256 * 32;
		REQUIRE(memcmp(redHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("All pixels same value, final histogram has different size")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set to 1000
		pGray->m_vPixels.assign(256 * 32, 1000);

		REQUIRE(avxHistogram.calcHistogram(0, 32, 1) == 0);
		std::vector<int> redHisto(65537), greenHisto(65537), blueHisto(65537);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expected(65536, 0);
		expected[1000] = 256 * 32;
		REQUIRE(memcmp(redHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("RGB pixels same value")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CColorBitmapT<std::uint16_t>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pColor = dynamic_cast<CColorBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set to 55, 66, 77
		pColor->m_Red.m_vPixels.assign(256 * 32, 55);
		pColor->m_Green.m_vPixels.assign(256 * 32, 66);
		pColor->m_Blue.m_vPixels.assign(256 * 32, 77);

		REQUIRE(avxHistogram.calcHistogram(0, 32, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expectedRed(65536, 0), expectedGreen(65536, 0), expectedBlue(65536, 0);
		expectedRed[55] = 256 * 32;
		expectedGreen[66] = 256 * 32;
		expectedBlue[77] = 256 * 32;
		REQUIRE(memcmp(redHisto.data(), expectedRed.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expectedGreen.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expectedBlue.data(), 65536 * sizeof(int)) == 0);
	}

	// Since we use the SimdSelector, even small arrays will be processed (by the class NonAvxHistogram).
	SECTION("Calculation even for small arrays")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(16, 4) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set to 287
		pGray->m_vPixels.assign(16 * 4, 287);

		REQUIRE(avxHistogram.calcHistogram(0, 4, 1) == 0);
	}

	SECTION("Gray pixels, calc 32 lines as 2 x 16")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		AvxHistogram avxHistogram1(*pBitmap), avxHistogram2(*pBitmap);
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set to 793
		pGray->m_vPixels.assign(256 * 32, 793);

		REQUIRE(avxHistogram1.calcHistogram(0, 16, 1) == 0);
		REQUIRE(avxHistogram2.calcHistogram(16, 32, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram1.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);
		REQUIRE(avxHistogram2.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expected(65536, 0);
		expected[793] = 256 * 32;
		REQUIRE(memcmp(redHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("Gray pixels, calc 32 lines one by one")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set to 348
		pGray->m_vPixels.assign(256 * 32, 348);

		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		bool b = true;
		for (size_t i = 0; i < 32; ++i)
		{
			AvxHistogram avxHistogram(*pBitmap);
			b &= (avxHistogram.calcHistogram(i, i + 1, 1) == 0);
			b &= (avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);
		}
		REQUIRE(b == true);

		std::vector<int> expected(65536, 0);
		expected[348] = 256 * 32;
		REQUIRE(memcmp(redHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("RGB pixels all possible value")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CColorBitmapT<std::uint16_t>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(256, 256) == true);

		auto* pColor = dynamic_cast<CColorBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set values
		std::uint16_t i = 0;
		std::for_each(pColor->m_Red.m_vPixels.begin(), pColor->m_Red.m_vPixels.end(), [&i](auto& v) { v = i++; });
		i = 0;
		std::for_each(pColor->m_Green.m_vPixels.begin(), pColor->m_Green.m_vPixels.end(), [&i](auto& v) { v = i++; });
		i = 0;
		std::for_each(pColor->m_Blue.m_vPixels.begin(), pColor->m_Blue.m_vPixels.end(), [&i](auto& v) { v = i++; });

		REQUIRE(avxHistogram.calcHistogram(0, 256, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expectedRed(65536, 1), expectedGreen(65536, 1), expectedBlue(65536, 1);
		REQUIRE(memcmp(redHisto.data(), expectedRed.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expectedGreen.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expectedBlue.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("Gray pixels float, values are truncated and limited to 65535")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<float>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<float>*>(pBitmap.get());
		// Set values
		pGray->m_vPixels.assign(256 * 32, 236.99f); // 236.99 should be truncated to 236.
		pGray->m_vPixels[1000] = 1e5f; // 100000 should be limited to 65535.

		REQUIRE(avxHistogram.calcHistogram(0, 32, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expected(65536, 0);
		expected[236] = 256 * 32 - 1;
		expected[65535] = 1;
		REQUIRE(memcmp(redHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("RGB pixels float, 1, 2, and 3 are repeated.")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CColorBitmapT<float>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pColor = dynamic_cast<CColorBitmapT<float>*>(pBitmap.get());
		// Set values
		int i = 0;
		std::for_each(pColor->m_Red.m_vPixels.begin(), pColor->m_Red.m_vPixels.end(), [&i](auto& v) { v = static_cast<float>(i++); i %= 3; }); // 0, 1, 2, 0, 1, 2, ...
		i = 1;
		std::for_each(pColor->m_Green.m_vPixels.begin(), pColor->m_Green.m_vPixels.end(), [&i](auto& v) { v = static_cast<float>(i++); i %= 3; }); // 1, 2, 0, 1, 2, 0, ...
		i = 2;
		std::for_each(pColor->m_Blue.m_vPixels.begin(), pColor->m_Blue.m_vPixels.end(), [&i](auto& v) { v = static_cast<float>(i++); i %= 3; }); // 2, 0, 1, 2, 0, 1, ...

		REQUIRE(avxHistogram.calcHistogram(0, 32, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expectedRed(65536, 0), expectedGreen(65536, 0), expectedBlue(65536, 0);
		expectedRed[0] = 2731;
		expectedRed[1] = 2731;
		expectedRed[2] = 2730;
		expectedGreen[1] = 2731;
		expectedGreen[2] = 2731;
		expectedGreen[0] = 2730;
		expectedBlue[2] = 2731;
		expectedBlue[0] = 2731;
		expectedBlue[1] = 2730;
		REQUIRE(memcmp(redHisto.data(), expectedRed.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expectedGreen.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expectedBlue.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("Gray pixels int32, values are shifted right by 16 bits")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint32_t>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(256, 256) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pBitmap.get());
		// Set values
		std::uint32_t i = 0;
		std::for_each(pGray->m_vPixels.begin(), pGray->m_vPixels.end(), [&i](auto& v) { v = i; i += 65536; });
		pGray->m_vPixels[1000] = 60000; // 60000 should become 0.
		pGray->m_vPixels[1001] = (7328 << 16);

		REQUIRE(avxHistogram.calcHistogram(0, 256, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expected(65536, 1);
		expected[0] = 2;
		expected[1000] = 0;
		expected[1001] = 0;
		expected[7328] = 2;
		REQUIRE(memcmp(redHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("Gray pixels int32, number of columns is no multiple of 16")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint32_t>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(271, 200) == true); // 271 is 16 * 16 + 15

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pBitmap.get());
		// Set values
		std::uint32_t i = 0;
		std::for_each(pGray->m_vPixels.begin(), pGray->m_vPixels.end(), [&i](auto& v) { v = i; i += 65536; });
		for (size_t n = 100 * 271; n < 101 * 271; ++n) // One line has constant value 64000.
			pGray->m_vPixels[n] = static_cast<std::uint32_t>(64000 << 16);

		REQUIRE(avxHistogram.calcHistogram(0, 200, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expected(65536, 1);
		for (size_t n = 271 * 200; n < 65536; ++n) // These pixels don't exist.
			expected[n] = 0;
		for (size_t n = 100 * 271; n < 101 * 271; ++n)
			expected[n] = 0; // These have been replaced with 64000.
		expected[64000] = 271;
		REQUIRE(memcmp(redHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
	}

	SECTION("Gray pixels double, values are multiplied by 256, truncated, and limited to 65535.")
	{
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<double>>();
		AvxHistogram avxHistogram(*pBitmap);
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<double>*>(pBitmap.get());
		// Set values
		pGray->m_vPixels.assign(256 * 32, 202.01133); // 202.01133 should become trunc(202.01133 * 256.0) = trunc(51714.90) = 51714.
		pGray->m_vPixels[1000] = 260.0; // 260.0 should be limited to 65535.

		REQUIRE(avxHistogram.calcHistogram(0, 32, 1) == 0);
		std::vector<int> redHisto(65536), greenHisto(65536), blueHisto(65536);
		REQUIRE(avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto) == 0);

		std::vector<int> expected(65536, 0);
		expected[51714] = 256 * 32 - 1;
		expected[65535] = 1;
		REQUIRE(memcmp(redHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(greenHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
		REQUIRE(memcmp(blueHisto.data(), expected.data(), 65536 * sizeof(int)) == 0);
	}
}
// 
// namespace DSS { class ProgressBase; }
// void CGrayBitmapT<float>::RemoveHotPixels(DSS::ProgressBase*) {}
// std::shared_ptr<CMemoryBitmap> CGrayMedianFilterEngineT<float>::GetFilteredImage(int, DSS::ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// std::shared_ptr<CMemoryBitmap> CColorMedianFilterEngineT<float>::GetFilteredImage(int, DSS::ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// 
// void CGrayBitmapT<unsigned short>::RemoveHotPixels(DSS::ProgressBase*) {}
// std::shared_ptr<CMemoryBitmap> CGrayMedianFilterEngineT<unsigned short>::GetFilteredImage(int, DSS::ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// std::shared_ptr<CMemoryBitmap> CColorMedianFilterEngineT<unsigned short>::GetFilteredImage(int, DSS::ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// 
// void CGrayBitmapT<std::uint32_t>::RemoveHotPixels(DSS::ProgressBase*) {}
// std::shared_ptr<CMemoryBitmap> CGrayMedianFilterEngineT<std::uint32_t>::GetFilteredImage(int, DSS::ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// std::shared_ptr<CMemoryBitmap> CColorMedianFilterEngineT<std::uint32_t>::GetFilteredImage(int, DSS::ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// 
// void CGrayBitmapT<double>::RemoveHotPixels(DSS::ProgressBase*) {}
// std::shared_ptr<CMemoryBitmap> CGrayMedianFilterEngineT<double>::GetFilteredImage(int, DSS::ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// std::shared_ptr<CMemoryBitmap> CColorMedianFilterEngineT<double>::GetFilteredImage(int, DSS::ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
