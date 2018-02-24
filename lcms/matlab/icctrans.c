//
//  Little cms
//  Copyright (C) 1998-2003 Marti Maria, Ignacio Ruiz de Conejo
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


// DESCRIPTION:
//      Apply a color transformation to a Matlab Structure.
//




#include "mex.h"
#include "lcms.h"

// xgetopt() interface -----------------------------------------------------

static int   xoptind;    
static char *xoptarg; 
static int   xopterr;  
static char  *letP;
static char   SW = '-';

// ------------------------------------------------------------------------


static int  Verbose ;			// Print some statistics
static char *cInProf;			// Input profile
static char *cOutProf;			// Output profile
static char *cProofing;			// Softproofing profile


static int  Intent;				// Rendering Intent
static int  ProofingIntent;		// RI for proof

static int  PrecalcMode;		// 0 = Not, 1=Normal, 2=Accurate, 3=Fast

static BOOL BlackPointCompensation;
static BOOL lIsDeviceLink;
static BOOL lMultiProfileChain;		// Multiple profile chain

static cmsHPROFILE hInput, hOutput, hProof;
static cmsHTRANSFORM hColorTransform;
static cmsHPROFILE hProfiles[255];
static int nProfiles;

static icColorSpaceSignature InputColorSpace, OutputColorSpace;
static int OutputChannels, InputChannels, nBytesDepth;


// Error. Print error message and abort

static
BOOL FatalError(const char *frm, ...)
{
    va_list args;
    char Buffer[1024];
    
    va_start(args, frm);
    vsprintf(Buffer, frm, args);
    mexErrMsgTxt(Buffer);   
    va_end(args);
    
    return FALSE;               
}

// This is the handler passed to lcms

static
int MatLabErrorHandler(int ErrorCode, const char *ErrorText)
{
    if (ErrorCode == LCMS_ERRC_ABORTED)
        
        mexErrMsgTxt(ErrorText);
    else
        mexWarnMsgTxt(ErrorText);
    
    return 1;
}
//
//  Parse the command line options, System V style.
//

static
void xoptinit()
{   
    xoptind = 1;
    xopterr = 0;
    letP = NULL;
}


static
int xgetopt(int argc, char *argv[], char *optionS)
{
    unsigned char ch;
    char *optP;
    
    
    
    if (SW == 0) {
        SW = '/';
    }
    
    if (argc > xoptind) {
        if (letP == NULL) {
            if ((letP = argv[xoptind]) == NULL ||
                *(letP++) != SW)  goto gopEOF;
            if (*letP == SW) {
                xoptind++;  goto gopEOF;
            }
        }
        if (0 == (ch = *(letP++))) {
            xoptind++;  goto gopEOF;
        }
        if (':' == ch  ||  (optP = strchr(optionS, ch)) == NULL)
            goto gopError;
        if (':' == *(++optP)) {
            xoptind++;
            if (0 == *letP) {
                if (argc <= xoptind)  goto  gopError;
                letP = argv[xoptind++];
            }
            xoptarg = letP;
            letP = NULL;
        } else {
            if (0 == *letP) {
                xoptind++;
                letP = NULL;
            }
            xoptarg = NULL;
        }
        return ch;
    }
gopEOF:
    xoptarg = letP = NULL;
    return EOF;
    
gopError:
    xoptarg = NULL;    
    if (xopterr)
        FatalError ("get command line option");
    return ('?');
}

//
// Convert from ICC enumerated type to lcms style
//

static
int ICC2LCMS(icColorSpaceSignature ProfileSpace)
{
    
       switch (ProfileSpace) {

       case icSigGrayData: return  PT_GRAY;
       case icSigRgbData:  return  PT_RGB;
       case icSigCmyData:  return  PT_CMY;
       case icSigCmykData: return  PT_CMYK;
       case icSigYCbCrData:return  PT_YCbCr;
       case icSigLuvData:  return  PT_YUV;
       case icSigXYZData:  return  PT_XYZ;
       case icSigLabData:  return  PT_Lab;
       case icSigLuvKData: return  PT_YUVK;
       case icSigHsvData:  return  PT_HSV;
       case icSigHlsData:  return  PT_HLS;
       case icSigYxyData:  return  PT_Yxy;

       case icSigHexachromeData: return PT_HiFi; 
       case icSig8colorData:     return PT_HiFi8;
         

       default:  return PT_ANY;
       }
}


// Return Mathlab type by depth

static
size_t SizeOfArrayType(const mxArray *Array)
{
	
  switch (mxGetClassID(Array))  {

     case mxINT8_CLASS:   return 1;
     case mxUINT8_CLASS:  return 1;
     case mxINT16_CLASS:  return 2;
     case mxUINT16_CLASS: return 2;     
     case mxDOUBLE_CLASS: return 0; // Special case -- lcms handles double as size=0
 

    default:
        FatalError("Unsupported data type");
        return 0;
    }
}


// Get number of pixels of input array. Supported arrays are 
// organized as NxMxD, being N and M the size of image and D the
// number of components.

static
size_t GetNumberOfPixels(const mxArray* In)
{
    int nDimensions  = mxGetNumberOfDimensions(In); 
    const int  *Dimensions   = mxGetDimensions(In);

    switch (nDimensions) {

        case 1: return 1;                            // It is just a spot color
        case 2: return Dimensions[0];                // A scanline
        case 3: return Dimensions[0]*Dimensions[1];  // A image

    default:
        FatalError("Unsupported array of %d dimensions", nDimensions);
        return 0;
    }
}   


// Allocates the output array. Copies the input array modifying the pixel
// definition to match "OutputChannels".

static
mxArray* AllocateOutputArray(const mxArray* In, int OutputChannels)
{       
 
	mxArray*	Out			  = mxDuplicateArray(In);   // Make a "deep copy" of Input array 
    int         nDimensions   = mxGetNumberOfDimensions(In);    
    const int*	Dimensions    = mxGetDimensions(In);
    int         InputChannels = Dimensions[nDimensions-1];


    // Modify pixel size only if needed

    if (InputChannels != OutputChannels) {
    
		
        int i, NewSize;
        int *ModifiedDimensions = (int*) mxMalloc(nDimensions * sizeof(int));
	

        CopyMemory(ModifiedDimensions, Dimensions, nDimensions * sizeof(int));
        ModifiedDimensions[nDimensions - 1] = OutputChannels;
        
		switch (mxGetClassID(In))  {

		case mxINT8_CLASS:   NewSize = sizeof(char); break;
		case mxUINT8_CLASS:  NewSize = sizeof(unsigned char); break;
		case mxINT16_CLASS:  NewSize = sizeof(short); break;
		case mxUINT16_CLASS: NewSize = sizeof(unsigned short); break;

		default:
		case mxDOUBLE_CLASS: NewSize = sizeof(double); break;
		}
 

        // NewSize = 1;
        for (i=0; i < nDimensions; i++)
            NewSize *= ModifiedDimensions[i];
        

        mxSetDimensions(Out, ModifiedDimensions, nDimensions);
        mxFree(ModifiedDimensions);
        
        mxSetPr(Out, mxRealloc(mxGetPr(Out), NewSize));             

    }


    return Out;
}
    

    
// Does create a format descriptor. "Bytes" is the sizeof type in bytes
//  
//  Bytes  Meaning
//  ------ --------
//   0      Floating point (double)
//   1      8-bit samples
//   2      16-bit samples   

static
DWORD MakeFormatDescriptor(icColorSpaceSignature ColorSpace, int Bytes)
{
    int Channels = _cmsChannelsOf(ColorSpace);
    return COLORSPACE_SH(ICC2LCMS(ColorSpace))|BYTES_SH(Bytes)|CHANNELS_SH(Channels)|PLANAR_SH(1);
}


// Opens a profile or proper built-in

static
cmsHPROFILE OpenProfile(const char* File)
{   
    
    if (!File) 
        return cmsCreate_sRGBProfile();    
    
	if (stricmp(File, "*sRGB") == 0)
        return cmsCreate_sRGBProfile(NULL);
    
    if (stricmp(File, "*Lab") == 0)
        return cmsCreateLabProfile(NULL);
    
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

    return cmsOpenProfileFromFile(File, "r");
}


static
DWORD GetFlags()
{
	DWORD dwFlags = 0; 
    
    switch (PrecalcMode) {
        
    case 0: dwFlags = cmsFLAGS_NOTPRECALC; break;
    case 2: dwFlags = cmsFLAGS_HIGHRESPRECALC; break;
    case 3: dwFlags = cmsFLAGS_LOWRESPRECALC; break;
    case 1: break;
           
       default: FatalError("Unknown precalculation mode '%d'", PrecalcMode);
    }

    if (BlackPointCompensation) 
        dwFlags |= cmsFLAGS_BLACKPOINTCOMPENSATION;

	return dwFlags;
}

// Create transforms

static
void OpenTransforms(int argc, char *argv[])
{
    
    DWORD dwIn, dwOut, dwFlags;
    

	if (lMultiProfileChain) {

		int i;
		cmsHTRANSFORM hTmp;

		
		nProfiles = argc - xoptind;
		for (i=0; i < nProfiles; i++) {

			hProfiles[i] = OpenProfile(argv[i+xoptind]);
		}

	
		// Create a temporary devicelink 

		hTmp = cmsCreateMultiprofileTransform(hProfiles, nProfiles, 
							0, 0, Intent, GetFlags());

		hInput = cmsTransform2DeviceLink(hTmp, 0);
		hOutput = NULL;
		cmsDeleteTransform(hTmp);

		InputColorSpace  = cmsGetColorSpace(hInput);
        OutputColorSpace = cmsGetPCS(hInput);        
		lIsDeviceLink = TRUE;

	}
	else
    if (lIsDeviceLink) {
        
        hInput  = cmsOpenProfileFromFile(cInProf, "r");
        hOutput = NULL;
        InputColorSpace  = cmsGetColorSpace(hInput);
        OutputColorSpace = cmsGetPCS(hInput);
        
        
    }
    else {
        
        hInput  = OpenProfile(cInProf);
        hOutput = OpenProfile(cOutProf);    
        
        InputColorSpace   = cmsGetColorSpace(hInput);
        OutputColorSpace  = cmsGetColorSpace(hOutput);
        
        if (cmsGetDeviceClass(hInput) == icSigLinkClass ||
            cmsGetDeviceClass(hOutput) == icSigLinkClass)   
            FatalError("Use %cl flag for devicelink profiles!\n", SW);
    
    }
    
    
    
    if (Verbose) {
        
        mexPrintf("From: %s\n", cmsTakeProductName(hInput));
        if (hOutput) mexPrintf("To  : %s\n\n", cmsTakeProductName(hOutput));
        
    }
    
        
    OutputChannels = _cmsChannelsOf(OutputColorSpace);
	InputChannels  = _cmsChannelsOf(InputColorSpace);
    

    dwIn  = MakeFormatDescriptor(InputColorSpace, nBytesDepth);
    dwOut = MakeFormatDescriptor(OutputColorSpace, nBytesDepth);
    
 
    dwFlags = GetFlags();
    
    if (cProofing != NULL) {

                   hProof = OpenProfile(cProofing);
                   dwFlags |= cmsFLAGS_SOFTPROOFING;
    }

   


     hColorTransform = cmsCreateProofingTransform(hInput, dwIn, 
                                          hOutput, dwOut, 
                                          hProof, Intent, 
                                          ProofingIntent, 
                                          dwFlags);
      
     
    
}



static
void ApplyTransforms(const mxArray *In, mxArray *Out)
{   
    double *Input  = mxGetPr(In); 
	double *Output = mxGetPr(Out);    
    size_t nPixels = GetNumberOfPixels(In);;
      	
    cmsDoTransform(hColorTransform, Input, Output, nPixels );

}


static
void CloseTransforms(void)
{
	int i;

    if (hColorTransform) cmsDeleteTransform(hColorTransform);
    if (hInput) cmsCloseProfile(hInput);
    if (hOutput) cmsCloseProfile(hOutput);             
    if (hProof) cmsCloseProfile(hProof);

	for (i=0; i < nProfiles; i++)
			cmsCloseProfile(hProfiles[i]);
       
    hColorTransform = NULL; hInput = NULL; hOutput = NULL; hProof = NULL;
}


static
void HandleSwitches(int argc, char *argv[])
{
    int  s;
    
    xoptinit();
    
    while ((s = xgetopt(argc, argv,"C:c:VvbBI:i:O:o:T:t:L:l:r:r:P:p:Mm")) != EOF) {
        
        
        switch (s){
            
        case 'b':
        case 'B': 
            BlackPointCompensation = TRUE;
            break;
            
        case 'c':
        case 'C':
            PrecalcMode = atoi(xoptarg);
            if (PrecalcMode < 0 || PrecalcMode > 3)
                FatalError("Unknown precalc mode '%d'", PrecalcMode);
            break;
            
            
            
        case 'v':
        case 'V':
            Verbose = TRUE;
            break;
            
            
            
        case 'i':
        case 'I':
            if (lIsDeviceLink)
                FatalError("Device-link already specified");
            cInProf = xoptarg;
            break;
            
        case 'o':
        case 'O':
            if (lIsDeviceLink)
                FatalError("Device-link already specified"); 
            cOutProf = xoptarg;
            break;
            
        case 't':
        case 'T':
            Intent = atoi(xoptarg);
            if (Intent > 3) Intent = 3;
            if (Intent < 0) Intent = 0;
            break;
            
            
        case 'l':
        case 'L': 
            cInProf = xoptarg;
            lIsDeviceLink = TRUE;
            break;
         
        case 'p':
        case 'P':
           cProofing = xoptarg;
           break;

     

        case 'r':
        case 'R':
            ProofingIntent = atoi(xoptarg);
            if (ProofingIntent > 3) ProofingIntent = 3;
            if (ProofingIntent < 0) ProofingIntent = 0;
            break;

     
		case 'm':
		case 'M':
			lMultiProfileChain = TRUE;
			break;

        default:
            FatalError("Unknown option.");
        }
    }

	 // For multiprofile, need to specify -m

     if (xoptind < argc) {

		if (!lMultiProfileChain)
				FatalError("Use %cm for multiprofile transforms", SW);
	}

}



// -------------------------------------------------- Print some fancy help
static
void PrintHelp(void)
{
    mexPrintf("(MX) little cms ColorSpace conversion tool - v0.3 BETA\n\n");
    
    mexPrintf("usage: icctrans (mVar, flags)\n\n");
    
    mexPrintf("mVar : Matlab array.\n");
    mexPrintf("flags: a string containing one or more of following options.\n\n");
    mexPrintf("\t%cv - Verbose\n", SW);
    mexPrintf("\t%ci<profile> - Input profile (defaults to sRGB)\n", SW);
    mexPrintf("\t%co<profile> - Output profile (defaults to sRGB)\n", SW);   
    mexPrintf("\t%cl<profile> - Transform by device-link profile\n", SW);      
    mexPrintf("\t%cm<profiles> - Apply multiprofile chain\n", SW);      

    mexPrintf("\t%ct<0,1,2,3> - Intent (0=Perceptual, 1=Colorimetric, 2=Saturation, 3=Absolute)\n", SW);    
        
    mexPrintf("\t%cb - Black point compensation\n", SW);
    mexPrintf("\t%cc<0,1,2,3> - Precalculates transform (0=Off, 1=Normal, 2=Hi-res, 3=LoRes) [defaults to 0]\n", SW);     
    
    mexPrintf("\t%cp<profile> - Soft proof profile\n", SW);
    mexPrintf("\t%cr<0,1,2,3> - Soft proof intent\n", SW);

    mexPrintf("\nYou can use following built-ins as profiles:\n\n");
	mexPrintf("\t'*sRGB'   -> IEC6 1996-2.1 sRGB\n");
    mexPrintf("\t'*Lab'    -> D50 based Lab\n");
    mexPrintf("\t'*LabD65' -> D65 based Lab\n");
    mexPrintf("\t'*XYZ'    -> XYZ (D50)\n");
    mexPrintf("\t'*Gray22' -> D50 gamma 2.2 grayscale.\n\n");

    mexPrintf("For suggestions, comments, bug reports etc. send mail to\n"
              "marti.maria@hp.com  or ignacio.ruiz-de-conejo@hp.com\n\n");
    
}



// Main entry point

void mexFunction(
                 int nlhs,              // Number of left hand side (output) arguments
                 mxArray *plhs[],       // Array of left hand side arguments
                 int nrhs,              // Number of right hand side (input) arguments
                 const mxArray *prhs[]  // Array of right hand side arguments
                 )
{
    
    char CommandLine[4096+1];
    char *pt, *argv[128];
    int argc = 1;
    
    
    if (nrhs != 2) {    

        PrintHelp();              
        return;
    }
    
    
    if(nlhs > 1) {        
        FatalError("Too many output arguments.");
    }
    
    
    // Setup error handler
    
    cmsSetErrorHandler(MatLabErrorHandler);
    
    // Defaults

    Verbose     = 0;
    cInProf     = NULL;
    cOutProf    = NULL;
    cProofing   = NULL;

	lMultiProfileChain = FALSE;
	nProfiles   = 0;

    Intent      = INTENT_PERCEPTUAL;
    ProofingIntent          = INTENT_ABSOLUTE_COLORIMETRIC;
    PrecalcMode = 0;
    BlackPointCompensation  = FALSE;
    lIsDeviceLink           = FALSE;

    // Check types. Fist parameter is array of values, second parameter is command line
    
    if (!mxIsNumeric(prhs[0]))
            FatalError("Type mismatch on argument 1 -- Must be numeric");

    if (!mxIsChar(prhs[1]))
            FatalError("Type mismatch on argument 2 -- Must be string");




    // Unpack string to command line buffer
    
    if (mxGetString(prhs[1], CommandLine, 4096))
        FatalError("Cannot unpack command string");
    
    // Separate to argv[] convention
    
    argv[0] = NULL;
    for (pt = strtok(CommandLine, " ");
                pt;
                pt = strtok(NULL, " ")) {
        
                        argv[argc++] = pt;
    }
    
    
    
    // Parse arguments
    HandleSwitches(argc, argv);
    

    nBytesDepth = SizeOfArrayType(prhs[0]);

    OpenTransforms(argc, argv);


    plhs[0] = AllocateOutputArray(prhs[0], OutputChannels);


    ApplyTransforms(prhs[0], plhs[0]);

    CloseTransforms();
    
    // Done!
}


