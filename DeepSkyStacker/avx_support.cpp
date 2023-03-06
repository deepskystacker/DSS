#include "stdafx.h"
#include "avx_support.h"
#include "BitmapCharacteristics.h"
#include "Multitask.h"



AvxSupport::AvxSupport(CMemoryBitmap& b) noexcept :
	bitmap{ b }
{};

int AvxSupport::getNrChannels() const
{
	CBitmapCharacteristics bitmapCharacteristics;
	const_cast<CMemoryBitmap&>(bitmap).GetCharacteristics(bitmapCharacteristics);
	return bitmapCharacteristics.m_lNrChannels;
};

bool AvxSupport::isColorBitmap() const
{
	return getNrChannels() == 3;
};

template <class T>
bool AvxSupport::isColorBitmapOfType() const
{
	auto* const p = const_cast<AvxSupport*>(this)->getColorPtr<T>();
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
	if (auto* const p = const_cast<AvxSupport*>(this)->getGrayPtr<T>())
	{
		// Note that Monochrome bitmaps are always topdown -> no extra check required! CF. CGrayBitmap::GetOffset().
		if constexpr (std::is_same<T, float>::value)
			return (p->IsFloat() && !p->IsCFA() && p->GetMultiplier() == 256.0);
		if constexpr (std::is_same<T, std::uint16_t>::value)
			return (!p->IsCFA() || isMonochromeCfaBitmapOfType<std::uint16_t>());
		return !p->IsCFA();
	}
	return false;
}

template <class T>
bool AvxSupport::isMonochromeCfaBitmapOfType() const
{
	// CFA only supported for T=16 bits unsigned
	if constexpr (std::is_same<T, std::uint16_t>::value)
	{
		auto* const pGray = const_cast<AvxSupport*>(this)->getGrayPtr<T>();
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
	if (auto* pGray = const_cast<AvxSupport*>(this)->getGrayPtr<std::uint16_t>()) // GetCFAType is a non-const funtion :-(
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

bool AvxSupport::checkAvx2CpuSupport()
{
	int cpuid[4] = { -1 };

	__cpuidex(cpuid, 1, 0);
	const bool FMAsupported = ((cpuid[2] & (1 << 12)) != 0);
	const bool XSAVEsupported = ((cpuid[2] & (1 << 26)) != 0);
	const bool OSXSAVEsupported = ((cpuid[2] & (1 << 27)) != 0);

	__cpuidex(cpuid, 7, 0);
	const bool AVX2supported = ((cpuid[1] & (1 << 5)) != 0);
	//const bool BMI1supported = ((cpuid[1] & (1 << 3) != 0);
	//const bool BMI2supported = ((cpuid[1] & (1 << 8)) != 0);

	const bool RequiredCpuFlags = FMAsupported && AVX2supported && XSAVEsupported && OSXSAVEsupported;

	// OS supports AVX (YMM registers) - Note: XGETBV may only be executed on CPUs with XSAVE flag set.
	const bool AVXenabledOS = RequiredCpuFlags ? ((_xgetbv(0) & 6) == 6) : false; // 6 = SSE (0x2) + YMM (0x4)

	// Additionally set flush to zero and denormals to zero - Note: (S)GETCSR are SSE instructions, so supported by all x64 CPUs.
	_mm_setcsr(_mm_getcsr() | _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);

	return (RequiredCpuFlags && AVXenabledOS);
};

bool AvxSupport::checkSimdAvailability()
{
	// If user has disabled SIMD vectorisation (settings dialog) -> return false;
	return CMultitask::GetUseSimd() && checkAvx2CpuSupport();
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
