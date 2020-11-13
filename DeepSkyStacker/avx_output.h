#pragma once

#include "BitmapExt.h"
#include <vector>


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

	template <class T>
	static float convertToFloat(const T value) noexcept;

	int processKappaSigma(const int line, std::vector<void*> const& lineAddresses);
	template <class T>
	int doProcessKappaSigma(const int line, std::vector<void*> const& lineAddresses);

	int processAutoAdaptiveWeightedAverage(const int line, std::vector<void*> const& lineAddresses);
	template <class T>
	int doProcessAutoAdaptiveWeightedAverage(const int line, std::vector<void*> const& lineAddresses);
};
