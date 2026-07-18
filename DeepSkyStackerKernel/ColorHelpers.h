#pragma once
#include "ColorRef.h"

void CYMGToRGB(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
//void CYMGToRGB12(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
//void CYMGToRGB3(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
double ToRGB1(double rm1, double rm2, double rh);

double hue2rgb(double p, double q, double t);

void ToHSL(double Red, double Green, double Blue, double& H, double& S, double& L);

//
// Convert RGB values to HSL.
// r, g, b expected in [0, 1].
// 
// Returns h in degrees [0,360), s and l in [0,1].
//
std::tuple<double, double, double> rgbToHsl(double r, double g, double b);

void ToRGB(double H, double S, double L, double& Red, double& Green, double& Blue);

//
// Convert HSL values to RGB.
// h expected in degrees (any real value, will be wrapped to [0,360)),
// s and l in [0,1].
// 
// Returns r, g, b in [0,1].
// 	
std::tuple<double, double, double> hslToRgb(double h, double s, double l);

//double GetLuminance(const COLORREF crColor);	// <--- Don't this this is used.
double GetLuminance(const COLORREF16& crColor);
double ClampPixel(const double fValue);

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
void rgbToHsl_sse2(std::vector<float>& redBuffer, std::vector<float>& greenBuffer, std::vector<float>& blueBuffer);

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
void hslToRgb_sse2(std::vector<float>& redBuffer, std::vector<float>& greenBuffer, std::vector<float>& blueBuffer);
