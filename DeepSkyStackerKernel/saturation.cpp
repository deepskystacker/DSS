/****************************************************************************
**
** Copyright (C) 2026 David C. Partridge
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
#include "DSSTools.h"
#include "Multitask.h"
#include "StackedBitmap.h"
#include "ColorHelpers.h"
#include "avx_includes.h"

#include <algorithm>
namespace
{
	//
	// Adjust the image saturation and vibrance using the saturationShift and vibranceFactor parameters.
	// 
	// On input saturationBuffer, contains the current saturation.
	// 
	// There's no need to have fall back code for the last few pixels, as the vector size of CGrayBitmap
	// is always a multiple of 32 bytes (8 floats).   This works perfectly for __m128 and __m256 types.
	// If we ever decide to use __m512 types we will need to revisit the over-allocation in CGrayBitmapT<T>.
	// 
	// See CGrayBitmapT<T>::InitInternals() for details
	//
	// Range for saturationShift was [-50, 50] in the original code.  The current GUI only uses a range 
	// of [-10, 50] where negative values decrease saturation and positive values increase saturation. 
	// 
	// Range for vibranceFactor is [0.0, 0.99], where increasingly positive values increase vibrance.
	//
	void adjustSaturation_sse2(std::vector<float>& saturationBuffer, float saturationShift, float vibranceFactor)
	{
		[[maybe_unused]] const int nrProcessors{ Multitask::GetNrProcessors() };
		using VecType = __m128;
		constexpr size_t vectorLength = sizeof(VecType) / sizeof(float);
		std::int64_t len = static_cast<std::int64_t>(saturationBuffer.size()) / vectorLength;
		ZASSERT(saturationBuffer.size() % 8 == 0);

		VecType minVal = _mm_set1_ps(0.0f);
		VecType maxVal = _mm_set1_ps(1.0f);

		float* const pSaturation = saturationBuffer.data();

		//
		// Now adjust the saturation for each pixel based on the saturationShift and vibranceFactor
		// There's no need to have fall back code for the last few pixels,
		// as the vector size is always a multiple of 8.
		//
#pragma omp parallel for default(shared) schedule(static) if (nrProcessors > 1) 
		for (std::int64_t n = 0; n < len ; ++n)
		{

			VecType s = _mm_loadu_ps(pSaturation + n * vectorLength);	// Load 4 saturation values
			VecType adjustedSaturation = s;

			// Adjust saturation using the shift value
			if (0.0f != saturationShift)
			{
				const VecType shiftVal = _mm_set1_ps(saturationShift > 0 ? 10.0f / saturationShift : -0.1f * saturationShift);
				adjustedSaturation = _mm_pow_ps(s, shiftVal);
				adjustedSaturation = _mm_min_ps(_mm_max_ps(adjustedSaturation, minVal), maxVal);	// Clamp to [0, 1]
			}
			//
			// Adjust the vibrance, which is a more subtle adjustment that increases saturation
			// for less saturated pixels.
			// 
			// Create a mask so we only increase saturation for pixels that are
			// not already highly saturated
			//	mask = 1.0 - adjustedSaturation
			//
			const VecType mask = _mm_sub_ps(maxVal, adjustedSaturation);

			// Calculate the adjusted saturation based on the vibrance factor and the mask
			//	adjustedSaturation = adjustedSaturation * (1.0 + mask * vibranceFactor);
			// and clamp it to the range [0, 1]
			adjustedSaturation = _mm_mul_ps(adjustedSaturation, _mm_add_ps(maxVal, _mm_mul_ps(mask, _mm_set1_ps(vibranceFactor))));
			adjustedSaturation = _mm_min_ps(_mm_max_ps(adjustedSaturation, minVal), maxVal);	// Clamp to [0, 1]

			_mm_storeu_ps(pSaturation + n * vectorLength, adjustedSaturation);
		}
	}
}

namespace DSS
{
	//
	// Range for saturationShift was [-50, 50] in the original code.  The current GUI only uses a range 
	// of [-10, 50] where negative values decrease saturation and positive values increase saturation. 
	// 
	// Range for vibranceFactor is [0.0, 0.99], where increasingly positive values increase vibrance.
	//
	void StackedBitmap::adjustSaturation(float saturationShift, float vibranceFactor)
	{
		//
		// Nothing to do if this is a monochrome image or the factors are zero
		//
		if (m_bMonochrome || (0.0f == saturationShift && 0.0f == vibranceFactor))
			return;

		//
		// Convert the RGB values to HSL, so we can adjust the saturation
		//
		rgbToHsl_sse2(m_vRedPlane, m_vGreenPlane, m_vBluePlane);

		
		// Now adjust the saturation for each pixel based on the saturationShift and vibranceFactor
		// The saturation is stored in the green plane, so we only need to adjust that plane.
		adjustSaturation_sse2(m_vGreenPlane, saturationShift, vibranceFactor);

		//
		// Convert the HSL values back to RGB
		//
		hslToRgb_sse2(m_vRedPlane, m_vGreenPlane, m_vBluePlane);
	}
}
