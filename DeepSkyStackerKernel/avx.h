#pragma once
#include "avx_cfa.h"

class AvxEntropy;
class CPixelTransform;
class CTaskInfo;
class CBackgroundCalibration;
class AvxStacking
{
private:
	int lineStart, lineEnd, colEnd;
	int width, height;
	int resultWidth, resultHeight;
	std::vector<__m256> xCoordinates;
	std::vector<__m256> yCoordinates;
	std::vector<__m256> redPixels;
	std::vector<__m256> greenPixels;
	std::vector<__m256> bluePixels;
	CMemoryBitmap& inputBitmap;
	CMemoryBitmap& tempBitmap;
	AvxCfaProcessing avxCfa;
	AvxEntropy& entropyData;
public:
	AvxStacking() = delete;
	AvxStacking(int lStart, int lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const DSSRect& resultRect, AvxEntropy& entrdat);
	AvxStacking(const AvxStacking&) = default;
	AvxStacking(AvxStacking&&) = delete;
	AvxStacking& operator=(const AvxStacking&) = delete;

	void init(const int lStart, const int lEnd);

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const int pixelSizeMultiplier);
private:
	void resizeColorVectors(const size_t nrVectors);

	template <class T>
	int doStack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const int pixelSizeMultiplier);

	int pixelTransform(const CPixelTransform& pixelTransformDef);

	template <class T, class LoopFunction, class InterpolParam>
	int backgroundCalibLoop(const LoopFunction& loopFunc, const class AvxSupport& avxSupport, const InterpolParam& redParams, const InterpolParam& greenParams, const InterpolParam& blueParams);

	template <class T>
	int backgroundCalibration(const CBackgroundCalibration& backgroundCalibrationDef);

	template <bool ISRGB, bool ENTROPY, class T>
	int pixelPartitioning();

	template <bool ISRGB>
	void getAvxEntropy(__m256& redEntropy, __m256& greenEntropy, __m256& blueEntropy, const __m256i xIndex, const int row);
};
