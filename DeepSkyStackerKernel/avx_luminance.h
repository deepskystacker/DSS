#pragma once

#include "BitmapBase.h"

class AvxLuminance
{
private:
	const CMemoryBitmap& inputBitmap;
	CMemoryBitmap& outputBitmap;
	bool avxReady;
public:
	AvxLuminance() = delete;
	explicit AvxLuminance(const CMemoryBitmap& inputbm, CMemoryBitmap& outbm);
	AvxLuminance(const AvxLuminance&) = default;
	AvxLuminance(AvxLuminance&&) = delete;
	AvxLuminance& operator=(const AvxLuminance&) = delete;

	int computeLuminanceBitmap(const size_t lineStart, const size_t lineEnd);
private:
	template <class T>
	int doComputeLuminance(const size_t lineStart, const size_t lineEnd);
};
