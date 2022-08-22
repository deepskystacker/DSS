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
      false,	false,	const_cast<char *>("DSSNumberOfFrames") },
    { TIFFTAG_DSS_TOTALEXPOSUREOLD, 1, 1, TIFF_LONG, FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSTotalExposureOld") },
    { TIFFTAG_DSS_TOTALEXPOSURE, 1, 1, TIFF_FLOAT, FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSTotalExposure") },
    { TIFFTAG_DSS_ISO,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSISO") },
    { TIFFTAG_DSS_GAIN,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSGain") },
    { TIFFTAG_DSS_SETTINGSAPPLIED,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSSettingsApplied") },
    { TIFFTAG_DSS_BEZIERSETTINGS,	-1,-1, TIFF_ASCII, FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSBezierSettings") },
    { TIFFTAG_DSS_ADJUSTSETTINGS,	-1,-1, TIFF_ASCII, FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSAdjustSettings") },
    { TIFFTAG_DSS_CFA,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSCFA") },
    { TIFFTAG_DSS_MASTER,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSMaster") },
    { TIFFTAG_DSS_CFATYPE,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      false,	false,	const_cast<char *>("DSSCFATYPE") },
	{ TIFFTAG_DSS_APERTURE, 1, 1, TIFF_FLOAT, FIELD_CUSTOM,
	  false,	false,	const_cast<char *>("DSSAperture") },

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

	const auto dwSkipExifInfo = settings.value("SkipTIFFExifInfo", uint(0)).toUInt();

	//
	// Quietly attempt to open the putative TIFF file 
	//
	TIFFErrorHandler	oldHandler = TIFFSetErrorHandler(nullptr);
	TIFFErrorHandlerExt	oldHandlerExt = TIFFSetErrorHandlerExt(nullptr);
	m_tiff = TIFFOpen(CT2CA(m_strFileName, CP_ACP), "r");
	TIFFSetErrorHandler(oldHandler);
	TIFFSetErrorHandlerExt(oldHandlerExt);

	if (m_tiff)
	{
		ZTRACE_RUNTIME("Opened %s", (LPCSTR)CT2CA(m_strFileName, CP_ACP));

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
			int			lExposure;
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
	constexpr double scaleFactorInt16 = double{ 1 + UCHAR_MAX };
	constexpr double scaleFactorInt32 = scaleFactorInt16 * (1 + USHORT_MAX);

	ZFUNCTRACE_RUNTIME();

	if (!m_tiff)
		return false;

	try
	{
		tmsize_t		scanLineSize;

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
		auto buffer = std::make_unique<unsigned char[]>(buffSize);
		//
		// The code used to read scan line by scan line and decode each individually
		// Now try to inhale the whole image in as few calls as possible using
		// TIFFReadEncodedStrip
		//
		auto stripCount = TIFFNumberOfStrips(m_tiff);
		ZTRACE_RUNTIME("Number of strips is %u", stripCount);

		unsigned char* curr = buffer.get();
		for (unsigned int i = 0; i < stripCount; i++)
		{
			const auto count = TIFFReadEncodedStrip(m_tiff, i, curr, -1);
			if (-1 == count)
			{
				ZTRACE_RUNTIME("TIFFReadEncodedStrip returned an error");
				return false;
			}
			curr += count; // Increment current buffer pointer
			if (m_pProgress != nullptr)
				m_pProgress->Progress2(nullptr, (this->h / 2 * i) / stripCount);
		}

		std::uint8_t* byteBuff = buffer.get();
		std::uint16_t* shortBuff = reinterpret_cast<std::uint16_t*>(byteBuff);
		std::uint32_t* u32Buff = reinterpret_cast<std::uint32_t*>(byteBuff);
		float* floatBuff = reinterpret_cast<float*>(byteBuff);

		const auto normalizeFloatValue = [sampleMin = this->samplemin, sampleMax = this->samplemax](const float value) -> double
		{
			constexpr double scaleFactor = double{ USHORT_MAX } / 256.0;
			const double normalizationFactor = scaleFactor / (sampleMax - sampleMin);
			return (static_cast<double>(value) - sampleMin) * normalizationFactor;
		};

		const auto loopOverPixels = [height = this->h, width = this->w, progress = this->m_pProgress](const auto& function) -> void
		{
			int progressCounter = 0;
#pragma omp parallel for default(none) schedule(dynamic, 10) if(CMultitask::GetNrProcessors() > 1) // GetNrProcessors() returns 1, if user selected single-thread.
			for (int row = 0; row < height; ++row)
			{
				for (int col = 0; col < width; ++col)
					function(col, row);
				if (progress != nullptr && omp_get_thread_num() == 0 && (progressCounter++ % 25) == 0)
					progress->Progress2(nullptr, (height + row) / 2);
			}
		};

		if (sampleformat == SAMPLEFORMAT_IEEEFP)
		{
			assert(bps == 32);

			if (spp == 1)
				loopOverPixels([&](const int x, const int y) {
					const double gray = normalizeFloatValue(floatBuff[y * w + x]);
					OnRead(x, y, gray, gray, gray);
				});
			else
				loopOverPixels([&](const int x, const int y) {
					const int index = (y * w + x) * spp;
					const double red = normalizeFloatValue(floatBuff[index]);
					const double green = normalizeFloatValue(floatBuff[index + 1]);
					const double blue = normalizeFloatValue(floatBuff[index + 2]);
					OnRead(x, y, red, green, blue);
				});
		}
		else
		{
			if (spp == 1)
				switch (bps)
				{
				case 8: loopOverPixels([&](const int x, const int y) {
					const double fGray = byteBuff[y * w + x];
					OnRead(x, y, fGray, fGray, fGray);
				}); break;
				case 16: loopOverPixels([&](const int x, const int y) {
					const double fGray = shortBuff[y * w + x] / scaleFactorInt16;
					OnRead(x, y, fGray, fGray, fGray);
				}); break;
				case 32: loopOverPixels([&](const int x, const int y) {
					const double fGray = u32Buff[y * w + x] / scaleFactorInt32;
					OnRead(x, y, fGray, fGray, fGray);
				}); break;
				}
			else
				switch (bps)
				{
				case 8: loopOverPixels([&](const int x, const int y) {
					const int index = (y * w + x) * spp;
					const double fRed = byteBuff[index];
					const double fGreen = byteBuff[index + 1];
					const double fBlue = byteBuff[index + 2];
					OnRead(x, y, fRed, fGreen, fBlue);
				}); break;
				case 16: loopOverPixels([&](const int x, const int y) {
					const int index = (y * w + x) * spp;
					const double fRed = shortBuff[index] / scaleFactorInt16;
					const double fGreen = shortBuff[index + 1] / scaleFactorInt16;
					const double fBlue = shortBuff[index + 2] / scaleFactorInt16;
					OnRead(x, y, fRed, fGreen, fBlue);
				}); break;
				case 32: loopOverPixels([&](const int x, const int y) {
					const int index = (y * w + x) * spp;
					const double fRed = u32Buff[index] / scaleFactorInt32;
					const double fGreen = u32Buff[index + 1] / scaleFactorInt32;
					const double fBlue = u32Buff[index + 2] / scaleFactorInt32;
					OnRead(x, y, fRed, fGreen, fBlue);
				}); break;
				}
		}

		if (m_pProgress)
			m_pProgress->End2();
	}
	catch (...)
	{
		return false;
	}

	return true;
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

void CTIFFWriter::SetFormat(int lWidth, int lHeight, TIFFFORMAT TiffFormat, CFATYPE CFAType, bool bMaster)
{
	cfatype = CFAType;
	cfa = CFAType == CFATYPE_NONE ? 0 : 1;

	master = bMaster ? 1 : 0;

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

	m_tiff = TIFFOpen(CT2CA(m_strFileName, CP_ACP), "w");
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

			//
			// Following line commented out as enabling this broke Photoshop (16 August 2020)
			//
			//if (IsFloat()) TIFFSetField(m_tiff, TIFFTAG_PREDICTOR, PREDICTOR_FLOATINGPOINT);

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
			TIFFSetField(m_tiff, TIFFTAG_XRESOLUTION, 100.0f);
			TIFFSetField(m_tiff, TIFFTAG_YRESOLUTION, 100.0f);
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

	//
    // Multipliers of 256.0 and 65536.0 were not correct and resulted in a fully saturated
	// pixel being written with a value of zero because the value overflowed the data type 
	// which was being stored.   Change the code to use UCHAR_MAX and USHRT_MAX
	//

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
			std::uint32_t* u32Buff = (std::uint32_t*)buff;
			float *	floatBuff = (float *)buff;

			int	rowProgress = 0;

#if defined(_OPENMP)
#pragma omp parallel for default(none)
#endif
			for (int row = 0; row < h; row++)
			{
				for (int col = 0; col < w; col++)
				{
					int index = (row * w * spp) + (col * spp);

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
							shortBuff[index] = fGrey * UCHAR_MAX;
							break;
						case 3:
						case 4:
							shortBuff[index] = fRed * UCHAR_MAX;
							shortBuff[index + 1] = fGreen * UCHAR_MAX;
							shortBuff[index + 2] = fBlue * UCHAR_MAX;
							break;
						}
						break;
					case 32:		// Unsigned int or 32 bit floating point 
						if (sampleformat == SAMPLEFORMAT_IEEEFP)
							switch (spp)
							{
							case 1:
								floatBuff[index] = fGrey / (1.0 + UCHAR_MAX) * (samplemax - samplemin) + samplemin;
								break;
							case 3:
							case 4:
								floatBuff[index] = fRed / (1.0 + UCHAR_MAX) * (samplemax - samplemin) + samplemin;
								floatBuff[index + 1] = fGreen / (1.0 + UCHAR_MAX) * (samplemax - samplemin) + samplemin;
								floatBuff[index + 2] = fBlue / (1.0 + UCHAR_MAX) * (samplemax - samplemin) + samplemin;
								break;
							}
						else switch (spp)	// unsigned int == DWORD
						{
						case 1:
							u32Buff[index] = fGrey * UCHAR_MAX * USHRT_MAX;
							break;
						case 3:
						case 4:
							u32Buff[index] = fRed * UCHAR_MAX * USHRT_MAX;
							u32Buff[index + 1] = fGreen * UCHAR_MAX * USHRT_MAX;
							u32Buff[index + 2] = fBlue * UCHAR_MAX * USHRT_MAX;
							break;

						}
					}

				}
#if defined (_OPENMP)
				if (m_pProgress && 0 == omp_get_thread_num())	// Are we on the master thread?
					m_pProgress->Progress2(nullptr, row / 2);
#else
				if (m_pProgress)
					m_pProgress->Progress2(nullptr, row / 2);
#endif
			};

			//
			// Write the image out as Strips (i.e. not scanline by scanline)
			// 
			const unsigned int STRIP_SIZE_DEFAULT = 4'194'304UL;		// 4MB

			//
			// Work out how many scanlines fit into the default strip
			//
			unsigned int rowsPerStrip = STRIP_SIZE_DEFAULT / scanLineSize;
			
			//
			// Handle the case where the scanline is longer the default strip size
			//
			// if (0 == rowsPerStrip) rowsPerStrip = 1; 
			TIFFSetField(m_tiff, TIFFTAG_ROWSPERSTRIP, rowsPerStrip);

			//
			// From that we derive the number of strips
			//
			int numStrips = h / rowsPerStrip;
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
			for (int strip = 0; strip < numStrips; strip++)
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

				if (m_pProgress != nullptr)
					m_pProgress->Progress2(nullptr, h / 2 + (h * strip) / (2 * numStrips));
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
	CMemoryBitmap* m_pMemoryBitmap;

private :
	TIFFFORMAT GetBestTiffFormat(const CMemoryBitmap* pBitmap);

public :
	CTIFFWriteFromMemoryBitmap(LPCTSTR szFileName, CMemoryBitmap* pBitmap, CDSSProgress* pProgress) :
		CTIFFWriter(szFileName, pProgress),
		m_pMemoryBitmap{ pBitmap }
	{}

	virtual ~CTIFFWriteFromMemoryBitmap()
	{
		Close();
	}

	virtual bool Close() { return OnClose(); }

	virtual bool OnOpen();
	void OnWrite(int lX, int lY, double & fRed, double & fGreen, double & fBlue) override;
	virtual bool OnClose();
};

/* ------------------------------------------------------------------- */

TIFFFORMAT CTIFFWriteFromMemoryBitmap::GetBestTiffFormat(const CMemoryBitmap* pBitmap)
{
	if (dynamic_cast<const C24BitColorBitmap*>(pBitmap) != nullptr)
		return TF_8BITRGB;
	else if (dynamic_cast<const C48BitColorBitmap*>(pBitmap) != nullptr)
		return TF_16BITRGB;
	else if (dynamic_cast<const C96BitColorBitmap*>(pBitmap) != nullptr)
		return TF_32BITRGB;
	else if (dynamic_cast<const C96BitFloatColorBitmap*>(pBitmap) != nullptr)
		return TF_32BITRGBFLOAT;
	else if (dynamic_cast<const C8BitGrayBitmap*>(pBitmap) != nullptr)
		return TF_8BITGRAY;
	else if (dynamic_cast<const C16BitGrayBitmap*>(pBitmap) != nullptr)
		return TF_16BITGRAY;
	else if (dynamic_cast<const C32BitGrayBitmap*>(pBitmap) != nullptr)
		return TF_32BITGRAY;
	else if (dynamic_cast<const C32BitFloatGrayBitmap*>(pBitmap) != nullptr)
		return TF_32BITGRAYFLOAT;
	return TF_UNKNOWN;
};

/* ------------------------------------------------------------------- */

bool CTIFFWriteFromMemoryBitmap::OnOpen()
{
	bool			bResult = true;
	int			lWidth,
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

void CTIFFWriteFromMemoryBitmap::OnWrite(int lX, int lY, double & fRed, double & fGreen, double & fBlue)
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
	return true;
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

bool WriteTIFF(LPCTSTR szFileName, CMemoryBitmap* pBitmap, CDSSProgress* pProgress, LPCTSTR szDescription, int lISOSpeed, int lGain, double fExposure, double fAperture)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap != nullptr)
	{
		CTIFFWriteFromMemoryBitmap tiff{ szFileName, pBitmap, pProgress };

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

bool WriteTIFF(LPCTSTR szFileName, CMemoryBitmap* pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription)
{
	return WriteTIFF(szFileName, pBitmap, pProgress, szDescription, /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0, /*fAperture*/ 0.0);
}

/* ------------------------------------------------------------------- */

bool WriteTIFF(LPCTSTR szFileName, CMemoryBitmap* pBitmap, CDSSProgress* pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression,
	LPCTSTR szDescription, int lISOSpeed, int lGain, double fExposure, double fAperture)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap)
	{
		CTIFFWriteFromMemoryBitmap tiff(szFileName, pBitmap, pProgress);

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

bool WriteTIFF(LPCTSTR szFileName, CMemoryBitmap* pBitmap, CDSSProgress* pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, LPCTSTR szDescription)
{
	return WriteTIFF(szFileName, pBitmap, pProgress, TIFFFormat, TIFFCompression, szDescription, /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0, /*fAperture*/ 0.0);
}

/* ------------------------------------------------------------------- */

class CTIFFReadInMemoryBitmap : public CTIFFReader
{
private :
	std::shared_ptr<CMemoryBitmap>& m_outBitmap;
	std::shared_ptr<CMemoryBitmap> m_pBitmap;

public :
	CTIFFReadInMemoryBitmap(LPCTSTR szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, CDSSProgress* pProgress):
		CTIFFReader(szFileName, pProgress),
		m_outBitmap{ rpBitmap }
	{}

	virtual ~CTIFFReadInMemoryBitmap() { Close(); };

	virtual bool Close() { return OnClose(); };

	virtual bool OnOpen() override;
	virtual void OnRead(int lX, int lY, double fRed, double fGreen, double fBlue) override;
	virtual bool OnClose() override;
};

bool CTIFFReadInMemoryBitmap::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (spp == 1)
	{
		if (bps == 8)
		{
			m_pBitmap = std::make_shared<C8BitGrayBitmap>();
			ZTRACE_RUNTIME("Creating 8 Gray bit memory bitmap %p", m_pBitmap.get());
		}
		else if (bps == 16)
		{
			m_pBitmap = std::make_shared<C16BitGrayBitmap>();
			ZTRACE_RUNTIME("Creating 16 Gray bit memory bitmap %p", m_pBitmap.get());
		}
		else if (bps == 32)
		{
			if (sampleformat == SAMPLEFORMAT_IEEEFP)
			{
				m_pBitmap = std::make_shared<C32BitFloatGrayBitmap>();
				ZTRACE_RUNTIME("Creating 32 float Gray bit memory bitmap %p", m_pBitmap.get());
			}
			else
			{
				m_pBitmap = std::make_shared<C32BitGrayBitmap>();
				ZTRACE_RUNTIME("Creating 32 Gray bit memory bitmap %p", m_pBitmap.get());
			}
		}
	}
	else if ((spp == 3) || (spp == 4))
	{
		if (bps == 8)
		{
			m_pBitmap = std::make_shared<C24BitColorBitmap>();
			ZTRACE_RUNTIME("Creating 8 RGB bit memory bitmap %p", m_pBitmap.get());
		}
		else if (bps == 16)
		{
			m_pBitmap = std::make_shared<C48BitColorBitmap>();
			ZTRACE_RUNTIME("Creating 16 RGB bit memory bitmap %p", m_pBitmap.get());
		}
		else if (bps == 32)
		{
			if (sampleformat == SAMPLEFORMAT_IEEEFP)
			{
				m_pBitmap = std::make_shared<C96BitFloatColorBitmap>();
				ZTRACE_RUNTIME("Creating 32 float RGB bit memory bitmap %p", m_pBitmap.get());
			}
			else
			{
				m_pBitmap = std::make_shared<C96BitColorBitmap>();
				ZTRACE_RUNTIME("Creating 32 RGB bit memory bitmap %p", m_pBitmap.get());
			}
		}
	}

	if (static_cast<bool>(m_pBitmap))
	{
		bResult = m_pBitmap->Init(w, h);
		m_pBitmap->SetCFA(cfa);
		if (cfatype != 0)
		{
			if (C16BitGrayBitmap* pGray16Bitmap = dynamic_cast<C16BitGrayBitmap*>(m_pBitmap.get()))
				pGray16Bitmap->SetCFAType(static_cast<CFATYPE>(cfatype));
		}

		m_pBitmap->SetMaster(master);
		m_pBitmap->SetISOSpeed(isospeed);
		m_pBitmap->SetGain(gain);
		m_pBitmap->SetExposure(exposureTime);
		m_pBitmap->SetAperture(aperture);
		m_pBitmap->m_DateTime = m_DateTime;

		CString strDescription;
		if (strMakeModel.GetLength() != 0)
			strDescription.Format(_T("TIFF (%s)"), static_cast<LPCTSTR>(strMakeModel));
		else
			strDescription	= "TIFF";
		m_pBitmap->SetDescription(strDescription);
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

void CTIFFReadInMemoryBitmap::OnRead(int lX, int lY, double fRed, double fGreen, double fBlue)
{

	try
	{
		if (static_cast<bool>(m_pBitmap))
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
}

/* ------------------------------------------------------------------- */

bool CTIFFReadInMemoryBitmap::OnClose()
{
	ZFUNCTRACE_RUNTIME();

	if (static_cast<bool>(m_pBitmap))
	{
		m_outBitmap = m_pBitmap;
		return true;
	}
	else
		return false;
}

/* ------------------------------------------------------------------- */

bool ReadTIFF(LPCTSTR szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, CDSSProgress *	pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CTIFFReadInMemoryBitmap	tiff(szFileName, rpBitmap, pProgress);
	return tiff.Open() && tiff.Read();
}

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


int LoadTIFFPicture(LPCTSTR szFileName, CBitmapInfo& BitmapInfo, std::shared_ptr<CMemoryBitmap>& rpBitmap, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	int result = -1;		// -1 means not a TIFF file.

	if (GetTIFFInfo(szFileName, BitmapInfo) && BitmapInfo.CanLoad())
	{
		if (ReadTIFF(szFileName, rpBitmap, pProgress))
		{
			if (BitmapInfo.IsCFA() && (IsSuperPixels() || IsRawBayer() || IsRawBilinear() || IsRawAHD()))
			{
				if (CCFABitmapInfo* pGrayBitmap = dynamic_cast<CCFABitmapInfo*>(rpBitmap.get()))
				{
					if (IsSuperPixels())
						pGrayBitmap->UseSuperPixels(true);
					else if (IsRawBayer())
						pGrayBitmap->UseRawBayer(true);
					else  if (IsRawBilinear())
						pGrayBitmap->UseBilinear(true);
					else if (IsRawAHD())
						pGrayBitmap->UseAHD(true);
				}
			}
			result = 0;
		}
		else
			result = 1; // Failed to load file
	}
	return result;
}
