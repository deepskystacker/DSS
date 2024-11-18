#pragma once
#include "avx_includes.h"

/*
* Class for
* 1. Calculation of the entropy data of the squares,
* 2. Managing the entropy coverage (the coverage bitmap itself is calculated while stacking with the class AvxStacking).
*/
class CMemoryBitmap;
class CEntropyInfo;
class AvxEntropy
{
private:
	friend class Avx256Stacking;
	friend class NonAvxStacking;
	friend class AvxAccumulation;

	typedef std::vector<float> EntropyVectorType;
	typedef std::vector<__m512> EntropyLayerVectorType;

	const CMemoryBitmap& inputBitmap;
	const CEntropyInfo& entropyInfo;
	CMemoryBitmap* pEntropyCoverage;
	EntropyLayerVectorType redEntropyLayer;
	EntropyLayerVectorType greenEntropyLayer;
	EntropyLayerVectorType blueEntropyLayer;
	bool avxReady;
public:
	AvxEntropy() = delete;
	AvxEntropy(const CMemoryBitmap& inputbm, const CEntropyInfo& entrinfo, CMemoryBitmap* entropycov);
	AvxEntropy(const AvxEntropy&) = default;
	AvxEntropy(AvxEntropy&&) = delete;
	AvxEntropy& operator=(const AvxEntropy&) = delete;
#if defined(UNIT_TESTS) 
	float* getRedEntropyLayer() { return reinterpret_cast<float*>(redEntropyLayer.data()); }
#endif 

	int calcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies);
private:
	template <class T>
	int doCalcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies);
};
