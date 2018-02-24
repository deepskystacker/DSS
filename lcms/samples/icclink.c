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

#include "lcms.h"
#include <stdarg.h>

// xgetopt() interface -----------------------------------------------------

extern int   xoptind;
extern char *xoptarg;
extern int   xopterr;
extern char  SW;
int    cdecl xgetopt(int argc, char *argv[], char *optionS);

// ------------------------------------------------------------------------

static char* Description = "Devicelink profile";
static int Intent = INTENT_PERCEPTUAL;
static char *cOutProf = "devicelink.icm";

static int PrecalcMode = 1;
static int NumOfGridPoints = 0;

static BOOL BlackPointCompensation = FALSE;
static int  BlackPreservation      = 0;

static double InkLimit             = 400;
static BOOL lUse8bits = FALSE;
static BOOL TagResult = FALSE;




static
void FatalError(const char *frm, ...)
{
       va_list args;

       va_start(args, frm);
       vfprintf(stderr, frm, args);
       va_end(args);

       exit(1);
}


static
void Help(int level)
{
    switch(level) {

     default:
     case 0:

     fprintf(stderr, "\nLinks two or more profiles into a single devicelink profile.\n");     
	 fprintf(stderr, "Colorspaces must be paired except Lab/XYZ, that can be interchanged.\n");
	 fprintf(stderr, "\n");     
     fprintf(stderr, "usage: icclink [flags] <profiles>\n\n");
     fprintf(stderr, "flags:\n\n");         
     fprintf(stderr, "%co<profile> - Output devicelink profile. [defaults to 'devicelink.icm']\n", SW);        
     fprintf(stderr, "%ct<0,1,2,3> - Intent (0=Perceptual, 1=Colorimetric, 2=Saturation, 3=Absolute)\n", SW);    
	 fprintf(stderr, "%cc<0,1,2> - Precission (0=LowRes, 1=Normal, 2=Hi-res) [defaults to 1]\n", SW);     
     fprintf(stderr, "%cn<gridpoints> - Alternate way to set precission, number of CLUT points\n", SW);     
     fprintf(stderr, "%cd<description> - description text (quotes can be used)\n", SW);     
     fprintf(stderr, "\n%cb - Black point compensation\n", SW);
     fprintf(stderr, "%cf<0,1> - Black preserving 0=off, 1=K ink only 2=K plane\n", SW);
     fprintf(stderr, "\n%ck<0..400> - Ink-limiting in %% (CMYK only)\n", SW);
     fprintf(stderr, "%c8 - Creates 8-bit devicelink\n", SW);
     fprintf(stderr, "%cx - Creatively, guess deviceclass of resulting profile.\n", SW);
     fprintf(stderr, "\n");
     fprintf(stderr, "%ch<0,1,2,3> - More help\n", SW);
     break;

     case 1:

     fprintf(stderr, "\nBuilt-in profiles:\n\n");
     fprintf(stderr, "\t*Lab  -- D50-based CIEL*a*b (PCS)\n"
                     "\t*XYZ  -- CIE XYZ (PCS)\n"
                     "\t*sRGB -- sRGB color space\n" 
                     "\t*Gray22- Monochrome of Gamma 2.2\n"
                     "\t*Lin2222- CMYK linearization of gamma 2.2 on each channel\n");
     break;

     case 2:

     fprintf(stderr, "\nExamples:\n\n"
                     "To create 'devicelink.icm' from a.icc to b.icc:\n"
                     "\ticclink a.icc b.icc\n\n"
                     "To create 'out.icc' from sRGB to cmyk.icc:\n"
                     "\ticclink -o out.icc *sRGB cmyk.icc\n\n"
                     "To create a sRGB input profile working in Lab:\n"
                     "\ticclink -x -o sRGBLab.icc *sRGB *Lab\n\n"
                     "To create a XYZ -> sRGB output profile:\n"
                     "\ticclink -x -o sRGBLab.icc *XYZ *sRGB\n\n"
                     "To create a abstract profile doing softproof for cmyk.icc:\n"
                     "\ticclink -t1 -x -o softproof.icc *Lab cmyk.icc cmyk.icc *Lab\n\n"
                     "To create a 'grayer' sRGB input profile:\n"
                     "\ticclink -x -o grayer.icc *sRGB gray.icc gray.icc *Lab\n\n"
                     "To embed ink limiting into a cmyk output profile:\n"
                     "\ticclink -x -o cmyklimited.icc -k 250 cmyk.icc *Lab\n\n");                     
      break;                       

     case 3:
	 
     fprintf(stderr, "This program is intended to be a demo of the little cms\n"
                     "engine. Both lcms and this program are freeware. You can\n"
                     "obtain both in source code at http://www.littlecms.com\n"
                     "For suggestions, comments, bug reports etc. send mail to\n"
                     "info@littlecms.com\n\n");
    }

    exit(0);
}

// The toggles stuff

static
void HandleSwitches(int argc, char *argv[])
{
       int s;
      
       while ((s = xgetopt(argc,argv,"xXH:h:8k:K:BbO:o:T:t:D:d:C:c:n:N:f:F:")) != EOF) {

       switch (s){

	 
       case '8':
            lUse8bits = TRUE;
            break;

       case 'd':
       case 'D':
            Description = xoptarg;
            break;

       case 'o':
       case 'O':
           cOutProf = xoptarg;
            break;


       case 't':
       case 'T':
            Intent = atoi(xoptarg);
            if (Intent > 3) Intent = 3;
            if (Intent < 0) Intent = 0;
            break;
     
		case 'c':
        case 'C':
            PrecalcMode = atoi(xoptarg);
            if (PrecalcMode < 0 || PrecalcMode > 2)
                    FatalError("ERROR: Unknown precalc mode '%d'", PrecalcMode);
            break;


        case 'n':
        case 'N':
                if (PrecalcMode != 1)
                    FatalError("Precalc mode already specified");
                NumOfGridPoints = atoi(xoptarg);
                break;

        case 'b':
        case 'B':
            BlackPointCompensation = TRUE;
            break;
			
        case 'f':
        case 'F': 
                BlackPreservation = atoi(xoptarg);
                if (BlackPreservation < 0 || BlackPreservation > 2)
                    FatalError("ERROR: Unknown black preservation mode '%d'", BlackPreservation);
                break;

        case 'k':
        case 'K':
                InkLimit = atof(xoptarg);
                if (InkLimit < 0.0 || InkLimit > 400.0)
                        FatalError("Ink limit must be 0%%..400%%");
                break;
        
        case 'x':
        case 'X': TagResult = TRUE;
                  break;

        case 'h':
        case 'H':
                Help(atoi(xoptarg));
                break;
        
     default:

       FatalError("Unknown option - run without args to see valid ones.\n");
    }       
    }
}

static
cmsHPROFILE OpenProfile(const char* File)
{
	cmsHPROFILE h;

       if (!File) 
            return cmsCreate_sRGBProfile();    
       
       if (stricmp(File, "*Lab") == 0)
                return cmsCreateLabProfile(NULL);
       
       if (stricmp(File, "*XYZ") == 0)
                return cmsCreateXYZProfile();
         
	   if (stricmp(File, "*srgb") == 0)
				return cmsCreate_sRGBProfile();

       
	   if (stricmp(File, "*Gray22") == 0) {
		   LPGAMMATABLE Gamma = cmsBuildGamma(256, 2.2);
		   cmsHPROFILE hProfile = cmsCreateGrayProfile(cmsD50_xyY(), Gamma);
		   cmsFreeGamma(Gamma);
		   return hProfile;

	   }
       
       if (stricmp(File, "*Lin2222") == 0) {

            LPGAMMATABLE Gamma = cmsBuildGamma(256, 2.2);
            LPGAMMATABLE Gamma4[4];
            cmsHPROFILE hProfile; 

            Gamma4[0] = Gamma4[1] = Gamma4[2] = Gamma4[3] = Gamma;
            hProfile = cmsCreateLinearizationDeviceLink(icSigCmykData, Gamma4);
		    cmsFreeGamma(Gamma);
		    return hProfile;

	   }


       h = cmsOpenProfileFromFile(File, "r");

	   
       if (cmsGetDeviceClass(h) == icSigNamedColorClass)
			FatalError("ERROR: Cannot make devicelink of named color profiles!");
       

	   return h;
}

static
int MyErrorHandler(int ErrorCode, const char *ErrorText)
{
    FatalError("icclink: %s", ErrorText);
    return 0;
}




int main(int argc, char *argv[])
{
	int i, nargs;
	cmsHPROFILE Profiles[257];
	cmsHPROFILE hProfile;
	DWORD dwFlags = 0;
	cmsHTRANSFORM hTransform;
    

     fprintf(stderr, "little cms device link generator - v1.6\n");

	 HandleSwitches(argc, argv);

     cmsSetErrorHandler(MyErrorHandler);

     nargs = (argc - xoptind);
	 if (nargs < 1)
				Help(0); 
	 
	 if (nargs > 255)
			FatalError("ERROR: Holy profile! what are you trying to do with so many profiles?");


	 for (i=0; i < nargs; i++) {
		 Profiles[i] = OpenProfile(argv[i + xoptind]);
	 }

	

	 switch (PrecalcMode) {
           	
	    case 0: dwFlags |= cmsFLAGS_LOWRESPRECALC; break;
		case 2: dwFlags |= cmsFLAGS_HIGHRESPRECALC; break;
		case 1: 
            if (NumOfGridPoints > 0)
                dwFlags |= cmsFLAGS_GRIDPOINTS(NumOfGridPoints);
            break;

		default: FatalError("ERROR: Unknown precalculation mode '%d'", PrecalcMode);
	 }

     if (BlackPointCompensation)
            dwFlags |= cmsFLAGS_BLACKPOINTCOMPENSATION;

     if (BlackPreservation > 0) {

            dwFlags |= cmsFLAGS_PRESERVEBLACK;
            cmsSetCMYKPreservationStrategy(BlackPreservation-1);
     }

     if (TagResult)
            dwFlags |= cmsFLAGS_GUESSDEVICECLASS;

     if (InkLimit != 400.0) {

            cmsHPROFILE hInkLimit = cmsCreateInkLimitingDeviceLink(
                                    cmsGetColorSpace(Profiles[nargs-1]), InkLimit);

            Profiles[nargs++] = hInkLimit;
     }

     if (lUse8bits) dwFlags |= cmsFLAGS_NOPRELINEARIZATION;

	 hTransform = cmsCreateMultiprofileTransform(Profiles, nargs, 0, 0, Intent, dwFlags);
	 if (hTransform) {

        size_t size = sizeof(int) + nargs * sizeof(cmsPSEQDESC);
        LPcmsSEQ pseq = (LPcmsSEQ) malloc(size);
        
        ZeroMemory(pseq, size);
        pseq ->n = nargs;

        for (i=0; i < nargs; i++) {

            strcpy(pseq ->seq[i].Manufacturer, cmsTakeManufacturer(Profiles[i]));
            strcpy(pseq ->seq[1].Model, cmsTakeModel(Profiles[i]));
        }
	       
		hProfile = 	cmsTransform2DeviceLink(hTransform, dwFlags);

		cmsAddTag(hProfile, icSigProfileDescriptionTag, (LPVOID) Description);
		cmsAddTag(hProfile, icSigCopyrightTag, (LPVOID) "Generated by littlecms icclink. No copyright, use freely");
        cmsAddTag(hProfile, icSigProfileSequenceDescTag, (LPVOID) pseq);

        if (lUse8bits) _cmsSetLUTdepth(hProfile, 8);

		if (_cmsSaveProfile(hProfile, cOutProf)) 
				fprintf(stderr, "Ok");
		else 
				fprintf(stderr, "Error saving file!");

		cmsCloseProfile(hProfile);
        free(pseq);
	 }

	 cmsDeleteTransform(hTransform);

	 for (i=0; i < nargs; i++) {
		 cmsCloseProfile(Profiles[i]);
	 }

		 	
     return 0;     
}
