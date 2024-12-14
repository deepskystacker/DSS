#include "pch.h"
#include "avx_bitmap_util.h"
#include "BitmapCharacteristics.h"


AvxBitmapUtil::AvxBitmapUtil(const CMemoryBitmap& b) :
	bitmap{ b },
	pBitmap{ nullptr }
{}

AvxBitmapUtil::AvxBitmapUtil(CMemoryBitmap& b) :
	bitmap{ b },
	pBitmap{ &b }
{}

int AvxBitmapUtil::getNrChannels() const
{
	CBitmapCharacteristics bitmapCharacteristics;
	bitmap.GetCharacteristics(bitmapCharacteristics);
	return bitmapCharacteristics.m_lNrChannels;
}

bool AvxBitmapUtil::isColorBitmap() const
{
	return getNrChannels() == 3;
}

template <class T>
bool AvxBitmapUtil::isColorBitmapOfType() const
{
	const auto* const p = this->getColorPtr<T>();
	const bool isColor = p != nullptr && p->isTopDown();
	if constexpr (std::is_same<T, float>::value)
		return isColor && p->IsFloat() && p->GetMultiplier() == 256.0;
	else
		return isColor;
}

bool AvxBitmapUtil::isMonochromeBitmap() const
{
	return getNrChannels() == 1;
}

template <class T>
bool AvxBitmapUtil::isMonochromeBitmapOfType() const
{
	if (const auto* const p = this->getGrayPtr<T>())
	{
		// Note that Monochrome bitmaps are always topdown -> no extra check required! CF. CGrayBitmap::GetOffset().
		if constexpr (std::is_same_v<T, float>)
			return (p->IsFloat() && !p->IsCFA() && p->GetMultiplier() == 256.0);
		else if constexpr (std::is_same_v<T, std::uint16_t>)
			return (!p->IsCFA() || isMonochromeCfaBitmapOfType<std::uint16_t>());
		else
			return !p->IsCFA();
	}
	else
		return false;
}

template <class T>
bool AvxBitmapUtil::isMonochromeCfaBitmapOfType() const
{
	// CFA only supported for T=16 bits unsigned
	if constexpr (std::is_same_v<T, std::uint16_t>)
	{
		const auto* const pGray = this->getGrayPtr<T>();
		// We support CFA only for RGGB or GBRG Bayer matrices with BILINEAR interpolation and no offsets.
		return (pGray != nullptr && pGray->IsCFA() && pGray->GetCFATransformation() == CFAT_BILINEAR && pGray->xOffset() == 0 && pGray->yOffset() == 0
			&& (pGray->GetCFAType() == CFATYPE_RGGB || pGray->GetCFAType() == CFATYPE_GBRG));
	}
	else
		return false;
}

bool AvxBitmapUtil::isColorBitmapOrCfa() const
{
	return isColorBitmap() || isMonochromeCfaBitmapOfType<std::uint16_t>();
}

CFATYPE AvxBitmapUtil::getCfaType() const
{
	if (auto* pGray = this->getGrayPtr<std::uint16_t>())
		return pGray->GetCFAType();
	else
		return CFATYPE_NONE;
}

const int AvxBitmapUtil::width() const {
	return bitmap.Width();
}

template <class T>
bool AvxBitmapUtil::bitmapHasCorrectType() const
{
	return (isColorBitmapOfType<T>() || isMonochromeBitmapOfType<T>());
}

// Explicit template instantiation for the types we need.

// Note that bitmapHasCorrectType() will also instantiate isColorBitmapOfType().
template bool AvxBitmapUtil::bitmapHasCorrectType<std::uint16_t>() const;
template bool AvxBitmapUtil::bitmapHasCorrectType<std::uint32_t>() const;
template bool AvxBitmapUtil::bitmapHasCorrectType<float>() const;
template bool AvxBitmapUtil::bitmapHasCorrectType<double>() const;

template bool AvxBitmapUtil::isMonochromeCfaBitmapOfType<std::uint16_t>() const;
template bool AvxBitmapUtil::isMonochromeCfaBitmapOfType<std::uint32_t>() const;
template bool AvxBitmapUtil::isMonochromeCfaBitmapOfType<float>() const;
template bool AvxBitmapUtil::isMonochromeCfaBitmapOfType<double>() const;
