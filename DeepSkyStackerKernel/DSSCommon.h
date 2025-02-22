#pragma once

constexpr auto PI = 3.14159265358979323846;

#define REGENTRY_BASEKEY_LIVE				_T("Software\\DeepSkyStacker5\\Live")
#define REGENTRY_BASEKEY_LIVE_POSITION _T("Software\\DeepSkyStacker5\\DeepSkyStackerLive\\Position")

constexpr int	STARMAXSIZE = 50;
constexpr int 	RCCHECKSIZE = (5 * STARMAXSIZE) + 2; // 252

//
// The value of stacking mode is persisted in filelist files and in settings files
// This means the NONE of these enums should ever be changed.  So SM_CUSTOM must 
// always have a value of 2 (for example)
//
typedef enum tagSTACKINGMODE
{
	SM_NORMAL = 0,
	SM_MOSAIC = 1,
	SM_CUSTOM = 2,				// WARNING!! NEVER CHANGE THE VALUE OF SM_CUSTOM
	SM_INTERSECTION = 3
}STACKINGMODE;

typedef enum INTERMEDIATEFILEFORMAT
{
	IFF_TIFF = 1,
	IFF_FITS = 2
}INTERMEDIATEFILEFORMAT;

typedef enum tagCOMETSTACKINGMODE
{
	CSM_STANDARD = 0,
	CSM_COMETONLY = 1,
	CSM_COMETSTAR = 2
}COMETSTACKINGMODE;

enum BACKGROUNDCALIBRATIONMODE : short
{
	BCM_NONE = 0,
	BCM_PERCHANNEL = 1,
	BCM_RGB = 2
};

enum BACKGROUNDCALIBRATIONINTERPOLATION : short
{
	BCI_LINEAR = 0,
	BCI_RATIONAL = 1
};

enum ALIGNMENT_VALUES : short
{
	ALIGN_AUTO = 0,
	ALIGN_BILINEAR = 2,
	ALIGN_BISQUARED = 3,
	ALIGN_BICUBIC = 4,
	ALIGN_NONE = 5
};

enum RGBBACKGROUNDCALIBRATIONMETHOD : short
{
	RBCM_MINIMUM = 0,
	RBCM_MIDDLE = 1,
	RBCM_MAXIMUM = 2
};

enum MULTIBITMAPPROCESSMETHOD : short
{
	MBP_AVERAGE = 1,
	MBP_MEDIAN = 2,
	MBP_MAXIMUM = 3,
	MBP_SIGMACLIP = 4,
	MBP_ENTROPYAVERAGE = 5,
	MBP_AUTOADAPTIVE = 6,
	MBP_MEDIANSIGMACLIP = 7,
	MBP_FASTAVERAGE = 8
};

typedef enum tagCOSMETICREPLACE
{
	CR_MEDIAN = 1,
	CR_GAUSSIAN = 2
}COSMETICREPLACE;

enum PICTURETYPE : short
{
	PICTURETYPE_UNKNOWN		= 0,
	PICTURETYPE_LIGHTFRAME	= 1,
	PICTURETYPE_DARKFRAME	= 2,
	PICTURETYPE_FLATFRAME	= 3,
	PICTURETYPE_OFFSETFRAME = 4,
	PICTURETYPE_REFLIGHTFRAME = 5,
	PICTURETYPE_DARKFLATFRAME = 6
};

typedef enum TIFFFORMAT
{
	TF_UNKNOWN			= 0,
	TF_8BITRGB			= 1,
	TF_16BITRGB			= 2,
	TF_32BITRGB			= 3,
	TF_32BITRGBFLOAT	= 4,
	TF_8BITGRAY			= 5,
	TF_16BITGRAY		= 6,
	TF_32BITGRAY		= 7,
	TF_32BITGRAYFLOAT	= 8
}TIFFFORMAT;

typedef enum TIFFCOMPRESSION
{
	TC_NONE				= 0,
	TC_LZW				= 1,
	TC_DEFLATE			= 2
}TIFFCOMPRESSION;

enum class TERMINAL_OUTPUT_MODE
{
	BASIC = 0,
	COLOURED = 1,
};

typedef enum FITSFORMAT
{
	FF_UNKNOWN			= 0,
	FF_8BITRGB			= 1,
	FF_16BITRGB			= 2,
	FF_32BITRGB			= 3,
	FF_32BITRGBFLOAT	= 4,
	FF_8BITGRAY			= 5,
	FF_16BITGRAY		= 6,
	FF_32BITGRAY		= 7,
	FF_32BITGRAYFLOAT	= 8
}FITSFORMAT;

enum class Column
{
	Path = 0, File, Type, Filter, Score, Quality,
	dX, dY, Angle, FileTime, Size, CFA, Depth,
	Info, ISO, Exposure, Aperture, FWHM, Stars,
	Background, MAX_COLS
};

enum class ActivePanel
{
	StackingPanel,
	ProcessingPanel
};


// TODO: reference additional headers your program requires here

#pragma warning( disable : 4244 )
#pragma warning( disable : 4018 )

#include "DSSVersion.h"
#define VERSION_LIBTIFF						"4.0.9"
#define VERSION_CFITSIO						"3.43"

#define NEWENGINE