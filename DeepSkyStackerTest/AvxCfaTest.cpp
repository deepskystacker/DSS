#include "stdafx.h"
#include "catch.h"
#include "../DeepSkyStacker/avx_cfa.h"

TEST_CASE("AVX CFA", "[AVX][CFA]")
{
	class BitMap : public CMemoryBitmap
	{
	public:
		virtual bool Init(LONG lWidth, LONG lHeight) override { return true; }
		virtual void SetPixel(LONG i, LONG j, double fRed, double fGreen, double fBlue) override {}
		virtual void SetPixel(LONG i, LONG j, double fGray) override {}
		virtual void GetPixel(LONG i, LONG j, double& fRed, double& fGreen, double& fBlue) override {}
		virtual void GetPixel(LONG i, LONG j, double& fGray) override {}
		virtual bool GetScanLine(LONG j, void* pScanLine) override { return true; }
		virtual bool SetScanLine(LONG j, void* pScanLine) override { return true; }
		virtual LONG Height() override { return 1; }
		virtual LONG Width() override { return 1; }
		virtual LONG BitPerSample() override { return 16; }
		virtual LONG IsFloat() override { return 0; }
		virtual bool IsMonochrome() override { return true; }
		virtual CMemoryBitmap* Clone(bool bEmpty = false) override { return nullptr; }
		virtual CMultiBitmap* CreateEmptyMultiBitmap() override { return nullptr; }
		virtual void GetMedianFilterEngine(CMedianFilterEngine** pMedianFilterEngine) override {}
		virtual void GetIterator(CPixelIterator** ppIterator, LONG x = 0, LONG y = 0) override {}
		virtual double GetMaximumValue() override { return 0.0; }
		virtual void GetCharacteristics(CBitmapCharacteristics& bc) override {}
	};

	BitMap bitmap1{};
	AvxCfaProcessing avxCfaProcessing(0, 0, bitmap1);

	SECTION("Init initializes the object and sets nrVectorsPerLine properly")
	{
		avxCfaProcessing.init(0, 1);
		REQUIRE(avxCfaProcessing.nrVectorsPerLine() == 1);
	}

	SECTION("Init sets nrVectorsPerLine to 2 for width 17")
	{
		class BitMap2 : public BitMap
		{
			virtual LONG Width() override { return 17; }
		};

		BitMap2 bitmap2{};
		AvxCfaProcessing avxCfaProcessing(0, 0, bitmap2);

		avxCfaProcessing.init(0, 1);
		REQUIRE(avxCfaProcessing.nrVectorsPerLine() == 2);
	}

	SECTION("Interpolate return 1 if pixelsizemultiplier is 2")
	{
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 2) == 1);
	}

	SECTION("Interpolate return 1, because input bitmap is not a gray bitmap")
	{
		REQUIRE(avxCfaProcessing.interpolate(0, 1, 1) == 1);
	}
}




bool CMultitask::GetUseSimd()
{
	return true;
}
