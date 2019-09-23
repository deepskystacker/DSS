#include <stdafx.h>
#include "BitmapExt.h"
#include "AHDDemosaicing.h"
#include <vector>
#include "DSSTools.h"
#include "DSSProgress.h"


/* ------------------------------------------------------------------- */

static	std::vector<float>		g_vLUT;

/* ------------------------------------------------------------------- */

CRGBToLab::CRGBToLab()
{
	if (!g_vLUT.size())
	{
		g_vLUT.reserve(0x10000);
		for (LONG i = 0;i<0x10000;i++)
		{
			float		r = float(i) / 65535.0f;
			float		f = r > 0.008856f ? pow((float)r, (float)1.0/3.0f) : 7.787f*r + 16.0/116.0f;
			g_vLUT.push_back(f);
		};
	};
};

/* ------------------------------------------------------------------- */

void CRGBToLab::RGBToLab(double fRed, double fGreen, double fBlue, double & L, double & a, double & b)
{
	double			X, Y, Z;

	X = 0.433953*fRed + 0.376219*fGreen + 0.189828*fBlue;
	Y = 0.212671*fRed + 0.715160*fGreen + 0.072169*fBlue;
	Z = 0.017758*fRed + 0.109477*fGreen + 0.872766*fBlue;

	X = g_vLUT[(int)floor(X*65535.0)];
	Y = g_vLUT[(int)floor(Y*65535.0)];
	Z = g_vLUT[(int)floor(Z*65535.0)];

	L = 116.0*Y - 16.0;
	a = 500.0*(X - Y);
	b = 200.0*(Y - Z);
};

/* ------------------------------------------------------------------- */
