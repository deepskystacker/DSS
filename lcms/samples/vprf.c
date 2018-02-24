
//
//  Little cms
//  Copyright (C) 1998-2006 Marti Maria
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

// Version 1.14

#include "lcms.h"

// Virtual profiles are handled here.

cmsHPROFILE OpenStockProfile(const char* File)
{   
       if (!File) 
            return cmsCreate_sRGBProfile();    
       
       if (stricmp(File, "*Lab") == 0)
                return cmsCreateLabProfile(NULL);
       
       if (stricmp(File, "*Lab4") == 0)
                return cmsCreateLab4Profile(NULL);
       
       if (stricmp(File, "*LabD65") == 0) {

           cmsCIExyY D65xyY;
           
           cmsWhitePointFromTemp(6504, &D65xyY);           
           return cmsCreateLabProfile(&D65xyY);
       }

       if (stricmp(File, "*XYZ") == 0)
                return cmsCreateXYZProfile();


       if (stricmp(File, "*Gray22") == 0) {

           LPGAMMATABLE Gamma = cmsBuildGamma(256, 2.2);
           cmsHPROFILE hProfile = cmsCreateGrayProfile(cmsD50_xyY(), Gamma);
           cmsFreeGamma(Gamma);
           return hProfile;
       }

       if (stricmp(File, "*srgb") == 0)
				return cmsCreate_sRGBProfile();

       if (stricmp(File, "*null") == 0)
				return cmsCreateNULLProfile();

           
        return cmsOpenProfileFromFile(File, "r");
}
