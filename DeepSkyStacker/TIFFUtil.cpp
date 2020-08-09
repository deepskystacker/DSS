#include <stdafx.h>
#include "TIFFUtil.h"

#include "zlib.h"
#include <iostream>
#include <QSettings>

#include <omp.h>

#define NRCUSTOMTIFFTAGS		12

static const TIFFFieldInfo DSStiffFieldInfo[NRCUSTOMTIFFTAGS] =
{
    { TIFFTAG_DSS_NRFRAMES,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      false,	false,	"DSSNumberOfFrames" },
    { TIFFTAG_DSS_TOTALEXPOSUREOLD, 1, 1, TIFF_LONG, FIELD_CUSTOM,
      false,	false,	"DSSTotalExposureOld" },
    { TIFFTAG_DSS_TOTALEXPOSURE, 1, 1, TIFF_FLOAT, FIELD_CUSTOM,
      false,	false,	"DSSTotalExposure" },
    { TIFFTAG_DSS_ISO,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      false,	false,	"DSSISO" },
    { TIFFTAG_DSS_GAIN,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      false,	false,	"DSSGain" },
    { TIFFTAG_DSS_SETTINGSAPPLIED,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      false,	false,	"DSSSettingsApplied" },
    { TIFFTAG_DSS_BEZIERSETTINGS,	-1,-1, TIFF_ASCII, FIELD_CUSTOM,
      false,	false,	"DSSBezierSettings" },
    { TIFFTAG_DSS_ADJUSTSETTINGS,	-1,-1, TIFF_ASCII, FIELD_CUSTOM,
      false,	false,	"DSSAdjustSettings" },
    { TIFFTAG_DSS_CFA,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      false,	false,	"DSSCFA" },
    { TIFFTAG_DSS_MASTER,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      false,	false,	"DSSMaster" },
    { TIFFTAG_DSS_CFATYPE,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      false,	false,	"DSSCFATYPE" },
	{ TIFFTAG_DSS_APERTURE, 1, 1, TIFF_FLOAT, FIELD_CUSTOM,
	  false,	false,	"DSSAperture" },

};

static TIFFExtendProc	g_TIFFParentExtender = nullptr;
static bool				g_TIFFInitialized = false;

/* ------------------------------------------------------------------- */

static void DSSTIFFDefaultDirectory(TIFF *tif)
{
    /* Install the extended Tag field info */
    TIFFMergeFieldInfo(tif, DSStiffFieldInfo, NRCUSTOMTIFFTAGS);

    /* Since an XTIFF client module may have overridden
     * the default directory method, we call it now to
     * allow it to set up the rest of its own methods.
     */

    if (g_TIFFParentExtender)
        (*g_TIFFParentExtender)(tif);
}

/* ------------------------------------------------------------------- */

void DSSTIFFInitialize()
{
    if (!g_TIFFInitialized)
	{
		g_TIFFInitialized = true;
	    /* Grab the inherited method and install */
		g_TIFFParentExtender = TIFFSetTagExtender(DSSTIFFDefaultDirectory);
	};
}

/* ------------------------------------------------------------------- */

bool CTIFFReader::Open()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	QSettings		settings;
	DWORD			dwSkipExifInfo = 0;

	dwSkipExifInfo = settings.value("SkipTIFFExifInfo", uint(0)).toUInt();

	//
	// Quietly attempt to open the putative TIFF file 
	//
	TIFFErrorHandler	oldHandler = TIFFSetErrorHandler(nullptr);
	TIFFErrorHandlerExt	oldHandlerExt = TIFFSetErrorHandlerExt(nullptr);
	m_tiff = TIFFOpen(CT2CA(m_strFileName, CP_UTF8), "r");
	TIFFSetErrorHandler(oldHandler);
	TIFFSetErrorHandlerExt(oldHandlerExt);

	if (m_tiff)
	{
		ZTRACE_RUNTIME("Opened %s", (LPCSTR)CT2CA(m_strFileName, CP_UTF8));

		cfa = 0;
		master = 0;
		samplemin = 0;
		samplemax = 1.0;
		exposureTime = 0.0;
		isospeed = 0;
		gain = -1;
		cfatype = CFATYPE_NONE;

		TIFFGetField(m_tiff,TIFFTAG_IMAGEWIDTH, &w);
        TIFFGetField(m_tiff,TIFFTAG_IMAGELENGTH, &h);

		if (!TIFFGetField(m_tiff, TIFFTAG_COMPRESSION, &compression))
			compression = COMPRESSION_NONE;
		if (!TIFFGetField(m_tiff, TIFFTAG_BITSPERSAMPLE, &bps))
			bps = 1;
		if (!TIFFGetField(m_tiff, TIFFTAG_SAMPLESPERPIXEL, &spp))
			spp = 1;
		if (!TIFFGetField(m_tiff, TIFFTAG_PLANARCONFIG, &planarconfig))
			planarconfig = PLANARCONFIG_CONTIG;
		TIFFGetField(m_tiff, TIFFTAG_PHOTOMETRIC, &photo);
		if (!TIFFGetField(m_tiff, TIFFTAG_SAMPLEFORMAT, &sampleformat))
			sampleformat = SAMPLEFORMAT_UINT;
		if (!TIFFGetField(m_tiff, TIFFTAG_DSS_CFA, &cfa))
			cfa = 0;
		if (!TIFFGetField(m_tiff, TIFFTAG_DSS_CFATYPE, &cfatype))
			cfatype = cfa ? CFATYPE_RGGB : CFATYPE_NONE;
		if (!TIFFGetField(m_tiff, TIFFTAG_DSS_MASTER, &master))
			master = 0;

		char *				szMake = nullptr;
		char *				szModel = nullptr;

		if (TIFFGetField(m_tiff, TIFFTAG_MODEL, &szModel))
			strMakeModel = szModel;
		else if (TIFFGetField(m_tiff, TIFFTAG_MAKE, &szMake))
			strMakeModel = szMake;

		if (!TIFFGetField(m_tiff, TIFFTAG_DSS_ISO, &isospeed))
			isospeed = 0;
		if (!TIFFGetField(m_tiff, TIFFTAG_DSS_GAIN, &gain))
			gain = -1;
		if (!TIFFGetField(m_tiff, TIFFTAG_DSS_APERTURE, &aperture))
			aperture = 0.0;
		if (!TIFFGetField(m_tiff, TIFFTAG_DSS_TOTALEXPOSURE, &exposureTime))
		{
			LONG			lExposure;
			if (TIFFGetField(m_tiff, TIFFTAG_DSS_TOTALEXPOSUREOLD, &lExposure))
				exposureTime = lExposure;
		};

		// Check that this is a compatible TIFF format
		// Support is :
		// 8, 16, 32 bits per pixels (with slight restriction on format)
		// No compression or LZW compression or ZIP (deflate) compression
		// sample per pixel 1 (gray levels) or 3 (rgb)
		if ((planarconfig == PLANARCONFIG_CONTIG) &&
			((compression == COMPRESSION_NONE) ||
			 (compression == COMPRESSION_LZW) ||
			 (compression == COMPRESSION_DEFLATE) ||
			 (compression == COMPRESSION_ADOBE_DEFLATE))&&
			((spp == 3) || (spp==4) || (spp == 1)))
		{
			if ((bps == 8) || (bps == 16))
			{
				bResult = (sampleformat == SAMPLEFORMAT_UINT) ||
						  (sampleformat == SAMPLEFORMAT_INT);
			}
			else if (bps == 32)
			{
				bResult = (sampleformat == SAMPLEFORMAT_UINT) ||
						  (sampleformat == SAMPLEFORMAT_INT) ||
						  (sampleformat == SAMPLEFORMAT_IEEEFP);

				if (sampleformat == SAMPLEFORMAT_IEEEFP)
				{
					// Read min/max values
					TIFFGetField(m_tiff, TIFFTAG_SMINSAMPLEVALUE, &samplemin);
					TIFFGetField(m_tiff, TIFFTAG_SMAXSAMPLEVALUE, &samplemax);
				};
			};

			if (bResult)
			{
				if ((spp == 3) || (spp == 4))
					bResult = (photo == PHOTOMETRIC_RGB);
				else if (spp == 1)
					bResult = (photo == PHOTOMETRIC_MINISBLACK);
			};
		};

		// Retrieve the Date/Time as in the TIFF TAG
		char *				szDateTime;

		if (TIFFGetField(m_tiff, TIFFTAG_DATETIME, &szDateTime))
		{
			CString			strDateTime = szDateTime;

			// Decode 2007:11:02 22:07:03
			//        0123456789012345678

			if (strDateTime.GetLength() >= 19)
			{
				m_DateTime.wYear = _ttol(strDateTime.Left(4));
				m_DateTime.wMonth = _ttol(strDateTime.Mid(5, 2));
				m_DateTime.wDay = _ttol(strDateTime.Mid(8, 2));
				m_DateTime.wHour = _ttol(strDateTime.Mid(11, 2));
				m_DateTime.wMinute = _ttol(strDateTime.Mid(14, 2));
				m_DateTime.wSecond = _ttol(strDateTime.Mid(17, 2));
			};
		};



		if (!dwSkipExifInfo)
		{
			// Try to read EXIF data
			uint64				ExifID;

			if (TIFFGetField(m_tiff, TIFFTAG_EXIFIFD, &ExifID))
			{
				//
				// Get current TIFF Directory so we can return to it
				//
				auto currentIFD = TIFFCurrentDirectory(m_tiff);
				if (TIFFReadEXIFDirectory(m_tiff, ExifID))
				{
					if (!TIFFGetField(m_tiff, EXIFTAG_EXPOSURETIME, &exposureTime))
						exposureTime = 0.0;
					if (!TIFFGetField(m_tiff, EXIFTAG_FNUMBER, &aperture))
						aperture = 0.0;
					// EXIFTAG_ISOSPEEDRATINGS is a uint16 according to the EXIF spec
					isospeed = 0;
					uint16	count = 0;
					uint16 * iso_setting = nullptr;
					if (!TIFFGetField(m_tiff, EXIFTAG_ISOSPEEDRATINGS, &count, &iso_setting))
						isospeed = 0;
					else
					{
						isospeed = iso_setting[0];
					}
					// EXIFTAG_GAINCONTROL does not represent a gain value, so ignore it.

					//
					// Revert IFD to status quo ante TIFFReadEXIFDirectory
					//
					TIFFSetDirectory(m_tiff, currentIFD);
				};
			};
		}
		else
		{
			CBitmapInfo			BitmapInfo;

			if (RetrieveEXIFInfo(m_strFileName, BitmapInfo))
			{
				exposureTime = BitmapInfo.m_fExposure;
				aperture	 = BitmapInfo.m_fAperture;
				isospeed	 = BitmapInfo.m_lISOSpeed;
				gain		 = BitmapInfo.m_lGain;
				m_DateTime	 = BitmapInfo.m_DateTime;
			};
		};

		if (bResult)
			bResult = OnOpen();
		if (!bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = nullptr;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CTIFFReader::Read()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

	if (m_tiff) do
	{
		bResult = true;
		tmsize_t		scanLineSize;
		tdata_t			buff = nullptr, curr = nullptr;

		if (m_pProgress)
			m_pProgress->Start2(nullptr, h);

		scanLineSize = TIFFScanlineSize(m_tiff);
		ZTRACE_RUNTIME("TIFF Scan Line Size %zu", scanLineSize);
		ZTRACE_RUNTIME("TIFF spp=%d, bps=%d, w=%d, h=%d", spp, bps, w, h);

		//
		// Allocate enough to hold the entire image
		//
		tmsize_t buffSize = scanLineSize * h;
		ZTRACE_RUNTIME("Allocating buffer of %zu bytes", buffSize);
		curr = buff = (tdata_t)malloc(h * scanLineSize);
		if (nullptr == buff)
		{
			bResult = false;
			break;
		}
		tdata_t limit = (byte*)buff + (1 + (h*scanLineSize));		// One past end of buffer
		//
		// The code used to read scan line by scan line and decode each individually
		// Now try to inhale the whole image in as few calls as possible using
		// TIFFReadEncodedStrip
		//
		auto stripCount = TIFFNumberOfStrips(m_tiff);
		ZTRACE_RUNTIME("Number of strips is %u", stripCount);

		for (int i = 0; i < stripCount; i++)
		{
			auto count = TIFFReadEncodedStrip(m_tiff, i, curr, -1);
			if (-1 == count)
			{
				ZTRACE_RUNTIME("TIFFReadEncodedStrip returned an error");
				bResult = false;
				break;
			}
			curr = static_cast<byte*>(curr) + count;		// Increment current buffer pointer
		}

		if (!bResult) break;

		BYTE *  byteBuff = (BYTE *)buff;
		WORD *	shortBuff = (WORD *)buff;
		DWORD * longBuff = (DWORD *)buff;
		float *	floatBuff = (float *)buff;

		int	rowProgress = 0;

#if defined(_OPENMP)
#pragma omp parallel for default(none)
#endif
		for (long row = 0; row < h; row++)
		{

			for (long col = 0; col < w; col++)
			{
				double fRed = 0;
				double fGreen = 0;
				double fBlue = 0;

				long index = (row * w * spp) + (col * spp);

				switch (bps)	// Bits per sample
				{
				case 8:			// One byte 
					switch (spp)
					{
					case 1:
						fRed	= fGreen = fBlue = byteBuff[index];
						break;
					case 3:
					case 4:
						fRed	= byteBuff[index];
						fGreen	= byteBuff[index + 1];
						fBlue	= byteBuff[index + 2];
						break;
					}
					break;
				case 16:		// Unsigned short == WORD 
					switch (spp)
					{
					case 1:
						fRed	= (double)(shortBuff[index]);
						fRed	= fGreen = fBlue = fRed / 256.0;
						break;
					case 3:
					case 4:
						fRed	= (double)(shortBuff[index]) / 256.0;
						fGreen	= (double)(shortBuff[index + 1]) / 256.0;
						fBlue	= (double)(shortBuff[index + 2]) / 256.0;
						break;
					}
					break;
				case 32:		// Unsigned long or 32 bit floating point 
					if (sampleformat == SAMPLEFORMAT_IEEEFP)
					switch (spp)
					{
					case 1:
						fRed	= (double)(floatBuff[index]);
						fRed	= fGreen = fBlue = (fRed - samplemin) / (samplemax - samplemin) * 256.0;
						break;
					case 3:
					case 4:
						fRed	= (double)(floatBuff[index]);
						fGreen	= (double)(floatBuff[index + 1]);
						fBlue	= (double)(floatBuff[index + 2]);

						fRed	= (fRed - samplemin) / (samplemax - samplemin) * 256.0;
						fGreen	= (fGreen - samplemin) / (samplemax - samplemin) * 256.0;
						fBlue	= (fBlue - samplemin) / (samplemax - samplemin) * 256.0;
						break;
					}
					else switch (spp)	// unsigned long == DWORD
					{
					case 1:
						fRed	= (double)(longBuff[index]);
						fRed	= fGreen = fBlue = fRed / 256.0 / 65536.0;
						break;
					case 3:
					case 4:
						fRed	= (double)(longBuff[index]);
						fGreen	= (double)(longBuff[index + 1]);
						fBlue	= (double)(longBuff[index + 2]);

						fRed	= fRed / 256.0 / 65536.0;
						fGreen  = fGreen / 256.0 / 65536.0;
						fBlue	= fBlue / 256.0 / 65536.0;
						break;

					}
				}

				OnRead(col, row, fRed, fGreen, fBlue);
			};
#if defined (_OPENMP)
			if (m_pProgress && 0 == omp_get_thread_num())	// Are we on the master thread?
			{
				rowProgress += omp_get_num_threads();
				m_pProgress->Progress2(nullptr, rowProgress);
			}
#else
			if (m_pProgress)
				m_pProgress->Progress2(nullptr, ++rowProgress);
#endif

		};
		free(buff);
		if (m_pProgress)
			m_pProgress->End2();
	} while (false);

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CTIFFReader::Close()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = true;
	if (m_tiff)
	{
		bResult = OnClose();
		if (bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = nullptr;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CTIFFWriter::SetFormat(LONG lWidth, LONG lHeight, TIFFFORMAT TiffFormat, CFATYPE CFAType, bool bMaster)
{
	cfatype = CFAType;
	if (CFAType != CFATYPE_NONE)
		cfa = 1;
	else
		cfa = 0;

	if (bMaster)
		master = 1;
	else
		master = 0;

	w = lWidth;
	h = lHeight;
	switch (TiffFormat)
	{
	case TF_8BITRGB	:
		spp = 3;
		bps = 8;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_16BITRGB :
		spp = 3;
		bps = 16;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_32BITRGB :
		spp = 3;
		bps = 32;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_32BITRGBFLOAT :
		spp = 3;
		bps = 32;
		sampleformat = SAMPLEFORMAT_IEEEFP;
		samplemin    = 0;
		samplemax    = 1.0;
		break;
	case TF_8BITGRAY :
		spp = 1;
		bps = 8;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_16BITGRAY :
		spp = 1;
		bps = 16;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_32BITGRAY :
		spp = 1;
		bps = 32;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_32BITGRAYFLOAT :
		spp = 1;
		bps = 32;
		sampleformat = SAMPLEFORMAT_IEEEFP;
		samplemin    = 0;
		samplemax    = 1.0;
		break;
	};
};

/* ------------------------------------------------------------------- */

bool CTIFFWriter::Open()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

	m_tiff = TIFFOpen(CT2CA(m_strFileName, CP_UTF8), "w");
	if (m_tiff)
	{
		photo = PHOTOMETRIC_RGB;

		bResult = OnOpen();
		if (bResult)
		{
			TIFFSetField(m_tiff, TIFFTAG_IMAGEWIDTH, w);
			TIFFSetField(m_tiff, TIFFTAG_IMAGELENGTH, h);
			TIFFSetField(m_tiff, TIFFTAG_COMPRESSION, compression);

			TIFFSetField(m_tiff, TIFFTAG_BITSPERSAMPLE, bps);
			TIFFSetField(m_tiff, TIFFTAG_SAMPLESPERPIXEL, spp);
			TIFFSetField(m_tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			if (spp == 1)
				TIFFSetField(m_tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
			else
				TIFFSetField(m_tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			TIFFSetField(m_tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
			TIFFSetField(m_tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);

			if (IsFloat()) TIFFSetField(m_tiff, TIFFTAG_PREDICTOR, PREDICTOR_FLOATINGPOINT);

			if (samplemax-samplemin)
			{
				TIFFSetField(m_tiff, TIFFTAG_SMINSAMPLEVALUE, samplemin);
				TIFFSetField(m_tiff, TIFFTAG_SMAXSAMPLEVALUE, samplemax);
			};

			//
			// Set Software name in the same way we do elsewhere.
			//
			CStringA			strSoftware = "DeepSkyStacker ";
			strSoftware += VERSION_DEEPSKYSTACKER;

			TIFFSetField(m_tiff, TIFFTAG_SOFTWARE, (LPCSTR)strSoftware);

			if (m_strDescription.GetLength())
			{
				CStringA temp = CT2CA(m_strDescription);
				TIFFSetField(m_tiff, TIFFTAG_IMAGEDESCRIPTION, (LPCSTR)temp);
			}

			if (m_DateTime.wYear)
			{
				// Set the DATETIME TIFF tag
				CStringA		strDateTime;

				strDateTime.Format("%04d:%02d:%02d %02d:%02d:%02d",
								   m_DateTime.wYear, m_DateTime.wMonth, m_DateTime.wDay,
								   m_DateTime.wHour, m_DateTime.wMinute, m_DateTime.wSecond);

				TIFFSetField(m_tiff, TIFFTAG_DATETIME, (LPCSTR)strDateTime);
			};

			/* It is good to set resolutions too (but it is not nesessary) */
			float		xres = 100,
						yres = 100;
			TIFFSetField(m_tiff, TIFFTAG_XRESOLUTION, xres);
			TIFFSetField(m_tiff, TIFFTAG_YRESOLUTION, yres);
			TIFFSetField(m_tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

			if (cfa)
				TIFFSetField(m_tiff, TIFFTAG_DSS_CFA, cfa);
			if (cfa && cfatype)
				TIFFSetField(m_tiff, TIFFTAG_DSS_CFATYPE, cfatype);

			if (master)
				TIFFSetField(m_tiff, TIFFTAG_DSS_MASTER, master);

			if (isospeed)
				TIFFSetField(m_tiff, TIFFTAG_DSS_ISO, isospeed);

			if (gain >= 0)
				TIFFSetField(m_tiff, TIFFTAG_DSS_GAIN, gain);

			if (exposureTime)
				TIFFSetField(m_tiff, TIFFTAG_DSS_TOTALEXPOSURE, exposureTime);

			if (aperture)
				TIFFSetField(m_tiff, TIFFTAG_DSS_APERTURE, aperture);

			if (nrframes)
				TIFFSetField(m_tiff, TIFFTAG_DSS_NRFRAMES, nrframes);

            TIFFSetField(m_tiff, TIFFTAG_ZIPQUALITY, Z_BEST_SPEED); // TODO: make it configurable?
		}
		else
		{
			TIFFClose(m_tiff);
			m_tiff = nullptr;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CTIFFWriter::Write()
{
	ZFUNCTRACE_RUNTIME();
	bool		bResult = false;
	bool		bError = false;


	if (m_tiff)
	{
		tmsize_t		scanLineSize;
		tdata_t			buff;

		scanLineSize = TIFFScanlineSize(m_tiff);
		ZTRACE_RUNTIME("TIFF Scan Line Size %zu", scanLineSize);
		ZTRACE_RUNTIME("TIFF spp=%d, bps=%d, w=%d, h=%d", spp, bps, w, h);

		//
        // Allocate enough to hold the entire image
        //
		tmsize_t buffSize = scanLineSize * h;
		ZTRACE_RUNTIME("Allocating buffer of %zu bytes", buffSize);
		buff = (tdata_t)malloc(buffSize);

		if (buff)
		{
			if (m_pProgress)
				m_pProgress->Start2(nullptr, h);

			BYTE *  byteBuff = (BYTE *)buff;
			WORD *	shortBuff = (WORD *)buff;
			DWORD * longBuff = (DWORD *)buff;
			float *	floatBuff = (float *)buff;

			int	rowProgress = 0;

#if defined(_OPENMP)
#pragma omp parallel for default(none)
#endif
			for (LONG row = 0; row < h; row++)
			{
				for (LONG col = 0; col < w; col++)
				{
					long index = (row * w * spp) + (col * spp);

					double		fRed = 0, fGreen = 0, fBlue = 0, fGrey = 0;

					OnWrite(col, row, fRed, fGreen, fBlue);

					//
					// If its a cfa bitmap, set grey level to maximum of RGB
					// else convert from RGB to HSL and use Luminance.
					// 
					if (cfa)
					{
						fGrey = max(fRed, max(fGreen, fBlue));
					}
					else
					{
						double H, S, L;
						ToHSL(fRed, fGreen, fBlue, H, S, L);
						fGrey = L * 255.0;
					}

					switch (bps)	// Bits per sample
					{
					case 8:			// One byte 
						switch (spp)
						{
						case 1:
							byteBuff[index] = fGrey;
							break;
						case 3:
						case 4:
							byteBuff[index] = fRed;
							byteBuff[index + 1] = fGreen;
							byteBuff[index + 2] = fBlue;
							break;
						}
						break;
					case 16:		// Unsigned short == WORD 
						switch (spp)
						{
						case 1:
							shortBuff[index] = fGrey * 256.0;
							break;
						case 3:
						case 4:
							shortBuff[index] = fRed * 256.0;
							shortBuff[index + 1] = fGreen * 256.0;
							shortBuff[index + 2] = fBlue * 256.0;
							break;
						}
						break;
					case 32:		// Unsigned long or 32 bit floating point 
						if (sampleformat == SAMPLEFORMAT_IEEEFP)
							switch (spp)
							{
							case 1:
								floatBuff[index] = fGrey / 256.0 * (samplemax - samplemin) + samplemin;
								break;
							case 3:
							case 4:
								floatBuff[index] = fRed / 256.0 * (samplemax - samplemin) + samplemin;
								floatBuff[index + 1] = fGreen / 256.0 * (samplemax - samplemin) + samplemin;
								floatBuff[index + 2] = fBlue / 256.0 * (samplemax - samplemin) + samplemin;
								break;
							}
						else switch (spp)	// unsigned long == DWORD
						{
						case 1:
							longBuff[index] = fGrey * 256.0 * 65536.0;
							break;
						case 3:
						case 4:
							longBuff[index] = fRed * 256.0 * 65536.0;
							longBuff[index + 1] = fGreen * 256.0 * 65536.0;
							longBuff[index + 2] = fBlue * 256.0 * 65536.0;
							break;

						}
					}

				}
#if defined (_OPENMP)
				if (m_pProgress && 0 == omp_get_thread_num())	// Are we on the master thread?
				{
					rowProgress += omp_get_num_threads();
					m_pProgress->Progress2(nullptr, rowProgress);
				}
#else
				if (m_pProgress)
					m_pProgress->Progress2(nullptr, ++rowProgress);
#endif
			};

			//
			// Write the image out as Strips (i.e. not scanline by scanline)
			// 
			const unsigned long STRIP_SIZE_DEFAULT = 262144UL;		// 256kB

			//
			// Work out how many scanlines fit into 256K
			//
			unsigned long rowsPerStrip = STRIP_SIZE_DEFAULT / scanLineSize;
			//
			// Handle the case where the scanline is longer the 256kB
			//
			if (0 == rowsPerStrip) rowsPerStrip = 1; 
			TIFFSetField(m_tiff, TIFFTAG_ROWSPERSTRIP, rowsPerStrip);

			//
			// From that we derive the number of strips
			//
			long numStrips = h / rowsPerStrip;
			//
			// If it wasn't an exact division (IOW there's a remainder), add one
			// for the final (short) strip.
			//
			if (0 != h % rowsPerStrip)
				++numStrips;

			ZTRACE_RUNTIME("Number of strips is %u", numStrips);

			BYTE * curr = (BYTE *)buff;
			tsize_t stripSize = rowsPerStrip * scanLineSize;
			tsize_t bytesRemaining = h * scanLineSize;
			tsize_t size = stripSize;
			for (long strip = 0; strip < numStrips; strip++)
			{
				if (bytesRemaining < stripSize)
					size = bytesRemaining;

				tsize_t result = TIFFWriteEncodedStrip(m_tiff, strip, curr, size);
				if (-1 == result)
				{
					ZTRACE_RUNTIME("TIFFWriteEncodedStrip() failed");
					bError = true;
					break;
				}
				curr += result;
				bytesRemaining -= result;
			}

			free(buff);
			if (m_pProgress)
				m_pProgress->End2();
		};
		bResult = (!bError) ? true : false;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CTIFFWriter::Close()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = true;

	if (m_tiff)
	{
		bResult = OnClose();
		if (bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = nullptr;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CTIFFWriteFromMemoryBitmap : public CTIFFWriter
{
private :
	CMemoryBitmap *			m_pMemoryBitmap;

private :
	TIFFFORMAT	GetBestTiffFormat(CMemoryBitmap * pBitmap);

public :
	CTIFFWriteFromMemoryBitmap(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress) :
	   CTIFFWriter(szFileName, pProgress)
	{
		m_pMemoryBitmap = pBitmap;
	};

	virtual ~CTIFFWriteFromMemoryBitmap()
	{
		Close();
	};

	virtual bool Close() { return OnClose(); };

	virtual bool	OnOpen();
	void	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue) override;
	virtual bool	OnClose();
};

/* ------------------------------------------------------------------- */

TIFFFORMAT	CTIFFWriteFromMemoryBitmap::GetBestTiffFormat(CMemoryBitmap * pBitmap)
{
	TIFFFORMAT						Result = TF_UNKNOWN;
	C24BitColorBitmap *			p24Color = dynamic_cast<C24BitColorBitmap *>(pBitmap);
	C48BitColorBitmap *			p48Color = dynamic_cast<C48BitColorBitmap *>(pBitmap);
	C96BitColorBitmap *			p96Color = dynamic_cast<C96BitColorBitmap *>(pBitmap);
	C96BitFloatColorBitmap*		p96FloatColor = dynamic_cast<C96BitFloatColorBitmap*>(pBitmap);
	C8BitGrayBitmap*				p8Gray	= dynamic_cast<C8BitGrayBitmap*>(pBitmap);
	C16BitGrayBitmap *				p16Gray = dynamic_cast<C16BitGrayBitmap *>(pBitmap);
	C32BitGrayBitmap *				p32Gray = dynamic_cast<C32BitGrayBitmap *>(pBitmap);
	C32BitFloatGrayBitmap *			p32FloatGray = dynamic_cast<C32BitFloatGrayBitmap *>(pBitmap);

	if (p24Color)
		Result = TF_8BITRGB;
	else if (p48Color)
		Result = TF_16BITRGB;
	else if (p96Color)
		Result = TF_32BITRGB;
	else if (p96FloatColor)
		Result = TF_32BITRGBFLOAT;
	else if (p8Gray)
		Result = TF_8BITGRAY;
	else if (p16Gray)
		Result = TF_16BITGRAY;
	else if (p32Gray)
		Result = TF_32BITGRAY;
	else if (p32FloatGray)
		Result = TF_32BITGRAYFLOAT;

	return Result;
};

/* ------------------------------------------------------------------- */

bool CTIFFWriteFromMemoryBitmap::OnOpen()
{
	bool			bResult = true;
	LONG			lWidth,
					lHeight;
	CFATYPE			CFAType = CFATYPE_NONE;
	bool			bMaster;

	lWidth  = m_pMemoryBitmap->Width();
	lHeight = m_pMemoryBitmap->Height();
	if (::IsCFA(m_pMemoryBitmap))
		CFAType = ::GetCFAType(m_pMemoryBitmap);
	bMaster = m_pMemoryBitmap->IsMaster();

	if (m_Format == TF_UNKNOWN)
		m_Format = GetBestTiffFormat(m_pMemoryBitmap);

	if (m_Format != TF_UNKNOWN)
	{
		SetFormat(lWidth, lHeight, m_Format, CFAType, bMaster);
		if (!isospeed)
			isospeed = m_pMemoryBitmap->GetISOSpeed();
		if (gain < 0)
			gain = m_pMemoryBitmap->GetGain();
		if (!exposureTime)
			exposureTime = m_pMemoryBitmap->GetExposure();
		if (!aperture)
			aperture = m_pMemoryBitmap->GetAperture();
		if (!nrframes)
			nrframes = m_pMemoryBitmap->GetNrFrames();
		m_DateTime = m_pMemoryBitmap->m_DateTime;
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CTIFFWriteFromMemoryBitmap::OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue)
{

	try
	{
		if (m_pMemoryBitmap)
		{
			if (spp == 1)
			{
				m_pMemoryBitmap->GetPixel(lX, lY, fRed);
				fGreen = fBlue = fRed;
			}
			else
				m_pMemoryBitmap->GetPixel(lX, lY, fRed, fGreen, fBlue);
		};
	}
	catch (ZException e)
	{
		CString errorMessage;
		CString name(CA2CT(e.name()));
		CString fileName(CA2CT(e.locationAtIndex(0)->fileName()));
		CString functionName(CA2CT(e.locationAtIndex(0)->functionName()));
		CString text(CA2CT(e.text(0)));

		errorMessage.Format(
			_T("Exception %s thrown from %s Function: %s() Line: %lu\n\n%s"),
			name,
			fileName,
			functionName,
			e.locationAtIndex(0)->lineNumber(),
			text);
#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
		exit(1);

	}

	return;
};

/* ------------------------------------------------------------------- */

bool CTIFFWriteFromMemoryBitmap::OnClose()
{
	bool			bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

bool	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure, double fAperture)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;

	if (pBitmap)
	{
		CTIFFWriteFromMemoryBitmap	tiff(szFileName, pBitmap, pProgress);

		if (szDescription)
			tiff.SetDescription(szDescription);
		if (lISOSpeed)
			tiff.SetISOSpeed(lISOSpeed);
		else
			tiff.SetISOSpeed(pBitmap->GetISOSpeed());
		if (lGain >= 0)
			tiff.SetGain(lGain);
		else
			tiff.SetGain(pBitmap->GetGain());
		if (fExposure)
			tiff.SetExposureTime(fExposure);
		else
			tiff.SetExposureTime(pBitmap->GetExposure());
		if (fAperture)
			tiff.SetAperture(fAperture);
		else
			tiff.SetAperture(pBitmap->GetAperture());
		tiff.SetNrFrames(pBitmap->GetNrFrames());
		if (tiff.Open())
		{
			bResult = tiff.Write();
			tiff.Close();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription)
{
	return WriteTIFF(szFileName, pBitmap, pProgress, szDescription,
			/*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0, /*fAperture*/ 0.0);
};

/* ------------------------------------------------------------------- */

bool	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure, double fAperture)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;

	if (pBitmap)
	{
		CTIFFWriteFromMemoryBitmap	tiff(szFileName, pBitmap, pProgress);

		if (szDescription)
			tiff.SetDescription(szDescription);
		if (lISOSpeed)
			tiff.SetISOSpeed(lISOSpeed);
		if (lGain >= 0)
			tiff.SetGain(lGain);
		if (fExposure)
			tiff.SetExposureTime(fExposure);
		if (fAperture)
			tiff.SetAperture(fAperture);
		tiff.SetFormatAndCompression(TIFFFormat, TIFFCompression);

		if (tiff.Open())
		{
			bResult = tiff.Write();
			tiff.Close();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, LPCTSTR szDescription)
{
	return WriteTIFF(szFileName, pBitmap, pProgress, TIFFFormat, TIFFCompression, szDescription,
			/*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0, /*fAperture*/ 0.0);
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CTIFFReadInMemoryBitmap : public CTIFFReader
{
private :
	CMemoryBitmap **			m_ppBitmap;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;

public :
	CTIFFReadInMemoryBitmap(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress *	pProgress)
		: CTIFFReader(szFileName, pProgress)
	{
		m_ppBitmap = ppBitmap;
	};

	virtual ~CTIFFReadInMemoryBitmap() { Close(); };

	virtual bool Close() { return OnClose(); };

	virtual bool	OnOpen();
	void	OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue) override;
	virtual bool	OnClose();
};

/* ------------------------------------------------------------------- */

bool CTIFFReadInMemoryBitmap::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

	if (spp == 1)
	{
		if (bps == 8)
		{
			m_pBitmap.Attach(new C8BitGrayBitmap());
			ZTRACE_RUNTIME("Creating 8 Gray bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (bps == 16)
		{
			m_pBitmap.Attach(new C16BitGrayBitmap());
			ZTRACE_RUNTIME("Creating 16 Gray bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (bps == 32)
		{
			if (sampleformat == SAMPLEFORMAT_IEEEFP)
			{
				m_pBitmap.Attach(new C32BitFloatGrayBitmap());
				ZTRACE_RUNTIME("Creating 32 float Gray bit memory bitmap %p", m_pBitmap.m_p);
			}
			else
			{
				m_pBitmap.Attach(new C32BitGrayBitmap());
				ZTRACE_RUNTIME("Creating 32 Gray bit memory bitmap %p", m_pBitmap.m_p);
			};
		};
	}
	else if ((spp == 3) || (spp == 4))
	{
		if (bps == 8)
		{
			m_pBitmap.Attach(new C24BitColorBitmap());
			ZTRACE_RUNTIME("Creating 8 RGB bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (bps == 16)
		{
			m_pBitmap.Attach(new C48BitColorBitmap());
			ZTRACE_RUNTIME("Creating 16 RGB bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (bps == 32)
		{
			if (sampleformat == SAMPLEFORMAT_IEEEFP)
			{
				m_pBitmap.Attach(new C96BitFloatColorBitmap());
				ZTRACE_RUNTIME("Creating 32 float RGB bit memory bitmap %p", m_pBitmap.m_p);
			}
			else
			{
				m_pBitmap.Attach(new C96BitColorBitmap());
				ZTRACE_RUNTIME("Creating 32 RGB bit memory bitmap %p", m_pBitmap.m_p);
			};
		};
	};

	if (m_pBitmap)
	{
		bResult = m_pBitmap->Init(w, h);
		m_pBitmap->SetCFA(cfa);
		if (cfatype)
		{
			C16BitGrayBitmap *		pGray16Bitmap = dynamic_cast<C16BitGrayBitmap *>(m_pBitmap.m_p);

			if (pGray16Bitmap)
				pGray16Bitmap->SetCFAType((CFATYPE)cfatype);
		};
		m_pBitmap->SetMaster(master);
		m_pBitmap->SetISOSpeed(isospeed);
		m_pBitmap->SetGain(gain);
		m_pBitmap->SetExposure(exposureTime);
		m_pBitmap->SetAperture(aperture);
		m_pBitmap->m_DateTime = m_DateTime;

		CString		strDescription;
		if (strMakeModel.GetLength())
			strDescription.Format(_T("TIFF (%s)"), (LPCTSTR)strMakeModel);
		else
			strDescription	= "TIFF";
		m_pBitmap->SetDescription(strDescription);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CTIFFReadInMemoryBitmap::OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue)
{

	try
	{
		if (m_pBitmap)
		{
			m_pBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);
		}
	}
	catch (ZException e)
	{
		CString errorMessage;
		CString name(CA2CT(e.name()));
		CString fileName(CA2CT(e.locationAtIndex(0)->fileName()));
		CString functionName(CA2CT(e.locationAtIndex(0)->functionName()));
		CString text(CA2CT(e.text(0)));

		errorMessage.Format(
			_T("Exception %s thrown from %s Function: %s() Line: %lu\n\n%s"),
			name,
			fileName,
			functionName,
			e.locationAtIndex(0)->lineNumber(),
			text);
#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
		exit(1);

	}

	return;
};

/* ------------------------------------------------------------------- */

bool CTIFFReadInMemoryBitmap::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

	if (m_pBitmap)
	{
		bResult = true;
		m_pBitmap.CopyTo(m_ppBitmap);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	ReadTIFF(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress *	pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool					bResult = false;
	CTIFFReadInMemoryBitmap	tiff(szFileName, ppBitmap, pProgress);

	if (ppBitmap)
	{
		bResult = tiff.Open();
		if (bResult)
			bResult = tiff.Read();
		//if (bResult)
		//	bResult = tiff.Close();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	GetTIFFInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool					bResult = false;
	CTIFFReader				tiff(szFileName, nullptr);

	if (tiff.Open())
	{
		BitmapInfo.m_strFileName	= szFileName;
		CString				strMakeModel;

		tiff.GetMakeModel(strMakeModel);

		if (strMakeModel.GetLength())
			BitmapInfo.m_strFileType.Format(_T("TIFF (%s)"), (LPCTSTR)strMakeModel);
		else
			BitmapInfo.m_strFileType	= _T("TIFF");
		BitmapInfo.m_lWidth			= tiff.Width();
		BitmapInfo.m_lHeight		= tiff.Height();
		BitmapInfo.m_lBitPerChannel = tiff.BitPerChannels();
		BitmapInfo.m_lNrChannels	= tiff.NrChannels();
		BitmapInfo.m_bFloat			= tiff.IsFloat();
		BitmapInfo.m_CFAType		= tiff.GetCFAType();
		BitmapInfo.m_bMaster		= tiff.IsMaster();
		BitmapInfo.m_bCanLoad		= true;
		BitmapInfo.m_lISOSpeed		= tiff.GetISOSpeed();
		BitmapInfo.m_lGain		= tiff.GetGain();
		BitmapInfo.m_fExposure		= tiff.GetExposureTime();
		BitmapInfo.m_fAperture		= tiff.GetAperture();
		BitmapInfo.m_DateTime		= tiff.GetDateTime();
		bResult = true; //tiff.Close();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	IsTIFFPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	return GetTIFFInfo(szFileName, BitmapInfo);
};

/* ------------------------------------------------------------------- */

int	LoadTIFFPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	int		result = -1;		// -1 means not a TIFF file.

	if (GetTIFFInfo(szFileName, BitmapInfo) && BitmapInfo.CanLoad())
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;

		if (ReadTIFF(szFileName, &pBitmap, pProgress))
		{
			if (BitmapInfo.IsCFA() && (IsSuperPixels() || IsRawBayer() || IsRawBilinear() || IsRawAHD()))
			{
				CCFABitmapInfo *	pGrayBitmap;

				pGrayBitmap = dynamic_cast<CCFABitmapInfo *>(pBitmap.m_p);

				if (pGrayBitmap)
				{
					if (IsSuperPixels())
						pGrayBitmap->UseSuperPixels(true);
					else if (IsRawBayer())
						pGrayBitmap->UseRawBayer(true);
					else  if (IsRawBilinear())
						pGrayBitmap->UseBilinear(true);
					else if (IsRawAHD())
						pGrayBitmap->UseAHD(true);
				};
			};
			pBitmap.CopyTo(ppBitmap);
			result = 0;
		}
		else
		{
			result = 1;		// Failed to load file
		}
	};

	return result;
};

/* ------------------------------------------------------------------- */
