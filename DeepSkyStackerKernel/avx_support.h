#pragma once

#include "avx_simd_check.h"
#include "avx_simd_factory.h"
#include "cfa.h"
#include "ColorBitmap.h"
#include "GrayBitmap.h"

class AvxSupport
{
private:
	const CMemoryBitmap& bitmap;
	CMemoryBitmap* pBitmap;

	template <class T>
	auto* getColorPtr() { return dynamic_cast<CColorBitmapT<T>*>(pBitmap); }
	template <class T>
	auto* getGrayPtr() { return dynamic_cast<CGrayBitmapT<T>*>(pBitmap); }
	template <class T>
	const auto* getColorPtr() const { return dynamic_cast<const CColorBitmapT<T>*>(&bitmap); }
	template <class T>
	const auto* getGrayPtr() const { return dynamic_cast<const CGrayBitmapT<T>*>(&bitmap); }

	int getNrChannels() const;
public:
	AvxSupport(CMemoryBitmap& b) noexcept;
	AvxSupport(const CMemoryBitmap& b) noexcept;

	bool isColorBitmap() const;
	template <class T> bool isColorBitmapOfType() const;
	bool isMonochromeBitmap() const;
	template <class T> bool isMonochromeBitmapOfType() const;
	template <class T> bool isMonochromeCfaBitmapOfType() const;
	bool isColorBitmapOrCfa() const;

	CFATYPE getCfaType() const;

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

	template <class ElementType, class VectorElementType>
	inline static size_t numberOfAvxVectors(const size_t width)
	{
		static_assert(sizeof(ElementType) == 1 || sizeof(ElementType) == 2 || sizeof(ElementType) == 4 || sizeof(ElementType) == 8);
		return width == 0 ? 0 : ((width - 1) * sizeof(ElementType)) / sizeof(VectorElementType) + 1;
	}

	// When returning from AVX-code to non-AVX-code we should zero the upper 128 bits of all ymm registers.
	// Otherwise old Intel CPUs could suffer from performance degradations.
	template <class T>
	inline static T zeroUpper(const T returnValue)
	{
		static_assert(std::is_integral<T>::value);
#if defined (_MSC_VER) // Simde doesn't know that, so we only call it on Windows builds where we use the intrinsics.
		_mm256_zeroupper();
#endif
		return returnValue;
	}
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#endif
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

	inline static __m128i cvtTruncatePsEpu16(const __m256 x) noexcept
	{
		const __m256i epi32 = _mm256_cvttps_epi32(x);
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
		const __m256i highBit = _mm256_set1_epi16(std::uint16_t{ 0x8000 });
		return _mm256_cmpgt_epi16(_mm256_xor_si256(a, highBit), _mm256_xor_si256(b, highBit));
	};

	// Read color values from T* and return 2 x 8 packed single.
	inline static std::tuple<__m256, __m256> read16PackedSingle(const std::uint16_t* const pColor) noexcept
	{
		const __m256i icolor = _mm256_loadu_si256((const __m256i*)pColor);
		const __m256 lo8 = wordToPackedFloat(_mm256_castsi256_si128(icolor));
		const __m256 hi8 = wordToPackedFloat(_mm256_extracti128_si256(icolor, 1));
		return { lo8, hi8 };
	};
	inline static std::tuple<__m256, __m256> read16PackedSingle(const std::uint32_t* const pColor) noexcept
	{
		return {
			_mm256_cvtepi32_ps(_mm256_srli_epi32(_mm256_loadu_si256((const __m256i*)pColor), 16)), // Shift 16 bits right while shifting in zeros.
			_mm256_cvtepi32_ps(_mm256_srli_epi32(_mm256_loadu_si256(((const __m256i*)pColor) + 1), 16))
		};
	}
	inline static std::tuple<__m256, __m256> read16PackedSingle(const float* const pColor) noexcept
	{
		return { _mm256_loadu_ps(pColor), _mm256_loadu_ps(pColor + 8) };
	}

	// Read 16 color values from T* with stride
	inline static std::tuple<__m256, __m256> read16PackedSingleStride(const std::uint16_t* const pColor, const int stride) noexcept
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
	inline static __m256i read16PackedShort(const std::uint16_t* const pColor)
	{
		return _mm256_loadu_si256((const __m256i*)pColor);
	}
	inline static __m256i read16PackedShort(const std::uint32_t* const pColor)
	{
		const __m256i lo = _mm256_srli_epi32(_mm256_loadu_si256((const __m256i*)pColor), 16); // Shift 16 bits right while shifting in zeros.
		const __m256i hi = _mm256_srli_epi32(_mm256_loadu_si256(((const __m256i*)pColor) + 1), 16);
		return cvt2xEpi32Epu16(lo, hi);
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
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const std::uint16_t* const pColor)
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
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const float* const pColor)
	{
		return {
			_mm256_min_epi32(_mm256_cvttps_epi32(_mm256_loadu_ps(pColor)), _mm256_set1_epi32(0x0000ffff)),
			_mm256_min_epi32(_mm256_cvttps_epi32(_mm256_loadu_ps(pColor + 8)), _mm256_set1_epi32(0x0000ffff))
		};
	}
	inline static std::tuple<__m256i, __m256i> read16PackedInt(const double*)
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
	inline static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const std::uint16_t* const pOutputBitmap, const bool fastload) noexcept
	{
		__m256i tempColor = _mm256_undefined_si256();
		if (fastload)
			tempColor = _mm256_cvtepu16_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(pOutputBitmap + _mm256_cvtsi256_si32(outNdx))));
		else
		{
			// Gather with scale factor of 2 -> outNdx points to WORDs. Load these 8 WORDs and interpret them as epi32.
			const __m256i tempColorAsI16 = _mm256_mask_i32gather_epi32(_mm256_setzero_si256(), reinterpret_cast<const int*>(pOutputBitmap), outNdx, mask, 2);
			// The high words of each epi32 color value are wrong -> we null them out.
			tempColor = _mm256_blend_epi16(tempColorAsI16, _mm256_setzero_si256(), 0xaa);
		}
		const __m256 accumulatedColor = _mm256_fmadd_ps(colorValue, fraction, _mm256_cvtepi32_ps(tempColor)); // tempColor = 8 int in the range [0, 65535]
		return _mm256_min_ps(accumulatedColor, _mm256_set1_ps(static_cast<float>(0x0000ffff)));
	}

	inline static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const std::uint32_t* const pOutputBitmap, const bool fastload) noexcept
	{
		static_assert(sizeof(unsigned int) == sizeof(std::uint32_t));

		const __m256 scalingFactor = _mm256_set1_ps(65536.0f);

		const __m256i tempColor = fastload
			? _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pOutputBitmap + _mm256_cvtsi256_si32(outNdx)))
			: _mm256_mask_i32gather_epi32(_mm256_setzero_si256(), reinterpret_cast<const int*>(pOutputBitmap), outNdx, mask, 4);
		const __m256 accumulatedColor = _mm256_fmadd_ps(colorValue, _mm256_mul_ps(fraction, scalingFactor), cvtEpu32Ps(tempColor));
		return _mm256_min_ps(accumulatedColor, _mm256_set1_ps(4294967040.0f)); // This constant is the next lower float value below UINTMAX.
	}

	inline static __m256 accumulateColorValues(const __m256i outNdx, const __m256 colorValue, const __m256 fraction, const __m256i mask, const float* const pOutputBitmap, const bool fastload) noexcept
	{
		const __m256 tempColor = fastload
			? _mm256_loadu_ps(pOutputBitmap + _mm256_cvtsi256_si32(outNdx))
			: _mm256_mask_i32gather_ps(_mm256_setzero_ps(), pOutputBitmap, outNdx, _mm256_castsi256_ps(mask), 4);
		return _mm256_fmadd_ps(colorValue, fraction, tempColor);
	}

	// Store accumulated color

	inline static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, std::uint16_t* const pOutputBitmap, const bool faststore) noexcept
	{
		if (faststore)
			_mm_storeu_si128(reinterpret_cast<__m128i*>(pOutputBitmap + _mm256_cvtsi256_si32(outNdx)), cvtPsEpu16(colorValue));
		else
		{
			const int iMask = _mm256_movemask_epi8(mask);
			const auto checkWrite = [pOutputBitmap, iMask](const int mask, const size_t ndx, const float color) -> void
				{
					if ((iMask & mask) != 0)
						pOutputBitmap[ndx] = static_cast<std::uint16_t>(color);
				};
			__m128 color = _mm256_castps256_ps128(colorValue);
			checkWrite(1, _mm256_cvtsi256_si32(outNdx), AvxSupport::extractPs<0>(color)); // Note: extract_ps(x, i) returns the bits of the i-th float as int.
			checkWrite(1 << 4, _mm256_extract_epi32(outNdx, 1), AvxSupport::extractPs<1>(color));
			checkWrite(1 << 8, _mm256_extract_epi32(outNdx, 2), AvxSupport::extractPs<2>(color));
			checkWrite(1 << 12, _mm256_extract_epi32(outNdx, 3), AvxSupport::extractPs<3>(color));
			color = _mm256_extractf128_ps(colorValue, 1);
			checkWrite(1 << 16, _mm256_extract_epi32(outNdx, 4), AvxSupport::extractPs<0>(color));
			checkWrite(1 << 20, _mm256_extract_epi32(outNdx, 5), AvxSupport::extractPs<1>(color));
			checkWrite(1 << 24, _mm256_extract_epi32(outNdx, 6), AvxSupport::extractPs<2>(color));
			checkWrite(1 << 28, _mm256_extract_epi32(outNdx, 7), AvxSupport::extractPs<3>(color));
		}
	}

	inline static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, std::uint32_t* const pOutputBitmap, const bool faststore) noexcept
	{
		static_assert(sizeof(unsigned int) == sizeof(std::uint32_t));

		if (faststore)
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(pOutputBitmap + _mm256_cvtsi256_si32(outNdx)), cvtPsEpu32(colorValue));
		else
		{
			const int iMask = _mm256_movemask_epi8(mask);
			const auto checkWrite = [pOutputBitmap, iMask](const int mask, const size_t ndx, const float color) -> void
				{
					if ((iMask & mask) != 0)
						pOutputBitmap[ndx] = static_cast<std::uint32_t>(color);
				};
			__m128 color = _mm256_castps256_ps128(colorValue);
			checkWrite(1, _mm256_cvtsi256_si32(outNdx), AvxSupport::extractPs<0>(color));
			checkWrite(1 << 4, _mm256_extract_epi32(outNdx, 1), AvxSupport::extractPs<1>(color));
			checkWrite(1 << 8, _mm256_extract_epi32(outNdx, 2), AvxSupport::extractPs<2>(color));
			checkWrite(1 << 12, _mm256_extract_epi32(outNdx, 3), AvxSupport::extractPs<3>(color));
			color = _mm256_extractf128_ps(colorValue, 1);
			checkWrite(1 << 16, _mm256_extract_epi32(outNdx, 4), AvxSupport::extractPs<0>(color));
			checkWrite(1 << 20, _mm256_extract_epi32(outNdx, 5), AvxSupport::extractPs<1>(color));
			checkWrite(1 << 24, _mm256_extract_epi32(outNdx, 6), AvxSupport::extractPs<2>(color));
			checkWrite(1 << 28, _mm256_extract_epi32(outNdx, 7), AvxSupport::extractPs<3>(color));
		}
	}

	inline static void storeColorValue(const __m256i outNdx, const __m256 colorValue, const __m256i mask, float* const pOutputBitmap, const bool faststore) noexcept
	{
		if (faststore)
			_mm256_storeu_ps(pOutputBitmap + _mm256_cvtsi256_si32(outNdx), colorValue);
		else
		{
			const int iMask = _mm256_movemask_epi8(mask);
			const auto checkWrite = [pOutputBitmap, iMask](const int mask, const size_t ndx, const float color) -> void
				{
					if ((iMask & mask) != 0)
						pOutputBitmap[ndx] = color;
				};
			__m128 color = _mm256_castps256_ps128(colorValue);
			checkWrite(1, _mm256_cvtsi256_si32(outNdx), AvxSupport::extractPs<0>(color)); // Note: extract_ps(x, i) returns the bits of the i-th float as int.
			checkWrite(1 << 4, _mm256_extract_epi32(outNdx, 1), AvxSupport::extractPs<1>(color));
			checkWrite(1 << 8, _mm256_extract_epi32(outNdx, 2), AvxSupport::extractPs<2>(color));
			checkWrite(1 << 12, _mm256_extract_epi32(outNdx, 3), AvxSupport::extractPs<3>(color));
			color = _mm256_extractf128_ps(colorValue, 1);
			checkWrite(1 << 16, _mm256_extract_epi32(outNdx, 4), AvxSupport::extractPs<0>(color));
			checkWrite(1 << 20, _mm256_extract_epi32(outNdx, 5), AvxSupport::extractPs<1>(color));
			checkWrite(1 << 24, _mm256_extract_epi32(outNdx, 6), AvxSupport::extractPs<2>(color));
			checkWrite(1 << 28, _mm256_extract_epi32(outNdx, 7), AvxSupport::extractPs<3>(color));
		}
	}

	template <class T>
	inline static T accumulateSingleColorValue(const size_t outNdx, const float newColor, const int mask, const T* const pOutputBitmap) noexcept
	{
		if (mask == 0)
			return T{ 0 };

		if constexpr (std::is_same<T, std::uint16_t>::value)
		{
			const float accumulatedColor = static_cast<float>(pOutputBitmap[outNdx]) + newColor;
			return static_cast<std::uint16_t>(std::min(accumulatedColor, static_cast<float>(std::numeric_limits<T>::max())));
		}

		if constexpr (std::is_same<T, std::uint32_t>::value)
		{
			const float accumulatedColor = static_cast<float>(pOutputBitmap[outNdx]) + newColor * 65536.0f;
			return static_cast<std::uint32_t>(std::min(accumulatedColor, 4294967040.0f)); // The next lower float value below UINTMAX.
		}

		if constexpr (std::is_same<T, float>::value)
			return pOutputBitmap[outNdx] + newColor;
	}

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
	inline static __m256i shiftRightVarEpi8(const __m256i x, const int N) noexcept
	{
		switch (N) {
		case 0: return x;
		case 1: return shiftRightEpi8<1>(x);
		case 2: return shiftRightEpi8<2>(x);
		case 3: return shiftRightEpi8<3>(x);
		case 4: return shiftRightEpi8<4>(x);
		case 5: return shiftRightEpi8<5>(x);
		case 6: return shiftRightEpi8<6>(x);
		case 7: return shiftRightEpi8<7>(x);
		case 8: return shiftRightEpi8<8>(x);
		case 9: return shiftRightEpi8<9>(x);
		case 10: return shiftRightEpi8<10>(x);
		case 11: return shiftRightEpi8<11>(x);
		case 12: return shiftRightEpi8<12>(x);
		case 13: return shiftRightEpi8<13>(x);
		case 14: return shiftRightEpi8<14>(x);
		case 15: return shiftRightEpi8<15>(x);
		case 16: return shiftRightEpi8<16>(x);
		case 17: return shiftRightEpi8<17>(x);
		case 18: return shiftRightEpi8<18>(x);
		case 19: return shiftRightEpi8<19>(x);
		case 20: return shiftRightEpi8<20>(x);
		case 21: return shiftRightEpi8<21>(x);
		case 22: return shiftRightEpi8<22>(x);
		case 23: return shiftRightEpi8<23>(x);
		case 24: return shiftRightEpi8<24>(x);
		case 25: return shiftRightEpi8<25>(x);
		case 26: return shiftRightEpi8<26>(x);
		case 27: return shiftRightEpi8<27>(x);
		case 28: return shiftRightEpi8<28>(x);
		case 29: return shiftRightEpi8<29>(x);
		case 30: return shiftRightEpi8<30>(x);
		case 31: return shiftRightEpi8<31>(x);
		}
		return _mm256_setzero_si256();
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
		return _mm256_permutevar8x32_epi32(x, _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6)); // permutevar8x32: dst[0] = x[idx[0]]; dst[1] = x[idx[1]]; ...
	}

	template <int N>
	inline static __m256 shiftRightPs(const __m256 x) noexcept
	{
		static_assert(N == 1);
		return _mm256_castsi256_ps(shiftRightEpi32<1>(_mm256_castps_si256(x)));
	}

	inline static __m256i shl1Epi16(const __m256i x, const int value) noexcept // CPU cycles: 2/2 + 3/1 + 1/1 = 6/4
	{
		const __m128i src = _mm_insert_epi16(_mm_undefined_si128(), value, 7); // Value to the lowest 128bit-lane [highest element].
		const __m256i b = _mm256_permute2x128_si256(x, _mm256_zextsi128_si256(src), 0x02); // [lo(x), src]
		return _mm256_alignr_epi8(x, b, 16 - 2); // [x, b] >> (14 bytes) lane-by-lane
	}
	inline static __m256i shr1Epi16(const __m256i x, const int value) noexcept // CPU cycles: 2/1 + 3/1 + 1/1 = 6/3
	{
		const __m128i src = _mm_cvtsi32_si128(value); // Value to 128-bit-lane [lowest element].
		const __m256i a = _mm256_permute2x128_si256(x, _mm256_zextsi128_si256(src), 0x21); // [src, hi(x)]
		return _mm256_alignr_epi8(a, x, 2); // [a, x] >> (2 bytes) lane-by-lane
	}


	template <int N>
	inline static __m256i rotateRightEpi8(const __m256i x) noexcept
	{
		if constexpr (N == 0 || N == 32)
			return x;
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
	template <int N>
	inline static __m256 rotateRightPs(const __m256 x) noexcept
	{
		if constexpr (N == 0 || N == 8)
			return x;
		return _mm256_castsi256_ps(rotateRightEpi32<N>(_mm256_castps_si256(x)));
	}

	// Extract for PS has a strange signature (returns int), so we write an own version.
	template <int NDX>
	inline static float extractPs(const __m128 ps)
	{
		static_assert(NDX >= 0 && NDX < 4);
		if constexpr (NDX == 0)
			return _mm_cvtss_f32(ps);
		else
			return _mm_cvtss_f32(_mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(ps), NDX * 4)));
	}
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
};

#if defined(_MSC_VER) // It seems that only Microsoft does not provide operator[] for the SIMD data types, but uses arrays m256_f32[8] or m256_f64[4].

namespace {
	template <typename T, typename TEST>
	concept IsSame = std::is_same_v<std::remove_cvref_t<T>, TEST>;
}

inline constexpr auto accessSimdElementConst(auto const& simdVector, const size_t elementIndex)
{
	using T = decltype(simdVector);

	if constexpr (IsSame<T, __m256>)
		return static_cast<float>(simdVector.m256_f32[elementIndex]);
	else if constexpr (IsSame<T, __m256d>)
		return static_cast<double>(simdVector.m256d_f64[elementIndex]);
	else
		static_assert(false, "accessSimdElementConst: SIMD type not implemented");
}

inline __m256 avxLog(const __m256 a)
{
	return _mm256_log_ps(a);
}

inline __m256 avxPow(const __m256 a, const __m256 b)
{
	return _mm256_pow_ps(a, b);
}

#elif defined (__GNUC__)

inline constexpr auto accessSimdElementConst(auto const& simdVector, const size_t elementIndex)
{
	return simdVector[elementIndex];
}

inline __m256 avxLog(__m256 a)
{
	constexpr size_t N = sizeof(a) / sizeof(a[0]);
	for (size_t i = 0; i < N; ++i)
		a[i] = logf(a[i]);
	return a;
}

inline __m256 avxPow(__m256 b, const __m256 e)
{
	constexpr size_t N = sizeof(b) / sizeof(b[0]);
	for (size_t i = 0; i < N; ++i)
		b[i] = powf(b[i], e[i]);
	return b;
}

#endif
