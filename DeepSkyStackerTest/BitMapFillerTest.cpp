#include "stdafx.h"
#include "catch.h"
#include "../DeepSkyStacker/BitMapFiller.h"
#include <immintrin.h>

template <size_t SZ>
void be2le(std::uint16_t (&out)[SZ], const std::uint16_t* pIn)
{
	for (size_t n = 0; n < SZ; ++n)
		out[n] = _load_be_u16(pIn + n);
}

// ------------------
// Gray
// ------------------

TEST_CASE("BitMapFiller gray", "[BitMap][BitMapFiller][gray]")
{
	CSmartPtr<CMemoryBitmap> pBitmap;
	pBitmap.Attach(new CGrayBitmapT<WORD>);
	auto filler = std::make_unique<BitMapFiller>(pBitmap, nullptr); // BitmapFillerInterface::makeBitmapFiller(pBitmap, nullptr);
	filler->setGrey(true);

	SECTION("Write 1 line 8 bps")
	{
		filler->setMaxColors(255);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint8_t inputData[8] = { 19, 45, 243, 65, 200, 110, 99, 225 };
		pBitmap->Init(8, 1);

		filler->Write(inputData, 1, 8);

		auto* pGray = dynamic_cast<CGrayBitmapT<WORD>*>(pBitmap.m_p);
		WORD data[8] = { 19 * 256, 45 * 256, 243 * 256, 65 * 256, 200 * 256, 110 * 256, 99 * 256, 225 * 256 };
		REQUIRE(memcmp(pGray->m_vPixels.data(), data, 16) == 0);
	}

	SECTION("Write 1 line 16 bps")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint16_t inputData[8] = { 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007 }; // Big endian
		pBitmap->Init(8, 1);

		filler->Write(inputData, 2, 8);

		auto* pGray = dynamic_cast<CGrayBitmapT<WORD>*>(pBitmap.m_p);
		WORD data[8] = { 59395, 59395 + 256, 59395 + 2 * 256, 59395 + 3 * 256, 59395 + 4 * 256, 59395 + 5 * 256, 59395 + 6 * 256, 59395 + 7 * 256 };
		REQUIRE(memcmp(pGray->m_vPixels.data(), data, 16) == 0);
	}

	SECTION("Write 2 lines 16 bps with 1 call")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(2);
		std::uint16_t inputData[16] = { 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007 };
		pBitmap->Init(8, 2);

		filler->Write(inputData, 2, 16);

		auto* pGray = dynamic_cast<CGrayBitmapT<WORD>*>(pBitmap.m_p);
		WORD data[16];
		be2le(data, inputData);

		REQUIRE(memcmp(pGray->m_vPixels.data(), data, 32) == 0);
	}

	SECTION("Write 2 lines 16 bps with 2 calls")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(2);
		std::uint16_t inputData[16] = { 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007 };
		pBitmap->Init(8, 2);

		filler->Write(inputData, 2, 8);
		filler->Write(inputData + 8, 2, 8);

		auto* pGray = dynamic_cast<CGrayBitmapT<WORD>*>(pBitmap.m_p);
		WORD data[16];
		be2le(data, inputData);

		REQUIRE(memcmp(pGray->m_vPixels.data(), data, 32) == 0);
	}

	SECTION("2 lines 16 bps and adjust RGGB (this must set the CFA type of the MemoryBitmap")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(2);
		filler->SetWhiteBalance(2.0, 3.0, 4.0);
		filler->SetCFAType(CFATYPE_RGGB);
		std::uint16_t inputData[16] = { 256, 257, 258, 1003, 1075, 2328, 32767, 20000,   5000, 6000, 7000, 9002, 9003, 9004, 10010, 10100 };
		be2le(inputData, inputData);
		pBitmap->Init(8, 2);

		filler->Write(inputData, 2, 16);

		auto* pGray = dynamic_cast<CGrayBitmapT<WORD>*>(pBitmap.m_p);
		WORD data[16] = { 256 * 2, 257 * 3, 258 * 2, 1003 * 3, 1075 * 2, 2328 * 3, 32767 * 2, 20000 * 3,
						5000 * 3, 6000 * 4, 7000 * 3, 9002 * 4, 9003 * 3, 9004 * 4, 10010 * 3, 10100 * 4 };

		auto* pGrayBitmap = dynamic_cast<C16BitGrayBitmap*>(pBitmap.m_p);

		REQUIRE(memcmp(pGray->m_vPixels.data(), data, 32) == 0);
		REQUIRE(pGrayBitmap != nullptr);
		REQUIRE(pGrayBitmap->GetCFAType() == CFATYPE_RGGB);
	}

	SECTION("2 lines 16 bps and adjust different CFA schemes")
	{
		std::uint16_t inputData[16] = { 256, 257, 258, 1003, 1075, 2328, 12767, 13209,   5000, 6000, 7000, 9002, 9003, 9004, 10010, 10100 };
		auto* pGray = dynamic_cast<CGrayBitmapT<WORD>*>(pBitmap.m_p);

		const auto testCfaType = [&](const CFATYPE cfaType, WORD wb0, WORD wb1, WORD wb2, WORD wb3) -> int
		{
			auto filler = std::make_unique<BitMapFiller>(pBitmap, nullptr);
			filler->setGrey(true);
			filler->setMaxColors(65535);
			filler->setWidth(8);
			filler->setHeight(2);
			filler->SetWhiteBalance(2.0, 3.0, 4.0);
			filler->SetCFAType(cfaType);

			std::uint16_t beData[16];
			be2le(beData, inputData);
			pBitmap->Init(8, 2);
			filler->Write(beData, 2, 16);
			WORD wb[2][2] = {{wb0, wb1}, {wb2, wb3}};
			WORD expected[16];
			for (int y = 0; y < 2; ++y)
				for (int x = 0; x < 8; ++x)
					expected[y * 8 + x] = inputData[y * 8 + x] * wb[y % 2][x % 2];
			return memcmp(pGray->m_vPixels.data(), expected, 32);
		};

		REQUIRE(testCfaType(CFATYPE_BGGR, 4, 3, 3, 2) == 0);
		REQUIRE(testCfaType(CFATYPE_GRBG, 3, 2, 4, 3) == 0);
		REQUIRE(testCfaType(CFATYPE_GBRG, 3, 4, 2, 3) == 0);
		REQUIRE(testCfaType(CFATYPE_RGGB, 2, 3, 3, 4) == 0);
		// These are CFA types that do not need white balancing, so the coefficients are all 1.
		REQUIRE(testCfaType(CFATYPE_NONE, 1, 1, 1, 1) == 0);
		REQUIRE(testCfaType(CFATYPE_YMGC, 1, 1, 1, 1) == 0);
		REQUIRE(testCfaType(CFATYPE_CYMGCYGM, 1, 1, 1, 1) == 0);
	}

	SECTION("1 line 16 bps adjust green color and limit to maximum")
	{
		filler->setMaxColors(65535);
		filler->setWidth(4);
		filler->setHeight(1);
		filler->SetWhiteBalance(1.0, 10.0, 1.0); // Green factor = 10
		filler->SetCFAType(CFATYPE_RGGB);
		std::uint16_t inputData[4] = { 2560, 3249, 29265, 50000 }; // R, G, R, G
		be2le(inputData, inputData);
		pBitmap->Init(4, 1);

		filler->Write(inputData, 2, 4);
		constexpr WORD MAXIMUM = 65534; // For some strange reason, the BitMapFiller limits this to MAXWORD - 1
		auto* pGray = dynamic_cast<CGrayBitmapT<WORD>*>(pBitmap.m_p);
		WORD data[4] = { 2560, 3249 * 10, 29265, MAXIMUM };
		REQUIRE(memcmp(pGray->m_vPixels.data(), data, 8) == 0);
	}
}

TEST_CASE("BitMapFiller gray multicall", "[BitMap][BitMapFiller][gray][!shouldfail]")
{
	CSmartPtr<CMemoryBitmap> pBitmap;
	pBitmap.Attach(new CGrayBitmapT<WORD>);
	auto filler = std::make_unique<BitMapFiller>(pBitmap, nullptr);
	filler->setGrey(true);

	SECTION("Write 1 line 16 bps with 2 calls")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint16_t inputData[8] = { 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007 }; // Big endian
		pBitmap->Init(8, 1);

		filler->Write(inputData, 1, 9);
		filler->Write(((char*)inputData + 9), 1, 16 - 9);

		auto* pGray = dynamic_cast<CGrayBitmapT<WORD>*>(pBitmap.m_p);
		WORD data[8] = { 59395, 59395 + 256, 59395 + 2 * 256, 59395 + 3 * 256, 59395 + 4 * 256, 59395 + 5 * 256, 59395 + 6 * 256, 59395 + 7 * 256 };
		// Test does not succeed!
		REQUIRE(memcmp(pGray->m_vPixels.data(), data, 16) == 0);
	}
}

// -------------------
// Color
// -------------------

TEST_CASE("BitMapFiller color", "[BitMap][BitMapFiller][color]")
{
	CSmartPtr<CMemoryBitmap> pBitmap;
	pBitmap.Attach(new CColorBitmapT<WORD>);
	auto filler = std::make_unique<BitMapFiller>(pBitmap, nullptr);
	filler->setGrey(false);

	SECTION("Write 1 line 8 bps")
	{
		filler->setMaxColors(255);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint8_t inputData[24] = { 19, 20, 21,  45, 46, 47,  243, 244, 245,  65, 66, 67,  200, 201, 202,  110, 111, 112,  99, 100, 101,  225, 226, 227 }; // r,g,b, r,g,b, ...
		pBitmap->Init(8, 1);

		filler->Write(inputData, 3, 8);

		auto* pGray = dynamic_cast<CColorBitmapT<WORD>*>(pBitmap.m_p);
		const WORD* pRed = pGray->m_Red.m_vPixels.data();
		const WORD* pGreen = pGray->m_Green.m_vPixels.data();
		const WORD* pBlue = pGray->m_Blue.m_vPixels.data();
		bool correct = true;
		constexpr WORD twofiftysix = 256;
		for (int n = 0; n < 8; ++n)
		{
			if (pRed[n] != twofiftysix * inputData[3 * n])
				correct = false;
			if (pGreen[n] != twofiftysix * inputData[3 * n + 1])
				correct = false;
			if (pBlue[n] != twofiftysix * inputData[3 * n + 2])
				correct = false;
		}
		REQUIRE(correct == true);
	}

	SECTION("Write 1 line 16 bps")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint16_t inputData[24] = { 1000, 1001, 1002,  255, 256, 257,  243, 244, 245,  2002, 2003, 2004,  16540, 16796, 17052,  11110, 11111, 11112,  9900, 9901, 9902,  24325, 24326, 24327 };
		pBitmap->Init(8, 1);

		filler->Write(inputData, 6, 8);

		auto* pGray = dynamic_cast<CColorBitmapT<WORD>*>(pBitmap.m_p);
		const WORD* pRed = pGray->m_Red.m_vPixels.data();
		const WORD* pGreen = pGray->m_Green.m_vPixels.data();
		const WORD* pBlue = pGray->m_Blue.m_vPixels.data();

		std::uint16_t data[24];
		be2le(data, inputData);

		bool correct = true;
		for (int n = 0; n < 8; ++n)
		{
			if (pRed[n] != data[3 * n])
				correct = false;
			if (pGreen[n] != data[3 * n + 1])
				correct = false;
			if (pBlue[n] != data[3 * n + 2])
				correct = false;
		}
		REQUIRE(correct == true);
	}

	SECTION("Write 1 line 16 bps with 2 calls after RGB pixel")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint16_t inputData[24] = { 1000, 1001, 1002,  255, 256, 257,  243, 244, 245,  2002, 2003, 2004,  16540, 16796, 17052,  11110, 11111, 11112,  9900, 9901, 9902,  24325, 24326, 24327 };
		pBitmap->Init(8, 1);

		filler->Write(inputData, 6, 6);
		filler->Write(inputData + 18, 6, 2);

		auto* pGray = dynamic_cast<CColorBitmapT<WORD>*>(pBitmap.m_p);
		const WORD* pRed = pGray->m_Red.m_vPixels.data();
		const WORD* pGreen = pGray->m_Green.m_vPixels.data();
		const WORD* pBlue = pGray->m_Blue.m_vPixels.data();

		std::uint16_t data[24];
		be2le(data, inputData);

		bool correct = true;
		for (int n = 0; n < 8; ++n)
		{
			if (pRed[n] != data[3 * n])
				correct = false;
			if (pGreen[n] != data[3 * n + 1])
				correct = false;
			if (pBlue[n] != data[3 * n + 2])
				correct = false;
		}
		REQUIRE(correct == true);
	}

	SECTION("Write 2 lines with 1 call")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(2);
		std::uint16_t inputData[48] = { 1000, 1001, 1002,   255, 256, 257,    243, 244, 245,    2002, 2003, 2004,  16540, 16796, 17052,  11110, 11111, 11112,  9900, 9901, 9902,    24325, 24326, 24327,
										3000, 3001, 3002,  2255, 2256, 2257,  2243, 2244, 2245, 4002, 4003, 4004,  18540, 18796, 19052,  13110, 13111, 13112,  11900, 11901, 11902, 26325, 26326, 26327 };

		pBitmap->Init(8, 2);

		filler->Write(inputData, 6, 16);

		auto* pGray = dynamic_cast<CColorBitmapT<WORD>*>(pBitmap.m_p);
		const WORD* pRed = pGray->m_Red.m_vPixels.data();
		const WORD* pGreen = pGray->m_Green.m_vPixels.data();
		const WORD* pBlue = pGray->m_Blue.m_vPixels.data();

		std::uint16_t data[48];
		be2le(data, inputData);

		bool correct = true;
		for (int n = 0; n < 16; ++n)
		{
			if (pRed[n] != data[3 * n])
				correct = false;
			if (pGreen[n] != data[3 * n + 1])
				correct = false;
			if (pBlue[n] != data[3 * n + 2])
				correct = false;
		}
		REQUIRE(correct == true);
	}

	SECTION("Write 2 lines with 2 calls")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(2);
		std::uint16_t inputData[48] = { 1000, 1001, 1002,   255, 256, 257,    243, 244, 245,    2002, 2003, 2004,  16540, 16796, 17052,  11110, 11111, 11112,  9900, 9901, 9902,    24325, 24326, 24327,
										3000, 3001, 3002,  2255, 2256, 2257,  2243, 2244, 2245, 4002, 4003, 4004,  18540, 18796, 19052,  13110, 13111, 13112,  11900, 11901, 11902, 26325, 26326, 26327 };

		pBitmap->Init(8, 2);

		filler->Write(inputData, 6, 8);
		filler->Write(inputData + 24, 6, 8);

		auto* pGray = dynamic_cast<CColorBitmapT<WORD>*>(pBitmap.m_p);
		const WORD* pRed = pGray->m_Red.m_vPixels.data();
		const WORD* pGreen = pGray->m_Green.m_vPixels.data();
		const WORD* pBlue = pGray->m_Blue.m_vPixels.data();

		std::uint16_t data[48];
		be2le(data, inputData);

		bool correct = true;
		for (int n = 0; n < 16; ++n)
		{
			if (pRed[n] != data[3 * n])
				correct = false;
			if (pGreen[n] != data[3 * n + 1])
				correct = false;
			if (pBlue[n] != data[3 * n + 2])
				correct = false;
		}
		REQUIRE(correct == true);
	}

	SECTION("Write 1 line 16 bps with 1 call per pixel")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint16_t inputData[24] = { 1000, 1001, 1002,  255, 256, 257,  243, 244, 245,  2002, 2003, 2004,  16540, 16796, 17052,  11110, 11111, 11112,  9900, 9901, 9902,  24325, 24326, 24327 };
		pBitmap->Init(8, 1);

		for (int n = 0; n < 8; ++n)
			filler->Write(inputData + 3 * n, 6, 1);

		auto* pGray = dynamic_cast<CColorBitmapT<WORD>*>(pBitmap.m_p);
		const WORD* pRed = pGray->m_Red.m_vPixels.data();
		const WORD* pGreen = pGray->m_Green.m_vPixels.data();
		const WORD* pBlue = pGray->m_Blue.m_vPixels.data();

		std::uint16_t data[24];
		be2le(data, inputData);

		bool correct = true;
		for (int n = 0; n < 8; ++n)
		{
			if (pRed[n] != data[3 * n])
				correct = false;
			if (pGreen[n] != data[3 * n + 1])
				correct = false;
			if (pBlue[n] != data[3 * n + 2])
				correct = false;
		}
		REQUIRE(correct == true);
	}

	SECTION("Write 1 line 16 bps and adjust colors")
	{
		filler->setMaxColors(65535);
		filler->SetWhiteBalance(0.3, 0.24, 0.37);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint16_t inputData[24] = { 1000, 1001, 1002,  255, 256, 257,  243, 244, 245,  2002, 2003, 2004,  16540, 16796, 17052,  11110, 11111, 11112,  9900, 9901, 9902,  24325, 24326, 24327 };
		pBitmap->Init(8, 1);

		filler->Write(inputData, 6, 8);

		auto* pGray = dynamic_cast<CColorBitmapT<WORD>*>(pBitmap.m_p);
		const WORD* pRed = pGray->m_Red.m_vPixels.data();
		const WORD* pGreen = pGray->m_Green.m_vPixels.data();
		const WORD* pBlue = pGray->m_Blue.m_vPixels.data();

		be2le(inputData, inputData);
		bool correct = true;
		for (int n = 0; n < 8; ++n)
		{
			if (pRed[n] != static_cast<WORD>(0.3 * static_cast<double>(inputData[3 * n])))
				correct = false;
			if (pGreen[n] != static_cast<WORD>(0.24 * static_cast<double>(inputData[3 * n + 1])))
				correct = false;
			if (pBlue[n] != static_cast<WORD>(0.37 * static_cast<double>(inputData[3 * n + 2])))
				correct = false;
		}
		REQUIRE(correct == true);
	}
}


TEST_CASE("BitMapFiller color multicall", "[BitMap][BitMapFiller][color][!shouldfail]")
{
	CSmartPtr<CMemoryBitmap> pBitmap;
	pBitmap.Attach(new CColorBitmapT<WORD>);
	auto filler = std::make_unique<BitMapFiller>(pBitmap, nullptr);
	filler->setGrey(false);

	SECTION("Write 1 line 16 bps with 2 calls after red component")
	{
		filler->setMaxColors(65535);
		filler->setWidth(8);
		filler->setHeight(1);
		std::uint16_t inputData[24] = { 1000, 1001, 1002,  255, 256, 257,  243, 244, 245,  2002, 2003, 2004,  16540, 16796, 17052,  11110, 11111, 11112,  9900, 9901, 9902,  24325, 24326, 24327 };
		pBitmap->Init(8, 1);

		filler->Write(inputData, 2, 16);
		filler->Write(inputData + 18, 2, 8);

		auto* pGray = dynamic_cast<CColorBitmapT<WORD>*>(pBitmap.m_p);
		const WORD* pRed = pGray->m_Red.m_vPixels.data();
		const WORD* pGreen = pGray->m_Green.m_vPixels.data();
		const WORD* pBlue = pGray->m_Blue.m_vPixels.data();

		std::uint16_t data[24];
		be2le(data, inputData);

		bool correct = true;
		for (int n = 0; n < 8; ++n)
		{
			if (pRed[n] != data[3 * n])
				correct = false;
			if (pGreen[n] != data[3 * n + 1])
				correct = false;
			if (pBlue[n] != data[3 * n + 2])
				correct = false;
		}
		// Test does not succeed!
		REQUIRE(correct == true);
	}
}
