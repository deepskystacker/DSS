#pragma once

#include "BitmapExt.h"
#include <vector>


#if defined(AVX_INTRINSICS) && defined(_M_X64)
class AvxOutputComposition
{
private:
	CMultiBitmap& inputBitmap;
	CMemoryBitmap* pOutput;
public:
	AvxOutputComposition() = delete;
	AvxOutputComposition(CMultiBitmap& mBitmap, CMemoryBitmap* pOut);
	AvxOutputComposition(const AvxOutputComposition&) = delete;
	AvxOutputComposition(AvxOutputComposition&&) = delete;
	AvxOutputComposition& AvxOutputComposition::operator=(const AvxOutputComposition&) = delete;

	int compose(const int line, std::vector<void*> const& lineAddresses);
private:
	int processKappaSigma(const int line, std::vector<void*> const& lineAddresses, C96BitFloatColorBitmap* const pOutputBitmap);
};

#else

class AvxOutputComposition
{
public:
	AvxOutputComposition(CMultiBitmap&, CMemoryBitmap*) {}
	int compose(const int, std::vector<void*> const&)
	{
		return 1;
	}
};

#endif
