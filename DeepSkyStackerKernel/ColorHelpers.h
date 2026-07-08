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
