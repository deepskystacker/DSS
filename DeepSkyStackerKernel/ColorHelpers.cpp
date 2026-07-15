#include "pch.h"
#include "Multitask.h"
#include "avx_includes.h"
#include "ColorHelpers.h"

void CYMGToRGB(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue)
{
	fRed = fGreen = fBlue = 0;
	// Basic formulae
	// M = R + B
	// Y = R + G
	// C = B + G


	// RGB from CYM
	// R = (M+Y-C)/2
	// G = (Y+C-M)/2
	// B = (M+C-Y)/2
	fRed = std::max(0.0, fMagenta + fYellow - fCyan) / 2.0;
	fGreen = std::max(0.0, fYellow + fCyan - fMagenta) / 2.0;
	fBlue = std::max(0.0, fMagenta + fCyan - fYellow) / 2.0;

	/*	if (fGreen2)
		{
			fRed  *= fGreen2/fGreen;
			fBlue *= fGreen2/fGreen;
			fGreen = fGreen2;
		};
		fRed = std::min(fRed, 255);
		fBlue = std::min(fBlue, 255);
		fGreen = std::min(fGreen, 255);*/

		// RGB from CYG
		// G = G
		// R = Y - G
		// B = C - G
	fGreen += fGreen2;
	fRed += fYellow - fGreen2;
	fBlue += fCyan - fGreen2;

	// RGB from CMG
	// G = G
	// B = C - G
	// R = M - B = M - C + G
//	fGreen += fGreen2;
	fBlue += fCyan - fGreen2;
	fRed += fMagenta - fCyan + fGreen2;

	// RGB from YMG
	// G = G
	// R = Y - G
	// B = M - R = M - Y + G
//	fGreen += fGreen2;
	fRed += fYellow - fGreen2;
	fBlue += fMagenta - fYellow + fGreen2;

	// Average the results
	fRed /= 4.0;
	fBlue /= 4.0;
	fGreen /= 2.0;

	double	R = fRed;
	double	B = fBlue;
	double	G = fGreen;

	fRed = R * 1.29948 + G * 0.0289296 - B * 0.934432;
	fGreen = -0.409754 * R + 1.31042 * G - 0.523692 * B;
	fBlue = 0.110277 * R - 0.339351 * G + 2.45812 * B;

	fRed = std::max(0.0, std::min(255.0, fRed));
	fGreen = std::max(0.0, std::min(255.0, fGreen));
	fBlue = std::max(0.0, std::min(255.0, fBlue));
}
#if (0)
void CYMGToRGB12(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue)
{
	double			Y, U, V;
	double			R, G, B;

	Y = (fCyan + fYellow + fMagenta + fGreen2) / 2.0;
	U = -(fMagenta + fCyan - fYellow - fGreen2);//*0.492;
	V = (fMagenta + fYellow - fCyan - fGreen2);//*0.877;

	R = 1.164 * Y + 1.596 * (V - 128.0);
	G = 1.164 * Y - 0.813 * (V - 128.0) - 0.391 * (U - 128);
	B = 1.164 * Y + 2.018 * (U - 128.0);


	fRed = R * 1.29948 + G * 0.0289296 - B * 0.934432;
	fGreen = -0.409754 * R + 1.31042 * G - 0.523692 * B;
	fBlue = 0.110277 * R - 0.339351 * G + 2.45812 * B;

	//	fRed = (Y+1.13983*V)*255.0;
	//	fGreen = (Y-0.39465*U-0.5806*V)*255.0;
	//	fBlue  = (Y+2.03211*U)*255.0;

	fRed = std::max(0.0, std::min(255.0, fRed));
	fGreen = std::max(0.0, std::min(255.0, fGreen));
	fBlue = std::max(0.0, std::min(255.0, fBlue));
}

void CYMGToRGB3(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue)
{
	fRed = fGreen = fBlue = 0;
	// Basic formulae
	// M = R + B
	// Y = R + G
	// C = B + G

	// RGB from CYM
	// R = (M+Y-C)/2
	// G = (Y+C-M)/2
	// B = (M+C-Y)/2
	fRed = max(0.0, fMagenta + fYellow - fCyan) / 2.0;
	fGreen = max(0.0, fYellow + fCyan - fMagenta) / 2.0;
	fBlue = max(0.0, fMagenta + fCyan - fYellow) / 2.0;

	// RGB from CYG
	// G = G
	// R = Y - G
	// B = C - G
	fGreen += fGreen2;
	fRed += max(0.0, fYellow - fGreen2);
	fBlue += max(0.0, fCyan - fGreen2);

	// RGB from CMG
	// G = G
	// B = C - G
	// R = M - B = M - C + G
	fBlue += max(0.0, fCyan - fGreen2);
	fRed += max(0.0, fMagenta - fCyan + fGreen2);

	// RGB from YMG
	// G = G
	// R = Y - G
	// B = M - R = M - Y + G
	fRed += max(0.0, fYellow - fGreen2);
	fBlue += max(0.0, fMagenta - fYellow + fGreen2);

	// Average the results
	fRed /= 4.0;
	fBlue /= 4.0;
	fGreen /= 2.0;

	fRed = max(0.0, fRed);
	fBlue = max(0.0, fBlue);
	fGreen = max(0.0, fGreen);
}
#endif

double ToRGB1(double rm1, double rm2, double rh)
{
	if (rh > 360.0)
		rh -= 360.0;
	else if (rh < 0.0)
		rh += 360.0;

	if (rh < 60.0)
		rm1 = rm1 + (rm2 - rm1) * rh / 60.0;
	else if (rh < 180.0)
		rm1 = rm2;
	else if (rh < 240.0)
		rm1 = rm1 + (rm2 - rm1) * (240. - rh) / 60.0;

	return rm1 * 255.0;
}

double hue2rgb(double p, double q, double t)
{
	if (t < 0.0) t += 1.0;
	if (t > 1.0) t -= 1.0;
	if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
	if (t < 1.0 / 2.0) return q;
	if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
	return p;
}

void ToHSL(double Red, double Green, double Blue, double& H, double& S, double& L)
{
	double minval = std::min(Red, std::min(Green, Blue));
	double maxval = std::max(Red, std::max(Green, Blue));
	double mdiff = maxval - minval;
	double msum = maxval + minval;

	L = msum / 510.0;

	if (maxval == minval)
	{
		S = 0.0;
		H = 0.0;
	}
	else
	{
		double rnorm = (maxval - Red) / mdiff;
		double gnorm = (maxval - Green) / mdiff;
		double bnorm = (maxval - Blue) / mdiff;

		S = (L <= 0.5) ? (mdiff / msum) : (mdiff / (510.0 - msum));

		if (Red == maxval) H = 60.0 * (6.0 + bnorm - gnorm);
		if (Green == maxval) H = 60.0 * (2.0 + rnorm - bnorm);
		if (Blue == maxval) H = 60.0 * (4.0 + gnorm - rnorm);
		if (H > 360.0)
			H = H - 360.0;
	}
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

void ToRGB(double H, double S, double L, double& Red, double& Green, double& Blue)
{
	if (S == 0.0)
	{
		Red = Green = Blue = L * 255.0;
	}
	else
	{
		double rm1, rm2;

		if (L <= 0.5)
			rm2 = L + L * S;
		else
			rm2 = L + S - L * S;
		rm1 = 2.0 * L - rm2;

		Red = ToRGB1(rm1, rm2, H + 120.0);
		Green = ToRGB1(rm1, rm2, H);
		Blue = ToRGB1(rm1, rm2, H - 120.0);
	}
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

// --- Don't this this is used ---
/* Return the HSL luminance value. */
// double GetLuminance(const COLORREF crColor)
// {
// 	constexpr double scalingFactor = 1.0 / 256.0;
// 
// 	const unsigned red = GetRValue(crColor);
// 	const unsigned green = GetGValue(crColor);
// 	const unsigned blue = GetBValue(crColor);
// 
// 	const unsigned minval = std::min(red, std::min(green, blue));
// 	const unsigned maxval = std::max(red, std::max(green, blue));
// 	const unsigned msum = maxval + minval;
// 
// 	return static_cast<double>(msum) * (0.5 * scalingFactor);
// }

/* Return the HSL luminance value. */
double GetLuminance(const COLORREF16& crColor)
{
	constexpr double scalingFactor = 1.0 / 256.0;

	const unsigned minval = std::min(crColor.red, std::min(crColor.green, crColor.blue));
	const unsigned maxval = std::max(crColor.red, std::max(crColor.green, crColor.blue));
	const unsigned msum = maxval + minval;

	return static_cast<double>(msum) * (0.5 * scalingFactor * scalingFactor); // (((double)msum / 256.0) / 510.0);
}

double ClampPixel(const double fValue)
{
	constexpr double maxVal = 255.0 * 256.0;
	if (fValue < 0.0)
		return 0.0;
	if (fValue > maxVal)
		return maxVal;
	return fValue;
}

//
// Convert vectors of RGB values to HSL vectorized with SSE2 (4 floats per loop).
// r, g, b expected in [0, 1].
// 
// Returns h in degrees [0,360), s and l in [0,1].
//
// On input redBuffer, greenBuffer, blueBuffer contain R,G,B values, on output they contain H,S,L values.
// 
// There's no need to have fall back code for the last few pixels, as the vector size of CGrayBitmap
// is always a multiple of 32 bytes (8 floats).   This works perfectly for __m128 and __m256 types.
// If we ever decide to use __m512 types we will need to revisit the over-allocation in CGrayBitmapT<T>.
// 
// See CGrayBitmapT<T>::InitInternals() for details
// 
void rgbToHsl_sse2(std::vector<float>& redBuffer, std::vector<float>& greenBuffer, std::vector<float>& blueBuffer)
{
	[[maybe_unused]] const int nrProcessors{ Multitask::GetNrProcessors() };
	using VecType = __m128;
	constexpr size_t vectorLength = sizeof(VecType) / sizeof(float);
	std::int64_t len = static_cast<std::int64_t>(greenBuffer.size()) / vectorLength;
	ZASSERT(redBuffer.size() % 8 == 0);

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
			// SSE2 replacement for _mm_blendv_ps: zero s where diff == 0
			s = _mm_andnot_ps(mask_diff_zero, s); // if (diff == 0) s = 0 else keep s

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

// H,S,L -> R,G,B vectorized with SSE2 (4 floats per loop).
// h: degrees in [0,360) (per-pixel), s,l in [0,1].
// r,g,b outputs in [0,1].
// 
// On input redBuffer, greenBuffer, blueBuffer contain H,S,L values, on output they contain R,G,B values.
// 
// There's no need to have fall back code for the last few pixels, as the vector size of CGrayBitmap
// is always a multiple of 32 bytes (8 floats).   This works perfectly for __m128 and __m256 types.
// If we ever decide to use __m512 types we will need to revisit the over-allocation in CGrayBitmapT<T>.
// 
// See CGrayBitmapT<T>::InitInternals() for details
//
void hslToRgb_sse2(std::vector<float>& redBuffer, std::vector<float>& greenBuffer, std::vector<float>& blueBuffer)
{
	[[maybe_unused]] const int nrProcessors{ Multitask::GetNrProcessors() };
	using VecType = __m128;
	constexpr size_t vectorLength = sizeof(VecType) / sizeof(float);
	std::int64_t len = static_cast<std::int64_t>(greenBuffer.size()) / vectorLength;
	ZASSERT(redBuffer.size() % 8 == 0);

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
