#include "stdafx.h"
#include "catch.h"
#include "dssrect.h"

#define UNIT_TESTS

#include "AvxAccumulateTest.h"
#include "BitmapBase.h"
#include "avx_avg.h"

#include "TaskInfo.h"
#include "EntropyInfo.h"
#include "ColorBitmap.h"
#include "MedianFilterEngine.h"


TEST_CASE("AVX Accumulation FASTAVERAGE", "[AVX][Accumulation][FastAverage]")
{
	CTaskInfo taskInfo;
	taskInfo.SetMethod(MBP_FASTAVERAGE, 0, 0);

	SECTION("One gray frame with identical values int16")
	{
		constexpr int W = 256 + 7;
		constexpr int H = 16 * 21 + 11;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.get());
		// Set to 127
		pGray->m_vPixels.assign(W * H, 127);

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

		for (int i = 0; i < 2; ++i)
		{
			std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.get());
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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CGrayBitmapT<float>> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

		for (int i = 0; i < 3; ++i)
		{
			std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<std::uint32_t>>();
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pTempBitmap.get());
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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

		for (int i = 0; i < 4; ++i)
		{
			std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<float>>();
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<float>*>(pTempBitmap.get());
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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CColorBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CColorBitmapT<float>*>(pOutBitmap.get());

		for (int i = 0; i < 3; ++i)
		{
			std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CColorBitmapT<std::uint32_t>>();
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pColor = dynamic_cast<CColorBitmapT<std::uint32_t>*>(pTempBitmap.get());
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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.get());
		// Set to 127
		pGray->m_vPixels.assign(W * H, 4938);

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

		for (int i = 0; i < 2; ++i)
		{
			std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pTempBitmap.get());
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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

		for (int i = 0; i < 3; ++i)
		{
			std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<std::uint32_t>>();
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pTempBitmap.get());
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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

		for (int i = 0; i < 4; ++i)
		{
			std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<float>>();
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pGray = dynamic_cast<CGrayBitmapT<float>*>(pTempBitmap.get());
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

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CColorBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CColorBitmapT<float>*>(pOutBitmap.get());

		for (int i = 0; i < 3; ++i)
		{
			std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CColorBitmapT<std::uint32_t>>();
			REQUIRE(pTempBitmap->Init(W, H) == true);
			auto* pColor = dynamic_cast<CColorBitmapT<std::uint32_t>*>(pTempBitmap.get());
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

TEST_CASE("AVX Accumulation ENTROPY", "[AVX][Accumulation][Entropy]")
{
	CTaskInfo taskInfo;
	taskInfo.SetMethod(MBP_ENTROPYAVERAGE, 0, 0);

	SECTION("One gray frame with identical values int32")
	{
		constexpr int W = 256 + 7;
		constexpr int H = 16 * 21 + 11;
		constexpr int windowSize = 10;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<std::uint32_t>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pTempBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = ((W + 5) * (H + 11) - i) << 16;

		std::shared_ptr<CMemoryBitmap> pOutBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pOutBitmap->Init(W, H) == true);
		const auto* pOut = dynamic_cast<CGrayBitmapT<float>*>(pOutBitmap.get());

		std::shared_ptr<CMemoryBitmap> pEntropyCoverage = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pEntropyCoverage->Init(W, H) == true);

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, windowSize, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, pEntropyCoverage.get());

		float* pRedEntropy = avxEntropy.getRedEntropyLayer();
		for (int i = 0; i < W * H; ++i)
			pRedEntropy[i] = static_cast<float>(i);

		AvxAccumulation avxAccumulation(rect, taskInfo, *pTempBitmap, *pOutBitmap, avxEntropy);
		REQUIRE(avxAccumulation.accumulate(0) == 0);

		const auto* pEntCov = dynamic_cast<CGrayBitmapT<float>*>(pEntropyCoverage.get());
		REQUIRE(memcmp(pEntCov->m_vPixels.data(), pRedEntropy, W * H * sizeof(float)) == 0);

		std::vector<float> expected(W * H);
		for (int i = 0; i < W * H; ++i)
			expected[i] = static_cast<float>(pGray->m_vPixels[i] >> 16) * pRedEntropy[i];
		REQUIRE(memcmp(pOut->m_vPixels.data(), expected.data(), expected.size() * sizeof(float)) == 0);
	}
}

BACKGROUNDCALIBRATIONMODE GetBackgroundCalibrationMode() { return BCM_RGB; }
BACKGROUNDCALIBRATIONINTERPOLATION GetBackgroundCalibrationInterpolation() { return BCI_RATIONAL; }
RGBBACKGROUNDCALIBRATIONMETHOD GetRGBBackgroundCalibrationMethod() { return RBCM_MIDDLE; }

//std::shared_ptr<CMemoryBitmap> CColorMedianFilterEngineT<unsigned int>::GetFilteredImage(int lFilterSize, ProgressBase* pProgress) const { return std::shared_ptr<CMemoryBitmap>{}; }
