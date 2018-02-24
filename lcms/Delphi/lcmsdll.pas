//
//  Little cms DELPHI wrapper
//  Copyright (C) 1998-2005 Marti Maria
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

// ver 1.15

UNIT lcmsdll;

INTERFACE

USES Windows;

 CONST

     // Intents

    INTENT_PERCEPTUAL               =  0;
    INTENT_RELATIVE_COLORIMETRIC    =  1;
    INTENT_SATURATION               =  2;
    INTENT_ABSOLUTE_COLORIMETRIC    =  3;

    // Flags

    cmsFLAGS_MATRIXINPUT            = $0001;
    cmsFLAGS_MATRIXOUTPUT           = $0002;
    cmsFLAGS_MATRIXONLY             = (cmsFLAGS_MATRIXINPUT OR cmsFLAGS_MATRIXOUTPUT);

    cmsFLAGS_NOPRELINEARIZATION     = $0010; // Don't create prelinearization tables
                                             // on precalculated transforms (internal use)                   

    cmsFLAGS_NOTPRECALC             = $0100;
    cmsFLAGS_NULLTRANSFORM          = $0200;  // Don't transform anyway
    cmsFLAGS_HIGHRESPRECALC         = $0400;  // Use more memory to give better accurancy
    cmsFLAGS_LOWRESPRECALC          = $0800;  // Use less memory to minimize resouces

    
    cmsFLAGS_GAMUTCHECK             = $1000;  // Mark Out of Gamut as alarm color (on proofing transform)
    cmsFLAGS_SOFTPROOFING           = $4000;  // Softproof of proofing profile

    cmsFLAGS_WHITEBLACKCOMPENSATION = $2000; 
    cmsFLAGS_BLACKPOINTCOMPENSATION = $2000;

    cmsFLAGS_NODEFAULTRESOURCEDEF   = $10000;   // PostScript

    // Format descriptors


        TYPE_GRAY_8                 = $30009;
        TYPE_GRAY_8_REV             = $32009;
        TYPE_GRAY_16                = $3000A;
        TYPE_GRAY_16_REV            = $3200A;
        TYPE_GRAY_16_SE             = $3080A;
        TYPE_GRAYA_8                = $30089;
        TYPE_GRAYA_16               = $3008A;
        TYPE_GRAYA_16_SE            = $3088A;
        TYPE_GRAYA_8_PLANAR         = $31089;
        TYPE_GRAYA_16_PLANAR        = $3108A;
        TYPE_RGB_8                  = $40019;
        TYPE_RGB_8_PLANAR           = $41019;
        TYPE_BGR_8                  = $40419;
        TYPE_BGR_8_PLANAR           = $41419;
        TYPE_RGB_16                 = $4001A;
        TYPE_RGB_16_PLANAR          = $4101A;
        TYPE_RGB_16_SE              = $4081A;
        TYPE_BGR_16                 = $4041A;
        TYPE_BGR_16_PLANAR          = $4141A;
        TYPE_BGR_16_SE              = $40C1A;
        TYPE_RGBA_8                 = $40099;
        TYPE_RGBA_8_PLANAR          = $41099;
        TYPE_RGBA_16                = $4009A;
        TYPE_RGBA_16_PLANAR         = $4109A;
        TYPE_RGBA_16_SE             = $4089A;
        TYPE_ARGB_8                 = $44099;
        TYPE_ARGB_16                = $4409A;
        TYPE_ABGR_8                 = $40499;
        TYPE_ABGR_16                = $4049A;
        TYPE_ABGR_16_PLANAR         = $4149A;
        TYPE_ABGR_16_SE             = $40C9A;
        TYPE_BGRA_8                 = $44499;
        TYPE_BGRA_16                = $4449A;
        TYPE_BGRA_16_SE             = $4489A;
        TYPE_CMY_8                  = $50019;
        TYPE_CMY_8_PLANAR           = $51019;
        TYPE_CMY_16                 = $5001A;
        TYPE_CMY_16_PLANAR          = $5101A;
        TYPE_CMY_16_SE              = $5081A;
        TYPE_CMYK_8                 = $60021;
        TYPE_CMYK_8_REV             = $62021;
        TYPE_YUVK_8                 = $62021;
        TYPE_CMYK_8_PLANAR          = $61021;
        TYPE_CMYK_16                = $60022;
        TYPE_CMYK_16_REV            = $62022;
        TYPE_YUVK_16                = $62022;
        TYPE_CMYK_16_PLANAR         = $61022;
        TYPE_CMYK_16_SE             = $60822;
        TYPE_KYMC_8                 = $60421;
        TYPE_KYMC_16                = $60422;
        TYPE_KYMC_16_SE             = $60C22;
        TYPE_KCMY_8                 = $64021;
        TYPE_KCMY_8_REV             = $66021;
        TYPE_KCMY_16                = $64022;
        TYPE_KCMY_16_REV            = $66022;
        TYPE_KCMY_16_SE             = $64822;
        TYPE_CMYKcm_8               = $0031;
        TYPE_CMYKcm_8_PLANAR        = $1031;
        TYPE_CMYKcm_16              = $0032;
        TYPE_CMYKcm_16_PLANAR       = $1032;
        TYPE_CMYKcm_16_SE           = $0832;
        TYPE_CMYK7_8                = $0039;
        TYPE_CMYK7_16               = $003A;
        TYPE_CMYK7_16_SE            = $083A;
        TYPE_KYMC7_8                = $0439;
        TYPE_KYMC7_16               = $043A;
        TYPE_KYMC7_16_SE            = $0C3A;
        TYPE_CMYK8_8                = $0041;
        TYPE_CMYK8_16               = $0042;
        TYPE_CMYK8_16_SE            = $0842;
        TYPE_KYMC8_8                = $0441;
        TYPE_KYMC8_16               = $0442;
        TYPE_KYMC8_16_SE            = $0C42;
        TYPE_CMYK9_8                = $0049;
        TYPE_CMYK9_16               = $004A;
        TYPE_CMYK9_16_SE            = $084A;
        TYPE_KYMC9_8                = $0449;
        TYPE_KYMC9_16               = $044A;
        TYPE_KYMC9_16_SE            = $0C4A;
        TYPE_CMYK10_8               = $0051;
        TYPE_CMYK10_16              = $0052;
        TYPE_CMYK10_16_SE           = $0852;
        TYPE_KYMC10_8               = $0451;
        TYPE_KYMC10_16              = $0452;
        TYPE_KYMC10_16_SE           = $0C52;
        TYPE_CMYK11_8               = $0059;
        TYPE_CMYK11_16              = $005A;
        TYPE_CMYK11_16_SE           = $085A;
        TYPE_KYMC11_8               = $0459;
        TYPE_KYMC11_16              = $045A;
        TYPE_KYMC11_16_SE           = $0C5A;
        TYPE_CMYK12_8               = $0061;
        TYPE_CMYK12_16              = $0062;
        TYPE_CMYK12_16_SE           = $0862;
        TYPE_KYMC12_8               = $0461;
        TYPE_KYMC12_16              = $0462;
        TYPE_KYMC12_16_SE           = $0C62;

        TYPE_XYZ_16                 = $9001A;
        TYPE_Lab_8                  = $A0019;
        TYPE_LabA_8                 = $A0099;
        TYPE_Lab_16                 = $A001A;
        TYPE_Yxy_16                 = $E001A;
        TYPE_YCbCr_8                = $70019;
        TYPE_YCbCr_8_PLANAR         = $71019;
        TYPE_YCbCr_16               = $7001A;
        TYPE_YCbCr_16_PLANAR        = $7101A;
        TYPE_YCbCr_16_SE            = $7081A;
        TYPE_YUV_8                  = $80019;
        TYPE_YUV_8_PLANAR           = $81019;
        TYPE_YUV_16                 = $8001A;
        TYPE_YUV_16_PLANAR          = $8101A;
        TYPE_YUV_16_SE              = $8081A;
        TYPE_HLS_8                  = $D0019;
        TYPE_HLS_8_PLANAR           = $D1019;
        TYPE_HLS_16                 = $D001A;
        TYPE_HLS_16_PLANAR          = $D101A;
        TYPE_HLS_16_SE              = $D081A;
        TYPE_HSV_8                  = $C0019;
        TYPE_HSV_8_PLANAR           = $C1019;
        TYPE_HSV_16                 = $C001A;
        TYPE_HSV_16_PLANAR          = $C101A;
        TYPE_HSV_16_SE              = $C081A;
        TYPE_NAMED_COLOR_INDEX      = $000A;

        TYPE_XYZ_DBL                = $90018;
        TYPE_Lab_DBL                = $A0018;
        TYPE_GRAY_DBL               = $30008;
        TYPE_RGB_DBL                = $40018;
        TYPE_CMYK_DBL               = $60020;


    // Some utility functions to compute new descriptors


    FUNCTION COLORSPACE_SH(e: Integer):Integer;
    FUNCTION SWAPFIRST_SH(e: Integer):Integer;
    FUNCTION FLAVOR_SH(e: Integer):Integer;
    FUNCTION PLANAR_SH(e: Integer):Integer;
    FUNCTION ENDIAN16_SH(e: Integer):Integer;
    FUNCTION DOSWAP_SH(e: Integer):Integer;
    FUNCTION EXTRA_SH(e: Integer):Integer;
    FUNCTION CHANNELS_SH(c: Integer):Integer;
    FUNCTION BYTES_SH(b: Integer):Integer;


  TYPE

      DWord                  = Cardinal;

      icTagSignature         = DWord;
      icColorSpaceSignature  = DWord;
      icProfileClassSignature= DWord;

  CONST

    // icc color space signatures

    icSigXYZData              = $58595A20;
    icSigLabData              = $4C616220;
    icSigLuvData              = $4C757620;
    icSigYCbCrData            = $59436272;
    icSigYxyData              = $59787920;
    icSigRgbData              = $52474220;
    icSigGrayData             = $47524159;
    icSigHsvData              = $48535620;
    icSigHlsData              = $484C5320;
    icSigCmykData             = $434D594B;
    icSigCmyData              = $434D5920;

    // icc Profile class signatures

    icSigInputClass           = $73636E72;
    icSigDisplayClass         = $6D6E7472;
    icSigOutputClass          = $70727472;
    icSigLinkClass            = $6C696E6B;
    icSigAbstractClass        = $61627374;
    icSigColorSpaceClass      = $73706163;
    icSigNamedColorClass      = $6e6d636c;


    // Added by lcms

    lcmsSignature                = $6c636d73;
    icSigLuvKData                = $4C75764B;  {'LuvK'}
    icSigChromaticityTag         = $6368726d;  { As per Addendum 2 to Spec. ICC.1:1998-09 }
    icSigChromaticAdaptationTag  = $63686164;
    


  TYPE

    cmsHPROFILE            = Pointer;
    cmsHTRANSFORM          = Pointer;
    LCMSHANDLE             = Pointer;

      LCMSGAMMAPARAMS = PACKED RECORD
                        Crc32      : DWord;         
                        TheType    : Integer;
                        Params     : ARRAY [0..9] OF Double;                        
                        END;

      GAMMATABLE = PACKED RECORD
                        Birth      : LCMSGAMMAPARAMS;
                        nEntries   : DWord;
                        GammaTable : ARRAY [0..1] OF Word;
                     END;

      LPGAMMATABLE = ^GAMMATABLE;


      // Colorimetric spaces

      cmsCIEXYZ = PACKED RECORD
                        X, Y, Z : Double;
                    END;

      LPcmsCIEXYZ = ^cmsCIEXYZ;

      cmsCIEXYZTRIPLE = PACKED RECORD
                        Red, Green, Blue : cmsCIEXYZ
                        END;

      LPcmsCIEXYZTRIPLE = ^cmsCIEXYZTRIPLE;

      cmsCIExyY = PACKED RECORD
                        x, y, YY : Double
                        END;

      LPcmsCIExyY = ^cmsCIExyY;


      cmsCIExyYTRIPLE = PACKED RECORD
                        Red, Green, Blue : cmsCIExyY
                        END;

      LPcmsCIExyYTRIPLE = ^cmsCIExyYTRIPLE;


      cmsCIELab = PACKED RECORD
                  L, a, b: Double
                  END;

      LPcmsCIELab = ^cmsCIELab;

      cmsCIELCh = PACKED RECORD
                  L, C, h : Double
                  END;

      LPcmsCIELCh = ^cmsCIELCh;

      // CIECAM97s

      cmsJCh   = PACKED RECORD
                   J, C, h : Double
                 END;

      LPcmsJCh  = ^cmsJCh;

      LPLUT = Pointer;

  CONST

    AVG_SURROUND_4     = 0;
    AVG_SURROUND       = 1;
    DIM_SURROUND       = 2;
    DARK_SURROUND      = 3;
    CUTSHEET_SURROUND  = 4;

    D_CALCULATE           =  -1;
    D_CALCULATE_DISCOUNT  =  -2;


  TYPE

    cmsViewingConditions = PACKED RECORD

                WhitePoint: cmsCIEXYZ;
                Yb        : Double;
                La        : Double;
                surround  : Integer;
                D_value   : Double
              END;


    LPcmsViewingConditions = ^cmsViewingConditions;
   
    cmsErrorHandler =  FUNCTION (Severity: Integer; Msg:PChar): Integer; cdecl;

    LCMSARRAYOFPCHAR = ARRAY OF PChar;

// Input/Output

FUNCTION  cmsOpenProfileFromFile(ICCProfile: PChar; sAccess: PChar) : cmsHPROFILE; StdCall;
FUNCTION  cmsOpenProfileFromMem(MemPtr: Pointer; dwSize: DWord) : cmsHPROFILE; StdCall;
FUNCTION  cmsCloseProfile(hProfile : cmsHPROFILE) : Boolean; StdCall;

FUNCTION  cmsCreateRGBProfile(WhitePoint : LPcmsCIExyY;
                                           Primaries: LPcmsCIExyYTRIPLE;
                                           TransferFunction: ARRAY OF LPGAMMATABLE) : cmsHPROFILE; StdCall;


FUNCTION cmsCreateGrayProfile(WhitePoint: LPcmsCIExyY;
                                              TransferFunction: LPGAMMATABLE) :  cmsHPROFILE; StdCall;


FUNCTION cmsCreateLinearizationDeviceLink(ColorSpace: icColorSpaceSignature; 
                                              TransferFunction: ARRAY OF LPGAMMATABLE) : cmsHPROFILE; StdCall;

FUNCTION cmsCreateInkLimitingDeviceLink(ColorSpace: icColorSpaceSignature; 
                                              Limit: Double) : cmsHPROFILE; StdCall;


FUNCTION  cmsCreateNULLProfile : cmsHPROFILE; StdCall;
FUNCTION  cmsCreateLabProfile(WhitePoint: LPcmsCIExyY): cmsHPROFILE; StdCall;
FUNCTION  cmsCreateLab4Profile(WhitePoint: LPcmsCIExyY): cmsHPROFILE; StdCall;
FUNCTION  cmsCreateXYZProfile:cmsHPROFILE; StdCall;
FUNCTION  cmsCreate_sRGBProfile:cmsHPROFILE; StdCall;
FUNCTION  cmsCreateBCHSWabstractProfile(nLUTPoints: Integer;
                                        Bright, Contrast, Hue, Saturation: Double;
                                        TempSrc, TempDest: Integer): cmsHPROFILE; StdCall;


// Utils

PROCEDURE cmsXYZ2xyY(Dest: LPcmsCIExyY; Source: LPcmsCIEXYZ); StdCall;
PROCEDURE cmsxyY2XYZ(Dest: LPcmsCIEXYZ; Source: LPcmsCIExyY); StdCall;
PROCEDURE cmsXYZ2Lab(WhitePoint: LPcmsCIEXYZ; xyz: LPcmsCIEXYZ; Lab: LPcmsCIELab); StdCall;
PROCEDURE cmsLab2XYZ(WhitePoint: LPcmsCIEXYZ; Lab: LPcmsCIELab; xyz: LPcmsCIEXYZ); StdCall;
PROCEDURE cmsLab2LCh(LCh: LPcmsCIELCh; Lab: LPcmsCIELab); StdCall;
PROCEDURE cmsLCh2Lab(Lab: LPcmsCIELab; LCh: LPcmsCIELCh); StdCall;

// CIELab handling

FUNCTION cmsDeltaE(Lab1, Lab2: LPcmsCIELab): Double; StdCall;
FUNCTION cmsCIE94DeltaE(Lab1, Lab2: LPcmsCIELab): Double; StdCall;
FUNCTION cmsBFDdeltaE(Lab1, Lab2: LPcmsCIELab): Double; StdCall;
FUNCTION cmsCMCdeltaE(Lab1, Lab2: LPcmsCIELab): Double; StdCall;
FUNCTION cmsCIE2000DeltaE(Lab1, Lab2: LPcmsCIELab; Kl, Kc, Kh: Double): Double; StdCall;

PROCEDURE cmsClampLab(Lab: LPcmsCIELab; amax, amin,  bmax, bmin: Double); StdCall;

// White point

FUNCTION  cmsWhitePointFromTemp(TempK: Integer; WhitePoint: LPcmsCIExyY) : Boolean; StdCall;


// CIECAM97s

FUNCTION    cmsCIECAM97sInit(pVC : LPcmsViewingConditions ) : Pointer; StdCall;
PROCEDURE   cmsCIECAM97sDone(hModel : Pointer); StdCall;

PROCEDURE   cmsCIECAM97sForward(hModel: Pointer; pIn: LPcmsCIEXYZ; pOut: LPcmsJCh ); StdCall;
PROCEDURE   cmsCIECAM97sReverse(hModel: Pointer; pIn: LPcmsJCh;   pOut: LPcmsCIEXYZ ); StdCall;

// CIECAM02

FUNCTION    cmsCIECAM02Init(pVC : LPcmsViewingConditions ) : Pointer; StdCall;
PROCEDURE   cmsCIECAM02Done(hModel : Pointer); StdCall;

PROCEDURE   cmsCIECAM02Forward(hModel: Pointer; pIn: LPcmsCIEXYZ; pOut: LPcmsJCh ); StdCall;
PROCEDURE   cmsCIECAM02Reverse(hModel: Pointer; pIn: LPcmsJCh;   pOut: LPcmsCIEXYZ ); StdCall;


// Gamma curves

FUNCTION  cmsBuildGamma(nEntries : Integer; Gamma: Double) : LPGAMMATABLE; StdCall;
FUNCTION  cmsAllocGamma(nEntries : Integer): LPGAMMATABLE; StdCall;
PROCEDURE cmsFreeGamma(Gamma: LPGAMMATABLE); StdCall;
PROCEDURE cmsFreeGammaTriple(Gamma: ARRAY OF LPGAMMATABLE); StdCall;
FUNCTION  cmsReverseGamma(nResultSamples: Integer; InGamma : LPGAMMATABLE): LPGAMMATABLE; StdCall;
FUNCTION  cmsJoinGamma(InGamma, OutGamma: LPGAMMATABLE): LPGAMMATABLE; StdCall;
FUNCTION  cmsJoinGammaEx(InGamma, OutGamma: LPGAMMATABLE; nPoints: Integer): LPGAMMATABLE; StdCall;
FUNCTION  cmsSmoothGamma(Gamma: LPGAMMATABLE; SmoothingLambda: Double): Boolean; StdCall;
FUNCTION  cmsDupGamma(Src: LPGAMMATABLE): LPGAMMATABLE; StdCall;
FUNCTION  cmsEstimateGamma(Src: LPGAMMATABLE): Double; StdCall;
FUNCTION  cmsEstimateGammaEx(Src: LPGAMMATABLE; Thereshold: Double): Double; StdCall;
FUNCTION  cmsReadICCGamma(hProfile: cmsHPROFILE; Sig: icTagSignature): LPGAMMATABLE; StdCall;
FUNCTION  cmsReadICCGammaReversed(hProfile: cmsHPROFILE; Sig: icTagSignature): LPGAMMATABLE; StdCall;

CONST

    lcmsParametricCurveExp          = 0;
    lcmsParametricCurveCIE_122_1966 = 1;
    lcmsParametricCurveIEC_61966_3  = 2;
    lcmsParametricCurveIEC_61966_2_1= 3;

FUNCTION cmsBuildParametricGamma(nEntries: Integer; TheType: Integer; Params: array of Double) : LPGAMMATABLE; StdCall;


// Access to Profile data.

PROCEDURE cmsSetLanguage(LanguageCode: Integer; CountryCode: Integer); StdCall;

FUNCTION cmsTakeMediaWhitePoint(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; StdCall;
FUNCTION cmsTakeMediaBlackPoint(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; StdCall;
FUNCTION cmsTakeIluminant(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; StdCall;
FUNCTION cmsTakeColorants(Dest: LPcmsCIEXYZTRIPLE; hProfile: cmsHPROFILE): Boolean; StdCall;
FUNCTION cmsTakeHeaderFlags(hProfile: cmsHPROFILE): DWord; StdCall;

FUNCTION cmsTakeProductName(hProfile: cmsHPROFILE): PChar; StdCall;
FUNCTION cmsTakeProductDesc(hProfile: cmsHPROFILE): PChar; StdCall;

FUNCTION cmsTakeManufacturer(hProfile: cmsHPROFILE): PChar; StdCall;
FUNCTION cmsTakeModel(hProfile: cmsHPROFILE): PChar; StdCall;
FUNCTION cmsTakeCopyright(hProfile: cmsHPROFILE): PChar; StdCall;
FUNCTION cmsTakeProfileID(hProfile: cmsHPROFILE): PByte; StdCall;



FUNCTION cmsIsTag(hProfile: cmsHPROFILE; sig: icTagSignature): Boolean; StdCall;
FUNCTION cmsTakeRenderingIntent(hProfile: cmsHPROFILE): Integer; StdCall;
FUNCTION cmsIsIntentSupported(hProfile: cmsHPROFILE; Intent, UsedDirection : Integer): Integer; StdCall;
FUNCTION cmsTakeCharTargetData(hProfile: cmsHPROFILE; var Data : PChar; var len: Cardinal): Boolean; StdCall;

FUNCTION _cmsICCcolorSpace(OurNotation: Integer) : icColorSpaceSignature; StdCall;
FUNCTION _cmsLCMScolorSpace(ProfileSpace: icColorSpaceSignature): Integer; StdCall;
FUNCTION _cmsChannelsOf(ColorSpace: icColorSpaceSignature): Integer; StdCall;


FUNCTION cmsGetPCS(hProfile: cmsHPROFILE): icColorSpaceSignature; StdCall;
FUNCTION cmsGetColorSpace(hProfile: cmsHPROFILE): icColorSpaceSignature; StdCall;
FUNCTION cmsGetDeviceClass( hProfile: cmsHPROFILE): icProfileClassSignature; StdCall;
FUNCTION cmsGetProfileICCversion( hProfile: cmsHPROFILE): DWord; StdCall;

// Profile creation

PROCEDURE cmsSetDeviceClass(hProfile: cmsHPROFILE; sig: icProfileClassSignature ); StdCall;
PROCEDURE cmsSetColorSpace(hProfile: cmsHPROFILE; sig: icProfileClassSignature ); StdCall;
PROCEDURE cmsSetPCS(hProfile: cmsHPROFILE; pcs: icColorSpaceSignature); StdCall;
PROCEDURE cmsSetRenderingIntent(hProfile: cmsHPROFILE; Intent: Integer); StdCall;
PROCEDURE cmsSetHeaderFlags(hProfile: cmsHPROFILE; dwFlags: DWord); StdCall;
PROCEDURE cmsSetProfileID(hProfile: cmsHPROFILE; ProfileID: PByte); StdCall;



FUNCTION  cmsAddTag(hProfile: cmsHPROFILE; Sig: icTagSignature; Data: Pointer): Boolean; StdCall;
FUNCTION  _cmsSaveProfile(hProfile: cmsHPROFILE; FileName: PChar): Boolean; StdCall;
FUNCTION  _cmsSaveProfileToMem(hProfile: cmsHPROFILE; MemPtr: Pointer; 
                                                    var BytesNeeded: DWord): Boolean; StdCall;



CONST
        LCMS_USED_AS_INPUT   =   0;
        LCMS_USED_AS_OUTPUT  =   1;
        LCMS_USED_AS_PROOF   =   2;



// Transforms

FUNCTION cmsCreateTransform(Input: cmsHPROFILE;
                            InputFormat: DWORD;
                            Output: cmsHPROFILE;
                            OutputFormat: DWORD;
                            Intent: Integer;
                            dwFlags: DWord): cmsHTRANSFORM; StdCall;

FUNCTION cmsCreateProofingTransform(Input: cmsHPROFILE;
                            InputFormat: DWORD;
                            Output: cmsHPROFILE;
                            OutputFormat: DWORD;
                            Proofing: cmsHPROFILE;
                            Intent: Integer;
                            ProofingIntent: Integer;
                            dwFlags: DWord): cmsHTRANSFORM; StdCall;


FUNCTION cmsCreateMultiprofileTransform(hProfiles : ARRAY OF cmsHPROFILE;
                                        nProfiles : Integer;
                                        InputFormat: DWord;
                                        OutputFormat: DWord;
                                        Intent: Integer;
                                        dwFlags: DWord): cmsHTRANSFORM; StdCall;


PROCEDURE cmsDeleteTransform( hTransform: cmsHTRANSFORM); StdCall;

PROCEDURE cmsDoTransform( Transform: cmsHTRANSFORM;
                          InputBuffer: Pointer;
                          OutputBuffer: Pointer;
                          Size: LongInt); StdCall;

PROCEDURE cmsChangeBuffersFormat(hTransform: cmsHTRANSFORM;
                                dwInputFormat, dwOutputFormat: DWord); StdCall;

// Devicelink generation

FUNCTION cmsTransform2DeviceLink(hTransform: cmsHTRANSFORM; 
                                dwFlags: DWord): cmsHPROFILE; StdCall;

PROCEDURE _cmsSetLUTdepth(hProfile: cmsHPROFILE; depth: Integer); StdCall;

// Named color support

FUNCTION cmsNamedColorCount(xform: cmsHTRANSFORM): Integer; StdCall;
FUNCTION cmsNamedColorInfo(xform: cmsHTRANSFORM; nColor: Integer;
                             Name, Prefix, Suffix: PChar) : Boolean; StdCall;

FUNCTION cmsNamedColorIndex(xform: cmsHTRANSFORM; Name: PChar): Integer; StdCall;


// PostScript ColorRenderingDictionary and ColorSpaceArray

FUNCTION cmsGetPostScriptCSA(hProfile: cmsHPROFILE ;
                                 Intent: Integer;
                                 Buffer: Pointer;
                                 dwBufferLen: DWord): DWord; StdCall;

FUNCTION cmsGetPostScriptCRD(hProfile: cmsHPROFILE ;
                                 Intent: Integer;
                                 Buffer: Pointer;
                                 dwBufferLen: DWord): DWord; StdCall;

FUNCTION cmsGetPostScriptCRDEx(hProfile: cmsHPROFILE ;
                                 Intent: Integer;
                                 dwFlags: DWord;
                                 Buffer: Pointer;
                                 dwBufferLen: DWord): DWord; StdCall;




// Gamut check

PROCEDURE cmsSetAlarmCodes(r, g, b: Integer); StdCall;
PROCEDURE cmsGetAlarmCodes(VAR r, g, b: Integer); StdCall;

// Error handling
CONST

        LCMS_ERROR_ABORT   =  0;
        LCMS_ERROR_SHOW    =  1;
        LCMS_ERROR_IGNORE  =  2;

PROCEDURE cmsErrorAction(nAction: Integer); StdCall;

PROCEDURE cmsSetErrorHandler(ErrorHandler: cmsErrorHandler); StdCall;


// CGATS.13 parser

FUNCTION  cmsIT8Alloc: LCMSHANDLE; StdCall;
PROCEDURE cmsIT8Free(hIT8: LCMSHANDLE); StdCall;

// Tables

FUNCTION  cmsIT8TableCount(hIT8: LCMSHANDLE): Integer; StdCall;
FUNCTION  cmsIT8SetTable(hIT8: LCMSHANDLE; nTable: Integer): Integer; StdCall;

// Persistence
FUNCTION  cmsIT8LoadFromFile(cFileName: PChar): LCMSHANDLE; StdCall;
FUNCTION  cmsIT8LoadFromMem(Ptr: Pointer; size :DWord): LCMSHANDLE; StdCall;

FUNCTION cmsIT8SaveToFile(hIT8: LCMSHANDLE; cFileName: PChar): Boolean; StdCall;

// Properties

FUNCTION cmsIT8GetSheetType(hIT8: LCMSHANDLE): PChar; StdCall;
FUNCTION cmsIT8SetSheetType(hIT8: LCMSHANDLE; TheType: PChar): Boolean; StdCall;

FUNCTION cmsIT8SetComment(hIT8: LCMSHANDLE; cComment: PChar): Boolean; StdCall;

FUNCTION cmsIT8SetPropertyStr(hIT8: LCMSHANDLE; cProp, Str: PChar): Boolean; StdCall;
FUNCTION cmsIT8SetPropertyDbl(hIT8: LCMSHANDLE; cProp: PChar; Val: Double): Boolean; StdCall;
FUNCTION cmsIT8SetPropertyHex(hIT8: LCMSHANDLE; cProp: PChar; Val: Integer): Boolean; StdCall;
FUNCTION cmsIT8SetPropertyUncooked(hIT8: LCMSHANDLE; Key, Buffer: PChar): Boolean; StdCall;


FUNCTION cmsIT8GetProperty(hIT8: LCMSHANDLE; cProp: PChar): PChar; StdCall;
FUNCTION cmsIT8GetPropertyDbl(hIT8: LCMSHANDLE; cProp: PChar): Double; StdCall;
FUNCTION cmsIT8EnumProperties(hIT8: LCMSHANDLE; var PropertyNames: LCMSARRAYOFPCHAR): Integer; StdCall;

// Datasets

FUNCTION cmsIT8GetDataRowCol(hIT8: LCMSHANDLE; row, col: Integer): PChar; StdCall;                                                
FUNCTION cmsIT8GetDataRowColDbl(hIT8: LCMSHANDLE; row, col: Integer): Double; StdCall;

FUNCTION cmsIT8SetDataRowCol(hIT8: LCMSHANDLE; row, col: Integer; Val: PChar): Boolean; StdCall;
FUNCTION cmsIT8SetDataRowColDbl(hIT8: LCMSHANDLE; row, col: Integer; Val: Double): Boolean; StdCall;

FUNCTION cmsIT8GetData(hIT8: LCMSHANDLE; cPatch, cSample: PChar): PChar; StdCall;                                                

FUNCTION cmsIT8GetDataDbl(hIT8: LCMSHANDLE;cPatch, cSample: PChar): Double; StdCall;

FUNCTION cmsIT8SetData(hIT8: LCMSHANDLE; cPatch, cSample, Val: PChar): Boolean; StdCall;

FUNCTION cmsIT8SetDataDbl(hIT8: LCMSHANDLE; cPatch, cSample: PChar; Val: Double): Boolean; StdCall;                                                

FUNCTION cmsIT8SetDataFormat(hIT8: LCMSHANDLE; n: Integer; Sample: PChar): Boolean; StdCall;
FUNCTION cmsIT8EnumDataFormat(hIT8: LCMSHANDLE; var SampleNames: LCMSARRAYOFPCHAR): Integer; StdCall;
FUNCTION cmsIT8GetPatchName(hIT8: LCMSHANDLE; nPatch: Integer; Buffer: PChar): PChar; StdCall;

// The LABEL extension

FUNCTION cmsIT8SetTableByLabel(hIT8: LCMSHANDLE; cSet, cField, ExpectedType: PChar): Integer; StdCall;








// Provided for compatibility with anterior revisions

PROCEDURE cmsLabEncoded2Float(Lab: LPcmsCIELab; wLab: Pointer); StdCall;
PROCEDURE cmsFloat2LabEncoded(wLab: Pointer; Lab: LPcmsCIELab); StdCall;
PROCEDURE cmsXYZEncoded2Float(fxyz : LPcmsCIEXYZ; XYZ: Pointer); StdCall;
PROCEDURE cmsFloat2XYZEncoded(XYZ: Pointer; fXYZ: LPcmsCIEXYZ); StdCall;

FUNCTION _cmsAddTextTag(hProfile: cmsHPROFILE; sig: icTagSignature; Text: PChar): Boolean; StdCall;
FUNCTION _cmsAddXYZTag(hProfile: cmsHPROFILE;  sig: icTagSignature;  XYZ: LPcmsCIEXYZ): Boolean; StdCall;
FUNCTION _cmsAddLUTTag(hProfile: cmsHPROFILE;  sig: icTagSignature; lut: PByte): Boolean; StdCall;


IMPLEMENTATION


// Helping functions to build format descriptors (C uses them as macros)

FUNCTION COLORSPACE_SH(e: Integer):Integer; BEGIN COLORSPACE_SH := ((e) shl 16) END;
FUNCTION SWAPFIRST_SH(e: Integer):Integer;  BEGIN SWAPFIRST_SH   := ((e) shl 13) END;
FUNCTION FLAVOR_SH(e: Integer):Integer;     BEGIN FLAVOR_SH   := ((e) shl 13) END;
FUNCTION PLANAR_SH(e: Integer):Integer;     BEGIN PLANAR_SH   := ((e) shl 12) END;
FUNCTION ENDIAN16_SH(e: Integer):Integer;   BEGIN ENDIAN16_SH := ((e) shl 11) END;
FUNCTION DOSWAP_SH(e: Integer):Integer;     BEGIN DOSWAP_SH   := ((e) shl 10) END;
FUNCTION EXTRA_SH(e: Integer):Integer;      BEGIN EXTRA_SH    := ((e) shl 7) END;
FUNCTION CHANNELS_SH(c: Integer):Integer;   BEGIN CHANNELS_SH := ((c) shl 3) END;
FUNCTION BYTES_SH(b: Integer):Integer;      BEGIN BYTES_SH    := (b) END;

CONST
   PT_ANY   =   0;   // Don't check colorspace
                     // 1 & 2 are reserved
   PT_GRAY  =   3;
   PT_RGB   =   4;
   PT_CMY   =   5;
   PT_CMYK  =   6;
   PT_YCbCr =   7;
   PT_YUV   =   8;     // Lu'v'
   PT_XYZ   =   9;
   PT_Lab   =   10;
   PT_YUVK  =   11;    // Lu'v'K
   PT_HSV   =   12;
   PT_HLS   =   13;
   PT_Yxy   =   14;


FUNCTION  cmsOpenProfileFromFile(ICCProfile : PChar; sAccess: PChar) : cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsOpenProfileFromMem(MemPtr: Pointer; dwSize: DWord) : cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsWhitePointFromTemp(TempK: Integer; WhitePoint: LPcmsCIExyY) : Boolean; EXTERNAL 'lcms.dll';

FUNCTION  cmsBuildGamma(nEntries : Integer;  Gamma: Double) : LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsAllocGamma(nEntries : Integer): LPGAMMATABLE; EXTERNAL 'lcms.dll';
PROCEDURE cmsFreeGamma(Gamma: LPGAMMATABLE); EXTERNAL 'lcms.dll';
PROCEDURE cmsFreeGammaTriple(Gamma: ARRAY OF LPGAMMATABLE); EXTERNAL 'lcms.dll';

FUNCTION  cmsReverseGamma(nResultSamples : Integer; InGamma : LPGAMMATABLE): LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsBuildParametricGamma(nEntries: Integer; TheType: Integer; Params: array of Double) : LPGAMMATABLE; EXTERNAL 'lcms.dll';

FUNCTION  cmsJoinGamma(InGamma, OutGamma : LPGAMMATABLE): LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsJoinGammaEx(InGamma, OutGamma: LPGAMMATABLE; nPoints: Integer): LPGAMMATABLE; EXTERNAL 'lcms.dll';

FUNCTION  cmsSmoothGamma(Gamma: LPGAMMATABLE; SmoothingLambda: Double): Boolean; EXTERNAL 'lcms.dll';

FUNCTION  cmsDupGamma(Src: LPGAMMATABLE): LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsEstimateGamma(Src: LPGAMMATABLE): Double; EXTERNAL 'lcms.dll';
FUNCTION  cmsEstimateGammaEx(Src: LPGAMMATABLE; Thereshold: Double): Double; EXTERNAL 'lcms.dll';
FUNCTION  cmsReadICCGamma(hProfile: cmsHPROFILE; Sig: icTagSignature): LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsReadICCGammaReversed(hProfile: cmsHPROFILE; Sig: icTagSignature): LPGAMMATABLE; EXTERNAL 'lcms.dll';


FUNCTION  cmsCreateRGBProfile( WhitePoint : LPcmsCIExyY;
                                         Primaries: LPcmsCIExyYTRIPLE;
                                         TransferFunction: ARRAY OF LPGAMMATABLE) : cmsHPROFILE; EXTERNAL 'lcms.dll';

FUNCTION cmsCreateGrayProfile(WhitePoint: LPcmsCIExyY;
                                              TransferFunction: LPGAMMATABLE) : cmsHPROFILE; EXTERNAL 'lcms.dll';

FUNCTION cmsCreateLinearizationDeviceLink(ColorSpace: icColorSpaceSignature; 
                                              TransferFunction: ARRAY OF LPGAMMATABLE) : cmsHPROFILE; EXTERNAL 'lcms.dll';

FUNCTION cmsCreateInkLimitingDeviceLink(ColorSpace: icColorSpaceSignature; 
                                              Limit: Double) : cmsHPROFILE; EXTERNAL 'lcms.dll';


FUNCTION  cmsCreateNULLProfile : cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsCreateLabProfile(WhitePoint: LPcmsCIExyY): cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsCreateLab4Profile(WhitePoint: LPcmsCIExyY): cmsHPROFILE; EXTERNAL 'lcms.dll';

FUNCTION  cmsCreateXYZProfile: cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsCreate_sRGBProfile: cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsCreateBCHSWabstractProfile(nLUTPoints: Integer;
                                        Bright, Contrast, Hue, Saturation: Double;
                                        TempSrc, TempDest: Integer): cmsHPROFILE; EXTERNAL 'lcms.dll';


FUNCTION  cmsCloseProfile( hProfile : cmsHPROFILE) : Boolean; EXTERNAL 'lcms.dll';


PROCEDURE cmsSetLanguage(LanguageCode: Integer; CountryCode: Integer); EXTERNAL 'lcms.dll';

FUNCTION  cmsTakeMediaWhitePoint(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  cmsTakeMediaBlackPoint(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; EXTERNAL 'lcms.dll';

FUNCTION  cmsTakeIluminant(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  cmsTakeColorants(Dest: LPcmsCIEXYZTRIPLE; hProfile: cmsHPROFILE): Boolean; EXTERNAL 'lcms.dll';
FUNCTION cmsTakeHeaderFlags(hProfile: cmsHPROFILE): DWord; EXTERNAL 'lcms.dll';

FUNCTION cmsTakeProductName(hProfile: cmsHPROFILE): PChar; EXTERNAL 'lcms.dll';
FUNCTION cmsTakeProductDesc(hProfile: cmsHPROFILE): PChar; EXTERNAL 'lcms.dll';
FUNCTION cmsTakeManufacturer(hProfile: cmsHPROFILE): PChar; EXTERNAL 'lcms.dll';
FUNCTION cmsTakeModel(hProfile: cmsHPROFILE): PChar; EXTERNAL 'lcms.dll';
FUNCTION cmsTakeCopyright(hProfile: cmsHPROFILE): PChar; EXTERNAL 'lcms.dll';
FUNCTION cmsTakeProfileID(hProfile: cmsHPROFILE): PByte; EXTERNAL 'lcms.dll';


FUNCTION cmsIsTag(hProfile: cmsHPROFILE; sig: icTagSignature): Boolean; EXTERNAL 'lcms.dll';
FUNCTION cmsTakeRenderingIntent( hProfile: cmsHPROFILE): Integer; EXTERNAL 'lcms.dll';
FUNCTION cmsGetPCS(hProfile: cmsHPROFILE): icColorSpaceSignature; EXTERNAL 'lcms.dll';
FUNCTION cmsGetColorSpace(hProfile: cmsHPROFILE): icColorSpaceSignature; EXTERNAL 'lcms.dll';
FUNCTION cmsGetDeviceClass( hProfile: cmsHPROFILE): icProfileClassSignature; EXTERNAL 'lcms.dll';
FUNCTION cmsGetProfileICCversion( hProfile: cmsHPROFILE): DWord; EXTERNAL 'lcms.dll';
FUNCTION cmsTakeCharTargetData(hProfile: cmsHPROFILE; var Data: PChar; var len: Cardinal): Boolean; EXTERNAL 'lcms.dll';

FUNCTION _cmsICCcolorSpace(OurNotation: Integer) : icColorSpaceSignature; EXTERNAL 'lcms.dll';
FUNCTION _cmsLCMScolorSpace(ProfileSpace: icColorSpaceSignature): Integer; EXTERNAL 'lcms.dll';
FUNCTION _cmsChannelsOf(ColorSpace: icColorSpaceSignature): Integer; EXTERNAL 'lcms.dll';




PROCEDURE cmsSetDeviceClass(hProfile: cmsHPROFILE; sig: icProfileClassSignature ); EXTERNAL 'lcms.dll';
PROCEDURE cmsSetColorSpace(hProfile: cmsHPROFILE; sig: icProfileClassSignature ); EXTERNAL 'lcms.dll';
PROCEDURE cmsSetPCS(hProfile: cmsHPROFILE; pcs: icColorSpaceSignature); EXTERNAL 'lcms.dll';
PROCEDURE cmsSetRenderingIntent(hProfile: cmsHPROFILE; Intent: Integer); EXTERNAL 'lcms.dll';
PROCEDURE cmsSetHeaderFlags(hProfile: cmsHPROFILE; dwFlags: DWord); EXTERNAL 'lcms.dll';
PROCEDURE cmsSetProfileID(hProfile: cmsHPROFILE; ProfileID: PByte); EXTERNAL 'lcms.dll';

FUNCTION  cmsAddTag(hProfile: cmsHPROFILE; Sig: icTagSignature; Data: Pointer): Boolean; EXTERNAL 'lcms.dll';


FUNCTION  _cmsSaveProfile(hProfile: cmsHPROFILE; FileName: PChar): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  _cmsSaveProfileToMem(hProfile: cmsHPROFILE; MemPtr: Pointer; 
                                                    var BytesNeeded: DWord): Boolean; EXTERNAL 'lcms.dll';


FUNCTION  cmsIsIntentSupported(hProfile: cmsHPROFILE; Intent, UsedDirection : Integer): Integer; EXTERNAL 'lcms.dll';

FUNCTION  cmsCreateTransform(Input: cmsHPROFILE;
                            InputFormat: DWORD;
                            Output: cmsHPROFILE;
                            OutputFormat: DWORD;
                            Intent: Integer;
                            dwFlags: DWord): cmsHTRANSFORM; EXTERNAL 'lcms.dll';

FUNCTION  cmsCreateProofingTransform(Input: cmsHPROFILE;
                            InputFormat: DWORD;
                            Output: cmsHPROFILE;
                            OutputFormat: DWORD;
                            Proofing: cmsHPROFILE;
                            Intent: Integer;
                            ProofingIntent: Integer;
                            dwFlags: DWord): cmsHTRANSFORM; EXTERNAL 'lcms.dll';

FUNCTION cmsCreateMultiprofileTransform(hProfiles : ARRAY OF cmsHPROFILE;
                                        nProfiles : Integer;
                                        InputFormat: DWord;
                                        OutputFormat: DWord;
                                        Intent: Integer;
                                        dwFlags: DWord): cmsHTRANSFORM; EXTERNAL 'lcms.dll';


PROCEDURE cmsDeleteTransform( hTransform: cmsHTRANSFORM); EXTERNAL 'lcms.dll';

PROCEDURE cmsDoTransform( Transform: cmsHTRANSFORM;
                          InputBuffer: Pointer;
                          OutputBuffer: Pointer;
                          Size: LongInt); EXTERNAL 'lcms.dll';

PROCEDURE cmsChangeBuffersFormat(hTransform: cmsHTRANSFORM;
                                dwInputFormat, dwOutputFormat: DWord);  EXTERNAL 'lcms.dll';


FUNCTION cmsTransform2DeviceLink(hTransform: cmsHTRANSFORM; dwFlags: DWord): cmsHPROFILE; EXTERNAL 'lcms.dll';

PROCEDURE _cmsSetLUTdepth(hProfile: cmsHPROFILE; depth: Integer);  EXTERNAL 'lcms.dll';

FUNCTION cmsNamedColorCount(xform: cmsHTRANSFORM): Integer; EXTERNAL 'lcms.dll';
FUNCTION cmsNamedColorInfo(xform: cmsHTRANSFORM; nColor: Integer; 
                             Name, Prefix, Suffix: PChar) : Boolean; EXTERNAL 'lcms.dll';

FUNCTION cmsNamedColorIndex(xform: cmsHTRANSFORM; Name: PChar): Integer; EXTERNAL 'lcms.dll';


FUNCTION cmsGetPostScriptCSA(hProfile: cmsHPROFILE ;
                                 Intent: Integer;
                                 Buffer: Pointer;
                                 dwBufferLen: DWord): DWord; EXTERNAL 'lcms.dll';

FUNCTION cmsGetPostScriptCRD(hProfile: cmsHPROFILE ;
                                 Intent: Integer;
                                 Buffer: Pointer;
                                 dwBufferLen: DWord): DWord; EXTERNAL 'lcms.dll';

FUNCTION cmsGetPostScriptCRDEx(hProfile: cmsHPROFILE ;
                                 Intent: Integer;
                                 dwFlags: DWord;
                                 Buffer: Pointer;
                                 dwBufferLen: DWord): DWord; EXTERNAL 'lcms.dll';

FUNCTION    cmsCIECAM97sInit(pVC : LPcmsViewingConditions ) : Pointer; EXTERNAL 'lcms.dll';
PROCEDURE   cmsCIECAM97sDone(hModel : Pointer); EXTERNAL 'lcms.dll';

PROCEDURE   cmsCIECAM97sForward(hModel: Pointer; pIn: LPcmsCIEXYZ; pOut: LPcmsJCh ); EXTERNAL 'lcms.dll';
PROCEDURE   cmsCIECAM97sReverse(hModel: Pointer; pIn: LPcmsJCh;   pOut: LPcmsCIEXYZ ); EXTERNAL 'lcms.dll';

// CIECAM02

FUNCTION    cmsCIECAM02Init(pVC : LPcmsViewingConditions ) : Pointer;  EXTERNAL 'lcms.dll';
PROCEDURE   cmsCIECAM02Done(hModel : Pointer);  EXTERNAL 'lcms.dll';

PROCEDURE   cmsCIECAM02Forward(hModel: Pointer; pIn: LPcmsCIEXYZ; pOut: LPcmsJCh );  EXTERNAL 'lcms.dll';
PROCEDURE   cmsCIECAM02Reverse(hModel: Pointer; pIn: LPcmsJCh;   pOut: LPcmsCIEXYZ );  EXTERNAL 'lcms.dll';

// Utils

PROCEDURE cmsXYZ2xyY(Dest: LPcmsCIExyY; Source: LPcmsCIEXYZ); EXTERNAL 'lcms.dll';
PROCEDURE cmsxyY2XYZ(Dest: LPcmsCIEXYZ; Source: LPcmsCIExyY); EXTERNAL 'lcms.dll';

PROCEDURE cmsXYZ2Lab(WhitePoint: LPcmsCIEXYZ; xyz: LPcmsCIEXYZ; Lab: LPcmsCIELab); EXTERNAL 'lcms.dll';
PROCEDURE cmsLab2XYZ(WhitePoint: LPcmsCIEXYZ; Lab: LPcmsCIELab; xyz: LPcmsCIEXYZ); EXTERNAL 'lcms.dll';
PROCEDURE cmsLab2LCh(LCh: LPcmsCIELCh; Lab: LPcmsCIELab); EXTERNAL 'lcms.dll';
PROCEDURE cmsLCh2Lab(Lab: LPcmsCIELab; LCh: LPcmsCIELCh); EXTERNAL 'lcms.dll';


// CIELab handling

FUNCTION cmsDeltaE(Lab1, Lab2: LPcmsCIELab): Double; EXTERNAL 'lcms.dll';
FUNCTION cmsCIE94DeltaE(Lab1, Lab2: LPcmsCIELab): Double; EXTERNAL 'lcms.dll';
FUNCTION cmsBFDdeltaE(Lab1, Lab2: LPcmsCIELab): Double; EXTERNAL 'lcms.dll';
FUNCTION cmsCMCdeltaE(Lab1, Lab2: LPcmsCIELab): Double; EXTERNAL 'lcms.dll';
FUNCTION cmsCIE2000DeltaE(Lab1, Lab2: LPcmsCIELab; Kl, Kc, Kh: Double): Double; StdCall; EXTERNAL 'lcms.dll';

PROCEDURE cmsClampLab(Lab: LPcmsCIELab; amax, amin,  bmax, bmin: Double); EXTERNAL 'lcms.dll';

PROCEDURE cmsSetAlarmCodes(r, g, b: Integer); EXTERNAL 'lcms.dll';
PROCEDURE cmsGetAlarmCodes(VAR r, g, b: Integer); EXTERNAL 'lcms.dll';

PROCEDURE cmsErrorAction(nAction: Integer); EXTERNAL 'lcms.dll';

PROCEDURE cmsSetErrorHandler(ErrorHandler: cmsErrorHandler); EXTERNAL 'lcms.dll';



FUNCTION  cmsIT8Alloc: LCMSHANDLE; EXTERNAL 'lcms.dll';
PROCEDURE cmsIT8Free(hIT8: LCMSHANDLE); EXTERNAL 'lcms.dll';

// Tables

FUNCTION  cmsIT8TableCount(hIT8: LCMSHANDLE): Integer; EXTERNAL 'lcms.dll';
FUNCTION  cmsIT8SetTable(hIT8: LCMSHANDLE; nTable: Integer): Integer; EXTERNAL 'lcms.dll';

// Persistence
FUNCTION  cmsIT8LoadFromFile(cFileName: PChar): LCMSHANDLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsIT8LoadFromMem(Ptr: Pointer; size :DWord): LCMSHANDLE; EXTERNAL 'lcms.dll';

FUNCTION cmsIT8SaveToFile(hIT8: LCMSHANDLE; cFileName: PChar): Boolean; EXTERNAL 'lcms.dll';

// Properties

FUNCTION cmsIT8GetSheetType(hIT8: LCMSHANDLE): PChar; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8SetSheetType(hIT8: LCMSHANDLE; TheType: PChar): Boolean; EXTERNAL 'lcms.dll';

FUNCTION cmsIT8SetComment(hIT8: LCMSHANDLE; cComment: PChar): Boolean; EXTERNAL 'lcms.dll';

FUNCTION cmsIT8SetPropertyStr(hIT8: LCMSHANDLE; cProp, Str: PChar): Boolean; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8SetPropertyDbl(hIT8: LCMSHANDLE; cProp: PChar; Val: Double): Boolean; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8SetPropertyHex(hIT8: LCMSHANDLE; cProp: PChar; Val: Integer): Boolean; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8SetPropertyUncooked(hIT8: LCMSHANDLE; Key, Buffer: PChar): Boolean; EXTERNAL 'lcms.dll';


FUNCTION cmsIT8GetProperty(hIT8: LCMSHANDLE; cProp: PChar): PChar; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8GetPropertyDbl(hIT8: LCMSHANDLE; cProp: PChar): Double; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8EnumProperties(hIT8: LCMSHANDLE; var PropertyNames: LCMSARRAYOFPCHAR): Integer; EXTERNAL 'lcms.dll';

// Datasets

FUNCTION cmsIT8GetDataRowCol(hIT8: LCMSHANDLE; row, col: Integer): PChar; EXTERNAL 'lcms.dll';                                                
FUNCTION cmsIT8GetDataRowColDbl(hIT8: LCMSHANDLE; row, col: Integer): Double; EXTERNAL 'lcms.dll';

FUNCTION cmsIT8SetDataRowCol(hIT8: LCMSHANDLE; row, col: Integer; Val: PChar): Boolean; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8SetDataRowColDbl(hIT8: LCMSHANDLE; row, col: Integer; Val: Double): Boolean; EXTERNAL 'lcms.dll';

FUNCTION cmsIT8GetData(hIT8: LCMSHANDLE; cPatch, cSample: PChar): PChar; EXTERNAL 'lcms.dll';                                                

FUNCTION cmsIT8GetDataDbl(hIT8: LCMSHANDLE;cPatch, cSample: PChar): Double; EXTERNAL 'lcms.dll';

FUNCTION cmsIT8SetData(hIT8: LCMSHANDLE; cPatch, cSample, Val: PChar): Boolean; EXTERNAL 'lcms.dll';

FUNCTION cmsIT8SetDataDbl(hIT8: LCMSHANDLE; cPatch, cSample: PChar; Val: Double): Boolean; EXTERNAL 'lcms.dll';                                                

FUNCTION cmsIT8SetDataFormat(hIT8: LCMSHANDLE; n: Integer; Sample: PChar): Boolean; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8EnumDataFormat(hIT8: LCMSHANDLE; var SampleNames: LCMSARRAYOFPCHAR): Integer; EXTERNAL 'lcms.dll';
FUNCTION cmsIT8GetPatchName(hIT8: LCMSHANDLE; nPatch: Integer; Buffer: PChar): PChar; EXTERNAL 'lcms.dll';

// The LABEL extension

FUNCTION cmsIT8SetTableByLabel(hIT8: LCMSHANDLE; cSet, cField, ExpectedType: PChar): Integer; EXTERNAL 'lcms.dll';




PROCEDURE cmsLabEncoded2Float(Lab: LPcmsCIELab; wLab: Pointer); EXTERNAL 'lcms.dll';
PROCEDURE cmsFloat2LabEncoded(wLab: Pointer; Lab: LPcmsCIELab); EXTERNAL 'lcms.dll';
PROCEDURE cmsXYZEncoded2Float(fxyz : LPcmsCIEXYZ; XYZ: Pointer); EXTERNAL 'lcms.dll';
PROCEDURE cmsFloat2XYZEncoded(XYZ: Pointer; fXYZ: LPcmsCIEXYZ); EXTERNAL 'lcms.dll';

FUNCTION _cmsAddTextTag(hProfile: cmsHPROFILE; sig: icTagSignature; Text: PChar): Boolean; EXTERNAL 'lcms.dll';
FUNCTION _cmsAddXYZTag(hProfile: cmsHPROFILE;  sig: icTagSignature;  XYZ: LPcmsCIEXYZ): Boolean; EXTERNAL 'lcms.dll';
FUNCTION _cmsAddLUTTag(hProfile: cmsHPROFILE;  sig: icTagSignature; lut: PByte): Boolean; EXTERNAL 'lcms.dll';



END.
