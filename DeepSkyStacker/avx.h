#pragma once

#include "avx_cfa.h"
#include "avx_entropy.h"
#include "PixelTransform.h"
#include "StackingTasks.h"
#include "BackgroundCalibration.h"
#include "BitmapExt.h"
#include <vector>
#include <tuple>


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
	AvxEntropy& entropyData;
public:
	AvxStacking() = delete;
	AvxStacking(long lStart, long lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const CRect& resultRect, AvxEntropy& entrdat);
	AvxStacking(const AvxStacking&) = default;
	AvxStacking(AvxStacking&&) = delete;
	AvxStacking& AvxStacking::operator=(const AvxStacking&) = delete;

	void init(const long lStart, const long lEnd);

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier);
private:
	void resizeColorVectors(const size_t nrVectors);

	template <class T>
	int doStack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier);

	int pixelTransform(const CPixelTransform& pixelTransformDef);

	template <class T, class LoopFunction, class InterpolParam>
	int backgroundCalibLoop(const LoopFunction& loopFunc, const class AvxSupport& avxSupport, const InterpolParam& redParams, const InterpolParam& greenParams, const InterpolParam& blueParams);

	template <class T>
	int backgroundCalibration(const CBackgroundCalibration& backgroundCalibrationDef);

	template <bool ISRGB, bool ENTROPY, class T>
	int pixelPartitioning();

	template <bool ISRGB>
	void getAvxEntropy(__m256& redEntropy, __m256& greenEntropy, __m256& blueEntropy, const __m256i xIndex, const int row);
};


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

	static bool checkAvx2CpuSupport() noexcept;
	static bool checkSimdAvailability() noexcept;

	template <size_t ElementSize>
	inline static size_t numberOfAvxVectors(const size_t width)
	{
		static_assert(ElementSize == 1 || ElementSize == 2 || ElementSize == 4 || ElementSize == 8);
		return width == 0 ? 0 : (width - 1) * ElementSize / sizeof(__m256i) + 1;
	}

	// When returning from AVX-code to non-AVX-code we should zero the upper 128 bits of all ymm registers. 
	// Otherwise old Intel CPUs could suffer from performance degradations. 
	template <class T>
	inline static T zeroUpper(const T returnValue)
	{
		static_assert(std::is_integral<T>::value);
		_mm256_zeroupper();
		return returnValue;
	}

	// SIMD functions

	inline static __m256 wordToPackedFloat(const __m128i x) noexcept
	{
		return _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(x));
	}

	inline static std::tuple<__m256d, __m256d, __m256d, __m256d> wordToPackedDouble(const __m256i x) noexcept
	{
		const __m256i i1 = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(x));
		const __m256i i2 = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(x, 1));
		return {
			_mm256_cvtepi32_pd(_mm256_castsi256_si128(i1)),
			_mm256_cvtepi32_pd(_mm256_extracti128_si256(i1, 1)),
			_mm256_cvtepi32_pd(_mm256_castsi256_si128(i2)),
			_mm256_cvtepi32_pd(_mm256_extracti128_si256(i2, 1))
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
		const __m256d corr1 = _mm256_add_pd(_mm256_set1_pd(static_cast<double>(0x100000000ULL)), d1); // UINTMAX - x (Note: 'add_pd' is correct!)
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
		const __m256i epu16 = _mm256_packus_epi32(epi32, _mm256_castsi128_si256(_mm256_extracti128_si256(epi32, 1))); // (?, ?, ?, ?, a7, a6, a5, a4, a7, a6, a5, a4, a3, a2, a1, a0)
		// Upper lane is now wrong and useless.
		return _mm256_castsi256_si128(epu16);
	}

	inline static __m128i cvtPsEpu16(const __m256 x) noexcept
	{
		const __m256i epi32 = _mm256_cvtps_epi32(x);
		return cvtEpi32Epu16(epi32);
	}

	inline static __m256i cvtPsEpu32(const __m256 x) noexcept
	{
		// x >= INTMAX + 1
		const __m256 mask = _mm256_cmp_ps(x, _mm256_set1_ps(2147483648.0f), 29); // 29 = _CMP_GE_OQ (greater or equal, ordered, quiet)
		const __m256 corr = _mm256_sub_ps(x, _mm256_set1_ps(4294967296.0f));
		return _mm256_cvttps_epi32(_mm256_blendv_ps(x, corr, mask));
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
	inline static std::tuple<__m256, __m256> read16PackedSingle(const std::uint32_t* const pColor) noexcept
	{
		return {
			_mm256_cvtepi32_ps(_mm256_srli_epi32(_mm256_loadu_epi32(pColor), 16)), // Shift 16 bits right while shifting in zeros.
			_mm256_cvtepi32_ps(_mm256_srli_epi32(_mm256_loadu_epi32(pColor + 8), 16))
		};
	}
	inline static std::tuple<__m256, __m256> read16PackedSingle(const unsigned long* const pColor) noexcept
	{
		static_assert(sizeof(unsigned long) == sizeof(std::uint32_t));
		return read16PackedSingle(reinterpret_cast<const std::uint32_t*>(pColor));
	}
	inline static std::tuple<__m256, __m256> read16PackedSingle(const float* const pColor) noexcept
	{
		return { _mm256_loadu_ps(pColor), _mm256_loadu_ps(pColor + 8) };
	}


	// Read 16 color values from T* with stride 
	inline static std::tuple<__m256, __m256> read16PackedSingleStride(const WORD* const pColor, const int stride) noexcept
	{
		const __m256i ndx = _mm256_mullo_epi32(_mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7), _mm256_set1_epi32(stride));
		const __m256i v1 = _mm256_i32gather_epi32((const int*)pColor, ndx, 2);
		const __m256i v2 = _mm256_i32gather_epi32((const int*)pColor, _mm256_add_epi32(ndx, _mm256_set1_epi32(8 * stride)), 2); // 8, 9, 10, 11, 12, 13, 14, 15 
		return {
			_mm256_cvtepi32_ps(_mm256_blend_epi16(v1, _mm256_setzero_si256(), 0xaa)),
			_mm256_cvtepi32_ps(_mm256_blend_epi16(v2, _mm256_setzero_si256(), 0xaa))
		};
	}
	// Note: ***** DOES NOT SHIFT 16 BITS RIGHT! ***** 
	inline static std::tuple<__m256, __m256> read16PackedSingleStride(const std::uint32_t* const pColor, const int stride) noexcept
	{
		const __m256i ndx = _mm256_mullo_epi32(_mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7), _mm256_set1_epi32(stride));
		const __m256i v1 = _mm256_i32gather_epi32((const int*)pColor, ndx, 4);
		const __m256i v2 = _mm256_i32gather_epi32((const int*)pColor, _mm256_add_epi32(ndx, _mm256_set1_epi32(8 * stride)), 4);
		return {
			_mm256_cvtepi32_ps(v1),
			_mm256_cvtepi32_ps(v2)
		};
	}
	inline static std::tuple<__m256, __m256> read16PackedSingleStride(const unsigned long* const pColor, const int stride) noexcept
	{
		static_assert(sizeof(unsigned long) == sizeof(std::uint32_t));
		return read16PackedSingleStride(reinterpret_cast<const std::uint32_t*>(pColor), stride);
	}
	inline static std::tuple<__m256, __m256> read16PackedSingleStride(const float* const pColor, const int stride) noexcept
	{
		const __m256i ndx1 = _mm256_mullo_epi32(_mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7), _mm256_set1_epi32(stride));
		const __m256i ndx2 = _mm256_add_epi32(ndx1, _mm256_set1_epi32(8 * stride));
		return { _mm256_i32gather_ps(pColor, ndx1, 4), _mm256_i32gather_ps(pColor, ndx2, 4) };
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
	inline static __m256i read16PackedShort(const unsigned long* const pColor)
	{
		static_assert(sizeof(unsigned long) == sizeof(std::uint32_t));
		return read16PackedShort(reinterpret_cast<const std::uint32_t*>(pColor));
	}
	inline static __m256i read16PackedShort(const float* const pColor)
	{
		// Min with 65536 not needed, because cvt2xEpi32Epu16 applies unsigned saturation to 16 bits.
//		const __m256i lo = _mm256_min_epi32(_mm256_cvtps_epi32(_mm256_loadu_ps(pColor)), _mm256_set1_epi32(0x0ffff));
//		const __m256i hi = _mm256_min_epi32(_mm256_cvtps_epi32(_mm256_loadu_ps(pColor + 8)), _mm256_set1_epi32(0x0ffff));
		const __m256i loEpi32 = _mm256_cvtps_epi32(_mm256_loadu_ps(pColor));
		const __m256i hiEpi32 = _mm256_cvtps_epi32(_mm256_loadu_ps(pColor + 8));
		return cvt2xEpi32Epu16(loEpi32, hiEpi32);
	}

	// Read color values from T* and return 2 x 8 x packed int
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const WORD* const pColor)
	{
		const __m256i epi16 = _mm256_loadu_si256((const __m256i*)pColor);
		return {
			_mm256_cvtepu16_epi32(_mm256_castsi256_si128(epi16)),
			_mm256_cvtepu16_epi32(_mm256_extracti128_si256(epi16, 1))
		};
	}
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const std::uint32_t* const pColor)
	{
		return {
			_mm256_srli_epi32(_mm256_loadu_si256((const __m256i*)pColor), 16), // Shift 16 bits right while shifting in zeros (divide by 65536).
			_mm256_srli_epi32(_mm256_loadu_si256(((const __m256i*)pColor) + 1), 16)
		};
	}
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const unsigned long* const pColor)
	{
		static_assert(sizeof(unsigned long) == sizeof(std::uint32_t));
		return read16PackedInt(reinterpret_cast<const std::uint32_t*>(pColor));
	}
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const float* const pColor)
	{
		return {
			_mm256_min_epi32(_mm256_cvttps_epi32(_mm256_loadu_ps(pColor)), _mm256_set1_epi32(0x0000ffff)),
			_mm256_min_epi32(_mm256_cvttps_epi32(_mm256_loadu_ps(pColor + 8)), _mm256_set1_epi32(0x0000ffff))
		};
	}
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const double* const pColor)
	{
		throw "read16PackedInt(const double*) is not implemented!";
	}
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const double* const pColor, const __m256d scalingFactor)
	{
		const __m128i lo1 = _mm256_cvttpd_epi32(_mm256_mul_pd(_mm256_loadu_pd(pColor), scalingFactor));
		const __m128i hi1 = _mm256_cvttpd_epi32(_mm256_mul_pd(_mm256_loadu_pd(pColor + 4), scalingFactor));

		const __m128i lo2 = _mm256_cvttpd_epi32(_mm256_mul_pd(_mm256_loadu_pd(pColor + 8), scalingFactor));
		const __m128i hi2 = _mm256_cvttpd_epi32(_mm256_mul_pd(_mm256_loadu_pd(pColor + 12), scalingFactor));

		return {
			_mm256_min_epi32(_mm256_set_m128i(hi1, lo1), _mm256_set1_epi32(0x0000ffff)),
			_mm256_min_epi32(_mm256_set_m128i(hi2, lo2), _mm256_set1_epi32(0x0000ffff))
		};
	}

	// Accumulate packed single newColor to T* oldColor
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const std::uint16_t *const pOutputBitmap, const bool fastload) noexcept;
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const unsigned long* const pOutputBitmap, const bool fastload) noexcept;
	static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const float *const pOutputBitmap, const bool fastload) noexcept;

	// Store accumulated color
	static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, std::uint16_t *const pOutputBitmap, const bool faststore) noexcept;
	static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, unsigned long *const pOutputBitmap, const bool faststore) noexcept;
	static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, float *const pOutputBitmap, const bool faststore) noexcept;

	template <class T>
	static T accumulateSingleColorValue(const size_t outNdx, const float newColor, const int mask, const T* const pOutputBitmap) noexcept;

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
	inline static __m256i shiftLeftEpi8(const __m256i x) noexcept
	{
		static_assert(N >= 0 && N <= 16);
		return _mm256_alignr_epi8(x, _mm256_permute2x128_si256(x, x, 8), 16 - N);
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
		return _mm_cvtss_f32(_mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(ps), NDX * 4)));
	}
	template <>
	inline static float extractPs<0>(const __m128 ps)
	{
		return _mm_cvtss_f32(ps);
	}
};
