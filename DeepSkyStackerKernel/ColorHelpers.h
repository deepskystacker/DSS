#pragma once
#include "ColorRef.h"

void CYMGToRGB(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
void CYMGToRGB2(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
void CYMGToRGB3(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);
void ToHSL(double Red, double Green, double Blue, double& H, double& S, double& L);
void ToRGB(double H, double S, double L, double& Red, double& Green, double& Blue);
double ToRGB1(float rm1, float rm2, float rh);
//double GetLuminance(const COLORREF crColor);	// <--- Don't this this is used.
double GetLuminance(const COLORREF16& crColor);
double ClampPixel(const double fValue);