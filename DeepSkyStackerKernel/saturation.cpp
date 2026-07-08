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
#include <algorithm>

namespace DSS
{
	void StackedBitmap::adjustSaturation(float saturationShift, float vibranceFactor)
	{
		//
		// Nothing to do if this is a monochrome image or the factors are zero
		//
		if (m_bMonochrome || (0.0f == saturationShift && 0.0f == vibranceFactor))
			return;

		float* buf[3] = {
			m_vRedPlane.data(),
			m_vGreenPlane.data(),
			m_vBluePlane.data()
		};


		const int nrProcessors{ Multitask::GetNrProcessors() };

		std::int64_t i, n = m_lWidth * m_lHeight;

#pragma omp parallel for schedule(static) if (nrProcessors > 1) shared(buf, n, saturationShift, vibranceFactor) private(i)
		for (i = 0; i < n; i++)
		{
			double R = buf[0][i], G = buf[1][i], B = buf[2][i];
			auto [h, s, l] = rgbToHsl(R, G, B);
			auto adjustedSaturation{ s };
			//
			// Initial step is to adjust overall saturation if asked to do so
			//
			if (0.0f != saturationShift)
			{
				float shiftVal = saturationShift > 0 ? 10.0f / saturationShift : -0.1f * saturationShift;
				adjustedSaturation = std::clamp(std::pow(s, shiftVal), 0.0, 1.0);
			}

			//
			// Adjust the vibrance, which is a more subtle adjustment that increases saturation
			// for less saturated pixels.
			// 
			// Create a mask so we only increase saturation for pixels that are
			// not already highly saturated
			//
			double mask = 1.0 - adjustedSaturation;

			//
			// Calculate the adjusted saturation based on the vibrance factor and the mask
			// and clip it to the range [0.0, 1.0]
			//
			adjustedSaturation = adjustedSaturation * (1.0 + mask * vibranceFactor);
			adjustedSaturation = std::clamp(adjustedSaturation, 0.0, 1.0);

			// Convert back to RGB with the adjusted saturation
			std::tie(R, G, B) = hslToRgb(h, adjustedSaturation, l);
			buf[0][i] = static_cast<float>(R); 
			buf[1][i] = static_cast<float>(G); 
			buf[2][i] = static_cast<float>(B); 
		}
	}
}
