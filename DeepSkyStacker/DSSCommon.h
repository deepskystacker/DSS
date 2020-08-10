#ifndef __DSSCOMMON_H__
#define __DSSCOMMON_H__

#define REGENTRY_BASEKEY					_T("Software\\DeepSkyStacker")
#define REGENTRY_BASEKEY_DEEPSKYSTACKER		_T("Software\\DeepSkyStacker\\DeepSkyStacker")
#define REGENTRY_BASEKEY_EDITSTARS			_T("Software\\DeepSkyStacker\\EditStars")
#define REGENTRY_BASEKEY_FOLDERS			_T("Software\\DeepSkyStacker\\Folders")
#define REGENTRY_BASEKEY_REGISTERSETTINGS	_T("Software\\DeepSkyStacker\\Register")
#define REGENTRY_BASEKEY_STACKINGSETTINGS	_T("Software\\DeepSkyStacker\\Stacking")
#define REGENTRY_BASEKEY_RAWSETTINGS		_T("Software\\DeepSkyStacker\\RawDDP")
#define REGENTRY_BASEKEY_FITSSETTINGS		_T("Software\\DeepSkyStacker\\FitsDDP")
#define REGENTRY_BASEKEY_STARMASK			_T("Software\\DeepSkyStacker\\StarMask")
#define REGENTRY_BASEKEY_FILELISTS			_T("Software\\DeepSkyStacker\\FileLists")
#define REGENTRY_BASEKEY_SETTINGFILES		_T("Software\\DeepSkyStacker\\SettingsFiles")
#define REGENTRY_BASEKEY_LIVE				_T("Software\\DeepSkyStacker\\Live")
#define REGENTRY_BASEKEY_OUTPUT				_T("Software\\DeepSkyStacker\\Output")

#define REGENTRY_BASEKEY_DEEPSKYSTACKER_POSITION		_T("Software\\DeepSkyStacker\\DeepSkyStacker\\Position")
#define REGENTRY_BASEKEY_DEEPSKYSTACKER_STEPS_POSITION	_T("Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\StackingSteps\\Position")
#define REGENTRY_BASEKEY_DEEPSKYSTACKER_RECO_POSITION	_T("Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\Recommended\\Position")
#define REGENTRY_BASEKEY_DEEPSKYSTACKER_BATCH_POSITION	_T("Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\Batch\\Position")
#define REGENTRY_BASEKEY_LIVE_POSITION					_T("Software\\DeepSkyStacker\\DeepSkyStackerLive\\Position")

constexpr unsigned int	STARMAXSIZE = 50;
constexpr unsigned int 	RCCHECKSIZE = (5 * STARMAXSIZE) + 2; // 252

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
#define VERSION_DCRAW						"0.20"
#define VERSION_LIBTIFF						"4.0.9"
#define VERSION_CFITSIO						"3.43"

#define NEWENGINE

#ifndef PCL_PROJECT
#define DSSFILEDECODING 1
#endif

#include "SmartPtr.h"

#endif