#include "stdafx.h"
#include "avx_includes.h"
#include "avx_cfa.h"
#include "avx_support.h"

AvxCfaProcessing::AvxCfaProcessing(const size_t lineStart, const size_t lineEnd, const CMemoryBitmap& inputbm) :
	redPixels{},
	greenPixels{},
	bluePixels{},
	inputBitmap{ inputbm },
	vectorsPerLine{ 0 },
	avxReady{ AvxSimdCheck::checkSimdAvailability() }
{
	init(lineStart, lineEnd);
}

void AvxCfaProcessing::init(const size_t lineStart, const size_t lineEnd) // You should be sure that lineEnd >= lineStart!
{
	const size_t height = lineEnd - lineStart;
	vectorsPerLine = AvxSupport::numberOfAvxVectors<std::uint16_t, VectorElementType>(inputBitmap.Width());
	const size_t nrVectors = vectorsPerLine * height;
	if (nrVectors != 0 && AvxSupport{ inputBitmap }.isMonochromeCfaBitmapOfType<std::uint16_t>())
	{
		redPixels.resize(nrVectors);
		greenPixels.resize(nrVectors);
		bluePixels.resize(nrVectors);
	}
}

int AvxCfaProcessing::interpolate(const size_t lineStart, const size_t lineEnd, const int pixelSizeMultiplier)
{
	if (!avxReady)
		return 1;
	if (pixelSizeMultiplier != 1)
		return 1;
	if (!AvxSupport{ inputBitmap }.isMonochromeCfaBitmapOfType<std::uint16_t>())
		return 1;

	return AvxSupport{ inputBitmap }.getCfaType() == CFATYPE_RGGB
		? Avx256CfaProcessing{ *this }.interpolateGrayCFA2Color<0>(lineStart, lineEnd)
		: Avx256CfaProcessing{ *this }.interpolateGrayCFA2Color<1>(lineStart, lineEnd);
}


// ***********************************************
// ************ AVX-256 interpolation ************
// ***********************************************

template <int RG_ROW>
int Avx256CfaProcessing::interpolateGrayCFA2Color(const size_t lineStart, const size_t lineEnd)
{
	if (const auto* const p{ dynamic_cast<const CGrayBitmapT<std::uint16_t>*>(&avxData.inputBitmap) })
	{
		if (!p->IsCFA())
			return 1;
	}
	else
		return 1;
	if ((lineStart % 2) != 0) // Must start with an even index (RG-line).
		return 2;

	const size_t width = avxData.inputBitmap.Width();
	const size_t height = avxData.inputBitmap.Height();
	if (width < 64 || height < 8) // AVX makes no sense for super-small arrays.
		return 2;

	constexpr size_t VecSize = sizeof(__m256i) / sizeof(std::uint16_t);
	static_assert(VecSize == 16);
	const size_t nrVectors = width / VecSize;
	const unsigned int remainingPixels = width % VecSize;

	__m256i thisRowCurrent, thisRowNext; // Current and next vector of current row.
	__m256i prevRowCurrent, prevRowNext; // ... of previous row.
	__m256i nextRowCurrent, nextRowNext; // ... of following row.
	int thisRowLast, prevRowLast, nextRowLast; // Last value of the previous line.

	const AvxSupport avxSupport{ avxData.inputBitmap };
	const std::uint16_t* pGray = avxSupport.grayPixels<std::uint16_t>().data() + lineStart * width;
	std::uint16_t* pRed = avxData.redCfaLine(0);
	std::uint16_t* pGreen = avxData.greenCfaLine(0);
	std::uint16_t* pBlue = avxData.blueCfaLine(0);
	std::int16_t prevRowMask = lineStart == 0 ? 0x0 : -1;

	const auto extract0 = [](const __m256i x) -> int { return _mm256_cvtsi256_si32(x); };
	const auto extract15 = [](const __m256i x) -> int { return _mm256_extract_epi16(x, 15); };
	const auto storePixel = [&pRed, &pGreen, &pBlue](const auto rgbVec) -> void
	{
		const auto [r, g, b] = rgbVec;
		_mm256_storeu_si256((__m256i*)pRed, r);
		_mm256_storeu_si256((__m256i*)pGreen, g);
		_mm256_storeu_si256((__m256i*)pBlue, b);
	};
	const auto loadRemainingPixels = [remainingPixels](const std::uint16_t* pGray, const bool doLoad) -> __m256i
	{
		if (!doLoad)
			return _mm256_setzero_si256();
		__m256i vec = _mm256_setzero_si256();
		memcpy(&vec, pGray, remainingPixels * sizeof(pGray[0]));
		return vec;
	};
	const auto storeRemainingPixel = [remainingPixels, &pRed, &pGreen, &pBlue](const auto rgbVec) -> void
	{
		const auto [r, g, b] = rgbVec;
		memcpy(pRed, &r, remainingPixels * sizeof(pRed[0]));
		memcpy(pGreen, &g, remainingPixels * sizeof(pGreen[0]));
		memcpy(pBlue, &b, remainingPixels * sizeof(pBlue[0]));
	};

	for (size_t row = 0, lineNdx = lineStart; lineNdx < lineEnd; ++row, ++lineNdx)
	{
		//                  curr. row prev. pix/curr. row curr. vector/curr. row next pix./prev. row prev. pix./prev. row current vec./prev. row next pix.  ...
		const auto debayer = [row](const int thisP, const __m256i thisCurr, const int thisN,
			const int prevP, const __m256i prevCurr, const int prevN, const int nextP, const __m256i nextCurr, const int nextN) -> std::tuple<__m256i, __m256i, __m256i>
		{
			const __m256i currRight = AvxSupport::shl1Epi16(thisCurr, thisP); // One pixel right is shifting to higher address. This is a shiftLeft(...).
			const __m256i currLeft = AvxSupport::shr1Epi16(thisCurr, thisN);
			const __m256i prevRight = AvxSupport::shl1Epi16(prevCurr, prevP);
			const __m256i prevLeft = AvxSupport::shr1Epi16(prevCurr, prevN);
			const __m256i nextRight = AvxSupport::shl1Epi16(nextCurr, nextP);
			const __m256i nextLeft = AvxSupport::shr1Epi16(nextCurr, nextN);
			const __m256i LRinterpol = _mm256_avg_epu16(currRight, currLeft);
			const __m256i UDinterpol = _mm256_avg_epu16(prevCurr, nextCurr);
			const __m256i crossInterpol = _mm256_avg_epu16(_mm256_avg_epu16(prevRight, prevLeft), _mm256_avg_epu16(nextRight, nextLeft));
			const __m256i greenInterpol = _mm256_avg_epu16(UDinterpol, LRinterpol);

			// RGGB pattern: RG_ROW==0 -> even row -> RG-line
			// GBRG pattern: RG_ROW==1 ->  odd row -> RG-line
			if ((row % 2) == RG_ROW)
			{
				const __m256i red = _mm256_blend_epi16(thisCurr, LRinterpol, 0xaa); // 0b10101010 = 0xaa
				const __m256i green = _mm256_blend_epi16(greenInterpol, thisCurr, 0xaa);
				const __m256i blue = _mm256_blend_epi16(crossInterpol, UDinterpol, 0xaa);
				return { red, green, blue };
			}
			else // GB-line
			{
				const __m256i red = _mm256_blend_epi16(UDinterpol, crossInterpol, 0xaa);
				const __m256i green = _mm256_blend_epi16(thisCurr, greenInterpol, 0xaa);
				const __m256i blue = _mm256_blend_epi16(LRinterpol, thisCurr, 0xaa);
				return { red, green, blue };
			}
		};

		const std::int16_t nextRowMask = lineNdx == height - 1 ? 0x0 : -1; // Prevent loading pixels of the following row if the current one is already the last.

		thisRowCurrent = _mm256_setzero_si256();
		thisRowNext = _mm256_loadu_si256((__m256i*)pGray);
		thisRowLast = 0;
		prevRowCurrent = _mm256_setzero_si256();
		prevRowNext = _mm256_maskload_epi32((int*)(pGray - width), _mm256_set1_epi32(prevRowMask)); // Load entire vector or nothing.
		prevRowLast = 0;
		nextRowCurrent = _mm256_setzero_si256();
		nextRowNext = _mm256_maskload_epi32((int*)(pGray + width), _mm256_set1_epi32(nextRowMask)); // Load entire vector or nothing.
		nextRowLast = 0;

		pRed = avxData.redCfaLine(row);
		pGreen = avxData.greenCfaLine(row);
		pBlue = avxData.blueCfaLine(row);

		for (size_t n = 1; n < nrVectors; ++n, pGray += VecSize, pRed += VecSize, pGreen += VecSize, pBlue += VecSize) // nrVectors - 1 iterations
		{
			thisRowLast = extract15(thisRowCurrent);
			thisRowCurrent = thisRowNext;
			thisRowNext = _mm256_loadu_si256((__m256i*)(pGray + VecSize));
			prevRowLast = extract15(prevRowCurrent);
			prevRowCurrent = prevRowNext;
			prevRowNext = _mm256_maskload_epi32((int*)(pGray + VecSize - width), _mm256_set1_epi32(prevRowMask));
			nextRowLast = extract15(nextRowCurrent);
			nextRowCurrent = nextRowNext;
			nextRowNext = _mm256_maskload_epi32((int*)(pGray + VecSize + width), _mm256_set1_epi32(nextRowMask));

			storePixel(debayer(thisRowLast, thisRowCurrent, extract0(thisRowNext), prevRowLast, prevRowCurrent, extract0(prevRowNext), nextRowLast, nextRowCurrent, extract0(nextRowNext)));
		}

		thisRowLast = extract15(thisRowCurrent);
		thisRowCurrent = thisRowNext;
		prevRowLast = extract15(prevRowCurrent);
		prevRowCurrent = prevRowNext;
		nextRowLast = extract15(nextRowCurrent);
		nextRowCurrent = nextRowNext;

		if (remainingPixels == 0)
		{
			// Last full vector
			storePixel(debayer(thisRowLast, thisRowCurrent, 0, prevRowLast, prevRowCurrent, 0, nextRowLast, nextRowCurrent, 0));
			pGray += VecSize;
		}
		else
		{
			thisRowNext = loadRemainingPixels(pGray + VecSize, true);
			prevRowNext = loadRemainingPixels(pGray + VecSize - width, prevRowMask != 0);
			nextRowNext = loadRemainingPixels(pGray + VecSize + width, nextRowMask != 0);

			// Last full vector
			storePixel(debayer(thisRowLast, thisRowCurrent, extract0(thisRowNext), prevRowLast, prevRowCurrent, extract0(prevRowNext), nextRowLast, nextRowCurrent, extract0(nextRowNext)));

			pGray += VecSize + remainingPixels;
			pRed += VecSize;
			pGreen += VecSize;
			pBlue += VecSize;

			// Process last few pixels that are less than a full vector.
			storeRemainingPixel(debayer(extract15(thisRowCurrent), thisRowNext, 0, extract15(prevRowCurrent), prevRowNext, 0, extract15(nextRowCurrent), nextRowNext, 0));
		}

		prevRowMask = -1;
	}
	return 0;
}
