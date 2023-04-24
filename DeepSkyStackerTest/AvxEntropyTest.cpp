#include "stdafx.h"
#include "catch.h"
#include "avx_entropy.h"
#include "ColorBitmap.h"
#include "Multitask.h"
#include "AvxEntropyTest.h"

std::tuple<float, float> calcEntropy(const std::vector<int>& incidences)
{
	const float nPixels = static_cast<float>(incidences.size());

	const float avx = std::accumulate(incidences.cbegin(), incidences.cend(), 0.0f, [lnN = std::log(nPixels)](const float accu, const float val) {
		return accu + val * (lnN - std::log(val));
	}) / (nPixels * std::log(2.0f));

	const float exact = std::accumulate(incidences.cbegin(), incidences.cend(), 0.0f, [nPixels](const float accu, const float val) {
		return accu - (val / nPixels) * log(val / nPixels) / log(2.0f);
	});

	return { avx, exact };
}

float roundSig(const float value, const float mult) {
	return std::round(value * mult);
}

float truncSig(const float value, const float mult) {
	return static_cast<int>(value * mult);
}

TEST_CASE("AVX Entropy", "[AVX][Entropy]")
{
	SECTION("Entropy is zero if all values equal")
	{
		constexpr int windowSize = 4;
		constexpr int squareSize = 2 * windowSize + 1;
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set to 100
		pGray->m_vPixels.assign(256 * 32, 100);

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		TestEntropyInfo entropyInfo;
		entropyInfo.Init(pBitmap, windowSize, nullptr);
		AvxEntropy avxEntropy(*pBitmap, entropyInfo, nullptr);
		avxEntropy.calcEntropies(squareSize, nSqX, nSqY, redEnt, greenEnt, blueEnt);

		std::vector<float> expectedRed(redEnt.size(), 0);
		REQUIRE(memcmp(redEnt.data(), expectedRed.data(), redEnt.size() * sizeof(float)) == 0);
	}

	SECTION("All values equal except two")
	{
		constexpr int windowSize = 10;
		constexpr int squareSize = 2 * windowSize + 1;
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		REQUIRE(pBitmap->Init(256, 64) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set to 100
		pGray->m_vPixels.assign(256 * 64, 555);
		pGray->m_vPixels[2 * windowSize * 256 + 2 * windowSize] = 7000;
		pGray->m_vPixels[2 * windowSize * 256 + 4 * windowSize + 2] = 18000;

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		TestEntropyInfo entropyInfo;
		entropyInfo.Init(pBitmap, windowSize, nullptr);
		AvxEntropy avxEntropy(*pBitmap, entropyInfo, nullptr);
		avxEntropy.calcEntropies(squareSize, nSqX, nSqY, redEnt, greenEnt, blueEnt);

		std::vector<int> incidences(squareSize * squareSize, squareSize * squareSize - 1); // All pixels except one are equal.
		*incidences.rbegin() = 1; // One pixel is different.
		const auto [entropyAvx, entropyExact] = calcEntropy(incidences);

		REQUIRE(roundSig(redEnt[0], 1e5f) == roundSig(entropyAvx, 1e5f));
		REQUIRE(truncSig(entropyAvx, 1e4f) == truncSig(entropyExact, 1e4f)); // Some decimal digits need to be identical.

		std::vector<float> expectedRed(redEnt.size(), 0);
		expectedRed[0] = redEnt[0];
		expectedRed[2] = redEnt[0];
		REQUIRE(memcmp(redEnt.data(), expectedRed.data(), redEnt.size() * sizeof(float)) == 0);
		REQUIRE(memcmp(redEnt.data(), greenEnt.data(), redEnt.size() * sizeof(float)) == 0);
		REQUIRE(memcmp(redEnt.data(), blueEnt.data(), redEnt.size() * sizeof(float)) == 0);
	}

	SECTION("All values different")
	{
		constexpr int windowSize = 10;
		constexpr int squareSize = 2 * windowSize + 1;
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
		REQUIRE(pBitmap->Init(24, 22) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set values
		std::uint16_t v = 3;
		std::for_each(pGray->m_vPixels.begin(), pGray->m_vPixels.end(), [&v](auto& elem) { elem = v++; });

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		TestEntropyInfo entropyInfo;
		entropyInfo.Init(pBitmap, windowSize, nullptr);
		AvxEntropy avxEntropy(*pBitmap, entropyInfo, nullptr);
		avxEntropy.calcEntropies(squareSize, nSqX, nSqY, redEnt, greenEnt, blueEnt);

		const auto compare = [&](const std::vector<int> incidences, const float calculatedEntropy) {
			const auto [entropyAvx, entropyExact] = calcEntropy(incidences);
			REQUIRE(roundSig(calculatedEntropy, 1e4f) == roundSig(entropyAvx, 1e4f));
			REQUIRE(roundSig(entropyAvx, 1e4f) == roundSig(entropyExact, 1e4f)); // Some decimal digits need to be identical.
		};

		compare(std::vector<int>(squareSize * squareSize, 1), redEnt[0]); // First square.
		compare(std::vector<int>((24 - squareSize) * squareSize, 1), redEnt[1]); // Second "square".
		compare(std::vector<int>(squareSize * (22 - squareSize), 1), redEnt[2]); // Third "square".
		compare(std::vector<int>((24 - squareSize) * (22 - squareSize), 1), redEnt[3]); // Fourth "square".
	}

	SECTION("Most values different (float)")
	{
		constexpr int windowSize = 10;
		constexpr int squareSize = 2 * windowSize + 1;
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<float>>();
		REQUIRE(pBitmap->Init(40, 37) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<float>*>(pBitmap.get());
		// Set values
		float v = 5384.6f;
		std::for_each(pGray->m_vPixels.begin(), pGray->m_vPixels.end(), [&v](auto& elem) { elem = v++; });
		pGray->m_vPixels[1] = pGray->m_vPixels[0];

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		TestEntropyInfo entropyInfo;
		entropyInfo.Init(pBitmap, windowSize, nullptr);
		AvxEntropy avxEntropy(*pBitmap, entropyInfo, nullptr);
		avxEntropy.calcEntropies(squareSize, nSqX, nSqY, redEnt, greenEnt, blueEnt);

		const auto compare = [&](const std::vector<int> incidences, const float calculatedEntropy) -> int {
			const auto [entropyAvx, entropyExact] = calcEntropy(incidences);
			return static_cast<int>(
				std::abs(roundSig(calculatedEntropy, 1e3f) - roundSig(entropyAvx, 1e3f))
				+ std::abs(roundSig(entropyAvx, 1e3f) - roundSig(entropyExact, 1e3f))
			);
		};

		std::vector<int> inci(squareSize * squareSize, 1);
		inci[0] = inci[1] = 2;
		REQUIRE(compare(inci, redEnt[0]) == 0); // First square.
		REQUIRE(compare(std::vector<int>((40 - squareSize) * squareSize, 1), redEnt[1]) == 0); // Second "square".
		REQUIRE(compare(std::vector<int>(squareSize * (37 - squareSize), 1), redEnt[2]) == 0); // Third "square".
		REQUIRE(compare(std::vector<int>((40 - squareSize) * (37 - squareSize), 1), redEnt[3]) == 0); // Fourth "square".
	}

	SECTION("Most values different (int32)")
	{
		constexpr int windowSize = 10;
		constexpr int squareSize = 2 * windowSize + 1;
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint32_t>>();
		REQUIRE(pBitmap->Init(40, 37) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pBitmap.get());
		// Set values
		std::uint32_t v = 963 << 16;
		std::for_each(pGray->m_vPixels.begin(), pGray->m_vPixels.end(), [&v](auto& elem) { elem = v; v += (1 << 16); });
		pGray->m_vPixels[21] = pGray->m_vPixels[22];

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		TestEntropyInfo entropyInfo;
		entropyInfo.Init(pBitmap, windowSize, nullptr);
		AvxEntropy avxEntropy(*pBitmap, entropyInfo, nullptr);
		avxEntropy.calcEntropies(squareSize, nSqX, nSqY, redEnt, greenEnt, blueEnt);

		const auto compare = [&](const std::vector<int> incidences, const float calculatedEntropy) -> int {
			const auto [entropyAvx, entropyExact] = calcEntropy(incidences);
			return static_cast<int>(
				std::abs(roundSig(calculatedEntropy, 1e3f) - roundSig(entropyAvx, 1e3f))
				+ std::abs(roundSig(entropyAvx, 1e3f) - roundSig(entropyExact, 1e3f))
			);
		};

		REQUIRE(compare(std::vector<int>(squareSize * squareSize, 1), redEnt[0]) == 0); // First square.
		std::vector<int> inci((40 - squareSize) * squareSize, 1);
		inci[0] = inci[1] = 2;
		REQUIRE(compare(inci, redEnt[1]) == 0); // Second "square".
		REQUIRE(compare(std::vector<int>(squareSize * (37 - squareSize), 1), redEnt[2]) == 0); // Third "square".
		REQUIRE(compare(std::vector<int>((40 - squareSize) * (37 - squareSize), 1), redEnt[3]) == 0); // Fourth "square".
	}

	SECTION("Most values different RGB")
	{
		constexpr int windowSize = 10;
		constexpr int squareSize = 2 * windowSize + 1;
		std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CColorBitmapT<std::uint16_t>>();
		REQUIRE(pBitmap->Init(24, 22) == true);

		auto* pGray = dynamic_cast<CColorBitmapT<std::uint16_t>*>(pBitmap.get());
		// Set values
		std::uint16_t v = 3;
		std::for_each(pGray->m_Red.m_vPixels.begin(), pGray->m_Red.m_vPixels.end(), [&v](auto& elem) { elem = v++; });
		std::for_each(pGray->m_Green.m_vPixels.begin(), pGray->m_Green.m_vPixels.end(), [&v](auto& elem) { elem = v++; });
		pGray->m_Green.m_vPixels[3] = pGray->m_Green.m_vPixels[20];
		std::for_each(pGray->m_Blue.m_vPixels.begin(), pGray->m_Blue.m_vPixels.end(), [&v](auto& elem) { elem = v++; });

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		TestEntropyInfo entropyInfo;
		entropyInfo.Init(pBitmap, windowSize, nullptr);
		AvxEntropy avxEntropy(*pBitmap, entropyInfo, nullptr);
		avxEntropy.calcEntropies(squareSize, nSqX, nSqY, redEnt, greenEnt, blueEnt);

		const auto compare = [&](const std::vector<int> incidences, const float calculatedEntropy) {
			const auto [entropyAvx, entropyExact] = calcEntropy(incidences);
			REQUIRE(roundSig(calculatedEntropy, 1e4f) == roundSig(entropyAvx, 1e4f));
			REQUIRE(roundSig(entropyAvx, 1e4f) == roundSig(entropyExact, 1e4f)); // Some decimal digits need to be identical.
		};

		compare(std::vector<int>(squareSize * squareSize, 1), redEnt[0]); // First square.
		compare(std::vector<int>((24 - squareSize) * squareSize, 1), redEnt[1]); // Second "square".
		compare(std::vector<int>(squareSize * (22 - squareSize), 1), redEnt[2]); // Third "square".
		compare(std::vector<int>((24 - squareSize) * (22 - squareSize), 1), redEnt[3]); // Fourth "square".

		std::vector<int> inci(squareSize * squareSize, 1);
		inci[3] = inci[20] = 2;
		compare(inci, greenEnt[0]);
		compare(std::vector<int>((24 - squareSize) * squareSize, 1), greenEnt[1]);
		compare(std::vector<int>(squareSize * (22 - squareSize), 1), greenEnt[2]);
		compare(std::vector<int>((24 - squareSize) * (22 - squareSize), 1), greenEnt[3]);

		compare(std::vector<int>(squareSize * squareSize, 1), blueEnt[0]);
		compare(std::vector<int>((24 - squareSize) * squareSize, 1), blueEnt[1]);
		compare(std::vector<int>(squareSize * (22 - squareSize), 1), blueEnt[2]);
		compare(std::vector<int>((24 - squareSize) * (22 - squareSize), 1), blueEnt[3]);
	}
}

int CMultitask::GetNrProcessors(bool) { return 1; }

 void TestEntropyInfo::InitSquareEntropies()
 {
 	const int yoff = m_lWindowSize == 10 ? 100 : 10;
 	const int xoff = m_lWindowSize == 10 ? 1 : 10;
 	m_lWindowSize = 10;
 	const int lSquareSize = m_lWindowSize * 2 + 1;
 
 	m_lNrSquaresX = m_pBitmap->Width() / lSquareSize;
 	m_lNrSquaresY = m_pBitmap->Height() / lSquareSize;
 
 	if (m_pBitmap->RealWidth() % lSquareSize)
 		m_lNrSquaresX++;
 	if (m_pBitmap->RealHeight() % lSquareSize)
 		m_lNrSquaresY++;
 
 	m_vRedEntropies.resize(m_lNrSquaresX * m_lNrSquaresY);
 	m_vGreenEntropies.resize(m_lNrSquaresX * m_lNrSquaresY);
 	m_vBlueEntropies.resize(m_lNrSquaresX * m_lNrSquaresY);
 
 	for (int y = 0; y < m_lNrSquaresY; ++y)
 		for (int x = 0; x < m_lNrSquaresX; ++x)
 			m_vRedEntropies[y * m_lNrSquaresX + x] = m_vGreenEntropies[y * m_lNrSquaresX + x] = m_vBlueEntropies[y * m_lNrSquaresX + x] = static_cast<float>(y * yoff + x + xoff);
 }
