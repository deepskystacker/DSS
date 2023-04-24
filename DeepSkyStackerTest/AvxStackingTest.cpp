#include "stdafx.h"
#include "catch.h"

#define UNIT_TESTS

#include "AvxAccumulateTest.h"
#include "AvxEntropyTest.h"
#include "avx.h"
#include "avx_median.h"
#include "GrayBitmap.h"
#include "EntropyInfo.h"
#include "TaskInfo.h"
#include "PixelTransform.h"
#include "avx_entropy.h"
#include "BackgroundCalibration.h"


TEST_CASE("AVX Stacking, no transform, no calib", "[AVX][Stacking][simple]")
{
	SECTION("Int16")
	{
		constexpr int W = 313;
		constexpr int H = 233;
		typedef std::uint16_t T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		pGray->m_vPixels.assign(W * H, 60003);

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		CPixelTransform pixTransform;
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.m_liRed.Initialize(0, 0, 1, 0, 0, 1); // This gives "in == out".

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		// Output should be identical to input.
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		auto* pOut = dynamic_cast<CGrayBitmapT<T>*>(pTempBitmap.get());
		REQUIRE(pOut != nullptr);
		REQUIRE(memcmp(pGray->m_vPixels.data(), pOut->m_vPixels.data(), W * H * sizeof(T)) == 0);
	}

	SECTION("Int32")
	{
		constexpr int W = 256 + 7;
		constexpr int H = 16 * 21 + 11;
		typedef std::uint32_t T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = (10000 + i / 16) << 16;

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		CPixelTransform pixTransform;
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.m_liRed.Initialize(0, 0, 1, 0, 0, 1); // This gives "in == out".

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		auto* pOut = dynamic_cast<CGrayBitmapT<T>*>(pTempBitmap.get());
		REQUIRE(pOut != nullptr);
		REQUIRE(pOut->m_vPixels[262] == (10016 << 16));
		std::vector<T> expected(W * H);
		for (int i = 0; i < W * H; ++i)
			expected[i] = (10000 + i / 16) << 16;
		REQUIRE(memcmp(expected.data(), pOut->m_vPixels.data(), W * H * sizeof(T)) == 0);
	}

	SECTION("Float")
	{
		constexpr int W = 149;
		constexpr int H = 229;
		typedef float T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 1732.498307f - i * i * 1e-6f;

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		CPixelTransform pixTransform;
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.SetMode(BCM_NONE, BCI_LINEAR, RBCM_MAXIMUM);

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		// Output should be identical to input.
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		auto* pOut = dynamic_cast<CGrayBitmapT<T>*>(pTempBitmap.get());
		REQUIRE(pOut != nullptr);
		REQUIRE(memcmp(pGray->m_vPixels.data(), pOut->m_vPixels.data(), W * H * sizeof(T)) == 0);
	}
}

TEST_CASE("AVX Stacking, transform, no calib", "[AVX][Stacking][transform]")
{
	SECTION("X/Y shift")
	{
		constexpr int W = 281;
		constexpr int H = 199;
		typedef float T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 2632.598002f - i * i * 1e-6f;

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		CPixelTransform pixTransform;
		pixTransform.SetShift(2.0, -3.0);
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.SetMode(BCM_NONE, BCI_LINEAR, RBCM_MAXIMUM);

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		auto* pOut = dynamic_cast<CGrayBitmapT<T>*>(pTempBitmap.get());
		REQUIRE(pOut != nullptr);
		std::vector<T> expected(W * H);
		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
			{
				const int yn = y - 3;
				const int xn = x + 2;
				const bool valid = yn >= 0 && yn < H && xn >= 0 && xn < W;
				if (valid)
					expected[yn * W + xn] = pGray->m_vPixels[y * W + x];
			}
		REQUIRE(memcmp(expected.data(), pOut->m_vPixels.data(), W * H * sizeof(T)) == 0);
	}

	SECTION("Bilinear constant shift")
	{
		constexpr int W = 383;
		constexpr int H = 463;
		typedef float T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 36032.598087f - i * i * 1e-6f;

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		CPixelTransform pixTransform;
		pixTransform.m_BilinearParameters.a0 = -3.5;
		pixTransform.m_BilinearParameters.b0 = 1.0;
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.SetMode(BCM_NONE, BCI_LINEAR, RBCM_MAXIMUM);

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		auto* pOut = dynamic_cast<CGrayBitmapT<T>*>(pTempBitmap.get());
		REQUIRE(pOut != nullptr);
		std::vector<T> expected(W * H);
		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
			{
				const int yn = y + 1;
				const float xf = x - 3.5f;
				const int xn = static_cast<int>(std::floor(xf));
				// a0 is -3.5, so 1/2 is added to (x-4), the other half is added to (x-3)
				if (yn >= 0 && yn <= (H - 1) && xf >= 0 && xf <= (W - 1))
				{
					expected[yn * W + xn] += 0.5f * pGray->m_vPixels[y * W + x];
					expected[yn * W + xn + 1] += 0.5f * pGray->m_vPixels[y * W + x];
				}
			}
		REQUIRE(memcmp(expected.data(), pOut->m_vPixels.data(), W * H * sizeof(T)) == 0);
	}

	SECTION("Bilinear x/y shift")
	{
		constexpr int W = 37;
		constexpr int H = 5;
		typedef float T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 106052.598087f - i * i * 4e-1f;

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		CPixelTransform pixTransform;
		pixTransform.m_BilinearParameters.a0 = -3.5; pixTransform.m_BilinearParameters.a2 = 0.4;  pixTransform.m_BilinearParameters.a3 = 0.03; // x += a2 * y + a3 * x * y
		pixTransform.m_BilinearParameters.b0 = 1.0; pixTransform.m_BilinearParameters.b1 = 0.1; // y += b1 * x
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.SetMode(BCM_NONE, BCI_LINEAR, RBCM_MAXIMUM);

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		auto* pOut = dynamic_cast<CGrayBitmapT<T>*>(pTempBitmap.get());
		REQUIRE(pOut != nullptr);
		std::vector<T> expected(W * H);
		const auto calcFrac = [&](const float xc, const float yc, const int xn, const int yn, const int x, const int y) -> void
		{
			const float xf = 1.0f - std::abs(xc - xn);
			const float yf = 1.0f - std::abs(yc - yn);
			if (xc >= 0 && xc <= (W - 1) && yc >= 0 && yc <= (H - 1)
				&& xn >= 0 && xn < W && yn >= 0 && yn < H)
			{
				expected[yn * W + xn] += xf * yf * pGray->m_vPixels[y * W + x];
			}
		};
		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
			{
				const float yc = y + 1 + 0.1f * x;
				const float xc = x - 3.5f + 0.4f * y + 0.03f * x * y;
				const int yn = static_cast<int>(std::floor(yc));
				const int xn = static_cast<int>(std::floor(xc));
				calcFrac(xc, yc, xn, yn, x, y);
				calcFrac(xc, yc, xn + 1, yn, x, y);
				calcFrac(xc, yc, xn, yn + 1, x, y);
				calcFrac(xc, yc, xn + 1, yn + 1, x, y);
			}
		for (int i = 0; i < W * H; ++i)
		{
			REQUIRE(pOut->m_vPixels[i] == Approx(expected[i]).epsilon(1e-5));
		}
	}
}

TEST_CASE("AVX Stacking, no transform, calib", "[AVX][Stacking][calib]")
{
	SECTION("Linear")
	{
		constexpr int W = 151;
		constexpr int H = 79;
		typedef float T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 146052.598087f - i * i * 1e-3f;

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		CPixelTransform pixTransform;
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.m_liRed.Initialize(0, -1, 0, 8.35, 1, 3); // => color * 2 + 3

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		auto* pOut = dynamic_cast<CGrayBitmapT<T>*>(pTempBitmap.get());
		REQUIRE(pOut != nullptr);
		std::vector<T> expected(W * H);
		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
				expected[y * W + x] = pGray->m_vPixels[y * W + x] < -1 ? 8.35f : pGray->m_vPixels[y * W + x] * 2.0f + 3.0f;
		REQUIRE(memcmp(expected.data(), pOut->m_vPixels.data(), W * H * sizeof(T)) == 0);
	}

	SECTION("Rational")
	{
		constexpr int W = 113;
		constexpr int H = 211;
		typedef float T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 106052.598087f - i * i * 1e-4f;

		const T median = pGray->m_vPixels[W * H / 2];

		CEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, nullptr);

		CPixelTransform pixTransform;
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.SetMode(BCM_RGB, BCI_RATIONAL, RBCM_MAXIMUM);
		backgroundCalib.m_riRed.Initialize(0, median, 106052.598087, 0, median * 0.84, 106052.598087);

//		printf("A: %f, B: %f, C: %f, Min: %f, Max: %f\n", backgroundCalib.m_riRed.getParameterA(), backgroundCalib.m_riRed.getParameterB(), backgroundCalib.m_riRed.getParameterC(), backgroundCalib.m_riRed.getParameterMin(), backgroundCalib.m_riRed.getParameterMax());

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		auto* pOut = dynamic_cast<CGrayBitmapT<T>*>(pTempBitmap.get());
		REQUIRE(pOut != nullptr);
		std::vector<T> expected(W * H);
		bool OK = true;
		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
			{
				const float d = backgroundCalib.m_riRed.getParameterB() * pGray->m_vPixels[y * W + x] + backgroundCalib.m_riRed.getParameterC();
				const float xa = backgroundCalib.m_riRed.getParameterA() + pGray->m_vPixels[y * W + x];
				expected[y * W + x] = std::max(std::min(d == 0.0f ? xa : (xa / d), backgroundCalib.m_riRed.getParameterMax()), backgroundCalib.m_riRed.getParameterMin());
				const float diff = expected[y * W + x] == 0.0f ? std::abs(expected[y * W + x] - pOut->m_vPixels[y * W + x]) : std::abs((expected[y * W + x] - pOut->m_vPixels[y * W + x]) / expected[y * W + x]);
				OK = OK && diff < 3e-4f;
			}
		REQUIRE(OK == true);
	}
}

TEST_CASE("AVX Stacking, Entropy", "[AVX][Stacking][Entropy]")
{
	SECTION("One image")
	{
		constexpr int W = 61;
		constexpr int H = 37;
		typedef float T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = 100.0f;

		std::shared_ptr<CMemoryBitmap> pEntropyCoverage = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pEntropyCoverage->Init(W, H) == true);

		TestEntropyInfo entropyInfo;
		entropyInfo.Init(pTempBitmap, 10, nullptr);
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, pEntropyCoverage.get());

		CPixelTransform pixTransform;
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		taskInfo.SetMethod(MBP_ENTROPYAVERAGE, 2, 5);
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.SetMode(BCM_NONE, BCI_LINEAR, RBCM_MAXIMUM);

		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);
		REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);

		for (int i = 0; i < 10; ++i)
			REQUIRE(avxEntropy.getRedEntropyLayer()[i] == Approx(1.0f).epsilon(1e-4f));
		REQUIRE(avxEntropy.getRedEntropyLayer()[10] == 1.0f);
		// Pixel 11 in first square
		float d0 = std::sqrt(10 * 10 + 1 * 1.0f);
		float d1 = std::sqrt(10 * 10 + 20 * 20.0f);
		float d2 = 3e5f;
		float n = 1.0f / d0 + 1.0f / d1 + 1.0f / d2;
		float e = (1.0f / d0 + 2.0f / d1 + 0.0f / d2) / n;
		REQUIRE(avxEntropy.getRedEntropyLayer()[11] == e);

		constexpr int ndx = 12 * W + 11; // x=11, y=12 => Square 0, neighbor squares are right and below.
		d0 = std::sqrt(1 + 2 * 2.0f);
		d1 = std::sqrt(2 * 2.0f + 20 * 20.0f);
		d2 = std::sqrt(19 * 19.0f + 1.0f);
		n = 1.0f / d0 + 1.0f / d1 + 1.0f / d2;
		e = (1.0f / d0 + 2.0f / d1 + 101.0f / d2) / n; // Square 0 -> entr = 1, right neighbor entr = 2, below entr = 101.
		REQUIRE(avxEntropy.getRedEntropyLayer()[ndx] == Approx(e).epsilon(1e-7f));
	}

	SECTION("Two images")
	{
		constexpr int W = 67;
		constexpr int H = 41;
		typedef std::uint32_t T;

		DSSRect rect(0, 0, W, H); // left, top, right, bottom

		std::shared_ptr<CMemoryBitmap> pTempBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pTempBitmap->Init(W, H) == true);

		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<T>>();
		REQUIRE(pBitmap->Init(W, H) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<T>*>(pBitmap.get());
		for (int i = 0; i < W * H; ++i)
			pGray->m_vPixels[i] = (9967 + i) << 16;

		std::shared_ptr<CMemoryBitmap> pEntropyCoverage = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pEntropyCoverage->Init(W, H) == true);

		CPixelTransform pixTransform;
		CTaskInfo taskInfo; // Determines if method is ENTROPY or not.
		taskInfo.SetMethod(MBP_ENTROPYAVERAGE, 2, 5);
		CBackgroundCalibration backgroundCalib;
		backgroundCalib.SetMode(BCM_NONE, BCI_LINEAR, RBCM_MAXIMUM);

		TestEntropyInfo entropyInfo;
		AvxEntropy avxEntropy(*pTempBitmap, entropyInfo, pEntropyCoverage.get());
		AvxStacking avxStacking(0, H, *pBitmap, *pTempBitmap, rect, avxEntropy);

		for (int img = 0; img < 2; ++img)
		{
			// Img 0, entropies are: 1, 2, 3, 4 / 101, 102, 103, 104.
			// Img 1, entropies are: 10, 11, 12, 13 / 20, 21, 22, 23.
			entropyInfo.Init(pTempBitmap, img == 0 ? 10 : 9, nullptr); // Note: entropyInfo always uses window size 10 internally. Here it's used to distinguish img 0 and img 1.
			REQUIRE(avxStacking.stack(pixTransform, taskInfo, backgroundCalib, 1) == 0);
		}

		for (int i = 0; i < 10; ++i)
			REQUIRE(avxEntropy.getRedEntropyLayer()[i] == Approx(11.0f).epsilon(1e-4f));
		REQUIRE(avxEntropy.getRedEntropyLayer()[10] == Approx(11.0f).epsilon(3e-5f));
		// Pixel 11 in first square
		float d0 = std::sqrt(10 * 10 + 1 * 1.0f);
		float d1 = std::sqrt(10 * 10 + 20 * 20.0f);
		float d2 = 3e5f;
		float n = 1.0f / d0 + 1.0f / d1 + 1.0f / d2;
		float e = ((1.0f + 10.0f) / d0 + (2.0f + 11.0f) / d1 + (0.0f) / d2) / n;
		REQUIRE(avxEntropy.getRedEntropyLayer()[11] == Approx(e).epsilon(1e-7f));

		constexpr int ndx = 12 * W + 11; // x=11, y=12 => Square 0, neighbor squares are right and below.
		d0 = std::sqrt(1 + 2 * 2.0f);
		d1 = std::sqrt(2 * 2.0f + 20 * 20.0f);
		d2 = std::sqrt(19 * 19.0f + 1.0f);
		n = 1.0f / d0 + 1.0f / d1 + 1.0f / d2;
		e = ((1.0f + 10.0f) / d0 + (2.0f + 11.0f) / d1 + (101.0f + 20.0f) / d2) / n; // Square 0 -> entr = 1+10, right neighbor entr = 2+11, below entr = 101+20.
		REQUIRE(avxEntropy.getRedEntropyLayer()[ndx] == Approx(e).epsilon(1e-7f));
	}
}

CBackgroundCalibration::CBackgroundCalibration() :
	m_bInitOk{ false },
	m_fMultiplier{ 1.0 },
	m_BackgroundCalibrationMode{ BCM_RGB },
	m_BackgroundInterpolation{ BCI_LINEAR },
	m_RGBBackgroundMethod{ RBCM_MAXIMUM }
{}
