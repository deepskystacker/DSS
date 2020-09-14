#pragma once

#include "BitmapExt.h"
#include <vector>


#if defined(AVX_INTRINSICS) && defined(_M_X64)
class AvxOutputComposition
{
private:
	CMultiBitmap& inputBitmap;
	CMemoryBitmap& outputBitmap;
	bool avxReady;
public:
	AvxOutputComposition() = delete;
	AvxOutputComposition(CMultiBitmap& mBitmap, CMemoryBitmap& outputbm);
	AvxOutputComposition(const AvxOutputComposition&) = delete;
	AvxOutputComposition(AvxOutputComposition&&) = delete;
	AvxOutputComposition& AvxOutputComposition::operator=(const AvxOutputComposition&) = delete;

	int compose(const int line, std::vector<void*> const& lineAddresses);
private:
	template <class INPUTTYPE, class OUTPUTTYPE>
	static bool bitmapColorOrGray(const CMultiBitmap& bitmap)  noexcept;

	int processKappaSigma(const int line, std::vector<void*> const& lineAddresses);
	template <class T>
	int doProcessKappaSigma(const int line, std::vector<void*> const& lineAddresses);

	int processAutoAdaptiveWeightedAverage(const int line, std::vector<void*> const& lineAddresses);
	template <class T>
	int doProcessAutoAdaptiveWeightedAverage(const int line, std::vector<void*> const& lineAddresses);
};

#else

class AvxOutputComposition
{
public:
	AvxOutputComposition(CMultiBitmap& mBitmap, CMemoryBitmap& outputbm) {}
	int compose(const int, std::vector<void*> const&)
	{
		return 1;
	}
};

#endif
