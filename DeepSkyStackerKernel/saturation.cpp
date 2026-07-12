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
	// Convert vectors of RGB values to HSL.
	// r, g, b expected in [0, 1].
	// 
	// Returns h in degrees [0,360), s and l in [0,1].
	//
	// On input redBuffer, greenBuffer, blueBuffer contain R,G,B values, on output they contain H,S,L values.
	// There's no need to have fall back code for the last few pixels,
	// as the vector size is always a multiple of 8.
	//
	void rgbToHsl_sse2(std::vector<float>& redBuffer, std::vector<float>& greenBuffer, std::vector<float>& blueBuffer)
	{
		[[maybe_unused]] const int nrProcessors{ Multitask::GetNrProcessors() };
		using VecType = __m128;
		constexpr size_t vectorLength = sizeof(VecType) / sizeof(float);
		std::int64_t len = static_cast<std::int64_t>(greenBuffer.size()) / vectorLength;

		const auto toHSL = [](const VecType r, const VecType g, const VecType b) -> std::tuple<VecType, VecType, VecType>
			{
				const VecType v0 = _mm_set1_ps(0.0f);
				const VecType v2 = _mm_set1_ps(2.0f);
				const VecType v4 = _mm_set1_ps(4.0f);
				const VecType v6 = _mm_set1_ps(6.0f);
				const VecType v60 = _mm_set1_ps(60.0f);
				const VecType v360 = _mm_set1_ps(360.0f);
				const VecType v_half = _mm_set1_ps(0.5f);
				const VecType full_mask = _mm_castsi128_ps(_mm_set1_epi32(-1)); // all ones

				// Calculate the minimum and maximum values for each pixel
				const VecType minimum = _mm_min_ps(_mm_min_ps(r, g), b);
				const VecType maximum = _mm_max_ps(_mm_max_ps(r, g), b);

				// Calculate the difference between the maximum and minimum values
				const VecType diff = _mm_sub_ps(maximum, minimum);

				// Compute l = (max + min) / 2
				const VecType l = _mm_mul_ps(_mm_add_ps(maximum, minimum), v_half);

				// Compute s = (l > 0.5) ? (diff / (2 - max - min)) : (diff / (max + min))
				const VecType denom1 = _mm_sub_ps(v2, _mm_add_ps(maximum, minimum));
				const VecType denom2 = _mm_add_ps(maximum, minimum);

				// Perform divisions (may produce NaN for zero denominators, which we will handle later)
				const VecType s1 = _mm_div_ps(diff, denom1);
				const VecType s2 = _mm_div_ps(diff, denom2);

				const VecType mask_l_gt_half = _mm_cmpgt_ps(l, v_half); // l > 0.5 ? all-ones : all-zeros
				VecType s = _mm_or_ps(_mm_and_ps(mask_l_gt_half, s1), _mm_andnot_ps(mask_l_gt_half, s2)); // if (l > 0.5) s = s1 else s = s2

				// Handle cases where diff is zero (i.e., max == min), which means s should be zero
				const VecType mask_diff_zero = _mm_cmpeq_ps(diff, v0); // diff == 0 ? all-ones : all-zeros
				s = _mm_blendv_ps(s, v0, mask_diff_zero); // if (diff == 0) s = 0 else s = s

				// Compute h based on which channel is the maximum
				// t_r = (g - b)/diff + (g < b ? 6 : 0)
				// t_g = (b - r)/diff + 2
				// t_b = (r - g)/diff + 4
				// choose based on which channel is max

				// Compute the differences normalized by diff any NaNs will be discarded later
				// by masking with the max channel
				VecType t_r = _mm_div_ps(_mm_sub_ps(g, b), diff);
				VecType t_g = _mm_add_ps(_mm_div_ps(_mm_sub_ps(b, r), diff), v2);
				VecType t_b = _mm_add_ps(_mm_div_ps(_mm_sub_ps(r, g), diff), v4);

				// Add 6 to t_r where g < b
				const VecType mask_g_lt_b = _mm_cmplt_ps(g, b); // g < b ? all-ones : all-zeros
				t_r = _mm_add_ps(t_r, _mm_and_ps(mask_g_lt_b, v6));

				// Create masks for which channel is the maximum
				const VecType mask_r_max = _mm_cmpeq_ps(maximum, r); // r == max ? all-ones : all-zeros
				const VecType mask_g_max = _mm_cmpeq_ps(maximum, g); // g == max ? all-ones : all-zeros
				const VecType mask_r_or_g_max = _mm_or_ps(mask_r_max, mask_g_max); // r == max || g == max ? all-ones : all-zeros
				const VecType mask_b_max = _mm_andnot_ps(mask_r_or_g_max, full_mask); // b == max ? all-ones : all-zeros

				// Select the h candidate value based on which channel is the maximum
				VecType h_candidate = _mm_or_ps(_mm_and_ps(mask_r_max, t_r), _mm_andnot_ps(mask_r_max, v0)); // if r is max, use t_r, else 0
				h_candidate = _mm_or_ps(_mm_and_ps(mask_g_max, t_g), _mm_andnot_ps(mask_g_max, h_candidate)); // if g is max, use t_g, else previous
				h_candidate = _mm_or_ps(_mm_and_ps(mask_b_max, t_b), _mm_andnot_ps(mask_b_max, h_candidate)); // if b is max, use t_b, else previous

				// h = 60 * h_candidate except when diff == 0, in which case h = 0
				VecType h = _mm_mul_ps(h_candidate, v60);
				h = _mm_andnot_ps(mask_diff_zero, h); // if diff == 0, h = 0 else h = h

				// Wrap h to [0, 360)	
				VecType div = _mm_div_ps(h, v360);
				__m128i q_i = _mm_cvttps_epi32(div);
				VecType q_f = _mm_cvtepi32_ps(q_i);
				h = _mm_sub_ps(h, _mm_mul_ps(q_f, v360));

				return { h, s, l };
			};

		//
		// Now convert the RGB values to HSL, so we can adjust the saturation
		// There's no need to have fall back code for the last few pixels,
		// as the vector size is always a multiple of 8.
		//
#pragma omp parallel for default(shared) schedule(static) if (nrProcessors > 1) 
		for (std::int64_t n = 0; n < len; ++n)
		{
			const auto [h, s, l] = toHSL(
				_mm_loadu_ps(redBuffer.data() + n * vectorLength),
				_mm_loadu_ps(greenBuffer.data() + n * vectorLength),
				_mm_loadu_ps(blueBuffer.data() + n * vectorLength)
			);
			_mm_storeu_ps(redBuffer.data() + n * vectorLength, h);
			_mm_storeu_ps(greenBuffer.data() + n * vectorLength, s);
			_mm_storeu_ps(blueBuffer.data() + n * vectorLength, l);
		}

	}

	void adjustSaturation_sse2(std::vector<float>& greenBuffer, float saturationShift, float vibranceFactor)
	{
		[[maybe_unused]] const int nrProcessors{ Multitask::GetNrProcessors() };
		using VecType = __m128;
		constexpr size_t vectorLength = sizeof(VecType) / sizeof(float);
		std::int64_t len = static_cast<std::int64_t>(greenBuffer.size()) / vectorLength;

		VecType minVal = _mm_set1_ps(0.0f);
		VecType maxVal = _mm_set1_ps(1.0f);

		float* const pGreen = greenBuffer.data();

		//
		// Now adjust the saturation for each pixel based on the saturationShift and vibranceFactor
		// There's no need to have fall back code for the last few pixels,
		// as the vector size is always a multiple of 8.
		//
#pragma omp parallel for default(shared) schedule(static) if (nrProcessors > 1) 
		for (std::int64_t n = 0; n < len ; ++n)
		{
			const VecType s = _mm_loadu_ps(pGreen + n * vectorLength);	// Load 4 saturation values
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
			adjustedSaturation = _mm_mul_ps(adjustedSaturation, _mm_add_ps(_mm_set1_ps(1.0f), _mm_mul_ps(mask, _mm_set1_ps(vibranceFactor))));
			adjustedSaturation = _mm_min_ps(_mm_max_ps(adjustedSaturation, minVal), maxVal);	// Clamp to [0, 1]

			_mm_storeu_ps(pGreen + n * vectorLength, adjustedSaturation);
		}
	}


	// H,S,L -> R,G,B vectorized with SSE2 (4 floats per loop).
	// h: degrees in [0,360) (per-pixel), s,l in [0,1].
	// r,g,b outputs in [0,1].
	// n = number of pixels.
	// 
	// On input redBuffer, greenBuffer, blueBuffer contain H,S,L values, on output they contain R,G,B values.
	// There's no need to have fall back code for the last few pixels,
	// as the vector size is always a multiple of 8.
	//
	void hslToRgb_sse2(std::vector<float>& redBuffer, std::vector<float>& greenBuffer, std::vector<float>& blueBuffer)
	{
		[[maybe_unused]] const int nrProcessors{ Multitask::GetNrProcessors() };
		using VecType = __m128;
		constexpr size_t vectorLength = sizeof(VecType) / sizeof(float);
		std::int64_t len = static_cast<std::int64_t>(greenBuffer.size()) / vectorLength;

		const VecType v0 = _mm_set1_ps(0.0f);
		const VecType v1 = _mm_set1_ps(1.0f);
		const VecType v_half = _mm_set1_ps(0.5f);
		const VecType v6 = _mm_set1_ps(6.0f);
		const VecType inv360 = _mm_set1_ps(1.0f / 360.0f);
		const VecType one_th = _mm_set1_ps(1.0f / 3.0f);   // 1/3
		const VecType two_th = _mm_set1_ps(2.0f / 3.0f);   // 2/3
		const VecType one_six = _mm_set1_ps(1.0f / 6.0f);

		//
		// Convert the HSL values back to RGB 
		// There's no need to have fall back code for the last few pixels,
		// as the vector size is always a multiple of 8.
		//
#pragma omp parallel for default(shared) schedule(static) if (nrProcessors > 1) 
		for (std::int64_t n = 0; n < len; ++n) {
			const VecType vh = _mm_loadu_ps(redBuffer.data() + n * vectorLength);
			const VecType vs = _mm_loadu_ps(greenBuffer.data() + n * vectorLength);
			const VecType vl = _mm_loadu_ps(blueBuffer.data() + n * vectorLength);

			// hk = h / 360  (assumes h in [0,360) so hk in [0,1) )
			const VecType vhk = _mm_mul_ps(vh, inv360);

			// t values for each channel
			VecType t_r = _mm_add_ps(vhk, one_th);
			VecType t_g = vhk;
			VecType t_b = _mm_sub_ps(vhk, one_th);

			// wrap t into [0,1) using masks (t < 0 -> +1, t > 1 -> -1)
			const VecType mask_tr_lt0 = _mm_cmplt_ps(t_r, v0);
			t_r = _mm_or_ps(_mm_and_ps(mask_tr_lt0, _mm_add_ps(t_r, v1)),
				_mm_andnot_ps(mask_tr_lt0, t_r));
			const VecType mask_tr_gt1 = _mm_cmpgt_ps(t_r, v1);
			t_r = _mm_or_ps(_mm_and_ps(mask_tr_gt1, _mm_sub_ps(t_r, v1)),
				_mm_andnot_ps(mask_tr_gt1, t_r));

			const VecType mask_tg_lt0 = _mm_cmplt_ps(t_g, v0);
			t_g = _mm_or_ps(_mm_and_ps(mask_tg_lt0, _mm_add_ps(t_g, v1)),
				_mm_andnot_ps(mask_tg_lt0, t_g));
			const VecType mask_tg_gt1 = _mm_cmpgt_ps(t_g, v1);
			t_g = _mm_or_ps(_mm_and_ps(mask_tg_gt1, _mm_sub_ps(t_g, v1)),
				_mm_andnot_ps(mask_tg_gt1, t_g));

			const VecType mask_tb_lt0 = _mm_cmplt_ps(t_b, v0);
			t_b = _mm_or_ps(_mm_and_ps(mask_tb_lt0, _mm_add_ps(t_b, v1)),
				_mm_andnot_ps(mask_tb_lt0, t_b));
			const VecType mask_tb_gt1 = _mm_cmpgt_ps(t_b, v1);
			t_b = _mm_or_ps(_mm_and_ps(mask_tb_gt1, _mm_sub_ps(t_b, v1)),
				_mm_andnot_ps(mask_tb_gt1, t_b));

			// q = (l < 0.5) ? l*(1+s) : (l + s - l*s)
			const VecType mask_l_lt_half = _mm_cmplt_ps(vl, v_half);
			const VecType q1 = _mm_mul_ps(vl, _mm_add_ps(v1, vs));               // l*(1+s)
			const VecType q2 = _mm_sub_ps(_mm_add_ps(vl, vs), _mm_mul_ps(vl, vs)); // l + s - l*s
			const VecType vq = _mm_or_ps(_mm_and_ps(mask_l_lt_half, q1),
				_mm_andnot_ps(mask_l_lt_half, q2));

			// p = 2*l - q
			const VecType vp = _mm_sub_ps(_mm_mul_ps(_mm_set1_ps(2.0f), vl), vq);

			// q_minus_p
			const VecType qmp = _mm_sub_ps(vq, vp);

			// hue2rgb for t_r, t_g, t_b
			auto hue2rgb_vec = [&](const VecType& tval)->VecType {
				// res1 = p + (q-p)*6*t   (t < 1/6)
				const VecType res1 = _mm_add_ps(vp, _mm_mul_ps(_mm_mul_ps(qmp, v6), tval));
				// res2 = q (t < 1/2)
				const VecType res2 = vq;
				// res3 = p + (q-p)*(2/3 - t) * 6 (t < 2/3)
				const VecType res3 = _mm_add_ps(vp, _mm_mul_ps(_mm_mul_ps(qmp, v6), _mm_sub_ps(two_th, tval)));
				// fallback p
				const VecType resp = vp;

				const VecType m1 = _mm_cmplt_ps(tval, one_six); // t < 1/6
				const VecType m2 = _mm_cmplt_ps(tval, v_half);  // t < 1/2
				const VecType m3 = _mm_cmplt_ps(tval, two_th);  // t < 2/3

				// exclusive masks
				const VecType m2_only = _mm_andnot_ps(m1, m2);                // !m1 & m2
				const VecType m1_or_m2 = _mm_or_ps(m1, m2);
				const VecType m3_only = _mm_andnot_ps(m1_or_m2, m3);          // ! (m1|m2) & m3

				// start with p
				VecType res = resp;
				// apply m3_only
				res = _mm_or_ps(_mm_and_ps(m3_only, res3), _mm_andnot_ps(m3_only, res));
				// apply m2_only
				res = _mm_or_ps(_mm_and_ps(m2_only, res2), _mm_andnot_ps(m2_only, res));
				// apply m1 (highest priority)
				res = _mm_or_ps(_mm_and_ps(m1, res1), _mm_andnot_ps(m1, res));

				return res;
				};

			VecType vr = hue2rgb_vec(t_r);
			VecType vg = hue2rgb_vec(t_g);
			VecType vb = hue2rgb_vec(t_b);

			// where s == 0 -> achromatic: r=g=b = l
			VecType mask_s_eq0 = _mm_cmpeq_ps(vs, v0);
			vr = _mm_or_ps(_mm_and_ps(mask_s_eq0, vl), _mm_andnot_ps(mask_s_eq0, vr));
			vg = _mm_or_ps(_mm_and_ps(mask_s_eq0, vl), _mm_andnot_ps(mask_s_eq0, vg));
			vb = _mm_or_ps(_mm_and_ps(mask_s_eq0, vl), _mm_andnot_ps(mask_s_eq0, vb));

			_mm_storeu_ps(redBuffer.data() + n * vectorLength, vr);
			_mm_storeu_ps(greenBuffer.data() + n * vectorLength, vg);
			_mm_storeu_ps(blueBuffer.data() + n * vectorLength, vb);
		}
	}
}
namespace DSS
{
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
		adjustSaturation_sse2(m_vGreenPlane, saturationShift, vibranceFactor);

		//
		// Convert the HSL values back to RGB
		//
		hslToRgb_sse2(m_vRedPlane, m_vGreenPlane, m_vBluePlane);
	}
}
