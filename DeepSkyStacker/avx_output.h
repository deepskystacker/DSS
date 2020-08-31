#pragma once

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