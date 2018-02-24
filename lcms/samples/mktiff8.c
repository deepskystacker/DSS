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
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "lcms.h"



static
double DecodeAbTIFF(double ab)
{
	if (ab <= 128.)
		ab += 127.;
	else
		ab -= 127.;

	return ab;
}


static
LPGAMMATABLE CreateStep(void)
{
	LPGAMMATABLE Gamma = cmsAllocGamma(4096);
	LPWORD Table = Gamma ->GammaTable;
	int i;
	double a;

	for (i=0; i < 4096; i++) {

		a = (double) i * 255. / 4095.;

		a = DecodeAbTIFF(a);

		Table[i] = (WORD) floor(a * 257. + 0.5);
	}

	return Gamma;
}


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


static
int Identity(register WORD In[], register WORD Out[], register LPVOID Cargo)
{
	Out[0] = In[0];
	Out[1] = In[1];
	Out[2] = In[2];
	return TRUE;
}




int main(int argc, char *argv[])
{
	cmsHPROFILE hProfile;
	LPLUT AToB0, BToA0;
	LPGAMMATABLE PreLinear[3];
	LPGAMMATABLE Lin, Step;

	fprintf(stderr, "Creating lcmstiff8.icm...");

    
    unlink("lcmstiff8.icm");
	hProfile = cmsOpenProfileFromFile("lcmstiff8.icm", "w");

	// Create linearization
	Lin  = CreateLinear();
	Step = CreateStep();

	PreLinear[0] = Lin;
	PreLinear[1] = Step;
	PreLinear[2] = Step;

    AToB0 = cmsAllocLUT();
	BToA0 = cmsAllocLUT();

	cmsAlloc3DGrid(AToB0, 2, 3, 3);
	cmsAlloc3DGrid(BToA0, 2, 3, 3);

	cmsSample3DGrid(AToB0, Identity, NULL, 0);
	cmsSample3DGrid(BToA0, Identity, NULL, 0);
	
	cmsAllocLinearTable(AToB0, PreLinear,  1);   
	cmsAllocLinearTable(BToA0, PreLinear,  2);   

    cmsAddTag(hProfile, icSigAToB0Tag, AToB0);
	cmsAddTag(hProfile, icSigBToA0Tag, BToA0);

	cmsAddTag(hProfile, icSigProfileDescriptionTag, "Little cms Tiff8 CIELab");
    cmsAddTag(hProfile, icSigCopyrightTag,          "Copyright (c) Marti Maria, 2003. All rights reserved.");
    cmsAddTag(hProfile, icSigDeviceMfgDescTag,      "Little cms");    
    cmsAddTag(hProfile, icSigDeviceModelDescTag,    "TIFF Lab8");
	

	cmsCloseProfile(hProfile);

	cmsFreeGamma(Lin);
	cmsFreeGamma(Step);
	cmsFreeLUT(AToB0);
	cmsFreeLUT(BToA0);

	
	fprintf(stderr, "Done.\n");

	return 0;
}
