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


// This program does apply profiles to (some) TIFF files


#include "lcms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef NON_WINDOWS
#include <io.h>
#endif

#include "tiffio.h"


// xgetopt() interface -----------------------------------------------------

extern int   xoptind;
extern char *xoptarg;
extern int   xopterr;
extern char   SW;
int    cdecl xgetopt(int argc, char *argv[], char *optionS);

// ------------------------------------------------------------------------

extern cmsHPROFILE OpenStockProfile(const char* File);


// Flags

static BOOL Verbose                = FALSE;
static BOOL BlackWhiteCompensation = FALSE;
static BOOL IgnoreEmbedded         = FALSE;
static BOOL EmbedProfile           = FALSE;
static BOOL Width16                = FALSE;
static BOOL GamutCheck             = FALSE;
static BOOL lIsDeviceLink          = FALSE;
static BOOL StoreAsAlpha           = FALSE;
static int PreserveBlack		   = 0;
static BOOL InputLabUsingICC	   = FALSE;

static int Intent                  = INTENT_PERCEPTUAL;
static int ProofingIntent          = INTENT_PERCEPTUAL;
static int PrecalcMode             = 1;
static double InkLimit             = 400;

static double ObserverAdaptationState = 0;


static const char *cInpProf  = NULL;
static const char *cOutProf  = NULL;
static const char *cProofing = NULL;

static const char* SaveEmbedded = NULL;

// Console error & warning


static
void ConsoleWarningHandler(const char* module, const char* fmt, va_list ap)
{
        char e[512] = { '\0' };
        if (module != NULL)
              strcat(strcpy(e, module), ": ");

        vsprintf(e+strlen(e), fmt, ap);
        strcat(e, ".");
        if (Verbose) {

              fprintf(stderr, "\nWarning");
              fprintf(stderr, " %s\n", e);
              fflush(stderr);
              }
}

static
void ConsoleErrorHandler(const char* module, const char* fmt, va_list ap)
{
       char e[512] = { '\0' };

       if (module != NULL)
              strcat(strcpy(e, module), ": ");

       vsprintf(e+strlen(e), fmt, ap);
       strcat(e, ".");
       fprintf(stderr, "\nError");
       fprintf(stderr, " %s\n", e);
       fflush(stderr);
}



// Force an error and exit w/ return code 1

static
void FatalError(const char *frm, ...)
{
       va_list args;

       va_start(args, frm);
       ConsoleErrorHandler("TIFFICC", frm, args);
       va_end(args);
       exit(1);
}

// Issue a warning

static
void Warning(const char *frm, ...)
{
       va_list args;

       va_start(args, frm);
       ConsoleWarningHandler("TIFFICC", frm, args);
       va_end(args);
}


static
int MyErrorHandler(int ErrorCode, const char *ErrorText)
{
    FatalError("%s", ErrorText);
    return 0;
}

// Out of mem

static
void OutOfMem(size_t size)
{
    FatalError("Out of memory on allocating %d bytes.", size);
}


// Build up the pixeltype descriptor

static
DWORD GetInputPixelType(TIFF *Bank)
{
     uint16 Photometric, bps, spp, extra, PlanarConfig, *info;
     uint16 Compression, reverse = 0;
     int ColorChannels, IsPlanar = 0, pt = 0;

     TIFFGetField(Bank,           TIFFTAG_PHOTOMETRIC,   &Photometric);
     TIFFGetFieldDefaulted(Bank,  TIFFTAG_BITSPERSAMPLE, &bps);

     if (bps == 1)
       FatalError("Sorry, bilevel TIFFs has nothig to do with ICC profiles");

     if (bps != 8 && bps != 16)
              FatalError("Sorry, 8 or 16 bits per sample only");

     TIFFGetFieldDefaulted(Bank, TIFFTAG_SAMPLESPERPIXEL, &spp);
     TIFFGetFieldDefaulted(Bank, TIFFTAG_PLANARCONFIG, &PlanarConfig);

     switch (PlanarConfig)
     {
     case PLANARCONFIG_CONTIG: IsPlanar = 0; break;
     case PLANARCONFIG_SEPARATE: IsPlanar = 1; break;
     default:

     FatalError("Unsupported planar configuration (=%d) ", (int) PlanarConfig);
     }

     // If Samples per pixel == 1, PlanarConfiguration is irrelevant and need
     // not to be included.

     if (spp == 1) IsPlanar = 0;


     // Any alpha?

     TIFFGetFieldDefaulted(Bank, TIFFTAG_EXTRASAMPLES, &extra, &info);

     // Read alpha channels as colorant

     if (StoreAsAlpha) {

         ColorChannels = spp;
         extra = 0;
     }
     else
        ColorChannels = spp - extra;

     switch (Photometric) {

     case PHOTOMETRIC_MINISWHITE:
                                   
            reverse = 1;

     case PHOTOMETRIC_MINISBLACK:
                                   
            pt = PT_GRAY;                                
            break;

     case PHOTOMETRIC_RGB:
                                   
            pt = PT_RGB;
            break;


     case PHOTOMETRIC_PALETTE:
                                             
            FatalError("Sorry, palette images not supported (at least on this version)"); 

     case PHOTOMETRIC_SEPARATED:
           if (ColorChannels == 4)
                  pt = PT_CMYK;
           else
           if (ColorChannels == 3)
                  pt = PT_CMY;
           else
           if (ColorChannels == 6)
                  pt = PT_HiFi;
           else
           if (ColorChannels == 7)
                  pt = PT_HiFi7;
           else
           if (ColorChannels == 8)
                  pt = PT_HiFi8;           
           else
           if (ColorChannels == 9)
                  pt = PT_HiFi9;           
           else
           if (ColorChannels == 10)
                  pt = PT_HiFi10;           
           else
           if (ColorChannels == 11)
                  pt = PT_HiFi11;           
           else
		   if (ColorChannels == 12)
                  pt = PT_HiFi8;           
		   else
           if (ColorChannels == 13)
                  pt = PT_HiFi13;           
           else
           if (ColorChannels == 14)
                  pt = PT_HiFi14;           
           else
           if (ColorChannels == 15)
                  pt = PT_HiFi15;           
           else
                  FatalError("What a weird separation of %d channels?!?!", ColorChannels);
           break;

     case PHOTOMETRIC_YCBCR:
           TIFFGetField(Bank, TIFFTAG_COMPRESSION, &Compression);
           {
                  uint16 subx, suby;

                  pt = PT_YCbCr;
                  TIFFGetFieldDefaulted(Bank, TIFFTAG_YCBCRSUBSAMPLING, &subx, &suby);
                  if (subx != 1 || suby != 1)
                         FatalError("Sorry, subsampled images not supported");

           }
           break;

     case 9:
			pt = PT_Lab;
			InputLabUsingICC = TRUE;
			break;

     case PHOTOMETRIC_CIELAB:
           pt = PT_Lab;
		   InputLabUsingICC = FALSE;
           break;

    
     case PHOTOMETRIC_LOGLUV:      /* CIE Log2(L) (u',v') */

           TIFFSetField(Bank, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_16BIT);
           pt = PT_YUV;             // *ICCSpace = icSigLuvData;
           bps = 16;               // 16 bits forced by LibTiff
           break;

     default:
           FatalError("Unsupported TIFF color space (Photometric %d)", Photometric);
     }

     // Convert bits per sample to bytes per sample

     bps >>= 3; 

     return (COLORSPACE_SH(pt)|PLANAR_SH(IsPlanar)|EXTRA_SH(extra)|CHANNELS_SH(ColorChannels)|BYTES_SH(bps)|FLAVOR_SH(reverse));
}



// Rearrange pixel type to build output descriptor

static
DWORD ComputeOutputFormatDescriptor(DWORD dwInput, int OutColorSpace, int bps)
{
    int IsPlanar  = T_PLANAR(dwInput);
    int Channels = 0;
    
    switch (OutColorSpace) {
        
    case PT_GRAY:
        Channels = 1;
        break;
    case PT_RGB:
    case PT_CMY:
    case PT_Lab:
    case PT_YUV:
    case PT_YCbCr:
        Channels = 3;
        break;
        
    case PT_CMYK:
        Channels = 4;
        break;
        
    case  PT_HiFi:
        Channels = 6;
        break;
        
    case PT_HiFi7:
        Channels = 7;
        break;
        
    case  PT_HiFi8:
        Channels = 8;
        break;
        
    case  PT_HiFi9:
        Channels = 9;
        break;
        
    case  PT_HiFi10:
        Channels = 10;
        break;
        
    case  PT_HiFi11:
        Channels = 11;
        break;
        
    case PT_HiFi12:
        Channels = 12;
        break;
        
    case  PT_HiFi13:
        Channels = 13;
        break;
        
    case  PT_HiFi14:
        Channels = 14;
        break;
        
    case  PT_HiFi15:
        Channels = 15;
        break;
        
    default:
        FatalError("Unsupported output color space");
    }
    
    return (COLORSPACE_SH(OutColorSpace)|PLANAR_SH(IsPlanar)|CHANNELS_SH(Channels)|BYTES_SH(bps));
}



// Tile based transforms

static
int TileBasedXform(cmsHTRANSFORM hXForm, TIFF* in, TIFF* out, int nPlanes)
{
    tsize_t BufSizeIn  = TIFFTileSize(in);
    tsize_t BufSizeOut = TIFFTileSize(out);
    unsigned char *BufferIn, *BufferOut;
    ttile_t i, TileCount = TIFFNumberOfTiles(in) / nPlanes;
    uint32 tw, tl;
    int PixelCount, j;


    TIFFGetFieldDefaulted(in, TIFFTAG_TILEWIDTH,  &tw);
    TIFFGetFieldDefaulted(in, TIFFTAG_TILELENGTH, &tl);

     PixelCount = (int) tw * tl;

     BufferIn = (unsigned char *) _TIFFmalloc(BufSizeIn * nPlanes);
    if (!BufferIn) OutOfMem(BufSizeIn * nPlanes);

    BufferOut = (unsigned char *) _TIFFmalloc(BufSizeOut * nPlanes);
    if (!BufferOut) OutOfMem(BufSizeOut * nPlanes);


    for (i = 0; i < TileCount; i++) {

        for (j=0; j < nPlanes; j++) {

            if (TIFFReadEncodedTile(in, i + (j* TileCount), 
                                    BufferIn + (j*BufSizeIn), BufSizeIn) < 0)   goto cleanup;
        }

      cmsDoTransform(hXForm, BufferIn, BufferOut, PixelCount);

      for (j=0; j < nPlanes; j++) {

             if (TIFFWriteEncodedTile(out, i + (j*TileCount),
                                    BufferOut + (j*BufSizeOut), BufSizeOut) < 0) goto cleanup;
      }

    }

    _TIFFfree(BufferIn);
    _TIFFfree(BufferOut);
    return 1;


cleanup:

    _TIFFfree(BufferIn);
   _TIFFfree(BufferOut);
   return 0;
}


// Strip based transforms

static
int StripBasedXform(cmsHTRANSFORM hXForm, TIFF* in, TIFF* out, int nPlanes)
{
    tsize_t BufSizeIn  = TIFFStripSize(in);
    tsize_t BufSizeOut = TIFFStripSize(out);
    unsigned char *BufferIn, *BufferOut;
    ttile_t i, StripCount = TIFFNumberOfStrips(in) / nPlanes;
    uint32 sw;
    uint32 sl;
    uint32 iml;
    int j;
    int PixelCount;

    TIFFGetFieldDefaulted(in, TIFFTAG_IMAGEWIDTH,  &sw);
    TIFFGetFieldDefaulted(in, TIFFTAG_ROWSPERSTRIP, &sl);
    TIFFGetFieldDefaulted(in, TIFFTAG_IMAGELENGTH, &iml);

   

    BufferIn = (unsigned char *) _TIFFmalloc(BufSizeIn * nPlanes);
    if (!BufferIn) OutOfMem(BufSizeIn * nPlanes);

    BufferOut = (unsigned char *) _TIFFmalloc(BufSizeOut * nPlanes);
    if (!BufferOut) OutOfMem(BufSizeOut * nPlanes);


    for (i = 0; i < StripCount; i++) {

        for (j=0; j < nPlanes; j++) {

            if (TIFFReadEncodedStrip(in, i + (j * StripCount), 
                                        BufferIn + (j * BufSizeIn), BufSizeIn) < 0)   goto cleanup;
        }

        PixelCount = (int) sw * (iml < sl ? iml : sl);
        iml -= sl;

        cmsDoTransform(hXForm, BufferIn, BufferOut, PixelCount);

        for (j=0; j < nPlanes; j++) {
            if (TIFFWriteEncodedStrip(out, i + (j * StripCount), 
                                     BufferOut + j * BufSizeOut, BufSizeOut) < 0) goto cleanup;
        }

    }

    _TIFFfree(BufferIn);
    _TIFFfree(BufferOut);
    return 1;


cleanup:

    _TIFFfree(BufferIn);
   _TIFFfree(BufferOut);
   return 0;
}





// Creates minimum required tags

static
void WriteOutputTags(TIFF *out, int Colorspace, int BytesPerSample)
{
    int BitsPerSample = (8 * BytesPerSample);
    uint16 Extra[] = { EXTRASAMPLE_UNASSALPHA, 
                       EXTRASAMPLE_UNASSALPHA, 
                       EXTRASAMPLE_UNASSALPHA,
                       EXTRASAMPLE_UNASSALPHA,
					   EXTRASAMPLE_UNASSALPHA, 
                       EXTRASAMPLE_UNASSALPHA, 
                       EXTRASAMPLE_UNASSALPHA,
                       EXTRASAMPLE_UNASSALPHA,
                       EXTRASAMPLE_UNASSALPHA,
                       EXTRASAMPLE_UNASSALPHA,
                       EXTRASAMPLE_UNASSALPHA
					};


  switch (Colorspace) {

  case PT_GRAY:
           TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
           TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
           TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
           break;

   case PT_RGB:
           TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
           TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
           TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
           break;

   case PT_CMY:
           TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
           TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
           TIFFSetField(out, TIFFTAG_INKSET, 2);
           TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
           break;

   case PT_CMYK:
           TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
           TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
           TIFFSetField(out, TIFFTAG_INKSET, INKSET_CMYK);
           TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
           break;

    case PT_Lab:
            if (BitsPerSample == 16) 
            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, 9);
            else
            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CIELAB);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);    // Needed by TIFF Spec
            break;

        

     case PT_HiFi:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 6);

            if (StoreAsAlpha) {                                            
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 2, Extra);            
            }
            else {            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }

            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;

    case PT_HiFi7:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);    
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 7);
            
            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 3, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }

            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;

     case PT_HiFi8:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 8);

            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 4, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }            
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;

    case PT_HiFi9:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 9);

            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 5, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }            
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;

    case PT_HiFi10:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 10);

            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 6, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }            
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;


    case PT_HiFi11:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 11);

            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 7, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }            
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;


	 case PT_HiFi12:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 12);

            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 8, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }            
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;

     case PT_HiFi13:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 13);

            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 9, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }            
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;

    case PT_HiFi14:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 14);

            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 10, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }            
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;

    case PT_HiFi15:

            TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);
            TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 15);

            if (StoreAsAlpha) {                                        
                TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 11, Extra);            
            }
            else {
            
                TIFFSetField(out, TIFFTAG_INKSET, 2);
            }            
            TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, BitsPerSample);
            break;

     default:
           FatalError("Unsupported output colorspace");

   }

}


// Copies a bunch of tages

static
void CopyOtherTags(TIFF* in, TIFF* out)
{
#define CopyField(tag, v) \
    if (TIFFGetField(in, tag, &v)) TIFFSetField(out, tag, v)


        short shortv;
        uint32 ow, ol;
        float floatv;
        char *stringv;
        uint32 longv;

        CopyField(TIFFTAG_SUBFILETYPE, longv);

        TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &ow);
        TIFFGetField(in, TIFFTAG_IMAGELENGTH, &ol);

        TIFFSetField(out, TIFFTAG_IMAGEWIDTH, ow);
        TIFFSetField(out, TIFFTAG_IMAGELENGTH, ol);

        CopyField(TIFFTAG_PLANARCONFIG, shortv);

        CopyField(TIFFTAG_COMPRESSION, shortv);
        CopyField(TIFFTAG_PREDICTOR, shortv);

        CopyField(TIFFTAG_THRESHHOLDING, shortv);
        CopyField(TIFFTAG_FILLORDER, shortv);
        CopyField(TIFFTAG_ORIENTATION, shortv);
        CopyField(TIFFTAG_MINSAMPLEVALUE, shortv);
        CopyField(TIFFTAG_MAXSAMPLEVALUE, shortv);
        CopyField(TIFFTAG_XRESOLUTION, floatv);
        CopyField(TIFFTAG_YRESOLUTION, floatv);
        CopyField(TIFFTAG_RESOLUTIONUNIT, shortv);
        CopyField(TIFFTAG_ROWSPERSTRIP, longv);
        CopyField(TIFFTAG_XPOSITION, floatv);
        CopyField(TIFFTAG_YPOSITION, floatv);
        CopyField(TIFFTAG_IMAGEDEPTH, longv);
        CopyField(TIFFTAG_TILEDEPTH, longv);

        CopyField(TIFFTAG_TILEWIDTH,  longv);
        CopyField(TIFFTAG_TILELENGTH, longv);

        CopyField(TIFFTAG_ARTIST, stringv);
        CopyField(TIFFTAG_IMAGEDESCRIPTION, stringv);
        CopyField(TIFFTAG_MAKE, stringv);
        CopyField(TIFFTAG_MODEL, stringv);

        CopyField(TIFFTAG_DATETIME, stringv);
        CopyField(TIFFTAG_HOSTCOMPUTER, stringv);
        CopyField(TIFFTAG_PAGENAME, stringv);
        CopyField(TIFFTAG_DOCUMENTNAME, stringv);

}

// A replacement for (the nonstandard) filelenght

static
int xfilelength(int fd)
{
#ifdef _MSC_VER
        return _filelength(fd);
#else
        struct stat sb;
        if (fstat(fd, &sb) < 0)
                return(-1);
        return(sb.st_size);
#endif


}

static
void DoEmbedProfile(TIFF* Out, const char* ProfileFile)
{
    FILE* f;
    size_t size, EmbedLen;
    LPBYTE EmbedBuffer;

        f = fopen(ProfileFile, "rb");
        if (f == NULL) return;

        size = xfilelength(fileno(f));
        EmbedBuffer = (LPBYTE) malloc(size + 1);
        EmbedLen = fread(EmbedBuffer, 1, size, f);
        fclose(f);
        EmbedBuffer[EmbedLen] = 0;

        TIFFSetField(Out, TIFFTAG_ICCPROFILE, EmbedLen, EmbedBuffer);
        free(EmbedBuffer);
}


static
void SaveMemoryBlock(const LPBYTE Buffer, DWORD dwLen, const char* Filename)
{
    FILE* out = fopen(Filename, "wb");
    if (out == NULL)
           FatalError("Cannot create '%s'", Filename);

    if (fwrite(Buffer, 1, dwLen, out) != dwLen)
            FatalError("Cannot write %ld bytes to %s", dwLen, Filename);

    if (fclose(out) != 0) 
            FatalError("Error flushing file '%s'", Filename);
}



static
cmsHPROFILE GetTIFFProfile(TIFF* in)
{    
    cmsCIExyYTRIPLE Primaries;
	float* chr;
    cmsCIExyY WhitePoint;
    float* wp;
    int i;       
    LPGAMMATABLE Gamma[3]; 
    LPWORD gmr, gmg, gmb;
    cmsHPROFILE hProfile;
    DWORD EmbedLen;
    LPBYTE EmbedBuffer;
      
              
       if (IgnoreEmbedded) return NULL;

       if (TIFFGetField(in, TIFFTAG_ICCPROFILE, &EmbedLen, &EmbedBuffer)) {

              hProfile = cmsOpenProfileFromMem(EmbedBuffer, EmbedLen);
   
              if (Verbose) {

                  fprintf(stdout, " (Embedded profile found)\n");
                  fprintf(stdout, "Product name: %s\n", cmsTakeProductName(hProfile));
                  fprintf(stdout, "Description : %s\n", cmsTakeProductDesc(hProfile));                          
                  fflush(stdout);
              }

              if (hProfile != NULL && SaveEmbedded != NULL)
                  SaveMemoryBlock(EmbedBuffer, EmbedLen, SaveEmbedded);

              if (hProfile) return hProfile;
       }

        // Try to see if "colorimetric" tiff

       if (TIFFGetField(in, TIFFTAG_PRIMARYCHROMATICITIES, &chr)) {
                      
           Primaries.Red.x   =  chr[0];
           Primaries.Red.y   =  chr[1];
           Primaries.Green.x =  chr[2];
           Primaries.Green.y =  chr[3];
           Primaries.Blue.x  =  chr[4];
           Primaries.Blue.y  =  chr[5];
           
           Primaries.Red.Y = Primaries.Green.Y = Primaries.Blue.Y = 1.0;
                      
           if (TIFFGetField(in, TIFFTAG_WHITEPOINT, &wp)) {
               
               WhitePoint.x = wp[0];
               WhitePoint.y = wp[1];
               WhitePoint.Y = 1.0;
                                             
               // Transferfunction is a bit harder....
               
               for (i=0; i < 3; i++)
                   Gamma[i] = cmsAllocGamma(256);
                                            
               TIFFGetFieldDefaulted(in, TIFFTAG_TRANSFERFUNCTION,
                   &gmr, 
                   &gmg,
                   &gmb);
               
               CopyMemory(Gamma[0]->GammaTable, gmr, 256*sizeof(WORD));
               CopyMemory(Gamma[1]->GammaTable, gmg, 256*sizeof(WORD));
               CopyMemory(Gamma[2]->GammaTable, gmb, 256*sizeof(WORD));
               
               hProfile = cmsCreateRGBProfile(&WhitePoint, &Primaries, Gamma);
               
               for (i=0; i < 3; i++)
                   cmsFreeGamma(Gamma[i]);

                if (Verbose) {
                  fprintf(stdout, " (Colorimetric TIFF)\n");
                }
             
               
               return hProfile;
           }
       }

       return NULL;
}


// Formatter for 8bit Lab TIFF (photometric 8)

static
unsigned char* UnrollTIFFLab8(register void* nfo, register WORD wIn[], register LPBYTE accum)
{
    _LPcmsTRANSFORM info = (_LPcmsTRANSFORM) nfo;

	   wIn[0] = (accum[0]) << 8;
	   wIn[1] = ((accum[1] > 127) ? (accum[1] - 128) : (accum[1] + 128)) << 8;
	   wIn[2] = ((accum[2] > 127) ? (accum[2] - 128) : (accum[2] + 128)) << 8;
       
       return accum + 3;
}


static
unsigned char* PackTIFFLab8(register void* nfo, register WORD wOut[], register LPBYTE output)
{
    _LPcmsTRANSFORM info = (_LPcmsTRANSFORM) nfo;

		*output++ = (wOut[0] + 0x0080) >> 8;

		wOut[1] = (wOut[1] + 0x0080) >> 8;
		wOut[2] = (wOut[2] + 0x0080) >> 8;

		*output++ = (wOut[1] < 128) ? (wOut[1] + 128) : (wOut[1] - 128);
		*output++ = (wOut[2] < 128) ? (wOut[2] + 128) : (wOut[2] - 128);

       return output;
}


// Transform one image

static
int TransformImage(TIFF* in, TIFF* out, const char *cDefInpProf, const char *cOutProf)
{
       cmsHPROFILE hIn, hOut, hProof, hInkLimit = NULL;
       cmsHTRANSFORM xform;
       DWORD wInput, wOutput;
       int OutputColorSpace;
       int bps = (Width16 ? 2 : 1);
       DWORD dwFlags = 0;        
       int nPlanes;

    // Observer adaptation state (only meaningful on absolute colorimetric intent)

       cmsSetAdaptationState(ObserverAdaptationState);

       if (EmbedProfile && cOutProf) 
           DoEmbedProfile(out, cOutProf);


       
       if (BlackWhiteCompensation) 
            dwFlags |= cmsFLAGS_WHITEBLACKCOMPENSATION;           
       

       if (PreserveBlack) {
			dwFlags |= cmsFLAGS_PRESERVEBLACK;
			if (PrecalcMode == 0) PrecalcMode = 1;
            cmsSetCMYKPreservationStrategy(PreserveBlack-1);
	   }

       switch (PrecalcMode) {
           
       case 0: dwFlags |= cmsFLAGS_NOTPRECALC; break;
       case 2: dwFlags |= cmsFLAGS_HIGHRESPRECALC; break;
       case 3: dwFlags |= cmsFLAGS_LOWRESPRECALC; break;
       case 1: break;

       default: FatalError("Unknown precalculation mode '%d'", PrecalcMode);
       }
        

       if (GamutCheck)
            dwFlags |= cmsFLAGS_GAMUTCHECK;
        

       hProof = NULL;
       hOut = NULL;

       if (lIsDeviceLink) {

            hIn = cmsOpenProfileFromFile(cDefInpProf, "r");                  
       }
       else {

               hIn =  GetTIFFProfile(in);

               if (hIn == NULL)                    
                       hIn = OpenStockProfile(cDefInpProf);               
               
               hOut = OpenStockProfile(cOutProf);
                 
               if (cProofing != NULL) {

                   hProof = OpenStockProfile(cProofing);
                   dwFlags |= cmsFLAGS_SOFTPROOFING;
               }
       }

       // Take input color space

       wInput = GetInputPixelType(in);

       // Assure both, input profile and input TIFF are on same colorspace

       if (_cmsLCMScolorSpace(cmsGetColorSpace(hIn)) != (int) T_COLORSPACE(wInput))
              FatalError("Input profile is not operating in proper color space");

      
       if (!lIsDeviceLink) 
                OutputColorSpace = _cmsLCMScolorSpace(cmsGetColorSpace(hOut));
       else 
                OutputColorSpace = _cmsLCMScolorSpace(cmsGetPCS(hIn));
                
       wOutput      = ComputeOutputFormatDescriptor(wInput, OutputColorSpace, bps);

       WriteOutputTags(out, OutputColorSpace, bps);
       CopyOtherTags(in, out);

       // Ink limit
       if (InkLimit != 400.0 && 
                (OutputColorSpace == PT_CMYK || OutputColorSpace == PT_CMY)) {

           cmsHPROFILE hProfiles[10];
           int nProfiles = 0;


           hInkLimit = cmsCreateInkLimitingDeviceLink(cmsGetColorSpace(hOut), InkLimit);

           hProfiles[nProfiles++] = hIn;
           if (hProof) {
                hProfiles[nProfiles++] = hProof;
                hProfiles[nProfiles++] = hProof;
           }

           hProfiles[nProfiles++] = hOut;
           hProfiles[nProfiles++] = hInkLimit;
                   
           xform = cmsCreateMultiprofileTransform(hProfiles, nProfiles, 
                                                wInput, wOutput, Intent, dwFlags);
           
       }
       else {

		   xform = cmsCreateProofingTransform(hIn, wInput, 
											  hOut, wOutput, 
											  hProof, Intent, 
											  ProofingIntent, 
											  dwFlags);
       }

      

       // Planar stuff

       if (T_PLANAR(wInput)) 
            nPlanes = T_CHANNELS(wInput) + T_EXTRA(wInput);
       else
            nPlanes = 1;


	   // TIFF Lab of 8 bits need special handling

		if (wInput == TYPE_Lab_8 && 
			   !InputLabUsingICC &&
			   cInpProf != NULL  &&
			   stricmp(cInpProf, "*Lab") == 0) {

					cmsSetUserFormatters(xform, TYPE_Lab_8, UnrollTIFFLab8, TYPE_Lab_8, NULL); 
		}


		if (wOutput == TYPE_Lab_8 && 			   
			   cOutProf != NULL  &&
			   stricmp(cOutProf, "*Lab") == 0) {

					cmsSetUserFormatters(xform, TYPE_Lab_8, NULL, TYPE_Lab_8, PackTIFFLab8); 
		}

	   
       // Handle tile by tile or strip by strip

       if (TIFFIsTiled(in)) {

                TileBasedXform(xform, in, out, nPlanes);
       }
       else {

                StripBasedXform(xform, in, out, nPlanes);
       }


       cmsDeleteTransform(xform);
       cmsCloseProfile(hIn);
       cmsCloseProfile(hOut);
       if (hInkLimit) 
           cmsCloseProfile(hInkLimit);
       if (hProof) 
           cmsCloseProfile(hProof);


       TIFFWriteDirectory(out);

       return 1;
}


// Simply print help

static
void Help(int level)
{
    fprintf(stderr, "little cms ICC profile applier for TIFF - v5.0\n\n");
    fflush(stderr);
    
     switch(level) {

     default:
     case 0:

     fprintf(stderr, "usage: tifficc [flags] input.tif output.tif\n");

     fprintf(stderr, "\nflags:\n\n");
     fprintf(stderr, "%cv - Verbose\n", SW);
     fprintf(stderr, "%ci<profile> - Input profile (defaults to sRGB)\n", SW);
     fprintf(stderr, "%co<profile> - Output profile (defaults to sRGB)\n", SW);   
     fprintf(stderr, "%cl<profile> - Transform by device-link profile\n", SW);   
     fprintf(stderr, "%ct<0,1,2,3> - Intent (0=Perceptual, 1=Colorimetric, 2=Saturation, 3=Absolute)\n", SW);    
     fprintf(stderr, "\n");
     fprintf(stderr, "%cc<0,1,2,3> - Precalculates transform (0=Off, 1=Normal, 2=Hi-res, 3=LoRes) [defaults to 1]\n", SW);     
     fprintf(stderr, "\n");

     fprintf(stderr, "%cw - Wide output (generates 16 bps tiff)\n", SW);
     fprintf(stderr, "%ca - Handle channels > 4 as alpha\n", SW);

     fprintf(stderr, "%cn - Ignore embedded profile on input\n", SW);
     fprintf(stderr, "%ce - Embed destination profile\n", SW);
     fprintf(stderr, "%cs<new profile> - Save embedded profile as <new profile>\n", SW);
     fprintf(stderr, "\n");

     
     fprintf(stderr, "%cp<profile> - Soft proof profile\n", SW);
     fprintf(stderr, "%cm<0,1,2,3> - Soft proof intent\n", SW);
     fprintf(stderr, "%cg - Marks out-of-gamut colors on softproof\n", SW);
     
     fprintf(stderr, "\n"); 
     fprintf(stderr, "%cb - Black point compensation\n", SW);
	 fprintf(stderr, "%cf<n> - Preserve black (CMYK only) 0=off, 1=black ink only, 2=full K plane\n", SW);
     fprintf(stderr, "%ck<0..400> - Ink-limiting in %% (CMYK only)\n", SW);
     fprintf(stderr, "%cd<0..1> - Observer adaptation state (abs.col. only)\n", SW);

     fprintf(stderr, "\n");
     fprintf(stderr, "%ch<0,1,2> - More help\n", SW);
     fprintf(stderr, "\n");
     fprintf(stderr, "You can also use '*Lab' and '*XYZ' as predefined, built-in\n");
     fprintf(stderr, "profiles for CIE L*a*b* and XYZ color spaces.\n");
     
     break;

     case 1:

     
     fprintf(stderr, "Examples:\n\n"
                     "To color correct from scanner to sRGB:\n"
                     "\ttifficc %ciscanner.icm in.tif out.tif\n"
                     "To convert from monitor1 to monitor2:\n"
                     "\ttifficc %cimon1.icm %comon2.icm in.tif out.tif\n"
                     "To make a CMYK separation:\n"
                     "\ttifficc %coprinter.icm inrgb.tif outcmyk.tif\n"
                     "To recover sRGB from a CMYK separation:\n"
                     "\ttifficc %ciprinter.icm incmyk.tif outrgb.tif\n"
                     "To convert from CIELab TIFF to sRGB\n"
                     "\ttifficc %ci*Lab in.tif out.tif\n\n", 
                     SW, SW, SW, SW, SW, SW);
     break;

     case 2:

    
     fprintf(stderr, "This program is intended to be a demo of the little cms\n"
                     "engine. Both lcms and this program are freeware. You can\n"
                     "obtain both in source code at http://www.littlecms.com\n"
                     "For suggestions, comments, bug reports etc. send mail to\n"
                     "info@littlecms.com\n\n");
    
     break;
     }

     fflush(stderr);
     exit(0);
}


// The toggles stuff

static
void HandleSwitches(int argc, char *argv[])
{
       int s;
      
       while ((s=xgetopt(argc,argv,"aAeEbBwWnNvVGgh:H:i:I:o:O:P:p:t:T:c:C:l:L:M:m:K:k:S:s:D:d:f:F:")) != EOF) {

       switch (s)
       {

       case 'a':
       case 'A':
            StoreAsAlpha = TRUE;
            break;
       case 'b':
       case 'B':
            BlackWhiteCompensation = TRUE;
            break;
       
       case 'c':
       case 'C':
            PrecalcMode = atoi(xoptarg);
            if (PrecalcMode < 0 || PrecalcMode > 3)
                    FatalError("Unknown precalc mode '%d'", PrecalcMode);
            break;

       case 'd':
       case 'D': ObserverAdaptationState = atof(xoptarg);
                 if (ObserverAdaptationState != 0 && 
                     ObserverAdaptationState != 1.0)
                        Warning("Adaptation states other that 0 or 1 are not yet implemented");
                 break;

       case 'e':
       case 'E':
            EmbedProfile = TRUE;
            break;

	   case 'f':
	   case 'F':
		    PreserveBlack = atoi(xoptarg);
            if (PreserveBlack < 0 || PreserveBlack > 2)
                    FatalError("Unknown PreserveBlack '%d'", PreserveBlack);
			break;

        case 'g':
        case 'G':
            GamutCheck = TRUE;
            break;

       case 'v':
       case 'V':
            Verbose = TRUE;
            break;

       case 'i':
       case 'I':
            if (lIsDeviceLink)
                   FatalError("Device-link already specified"); 

            cInpProf = xoptarg;
            break;

       case 'o':
       case 'O':
           if (lIsDeviceLink)
                   FatalError("Device-link already specified"); 

           cOutProf = xoptarg;
           break;

       case 'l':
       case 'L': 
                cInpProf = xoptarg;
                lIsDeviceLink = TRUE;
                break;

       case 'p':
       case 'P':
           cProofing = xoptarg;
           break;

       case 't':
       case 'T':
            Intent = atoi(xoptarg);
            if (Intent > 3) Intent = 3;
            if (Intent < 0) Intent = 0;
            break;


       case 'm':
       case 'M':
            ProofingIntent = atoi(xoptarg);
            if (ProofingIntent > 3) ProofingIntent = 3;
            if (ProofingIntent < 0) ProofingIntent = 0;
            break;

       case 'N':
       case 'n':
            IgnoreEmbedded = TRUE;
            break;

       case 'W':
       case 'w':
            Width16 = TRUE;
            break;

           
        case 'k':
        case 'K':
                InkLimit = atof(xoptarg);
                if (InkLimit < 0.0 || InkLimit > 400.0)
                        FatalError("Ink limit must be 0%%..400%%");
                break;
                

        case 's':
        case 'S': SaveEmbedded = xoptarg;
                  break;
                  
        case 'H':
        case 'h':  {

            int a =  atoi(xoptarg);
            Help(a); 
            }
            break;

  default:

       FatalError("Unknown option - run without args to see valid ones");
    }
       
    }
}


// The main sink

int main(int argc, char* argv[])
{
      TIFF *in, *out;
      char *Intents[] = {"perceptual",
                         "relative colorimetric",
                         "saturation",
                         "absolute colorimetric" };

      HandleSwitches(argc, argv);
  
	  cmsSetErrorHandler(MyErrorHandler);

      if ((argc - xoptind) != 2) {

              Help(0);              
              }

     
      if (Verbose) {

        if (lIsDeviceLink)
            fprintf(stdout, "%s(device link) -> %s [%s]", 
                                                argv[xoptind], 
                                                argv[xoptind+1], 
                                                Intents[Intent]);

        else
           fprintf(stdout, "%s(%s) -> %s(%s) [%s]", argv[xoptind],
                                                (cInpProf == NULL ? "sRGB": cInpProf), 
                                                argv[xoptind+1],
                                                (cOutProf == NULL ? "sRGB" : cOutProf), 
                                                Intents[Intent]);
        fflush(stdout);
      }

      TIFFSetErrorHandler(ConsoleErrorHandler);
      TIFFSetWarningHandler(ConsoleWarningHandler);

      in = TIFFOpen(argv[xoptind], "r");
      if (in == NULL) FatalError("Unable to open '%s'", argv[xoptind]);

      out = TIFFOpen(argv[xoptind+1], "w");

      if (out == NULL) {

             TIFFClose(in);
             FatalError("Unable to write '%s'", argv[xoptind+1]);
             }

      do {

              TransformImage(in, out, cInpProf, cOutProf);


      } while (TIFFReadDirectory(in));


      if (Verbose) { fprintf(stdout, "\n"); fflush(stdout); }

      TIFFClose(in);
      TIFFClose(out);

      return 0;
}

