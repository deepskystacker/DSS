#include "stdafx.h"
#include "TIFFUtil.h"
//#include "resource.h"
#include "ztrace.h"
#include "BitmapInfo.h"
#include "DSSProgress.h"
#include "Multitask.h"
#include "ColorHelpers.h"
#include "ColorBitmap.h"
#include "zexcbase.h"
#include "RAWUtils.h"
#include "tiffio.h"
#include "dssbase.h"
#include "zlib.h"

using namespace DSS;

namespace
{

constexpr size_t NRCUSTOMTIFFTAGS = 12;

constexpr std::array<TIFFFieldInfo, NRCUSTOMTIFFTAGS> DssTiffFieldTable {
{
	{ TIFFTAG_DSS_NRFRAMES,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSNumberOfFrames") },
	{ TIFFTAG_DSS_TOTALEXPOSUREOLD, 1, 1, TIFF_LONG, FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSTotalExposureOld") },
	{ TIFFTAG_DSS_TOTALEXPOSURE, 1, 1, TIFF_FLOAT, FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSTotalExposure") },
	{ TIFFTAG_DSS_ISO,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSISO") },
	{ TIFFTAG_DSS_GAIN,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSGain") },
	{ TIFFTAG_DSS_SETTINGSAPPLIED,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSSettingsApplied") },
	{ TIFFTAG_DSS_BEZIERSETTINGS,	-1,-1, TIFF_ASCII, FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSBezierSettings") },
	{ TIFFTAG_DSS_ADJUSTSETTINGS,	-1,-1, TIFF_ASCII, FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSAdjustSettings") },
	{ TIFFTAG_DSS_CFA,	1, 1, TIFF_LONG, FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSCFA") },
	{ TIFFTAG_DSS_MASTER,	1, 1, TIFF_LONG, FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSMaster") },
	{ TIFFTAG_DSS_CFATYPE,	1, 1, TIFF_LONG, FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSCFATYPE") },
	{ TIFFTAG_DSS_APERTURE, 1, 1, TIFF_FLOAT, FIELD_CUSTOM,
	  false,	false,	const_cast<char*>("DSSAperture") }
} };

constexpr std::array<uint8_t, 4> TIFF_CFAPattern_RGGB{ 0, 1, 1, 2 };
constexpr std::array<uint8_t, 4> TIFF_CFAPattern_BGGR{ 2, 1, 1, 0 };
constexpr std::array<uint8_t, 4> TIFF_CFAPattern_GRBG{ 1, 0, 2, 1 };
constexpr std::array<uint8_t, 4> TIFF_CFAPattern_GBRG{ 1, 2, 0, 1 };

//
// Write the image out as Strips (i.e. not scanline by scanline)
// 
constexpr unsigned int STRIP_SIZE_DEFAULT = 16 * 1024 * 1024;	// 16MB

struct
{
	uint16_t dim[2]{ 0 };
	union
	{
		uint8_t cfa4[4];
		uint8_t cfa9[9];
		uint8_t cfa16[16];
	} cfa { 0 };
} cfaDimPat;

constinit TIFFExtendProc g_TIFFParentExtender = nullptr;
constinit bool			 g_TIFFInitialized = false;

/* ------------------------------------------------------------------- */

void DSSTIFFDefaultDirectory(TIFF *tif)
{
	static_assert(DssTiffFieldTable.size() == NRCUSTOMTIFFTAGS);

    /* Install the extended Tag field info */
    TIFFMergeFieldInfo(tif, DssTiffFieldTable.data(), NRCUSTOMTIFFTAGS);

    /* Since an XTIFF client module may have overridden
     * the default directory method, we call it now to
     * allow it to set up the rest of its own methods.
     */

    if (g_TIFFParentExtender != nullptr)
        g_TIFFParentExtender(tif);
}

} // anonymous namespace

void DSSTIFFInitialize()
{
    if (!g_TIFFInitialized)
	{
		g_TIFFInitialized = true;
	    /* Grab the inherited method and install */
		g_TIFFParentExtender = TIFFSetTagExtender(DSSTIFFDefaultDirectory);
	}
}


/* ------------------------------------------------------------------- */

CTIFFReader::CTIFFReader(const fs::path& p, ProgressBase* pProgress) :
	m_tiff{ nullptr },
	file{ p },
	m_pProgress{ pProgress }
{}

void CTIFFReader::decodeCfaDimPat(int patternSize, const char* tagName)
{
	ZTRACE_RUNTIME("TIFF - Get CFA type from %s", tagName);
	ZTRACE_RUNTIME("CFA pattern dimension: %hhu x %hhu", cfaDimPat.dim[0], cfaDimPat.dim[1]);

	const auto setCfaType = [this](const decltype(CTIFFHeader::cfatype) type) {
		this->cfatype = type;
		this->cfa = (type != CFATYPE_NONE);
	};

	if (4 == patternSize)
	{
		ZTRACE_RUNTIME("CFAPATTERN: %hhu%hhu%hhu%hhu", cfaDimPat.cfa.cfa4[0], cfaDimPat.cfa.cfa4[1], cfaDimPat.cfa.cfa4[2], cfaDimPat.cfa.cfa4[3]);

		if (0 == memcmp(cfaDimPat.cfa.cfa4, TIFF_CFAPattern_RGGB.data(), sizeof(cfaDimPat.cfa.cfa4)))
		{
			ZTRACE_RUNTIME("CFAType set to RGGB");
			setCfaType(CFATYPE_RGGB);
		}
		else if (0 == memcmp(cfaDimPat.cfa.cfa4, TIFF_CFAPattern_BGGR.data(), sizeof(cfaDimPat.cfa.cfa4)))
		{
			ZTRACE_RUNTIME("CFAType set to BGGR");
			setCfaType(CFATYPE_BGGR);
		}
		else if (0 == memcmp(cfaDimPat.cfa.cfa4, TIFF_CFAPattern_GRBG.data(), sizeof(cfaDimPat.cfa.cfa4)))
		{
			ZTRACE_RUNTIME("CFAType set to GRBG");
			setCfaType(CFATYPE_GRBG);
		}
		else if (0 == memcmp(cfaDimPat.cfa.cfa4, TIFF_CFAPattern_GBRG.data(), sizeof(cfaDimPat.cfa.cfa4)))
		{
			ZTRACE_RUNTIME("CFAType set to GBRG");
			setCfaType(CFATYPE_GBRG);
		}
		else
		{
			DSSBase::instance()->reportError(
				QCoreApplication::translate("TIFFUtil", "CFA pattern: %1%2%3%4 is not supported")
				.arg(cfaDimPat.cfa.cfa4[0])
				.arg(cfaDimPat.cfa.cfa4[1])
				.arg(cfaDimPat.cfa.cfa4[2])
				.arg(cfaDimPat.cfa.cfa4[3]),
				"Unsupported CFA Pattern",
				DSSBase::Severity::Warning,
				DSSBase::Method::QErrorMessage);
		}

	}
	else
	{
		DSSBase::instance()->reportError(
			QCoreApplication::translate("TIFFUtil", "CFA pattern dimension: %1x%2 found is not supported")
			.arg(cfaDimPat.dim[0])
			.arg(cfaDimPat.dim[1]),
			"Unsupported CFA PatternDim",
			DSSBase::Severity::Warning,
			DSSBase::Method::QErrorMessage);
	}

}

bool CTIFFReader::Open()
{
	ZFUNCTRACE_RUNTIME();

	//
	// Used for reading lengths of TIFF custom tags.  Different tags
	// return length as uint8, uint16, uint16[2], or uint32
	//
	union
	{
		uint32_t Long;
		uint16_t Short1;
		uint16_t Short2[2];
		uint8_t Char[4];
	} unionLong{ 0 };

	// Used to read pointer to data when reading custom tags
	void* pVoidArray{ nullptr };

	bool			bResult = false;
	QSettings		settings;

	const auto dwSkipExifInfo = settings.value("SkipTIFFExifInfo", uint(0)).toUInt();

	//
	// Quietly attempt to open the putative TIFF file 
	//
	TIFFErrorHandler	oldHandler = TIFFSetErrorHandler(nullptr);
	TIFFErrorHandlerExt	oldHandlerExt = TIFFSetErrorHandlerExt(nullptr);
#ifdef Q_OS_WIN
	m_tiff = TIFFOpenW(file.wstring().c_str(), "r");
#else
	m_tiff = TIFFOpen(reinterpret_cast<const char*>(file.u8string().c_str()), "r");
#endif
	TIFFSetErrorHandler(oldHandler);
	TIFFSetErrorHandlerExt(oldHandlerExt);

	if (m_tiff != nullptr)
	{
		ZTRACE_RUNTIME("Opened %s", file.generic_u8string().c_str());

		cfa = false;
		cfatype = CFATYPE_NONE;
		master = 0;
		samplemin = 0;
		samplemax = 1.0;
		exposureTime = 0.0;
		isospeed = 0;
		gain = -1;

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
		}

		// Check that this is a compatible TIFF format
		// Support is :
		// 8, 16, 32 bits per pixels (with slight restriction on format)
		// No compression or LZW compression or ZIP (deflate) compression
		// sample per pixel 1 (gray levels) or 3 (rgb)
		if ((planarconfig == PLANARCONFIG_CONTIG) &&
			((compression == COMPRESSION_NONE) ||
			 (compression == COMPRESSION_LZW) ||
			 (compression == COMPRESSION_DEFLATE) ||
			 (compression == COMPRESSION_ADOBE_DEFLATE)) &&
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
				}
			}

			if (bResult)
			{
				if ((spp == 3) || (spp == 4))
					bResult = (PHOTOMETRIC_RGB == photo);
				else if (spp == 1)
					bResult = (PHOTOMETRIC_MINISBLACK == photo || PHOTOMETRIC_CFA == photo);
			}
		}

		// Retrieve the Date/Time as in the TIFF TAG
		char* szDateTime;

		if (TIFFGetField(m_tiff, TIFFTAG_DATETIME, &szDateTime))
		{
			QString	strDateTime{ szDateTime };

			// Decode 2007:11:02 22:07:03
			//        0123456789012345678

			if (strDateTime.length() >= 19)
			{
				m_DateTime = QDateTime::fromString(strDateTime, "yyyy:MM:dd hh:mm:ss");
			}
		}

		//
		// Attempt to read the CFA from the root IFD if this is could be a CFA image
		//
		if (bResult && 1 == spp)
		{
			if (PHOTOMETRIC_CFA == photo) ZTRACE_RUNTIME("TIFFTAG_PHOTOMETRIC is set to PHOTOMETRIC_CFA");
			else ZTRACE_RUNTIME("TIFFTAG_PHOTOMETRIC is set to PHOTOMETRIC_MINISBLACK");
			int count{ 0 };

			ZTRACE_RUNTIME("Checking for TIFFTAG_CFAREPEATPATTERNDIM, TIFFTAG_CFAPATTERN");

			if (TIFFGetField(m_tiff, TIFFTAG_CFAREPEATPATTERNDIM, &pVoidArray))
			{
				ZTRACE_RUNTIME("TIFFTAG_CFAREPEATPATTERNDIM read OK");
				cfaDimPat = {};		// clear the Dimension and Pattern structure
				memcpy(&cfaDimPat.dim, pVoidArray, sizeof(cfaDimPat.dim));

				int patternSize{ cfaDimPat.dim[0] * cfaDimPat.dim[1] };

				if (TIFFGetField(m_tiff, TIFFTAG_CFAPATTERN, &unionLong, &pVoidArray))
				{
					ZTRACE_RUNTIME("TIFFTAG_CFAPATTERN read OK");
					count = unionLong.Short1;
					ZASSERT(count == patternSize && count <= sizeof(cfaDimPat.cfa));
					memcpy(&cfaDimPat.cfa, pVoidArray, count);
					decodeCfaDimPat(patternSize, "TIFFTAG_CFAPATTERN");
				}
			}
		}

		if (dwSkipExifInfo == 0)
		{
			// Try to read EXIF data
			uint64_t ExifID;

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

					// EXIFTAG_ISOSPEEDRATINGS is an array of uint16 according to the EXIF spec
					isospeed = 0;
					uint16_t	count = 0;
					uint16_t* iso_setting = nullptr;
					if (TIFFGetField(m_tiff, EXIFTAG_ISOSPEEDRATINGS, &count, &iso_setting))
					{
						isospeed = iso_setting[0];
					}
					else isospeed = 0;

					// EXIFTAG_GAINCONTROL does not represent a gain value, so ignore it.

					//
					// If we've not yet detected a cfa pattern interrogate EXIFTAG_CFAPATTERN
					//
					if (!cfa && spp == 1)
					{
						ZTRACE_RUNTIME("Checking for EXIF_CFAPATTERN tag");

						if (TIFFGetField(m_tiff, EXIFTAG_CFAPATTERN, &unionLong, &pVoidArray))
						{
							memcpy(&cfaDimPat, pVoidArray, unionLong.Short1);
							decodeCfaDimPat(cfaDimPat.dim[0] * cfaDimPat.dim[1], "EXIFTAG_CFAPATTERN");
						}
					}

					// Revert IFD to status quo ante TIFFReadEXIFDirectory
					//
					TIFFSetDirectory(m_tiff, currentIFD);
				}
			}
		}
		else
		{
			CBitmapInfo			BitmapInfo;
			if (RetrieveEXIFInfo(file, BitmapInfo))
			{
				exposureTime = BitmapInfo.m_fExposure;
				aperture	 = BitmapInfo.m_fAperture;
				isospeed	 = BitmapInfo.m_lISOSpeed;
				gain		 = BitmapInfo.m_lGain;
				m_DateTime	 = BitmapInfo.m_DateTime;
			}
		}

		//
		// If we have not yet found a setting for the CFA look to see if
		// it is recorded in our private TIFF tags.
		//
		if (!cfa && spp == 1)
		{
			ZTRACE_RUNTIME("CFAType not yet set: Checking DSS private TIFF tags");
			int32_t cfaValue = 0;

			if (!TIFFGetField(m_tiff, TIFFTAG_DSS_CFA, &cfaValue))
				cfaValue = 0;
			if (0 != cfaValue)
				cfa = true;
			if (TIFFGetField(m_tiff, TIFFTAG_DSS_CFATYPE, &cfatype))
			{
				ZTRACE_RUNTIME("CFAType set to %u", cfatype);
			}
			else 
			{
				cfa = false;
				cfatype = CFATYPE_NONE;
			}

		}

		//
		// If this file is an eight bit TIFF, and purports to have a Bayer pattern
		// inform the the user that we aren't going to play
		//
		if ((1 == spp) && (8 == bps) && cfa)
		{
			// 
			// Set CFA type to none even if the TIFF tags specified otherwise
			//
			cfatype = CFATYPE_NONE;
			cfa = false;
			QString errorMessage{ QCoreApplication::translate("TIFFUtil",
									"DeepSkyStacker will not de-Bayer 8 bit images",
									"IDS_8BIT_FITS_NODEBAYER") };
			DSSBase::instance()->reportError(
				errorMessage,
				"Will not de-Bayer 8 bit images",
				DSSBase::Severity::Warning,
				DSSBase::Method::QErrorMessage);
		}




		if (bResult)
			bResult = OnOpen();
		if (!bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = nullptr;
		}
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CTIFFReader::Read()
{
	constexpr double scaleFactorInt16 = 1.0 + std::numeric_limits<std::uint8_t>::max();
	constexpr double scaleFactorInt32 = scaleFactorInt16 * (1 + std::numeric_limits<std::uint16_t>::max());

	ZFUNCTRACE_RUNTIME();

	if (!m_tiff)
		return false;

	tmsize_t		scanLineSize;

	if (m_pProgress)
		m_pProgress->Start2(h);

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
			m_pProgress->Progress2((this->h / 2 * i) / stripCount);
	}

	std::uint8_t* byteBuff = buffer.get();
	std::uint16_t* shortBuff = reinterpret_cast<std::uint16_t*>(byteBuff);
	std::uint32_t* u32Buff = reinterpret_cast<std::uint32_t*>(byteBuff);
	float* floatBuff = reinterpret_cast<float*>(byteBuff);

	std::atomic_bool stop { false };

	const auto normalizeFloatValue = [sampleMin = this->samplemin, sampleMax = this->samplemax](const float value) -> double
	{
		constexpr double scaleFactor = (1.0 + std::numeric_limits<std::uint16_t>::max()) / 256.0;
		const double normalizationFactor = scaleFactor / (sampleMax - sampleMin);
		return (static_cast<double>(value) - sampleMin) * normalizationFactor;
	};

	const auto loopOverPixels = [height = this->h, width = this->w, progress = this->m_pProgress](const int row, std::predicate<int> auto const& function) -> void
	{
		for (int col = 0; col < width; ++col)
		{
			if (!function(col))
				return;
		}
		if (progress != nullptr && omp_get_thread_num() == 0)
			progress->Progress2((height + row) / 2);

	};

	if (sampleformat == SAMPLEFORMAT_IEEEFP)
	{
		assert(bps == 32);

		if (spp == 1)
#pragma omp parallel for default(shared) schedule(dynamic, 50) shared(stop) if(CMultitask::GetNrProcessors() > 1) // GetNrProcessors() returns 1, if user selected single-thread.
			for (int y = 0; y < this->h; ++y)
			{
				if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
				loopOverPixels(y, [&](const int x) -> bool
				{
					//
					// If another thread has set stop, return false to tell loopOverPixels to stop immediately
					//
					if (stop.load()) return false;

					const double gray = normalizeFloatValue(floatBuff[y * w + x]);
					if (OnRead(x, y, gray, gray, gray))
						return true;
					stop = true;
					return false;				
				});
			}
		else
#pragma omp parallel for default(shared) schedule(dynamic, 50) shared(stop) if(CMultitask::GetNrProcessors() > 1)
			for (int y = 0; y < this->h; ++y)
			{
				if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
				loopOverPixels(y, [&](const int x) -> bool
				{
					//
					// If another thread has set stop, return false to tell loopOverPixels to stop immediately
					//
					if (stop.load()) return false;

					const int index = (y * w + x) * spp;
					const double red = normalizeFloatValue(floatBuff[index]);
					const double green = normalizeFloatValue(floatBuff[index + 1]);
					const double blue = normalizeFloatValue(floatBuff[index + 2]);
					if (OnRead(x, y, red, green, blue))
						return true;
					stop = true;
					return false;
				});
			}
	}
	else
	{
		if (spp == 1)
			switch (bps)
			{
			case 8: {
#pragma omp parallel for default(shared) schedule(dynamic, 50) shared(stop) if(CMultitask::GetNrProcessors() > 1)
				for (int y = 0; y < this->h; ++y)
				{
					if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
					loopOverPixels(y, [&](const int x) -> bool
					{
						//
						// If another thread has set stop, return false to tell loopOverPixels to stop immediately
						//
						if (stop.load()) return false;

						const double fGray = byteBuff[y * w + x];
						if (OnRead(x, y, fGray, fGray, fGray))
							return true;
						stop = true;
						return false;

					});
				}
				} break;
			case 16: {
#pragma omp parallel for default(shared) schedule(dynamic, 50) shared(stop) if(CMultitask::GetNrProcessors() > 1)
				for (int y = 0; y < this->h; ++y)
				{
					if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
					loopOverPixels(y, [&](const int x) -> bool
					{
						//
						// If another thread has set stop, return false to tell loopOverPixels to stop immediately
						//
						if (stop.load()) return false;

						const double fGray = shortBuff[y * w + x] / scaleFactorInt16;
						if (OnRead(x, y, fGray, fGray, fGray))
							return true;
						stop = true;
						return false;
					});
				}
				} break;
			case 32: {
#pragma omp parallel for default(shared) schedule(dynamic, 50) shared(stop) if(CMultitask::GetNrProcessors() > 1)
				for (int y = 0; y < this->h; ++y)
				{
					if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
					loopOverPixels(y, [&](const int x) -> bool
					{
						//
						// If another thread has set stop, return false to tell loopOverPixels to stop immediately
						//
						if (stop.load()) return false;

						const double fGray = u32Buff[y * w + x] / scaleFactorInt32;
						if (OnRead(x, y, fGray, fGray, fGray))
							return true;
						stop = true;
						return false;
					});
				}

				} break;
			}
		else
			switch (bps)
			{
			case 8: {
#pragma omp parallel for default(shared) schedule(dynamic, 50) shared(stop) if(CMultitask::GetNrProcessors() > 1)
				for (int y = 0; y < this->h; ++y)
				{
					if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
					loopOverPixels(y, [&](const int x) -> bool
					{
						//
						// If another thread has set stop, return false to tell loopOverPixels to stop immediately
						//
						if (stop.load()) return false;

						const int index = (y * w + x) * spp;
						const double fRed = byteBuff[index];
						const double fGreen = byteBuff[index + 1];
						const double fBlue = byteBuff[index + 2];
						if (OnRead(x, y, fRed, fGreen, fBlue))
							return true;
						stop = true;
						return false;
					});
				}
				} break;
			case 16: {
#pragma omp parallel for default(shared) schedule(dynamic, 50) shared(stop) if(CMultitask::GetNrProcessors() > 1)
				for (int y = 0; y < this->h; ++y)
				{
					if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
					loopOverPixels(y, [&](const int x) -> bool
					{
						//
						// If another thread has set stop, return false to tell loopOverPixels to stop immediately
						//
						if (stop.load()) return false;

						const int index = (y * w + x) * spp;
						const double fRed = shortBuff[index] / scaleFactorInt16;
						const double fGreen = shortBuff[index + 1] / scaleFactorInt16;
						const double fBlue = shortBuff[index + 2] / scaleFactorInt16;
						if (OnRead(x, y, fRed, fGreen, fBlue))
							return true;
						stop = true;
						return false;
					});
				}
				} break;
			case 32: {
#pragma omp parallel for default(shared) schedule(dynamic, 50) shared(stop) if(CMultitask::GetNrProcessors() > 1)
				for (int y = 0; y < this->h; ++y)
				{
					if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
					loopOverPixels(y, [&](const int x) -> bool
					{
						//
						// If another thread has set stop, return false to tell loopOverPixels to stop immediately
						//
						if (stop.load()) return false;

						const int index = (y * w + x) * spp;
						const double fRed = u32Buff[index] / scaleFactorInt32;
						const double fGreen = u32Buff[index + 1] / scaleFactorInt32;
						const double fBlue = u32Buff[index + 2] / scaleFactorInt32;
						if (OnRead(x, y, fRed, fGreen, fBlue))
							return true;
						stop = true;
						return false;
						});
				}
				} break;
			}
	}

	if (m_pProgress)
		m_pProgress->End2();
	if (stop.load()) return false;

	return true;
}

/* ------------------------------------------------------------------- */

bool CTIFFReader::Close()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = true;
	if (m_tiff != nullptr)
	{
		bResult = OnClose();
		if (bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = nullptr;
		}
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

CTIFFWriter::CTIFFWriter(const fs::path& p, ProgressBase* pProgress) :
	m_tiff{ nullptr },
	file{ p },
	m_pProgress{ pProgress },
	m_Format{ TF_UNKNOWN }
{
	compression = COMPRESSION_DEFLATE;
}

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
	}
}

void CTIFFWriter::SetCompression(TIFFCOMPRESSION tiffcomp)
{
	switch (tiffcomp)
	{
	case TC_LZW:
		compression = COMPRESSION_LZW;
		break;
	case TC_DEFLATE:
		compression = COMPRESSION_DEFLATE;
		break;
	default:
		compression = COMPRESSION_NONE;
	}
}

void CTIFFWriter::SetFormatAndCompression(TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression)
{
	m_Format = TIFFFormat;
	SetCompression(TIFFCompression);
}

bool CTIFFWriter::Open()
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;
	constexpr unsigned char exifVersion[4] {'0', '2', '3', '1' }; // EXIF 2.31 version is 4 characters of a string!
	uint64_t dir_offset_EXIF{ 0 };

#ifdef Q_OS_WIN
	m_tiff = TIFFOpenW(file.wstring().c_str(), "w");
#else
	m_tiff = TIFFOpen(reinterpret_cast<const char*>(file.u8string().c_str()), "w");
#endif
	if (m_tiff != nullptr)
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
			{
				//
				// Only support CFA types BGGR, GRBG, GBRG, RGGB
				//
				if (cfa && cfatype >= static_cast<uint32_t>(CFATYPE_BGGR) && cfatype <= static_cast<uint32_t>(CFATYPE_RGGB))
				{
					cfaDimPat = {};
					// TIFFSetField(m_tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CFA);  // Don't use this - breaks too many things
					cfaDimPat.dim[0] = 2; cfaDimPat.dim[1] = 2;
					TIFFSetField(m_tiff, TIFFTAG_CFAREPEATPATTERNDIM, cfaDimPat.dim);
					//
					// Note that when writing the CFA pattern, need to specify how many
					// octets are to be written.
					//
					switch (cfatype)
					{
					case CFATYPE_BGGR:
						memcpy(cfaDimPat.cfa.cfa4, TIFF_CFAPattern_BGGR.data(), sizeof(cfaDimPat.cfa.cfa4));
						break;
					case CFATYPE_GRBG:
						memcpy(cfaDimPat.cfa.cfa4, TIFF_CFAPattern_GRBG.data(), sizeof(cfaDimPat.cfa.cfa4));
						break;
					case CFATYPE_GBRG:
						memcpy(cfaDimPat.cfa.cfa4, TIFF_CFAPattern_GBRG.data(), sizeof(cfaDimPat.cfa.cfa4));
						break;
					case CFATYPE_RGGB:
						memcpy(cfaDimPat.cfa.cfa4, TIFF_CFAPattern_RGGB.data(), sizeof(cfaDimPat.cfa.cfa4));
						break;
					}
					TIFFSetField(m_tiff, TIFFTAG_CFAPATTERN, 4, cfaDimPat.cfa.cfa4);

				}
				photo = PHOTOMETRIC_MINISBLACK;
			}
			
			TIFFSetField(m_tiff, TIFFTAG_PHOTOMETRIC, photo);

			TIFFSetField(m_tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
			TIFFSetField(m_tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);

			//
			// Following line commented out as enabling this broke Photoshop (16 August 2020)
			//
			//if (IsFloat()) TIFFSetField(m_tiff, TIFFTAG_PREDICTOR, PREDICTOR_FLOATINGPOINT);

			if (samplemax != samplemin)
			{
				TIFFSetField(m_tiff, TIFFTAG_SMINSAMPLEVALUE, samplemin);
				TIFFSetField(m_tiff, TIFFTAG_SMAXSAMPLEVALUE, samplemax);
			}

			//
			// Set Software name in the same way we do elsewhere.
			//
			const QString strSoftware{ QString("DeepSkyStacker %1").arg(VERSION_DEEPSKYSTACKER) };
			TIFFSetField(m_tiff, TIFFTAG_SOFTWARE, strSoftware.toLatin1().constData());

			if (!m_strDescription.isEmpty())
			{
				TIFFSetField(m_tiff, TIFFTAG_IMAGEDESCRIPTION, m_strDescription.toStdString().c_str());
			}

			if (m_DateTime.isValid())
			{
				// Set the DATETIME TIFF tag
				QString strDateTime = m_DateTime.toString("yyyy:MM:dd hh:mm:ss");

				TIFFSetField(m_tiff, TIFFTAG_DATETIME, strDateTime.toStdString().c_str());
			}

			/* It is good to set resolutions too (but it is not nesessary) */
			TIFFSetField(m_tiff, TIFFTAG_XRESOLUTION, 100.0f);
			TIFFSetField(m_tiff, TIFFTAG_YRESOLUTION, 100.0f);
			TIFFSetField(m_tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

			if (cfa)
				TIFFSetField(m_tiff, TIFFTAG_DSS_CFA, cfa ? 1 : 0);
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

			const tmsize_t scanLineSize = TIFFScanlineSize(m_tiff);
			ZTRACE_RUNTIME("TIFF Scan Line Size %zu", scanLineSize);

			//
			// Work out how many scanlines fit into the default strip
			//
			const unsigned int rowsPerStrip = STRIP_SIZE_DEFAULT / scanLineSize;

			ZTRACE_RUNTIME("Seting TIFFTAG_ROWSPERSTRIP to: %u", rowsPerStrip);
			TIFFSetField(m_tiff, TIFFTAG_ROWSPERSTRIP, rowsPerStrip);

			int numStrips = h / rowsPerStrip;
			//
			// If it wasn't an exact division (IOW there's a remainder), add one
			// for the final (short) strip.
			//
			if (0 != (h % rowsPerStrip))
				++numStrips;

			//
			// Pre-fill the StripOffsets and StripByteCounts tags with values of zero.
			// 
			// The values will be updated when the data is actually written, but the size
			// of the base IFD won't change which in turn means it can be rewritten in 
			// the same location as the file is closed.
			// 
			// Turn off the TIFF error handle while we do this, otherwise an error message
			// 
			//		ZIPEncode: Encoder error: buffer error.
			//
			// is issued for each empty strip.
			//
			TIFFErrorHandler    oldHandler = TIFFSetErrorHandler(nullptr);
			TIFFErrorHandlerExt oldHandlerExt = TIFFSetErrorHandlerExt(nullptr);
			ZTRACE_RUNTIME("Writing %d empty encoded strips", numStrips);
			for (int strip = 0; strip < numStrips; ++strip)
			{
				TIFFWriteEncodedStrip(m_tiff, strip, nullptr, 0);
			}
			TIFFSetErrorHandler(oldHandler);
			TIFFSetErrorHandlerExt(oldHandlerExt);


			//***************************************************************************
			// 
			// Now write the EXIF IFD
			// 
			// **************************************************************************
			
			//
			// Set a dummy EXIF tag in the original tiff-structure in order to reserve
			// space for final dir_offset value, which is properly written at the end.
			// 
			// Initially use a value of 0 for dir_offset_EXIF
			//
			TIFFSetField(m_tiff, TIFFTAG_EXIFIFD, dir_offset_EXIF);

			//
			// Save current tiff-directory to file before directory is changed.
			// Otherwise it will be lost! The tif-structure is overwritten/ freshly
			// initialized by any "CreateDirectory"
			//
			TIFFWriteDirectory(m_tiff);
			//
			// Get current TIFF Directory so we can return to it
			//
			const auto currentIFD = TIFFCurrentDirectory(m_tiff);
			TIFFCreateEXIFDirectory(m_tiff);
			TIFFSetField(m_tiff, EXIFTAG_EXIFVERSION, exifVersion);

			//
			// Now we can write EXIF tags we want to ...
			//
			if (0.0 != exposureTime)
			{
				TIFFSetField(m_tiff, EXIFTAG_EXPOSURETIME, exposureTime);
			}
			if (0.0 != aperture)
			{
				TIFFSetField(m_tiff, EXIFTAG_FNUMBER, aperture);
			}
			if (0 != isospeed)
			{
				// EXIFTAG_ISOSPEEDRATINGS is array of uint16 according to the EXIF spec
				constexpr uint16_t count = 1U;
				const uint16_t iso_setting = static_cast<uint16_t>(isospeed);
				TIFFSetField(m_tiff, EXIFTAG_ISOSPEEDRATINGS, count, &iso_setting);
			}
			
			//
			// Now write EXIFTAG_CFAPATTERN which is basically what we put into cfaDimPat 
			// when the other (non-EXIF) CFA related tags were written.
			//
			if (cfa)
			{
				constexpr uint16_t count = sizeof(cfaDimPat.dim) + sizeof(cfaDimPat.cfa.cfa4);
				TIFFSetField(m_tiff, EXIFTAG_CFAPATTERN, count, cfaDimPat);
			}
			
			//
			// Now that all the EXIF tags are written, need to write the EXIF
			// custom directory into the file...
			// 
			// WriteCustomDirectory returns the actual offset of the EXIF directory.
			//
			TIFFWriteCustomDirectory(m_tiff, &dir_offset_EXIF);

			// Go back to the first (main) directory, and set correct value of the
			// EXIFIFD pointer. Note that the directory is reloaded from the file!
			//
			TIFFSetDirectory(m_tiff, currentIFD);
			TIFFSetField(m_tiff, TIFFTAG_EXIFIFD, dir_offset_EXIF);
			TIFFCheckpointDirectory(m_tiff);

			//
			// The ZIP compression level must be set after the ZIP state has been re-initialised by TIFFSetDirectory().
			//
			if (COMPRESSION_DEFLATE == compression) TIFFSetField(m_tiff, TIFFTAG_ZIPQUALITY, Z_BEST_SPEED);
		}
		else
		{
			TIFFClose(m_tiff);
			m_tiff = nullptr;
		}
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CTIFFWriter::Write()
{
	ZFUNCTRACE_RUNTIME();
	bool result = true;
	bool error = false;

	//
    // Multipliers of 256.0 and 65536.0 were not correct and resulted in a fully saturated
	// pixel being written with a value of zero because the value overflowed the data type 
	// which was being stored.   Change the code to use UCHAR_MAX and USHRT_MAX
	//

	if (m_tiff != nullptr)
	{
		tmsize_t scanLineSize = TIFFScanlineSize(m_tiff);

		ZTRACE_RUNTIME("TIFF spp=%d, bps=%d, w=%d, h=%d", spp, bps, w, h);

		//
        // Allocate enough to hold the entire image
        //
		const tmsize_t buffSize = scanLineSize * h;
		ZTRACE_RUNTIME("Allocating buffer of %zu bytes", buffSize);
		const tdata_t buff = static_cast<tdata_t>(malloc(buffSize));

		if (buff != nullptr)
		{
			const int nrProcessors = CMultitask::GetNrProcessors();
			if (m_pProgress != nullptr)
				m_pProgress->Start2(h);

			auto* byteBuff = static_cast<std::uint8_t*>(buff);
			auto* shortBuff = static_cast<std::uint16_t*>(buff);
			auto* u32Buff = static_cast<std::uint32_t*>(buff);
			float* floatBuff = static_cast<float*>(buff);

			// int	rowProgress = 0;

			std::atomic_bool stop{ false };
#pragma omp parallel for default(shared) if(nrProcessors > 1)
			for (int row = 0; row < h; row++)
			{
				if (stop.load())
					continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.
				for (int col = 0; col < w; col++)
				{
					if (stop.load())
						break;	// OK to break from inner loop
					int index = (row * w * spp) + (col * spp);

					double fRed = 0, fGreen = 0, fBlue = 0, fGrey = 0;

					if (!OnWrite(col, row, fRed, fGreen, fBlue))
					{
						stop = true;
						result = false;
						break;
					}

					//
					// If its a cfa bitmap, set grey level to maximum of RGB
					// else convert from RGB to HSL and use Luminance.
					// 
					if (cfa)
					{
						fGrey = std::max(fRed, std::max(fGreen, fBlue));
					}
					else
					{
						double H, S, L;
						ToHSL(fRed, fGreen, fBlue, H, S, L);
						fGrey = L * 255.0;
					}

					switch (bps) // Bits per sample
					{
					case 8: // One byte
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
					case 16: // Two bytes
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
					case 32: // Unsigned int or 32 bit floating point 
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
						else switch (spp)
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
				if (m_pProgress != nullptr && 0 == omp_get_thread_num()) // Are we on the master thread? Without OPENMP omp_get_thread_num() returns always 0.
					m_pProgress->Progress2( (row * nrProcessors) / 2);	// Half the progress on the conversion, the other below on the writing.
			}

			if (false == result)
				return false;

			//
			// Work out how many scanlines fit into the default strip
			//
			const unsigned int rowsPerStrip = STRIP_SIZE_DEFAULT / scanLineSize;
			
			//
			// From that we derive the number of strips
			//
			int numStrips = h / rowsPerStrip;
			//
			// If it wasn't an exact division (IOW there's a remainder), add one
			// for the final (short) strip.
			//
			if (0 != (h % rowsPerStrip))
				++numStrips;

			ZTRACE_RUNTIME("Number of strips is %u", numStrips);

			auto* curr = static_cast<std::uint8_t*>(buff);
			tsize_t stripSize = rowsPerStrip * scanLineSize;
			tsize_t bytesRemaining = h * scanLineSize;
			tsize_t size = stripSize;
			int percentStep = (h / numStrips);
			for (int strip = 0; strip < numStrips; strip++)
			{
				if (bytesRemaining < stripSize)
					size = bytesRemaining;

				const tsize_t written = TIFFWriteEncodedStrip(m_tiff, strip, curr, size);
				if (-1 == written)
				{
					ZTRACE_RUNTIME("TIFFWriteEncodedStrip() failed");
					error = true;
					break;
				}
				curr += written;
				bytesRemaining -= written;

				if (m_pProgress != nullptr)
					m_pProgress->Progress2((h/2) + ((percentStep * strip) / 2));
			}

			free(buff);
			if (m_pProgress)
				m_pProgress->End2();
		}
		result = (!error);
	}

	return result;
}

/* ------------------------------------------------------------------- */

bool CTIFFWriter::Close()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = true;

	if (m_tiff != nullptr)
	{
		bResult = OnClose();
		//
		// Write the updated base IFD to disk.  This is only safe to do if the directory SIZE
		// hasn't changed at all since the end of the Open() member function.
		// 
		// This should be be case if the number of entries in the StripOffsets and StripByteCounts
		// tags haven't changed.
		//
		TIFFWriteDirectory(m_tiff);
		if (bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = nullptr;
		}
	}

	return bResult;
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CTIFFWriteFromMemoryBitmap : public CTIFFWriter
{
private :
	CMemoryBitmap* m_pMemoryBitmap;

private :
	TIFFFORMAT GetBestTiffFormat(const CMemoryBitmap* pBitmap);

public :
	CTIFFWriteFromMemoryBitmap(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress) :
		CTIFFWriter{ szFileName, pProgress },
		m_pMemoryBitmap{ pBitmap }
	{}

	virtual ~CTIFFWriteFromMemoryBitmap()
	{
		Close();
	}

	virtual bool Close() { return OnClose(); }
private:
	virtual bool OnOpen() override;
	virtual bool OnWrite(int lX, int lY, double & fRed, double & fGreen, double & fBlue) override;
	virtual bool OnClose() override;
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
	if (m_pMemoryBitmap->IsCFA())
		CFAType = m_pMemoryBitmap->GetCFAType();
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
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CTIFFWriteFromMemoryBitmap::OnWrite(int lX, int lY, double & fRed, double & fGreen, double & fBlue)
{
	bool result { true };
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
	catch (ZException& e)
	{
		QString errorMessage;
		if (e.locationAtIndex(0))
		{
			errorMessage = QCoreApplication::translate("Kernel",
				"Exception %1 thrown from %2 Function : %3() Line : %4\n\n %5")
				.arg(e.name())
				.arg(e.locationAtIndex(0)->fileName())
				.arg(e.locationAtIndex(0)->functionName())
				.arg(e.text(0));
		}
		else
		{
			errorMessage = QCoreApplication::translate("Kernel",
				"Exception %1 thrown from an unknown Function.\n\n%2")
				.arg(e.name())
				.arg(e.text(0));
		}
		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Critical, DSSBase::Method::QMessageBox);
		result = false;
	}

	return result;
}

/* ------------------------------------------------------------------- */

bool CTIFFWriteFromMemoryBitmap::OnClose()
{
	return true;
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, const QString& szDescription, int lISOSpeed, int lGain, double fExposure, double fAperture)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap != nullptr)
	{
		CTIFFWriteFromMemoryBitmap tiff{ szFileName, pBitmap, pProgress };

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
		}
	}

	return bResult;
}

/* ------------------------------------------------------------------- */
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress)
{
	return WriteTIFF(szFileName, pBitmap, pProgress, /*description*/"", /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0, /*fAperture*/ 0.0);
}
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase * pProgress, const QString& szDescription)
{
	return WriteTIFF(szFileName, pBitmap, pProgress, szDescription, /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0, /*fAperture*/ 0.0);
}

/* ------------------------------------------------------------------- */

bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression,
	const QString& szDescription, int lISOSpeed, int lGain, double fExposure, double fAperture)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap)
	{
		CTIFFWriteFromMemoryBitmap tiff(szFileName, pBitmap, pProgress);

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
		}
	}

	return bResult;
}

/* ------------------------------------------------------------------- */
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression)
{
	return WriteTIFF(szFileName, pBitmap, pProgress, TIFFFormat, TIFFCompression, /*description*/"", /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0, /*fAperture*/ 0.0);
}
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, const QString& szDescription)
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
	CTIFFReadInMemoryBitmap(const fs::path& szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase* pProgress):
		CTIFFReader(szFileName, pProgress),
		m_outBitmap{ rpBitmap }
	{}

	virtual ~CTIFFReadInMemoryBitmap() { Close(); };
private:
	virtual bool Close() { return OnClose(); };

	virtual bool OnOpen() override;
	virtual bool OnRead(int lX, int lY, double fRed, double fGreen, double fBlue) override;
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
		if (CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(m_pBitmap.get()))
		{
			if (0 != cfatype) pCFABitmapInfo->SetCFAType(static_cast<CFATYPE>(cfatype));
		}

		m_pBitmap->SetMaster(master);
		m_pBitmap->SetISOSpeed(isospeed);
		m_pBitmap->SetGain(gain);
		m_pBitmap->SetExposure(exposureTime);
		m_pBitmap->SetAperture(aperture);
		m_pBitmap->m_DateTime = m_DateTime;

		QString strDescription;
		if (!strMakeModel.isEmpty())
			strDescription = QString("TIFF (%1)").arg(strMakeModel);
		else
			strDescription = "TIFF";

		m_pBitmap->SetDescription(strDescription);
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CTIFFReadInMemoryBitmap::OnRead(int lX, int lY, double fRed, double fGreen, double fBlue)
{
	bool result = true;

	try
	{
		if (static_cast<bool>(m_pBitmap))
		{
			m_pBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);
		}
	}
	catch (ZException& e)
	{
		QString errorMessage;
		if (e.locationAtIndex(0))
		{
			errorMessage = QCoreApplication::translate("Kernel",
				"Exception %1 thrown from %2 Function : %3() Line : %4\n\n %5")
				.arg(e.name())
				.arg(e.locationAtIndex(0)->fileName())
				.arg(e.locationAtIndex(0)->functionName())
				.arg(e.text(0));
		}
		else
		{
			errorMessage = QCoreApplication::translate("Kernel",
				"Exception %1 thrown from an unknown Function.\n\n%2")
				.arg(e.name())
				.arg(e.text(0));
		}		
		DSSBase::instance()->reportError( errorMessage, "", DSSBase::Severity::Critical);
		result = false;
	}
	return result;
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

bool ReadTIFF(const fs::path& szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase *	pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CTIFFReadInMemoryBitmap	tiff(szFileName, rpBitmap, pProgress);
	return tiff.Open() && tiff.Read();
}

/* ------------------------------------------------------------------- */

bool	GetTIFFInfo(const fs::path& szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool					bResult = false;
	CTIFFReader				tiff(szFileName, nullptr);

	if (tiff.Open())
	{
		BitmapInfo.m_strFileName	= szFileName;

		QString makeModel{ tiff.getMakeModel() };
		if (!makeModel.isEmpty())
			BitmapInfo.m_strFileType = QString("TIFF (%1)").arg(tiff.getMakeModel());
		else
			BitmapInfo.m_strFileType = "TIFF";

		BitmapInfo.m_lWidth			= tiff.Width();
		BitmapInfo.m_lHeight		= tiff.Height();
		BitmapInfo.m_lBitsPerChannel = tiff.BitPerChannels();
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
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool	IsTIFFPicture(const fs::path& szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	return GetTIFFInfo(szFileName, BitmapInfo);
}


int LoadTIFFPicture(const fs::path& szFileName, CBitmapInfo& BitmapInfo, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase* pProgress)
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

CTIFFHeader::CTIFFHeader() :
	samplemax{ 1.0 },
	samplemin{ 0.0 },
	exposureTime{ 0.0f },
	aperture{ 0.0f },
	isospeed{ 0 },
	gain{ -1 },
	cfatype{ 0 },
	cfa{ false },
	nrframes{ 0 },
	w{ 0 },
	h{ 0 },
	spp{ 0 },
	bps{ 0 },
	photo{ 0 },
	compression{ 0 },
	planarconfig{ 0 },
	sampleformat{ 0 },
	master{ 0 }
{
	TIFFSetWarningHandler(nullptr);
	TIFFSetWarningHandlerExt(nullptr);
	//TIFFSetErrorHandler(nullptr);
	//TIFFSetErrorHandlerExt(nullptr);
	DSSTIFFInitialize();
}

bool CTIFFHeader::IsFloat() const
{
	return (sampleformat == SAMPLEFORMAT_IEEEFP) && (bps == 32);
}
