#pragma once 
#include "avx_cfa.h" 
#include "avx_simd_factory.h"
#include "avx_includes.h"

class AvxEntropy;
class CPixelTransform;
class CTaskInfo;
class CBackgroundCalibration;
class AvxStacking
{
private:
	friend class Avx256Stacking;
	friend class NonAvxStacking;

	typedef __m512 VectorElementType;
	typedef std::vector<VectorElementType> VectorType;

	int lineStart, lineEnd, colEnd;
	int width, height;
	int resultWidth, resultHeight;
	size_t vectorsPerLine;
	VectorType xCoordinates;
	VectorType yCoordinates;
	VectorType redPixels;
	VectorType greenPixels;
	VectorType bluePixels;
	const CMemoryBitmap& inputBitmap;
	CMemoryBitmap& tempBitmap;
	AvxCfaProcessing avxCfa;
	AvxEntropy& entropyData;
	bool avx2Enabled;
public:
	AvxStacking() = delete;
	AvxStacking(const int lStart, const int lEnd, const CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const class DSSRect& resultRect, AvxEntropy& entrdat);
	AvxStacking(const AvxStacking&) = default;
	AvxStacking(AvxStacking&&) = delete;
	AvxStacking& operator=(const AvxStacking&) = delete;

	void init(const int lStart, const int lEnd);

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, std::shared_ptr<CMemoryBitmap> outputBitmap, const int pixelSizeMultiplier);
private:
	void resizeColorVectors(const size_t nrVectors);
	inline float* row(VectorType& vector, const size_t rowIndex)
	{
		return reinterpret_cast<float*>(&vector[rowIndex * this->vectorsPerLine]);
	}
	inline const float* row(const VectorType& vector, const size_t rowIndex) const
	{
		return reinterpret_cast<const float*>(&vector[rowIndex * this->vectorsPerLine]);
	}
};

class Avx256Stacking : public SimdFactory<Avx256Stacking>
{
private:
	friend class AvxStacking;
	friend class SimdFactory<Avx256Stacking>;

	AvxStacking& stackData;
	Avx256Stacking(AvxStacking& sd) : stackData{ sd } {}

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, std::shared_ptr<CMemoryBitmap>, const int pixelSizeMultiplier);

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

class NonAvxStacking : public SimdFactory<NonAvxStacking>
{
private:
	friend class AvxStacking;
	friend class SimdFactory<NonAvxStacking>;

	AvxStacking& stackData;
	NonAvxStacking(AvxStacking& sd) : stackData{ sd } {}

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, std::shared_ptr<CMemoryBitmap> outputBitmap, const int pixelSizeMultiplier);
};
