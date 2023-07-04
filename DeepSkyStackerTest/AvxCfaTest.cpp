#include "stdafx.h"
#include "catch.h"
#include "avx_cfa.h"
#include "GrayBitmap.h"
#include "Multitask.h"
#include "MultiBitmap.h"
#include "MedianFilterEngine.h"

TEST_CASE("AVX CFA", "[AVX][CFA]")
{
	const auto compare = [](const std::uint16_t* pColor, const std::initializer_list<std::uint16_t>& data) -> int
	{
		const int retval = memcmp(pColor, data.begin(), data.size() * sizeof(std::uint16_t));
		if (retval != 0)
		{
			for (int n = 0; n < data.size(); ++n)
			{
				printf("first[%i] = %i, second[%i] = %i\n", n, pColor[n], n, *(data.begin() + n));
			}
		}
		return retval;
	};

	const auto avg2 = [](const std::initializer_list<std::uint16_t>& data) -> std::uint16_t
	{
		constexpr std::uint16_t one = 1;
		constexpr std::uint16_t two = 2;
		return (one + std::accumulate(data.begin(), data.end(), std::uint16_t{ 0 })) / two;
	};

	const auto emulate_simd_avg = [&avg2](const std::initializer_list<std::uint16_t>& data) -> std::uint16_t
	{
		if (data.size() == 2)
			return avg2(data);
		if (data.size() == 4)
			return avg2({ avg2({ *data.begin(), *(data.begin() + 1) }), avg2({ *(data.begin() + 2), *(data.begin() + 3) }) });
		else
			throw "Only 2 or 4 arguments valid.";
	};

	// Just to indicate that the color-component of a RGB pixel matchen the bayer pattern color at that position.
	// E.g. the red color of a RGB pixel in a RG-line at positions 0, 2, 4, ...
	const auto pixel_and_pattern_same_color = [](const std::uint16_t val) { return val; };

	std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<std::uint16_t>>();
	AvxCfaProcessing avxCfaProcessing(0, 0, *pBitmap);

	SECTION("Init initializes the object and sets nrVectorsPerLine properly")
	{
		const bool b = pBitmap->Init(8, 1);
		REQUIRE(b == true);
		avxCfaProcessing.init(0, 1);
		REQUIRE(avxCfaProcessing.nrVectorsPerLine() == 1);
	}

	SECTION("Init sets nrVectorsPerLine to 2 for width 33")
	{
		const bool b = pBitmap->Init(33, 1);
		REQUIRE(b == true);
		avxCfaProcessing.init(0, 1);
		REQUIRE(avxCfaProcessing.nrVectorsPerLine() == 2);
	}

	SECTION("Interpolate returns 1 if pixelsizemultiplier is 2")
	{
		avxCfaProcessing.init(0, 1);
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 2) == 1);
	}

	SECTION("Interpolate fails if CFA type is NONE")
	{
		pBitmap->Init(64, 8);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		REQUIRE(pGray != nullptr);
		pGray->UseBilinear(true);
		pGray->SetCFAType(CFATYPE_NONE);
		memset(pGray->m_vPixels.data(), 0, sizeof(std::uint16_t) * 64 * 8);
		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 1) != 0);
	}

	SECTION("Interpolate fails if CFA type is BGGR")
	{
		pBitmap->Init(64, 8);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		REQUIRE(pGray != nullptr);
		pGray->UseBilinear(true);
		pGray->SetCFAType(CFATYPE_BGGR);
		memset(pGray->m_vPixels.data(), 0, sizeof(std::uint16_t) * 64 * 8);
		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 1) != 0);
	}

	SECTION("Interpolate fails if CFA type is GRBG")
	{
		pBitmap->Init(64, 8);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		REQUIRE(pGray != nullptr);
		pGray->UseBilinear(true);
		pGray->SetCFAType(CFATYPE_GRBG);
		memset(pGray->m_vPixels.data(), 0, sizeof(std::uint16_t) * 64 * 8);
		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 1) != 0);
	}

	SECTION("Interpolate fails if CFA transform is superpixel although CFA type is RGGB")
	{
		pBitmap->Init(64, 8);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		REQUIRE(pGray != nullptr);
		pGray->UseSuperPixels(true);
		pGray->SetCFAType(CFATYPE_RGGB);
		memset(pGray->m_vPixels.data(), 0, sizeof(std::uint16_t) * 64 * 8);
		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 1) != 0);
	}

	SECTION("Interpolate fails if CFA transform is superpixel although CFA type is GBRG")
	{
		pBitmap->Init(64, 8);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		REQUIRE(pGray != nullptr);
		pGray->UseSuperPixels(true);
		pGray->SetCFAType(CFATYPE_GBRG);
		memset(pGray->m_vPixels.data(), 0, sizeof(std::uint16_t) * 64 * 8);
		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 1) != 0);
	}

	SECTION("Interpolate fails if CFA transform is rawbayer although CFA type is RGGB")
	{
		pBitmap->Init(64, 8);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		REQUIRE(pGray != nullptr);
		pGray->UseRawBayer(true);
		pGray->SetCFAType(CFATYPE_RGGB);
		memset(pGray->m_vPixels.data(), 0, sizeof(std::uint16_t) * 64 * 8);
		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 1) != 0);
	}

	SECTION("Interpolate fails if CFA transform is AHD although CFA type is RGGB")
	{
		pBitmap->Init(64, 8);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		REQUIRE(pGray != nullptr);
		pGray->UseAHD(true);
		pGray->SetCFAType(CFATYPE_RGGB);
		memset(pGray->m_vPixels.data(), 0, sizeof(std::uint16_t) * 64 * 8);
		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 1) != 0);
	}

	SECTION("Interpolate zeros gives zeros")
	{
		const bool b = pBitmap->Init(64, 8);
		REQUIRE(b == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		pGray->UseBilinear(true);
		pGray->SetCFAType(CFATYPE_RGGB);
		memset(pGray->m_vPixels.data(), 0, sizeof(std::uint16_t) * 64 * 8);
		
		avxCfaProcessing.init(0, 8);
		const int r = avxCfaProcessing.interpolate(0, 8, 1);
		REQUIRE(r == 0);

		std::uint16_t d[64] = { 0 };
		REQUIRE(memcmp(avxCfaProcessing.redCfaLine<std::uint16_t>(0), d, 128) == 0);
		REQUIRE(memcmp(avxCfaProcessing.greenCfaLine<std::uint16_t>(0), d, 128) == 0);
		REQUIRE(memcmp(avxCfaProcessing.blueCfaLine<std::uint16_t>(0), d, 128) == 0);
	}

	SECTION("Interpolate a RG line of a RGGB pattern")
	{
		REQUIRE(pBitmap->Init(64, 8) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		pGray->UseBilinear(true);
		pGray->SetCFAType(CFATYPE_RGGB);
		std::uint16_t v = 0;
		for (std::uint16_t& x : pGray->m_vPixels) {
			x = v++;
		}

		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 8, 1) == 0);

		REQUIRE(compare(avxCfaProcessing.redCfaLine<std::uint16_t>(2), { 128, 129, 130, 131, 132, 133 }) == 0);
		REQUIRE(compare(avxCfaProcessing.greenCfaLine<std::uint16_t>(2), { emulate_simd_avg({ 129, 0, 64, 192 }), 129, 130, 131, 132, 133 }) == 0);
		REQUIRE(compare(avxCfaProcessing.blueCfaLine<std::uint16_t>(2), { emulate_simd_avg({ 65, 0, 193, 0 }), 129, 130, 131, 132, 133 }) == 0);
	}

	SECTION("Interpolate a RG line of a GBRG pattern")
	{
		REQUIRE(pBitmap->Init(64, 8) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		pGray->UseBilinear(true);
		pGray->SetCFAType(CFATYPE_GBRG);
		std::uint16_t v = 0;
		for (std::uint16_t& x : pGray->m_vPixels) {
			x = v++;
		}

		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 8, 1) == 0);

		REQUIRE(compare(avxCfaProcessing.redCfaLine<std::uint16_t>(1), { 64, 65, 66, 67, 68, 69 }) == 0);
		REQUIRE(compare(avxCfaProcessing.greenCfaLine<std::uint16_t>(1), { emulate_simd_avg({ 65, 0, 0, 128 }), 65, 66, 67, 68, 69 }) == 0);
		REQUIRE(compare(avxCfaProcessing.blueCfaLine<std::uint16_t>(1), { emulate_simd_avg({ 1, 0, 129, 0 }), 65, 66, 67, 68, 69 }) == 0);
	}

	SECTION("Interpolate a GB line of a RGGB pattern")
	{
		REQUIRE(pBitmap->Init(64, 8) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		pGray->UseBilinear(true);
		pGray->SetCFAType(CFATYPE_RGGB);
		std::uint16_t v = 0;
		for (std::uint16_t& x : pGray->m_vPixels) {
			x = v++;
		}

		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 8, 1) == 0);

		REQUIRE(compare(avxCfaProcessing.redCfaLine<std::uint16_t>(3), { emulate_simd_avg({ 128, 256 }), 193, 194, 195, 196, 197}) == 0);
		REQUIRE(compare(avxCfaProcessing.greenCfaLine<std::uint16_t>(3), { 192, 193, 194, 195, 196, 197 }) == 0);
		REQUIRE(compare(avxCfaProcessing.blueCfaLine<std::uint16_t>(3), { emulate_simd_avg({ 0, 193}), 193, emulate_simd_avg({ 193, 195 }), 195, 196, 197 }) == 0);
	}

	SECTION("Interpolate GB line of a GBRG pattern")
	{
		REQUIRE(pBitmap->Init(64, 8) == true);
		auto* pGray = dynamic_cast<CGrayBitmapT<std::uint16_t>*>(pBitmap.get());
		pGray->UseBilinear(true);
		pGray->SetCFAType(CFATYPE_GBRG);
		std::uint16_t v = 0;
		for (std::uint16_t& x : pGray->m_vPixels) {
			x = v++;
		}
		pGray->m_vPixels[194] = 3; pGray->m_vPixels[195] = 100; pGray->m_vPixels[196] = 220;
		pGray->m_vPixels[258] = 10; pGray->m_vPixels[259] = 21; pGray->m_vPixels[260] = 41;
		pGray->m_vPixels[322] = 17; pGray->m_vPixels[323] = 29; pGray->m_vPixels[324] = 111;

		avxCfaProcessing.init(0, 8);
		REQUIRE(avxCfaProcessing.interpolate(0, 8, 1) == 0);

		REQUIRE(compare(avxCfaProcessing.redCfaLine<std::uint16_t>(4), { 256, 134, emulate_simd_avg({ 3, 17 }), emulate_simd_avg({ 3, 220, 17, 111 }), 166, 214 }) == 0);
		REQUIRE(compare(avxCfaProcessing.greenCfaLine<std::uint16_t>(4), { 256, 195, pixel_and_pattern_same_color(10), emulate_simd_avg({ 10, 41, 100, 29 }), 41, 207 }) == 0);
		REQUIRE(compare(avxCfaProcessing.blueCfaLine<std::uint16_t>(4), { 129, 257, emulate_simd_avg({ 257, 21 }), pixel_and_pattern_same_color(21), 141, 261 }) == 0);
	}
}




bool CMultitask::GetUseSimd() { return true; }
int CMultitask::GetNrCurrentOmpThreads() { return 1; } // Placeholder!!!

void CMultiBitmap::removeTempFiles() {}
void CMultiBitmap::SetBitmapModel(const CMemoryBitmap*) {}
bool CMultiBitmap::AddBitmap(CMemoryBitmap*, ProgressBase*) { return true; }
std::shared_ptr<CMemoryBitmap> CMultiBitmap::GetResult(ProgressBase*) { return std::shared_ptr<CMemoryBitmap>{}; }

//void CYMGToRGB(double, double, double, double, double&, double&, double&) {}
std::shared_ptr<CMemoryBitmap> CreateBitmap(const CBitmapCharacteristics&) { return std::shared_ptr<CMemoryBitmap>{}; }
// std::shared_ptr<CMemoryBitmap> CGrayMedianFilterEngineT<unsigned short>::GetFilteredImage(int, class ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// 
// std::shared_ptr<CMemoryBitmap> CColorMedianFilterEngineT<unsigned short>::GetFilteredImage(int, class ProgressBase*) const { return std::shared_ptr<CMemoryBitmap>{}; }
// 
// void CGrayBitmapT<unsigned short>::RemoveHotPixels(class ProgressBase*) {}
