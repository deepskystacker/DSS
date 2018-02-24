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


#define GRID_POINTS 33

static
int Forward(register WORD In[], register WORD Out[], register LPVOID Cargo)
{	
    cmsCIELab Lab;

   

    cmsLabEncoded2Float(&Lab, In);
    Lab.a = Lab.b = 0;
    cmsFloat2LabEncoded(Out, &Lab);
	
	return TRUE;
}




	
int main(int argc, char *argv[])
{
	LPLUT AToB0;
	cmsHPROFILE hProfile;

	fprintf(stderr, "Creating grayer.icm...");

	unlink("grayer.icm");
	hProfile = cmsOpenProfileFromFile("grayer.icm", "w");


    AToB0 = cmsAllocLUT();
	 

	cmsAlloc3DGrid(AToB0, GRID_POINTS, 3, 3);
	    
	cmsSample3DGrid(AToB0, Forward, NULL, 0);
			
    cmsAddTag(hProfile, icSigAToB0Tag, AToB0);
	                                
	cmsSetColorSpace(hProfile, icSigLabData);
    cmsSetPCS(hProfile, icSigLabData);
    cmsSetDeviceClass(hProfile, icSigAbstractClass);

	cmsAddTag(hProfile, icSigProfileDescriptionTag, "Little cms Grayifier");
    cmsAddTag(hProfile, icSigCopyrightTag,          "Copyright (c) Marti Maria 2003. All rights reserved.");
    cmsAddTag(hProfile, icSigDeviceMfgDescTag,      "Little cms");    
    cmsAddTag(hProfile, icSigDeviceModelDescTag,    "Grayifier abstract profile");

	
	cmsCloseProfile(hProfile);
    
	cmsFreeLUT(AToB0);
	
	fprintf(stderr, "Done.\n");

	return 0;
}
