#include "StdAfx.h"
#include "BitMapFiller.h"
#include "avx_bitmap_filler.h"
#include "avx_support.h"

// ----------------------------------
// BitmapFillerInterface
// ----------------------------------

std::unique_ptr<BitmapFillerInterface> BitmapFillerInterface::makeBitmapFiller(CMemoryBitmap* pBitmap, CDSSProgress* pProgress, const double redWb, const double greenWb, const double blueWb)
{
	if (AvxSupport::checkSimdAvailability())
		return std::make_unique<AvxBitmapFiller>(pBitmap, pProgress, redWb, greenWb, blueWb);
	else
		return std::make_unique<NonAvxBitmapFiller>(pBitmap, pProgress, redWb, greenWb, blueWb);
}

bool BitmapFillerInterface::isThreadSafe() const { return false; }


// ----------------------------------
// BitmapFillerBase
// ----------------------------------

BitmapFillerBase::BitmapFillerBase(CMemoryBitmap* pB, CDSSProgress* pP, const double redWb, const double greenWb, const double blueWb) :
	pBitmap{ pB },
	pProgress{ pP },
	redScale{ static_cast<float>(redWb) },
	greenScale{ static_cast<float>(greenWb) },
	blueScale{ static_cast<float>(blueWb) },

	cfaType{ CFATYPE_NONE },
	isGray{ true },
	width{ 0 },
	height{ 0 },
	bytesPerChannel{ 0 },
	redBuffer{},
	greenBuffer{},
	blueBuffer{},
	cfaFactors{ 1.0f, 1.0f, 1.0f, 1.0f }
{}

void BitmapFillerBase::SetCFAType(CFATYPE cfaTp)
{
	this->cfaType = cfaTp;
	if (auto* pGray16Bitmap = dynamic_cast<C16BitGrayBitmap*>(pBitmap))
		pGray16Bitmap->SetCFAType(cfaTp);
	setCfaFactors();
}

void BitmapFillerBase::setCfaFactors()
{
	const auto setFactors = [this](const float f0, const float f1, const float f2, const float f3) -> void
	{
		this->cfaFactors.assign({ f0, f1, f2, f3 });
	};

	switch (cfaType)
	{
	case CFATYPE_BGGR: return setFactors(blueScale, greenScale, greenScale, redScale);
	case CFATYPE_GRBG: return setFactors(greenScale, redScale, blueScale, greenScale);
	case CFATYPE_GBRG: return setFactors(greenScale, blueScale, redScale, greenScale);
	case CFATYPE_RGGB: return setFactors(redScale, greenScale, greenScale, blueScale);
	}
};

bool BitmapFillerBase::isRgbBayerPattern() const
{
	switch (this->cfaType)
	{
	case CFATYPE_BGGR:
	case CFATYPE_GRBG:
	case CFATYPE_GBRG:
	case CFATYPE_RGGB: return true;
	}
	return false;
}

void BitmapFillerBase::setGrey(bool grey)
{
	this->isGray = grey;
}

void BitmapFillerBase::setWidth(LONG width)
{
	this->width = width;
}

void BitmapFillerBase::setHeight(LONG height)
{
	this->height = height;
	if (pProgress != nullptr)
		pProgress->Start2(nullptr, pBitmap->Height());
}

void BitmapFillerBase::setMaxColors(LONG maxcolors)
{
	this->bytesPerChannel = maxcolors > 255 ? 2 : 1;
}
