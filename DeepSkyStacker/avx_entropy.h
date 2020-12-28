#pragma once 

#include "BitmapExt.h" 
#include <vector> 

class AvxEntropy
{
private:
	typedef std::vector<float> EntropyVectorType;
	CMemoryBitmap& inputBitmap;
	bool avxReady;
public:
	AvxEntropy() = delete;
	AvxEntropy(CMemoryBitmap& inputbm);
	AvxEntropy(const AvxEntropy&) = default;
	AvxEntropy(AvxEntropy&&) = delete;
	AvxEntropy& operator=(const AvxEntropy&) = delete;

	int calcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies);
private:
	template <class T>
	int doCalcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies);
};
