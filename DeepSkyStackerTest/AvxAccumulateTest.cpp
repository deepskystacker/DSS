#include "stdafx.h"
#include "catch.h"
#include "AvxAccumulateTest.h"
#include "../DeepSkyStacker/BitmapBase.h"
#include "../DeepSkyStacker/avx_avg.h"

#include "../DeepSkyStacker/TaskInfo.h"


TEST_CASE("AVX Accumulation FASTAVERAGE", "[AVX][Accumulation][FastAverage]")
{
	CTaskInfo taskInfo;
	taskInfo.SetMethod(MBP_FASTAVERAGE, 0, 0);

	SECTION("One gray frame with identical values int16")
	{
		constexpr int W = 256 + 7;
		constexpr int H = 16 * 21 + 11;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pTempBitmap;
		pTempBitmap.Attach(new CGrayBitmapT<std::uint16_t>);
		REQUIRE(pTempBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.m_p);
		// Set to 127
		pGray->m_vPixels.assign(W * H, 127);

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>); // July 2021: Output bitmap must be float.
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
		REQUIRE(avxAccumulation.accumulate(0) == 0);

		std::vector<float> expected(W * H, 127.0f);
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}

	SECTION("Two gray frames int16")
	{
		constexpr int W = 256 + 7;
		constexpr int H = 32 + 11;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		for (int i = 0; i < 2; ++i)
		{
			CSmartPtr<CMemoryBitmap> pTempBitmap;
			pTempBitmap.Attach(new CGrayBitmapT<std::uint16_t>);
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.m_p);
			pGray->m_vPixels.assign(W * H, 100 + i * 3000);

			CEntropyInfo entropyInfo;
			entropyInfo.Init(pTempBitmap, 10, nullptr);
			AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

			AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
			REQUIRE(avxAccumulation.accumulate(i) == 0);
		}

		std::vector<float> expected(W * H, 0.5f * (100.0f + 3100.0f));
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}

	SECTION("Three gray frames int32")
	{
		constexpr int W = 16 * 18 + 3;
		constexpr int H = 16 * 7 + 6;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		for (int i = 0; i < 3; ++i)
		{
			CSmartPtr<CMemoryBitmap> pTempBitmap;
			pTempBitmap.Attach(new CGrayBitmapT<std::uint32_t>);
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pTempBitmap.m_p);
			pGray->m_vPixels.assign(W * H, (303 + i * 4200) << 16);

			CEntropyInfo entropyInfo;
			entropyInfo.Init(pTempBitmap, 10, nullptr);
			AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

			AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
			REQUIRE(avxAccumulation.accumulate(i) == 0);
		}

		std::vector<float> expected(W * H, (static_cast<float>(303) + static_cast<float>(303 + 4200) + static_cast<float>(303 + 8400)) / 3.0f);
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}

	SECTION("Four gray frames float")
	{
		constexpr int W = 16 * 18 + 8;
		constexpr int H = 16 * 27 + 13;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		for (int i = 0; i < 4; ++i)
		{
			CSmartPtr<CMemoryBitmap> pTempBitmap;
			pTempBitmap.Attach(new CGrayBitmapT<float>);
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<float>*>(pTempBitmap.m_p);
			pGray->m_vPixels.assign(W * H, 16000.0f + i * 17.35f);
			pGray->m_vPixels[15] = i == 0 ? 0.0f : 1.263e7f;

			CEntropyInfo entropyInfo;
			entropyInfo.Init(pTempBitmap, 10, nullptr);
			AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

			AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
			REQUIRE(avxAccumulation.accumulate(i) == 0);
		}

		std::vector<float> expected(W * H, (16000.0f + 16017.35f + 16034.7f + 16052.05f) / 4.0f);
		expected[15] = (3.0f * 1.263e7f) / 4.0f;
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}

	SECTION("Three RGB frames int32")
	{
		constexpr int W = 16 * 69 + 2;
		constexpr int H = 16 * 73 + 15;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CColorBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CColorBitmapT<float>*>(pOutBitmap.m_p);

		for (int i = 0; i < 3; ++i)
		{
			CSmartPtr<CMemoryBitmap> pTempBitmap;
			pTempBitmap.Attach(new CColorBitmapT<std::uint32_t>);
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pColor = dynamic_cast<CColorBitmapT<std::uint32_t>*>(pTempBitmap.m_p);
			pColor->m_Red.m_vPixels.assign(W * H, (303 + i * 4200) << 16);
			pColor->m_Green.m_vPixels.assign(W * H, (304 + i * 4201) << 16);
			pColor->m_Blue.m_vPixels.assign(W * H, (305 + i * 4202) << 16);

			CEntropyInfo entropyInfo;
			entropyInfo.Init(pTempBitmap, 10, nullptr);
			AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

			AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
			REQUIRE(avxAccumulation.accumulate(i) == 0);
		}

		std::vector<float> expectedRed(W * H, (static_cast<float>(303) + static_cast<float>(303 + 4200) + static_cast<float>(303 + 8400)) / 3.0f);
		std::vector<float> expectedGreen(W * H, (static_cast<float>(304) + static_cast<float>(304 + 4201) + static_cast<float>(304 + 8402)) / 3.0f);
		std::vector<float> expectedBlue(W * H, (static_cast<float>(305) + static_cast<float>(305 + 4202) + static_cast<float>(305 + 8404)) / 3.0f);
		REQUIRE(memcmp(pOut->m_Red.m_vPixels.data(), expectedRed.data(), expectedRed.size() * sizeof(float)) == 0);
		REQUIRE(memcmp(pOut->m_Green.m_vPixels.data(), expectedGreen.data(), expectedGreen.size() * sizeof(float)) == 0);
		REQUIRE(memcmp(pOut->m_Blue.m_vPixels.data(), expectedBlue.data(), expectedBlue.size() * sizeof(float)) == 0);
	}
}

TEST_CASE("AVX Accumulation MAXIMUM", "[AVX][Accumulation][Maximum]")
{
	CTaskInfo taskInfo;
	taskInfo.SetMethod(MBP_MAXIMUM, 0, 0);

	SECTION("One gray frame with identical values int16")
	{
		constexpr int W = 256 + 7;
		constexpr int H = 16 * 21 + 11;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pTempBitmap;
		pTempBitmap.Attach(new CGrayBitmapT<std::uint16_t>);
		REQUIRE(pTempBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.m_p);
		// Set to 127
		pGray->m_vPixels.assign(W * H, 4938);

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
		REQUIRE(avxAccumulation.accumulate(0) == 0);

		std::vector<float> expected(W * H, 4938.0f);
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}

	SECTION("Two gray frames int16")
	{
		constexpr int W = 256 + 7;
		constexpr int H = 32 + 11;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		for (int i = 0; i < 2; ++i)
		{
			CSmartPtr<CMemoryBitmap> pTempBitmap;
			pTempBitmap.Attach(new CGrayBitmapT<std::uint16_t>);
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.m_p);
			pGray->m_vPixels.assign(W * H, 100 + i * 3000);

			CEntropyInfo entropyInfo;
			entropyInfo.Init(pTempBitmap, 10, nullptr);
			AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

			AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
			REQUIRE(avxAccumulation.accumulate(i) == 0);
		}

		std::vector<float> expected(W * H, 3100.0f); // Maximum of 100 and 3100.
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}

	SECTION("Three gray frames int32")
	{
		constexpr int W = 16 * 18 + 3;
		constexpr int H = 16 * 7 + 6;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		for (int i = 0; i < 3; ++i)
		{
			CSmartPtr<CMemoryBitmap> pTempBitmap;
			pTempBitmap.Attach(new CGrayBitmapT<std::uint32_t>);
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pTempBitmap.m_p);
			pGray->m_vPixels.assign(W * H, (303 + i * 4200) << 16);

			CEntropyInfo entropyInfo;
			entropyInfo.Init(pTempBitmap, 10, nullptr);
			AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

			AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
			REQUIRE(avxAccumulation.accumulate(i) == 0);
		}

		std::vector<float> expected(W * H, static_cast<float>(303 + 2 * 4200));
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}

	SECTION("Four gray frames float")
	{
		constexpr int W = 16 * 18 + 8;
		constexpr int H = 16 * 27 + 13;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		for (int i = 0; i < 4; ++i)
		{
			CSmartPtr<CMemoryBitmap> pTempBitmap;
			pTempBitmap.Attach(new CGrayBitmapT<float>);
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<float>*>(pTempBitmap.m_p);
			pGray->m_vPixels.assign(W * H, 16000.0f + i * 17.35f);
			pGray->m_vPixels[15] = i == 1 ? 1.263e7f : 1e7f;

			CEntropyInfo entropyInfo;
			entropyInfo.Init(pTempBitmap, 10, nullptr);
			AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

			AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
			REQUIRE(avxAccumulation.accumulate(i) == 0);
		}

		std::vector<float> expected(W * H, 16052.05f);
		expected[15] = 1.263e7f;
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}

	SECTION("Three RGB frames int32")
	{
		constexpr int W = 16 * 69 + 2;
		constexpr int H = 16 * 73 + 15;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CColorBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CColorBitmapT<float>*>(pOutBitmap.m_p);

		for (int i = 0; i < 3; ++i)
		{
			CSmartPtr<CMemoryBitmap> pTempBitmap;
			pTempBitmap.Attach(new CColorBitmapT<std::uint32_t>);
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pColor = dynamic_cast<CColorBitmapT<std::uint32_t>*>(pTempBitmap.m_p);
			pColor->m_Red.m_vPixels.assign(W * H, (303 + i * 4200) << 16);
			pColor->m_Green.m_vPixels.assign(W * H, (304 + i * 4201) << 16);
			pColor->m_Blue.m_vPixels.assign(W * H, (305 + i * 4202) << 16);
			if (i == 2) {
				pColor->m_Green.m_vPixels[10025] = 10025 << 16;
				pColor->m_Blue.m_vPixels[W * H - 1] = 31234 << 16;
			}

			CEntropyInfo entropyInfo;
			entropyInfo.Init(pTempBitmap, 10, nullptr);
			AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

			AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
			REQUIRE(avxAccumulation.accumulate(i) == 0);
		}

		std::vector<float> expectedRed(W * H, static_cast<float>(303 + 8400));
		std::vector<float> expectedGreen(W * H, static_cast<float>(304 + 8402));
		expectedGreen[10025] = 10025.0f;
		std::vector<float> expectedBlue(W * H, static_cast<float>(305 + 8404));
		*expectedBlue.rbegin() = 31234;
		REQUIRE(memcmp(pOut->m_Red.m_vPixels.data(), expectedRed.data(), expectedRed.size() * sizeof(float)) == 0);
		REQUIRE(memcmp(pOut->m_Green.m_vPixels.data(), expectedGreen.data(), expectedGreen.size() * sizeof(float)) == 0);
		REQUIRE(memcmp(pOut->m_Blue.m_vPixels.data(), expectedBlue.data(), expectedBlue.size() * sizeof(float)) == 0);
	}
}
/*
TEST_CASE("AVX Accumulation ENTROPY", "[AVX][Accumulation][Entropy]")
{
	CTaskInfo taskInfo;
	taskInfo.SetMethod(MBP_ENTROPYAVERAGE, 0, 0);

	SECTION("One gray frame with identical values int16")
	{
		constexpr int W = 256 + 7;
		constexpr int H = 16 * 21 + 11;
		constexpr int windowSize = 10;
		constexpr int squareSize = 2 * windowSize + 1;

		CRect rect(0, 0, W, H); // left, top, right, bottom

		CSmartPtr<CMemoryBitmap> pTempBitmap;
		pTempBitmap.Attach(new CGrayBitmapT<std::uint16_t>);
		REQUIRE(pTempBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.m_p);
		pGray->m_vPixels.assign(W * H, 4938);

		CSmartPtr<CMemoryBitmap> pOutBitmap;
		pOutBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.m_p);

		CSmartPtr<CMemoryBitmap> pEntropyCoverage;
		pEntropyCoverage.Attach(new CGrayBitmapT<float>);
		REQUIRE(pEntropyCoverage->Init(W, H) == true);
		const int nSqX = 1 + (W - 1) / squareSize;
		const int nSqY = 1 + (H - 1) / squareSize;

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, windowSize, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, pEntropyCoverage);

		AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
		REQUIRE(avxAccumulation.accumulate(0) == 0);

		std::vector<float> expected(W * H, 0.0f);
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}
}
*/
BACKGROUNDCALIBRATIONMODE GetBackgroundCalibrationMode() { return BCM_RGB; }
BACKGROUNDCALIBRATIONINTERPOLATION GetBackgroundCalibrationInterpolation() { return BCI_RATIONAL; }
RGBBACKGROUNDCALIBRATIONMETHOD GetRGBBackgroundCalibrationMethod() { return RBCM_MIDDLE; }

bool CColorMedianFilterEngineT<unsigned int>::GetFilteredImage(class CMemoryBitmap**, int, class CDSSProgress*) { return true; }
