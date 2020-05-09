#ifndef __DSSCOMMON_H__
#define __DSSCOMMON_H__

#define REGENTRY_BASEKEY_LIVE_POSITION					_T("Software\\DeepSkyStacker\\DeepSkyStackerLive\\Position")

typedef enum PICTURETYPE
{
	PICTURETYPE_UNKNOWN		= 0,
	PICTURETYPE_LIGHTFRAME	= 1,
	PICTURETYPE_DARKFRAME	= 2,
	PICTURETYPE_FLATFRAME	= 3,
	PICTURETYPE_OFFSETFRAME = 4,
	PICTURETYPE_REFLIGHTFRAME = 5,
	PICTURETYPE_DARKFLATFRAME = 6
}PICTURETYPE;

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

// TODO: reference additional headers your program requires here

#pragma warning( disable : 4244 )
#pragma warning( disable : 4018 )

#include "DSSVersion.h"
#define VERSION_DCRAW						"0.19.3"
#define VERSION_LIBTIFF						"4.0.9"
#define VERSION_CFITSIO						"3.43"

#define NEWENGINE

#ifndef PCL_PROJECT
#define DSSFILEDECODING 1
#endif

#include "SmartPtr.h"

#endif