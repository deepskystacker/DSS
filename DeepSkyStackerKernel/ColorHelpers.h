#pragma once
#include "ColorRef.h"

void CYMGToRGB(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
//void CYMGToRGB12(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
//void CYMGToRGB3(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
void ToHSL(double Red, double Green, double Blue, double& H, double& S, double& L);
void ToRGB(double H, double S, double L, double& Red, double& Green, double& Blue);
double ToRGB1(double rm1, double rm2, double rh);
//double GetLuminance(const COLORREF crColor);	// <--- Don't this this is used.
double GetLuminance(const COLORREF16& crColor);
double ClampPixel(const double fValue);
