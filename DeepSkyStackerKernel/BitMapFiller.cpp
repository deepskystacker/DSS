#include "pch.h"
#include "BitMapFiller.h"
#include "avx_bitmap_filler.h"
#include "avx_simd_check.h"
#include "DSSProgress.h"
#include "GrayBitmap.h"
#include "ColorBitmap.h"

using namespace DSS;

// ----------------------------------
// BitmapFillerInterface
// ----------------------------------

std::unique_ptr<BitmapFillerInterface> BitmapFillerInterface::makeBitmapFiller(CMemoryBitmap* pBitmap, OldProgressBase* pProgress)
{
	if (AvxSimdCheck::checkSimdAvailability())
		return std::make_unique<AvxBitmapFiller>(pBitmap, pProgress);
	else
		return std::make_unique<NonAvxBitmapFiller>(pBitmap, pProgress);
}

bool BitmapFillerInterface::isThreadSafe() const { return false; }


// ----------------------------------
// BitmapFillerBase
// ----------------------------------

BitmapFillerBase::BitmapFillerBase(CMemoryBitmap* pB, OldProgressBase* pP) :
	BitmapFillerInterface{},
	pProgress{ pP },
	pBitmap{ pB },
	cfaType{ CFATYPE_NONE },
	isGray{ true },
	width{ 0 },
	height{ 0 },
	bytesPerChannel{ 0 },
	redBuffer{},
	greenBuffer{},
	blueBuffer{}
{}

void BitmapFillerBase::SetCFAType(CFATYPE cfaTp)
{
	this->cfaType = cfaTp;
	if (auto* pGray16Bitmap = dynamic_cast<C16BitGrayBitmap*>(pBitmap))
		pGray16Bitmap->SetCFAType(cfaTp);
}

bool BitmapFillerBase::isRgbBayerPattern() const
{
	switch (this->cfaType)
	{
	case CFATYPE_BGGR:
	case CFATYPE_GRBG:
	case CFATYPE_GBRG:
	case CFATYPE_RGGB: return true;
	default: return false;
	}
}

void BitmapFillerBase::setGrey(bool grey)
{
	this->isGray = grey;
}

void BitmapFillerBase::setWidth(int w)
{
	this->width = w;
}

void BitmapFillerBase::setHeight(int h)
{
	this->height = h;
	//if (pProgress != nullptr)					// Commented out as no matching End2()
	//	pProgress->Start2(pBitmap->Height());
}

void BitmapFillerBase::setMaxColors(int maxcolors)
{
	this->bytesPerChannel = maxcolors > 255 ? 2 : 1;
}


// ---------------------------------
// Non-AVX Bitmap Filler
// ---------------------------------

NonAvxBitmapFiller::NonAvxBitmapFiller(CMemoryBitmap* pB, OldProgressBase* pP) :
	BitmapFillerBase{ pB, pP }
{}

bool NonAvxBitmapFiller::isThreadSafe() const { return true; }

std::unique_ptr<BitmapFillerInterface> NonAvxBitmapFiller::clone()
{
	return std::make_unique<NonAvxBitmapFiller>(*this);
}
#if defined(_MSC_VER)
#define bswap_16(x) _byteswap_ushort(x)
#elif defined(__GNUC__)
#define bswap_16(x) __builtin_bswap16(x)
#else
#error Compiler not yet supported
#endif

size_t NonAvxBitmapFiller::Write(const void* source, const size_t bytesPerPixel, const size_t nrPixels, const size_t rowIndex)
{
	ZASSERTSTATE(0 != this->width);
	ZASSERTSTATE(0 != this->height);
	ZASSERTSTATE(0 != this->bytesPerChannel);
	ZASSERTSTATE((nrPixels % static_cast<size_t>(this->width)) == 0);

	if (this->isGray)
	{
		ZASSERTSTATE(bytesPerPixel == this->bytesPerChannel);
		// constexpr size_t vectorLen = 16;
		redBuffer.resize(nrPixels);

		if (this->bytesPerChannel == 1)
		{
			const std::uint8_t* const pData = static_cast<const std::uint8_t*>(source);
			for (size_t i = 0; i < nrPixels; ++i)
				redBuffer[i] = static_cast<float>(static_cast<std::uint16_t>(pData[i]) << 8);
		}
		else
		{
			const std::uint16_t* const pData = static_cast<const std::uint16_t*>(source);
			for (size_t i = 0; i < nrPixels; ++i)
				redBuffer[i] = static_cast<float>(bswap_16(pData[i])); // Load and convert to little endian
		}

		auto* pGray16Bitmap = dynamic_cast<C16BitGrayBitmap*>(pBitmap);
		ZASSERTSTATE(pGray16Bitmap != nullptr);
		std::uint16_t* const pOut = pGray16Bitmap->m_vPixels.data() + rowIndex * nrPixels;
		for (size_t i = 0; i < nrPixels; ++i)
			pOut[i] = static_cast<std::uint16_t>(redBuffer[i]);
	}
	else
	{
		ZASSERTSTATE(bytesPerPixel == this->bytesPerChannel * 3);
		redBuffer.resize(nrPixels);
		greenBuffer.resize(nrPixels);
		blueBuffer.resize(nrPixels);

		if (this->bytesPerChannel == 1)
		{
			const std::uint8_t* pData = static_cast<const std::uint8_t*>(source);
			for (size_t i = 0; i < nrPixels; ++i, pData += 3)
			{
				redBuffer[i] = static_cast<float>(static_cast<std::uint16_t>(pData[0]) << 8);
				greenBuffer[i] = static_cast<float>(static_cast<std::uint16_t>(pData[1]) << 8);
				blueBuffer[i] = static_cast<float>(static_cast<std::uint16_t>(pData[2]) << 8);
			}
		}
		else
		{
			const std::uint16_t* pData = static_cast<const std::uint16_t*>(source);
			for (size_t i = 0; i < nrPixels; ++i, pData += 3)
			{
				redBuffer[i] = static_cast<float>(bswap_16(pData[0]));
				greenBuffer[i] = static_cast<float>(bswap_16(pData[1]));
				blueBuffer[i] = static_cast<float>(bswap_16(pData[2]));
			}
		}

		auto* pColor16Bitmap = dynamic_cast<C48BitColorBitmap*>(pBitmap);
		ZASSERTSTATE(pColor16Bitmap != nullptr);
		std::uint16_t* const pOutRed = pColor16Bitmap->m_Red.m_vPixels.data() + rowIndex * nrPixels;
		std::uint16_t* const pOutGreen = pColor16Bitmap->m_Green.m_vPixels.data() + rowIndex * nrPixels;
		std::uint16_t* const pOutBlue = pColor16Bitmap->m_Blue.m_vPixels.data() + rowIndex * nrPixels;
		for (size_t i = 0; i < nrPixels; ++i)
		{
			pOutRed[i] = static_cast<std::uint16_t>(redBuffer[i]);
			pOutGreen[i] = static_cast<std::uint16_t>(greenBuffer[i]);
			pOutBlue[i] = static_cast<std::uint16_t>(blueBuffer[i]);
		}
	}

	//if (((rowIndex + 1) % 32) == 0 && this->pProgress != nullptr)
	//	this->pProgress->Progress2(static_cast<int>(rowIndex + 1));

	return nrPixels;
}
