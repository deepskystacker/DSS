#pragma once

#include "PixelTransform.h"
#include "StackingTasks.h"
#include "BackgroundCalibration.h"
#include "BitmapExt.h"
#include <vector>
#include <tuple>


#if defined(AVX_INTRINSICS) && defined(_M_X64)
class AvxStacking {
private:
	long lineStart, lineEnd, colEnd;
	int width, height;
	int resultWidth, resultHeight;
	std::vector<float> xCoordinates;
	std::vector<float> yCoordinates;
	std::vector<float> redPixels;
	std::vector<float> greenPixels;
	std::vector<float> bluePixels;
	CMemoryBitmap& inputBitmap;
	CMemoryBitmap& tempBitmap;
public:
	AvxStacking() = delete;
	AvxStacking(long lStart, long lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const CRect& resultRect);
	AvxStacking(const AvxStacking&) = delete;
	AvxStacking(AvxStacking&&) = delete;
	AvxStacking& AvxStacking::operator=(const AvxStacking&) = delete;

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier);
private:
	template <class T>
	int doStack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier);

	int pixelTransform(const CPixelTransform& pixelTransformDef);

	template <class T>
	int backgroundCalibration(const CBackgroundCalibration& backgroundCalibrationDef);

	template <bool ISRGB, class T>
	int pixelDispatchAndAccumulate();
};

#else

class AvxStacking
{
public:
	AvxStacking(long lStart, long lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const CRect& resultRect) {}
	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier)
	{
		return 1;
	}
};
#endif


#if defined(AVX_INTRINSICS) && defined(_M_X64)

class AvxSupport
{
private:
	// Unfortunately, we cannot use const here, because the member function are hardly never const declared. :-(
	CMemoryBitmap& bitmap;

	template <class T>
	auto* getColorPtr() { return dynamic_cast<CColorBitmapT<T>*>(&bitmap); }
	template <class T>
	auto* getGrayPtr() { return dynamic_cast<CGrayBitmapT<T>*>(&bitmap); }
	template <class T>
	const auto* getColorPtr() const { return dynamic_cast<const CColorBitmapT<T>*>(&bitmap); }
	template <class T>
	const auto* getGrayPtr() const { return dynamic_cast<const CGrayBitmapT<T>*>(&bitmap); }

	int getNrChannels() const;
public:
	AvxSupport(CMemoryBitmap& b) noexcept;

	bool isColorBitmap() const;
	template <class T>
	bool isColorBitmapOfType() const;
	bool isMonochromeBitmap() const;
	template <class T>
	bool isMonochromeBitmapOfType() const;

	template <class T>
	const std::vector<T>& redPixels() const { return getColorPtr<T>()->m_Red.m_vPixels; }
	template <class T>
	const std::vector<T>& greenPixels() const { return getColorPtr<T>()->m_Green.m_vPixels; }
	template <class T>
	const std::vector<T>& bluePixels() const { return getColorPtr<T>()->m_Blue.m_vPixels; }
	template <class T>
	const std::vector<T>& grayPixels() const { return getGrayPtr<T>()->m_vPixels; }

	template <class T>
	std::vector<T>& redPixels() { return getColorPtr<T>()->m_Red.m_vPixels; }
	template <class T>
	std::vector<T>& greenPixels() { return getColorPtr<T>()->m_Green.m_vPixels; }
	template <class T>
	std::vector<T>& bluePixels() { return getColorPtr<T>()->m_Blue.m_vPixels; }
	template <class T>
	std::vector<T>& grayPixels() { return getGrayPtr<T>()->m_vPixels; }

	const int width() const;

	template <class T>
	bool bitmapHasCorrectType() const;

	static bool checkCpuFeatures() noexcept;

	inline static __m256 wordToPackedFloat(const __m128i x) noexcept
	{
		return _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(x));
	}

	inline static __m256 cvtEpu32Ps(const __m256i x) noexcept
	{
		const __m256i mask = _mm256_cmpgt_epi32(_mm256_setzero_si256(), x); // 0 > x (= x < 0)
		const __m256 ps = _mm256_cvtepi32_ps(x);
		const __m256 corr = _mm256_add_ps(_mm256_set1_ps(static_cast<float>(0xffffffffU)), ps); // UINTMAX - x
		return _mm256_blendv_ps(ps, corr, _mm256_castsi256_ps(mask)); // Take (UINTMAX - x) where x < 0
	}

	inline static __m256i cmpGtEpu16(const __m256i a, const __m256i b) noexcept
	{
		const __m256i highBit = _mm256_set1_epi16(WORD{ 0x8000 });
		return _mm256_cmpgt_epi16(_mm256_xor_si256(a, highBit), _mm256_xor_si256(b, highBit));
	};

	// Read color values from T* and return 2 x 8 packed single.
	inline static std::tuple<__m256, __m256> read16PackedSingle(const WORD *const pColor) noexcept
	{
			const __m256i icolor = _mm256_loadu_si256((const __m256i*)pColor);
			const __m256 lo8 = wordToPackedFloat(_mm256_castsi256_si128(icolor));
			const __m256 hi8 = wordToPackedFloat(_mm256_extracti128_si256(icolor, 1));
			return { lo8, hi8 };
	};
	inline static std::tuple<__m256, __m256> read16PackedSingle(const float *const pColor) noexcept
	{
		return { _mm256_loadu_ps(pColor), _mm256_loadu_ps(pColor + 8) };
	}
	inline static std::tuple<__m256, __m256> read16PackedSingle(const std::uint32_t* const pColor) noexcept
	{
		return {
			_mm256_cvtepi32_ps(_mm256_srli_epi32(_mm256_loadu_epi32(pColor), 16)), // Shift 16 bits right while shifting in zeros.
			_mm256_cvtepi32_ps(_mm256_srli_epi32(_mm256_loadu_epi32(pColor + 8), 16))
		};
	}

	// Accumulate packed single newColor to T* oldColor
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 newColor, const __m256i mask, const std::uint16_t *const pOutputBitmap) noexcept;
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 newColor, const __m256i mask, const std::uint32_t* const pOutputBitmap) noexcept;
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 newColor, const __m256i mask, const float *const pOutputBitmap) noexcept;

	template <class T>
	static float accumulateSingleColorValue(const size_t outNdx, const float newColor, const int mask, const T* const pOutputBitmap) noexcept;
};

#endif
