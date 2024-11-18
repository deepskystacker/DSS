#include "stdafx.h"
#include "avx_includes.h"
#include "avx_support.h"
#include "BitmapCharacteristics.h"
#include "ztrace.h"


AvxSupport::AvxSupport(const CMemoryBitmap& b) noexcept :
	bitmap{ b },
	pBitmap{ nullptr }
{};

AvxSupport::AvxSupport(CMemoryBitmap& b) noexcept :
	bitmap{ b },
	pBitmap{ &b }
{};

int AvxSupport::getNrChannels() const
{
	CBitmapCharacteristics bitmapCharacteristics;
	bitmap.GetCharacteristics(bitmapCharacteristics);
	return bitmapCharacteristics.m_lNrChannels;
};

bool AvxSupport::isColorBitmap() const
{
	return getNrChannels() == 3;
};

template <class T>
bool AvxSupport::isColorBitmapOfType() const
{
	const auto* const p = this->getColorPtr<T>();
	const bool isColor = p != nullptr && p->isTopDown();
	if constexpr (std::is_same<T, float>::value)
		return isColor && p->IsFloat() && p->GetMultiplier() == 256.0;
	else
		return isColor;
}

bool AvxSupport::isMonochromeBitmap() const
{
	return getNrChannels() == 1;
};

template <class T>
bool AvxSupport::isMonochromeBitmapOfType() const
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
bool AvxSupport::isMonochromeCfaBitmapOfType() const
{
	// CFA only supported for T=16 bits unsigned
	if constexpr (std::is_same<T, std::uint16_t>::value)
	{
		const auto* const pGray = this->getGrayPtr<T>();
		// We support CFA only for RGGB Bayer matrices with BILINEAR interpolation and no offsets.
		return (pGray != nullptr && pGray->IsCFA() && pGray->GetCFATransformation() == CFAT_BILINEAR && pGray->xOffset() == 0 && pGray->yOffset() == 0
			&& (pGray->GetCFAType() == CFATYPE_RGGB || pGray->GetCFAType() == CFATYPE_GBRG));
	}
	else
		return false;
};

bool AvxSupport::isColorBitmapOrCfa() const
{
	return isColorBitmap() || isMonochromeCfaBitmapOfType<std::uint16_t>();
}

CFATYPE AvxSupport::getCfaType() const
{
	if (auto* pGray = this->getGrayPtr<std::uint16_t>())
		return pGray->GetCFAType();
	else
		return CFATYPE_NONE;
}

const int AvxSupport::width() const {
	return bitmap.Width();
}

template <class T>
bool AvxSupport::bitmapHasCorrectType() const
{
	return (isColorBitmapOfType<T>() || isMonochromeBitmapOfType<T>());
}

// Explicit template instantiation for the types we need.
template bool AvxSupport::bitmapHasCorrectType<std::uint16_t>() const;
template bool AvxSupport::bitmapHasCorrectType<std::uint32_t>() const;
template bool AvxSupport::bitmapHasCorrectType<float>() const;
template bool AvxSupport::bitmapHasCorrectType<double>() const;

template bool AvxSupport::isMonochromeCfaBitmapOfType<std::uint16_t>() const;
template bool AvxSupport::isMonochromeCfaBitmapOfType<std::uint32_t>() const;
template bool AvxSupport::isMonochromeCfaBitmapOfType<float>() const;
template bool AvxSupport::isMonochromeCfaBitmapOfType<double>() const;
