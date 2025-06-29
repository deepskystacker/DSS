#pragma once 
/****************************************************************************
**
** Copyright (C) 2020, 2025 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
#include "avx_cfa.h" 
#include "avx_simd_factory.h"
#include "avx_includes.h"
#include "boost/align/aligned_allocator.hpp"

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
	typedef std::vector<VectorElementType, boost::alignment::aligned_allocator<VectorElementType, 64> > VectorType;

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

class Avx256Stacking
{
private:
	friend class AvxStacking;

	AvxStacking& stackData;
	Avx256Stacking(AvxStacking& sd) : stackData{ sd } {}

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, std::shared_ptr<CMemoryBitmap>, const int pixelSizeMultiplier);

	template <class T>
	int doStack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const int pixelSizeMultiplier);

	int pixelTransform(const CPixelTransform& pixelTransformDef);

	template <class T, class LoopFunction, class InterpolParam>
	int backgroundCalibLoop(const LoopFunction& loopFunc, const class AvxBitmapUtil& avxSupport, const InterpolParam& redParams, const InterpolParam& greenParams, const InterpolParam& blueParams);

	template <class T>
	int backgroundCalibration(const CBackgroundCalibration& backgroundCalibrationDef);

	template <bool ISRGB, bool ENTROPY, class T>
	int pixelPartitioning();

	template <bool ISRGB>
	void getAvxEntropy(__m256& redEntropy, __m256& greenEntropy, __m256& blueEntropy, const __m256i xIndex, const int row);
};

class NonAvxStacking
{
private:
	friend class AvxStacking;

	AvxStacking& stackData;
	NonAvxStacking(AvxStacking& sd) : stackData{ sd } {}

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, std::shared_ptr<CMemoryBitmap> outputBitmap, const int pixelSizeMultiplier);
};
