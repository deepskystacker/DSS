#pragma once

#include "avx_cfa.h"
#include "PixelTransform.h"
#include "StackingTasks.h"
#include "BackgroundCalibration.h"
#include "BitmapExt.h"
#include <vector>
#include <tuple>


#if defined(AVX_INTRINSICS) && defined(_M_X64)

class AvxStacking
{
private:
	long lineStart, lineEnd, colEnd;
	int width, height;
	int resultWidth, resultHeight;
	std::vector<__m256> xCoordinates;
	std::vector<__m256> yCoordinates;
	std::vector<__m256> redPixels;
	std::vector<__m256> greenPixels;
	std::vector<__m256> bluePixels;
	CMemoryBitmap& inputBitmap;
	CMemoryBitmap& tempBitmap;
	AvxCfaProcessing avxCfa;
public:
	AvxStacking() = delete;
	AvxStacking(long lStart, long lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const CRect& resultRect);
	AvxStacking(const AvxStacking&) = default;
	AvxStacking(AvxStacking&&) = delete;
	AvxStacking& AvxStacking::operator=(const AvxStacking&) = delete;

	void init(const long lStart, const long lEnd);

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier);
private:
	static size_t numberOfAvxPsVectors(const size_t width);
	void resizeColorVectors(const size_t nrVectors);

	template <class T>
	int doStack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier);

	int pixelTransform(const CPixelTransform& pixelTransformDef);

	template <class T, class LoopFunction, class InterpolParam>
	int backgroundCalibLoop(const LoopFunction& loopFunc, const class AvxSupport& avxSupport, const InterpolParam& redParams, const InterpolParam& greenParams, const InterpolParam& blueParams);

	template <class T>
	int backgroundCalibration(const CBackgroundCalibration& backgroundCalibrationDef);

	template <bool ISRGB, class T>
	int pixelPartitioning();
};

#else

class AvxStacking
{
public:
	AvxStacking(long lStart, long lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const CRect& resultRect) {}
	void init(const long, const long) {}
	int stack(const CPixelTransform&, const CTaskInfo&, const CBackgroundCalibration&, const long)
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
	template <class T> bool isColorBitmapOfType() const;
	bool isMonochromeBitmap() const;
	template <class T> bool isMonochromeBitmapOfType() const;
	template <class T> bool isMonochromeCfaBitmapOfType() const;
	bool isColorBitmapOrCfa() const;

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

	static bool checkSimdAvailability() noexcept;

	inline static __m256 wordToPackedFloat(const __m128i x) noexcept
	{
		return _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(x));
	}

	inline static std::tuple<__m256d, __m256d, __m256d, __m256d> wordToPackedDouble(const __m256i x) noexcept
	{
		const __m256i i1 = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(x)); // cvt
		const __m256i i2 = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(x, 1)); // extract, cvt
		return {
			_mm256_cvtepi32_pd(_mm256_castsi256_si128(i1)), // cvt
			_mm256_cvtepi32_pd(_mm256_extracti128_si256(i1, 1)), // extract, cvt
			_mm256_cvtepi32_pd(_mm256_castsi256_si128(i2)), // cvt
			_mm256_cvtepi32_pd(_mm256_extracti128_si256(i2, 1)) // extract, cvt
		};
	}

	inline static __m256 cvtEpu32Ps(const __m256i x) noexcept
	{
		const __m256i mask = _mm256_cmpgt_epi32(_mm256_setzero_si256(), x); // 0 > x (= x < 0)
		const __m256 ps = _mm256_cvtepi32_ps(x);
		const __m256 corr = _mm256_add_ps(_mm256_set1_ps(static_cast<float>(0x100000000ULL)), ps); // UINTMAX - x (Note: 'add_ps' is correct!)
		return _mm256_blendv_ps(ps, corr, _mm256_castsi256_ps(mask)); // Take (UINTMAX - x) where x < 0
	}

	inline static std::tuple<__m256d, __m256d> cvtEpu32Pd(const __m256i x) noexcept
	{
		const __m256i mask = _mm256_cmpgt_epi32(_mm256_setzero_si256(), x); // 0 > x (= x < 0)
		const __m256d d1 = _mm256_cvtepi32_pd(_mm256_castsi256_si128(x));
		const __m256d d2 = _mm256_cvtepi32_pd(_mm256_extracti128_si256(x, 1));
		const __m256d corr1 = _mm256_add_pd(_mm256_set1_pd(static_cast<double>(0x100000000ULL)), d1); // UINTMAX - x (Note: 'add_ps' is correct!)
		const __m256d corr2 = _mm256_add_pd(_mm256_set1_pd(static_cast<double>(0x100000000ULL)), d2);
		return {
			_mm256_blendv_pd(d1, corr1, _mm256_cmp_pd(d1, _mm256_setzero_pd(), 17)), // 17: OP := _CMP_LT_OQ
			_mm256_blendv_pd(d2, corr2, _mm256_cmp_pd(d2, _mm256_setzero_pd(), 17)) // Take (UINTMAX - x) where x < 0
		};
	}

	inline static std::tuple<__m256d, __m256d> cvtPsPd(const __m256 x) noexcept
	{
		return {
			_mm256_cvtps_pd(_mm256_castps256_ps128(x)),
			_mm256_cvtps_pd(_mm256_extractf128_ps(x, 1))
		};
	}

	inline static __m128i cvtEpi32Epu16(const __m256i epi32) noexcept
	{
		const __m256i epu16 = _mm256_packus_epi32(epi32, _mm256_permute2x128_si256(epi32, epi32, 1)); // (a3, a2, a1, a0, a7, a6, a5, a4, a7, a6, a5, a4, a3, a2, a1, a0)
		// Upper lane is now wrong and useless.
		return _mm256_castsi256_si128(epu16);
	}

	inline static __m128i cvtPsEpu16(const __m256 x) noexcept
	{
		const __m256i epi32 = _mm256_cvtps_epi32(x);
		return cvtEpi32Epu16(epi32);
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

	inline static __m256i cvt2xEpi32Epu16(const __m256i lo, const __m256i hi)
	{
		return _mm256_packus_epi32(_mm256_permute2x128_si256(lo, hi, 0x20), _mm256_permute2x128_si256(lo, hi, 0x31));
	}

	// Read color values from T* and return 16 x packed short
	inline static __m256i read16PackedShort(const WORD* const pColor)
	{
		return _mm256_loadu_epi16(pColor);
	}
	inline static __m256i read16PackedShort(const std::uint32_t* const pColor)
	{
		const __m256i lo = _mm256_srli_epi32(_mm256_loadu_epi32(pColor), 16); // Shift 16 bits right while shifting in zeros.
		const __m256i hi = _mm256_srli_epi32(_mm256_loadu_epi32(pColor + 8), 16);
		return cvt2xEpi32Epu16(lo, hi);
	}
	inline static __m256i read16PackedShort(const float* const pColor)
	{
		const __m256i lo = _mm256_min_epi32(_mm256_cvtps_epi32(_mm256_loadu_ps(pColor)), _mm256_set1_epi32(0x0ffff));
		const __m256i hi = _mm256_min_epi32(_mm256_cvtps_epi32(_mm256_loadu_ps(pColor + 8)), _mm256_set1_epi32(0x0ffff));
		return cvt2xEpi32Epu16(lo, hi);
	}

	// Read color values from T* and return 8 x packed int
	inline static __m256i read8PackedInt(const WORD* const pColor)
	{
		return _mm256_cvtepu16_epi32(_mm_loadu_epi16(pColor));
	}
	inline static __m256i read8PackedInt(const std::uint32_t* const pColor)
	{
		return _mm256_srli_epi32(_mm256_loadu_epi32(pColor), 16); // Shift 16 bits right while shifting in zeros.
	}
	inline static __m256i read8PackedInt(const float* const pColor)
	{
		return _mm256_min_epi32(_mm256_cvtps_epi32(_mm256_loadu_ps(pColor)), _mm256_set1_epi32(0x0ffff));
	}

	// Accumulate packed single newColor to T* oldColor
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const std::uint16_t *const pOutputBitmap, const bool fastload) noexcept;
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const std::uint32_t* const pOutputBitmap, const bool fastload) noexcept;
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const float *const pOutputBitmap, const bool fastload) noexcept;

	// Store accumulated color
	static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, std::uint16_t *const pOutputBitmap, const bool faststore) noexcept;
	static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, std::uint32_t *const pOutputBitmap, const bool faststore) noexcept;
	static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, float *const pOutputBitmap, const bool faststore) noexcept;

	template <class T>
	static float accumulateSingleColorValue(const size_t outNdx, const float newColor, const int mask, const T* const pOutputBitmap) noexcept;

	// Shift and rotate for whole AVX vectors.

	template <int N>
	inline static __m256i shiftRightEpi8(const __m256i x) noexcept
	{
		return _mm256_alignr_epi8(_mm256_zextsi128_si256(_mm256_extracti128_si256(x, 1)), x, N);
	}
	template <int N>
	inline static __m256i shiftRightEpi32(const __m256i x) noexcept
	{
		return shiftRightEpi8<4 * N>(x);
	}

	template <int N>
	inline static __m256i shiftLeftEpi32(const __m256i x) noexcept
	{
		static_assert(N == 1);
		return _mm256_permutevar8x32_epi32(x, _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6));
	}

	template <int N>
	inline static __m256i rotateRightEpi8(const __m256i x) noexcept
	{
		if constexpr (N > 16)
			return _mm256_alignr_epi8(x, _mm256_permute2x128_si256(x, x, 1), N - 16);
		else
			return _mm256_alignr_epi8(_mm256_permute2x128_si256(x, x, 1), x, N);
	}
	template <int N>
	inline static __m256i rotateRightEpi32(const __m256i x) noexcept
	{
		return rotateRightEpi8<4 * N>(x);
	}

	// Extract for PS has a strange signature (returns int), so we write an own version.
	template <int NDX>
	inline static float extractPs(const __m128 ps)
	{
		static_assert(NDX >= 0 && NDX < 4);
		const int extracted = _mm_extract_ps(ps, NDX); // Note: extract_ps(x, i) returns the bits of the i-th float as int.
		return reinterpret_cast<const float&>(extracted);
	}
};

#endif
