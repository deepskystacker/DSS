#pragma once

#include "BitmapExt.h"
#include <tuple>

#if defined(AVX_INTRINSICS) && defined(_M_X64)

class AvxLuminance
{
private:
	CMemoryBitmap& inputBitmap;
	CMemoryBitmap& outputBitmap;
	bool avxReady;
public:
	AvxLuminance() = delete;
	AvxLuminance(CMemoryBitmap& inputbm, CMemoryBitmap& outbm) noexcept;
	AvxLuminance(const AvxLuminance&) = delete;
	AvxLuminance(AvxLuminance&&) = delete;
	AvxLuminance& operator=(const AvxLuminance&) = delete;

	int computeLuminanceBitmap(const size_t lineStart, const size_t lineEnd);
private:
	template <class T>
	int doComputeLuminance(const size_t lineStart, const size_t lineEnd);

	inline static std::tuple<__m256d, __m256d, __m256d, __m256d> multiplyPd(const std::tuple<__m256d, __m256d, __m256d, __m256d> x, const double factor) noexcept
	{
		const __m256d f = _mm256_set1_pd(factor);
		return {
			_mm256_mul_pd(std::get<0>(x), f),
			_mm256_mul_pd(std::get<1>(x), f),
			_mm256_mul_pd(std::get<2>(x), f),
			_mm256_mul_pd(std::get<3>(x), f)
		};
	}

	template <class T>
	static std::tuple<__m256d, __m256d, __m256d, __m256d> colorLuminance(const T *const pRed, const T *const pGreen, const T *const pBlue);

	template <class T>
	static std::tuple<__m256d, __m256d, __m256d, __m256d> greyLuminance(const T* const pGrey);

	template <class T>
	static constexpr double normalizationFactor() noexcept;
};

#else

class AvxLuminance
{
};

#endif