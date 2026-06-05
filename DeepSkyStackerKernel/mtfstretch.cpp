/****************************************************************************
**
** Copyright (C) 2026 Amila Sanjaya Karunarathna
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
#include "pch.h"
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <vector>
#include "Multitask.h"
#include "StackedBitmap.h"

namespace
{
	//
	// The Midtone Transfer Function (MTF) is a pixel-value remapping function used to
	// non-linearly stretch an image such that a chosen midtone value is moved to a target
	// brightness level.  It is the same function used by PixInsight's Screen Transfer
	// Function (STF) and Autostretch routines.
	//
	// Given a midtone balance parameter 'b' in (0, 1) and an input value 'x' in [0, 1]:
	//
	//   MTF(b, x) = ((b - 1) * x) / ((2*b - 1) * x - b)
	//
	// Special cases:
	//   MTF(b, 0) = 0
	//   MTF(b, 1) = 1
	//   MTF(0.5, x) = x   (identity)
	//

	//
	// Helper: compute the median of a vector using nth_element (O(n) average).
	// The input vector is modified (partially sorted).
	//
	static float computeMedian(std::vector<float>& data)
	{
		if (data.empty())
			return 0.0f;

		const size_t n = data.size();
		const size_t mid = n / 2;

		std::nth_element(data.begin(), data.begin() + mid, data.end());

		if (n % 2 == 0)
		{
			// For even-sized data, average the two middle values
			float midVal = data[mid];
			auto it = std::max_element(data.begin(), data.begin() + mid);
			return (midVal + *it) * 0.5f;
		}
		return data[mid];
	}

	//
	// Helper: compute the Median Absolute Deviation (MAD) of data around a given median.
	// MAD = median( |x_i - median| )
	//
	static float computeMAD(const std::vector<float>& source, float median)
	{
		std::vector<float> absDevs(source.size());

		for (size_t i = 0; i < source.size(); i++)
			absDevs[i] = std::abs(source[i] - median);

		return computeMedian(absDevs);
	}

	//
	// Helper: compute the midtone balance parameter 'b' such that MTF(b, medianNorm) = target.
	//
	// Derivation (solving MTF(b, m) = t for b):
	//
	//   b = m * (t - 1) / (2 * t * m - t - m)
	//
	// where m = normalised median (after shadow clipping), t = target brightness.
	//
	static float computeMidtoneBalance(float medianNorm, float target)
	{
		// Guard against degenerate cases
		if (medianNorm <= 0.0f)
			return 0.5f;	// Identity - image is already black
		if (medianNorm >= 1.0f)
			return 0.5f;	// Identity - image is already white
		if (std::abs(medianNorm - target) < 1.0e-6f)
			return 0.5f;	// Already at target

		const float denom = 2.0f * target * medianNorm - target - medianNorm;
		if (std::abs(denom) < 1.0e-10f)
			return 0.5f;	// Avoid division by zero

		float b = medianNorm * (target - 1.0f) / denom;

		// Clamp to valid range (0, 1) exclusive
		b = std::clamp(b, 0.0001f, 0.9999f);

		return b;
	}

	//
	// Helper: apply the MTF to a single pixel value.
	//
	//   MTF(b, x) = ((b - 1) * x) / ((2*b - 1) * x - b)
	//
	static inline float applyMTF(float b, float x)
	{
		if (x <= 0.0f)
			return 0.0f;
		if (x >= 1.0f)
			return 1.0f;

		const float numerator = (b - 1.0f) * x;
		const float denominator = (2.0f * b - 1.0f) * x - b;

		if (std::abs(denominator) < 1.0e-10f)
			return x;

		return std::clamp(numerator / denominator, 0.0f, 1.0f);
	}
}

namespace DSS
{

	//
	// The MTF (Midtone Transfer Function) autostretch is a non-linear stretch that automatically 
	// evaluates the image statistics (median and MAD) to determine the optimal black point and 
	// midtone balance.
	//
	// When 'linked' is true, statistics are computed as the average across all channels,
	// preserving the existing colour balance of the image.
	//
	// When 'linked' is false, statistics are computed independently for each channel,
	// performing an implicit background neutralisation that removes colour casts.
	//
	void StackedBitmap::mtfComputeAutoStretchParameters(bool linked, MTFStretchParameters& parameters, float targetMedian, float shadowClipFactor)
	{
		const std::int64_t n = static_cast<std::int64_t>(m_lWidth) * m_lHeight;

		if (n == 0)
			return;

		float* buf[3] = {
			m_vRedPlane.data(),
			m_bMonochrome ? nullptr : m_vGreenPlane.data(),
			m_bMonochrome ? nullptr : m_vBluePlane.data()
		};

		const int numChannels = m_bMonochrome ? 1 : 3;

		// Per-channel statistics
		float median[3] = { 0.0f, 0.0f, 0.0f };
		float mad[3] = { 0.0f, 0.0f, 0.0f };
		float clipPoint[3] = { 0.0f, 0.0f, 0.0f };
		float midtoneBalance[3] = { 0.5f, 0.5f, 0.5f };


		for (int ch = 0; ch < numChannels; ch++)
		{
			//
			// Subsample the data to improve performance on large images
			//
			constexpr std::int64_t MAX_SAMPLE_SIZE = 250000;
			std::int64_t stride = 1;
			if (n > MAX_SAMPLE_SIZE)
				stride = n / MAX_SAMPLE_SIZE;

			std::vector<float> sampleData;
			sampleData.reserve((n / stride) + 1);
			for (std::int64_t i = 0; i < n; i += stride)
			{
				sampleData.push_back(buf[ch][i]);
			}
			median[ch] = computeMedian(sampleData);

			// Recompute MAD from the original data (sampleData was modified by computeMedian)
			std::vector<float> sampleDataForMAD;
			sampleDataForMAD.reserve((n / stride) + 1);
			for (std::int64_t i = 0; i < n; i += stride)
			{
				sampleDataForMAD.push_back(buf[ch][i]);
			}
			mad[ch] = computeMAD(sampleDataForMAD, median[ch]);
		}


		if (linked || m_bMonochrome)
		{
			// Linked mode: use average statistics across all channels
			float avgMedian = median[0];
			float avgMAD = mad[0];

			if (!m_bMonochrome)
			{
				avgMedian = (median[0] + median[1] + median[2]) / 3.0f;
				avgMAD = (mad[0] + mad[1] + mad[2]) / 3.0f;
			}

			// Shadow clipping point
			float c = std::max(0.0f, avgMedian - shadowClipFactor * avgMAD);

			// Normalise the median relative to the clipping range
			float medNorm = (1.0f - c > 1.0e-6f) ? (avgMedian - c) / (1.0f - c) : avgMedian;

			// Compute midtone balance
			float b = computeMidtoneBalance(medNorm, targetMedian);

			// Apply same values to all channels
			for (int ch = 0; ch < numChannels; ch++)
			{
				clipPoint[ch] = c;
				midtoneBalance[ch] = b;
			}
		}
		else
		{
			// Unlinked mode: independent per-channel computation
			for (int ch = 0; ch < numChannels; ch++)
			{
				clipPoint[ch] = std::max(0.0f, median[ch] - shadowClipFactor * mad[ch]);

				float medNorm = (1.0f - clipPoint[ch] > 1.0e-6f)
					? (median[ch] - clipPoint[ch]) / (1.0f - clipPoint[ch])
					: median[ch];

				midtoneBalance[ch] = computeMidtoneBalance(medNorm, targetMedian);
			}
		}

		float whitePoint[3] = { 1.0f, 1.0f, 1.0f };

		for (int ch = 0; ch < numChannels; ch++)
		{
			parameters.clipPoint[ch] = clipPoint[ch];
			parameters.midtoneBalance[ch] = midtoneBalance[ch];
			parameters.whitePoint[ch] = whitePoint[ch];
		}

		return; 
#if (0)
		if (m_bMonochrome)
		{
			const float c = clipPoint[0];
			const float b = midtoneBalance[0];
			const float w = whitePoint[0];
			const float range = w - c;

#pragma omp parallel for schedule(static) if (nrProcessors > 1)
			for (std::int64_t i = 0; i < n; i++)
			{
				// Rescale: clip shadows and normalise to [0, 1]
				float x = (range > 1.0e-6f) ? std::clamp((buf[0][i] - c) / range, 0.0f, 1.0f) : 0.0f;
				buf[0][i] = applyMTF(b, x);
			}
		}
		else
		{
			const float cR = clipPoint[0], cG = clipPoint[1], cB = clipPoint[2];
			const float bR = midtoneBalance[0], bG = midtoneBalance[1], bB = midtoneBalance[2];
			const float wR = whitePoint[0], wG = whitePoint[1], wB = whitePoint[2];
			const float rangeR = wR - cR, rangeG = wG - cG, rangeB = wB - cB;

#pragma omp parallel for schedule(static) if (nrProcessors > 1)
			for (std::int64_t i = 0; i < n; i++)
			{
				float xR = (rangeR > 1.0e-6f) ? std::clamp((buf[0][i] - cR) / rangeR, 0.0f, 1.0f) : 0.0f;
				float xG = (rangeG > 1.0e-6f) ? std::clamp((buf[1][i] - cG) / rangeG, 0.0f, 1.0f) : 0.0f;
				float xB = (rangeB > 1.0e-6f) ? std::clamp((buf[2][i] - cB) / rangeB, 0.0f, 1.0f) : 0.0f;

				buf[0][i] = applyMTF(bR, xR);
				buf[1][i] = applyMTF(bG, xG);
				buf[2][i] = applyMTF(bB, xB);
			}
		}
#endif
	}

	//
	// Manual MTF Stretch
	//
	void StackedBitmap::mtfStretch(const MTFStretchParameters& parameters)
	{
		const int nrProcessors{ ::Multitask::GetNrProcessors() };
		const std::int64_t n = static_cast<std::int64_t>(m_lWidth) * m_lHeight;

		if (n == 0)
			return;

		float* buf[3] = {
			m_vRedPlane.data(),
			m_bMonochrome ? nullptr : m_vGreenPlane.data(),
			m_bMonochrome ? nullptr : m_vBluePlane.data()
		};

		const float rangeR = parameters.whitePoint[0] - parameters.clipPoint[0];
		const float rangeG = parameters.whitePoint[1] - parameters.clipPoint[1];
		const float rangeB = parameters.whitePoint[2] - parameters.clipPoint[2];

		if (m_bMonochrome)
		{
			#pragma omp parallel for schedule(static) if (nrProcessors > 1)
			for (std::int64_t i = 0; i < n; i++)
			{
				float x = (rangeR > 1.0e-6f) ? std::clamp((buf[0][i] - parameters.clipPoint[0]) / rangeR, 0.0f, 1.0f) : 0.0f;
				buf[0][i] = applyMTF(parameters.midtoneBalance[0], x);
			}
		}
		else
		{
			#pragma omp parallel for schedule(static) if (nrProcessors > 1)
			for (std::int64_t i = 0; i < n; i++)
			{
				float xR = (rangeR > 1.0e-6f) ? std::clamp((buf[0][i] - parameters.clipPoint[0]) / rangeR, 0.0f, 1.0f) : 0.0f;
				float xG = (rangeG > 1.0e-6f) ? std::clamp((buf[1][i] - parameters.clipPoint[1]) / rangeG, 0.0f, 1.0f) : 0.0f;
				float xB = (rangeB > 1.0e-6f) ? std::clamp((buf[2][i] - parameters.clipPoint[2]) / rangeB, 0.0f, 1.0f) : 0.0f;

				buf[0][i] = applyMTF(parameters.midtoneBalance[0], xR);
				buf[1][i] = applyMTF(parameters.midtoneBalance[1], xG);
				buf[2][i] = applyMTF(parameters.midtoneBalance[2], xB);
			}
		}
	}

} // namespace DSS
