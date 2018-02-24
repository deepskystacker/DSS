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


// This program does apply profiles to (some) JPEG files


#include "lcms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

#ifndef NON_WINDOWS
#    include <io.h>
#endif

#include "jpeglib.h"
#include "iccjpeg.h"

#define PROGNAME    "JPEGICC"

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
static BOOL BlackPointCompensation = FALSE;
static BOOL IgnoreEmbedded         = FALSE;
static BOOL GamutCheck             = FALSE;
static BOOL lIsITUFax              = FALSE;
static BOOL lIsPhotoshopApp13      = FALSE;
static BOOL lIsDeviceLink          = FALSE;
static BOOL EmbedProfile           = FALSE;
static int PreserveBlack		   = 0;

static const char* SaveEmbedded = NULL;

static int Intent                  = INTENT_PERCEPTUAL;
static int ProofingIntent          = INTENT_PERCEPTUAL;
static int PrecalcMode             = 1;

static int jpegQuality             = 75;

static char *cInpProf  = NULL;
static char *cOutProf  = NULL;
static char *cProofing = NULL;

static FILE * InFile;
static FILE * OutFile;

static struct jpeg_decompress_struct Decompressor;
static struct jpeg_compress_struct   Compressor;


static struct my_error_mgr {

    struct  jpeg_error_mgr pub;  // "public" fields
    LPVOID  Cargo;               // "private" fields

} ErrorHandler;

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
}

// Force an error and exit w/ return code 1

static
void FatalError(const char *frm, ...)
{
       va_list args;

       va_start(args, frm);
       ConsoleErrorHandler(PROGNAME, frm, args);
       va_end(args);
       exit(1);
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


METHODDEF(void)
my_output_message (j_common_ptr cinfo)
{
  char buffer[JMSG_LENGTH_MAX];

  (*cinfo->err->format_message) (cinfo, buffer);
  FatalError("%s %s", PROGNAME, buffer);  
}


METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  // struct my_error_mgr *myerr = (struct my_error_mgr *) cinfo->err;
  char buffer[JMSG_LENGTH_MAX];
  
  (*cinfo->err->format_message) (cinfo, buffer);
  FatalError(buffer);
}



static
BOOL IsITUFax(jpeg_saved_marker_ptr ptr)
{
    while (ptr) {
        
        if (ptr -> marker == (JPEG_APP0 + 1) && ptr -> data_length > 5)
        {
            JOCTET FAR* data = ptr -> data;
            
            if (GETJOCTET(data[0]) == 0x47 &&
                GETJOCTET(data[1]) == 0x33 &&
                GETJOCTET(data[2]) == 0x46 &&
                GETJOCTET(data[3]) == 0x41 &&
                GETJOCTET(data[4]) == 0x58)
                return TRUE;
        }
        
        ptr = ptr -> next;
    }

   return FALSE;

}

#define PS_FIXED_TO_FLOAT(h, l) ((float) (h) + ((float) (l)/(1<<16)))

static
BOOL ProcessPhotoshopAPP13(JOCTET FAR *data, int datalen)
{
    int i;

    for (i = 14; i < datalen; )
    {
        long len;
        unsigned int type;
                
        if (!(GETJOCTET(data[i]  ) == 0x38 &&
              GETJOCTET(data[i+1]) == 0x42 &&
              GETJOCTET(data[i+2]) == 0x49 &&
              GETJOCTET(data[i+3]) == 0x4D)) break; // Not recognized
                    
        i += 4; // identifying string 
        
        type = (unsigned int) (GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]));

        i += 2; // resource type 
        
        i += GETJOCTET(data[i]) + ((GETJOCTET(data[i]) & 1) ? 1 : 2);   // resource name 
        
        len = ((((GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]))<<8) + 
                         GETJOCTET(data[i+2]))<<8) + GETJOCTET(data[i+3]);

        i += 4; // Size 
        
        if (type == 0x03ED && len >= 16) {
            
            Decompressor.X_density = (int) PS_FIXED_TO_FLOAT(GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]),
                                                 GETJOCTET(data[i+2]<<8) + GETJOCTET(data[i+3]));
            Decompressor.Y_density = (int) PS_FIXED_TO_FLOAT(GETJOCTET(data[i+8]<<8) + GETJOCTET(data[i+9]),
                                                 GETJOCTET(data[i+10]<<8) + GETJOCTET(data[i+11]));
            
            // Set the density unit to 1 since the 
            // Vertical and Horizontal resolutions 
            // are specified in Pixels per inch 

            Decompressor.density_unit = 0x01;
            return TRUE;
            
        }
                
        i += len + ((len & 1) ? 1 : 0);   // Alignment             
    }
    return FALSE;
}


static
BOOL HandlePhotoshopAPP13(jpeg_saved_marker_ptr ptr)
{
    while (ptr) {
        
        if (ptr -> marker == (JPEG_APP0 + 13) && ptr -> data_length > 9)
        {    
            JOCTET FAR* data = ptr -> data;    
            
            if(GETJOCTET(data[0]) == 0x50 &&
               GETJOCTET(data[1]) == 0x68 &&
               GETJOCTET(data[2]) == 0x6F &&
               GETJOCTET(data[3]) == 0x74 &&
               GETJOCTET(data[4]) == 0x6F &&
               GETJOCTET(data[5]) == 0x73 &&
               GETJOCTET(data[6]) == 0x68 &&
               GETJOCTET(data[7]) == 0x6F &&
               GETJOCTET(data[8]) == 0x70) {
             
                ProcessPhotoshopAPP13(data, ptr -> data_length);
                return TRUE;
            }
        }
        
        ptr = ptr -> next;
    }
    
    return FALSE;    
}



static
BOOL OpenInput(const char* FileName)
{
    int m;
    
    lIsITUFax = FALSE;   
    InFile  = fopen(FileName, "rb");
    if (InFile == NULL) {
        FatalError("Cannot open '%s'", FileName);
        return FALSE;
    }
    
    // Now we can initialize the JPEG decompression object.
    
    Decompressor.err                 = jpeg_std_error(&ErrorHandler.pub);
    ErrorHandler.pub.error_exit      = my_error_exit;
    ErrorHandler.pub.output_message  = my_output_message;
    
    jpeg_create_decompress(&Decompressor);
    jpeg_stdio_src(&Decompressor, InFile);
    
    for (m = 0; m < 16; m++)
        jpeg_save_markers(&Decompressor, JPEG_APP0 + m, 0xFFFF);
    
    setup_read_icc_profile(&Decompressor);  
    
    fseek(InFile, 0, SEEK_SET);
    jpeg_read_header(&Decompressor, TRUE);
    
    return TRUE;
}


static
BOOL OpenOutput(const char* FileName)
{

        OutFile = fopen(FileName, "wb");
        if (OutFile == NULL) {
                    FatalError("Cannot create '%s'", FileName);
                    return FALSE;
        }

        Compressor.err                   = jpeg_std_error(&ErrorHandler.pub);
        ErrorHandler.pub.error_exit      = my_error_exit;
        ErrorHandler.pub.output_message  = my_output_message;

        Compressor.input_components = Compressor.num_components = 4;
        
        jpeg_create_compress(&Compressor);
        jpeg_stdio_dest(&Compressor, OutFile);  
        
        return TRUE;
}

static
BOOL Done()
{
       jpeg_destroy_decompress(&Decompressor);
       jpeg_destroy_compress(&Compressor);
       return fclose(InFile) + fclose(OutFile);
       
}


// Build up the pixeltype descriptor

static
DWORD GetInputPixelType(void)
{
     int space, bps, extra, ColorChannels, Flavor;
        
   
     lIsITUFax         = IsITUFax(Decompressor.marker_list);
     lIsPhotoshopApp13 = HandlePhotoshopAPP13(Decompressor.marker_list);

     ColorChannels = Decompressor.num_components;
     extra  = 0;            // Alpha = None
     bps    = 1;            // 8 bits
     Flavor = 0;            // Vanilla

     if (lIsITUFax) {

        space = PT_Lab;
        Decompressor.out_color_space = JCS_YCbCr;  // Fake to don't touch
     }
     else
     switch (Decompressor.jpeg_color_space) {

     case JCS_GRAYSCALE:        // monochrome
              space = PT_GRAY;
              Decompressor.out_color_space = JCS_GRAYSCALE;
              break;

     case JCS_RGB:             // red/green/blue
              space = PT_RGB;
              Decompressor.out_color_space = JCS_RGB;
              break;

     case JCS_YCbCr:               // Y/Cb/Cr (also known as YUV)
              space = PT_RGB;      // Let IJG code to do the conversion
              Decompressor.out_color_space = JCS_RGB;   
              break;

     case JCS_CMYK:            // C/M/Y/K
              space = PT_CMYK;
              Decompressor.out_color_space = JCS_CMYK;
              if (Decompressor.saw_Adobe_marker)            // Adobe keeps CMYK inverted, so change flavor
                                Flavor = 1;                 // from vanilla to chocolate
              break;

     case JCS_YCCK:            // Y/Cb/Cr/K              
              space = PT_CMYK;
              Decompressor.out_color_space = JCS_CMYK;
              if (Decompressor.saw_Adobe_marker)            // ditto
                                Flavor = 1;                 
              break;

     default:
              FatalError("Unsupported color space (0x%x)", Decompressor.jpeg_color_space);
              return 0;
     }

     return (EXTRA_SH(extra)|CHANNELS_SH(ColorChannels)|BYTES_SH(bps)|COLORSPACE_SH(space)|FLAVOR_SH(Flavor));
}





// Rearrange pixel type to build output descriptor

static
DWORD ComputeOutputFormatDescriptor(DWORD dwInput, int OutColorSpace)
{
   int IsPlanar  = T_PLANAR(dwInput);
   int Channels  = 0;
   int Flavor    = 0;

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
               if (Compressor.write_Adobe_marker)   // Adobe keeps CMYK inverted, so change flavor to chocolate
                        Flavor = 1;

               Channels = 4;
               break;
   default:
               FatalError("Unsupported output color space");
   }

    return (COLORSPACE_SH(OutColorSpace)|PLANAR_SH(IsPlanar)|CHANNELS_SH(Channels)|BYTES_SH(1)|FLAVOR_SH(Flavor));
}


// Equivalence between ICC color spaces and lcms color spaces

static
int GetProfileColorSpace(cmsHPROFILE hProfile)
{
    icColorSpaceSignature ProfileSpace = cmsGetColorSpace(hProfile);

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


       default:  return icMaxEnumData;
       }
}

// From TRANSUPP

static 
void jcopy_markers_execute(j_decompress_ptr srcinfo, j_compress_ptr dstinfo)
{
  jpeg_saved_marker_ptr marker;

  /* In the current implementation, we don't actually need to examine the
   * option flag here; we just copy everything that got saved.
   * But to avoid confusion, we do not output JFIF and Adobe APP14 markers
   * if the encoder library already wrote one.
   */
  for (marker = srcinfo->marker_list; marker != NULL; marker = marker->next) {
    if (dstinfo->write_JFIF_header &&
    marker->marker == JPEG_APP0 &&
    marker->data_length >= 5 &&
    GETJOCTET(marker->data[0]) == 0x4A &&
    GETJOCTET(marker->data[1]) == 0x46 &&
    GETJOCTET(marker->data[2]) == 0x49 &&
    GETJOCTET(marker->data[3]) == 0x46 &&
    GETJOCTET(marker->data[4]) == 0)
      continue;         /* reject duplicate JFIF */

    if (dstinfo->write_Adobe_marker &&
    marker->marker == JPEG_APP0+14 &&
    marker->data_length >= 5 &&
    GETJOCTET(marker->data[0]) == 0x41 &&
    GETJOCTET(marker->data[1]) == 0x64 &&
    GETJOCTET(marker->data[2]) == 0x6F &&
    GETJOCTET(marker->data[3]) == 0x62 &&
    GETJOCTET(marker->data[4]) == 0x65)
      continue;         /* reject duplicate Adobe */

#ifdef NEED_FAR_POINTERS
    /* We could use jpeg_write_marker if the data weren't FAR... */
    {
      unsigned int i;

      jpeg_write_m_header(dstinfo, marker->marker, marker->data_length);
      for (i = 0; i < marker->data_length; i++)
        jpeg_write_m_byte(dstinfo, marker->data[i]);
    }
#else
    jpeg_write_marker(dstinfo, marker->marker,
              marker->data, marker->data_length);
#endif
  }
}

static
void WriteOutputFields(int OutputColorSpace)
{
    J_COLOR_SPACE in_space, jpeg_space;
    int components;
    
    switch (OutputColorSpace) {
        
    case PT_GRAY: in_space = jpeg_space = JCS_GRAYSCALE; 
                  components = 1; 
                  break;

    case PT_RGB:  in_space = JCS_RGB; 
                  jpeg_space = JCS_YCbCr; 
                  components = 3; 
                  break;       // red/green/blue

    case PT_YCbCr: in_space = jpeg_space = JCS_YCbCr; 
                   components = 3; 
                   break;               // Y/Cb/Cr (also known as YUV)

    case PT_CMYK: in_space = JCS_CMYK; 
                  jpeg_space = JCS_YCCK;  
                  components = 4; 
                  break;      // C/M/Y/components              

    case PT_Lab:  in_space = jpeg_space = JCS_YCbCr; 
                  components = 3; 
                  break;                // Fake to don't touch
    default:
                FatalError("Unsupported output color space");
                return;
    }
    

    if (jpegQuality >= 100) {

     // avoid destructive conversion when asking for lossless compression 
        jpeg_space = in_space;
    }

    Compressor.in_color_space =  in_space;
    Compressor.jpeg_color_space = jpeg_space;
    Compressor.input_components = Compressor.num_components = components;
    jpeg_set_defaults(&Compressor);
    jpeg_set_colorspace(&Compressor, jpeg_space);

    // Make sure to pass resolution through
    if (OutputColorSpace == PT_CMYK)
        Compressor.write_JFIF_header = 1;

    //avoid subsampling on high quality factor
    jpeg_set_quality(&Compressor, jpegQuality, 1);    
    if (jpegQuality >= 70) {

      int i;
      for(i=0; i < Compressor.num_components; i++) {

	        Compressor.comp_info[i].h_samp_factor = 1;
            Compressor.comp_info[i].v_samp_factor = 1;
      }

    }

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
void DoEmbedProfile(const char* ProfileFile)
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

        write_icc_profile (&Compressor, EmbedBuffer, EmbedLen);		   
        free(EmbedBuffer);
}



static
int DoTransform(cmsHTRANSFORM hXForm)
{       
    JSAMPROW ScanLineIn;
    JSAMPROW ScanLineOut;

    
       //Preserve resolution values from the original
       // (Thanks to robert bergs for finding out this bug)

       Compressor.density_unit = Decompressor.density_unit;
       Compressor.X_density    = Decompressor.X_density;
       Compressor.Y_density    = Decompressor.Y_density;

      //  Compressor.write_JFIF_header = 1;
     
       jpeg_start_decompress(&Decompressor);
       jpeg_start_compress(&Compressor, TRUE);

       // Embed the profile if needed
       if (EmbedProfile && cOutProf) 
           DoEmbedProfile(cOutProf);

       ScanLineIn  = (JSAMPROW) malloc(Decompressor.output_width * Decompressor.num_components);
       ScanLineOut = (JSAMPROW) malloc(Compressor.image_width * Compressor.num_components);

       while (Decompressor.output_scanline <
                            Decompressor.output_height) {

       jpeg_read_scanlines(&Decompressor, &ScanLineIn, 1);

       cmsDoTransform(hXForm, ScanLineIn, ScanLineOut, Decompressor.output_width);

       jpeg_write_scanlines(&Compressor, &ScanLineOut, 1);

       }

       free(ScanLineIn); 
       free(ScanLineOut);

       jpeg_finish_decompress(&Decompressor);
       jpeg_finish_compress(&Compressor);
       
       return TRUE;
}




static
void SaveMemoryBlock(const BYTE* Buffer, DWORD dwLen, const char* Filename)
{
    FILE* out = fopen(Filename, "wb");
    if (out == NULL)
           FatalError("Cannot create '%s'", Filename);

    if (fwrite(Buffer, 1, dwLen, out) != dwLen)
            FatalError("Cannot write %ld bytes to %s", dwLen, Filename);

    if (fclose(out) != 0) 
            FatalError("Error flushing file '%s'", Filename);
}



// Transform one image

static
int TransformImage(char *cDefInpProf, char *cOutProf)
{
       cmsHPROFILE hIn, hOut, hProof;
       cmsHTRANSFORM xform;
       DWORD wInput, wOutput;
       int OutputColorSpace;
       DWORD dwFlags = 0; 
       DWORD EmbedLen;
       LPBYTE EmbedBuffer;


       if (BlackPointCompensation) {

            dwFlags |= cmsFLAGS_BLACKPOINTCOMPENSATION;            
       }


       if (PreserveBlack) {
			dwFlags |= cmsFLAGS_PRESERVEBLACK;
			if (PrecalcMode == 0) PrecalcMode = 1;
            cmsSetCMYKPreservationStrategy(PreserveBlack-1);
	   }


       switch (PrecalcMode) {
           
       case 0: dwFlags |= cmsFLAGS_NOTPRECALC; break;
       case 2: dwFlags |= cmsFLAGS_HIGHRESPRECALC; break;
       case 3: dwFlags |= cmsFLAGS_LOWRESPRECALC; break;
       default:;
       }
        

       if (GamutCheck)
            dwFlags |= cmsFLAGS_GAMUTCHECK;
        

        if (lIsDeviceLink) {

            hIn = cmsOpenProfileFromFile(cDefInpProf, "r");
            hOut = NULL;
            hProof = NULL;
       }
        else {

        if (!IgnoreEmbedded && read_icc_profile(&Decompressor, &EmbedBuffer, &EmbedLen))
        {
              hIn = cmsOpenProfileFromMem(EmbedBuffer, EmbedLen);

               if (Verbose) {

                  fprintf(stdout, " (Embedded profile found)\n");
                  fprintf(stdout, "Product name: %s\n", cmsTakeProductName(hIn));
                  fprintf(stdout, "Description : %s\n", cmsTakeProductDesc(hIn));                          
                  fflush(stdout);
              }

               if (hIn != NULL && SaveEmbedded != NULL)
                          SaveMemoryBlock(EmbedBuffer, EmbedLen, SaveEmbedded);

              free(EmbedBuffer);
        }
        else
        {
                hIn = OpenStockProfile(cDefInpProf);
       }

        hOut = OpenStockProfile(cOutProf);


       hProof = NULL;
       if (cProofing != NULL) {

           hProof = OpenStockProfile(cProofing);
           dwFlags |= cmsFLAGS_SOFTPROOFING;
          }
       }

       // Take input color space

       wInput = GetInputPixelType();

       // Assure both, input profile and input JPEG are on same colorspace

       
       if (cmsGetColorSpace(hIn) != _cmsICCcolorSpace(T_COLORSPACE(wInput)))
              FatalError("Input profile is not operating in proper color space");
       

       // Output colorspace is given by output profile

        if (lIsDeviceLink) {
            OutputColorSpace = T_COLORSPACE(wInput);
        }
        else {
            OutputColorSpace = GetProfileColorSpace(hOut);
        }

       jpeg_copy_critical_parameters(&Decompressor, &Compressor);
       
       WriteOutputFields(OutputColorSpace);               
       
       wOutput      = ComputeOutputFormatDescriptor(wInput, OutputColorSpace);
       
       xform = cmsCreateProofingTransform(hIn, wInput, 
                                          hOut, wOutput, 
                                          hProof, Intent, 
                                          ProofingIntent, dwFlags);

       // Handle tile by tile or strip by strip strtok

       DoTransform(xform);

       
       jcopy_markers_execute(&Decompressor, &Compressor);
       
       cmsDeleteTransform(xform);
       cmsCloseProfile(hIn);
       cmsCloseProfile(hOut);
       if (hProof) cmsCloseProfile(hProof);
       
       return 1;
}


// Simply print help

static
void Help(int level)
{
     fprintf(stderr, "little cms ICC profile applier for JPEG - v2.0\n\n");

     switch(level) {

     default:
     case 0:

     fprintf(stderr, "usage: jpegicc [flags] input.jpg output.jpg\n");

     fprintf(stderr, "\nflags:\n\n");
     fprintf(stderr, "%cv - Verbose\n", SW);
     fprintf(stderr, "%ci<profile> - Input profile (defaults to sRGB)\n", SW);
     fprintf(stderr, "%co<profile> - Output profile (defaults to sRGB)\n", SW);   
     fprintf(stderr, "%ct<0,1,2,3> - Intent (0=Perceptual, 1=Colorimetric, 2=Saturation, 3=Absolute)\n", SW);    
     
     fprintf(stderr, "\n");

     fprintf(stderr, "%cb - Black point compensation\n", SW);
     fprintf(stderr, "%cf<n> - Preserve black (CMYK only) 0=off, 1=black ink only, 2=full K plane\n", SW);
     fprintf(stderr, "%cn - Ignore embedded profile\n", SW);
     fprintf(stderr, "%ce - Embed destination profile\n", SW);
     fprintf(stderr, "%cs<new profile> - Save embedded profile as <new profile>\n", SW);

     fprintf(stderr, "\n");

     fprintf(stderr, "%cc<0,1,2,3> - Precalculates transform (0=Off, 1=Normal, 2=Hi-res, 3=LoRes) [defaults to 1]\n", SW);     
     fprintf(stderr, "\n");

     fprintf(stderr, "%cp<profile> - Soft proof profile\n", SW);
     fprintf(stderr, "%cm<0,1,2,3> - SoftProof intent\n", SW);
     fprintf(stderr, "%cg - Marks out-of-gamut colors on softproof\n", SW);

     fprintf(stderr, "\n");
     fprintf(stderr, "%cq<0..100> - Output JPEG quality\n", SW);

     fprintf(stderr, "\n");
     fprintf(stderr, "%ch<0,1,2> - More help\n", SW);
     break;

     case 1:

     
     fprintf(stderr, "Examples:\n\n"
                     "To color correct from scanner to sRGB:\n"
                     "\tjpegicc %ciscanner.icm in.jpg out.jpg\n"
                     "To convert from monitor1 to monitor2:\n"
                     "\tjpegicc %cimon1.icm %comon2.icm in.jpg out.jpg\n"
                     "To make a CMYK separation:\n"
                     "\tjpegicc %coprinter.icm inrgb.jpg outcmyk.jpg\n"
                     "To recover sRGB from a CMYK separation:\n"
                     "\tjpegicc %ciprinter.icm incmyk.jpg outrgb.jpg\n"
                     "To convert from CIELab ITU/Fax JPEG to sRGB\n"
                     "\tjpegicc %ciitufax.icm in.jpg out.jpg\n\n", 
                     SW, SW, SW, SW, SW, SW);
     break;

     case 2:

     fprintf(stderr, "This program is intended to be a demo of the little cms\n"
                     "engine. Both lcms and this program are freeware. You can\n"
                     "obtain both in source code at http://www.littlecms.com\n"
                     "For suggestions, comments, bug reports etc. send mail to\n"
                     "marti@littlecms.com\n\n");
     break;
     }

     exit(0);
}


// The toggles stuff

static
void HandleSwitches(int argc, char *argv[])
{
    int s;
    
    while ((s=xgetopt(argc,argv,"bBnNvVGgh:H:i:I:o:O:P:p:t:T:c:C:Q:q:M:m:L:l:eEs:S:F:f:")) != EOF) {
        
        switch (s)
        {
            
        case 'b':
        case 'B':
            BlackPointCompensation = TRUE;
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
            
        case 'N':
        case 'n':
            IgnoreEmbedded = TRUE;
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
            
        case 'c':
        case 'C':
            PrecalcMode = atoi(xoptarg);
            if (PrecalcMode < 0 || PrecalcMode > 2)
                FatalError("Unknown precalc mode '%d'", PrecalcMode);
            break;
            
        case 'H':
        case 'h':  {
            
            int a =  atoi(xoptarg);
            Help(a); 
                   }
            break;
            
        case 'q':
        case 'Q':
            jpegQuality = atoi(xoptarg);
            if (jpegQuality > 100) jpegQuality = 100;
            if (jpegQuality < 0)   jpegQuality = 0;
            break;
            
        case 'm':
        case 'M':
            ProofingIntent = atoi(xoptarg);
            if (ProofingIntent > 3) ProofingIntent = 3;
            if (ProofingIntent < 0) ProofingIntent = 0;
            break;
            
        case 's':
        case 'S': SaveEmbedded = xoptarg;
            break;
            
            
        default:
            
            FatalError("Unknown option - run without args to see valid ones");
        }
        
    }
}


// The main sink

int main(int argc, char* argv[])
{
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
      }

      OpenInput(argv[xoptind]);
      OpenOutput(argv[xoptind+1]);
      TransformImage(cInpProf, cOutProf);


      if (Verbose) fprintf(stdout, "\n");

      Done();

      return 0;
}



