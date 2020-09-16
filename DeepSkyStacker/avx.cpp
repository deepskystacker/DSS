#include "StdAfx.h"
#include "avx.h"

#if defined(AVX_INTRINSICS) && defined(_M_X64)

#include <immintrin.h>


AvxStacking::AvxStacking(long lStart, long lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const CRect& resultRect) :
	lineStart{ lStart }, lineEnd{ lEnd }, colEnd{ inputbm.Width() },
	width{ colEnd }, height{ lineEnd - lineStart },
	resultWidth{ resultRect.Width() }, resultHeight{ resultRect.Height() },
	xCoordinates(width >= 0 && height >= 0 ? width * height : 0),
	yCoordinates(width >= 0 && height >= 0 ? width * height : 0),
	redPixels(width >= 0 && height >= 0 ? width * height : 0),
	greenPixels{},
	bluePixels{},
	inputBitmap{ inputbm },
	tempBitmap{ tempbm }
{
	if (width < 0 || height < 0)
		throw std::invalid_argument("End index smaller than start index for line or column of AvxStacking");

	if (AvxSupport{inputBitmap}.isColorBitmap())
	{
		greenPixels.resize(width * height);
		bluePixels.resize(width * height);
	}
}

int AvxStacking::stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier) {
	if (!AvxSupport::checkCpuFeatures())
		return 1;

	if (doStack<WORD>(pixelTransformDef, taskInfo, backgroundCalibrationDef, pixelSizeMultiplier) == 0)
		return 0;
	if (doStack<std::uint32_t>(pixelTransformDef, taskInfo, backgroundCalibrationDef, pixelSizeMultiplier) == 0)
		return 0;
	if (doStack<float>(pixelTransformDef, taskInfo, backgroundCalibrationDef, pixelSizeMultiplier) == 0)
		return 0;
	return 1;
}

template <class T>
int AvxStacking::doStack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier)
{
	if (pixelSizeMultiplier != 1 || pixelTransformDef.m_lPixelSizeMultiplier != 1)
		return 1;

	// Check input bitmap.
	const AvxSupport avxInputSupport{ inputBitmap };
	if (!avxInputSupport.isColorBitmapOfType<T>() && !avxInputSupport.isMonochromeBitmapOfType<T>())
		return 1;

	// Check output (temp) bitmap.
	const AvxSupport avxTempSupport{ tempBitmap };
	if (!avxTempSupport.isColorBitmapOfType<T>() && !avxTempSupport.isMonochromeBitmapOfType<T>())
		return 1;

	// Entropy not yet supported.
	if (taskInfo.m_Method == MBP_ENTROPYAVERAGE)
		return 1;


	if (pixelTransform(pixelTransformDef) != 0)
		return 1;
	if (backgroundCalibration<T>(backgroundCalibrationDef) != 0)
		return 1;
	if (avxTempSupport.isColorBitmap() && pixelDispatchAndAccumulate<true, T>() != 0)
		return 1;
	if (avxTempSupport.isMonochromeBitmap() && pixelDispatchAndAccumulate<false, T>() != 0)
		return 1;

	return 0;
};

int AvxStacking::pixelTransform(const CPixelTransform& pixelTransformDef) {
	const CBilinearParameters& bilinearParams = pixelTransformDef.m_BilinearParameters;

	// Number of vectors with 8 pixels each to process.
	const int nrVectors = width / 8;
	const float fxShift = static_cast<float>(pixelTransformDef.m_fXShift);
	const float fyShift = static_cast<float>(pixelTransformDef.m_fYShift);
	const __m256 fxShiftVec = _mm256_set1_ps(fxShift);

	// Superfast version if no transformation required: indices = coordinates.
	if (bilinearParams.Type == TT_BILINEAR && (
		bilinearParams.fXWidth == 1.0f && bilinearParams.fYWidth == 1.0f &&
		bilinearParams.a1 == 1.0f && bilinearParams.b2 == 1.0f &&
		bilinearParams.a0 == 0.0f && bilinearParams.a2 == 0.0f && bilinearParams.a3 == 0.0f &&
		bilinearParams.b0 == 0.0f && bilinearParams.b1 == 0.0f && bilinearParams.b3 == 0.0f
		))
	{
		for (int row = 0; row < height; ++row)
		{
			const float yShifted = static_cast<float>(lineStart + row) + fyShift;
			float* pXLine = &xCoordinates.at(row * width);
			float* pYLine = &yCoordinates.at(row * width);
			__m256i xline = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);

			for (int counter = 0; counter < nrVectors; ++counter, pXLine += 8, pYLine += 8)
			{
				const __m256 fxline = _mm256_cvtepi32_ps(xline);
				xline = _mm256_add_epi32(xline, _mm256_set1_epi32(8));
				_mm256_storeu_ps(pXLine, _mm256_add_ps(fxline, fxShiftVec));
				_mm256_storeu_ps(pYLine, _mm256_set1_ps(yShifted));
			}
			// Remaining pixels after last complete vector.
			for (int n = nrVectors * 8; n < colEnd; ++n, ++pXLine, ++pYLine)
			{
				*pXLine = static_cast<float>(n) + fxShift;
				*pYLine = yShifted;
			}
		}
		return 0;
	}

	const float fa0 = static_cast<float>(bilinearParams.a0);
	const float fa1 = static_cast<float>(bilinearParams.a1);
	const float fa2 = static_cast<float>(bilinearParams.a2);
	const float fa3 = static_cast<float>(bilinearParams.a3);
	const float fb0 = static_cast<float>(bilinearParams.b0);
	const float fb1 = static_cast<float>(bilinearParams.b1);
	const float fb2 = static_cast<float>(bilinearParams.b2);
	const float fb3 = static_cast<float>(bilinearParams.b3);
	const __m256 xWidth = _mm256_set1_ps(static_cast<float>(bilinearParams.fXWidth));
	const __m256 yWidth = _mm256_set1_ps(static_cast<float>(bilinearParams.fYWidth));
	const __m256 a0 = _mm256_set1_ps(fa0);
	const __m256 a1 = _mm256_set1_ps(fa1);
	const __m256 a2 = _mm256_set1_ps(fa2);
	const __m256 a3 = _mm256_set1_ps(fa3);
	const __m256 b0 = _mm256_set1_ps(fb0);
	const __m256 b1 = _mm256_set1_ps(fb1);
	const __m256 b2 = _mm256_set1_ps(fb2);
	const __m256 b3 = _mm256_set1_ps(fb3);
	const __m256 fyShiftVec = _mm256_set1_ps(fyShift);

	const auto linearTransformX = [&a0, &a1, &a2, &a3](const __m256 x, const __m256 y, const __m256 xy) -> __m256
	{
		return _mm256_fmadd_ps(a3, xy, _mm256_fmadd_ps(a2, y, _mm256_fmadd_ps(a1, x, a0))); // (((a0 + a1*x) + a2*y) + a3*x*y)
	};
	const auto linearTransformY = [&b0, &b1, &b2, &b3](const __m256 x, const __m256 y, const __m256 xy) -> __m256
	{
		return _mm256_fmadd_ps(b3, xy, _mm256_fmadd_ps(b2, y, _mm256_fmadd_ps(b1, x, b0))); // (((b0 + b1*x) + b2*y) + b3*x*y)
	};

	if (bilinearParams.Type == TT_BILINEAR)
	{
		for (int row = 0; row < height; ++row)
		{
			const float y = static_cast<float>(lineStart + row) / static_cast<float>(bilinearParams.fYWidth);
			const __m256 vy = _mm256_set1_ps(y);
			float* pXLine = &xCoordinates.at(row * width);
			float* pYLine = &yCoordinates.at(row * width);
			// Vector with x-indices of the current 8 pixels of the line.
			__m256i xline = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);

			for (int counter = 0; counter < nrVectors; ++counter, pXLine += 8, pYLine += 8)
			{
				const __m256 vx = _mm256_div_ps(_mm256_cvtepi32_ps(xline), xWidth);
				// Indices of the next 8 pixels.
				xline = _mm256_add_epi32(xline, _mm256_set1_epi32(8));

				const __m256 xy = _mm256_mul_ps(vx, vy);
				// X- and y-coordinates for the bilinear transformation of the current 8 pixels.
				const __m256 xr = linearTransformX(vx, vy, xy);
				const __m256 yr = linearTransformY(vx, vy, xy);

				// Save result.
				_mm256_storeu_ps(pXLine, _mm256_fmadd_ps(xr, xWidth, fxShiftVec)); // xr * fxWidth + fxShift
				_mm256_storeu_ps(pYLine, _mm256_fmadd_ps(yr, yWidth, fyShiftVec)); // yr * fyWidth + fyShift
			}
			// Remaining pixels of the line.
			for (int n = nrVectors * 8; n < colEnd; ++n, ++pXLine, ++pYLine)
			{
				const float x = static_cast<float>(n) / static_cast<float>(bilinearParams.fXWidth);
				*pXLine = static_cast<float>(bilinearParams.fXWidth) * (fa0 + fa1 * x + fa2 * y + fa3 * x * y) + fxShift;
				*pYLine = static_cast<float>(bilinearParams.fYWidth) * (fb0 + fb1 * x + fb2 * y + fb3 * x * y) + fyShift;
			}
		}
		return 0;
	}

	const float fa4 = static_cast<float>(bilinearParams.a4);
	const float fa5 = static_cast<float>(bilinearParams.a5);
	const float fa6 = static_cast<float>(bilinearParams.a6);
	const float fa7 = static_cast<float>(bilinearParams.a7);
	const float fa8 = static_cast<float>(bilinearParams.a8);
	const float fb4 = static_cast<float>(bilinearParams.b4);
	const float fb5 = static_cast<float>(bilinearParams.b5);
	const float fb6 = static_cast<float>(bilinearParams.b6);
	const float fb7 = static_cast<float>(bilinearParams.b7);
	const float fb8 = static_cast<float>(bilinearParams.b8);
	const __m256 a4 = _mm256_set1_ps(fa4);
	const __m256 a5 = _mm256_set1_ps(fa5);
	const __m256 a6 = _mm256_set1_ps(fa6);
	const __m256 a7 = _mm256_set1_ps(fa7);
	const __m256 a8 = _mm256_set1_ps(fa8);
	const __m256 b4 = _mm256_set1_ps(fb4);
	const __m256 b5 = _mm256_set1_ps(fb5);
	const __m256 b6 = _mm256_set1_ps(fb6);
	const __m256 b7 = _mm256_set1_ps(fb7);
	const __m256 b8 = _mm256_set1_ps(fb8);

	const auto squaredTransformX = [&a4, &a5, &a6, &a7, &a8](const __m256 xLinear, const __m256 x2, const __m256 y2, const __m256 x2y, const __m256 xy2, const __m256 x2y2) -> __m256
	{
		return _mm256_fmadd_ps(a8, x2y2, _mm256_fmadd_ps(a7, xy2, _mm256_fmadd_ps(a6, x2y, _mm256_fmadd_ps(a5, y2, _mm256_fmadd_ps(a4, x2, xLinear))))); // (((((xl + a4*x2) + a5*y2) + a6*x2y) + a7*xy2) + a8*x2y2)
	};
	const auto squaredTransformY = [&b4, &b5, &b6, &b7, &b8](const __m256 yLinear, const __m256 x2, const __m256 y2, const __m256 x2y, const __m256 xy2, const __m256 x2y2) -> __m256
	{
		return _mm256_fmadd_ps(b8, x2y2, _mm256_fmadd_ps(b7, xy2, _mm256_fmadd_ps(b6, x2y, _mm256_fmadd_ps(b5, y2, _mm256_fmadd_ps(b4, x2, yLinear))))); // (((((yl + b4*x2) + b5*y2) + b6*x2y) + b7*xy2) + b8*x2y2)
	};

	if (bilinearParams.Type == TT_BISQUARED)
	{

		for (int row = 0; row < height; ++row) {
			const float y = static_cast<float>(lineStart + row) / static_cast<float>(bilinearParams.fYWidth);
			const __m256 vy = _mm256_set1_ps(y);
			float* pXLine = &xCoordinates.at(row * width);
			float* pYLine = &yCoordinates.at(row * width);
			__m256i xline = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);

			for (int counter = 0; counter < nrVectors; ++counter, pXLine += 8, pYLine += 8)
			{
				const __m256 vx = _mm256_div_ps(_mm256_cvtepi32_ps(xline), xWidth);
				xline = _mm256_add_epi32(xline, _mm256_set1_epi32(8));

				// Linear part
				const __m256 xy = _mm256_mul_ps(vx, vy);
				const __m256 rlx = linearTransformX(vx, vy, xy);
				const __m256 rly = linearTransformY(vx, vy, xy);

				// Square parameters
				const __m256 x2 = _mm256_mul_ps(vx, vx);
				const __m256 y2 = _mm256_mul_ps(vy, vy);
				const __m256 x2y = _mm256_mul_ps(x2, vy);
				const __m256 xy2 = _mm256_mul_ps(vx, y2);
				const __m256 x2y2 = _mm256_mul_ps(x2, y2);

				// The bisqared transformation.
				const __m256 xr = squaredTransformX(rlx, x2, y2, x2y, xy2, x2y2);
				const __m256 yr = squaredTransformY(rly, x2, y2, x2y, xy2, x2y2);

				_mm256_storeu_ps(pXLine, _mm256_fmadd_ps(xr, xWidth, fxShiftVec));
				_mm256_storeu_ps(pYLine, _mm256_fmadd_ps(yr, yWidth, fyShiftVec));
			}
			// Remaining pixels of the line.
			for (int n = nrVectors * 8; n < colEnd; ++n, ++pXLine, ++pYLine)
			{
				const float x = static_cast<float>(n) / static_cast<float>(bilinearParams.fXWidth);
				*pXLine = static_cast<float>(bilinearParams.fXWidth) * (fa0 + fa1 * x + fa2 * y + fa3 * x * y + fa4 * x * x + fa5 * y * y + fa6 * x * x * y + fa7 * x * y * y + fa8 * x * x * y * y) + fxShift;
				*pYLine = static_cast<float>(bilinearParams.fYWidth) * (fb0 + fb1 * x + fb2 * y + fb3 * x * y + fb4 * x * x + fb5 * y * y + fb6 * x * x * y + fb7 * x * y * y + fb8 * x * x * y * y) + fyShift;
			}
		}
		return 0;
	}

	const float fa9 = static_cast<float>(bilinearParams.a9);
	const float fa10 = static_cast<float>(bilinearParams.a10);
	const float fa11 = static_cast<float>(bilinearParams.a11);
	const float fa12 = static_cast<float>(bilinearParams.a12);
	const float fa13 = static_cast<float>(bilinearParams.a13);
	const float fa14 = static_cast<float>(bilinearParams.a14);
	const float fa15 = static_cast<float>(bilinearParams.a15);
	const float fb9 = static_cast<float>(bilinearParams.b9);
	const float fb10 = static_cast<float>(bilinearParams.b10);
	const float fb11 = static_cast<float>(bilinearParams.b11);
	const float fb12 = static_cast<float>(bilinearParams.b12);
	const float fb13 = static_cast<float>(bilinearParams.b13);
	const float fb14 = static_cast<float>(bilinearParams.b14);
	const float fb15 = static_cast<float>(bilinearParams.b15);
	const __m256 a9 = _mm256_set1_ps(fa9);
	const __m256 a10 = _mm256_set1_ps(fa10);
	const __m256 a11 = _mm256_set1_ps(fa11);
	const __m256 a12 = _mm256_set1_ps(fa12);
	const __m256 a13 = _mm256_set1_ps(fa13);
	const __m256 a14 = _mm256_set1_ps(fa14);
	const __m256 a15 = _mm256_set1_ps(fa15);
	const __m256 b9 = _mm256_set1_ps(fb9);
	const __m256 b10 = _mm256_set1_ps(fb10);
	const __m256 b11 = _mm256_set1_ps(fb11);
	const __m256 b12 = _mm256_set1_ps(fb12);
	const __m256 b13 = _mm256_set1_ps(fb13);
	const __m256 b14 = _mm256_set1_ps(fb14);
	const __m256 b15 = _mm256_set1_ps(fb15);

	const auto cubicTransformX = [&a9, &a10, &a11, &a12, &a13, &a14, &a15](
		const __m256 xSquared, const __m256 x3, const __m256 y3, const __m256 x3y, const __m256 xy3, const __m256 x3y2, const __m256 x2y3, const __m256 x3y3) -> __m256
	{
		// (((((squarePart + a9*x3) + a10*y3) + a11*x3y) + a12*xy3) + a13*x3y2) + a14*x2y3) + a15*x3y3)
		return _mm256_fmadd_ps(a15, x3y3, _mm256_fmadd_ps(a14, x2y3, _mm256_fmadd_ps(a13, x3y2, _mm256_fmadd_ps(a12, xy3, _mm256_fmadd_ps(a11, x3y, _mm256_fmadd_ps(a10, y3, _mm256_fmadd_ps(a9, x3, xSquared)))))));
	};
	const auto cubicTransformY = [&b9, &b10, &b11, &b12, &b13, &b14, &b15](
		const __m256 ySquared, const __m256 x3, const __m256 y3, const __m256 x3y, const __m256 xy3, const __m256 x3y2, const __m256 x2y3, const __m256 x3y3) -> __m256
	{
		// (((((squarePart + b9*x3) + b10*y3) + b11*x3y) + b12*xy3) + b13*x3y2) + b14*x2y3) + b15*x3y3)
		return _mm256_fmadd_ps(b15, x3y3, _mm256_fmadd_ps(b14, x2y3, _mm256_fmadd_ps(b13, x3y2, _mm256_fmadd_ps(b12, xy3, _mm256_fmadd_ps(b11, x3y, _mm256_fmadd_ps(b10, y3, _mm256_fmadd_ps(b9, x3, ySquared)))))));
	};

	if (bilinearParams.Type == TT_BICUBIC)
    {
		for (int row = 0; row < height; ++row)
		{
			const float y = static_cast<float>(lineStart + row) / static_cast<float>(bilinearParams.fYWidth);
			const __m256 vy = _mm256_set1_ps(y);
			float* pXLine = &xCoordinates.at(row * width);
			float* pYLine = &yCoordinates.at(row * width);
			__m256i xline = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);

			for (int counter = 0; counter < nrVectors; ++counter, pXLine += 8, pYLine += 8)
			{
				const __m256 vx = _mm256_div_ps(_mm256_cvtepi32_ps(xline), xWidth);
				xline = _mm256_add_epi32(xline, _mm256_set1_epi32(8));

				// Linear part
				const __m256 xy = _mm256_mul_ps(vx, vy);
				const __m256 rlx = linearTransformX(vx, vy, xy);
				const __m256 rly = linearTransformY(vx, vy, xy);

				// Square part
				const __m256 x2 = _mm256_mul_ps(vx, vx);
				const __m256 y2 = _mm256_mul_ps(vy, vy);
				const __m256 x2y = _mm256_mul_ps(x2, vy);
				const __m256 xy2 = _mm256_mul_ps(vx, y2);
				const __m256 x2y2 = _mm256_mul_ps(x2, y2);
				const __m256 rsx = squaredTransformX(rlx, x2, y2, x2y, xy2, x2y2);
				const __m256 rsy = squaredTransformY(rly, x2, y2, x2y, xy2, x2y2);

				// Cubic parameters
				const __m256 x3 = _mm256_mul_ps(x2, vx);
				const __m256 y3 = _mm256_mul_ps(y2, vy);
				const __m256 x3y = _mm256_mul_ps(x3, vy);
				const __m256 xy3 = _mm256_mul_ps(vx, y3);
				const __m256 x3y2 = _mm256_mul_ps(x3, y2);
				const __m256 x2y3 = _mm256_mul_ps(x2, y3);
				const __m256 x3y3 = _mm256_mul_ps(x3, y3);

				// The bicubic transformation
				const __m256 xr = cubicTransformX(rsx, x3, y3, x3y, xy3, x3y2, x2y3, x3y3);
				const __m256 yr = cubicTransformY(rsy, x3, y3, x3y, xy3, x3y2, x2y3, x3y3);

				_mm256_storeu_ps(pXLine, _mm256_fmadd_ps(xr, xWidth, fxShiftVec));
				_mm256_storeu_ps(pYLine, _mm256_fmadd_ps(yr, yWidth, fyShiftVec));
			}
			// Remaining pixels of the line
			for (int n = nrVectors * 8; n < colEnd; ++n, ++pXLine, ++pYLine)
			{
				const float x = static_cast<float>(n) / static_cast<float>(bilinearParams.fXWidth);
				*pXLine = static_cast<float>(bilinearParams.fXWidth) * (fa0 + fa1 * x + fa2 * y + fa3 * x * y + fa4 * x * x + fa5 * y * y + fa6 * x * x * y + fa7 * x * y * y + fa8 * x * x * y * y +
					fa9 * x * x * x + fa10 * y * y * y + fa11 * x * x * x * y + fa12 * x * y * y * y + fa13 * x * x * x * y * y + fa14 * x * x * y * y * y + fa15 * x * x * x * y * y * y) + fxShift;
				*pYLine = static_cast<float>(bilinearParams.fYWidth) * (fb0 + fb1 * x + fb2 * y + fb3 * x * y + fb4 * x * x + fb5 * y * y + fb6 * x * x * y + fb7 * x * y * y + fb8 * x * x * y * y +
					fb9 * x * x * x + fb10 * y * y * y + fb11 * x * x * x * y + fb12 * x * y * y * y + fb13 * x * x * x * y * y + fb14 * x * x * y * y * y + fb15 * x * x * x * y * y * y) + fyShift;
			}
		}
		return 0;
	}

	return 1;
};

template <class T>
int AvxStacking::backgroundCalibration(const CBackgroundCalibration& backgroundCalibrationDef)
{
	// We calculate vectors with 16 pixels each, so this is the number of vectors to process.
	const int nrVectors = width / 16;
	const AvxSupport avxSupport{ inputBitmap };

	if (backgroundCalibrationDef.m_BackgroundCalibrationMode == BCM_NONE)
	{
		// Just copy color values as they are, pixel by pixel.
		const auto loop = [this, nrVectors](const auto& pixels, std::vector<float>& result) -> void
		{
			for (int row = 0; row < this->height; ++row)
			{
				const T* pColor = &pixels.at((this->lineStart + row) * this->width);
				float* pResult = &result.at(row * this->width);
				for (int counter = 0; counter < nrVectors; ++counter, pColor += 16, pResult += 16)
				{
					const auto [lo8, hi8] = AvxSupport::read16PackedSingle(pColor);
					_mm256_storeu_ps(pResult, lo8);
					_mm256_storeu_ps(pResult + 8, hi8);
				}
				// Remaining pixels of line
				for (int n = nrVectors * 16; n < this->colEnd; ++n, ++pColor, ++pResult)
				{
					*pResult = static_cast<float>(*pColor);
				}
			}
		};

		if (avxSupport.isColorBitmapOfType<T>())
		{
			loop(avxSupport.redPixels<T>(), redPixels);
			loop(avxSupport.greenPixels<T>(), greenPixels);
			loop(avxSupport.bluePixels<T>(), bluePixels);
			return 0;
		}

		if (avxSupport.isMonochromeBitmapOfType<T>())
		{
			loop(avxSupport.grayPixels<T>(), redPixels);
			return 0;
		}

		return 1;
	}
	else if (backgroundCalibrationDef.m_BackgroundInterpolation == BCI_RATIONAL)
	{
		const auto loop = [this, nrVectors](const auto& pixels, const auto& params, std::vector<float>& result) -> void
		{
			const __m256 a = _mm256_set1_ps(params.getParameterA());
			const __m256 b = _mm256_set1_ps(params.getParameterB());
			const __m256 c = _mm256_set1_ps(params.getParameterC());
			const __m256 fmin = _mm256_set1_ps(params.getParameterMin());
			const __m256 fmax = _mm256_set1_ps(params.getParameterMax());

			const auto interpolate = [&a, &b, &c, &fmin, &fmax](const __m256 color) noexcept -> __m256
			{
				const __m256 denom = _mm256_fmadd_ps(b, color, c); // b * color + c
				const __m256 mask = _mm256_cmp_ps(denom, _mm256_setzero_ps(), 0); // cmp: denom==0 ? 1 : 0
				const __m256 xplusa = _mm256_add_ps(color, a);
				const __m256 division = _mm256_div_ps(xplusa, denom);
				// If denominator == 0 => use (x+a) else use (x+a)/denominator, then do the max and min.
				return _mm256_max_ps(_mm256_min_ps(_mm256_blendv_ps(division, xplusa, mask), fmax), fmin); // blend: mask==1 ? b : a;
			};

			for (int row = 0; row < this->height; ++row)
			{
				const T* pColor = &pixels.at((this->lineStart + row) * this->width);
				float* pResult = &result.at(row * this->width);
				for (int counter = 0; counter < nrVectors; ++counter, pColor += 16, pResult += 16)
				{
					const auto [lo8, hi8] = AvxSupport::read16PackedSingle(pColor);
					_mm256_storeu_ps(pResult, interpolate(lo8));
					_mm256_storeu_ps(pResult + 8, interpolate(hi8));
				}
				// Remaining pixels of line
				for (int n = nrVectors * 16; n < this->colEnd; ++n, ++pColor, ++pResult)
				{
					const float fcolor = static_cast<float>(*pColor);
					const float denom = b.m256_f32[0] * fcolor + c.m256_f32[0];
					const float xplusa = fcolor + a.m256_f32[0];
					*pResult = std::max(std::min(denom == 0.0f ? xplusa : (xplusa / denom), fmax.m256_f32[0]), fmin.m256_f32[0]);
				}
			}
		};

		if (avxSupport.isColorBitmapOfType<T>())
		{
			loop(avxSupport.redPixels<T>(), backgroundCalibrationDef.m_riRed, redPixels);
			loop(avxSupport.greenPixels<T>(), backgroundCalibrationDef.m_riGreen, greenPixels);
			loop(avxSupport.bluePixels<T>(), backgroundCalibrationDef.m_riBlue, bluePixels);
			return 0;
		}

		if (avxSupport.isMonochromeBitmapOfType<T>())
		{
			loop(avxSupport.grayPixels<T>(), backgroundCalibrationDef.m_riRed, redPixels);
			return 0;
		}

		return 1;
	}
	else // LINEAR
	{
		const auto loop = [this, nrVectors](const auto& pixels, const auto& params, std::vector<float>& result) -> void
		{
			const __m256 a0 = _mm256_set1_ps(params.getParameterA0());
			const __m256 a1 = _mm256_set1_ps(params.getParameterA1());
			const __m256 b0 = _mm256_set1_ps(params.getParameterB0());
			const __m256 b1 = _mm256_set1_ps(params.getParameterB1());
			const __m256 xm = _mm256_set1_ps(params.getParameterXm());

			const auto interpolate = [a0, a1, b0, b1, xm](const __m256 x) noexcept -> __m256
			{
				const __m256 mask = _mm256_cmp_ps(x, xm, 17); // cmp: x < xm ? 1 : 0
				// If x < xm => use a0 and b0, else use a1 and b1.
				const __m256 aSelected = _mm256_blendv_ps(a1, a0, mask); // blend(arg1, arg2, mask): mask==1 ? arg2 : arg1;
				const __m256 bSelected = _mm256_blendv_ps(b1, b0, mask);
				return _mm256_fmadd_ps(x, aSelected, bSelected); // x * a + b
			};

			for (int row = 0; row < this->height; ++row)
			{
				const T* pColor = &pixels.at((this->lineStart + row) * this->width);
				float* pResult = &result.at(row * this->width);
				for (int counter = 0; counter < nrVectors; ++counter, pColor += 16, pResult += 16)
				{
					const auto [lo8, hi8] = AvxSupport::read16PackedSingle(pColor);
					_mm256_storeu_ps(pResult, interpolate(lo8));
					_mm256_storeu_ps(pResult + 8, interpolate(hi8));
				}
				// Remaining pixels of line
				for (int n = nrVectors * 16; n < this->colEnd; ++n, ++pColor, ++pResult)
				{
					const float fcolor = static_cast<float>(*pColor);
					*pResult = fcolor < xm.m256_f32[0] ? (fcolor * a0.m256_f32[0] + b0.m256_f32[0]) : (fcolor * a1.m256_f32[0] + b1.m256_f32[0]);
				}
			}
		};

		if (avxSupport.isColorBitmapOfType<T>())
		{
			loop(avxSupport.redPixels<T>(), backgroundCalibrationDef.m_liRed, redPixels);
			loop(avxSupport.greenPixels<T>(), backgroundCalibrationDef.m_liGreen, greenPixels);
			loop(avxSupport.bluePixels<T>(), backgroundCalibrationDef.m_liBlue, bluePixels);
			return 0;
		}

		if (avxSupport.isMonochromeBitmapOfType<T>())
		{
			loop(avxSupport.grayPixels<T>(), backgroundCalibrationDef.m_liRed, redPixels);
			return 0;
		}

		return 1;
	}

	return 0;
}

template <bool ISRGB, class T>
int AvxStacking::pixelDispatchAndAccumulate()
{
	AvxSupport avxTempBitmap{ tempBitmap };
	// Check if we were called with the correct template argument.
	if constexpr (ISRGB) {
		if (!avxTempBitmap.isColorBitmapOfType<T>())
			return 1;
	}
	else {
		if (!avxTempBitmap.isMonochromeBitmapOfType<T>())
			return 1;
	}

	const int nrVectors = width / 8;
	const int outWidth = avxTempBitmap.width();
	if (outWidth <= 0)
		return 1;

	// outWidth = width of the temp bitmap.
	// resultWidth = width of the rect we want to write (in temp bitmap)

	// Non-vectorized accumulation for the case of 2 (or more) x-coordinates being identical.
	// Vectorized version would be incorrect in that case.
	const auto accumulateSingle = [outWidth](const __m256 newColor, const __m256i xi, const __m256i yi, const __m256i mask, T* pOutputBitmap) -> void
	{
		// This needs to be done pixel by pixel of the vector, because neighboring pixels have identical indices (due to prior pixel transform step).
		for (int n = 0; n < 8; ++n)
		{
			if (mask.m256i_i32[n] != 0)
			{
				const size_t ndx = yi.m256i_i32[n] * outWidth + xi.m256i_i32[n];
				pOutputBitmap[ndx] = static_cast<T>(AvxSupport::accumulateSingleColorValue<T>(ndx, newColor.m256_f32[n], mask.m256i_i32[n], pOutputBitmap));
			}
		}
	};

	const __m256i resultWidthVec = _mm256_set1_epi32(this->resultWidth);
	const __m256i resultHeightVec = _mm256_set1_epi32(this->resultHeight);

	const auto accumulateAVX = [&resultWidthVec, &resultHeightVec, &accumulateSingle](const __m256i outNdx, const __m256 newColor, const __m256i col, const __m256i row, T* pOutputBitmap) -> void
	{
		const __m256i columnMask = _mm256_andnot_si256(_mm256_cmpgt_epi32(_mm256_setzero_si256(), col), _mm256_cmpgt_epi32(resultWidthVec, col)); // !(0 > x) and (width > x) = (x >= 0) and (x < width)
		const __m256i mask = _mm256_and_si256(columnMask, _mm256_andnot_si256(_mm256_cmpgt_epi32(_mm256_setzero_si256(), row), _mm256_cmpgt_epi32(resultHeightVec, row))); // Same for y.
		// Check if two adjacent indices are equal: Subtract the x-coordinates horizontally and check if any of the results equals zero. If so -> adjacent x-coordinates are equal.
		const bool equalIndex = (0 == _mm256_testc_si256(_mm256_setzero_si256(), _mm256_cmpeq_epi32(_mm256_setzero_si256(), _mm256_hsub_epi32(outNdx, _mm256_permutevar8x32_epi32(outNdx, _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 0))))));
		if (equalIndex) // If so, we cannot use AVX.
			return accumulateSingle(newColor, col, row, mask, pOutputBitmap);
		const __m256 limitedColor = AvxSupport::accumulateColorValues(outNdx, newColor, mask, pOutputBitmap);

		for (int n = 0; n < 8; ++n)
		{
			if (mask.m256i_i32[n] != 0) {
				const int ndx = outNdx.m256i_i32[n];
				pOutputBitmap[ndx] = static_cast<T>(limitedColor.m256_f32[n]);
			}
		}
	};

	const __m256i outWidthVec = _mm256_set1_epi32(outWidth);
	const auto colorPointer = [](const std::vector<float>& colorPixels, const size_t offset) -> const float*
	{
		if constexpr (ISRGB)
			return &*colorPixels.begin() + offset;
		else
			return nullptr;
	};
	const auto colorValue = [](const float *const pColor) -> __m256
	{
		if constexpr (ISRGB)
			return _mm256_loadu_ps(pColor);
		else
			return _mm256_undefined_ps();
	};
	const auto accumulateRGBorMono = [&](const __m256 red, const __m256 green, const __m256 blue, const __m256 fraction, const __m256i xCoord, const __m256i yCoord) -> void
	{
		const __m256i outNdx = _mm256_add_epi32(_mm256_mullo_epi32(outWidthVec, yCoord), xCoord);
		if constexpr (ISRGB)
		{
			accumulateAVX(outNdx, _mm256_mul_ps(red, fraction), xCoord, yCoord, &*avxTempBitmap.redPixels<T>().begin());
			accumulateAVX(outNdx, _mm256_mul_ps(green, fraction), xCoord, yCoord, &*avxTempBitmap.greenPixels<T>().begin());
			accumulateAVX(outNdx, _mm256_mul_ps(blue, fraction), xCoord, yCoord, &*avxTempBitmap.bluePixels<T>().begin());
		}
		else
		{
			accumulateAVX(outNdx, _mm256_mul_ps(red, fraction), xCoord, yCoord, &*avxTempBitmap.grayPixels<T>().begin());
		}
	};

	for (int row = 0; row < height; ++row)
	{
		const size_t offset = static_cast<size_t>(row) * static_cast<size_t>(width);
		const float* pXLine = &*xCoordinates.begin() + offset;
		const float* pYLine = &*yCoordinates.begin() + offset;
		const float* pRed = &*redPixels.begin() + offset;
		const float* pGreen = colorPointer(greenPixels, offset);
		const float* pBlue = colorPointer(bluePixels, offset);

		for (int counter = 0; counter < nrVectors; ++counter, pXLine += 8, pYLine += 8)
		{
			const __m256 fxline = _mm256_loadu_ps(pXLine);
			const __m256 fyline = _mm256_loadu_ps(pYLine);
			const __m256 xi = _mm256_floor_ps(fxline);
			const __m256 yi = _mm256_floor_ps(fyline);
			const __m256 xr = _mm256_sub_ps(fxline, xi);
			const __m256 yr = _mm256_sub_ps(fyline, yi);
			const __m256 xr1 = _mm256_sub_ps(_mm256_set1_ps(1.0f), xr);
			const __m256 yr1 = _mm256_sub_ps(_mm256_set1_ps(1.0f), yr);

			const __m256 red = _mm256_loadu_ps(pRed);
			const __m256 green = colorValue(pGreen);
			const __m256 blue = colorValue(pBlue);

			// Different pixels of the vector can have different number of fractions. So we always need to consider all 4 fractions.

			// 1.Fraction at (xi, yi)
			__m256 fraction = _mm256_mul_ps(xr1, yr1);
			__m256i xii = _mm256_cvtps_epi32(xi); // Rounding mode irrelevant, because xi was result of floor().
			__m256i yii = _mm256_cvtps_epi32(yi);
			accumulateRGBorMono(red, green, blue, fraction, xii, yii);

			// 2.Fraction at (xi+1, yi)
			fraction = _mm256_mul_ps(xr, yr1);
			xii = _mm256_add_epi32(xii, _mm256_set1_epi32(1));
			accumulateRGBorMono(red, green, blue, fraction, xii, yii);

			// 4.Fraction at (xi+1, yi+1)
			fraction = _mm256_mul_ps(xr, yr);
			yii = _mm256_add_epi32(yii, _mm256_set1_epi32(1));
			accumulateRGBorMono(red, green, blue, fraction, xii, yii);

			// 3.Fraction at (xi, yi+1)
			fraction = _mm256_mul_ps(xr1, yr);
			xii = _mm256_cvtps_epi32(xi);
			accumulateRGBorMono(red, green, blue, fraction, xii, yii);

			pRed += 8;
			if constexpr (ISRGB)
			{
				pGreen += 8;
				pBlue += 8;
			}
		}

		// Rest of line
		const auto accumulate1 = [outWidth, this](const float fraction, const size_t col, const size_t row, T* pOutputBitmap, const float* pColor) -> void
		{
			if (const int mask = (col >= 0 && col < this->resultWidth && row >= 0 && row < this->resultHeight) ? 0xffffffff : 0)
			{
				const size_t ndx = row * outWidth + col;
				pOutputBitmap[ndx] = static_cast<T>(AvxSupport::accumulateSingleColorValue<T>(ndx, *pColor * fraction, mask, pOutputBitmap));
			}
		};
		const auto accumulate_1_RGBorMono = [&](const float fraction, const int xCoord, const int yCoord, const float* pRed, const float* pGreen, const float* pBlue) -> void
		{
			if constexpr (ISRGB)
			{
				accumulate1(fraction, xCoord, yCoord, &*avxTempBitmap.redPixels<T>().begin(), pRed);
				accumulate1(fraction, xCoord, yCoord, &*avxTempBitmap.greenPixels<T>().begin(), pGreen);
				accumulate1(fraction, xCoord, yCoord, &*avxTempBitmap.bluePixels<T>().begin(), pBlue);
			}
			else
			{
				accumulate1(fraction, xCoord, yCoord, &*avxTempBitmap.grayPixels<T>().begin(), pRed);
			}
		};

		for (int n = nrVectors * 8; n < colEnd; ++n, ++pXLine, ++pYLine, ++pRed, ++pGreen, ++pBlue)
		{
			const float fx = *pXLine;
			const float fy = *pYLine;
			const float xi = std::floorf(fx);
			const float yi = std::floorf(fy);
			const float xr = fx - xi;
			const float yr = fy - yi;
			const float xr1 = 1.0f - xr;
			const float yr1 = 1.0f - yr;

			float fraction = xr1 * yr1;
			int xii = static_cast<int>(xi);
			int yii = static_cast<int>(yi);
			accumulate_1_RGBorMono(fraction, xii, yii, pRed, pGreen, pBlue);

			fraction = xr * yr1;
			++xii;
			accumulate_1_RGBorMono(fraction, xii, yii, pRed, pGreen, pBlue);

			fraction = xr * yr;
			++yii;
			accumulate_1_RGBorMono(fraction, xii, yii, pRed, pGreen, pBlue);

			fraction = xr1 * yr;
			xii = static_cast<int>(xi);
			accumulate_1_RGBorMono(fraction, xii, yii, pRed, pGreen, pBlue);
		}
	}

	return 0;
}

// *********************************
//    AVX Support
// *********************************

AvxSupport::AvxSupport(CMemoryBitmap& b) noexcept :
	bitmap{ b }
{};

int AvxSupport::getNrChannels() const
{
	CBitmapCharacteristics bitmapCharacteristics;
	const_cast<CMemoryBitmap&>(bitmap).GetCharacteristics(bitmapCharacteristics);
	return bitmapCharacteristics.m_lNrChannels;
};

bool AvxSupport::isColorBitmap() const
{
	return getNrChannels() == 3;
};

template <class T>
bool AvxSupport::isColorBitmapOfType() const
{
	auto *const p = const_cast<AvxSupport*>(this)->getColorPtr<T>();
	const bool isColor = p != nullptr && p->isTopDown();
	if constexpr (std::is_same<T, float>::value)
		return isColor && p->IsFloat();
	else
		return isColor;
}

bool AvxSupport::isMonochromeBitmap() const
{
	return getNrChannels() == 1;
};

template <class T>
bool AvxSupport::isMonochromeBitmapOfType() const
{
	auto *const p = const_cast<AvxSupport*>(this)->getGrayPtr<T>();
	// Note that Monochrome bitmaps are always topdown -> no extra check required! CF. CGrayBitmap::GetOffset(). Color filter arrays CFA (e.g. from DSLR) not yet supported.
	const bool isGray = p != nullptr && !p->IsCFA();
	if constexpr (std::is_same<T, float>::value)
		return isGray && p->IsFloat();
	else
		return isGray;
}

const int AvxSupport::width() const {
	return bitmap.Width();
}

template <class T>
bool AvxSupport::bitmapHasCorrectType() const
{
	return (isColorBitmapOfType<T>() || isMonochromeBitmapOfType<T>());
}

bool AvxSupport::checkCpuFeatures() noexcept {
	int cpuid[4] = { -1 };
	// FMA Flag
	__cpuidex(cpuid, 1, 0);
	const bool FMAsupported = ((cpuid[2] & 0x01000) != 0);
	// AVX2 Flag
	__cpuidex(cpuid, 7, 0);
	const bool AVX2supported = ((cpuid[1] & 0x020) != 0);

	//const bool BMI1supported = ((cpuid[1] & 0x04) != 0);
	//const bool BMI2supported = ((cpuid[1] & 0x0100) != 0);

	return (FMAsupported && AVX2supported);
}

inline __m256 AvxSupport::accumulateColorValues(const __m256i outNdx, const __m256 newColor, const __m256i mask, const std::uint16_t *const pOutputBitmap) noexcept
{
	const __m256i indexIsOddX16 = _mm256_slli_epi32(_mm256_and_si256(outNdx, _mm256_set1_epi32(0x01)), 4); // 0 if outNdx is even, 16 if outNdx is odd.
	const __m256i ndx = _mm256_srai_epi32(outNdx, 1); // outNdx / 2 (to fetch WORD with 32-bit-gather)
	const __m256i tempColorAsI16 = _mm256_mask_i32gather_epi32(_mm256_setzero_si256(), reinterpret_cast<const int*>(pOutputBitmap), ndx, mask, 4);
	const __m256i tempColor = _mm256_and_si256(_mm256_srlv_epi32(tempColorAsI16, indexIsOddX16), _mm256_set1_epi32(0x0000ffff)); // If outNdx was odd -> shift right tempColor by 16 bits.
	const __m256 accumulatedColor = _mm256_add_ps(_mm256_cvtepi32_ps(tempColor), newColor); // tempColor = 8 int in the range [0, 65535]
	return _mm256_min_ps(accumulatedColor, _mm256_set1_ps(static_cast<float>(0x0000ffff)));
}

inline __m256 AvxSupport::accumulateColorValues(const __m256i outNdx, const __m256 newColor, const __m256i mask, const std::uint32_t *const pOutputBitmap) noexcept
{
	const __m256i tempColor = _mm256_mask_i32gather_epi32(_mm256_setzero_si256(), reinterpret_cast<const int*>(pOutputBitmap), outNdx, mask, 4);
	const __m256 accumulatedColor = _mm256_add_ps(cvtEpu32Ps(tempColor), newColor);
	return _mm256_min_ps(accumulatedColor, _mm256_set1_ps(static_cast<float>(0xffffffffU)));
}

inline __m256 AvxSupport::accumulateColorValues(const __m256i outNdx, const __m256 newColor, const __m256i mask, const float *const pOutputBitmap) noexcept
{
	const __m256 tempColor = _mm256_mask_i32gather_ps(_mm256_setzero_ps(), pOutputBitmap, outNdx, _mm256_castsi256_ps(mask), 4);
	return _mm256_add_ps(tempColor, newColor);
}

template <class T>
inline float AvxSupport::accumulateSingleColorValue(const size_t outNdx, const float newColor, const int mask, const T* const pOutputBitmap) noexcept
{
	if (mask != 0)
	{
		const float accumulatedColor = static_cast<float>(pOutputBitmap[outNdx]) + newColor;
		return std::min(accumulatedColor, static_cast<float>(std::numeric_limits<T>::max()));
	}
	else
		return 0.0f;
}

// Explicit template instantiation for the types we need.
template bool AvxSupport::bitmapHasCorrectType<WORD>() const;
template bool AvxSupport::bitmapHasCorrectType<std::uint32_t>() const;
template bool AvxSupport::bitmapHasCorrectType<float>() const;

#endif
