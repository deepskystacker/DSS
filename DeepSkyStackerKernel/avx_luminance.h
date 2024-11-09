#pragma once

#include "BitmapBase.h"
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#endif
class AvxLuminance
{
private:
	const CMemoryBitmap& inputBitmap;
	CMemoryBitmap& outputBitmap;
	bool avxReady;
public:
	AvxLuminance() = delete;
	AvxLuminance(const CMemoryBitmap& inputbm, CMemoryBitmap& outbm);
	AvxLuminance(const AvxLuminance&) = default;
	AvxLuminance(AvxLuminance&&) = delete;
	AvxLuminance& operator=(const AvxLuminance&) = delete;

	int computeLuminanceBitmap(const size_t lineStart, const size_t lineEnd);
private:
	template <class T>
	int doComputeLuminance(const size_t lineStart, const size_t lineEnd);

	template <class T>
	static std::tuple<__m256d, __m256d, __m256d, __m256d> colorLuminance(const T* const pRed, const T* const pGreen, const T* const pBlue);

	template <class T>
	static std::tuple<__m256d, __m256d, __m256d, __m256d> greyLuminance(const T* const pGray);
};
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif