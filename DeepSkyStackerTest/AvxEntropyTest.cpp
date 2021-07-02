#include "stdafx.h"
#include "catch.h"
#include "../DeepSkyStacker/avx_entropy.h"

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
		CSmartPtr<CMemoryBitmap> pBitmap;
		pBitmap.Attach(new CGrayBitmapT<std::uint16_t>);
		REQUIRE(pBitmap->Init(256, 32) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.m_p);
		// Set to 100
		pGray->m_vPixels.assign(256 * 32, 100);

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		CEntropyInfo entropyInfo;
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
		CSmartPtr<CMemoryBitmap> pBitmap;
		pBitmap.Attach(new CGrayBitmapT<std::uint16_t>);
		REQUIRE(pBitmap->Init(256, 64) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.m_p);
		// Set to 100
		pGray->m_vPixels.assign(256 * 64, 555);
		pGray->m_vPixels[2 * windowSize * 256 + 2 * windowSize] = 7000;
		pGray->m_vPixels[2 * windowSize * 256 + 4 * windowSize + 2] = 18000;

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		CEntropyInfo entropyInfo;
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
		CSmartPtr<CMemoryBitmap> pBitmap;
		pBitmap.Attach(new CGrayBitmapT<std::uint16_t>);
		REQUIRE(pBitmap->Init(24, 22) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.m_p);
		// Set values
		std::uint16_t v = 3;
		std::for_each(pGray->m_vPixels.begin(), pGray->m_vPixels.end(), [&v](auto& elem) { elem = v++; });

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		CEntropyInfo entropyInfo;
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
		CSmartPtr<CMemoryBitmap> pBitmap;
		pBitmap.Attach(new CGrayBitmapT<float>);
		REQUIRE(pBitmap->Init(40, 37) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<float>*>(pBitmap.m_p);
		// Set values
		float v = 5384.6f;
		std::for_each(pGray->m_vPixels.begin(), pGray->m_vPixels.end(), [&v](auto& elem) { elem = v++; });
		pGray->m_vPixels[1] = pGray->m_vPixels[0];

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		CEntropyInfo entropyInfo;
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
		CSmartPtr<CMemoryBitmap> pBitmap;
		pBitmap.Attach(new CGrayBitmapT<std::uint32_t>);
		REQUIRE(pBitmap->Init(40, 37) == true);

		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint32_t>*>(pBitmap.m_p);
		// Set values
		std::uint32_t v = 963 << 16;
		std::for_each(pGray->m_vPixels.begin(), pGray->m_vPixels.end(), [&v](auto& elem) { elem = v; v += (1 << 16); });
		pGray->m_vPixels[21] = pGray->m_vPixels[22];

		const int nSqX = 1 + (pBitmap->Width() - 1) / squareSize;
		const int nSqY = 1 + (pBitmap->Height() - 1) / squareSize;
		std::vector<float> redEnt(nSqX * nSqY), greenEnt(nSqX * nSqY), blueEnt(nSqX * nSqY);

		CEntropyInfo entropyInfo;
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
}

int CMultitask::GetNrProcessors(bool) { return 1; }
void CEntropyInfo::InitSquareEntropies() {}
