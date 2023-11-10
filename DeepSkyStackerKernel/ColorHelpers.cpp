#include "stdafx.h"
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

void ToHSL(double Red, double Green, double Blue, double& H, double& S, double& L)
{
	double minval = std::min(Red, std::min(Green, Blue));
	double maxval = std::max(Red, std::max(Green, Blue));
	double mdiff = maxval - minval;
	double msum = maxval + minval;

	L = msum / 510.0f;

	if (maxval == minval)
	{
		S = 0.0f;
		H = 0.0f;
	}
	else
	{
		double rnorm = (maxval - Red) / mdiff;
		double gnorm = (maxval - Green) / mdiff;
		double bnorm = (maxval - Blue) / mdiff;

		S = (L <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

		if (Red == maxval) H = 60.0f * (6.0f + bnorm - gnorm);
		if (Green == maxval) H = 60.0f * (2.0f + rnorm - bnorm);
		if (Blue == maxval) H = 60.0f * (4.0f + gnorm - rnorm);
		if (H > 360.0f)
			H = H - 360.0f;
	}
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

		if (L <= 0.5f)
			rm2 = (double)(L + L * S);
		else
			rm2 = (double)(L + S - L * S);
		rm1 = (double)(2.0f * L - rm2);

		Red = ToRGB1(rm1, rm2, (double)(H + 120.0f));
		Green = ToRGB1(rm1, rm2, (double)H);
		Blue = ToRGB1(rm1, rm2, (double)(H - 120.0f));
	}
}

double ToRGB1(float rm1, float rm2, float rh)
{
	if (rh > 360.0f)
		rh -= 360.0f;
	else if (rh < 0.0f)
		rh += 360.0f;

	if (rh < 60.0f)
		rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
	else if (rh < 180.0f)
		rm1 = rm2;
	else if (rh < 240.0f)
		rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;

	return (rm1 * 255.0);
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