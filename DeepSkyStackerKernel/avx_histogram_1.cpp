#include "pch.h"
#include "avx_histogram.h"
#include "MemoryBitmap.h"
#include "ColorHelpers.h"


// *****************
// Non-AVX Histogram
// *****************

int NonAvxHistogram::calcHistogram(const size_t lineStart, const size_t lineEnd, const double multiplier)
{
	const size_t width = histoData.inputBitmap.Width();
	const double fMultiplier = multiplier * 256.0;

	for (size_t row = lineStart; row < lineEnd; ++row)
	{
		for (size_t col = 0; col < width; ++col)
		{
			double fRed, fGreen, fBlue;
			this->histoData.inputBitmap.GetPixel(col, row, fRed, fGreen, fBlue);

			constexpr auto ColorToIndex = [](const double color) {
				constexpr double Maxvalue = static_cast<double>(std::numeric_limits<std::uint16_t>::max());
				return static_cast<size_t>(std::min(color, Maxvalue));
				};

			++this->histoData.redHisto[ColorToIndex(fRed * fMultiplier)];
			++this->histoData.greenHisto[ColorToIndex(fGreen * fMultiplier)];
			++this->histoData.blueHisto[ColorToIndex(fBlue * fMultiplier)];
		}
	}

	return 0;
}


// ------------------------
// Non AVX Bezier functions
// ------------------------

int NonAvxBezierAndSaturation::avxAdjustRGB(const int nBitmaps, const class DSS::RGBHistogramAdjust& histoAdjust)
{
	const float scale = 255.0f / static_cast<float>(nBitmaps);

	for (size_t n = 0, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		this->histoData.redBuffer[n] *= scale;
		this->histoData.greenBuffer[n] *= scale;
		this->histoData.blueBuffer[n] *= scale;
	}
	for (size_t n = 0, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		double r = this->histoData.redBuffer[n];
		double g = this->histoData.greenBuffer[n];
		double b = this->histoData.blueBuffer[n];
		histoAdjust.Adjust(r, g, b);

		this->histoData.redBuffer[n] = static_cast<float>(r);
		this->histoData.greenBuffer[n] = static_cast<float>(g);
		this->histoData.blueBuffer[n] = static_cast<float>(b);
	}

	constexpr size_t VecLen = sizeof(__m128) / sizeof(float);
	constexpr float scalingFactor = static_cast<float>(1.0 / 255.0);

	// SSE2 code is OK, because every x64 CPU supports it.
	const auto loadAndScale = [scalingVector = _mm_set1_ps(scalingFactor), minVal = _mm_set1_ps(255.0f)](const float* pValue) -> __m128 {
		return _mm_min_ps(_mm_mul_ps(_mm_loadu_ps(pValue), scalingVector), minVal);
	};

	for (size_t n = 0, nVecs = this->histoData.redBuffer.size() / VecLen; n < nVecs; ++n)
	{
		__m128 r = loadAndScale(this->histoData.redBuffer.data() + n * 4);
		__m128 g = loadAndScale(this->histoData.greenBuffer.data() + n * 4);
		__m128 b = loadAndScale(this->histoData.blueBuffer.data() + n * 4);
		_mm_storeu_ps(this->histoData.redBuffer.data() + n * 4, r);
		_mm_storeu_ps(this->histoData.greenBuffer.data() + n * 4, g);
		_mm_storeu_ps(this->histoData.blueBuffer.data() + n * 4, b);
	}
	for (size_t n = (this->histoData.redBuffer.size() / VecLen) * VecLen, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		this->histoData.redBuffer[n] = std::min(this->histoData.redBuffer[n] * scalingFactor, 255.0f);
		this->histoData.greenBuffer[n] = std::min(this->histoData.greenBuffer[n] * scalingFactor, 255.0f);
		this->histoData.blueBuffer[n] = std::min(this->histoData.blueBuffer[n] * scalingFactor, 255.0f);
	}

	return 0;
}

int NonAvxBezierAndSaturation::avxToHsl()
{
	for (size_t n = 0, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		double h, s, l;
		ToHSL(this->histoData.redBuffer[n], this->histoData.greenBuffer[n], this->histoData.blueBuffer[n], h, s, l);
		this->histoData.redBuffer[n] = h;
		this->histoData.greenBuffer[n] = s;
		this->histoData.blueBuffer[n] = l;
	}
	return 0;
}

int NonAvxBezierAndSaturation::avxBezierAdjust(const size_t)
{
	for (size_t n = 0, bufferLen = this->histoData.blueBuffer.size(); n < bufferLen; ++n)
	{
		const auto it = std::lower_bound(this->histoData.bezierX.cbegin(), this->histoData.bezierX.cend(), this->histoData.blueBuffer[n]);
		const std::ptrdiff_t ndx = it - this->histoData.bezierX.cbegin();
		if (ndx < static_cast<std::ptrdiff_t>(this->histoData.bezierX.size()))
			this->histoData.blueBuffer[n] = this->histoData.bezierY[ndx];
	}
	return 0;
}

int NonAvxBezierAndSaturation::avxBezierSaturation(const size_t, const float saturationShift)
{
	if (saturationShift == 0)
		return 0;

	const auto satShift = [shiftVal = saturationShift > 0 ? 10.0f / saturationShift : -0.1f * saturationShift](const float v) -> float
	{
		return std::pow(v, shiftVal);
	};

	std::transform(std::cbegin(this->histoData.greenBuffer), std::cend(this->histoData.greenBuffer), std::begin(this->histoData.greenBuffer), satShift);

	return 0;
}

int NonAvxBezierAndSaturation::avxToRgb(const bool markOverAndUnderExposure)
{
	for (size_t n = 0, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		const float l = this->histoData.blueBuffer[n];

		double r, g, b;
		ToRGB(this->histoData.redBuffer[n], this->histoData.greenBuffer[n], l, r, g, b);

		if (markOverAndUnderExposure)
		{
			const bool notoverexposed = l <= 1.0f;
			const bool notunderexposed = l > (2.0f / 255.0f);

			this->histoData.redBuffer[n] = notoverexposed ? (notunderexposed ? r : 0.0f) : 255.0f;
			this->histoData.greenBuffer[n] = (notoverexposed && notunderexposed) ? g : 0.0f;
			this->histoData.blueBuffer[n] = notunderexposed ? (notoverexposed ? b : 0.0f) : 255.0f;
		}
		else
		{
			this->histoData.redBuffer[n] = r;
			this->histoData.greenBuffer[n] = g;
			this->histoData.blueBuffer[n] = b;
		}
	}
	return 0;
}
