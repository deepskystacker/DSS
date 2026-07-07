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
#include <algorithm>

namespace
{
	static double hue2rgb(double p, double q, double t)
	{
		if (t < 0.0) t += 1.0;
		if (t > 1.0) t -= 1.0;
		if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
		if (t < 1.0 / 2.0) return q;
		if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
		return p;
	}

	//
	// Convert RGB values to HSL.
	// r, g, b expected in [0, 1].
	// 
	// Returns h in degrees [0,360), s and l in [0,1].
	//
	std::tuple<double, double, double> rgbToHsl(double r, double g, double b)
	{
		double maxval = std::max({ r, g, b });
		double minval = std::min({ r, g, b });
		double l = (maxval + minval) * 0.5;
		double h = 0.0;
		double s = 0.0;

		if (maxval != minval) {
			double diff = maxval - minval;
			s = (l > 0.5) ? (diff / (2.0 - maxval - minval)) : (diff / (maxval + minval));

			if (maxval == r) {
				h = (g - b) / diff + (g < b ? 6.0 : 0.0);
			}
			else if (maxval == g) {
				h = (b - r) / diff + 2.0;
			}
			else { // maxval == b
				h = (r - g) / diff + 4.0;
			}
			h *= 60.0; // convert to degrees
		}

		return { h, s, l };
	}

	//
	// Convert HSL values to RGB.
	// h expected in degrees (any real value, will be wrapped to [0,360)),
	// s and l in [0,1].
	// 
	// Returns r, g, b in [0,1].
	// 	
	std::tuple<double, double, double> hslToRgb(double h, double s, double l)
	{
		double r, g, b;
		if (s == 0.0)
		{
			r = g = b = l; // achromatic
		}
		else
		{
			// normalize hue to [0,1)
			double hk = std::fmod(h, 360.0) / 360.0;
			if (hk < 0.0) hk += 1.0;

			double q = (l < 0.5) ? (l * (1.0 + s)) : (l + s - l * s);
			double p = 2.0 * l - q;
			r = hue2rgb(p, q, hk + 1.0 / 3.0);
			g = hue2rgb(p, q, hk);
			b = hue2rgb(p, q, hk - 1.0 / 3.0);
		}
		return { r, g, b };
	}
}

namespace DSS
{
	void StackedBitmap::adjustVibrance(float vibranceFactor)
	{
		//
		// Nothing to do if this is a monochrome image or the factor is zero
		//
		if (m_bMonochrome || 0.0f == vibranceFactor)
			return;

		float* buf[3] = {
			m_vRedPlane.data(),
			m_vGreenPlane.data(),
			m_vBluePlane.data()
		};


		const int nrProcessors{ Multitask::GetNrProcessors() };

		std::int64_t i, n = m_lWidth * m_lHeight;

#pragma omp parallel for schedule(static) if (nrProcessors > 1)
		for (i = 0; i < n; i++)
		{
			double R = buf[0][i], G = buf[1][i], B = buf[2][i];
			auto [h, s, l] = rgbToHsl(R, G, B);
			//
			// Create a mask so we only increase saturation for pixels that are
			// not already highly saturated
			//
			double mask = 1.0 - s;

			//
			// Calculate the adjusted saturation based on the vibrance factor and the mask
			// and clip it to the range [0.0, 1.0]
			//
			double adjustedSaturation = s * (1.0 + mask * vibranceFactor);
			adjustedSaturation = std::clamp(adjustedSaturation, 0.0, 1.0);

			// Convert back to RGB with the adjusted saturation
			std::tie(R, G, B) = hslToRgb(h, adjustedSaturation, l);
			buf[0][i] = static_cast<float>(R); 
			buf[1][i] = static_cast<float>(G); 
			buf[2][i] = static_cast<float>(B); 
		}
	}
}
