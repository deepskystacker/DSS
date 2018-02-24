//
//  Little cms
//  Copyright (C) 1998-2003 Marti Maria
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THIS SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//
// IN NO EVENT SHALL MARTI MARIA BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
// INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
// OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
// LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
// OF THIS SOFTWARE.
//
// Version 1.12


#include "lcms.h"


typedef struct {
				cmsHPROFILE   hLab;
				cmsHPROFILE   hRGB;
				cmsHTRANSFORM Lab2RGB;
				cmsHTRANSFORM RGB2Lab;

				} CARGO, FAR* LPCARGO;

static cmsCIExyY Cus;

// Does create our perceptual CIE-Based RGB space

static
cmsHPROFILE CreateRGBSpace(void)
{
	cmsHPROFILE hProfile;
	
	LPGAMMATABLE Gamma3[3];
	cmsCIExyYTRIPLE CIEPrimaries = {    {0.7355,0.2645 },
										{0.2658,0.7243 },
										{0.1669,0.0085}};


	Gamma3[0] = Gamma3[1] = Gamma3[2] = cmsBuildGamma(4096, 4.5);

	hProfile = cmsCreateRGBProfile(&Cus, &CIEPrimaries, Gamma3);

	cmsFreeGamma(Gamma3[0]);

	return hProfile;
}

	 
 
// Does create a linear ramp
static
LPGAMMATABLE CreateLinear()
{
	LPGAMMATABLE Gamma = cmsAllocGamma(4096);
	LPWORD Table = Gamma ->GammaTable;
	int i;

       for (i=0; i < 4096; i++) {

		   Table[i] = _cmsQuantizeVal(i, 4096);              		   

       }
       return Gamma;
}



// Our space will be CIE primaries plus a gamma of 4.5

static
int Forward(register WORD In[], register WORD Out[], register LPVOID Cargo)
{	
	LPCARGO C = (LPCARGO) Cargo;	
	WORD RGB[3];

	cmsDoTransform(C ->Lab2RGB, In, &RGB, 1);

	Out[0] = 0xFFFF - RGB[0]; // Our CMY is negative of RGB
	Out[1] = 0xFFFF - RGB[1]; 
	Out[2] = 0xFFFF - RGB[2]; 
	
	
	return TRUE;

}


static
int Reverse(register WORD In[], register WORD Out[], register LPVOID Cargo)
{	

	LPCARGO C = (LPCARGO) Cargo;	
	WORD RGB[3];

	RGB[0] = 0xFFFF - In[0];
	RGB[1] = 0xFFFF - In[1];
	RGB[2] = 0xFFFF - In[2];

	cmsDoTransform(C ->RGB2Lab, &RGB, Out, 1);
	
	return TRUE;

}



static
void InitCargo(LPCARGO Cargo)
{
	

	Cargo -> hLab = cmsCreateLabProfile(NULL);
	Cargo -> hRGB = CreateRGBSpace();
	
	Cargo->Lab2RGB = cmsCreateTransform(Cargo->hLab, TYPE_Lab_16, 
									    Cargo ->hRGB, TYPE_RGB_16,
										INTENT_RELATIVE_COLORIMETRIC, 
										cmsFLAGS_NOTPRECALC);

	Cargo->RGB2Lab = cmsCreateTransform(Cargo ->hRGB, TYPE_RGB_16, 
										Cargo ->hLab, TYPE_Lab_16, 
										INTENT_RELATIVE_COLORIMETRIC, 
										cmsFLAGS_NOTPRECALC);
}




static
void FreeCargo(LPCARGO Cargo)
{
	cmsDeleteTransform(Cargo ->Lab2RGB);
	cmsDeleteTransform(Cargo ->RGB2Lab);
	cmsCloseProfile(Cargo ->hLab);
	cmsCloseProfile(Cargo ->hRGB);
}

	
	
	
int main(int argc, char *argv[])
{
	LPLUT AToB0, BToA0;
	LPGAMMATABLE PreLinear[3];
	LPGAMMATABLE Lin;
	CARGO Cargo;
	cmsHPROFILE hProfile;
	cmsCIEXYZ wp;

	fprintf(stderr, "Creating lcmscmy.icm...");
	

	wp.X = 55.6549;
	wp.Y = 59.0485;
	wp.Z = 72.5494;
	
	cmsXYZ2xyY(&Cus, &wp);

	InitCargo(&Cargo);

	hProfile = cmsCreateLabProfile(&Cus);

	// Create linearization
	Lin  = CreateLinear();
	
	PreLinear[0] = Lin;
	PreLinear[1] = Lin;
	PreLinear[2] = Lin;

    AToB0 = cmsAllocLUT();
	BToA0 = cmsAllocLUT();

	cmsAlloc3DGrid(AToB0, 33, 3, 3);
	cmsAlloc3DGrid(BToA0, 33, 3, 3);

	cmsSample3DGrid(AToB0, Reverse, &Cargo, 0);
	cmsSample3DGrid(BToA0, Forward, &Cargo, 0);
	
	cmsAllocLinearTable(AToB0, PreLinear,  1);   
	cmsAllocLinearTable(BToA0, PreLinear, 2);   

    cmsAddTag(hProfile, icSigAToB0Tag, AToB0);
	cmsAddTag(hProfile, icSigBToA0Tag, BToA0);

	cmsSetColorSpace(hProfile, icSigCmyData);
	cmsSetDeviceClass(hProfile, icSigOutputClass);

	cmsAddTag(hProfile, icSigProfileDescriptionTag, "Little cms CMY mixing");
    cmsAddTag(hProfile, icSigCopyrightTag,          "Copyright (c) Marti Maria, 2005. All rights reserved.");
    cmsAddTag(hProfile, icSigDeviceMfgDescTag,      "Little cms");    
    cmsAddTag(hProfile, icSigDeviceModelDescTag,    "CMY mixing");

	_cmsSaveProfile(hProfile, "lcmscmy.icm");
	

	cmsFreeGamma(Lin);
	cmsFreeLUT(AToB0);
	cmsFreeLUT(BToA0);
	cmsCloseProfile(hProfile);	
	FreeCargo(&Cargo);
	fprintf(stderr, "Done.\n");



	return 0;
}
