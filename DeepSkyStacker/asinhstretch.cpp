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
#include "DeepSkyStacker.h"
#include "DSSTools.h"
#include "Multitask.h"
#include "StackedBitmap.h"
#include <algorithm>

namespace DSS 
{
	//
	// Normalise the image data to a range of [0.0, 1.0], which is required for
	// the ASinH stretch processing
	//
	void StackedBitmap::normalise()
	{
		const float scaleFactor{ m_lNrBitmaps * 256.0f };
#pragma omp parallel for schedule(static) if (Multitask::GetNrProcessors() > 1)
		for (std::int64_t i = 0; i < m_vRedPlane.size(); i++)
		{
			m_vRedPlane[i] /= scaleFactor;
			m_vGreenPlane[i] /= scaleFactor;
			m_vBluePlane[i] /= scaleFactor;
		}
	}

	//
	// De-normalise the image data after the ASinH stretch processing, to bring it back to the
	// normal range of pixel values.
	//
	void StackedBitmap::deNormalise()
	{
		const float scaleFactor{ m_lNrBitmaps * 256.0f };
		for (std::int64_t i = 0; i < m_vRedPlane.size(); i++)
		{
			m_vRedPlane[i] *= scaleFactor;
			m_vGreenPlane[i] *= scaleFactor;
			m_vBluePlane[i] *= scaleFactor;
		}
	}

	//
	//
	// The asinh, or inverse hyperbolic sine, stretch is a non-linear stretch that can be used to bring out
	// faint details in an image while preserving the overall structure and color balance.
	// 
	// The beta parameter controls the strength of the stretch, with higher values resulting in a more
	// pronounced stretch.
	// 
	// The offset parameter can be used to adjust the point at which the stretch begins (i.e the black point),
	// allowing for further fine-tuning of the final image.
	//
	//	The human_luminance option allows the stretch to be applied in a way that preserves the perceived
	//	luminance of the image, which can help to maintain a more natural appearance.
	//
	// When the Use Human-weighted Luminance option is not ticked, rgb_original is the mean of the three
	// pixel values; when it is set, the weighting changes to:
	// 
	//		0.2126 for the red value,
	//		0.7152 for the green value,	and
	//		0.0722 for the blue value
	// 
	// which gets results closer to human perceptual color balance.
	//
	void StackedBitmap::asinhStretch(float beta, float offset, bool human_luminance)
	{
		float* buf[3] = {	
			m_vRedPlane.data(), 
			m_bMonochrome ? nullptr : m_vGreenPlane.data(),
			m_bMonochrome ? nullptr : m_vBluePlane.data() };

		const int nrProcessors{ Multitask::GetNrProcessors() };
		const bool do_channel[3] = { true, true, true };

		float m_CB = 1.0f;
		float asinh_beta = std::asinh(beta);
		float factor_red = human_luminance ? 0.2126f : 0.3333f;
		float factor_green = human_luminance ? 0.7152f : 0.3333f;
		float factor_blue = human_luminance ? 0.0722f : 0.3333f
			;
		const int RLAYER = 0, GLAYER = 1, BLAYER = 2;

		std::int64_t i, n = m_lWidth * m_lHeight;
		if (!m_bMonochrome)		// colour image
		{
			//
			// For colour images, the transform function that is applied is:
			// 
			// pixel = (original - offset) x asinh(rgb_original x beta)
			//         ------------------------------------------------
			//                (rgb_original x asinh(beta))
			//
			// rgb_original is computed from the pixel values of the three channels using the rgbblend algorithm
			// rather than using the average of the three  channels.
			//
			// If the average of the three channels were used for rgb_original, one or two channel values will be
			// greater than the average, and can therefore clip.   This can cause colour artefacts when bright,
			// strongly coloured regions of the image are stretched.   To mitigate this problem the RGB blend
			// algorithm was developed by the authors of the GHS Astro tool.
			// 
			// See the source of rgbblend in DSSTools.h for the full details.
			//
#pragma omp parallel for schedule(static) if (nrProcessors > 1)
			for (i = 0; i < n; i++)
			{
				blend_data data = { .sf = { 0.0f }, .tf = { 0.0f }, .do_channel = do_channel };
				float val[3] = { buf[RLAYER][i], buf[GLAYER][i], buf[BLAYER][i] };

				float prime[3];
				for (int chan = 0; chan < 3; chan++)
					prime[chan] = std::max(0.0f, (val[chan] - offset) / (1.0f - offset));

				float x = factor_red * prime[RLAYER] + factor_green * prime[GLAYER] + factor_blue * prime[BLAYER];
				float k = (x == 0.0f) ? 0.0f : (beta == 0.0f) ? 1.0f : asinhf(beta * x) / (x * asinh_beta);
				
				//
				// Perform rgbblend processing
				//
				for (int chan = 0; chan < 3; chan++)
				{
					data.sf[chan] = std::min(1.0f, std::max(0.0f, (prime[chan] * k)));
					data.tf[chan] = (prime[chan] == 0.0f) ? 0.0f : (beta == 0.0f) ? 1.0f : std::asinh(beta * prime[chan]) / (prime[chan] * asinh_beta);
				}
				rgbblend(&data, &buf[RLAYER][i], &buf[GLAYER][i], &buf[BLAYER][i], m_CB);
			}
		}
		else    // Monochrome image
		{
#pragma omp parallel for schedule(static) if (nrProcessors > 1)
			for (i = 0; i < n; i++)
			{
				float x, k;
				x = buf[RLAYER][i];
				float xprime = max(0.0f, (x - offset) / (1.0f - offset));
				k = (xprime == 0.0f) ? 0.0f : (beta == 0.0f) ? 1.0f : std::asinh(beta * xprime) / (xprime * asinh_beta);
				buf[RLAYER][i] = min(1.0f, max(0.0f, (xprime * k)));
			}
		}
	}
} // namespace DSS
