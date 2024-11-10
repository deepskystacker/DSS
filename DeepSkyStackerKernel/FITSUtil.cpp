#include "stdafx.h"
//#include "resource.h"
#include "Workspace.h"
#include "FITSUtil.h"
#include <fitsio.h>
#include "ztrace.h"
#include "DSSProgress.h"
#include "zexcbase.h"
#include "Multitask.h"
#include "GrayBitmap.h"
#include "ColorBitmap.h"
#include "RAWUtils.h"
#include "BitmapInfo.h"
#include "ColorHelpers.h"
#include "dssbase.h"

using namespace DSS;



/* ------------------------------------------------------------------- */

static	std::mutex mutex;

CFITSHeader::CFITSHeader()
{
	m_bFloat = false;
	m_fExposureTime = 0;
	m_lISOSpeed     = 0;
	m_lGain         = -1;
	m_CFAType		= CFATYPE_NONE;
	m_Format		= FF_UNKNOWN;
	m_bSigned		= false;
	mutex.lock();
    m_lWidth = 0;
    m_lHeight = 0;
	m_lBitsPerPixel = 0;
    m_lNrChannels = 0;
	m_xBayerOffset = 0;
	m_yBayerOffset = 0;
	m_bitPix = 0;
	m_nrframes = 0;	
};

/* ------------------------------------------------------------------- */

CFITSHeader::~CFITSHeader()
{
	mutex.unlock();
};

/* ------------------------------------------------------------------- */

inline double AdjustColor(const double fColor)
{
	//
	// Clamping is now done by the bitmap classes
	// 
	if (std::isfinite(fColor))
		return fColor;	// Was return std::clamp(fColor, 0.0, 255.0);
	else
		return 0.0;
};

/* ------------------------------------------------------------------- */

bool IsFITSRaw()
{
	return Workspace{}.value("FitsDDP/FITSisRAW", false).toBool();
}

CFATYPE GetFITSCFATYPE()
{
	Workspace workspace{};

	const bool isFitsRaw = workspace.value("FitsDDP/FITSisRAW", false).toBool();
	const CFATYPE pattern = static_cast<CFATYPE>(workspace.value("FitsDDP/BayerPattern", (uint)CFATYPE_NONE).toUInt());
	return isFitsRaw ? pattern : CFATYPE_NONE;
}

bool IsFITSRawBayer()
{
	return Workspace{}.value("RawDDP/RawBayer", false).toBool();
}

bool IsFITSSuperPixels()
{
	return Workspace{}.value("RawDDP/SuperPixels", false).toBool();
}

bool IsFITSBilinear()
{
	const QString interpolation = Workspace{}.value("RawDDP/Interpolation").toString();
	return (0 == interpolation.compare("Bilinear", Qt::CaseInsensitive));
}

bool IsFITSAHD()
{
	const QString interpolation = Workspace{}.value("RawDDP/Interpolation").toString();
	return (0 == interpolation.compare("AHD", Qt::CaseInsensitive));
}

double GetFITSBrightnessRatio()
{
	return Workspace{}.value("FitsDDP/Brightness", 1.0).toDouble();
}

void GetFITSRatio(double& fRed, double& fGreen, double& fBlue)
{
	Workspace workspace{};

	fGreen = workspace.value("FitsDDP/Brightness", 1.0).toDouble();
	fRed = fGreen * workspace.value("FitsDDP/RedScale", 1.0).toDouble();
	fBlue = fGreen * workspace.value("FitsDDP/BlueScale", 1.0).toDouble();
}


bool CFITSReader::ReadKey(const char * szKey, double& fValue, QString& strComment)
{
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		char szComment[500];

		fits_read_key(m_fits, TDOUBLE, szKey, &fValue, szComment, &nStatus);
		if (!nStatus)
		{
			bResult = true;
			strComment = QString::fromLatin1(szComment);
		};
	};

	return bResult;
};

bool CFITSReader::ReadKey(const char * szKey, double & fValue)
{
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_read_key(m_fits, TDOUBLE, szKey, &fValue, nullptr, &nStatus);
		if (!nStatus)
			bResult = true;
	};

	return bResult;
};

bool CFITSReader::ReadKey(const char * szKey, int& lValue)
{
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_read_key(m_fits, TLONG, szKey, &lValue, nullptr, &nStatus);
		if (!nStatus)
			bResult = true;
	};

	return bResult;
};

bool CFITSReader::ReadKey(const char * szKey, QString & strValue)
{
	bool				bResult = false;
	char				szValue[2000];
	int					nStatus = 0;

	if (m_fits)
	{
		fits_read_key(m_fits, TSTRING, szKey, szValue, nullptr, &nStatus);
		if (!nStatus)
		{
			strValue = QString::fromLatin1(szValue);
			bResult = true;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CFITSReader::ReadAllKeys()
{
	if (m_fits)
	{
		int nKeywords;
		int nStatus = 0;
		QSettings settings;
		QString strPropagated(settings.value("FitsDDP/Propagated", "").toString());

		if (strPropagated.isEmpty())
			strPropagated = "[DATE-OBS][CRVAL1][CRVAL2][CRTYPE1][CRTYPE2][DEC][RA][OBJCTDEC][OBJCTRA][OBJCTALT][OBJCTAZ][OBJCTHA][SITELAT][SITELONG][TELESCOP][INSTRUME][OBSERVER][RADECSYS]";


		fits_get_hdrspace(m_fits, &nKeywords, nullptr, &nStatus);
		for (int i = 1;i<=nKeywords;i++)
		{
			char szKeyName[FLEN_CARD];
			char szValue[FLEN_VALUE];
			char szComment[FLEN_COMMENT];
			int				nKeyClass;

			fits_read_keyn(m_fits, i, szKeyName, szValue, szComment, &nStatus);
			nKeyClass = fits_get_keyclass(szKeyName);
			if ((nKeyClass == TYP_USER_KEY) || (nKeyClass == TYP_REFSYS_KEY))
			{
				bool bPropagate = false;
				QString strKeyName;
				strKeyName = QString("[%1]").arg(szKeyName);

				if (strPropagated.indexOf(strKeyName) != -1)
					bPropagate = true;
				m_ExtraInfo.AddInfo(
					szKeyName,
					szValue,
					szComment, bPropagate);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

bool CFITSReader::Open()
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
	int					status = 0;
	char error_text[31] = "";			// Error text for FITS errors.

	// Open filename as ascii - won't work with international characters but we are limited to char with fits library.
	fits_open_diskfile(&m_fits, reinterpret_cast<const char*>(file.generic_u8string().c_str()), READONLY, &status);
	if (0 != status)
	{
		fits_get_errstatus(status, error_text);
		QString errorMessage(QCoreApplication::translate("FITSUtil",
														  "fits_open_diskfile %1\nreturned a status of %2, error text is:\n\"%3\"")
															.arg(file.generic_u16string().c_str())
															.arg(status)
															.arg(error_text));
		ZTRACE_RUNTIME(errorMessage);
		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Warning);

	}


	if (m_fits)
	{
		ZTRACE_RUNTIME("Opened %s", file.generic_u8string().c_str());

		// File ok - move to the first image HDU
		QString			strSimple;
		int			lNrAxis = 0;
		int			lWidth  = 0,
						lHeight = 0,
						lNrChannels = 0;
		double			fExposureTime = 0;
		QString			strMake;
		QString			strISOSpeed;
		QString			CFAPattern("");
		QString			filterName("");
		int			lISOSpeed = 0;
		int			lGain = -1;
		double			xBayerOffset = 0.0, yBayerOffset = 0.0;

		m_bDSI = false;

		bResult = ReadKey("SIMPLE", strSimple);
		bResult = ReadKey("NAXIS", lNrAxis);
		if ((strSimple == "T") && (lNrAxis >= 2 && lNrAxis <= 3))
		{
			QString				strComment;
			ReadAllKeys();

			bResult = ReadKey("INSTRUME", strMake);

			//
			// Attempt to get the exposure time
			//
			bResult = ReadKey("EXPTIME", fExposureTime, strComment);
			if (!bResult)
				bResult = ReadKey("EXPOSURE", fExposureTime, strComment);

			if (bResult && !strComment.isEmpty())
			{
				if ((strComment.indexOf("in seconds")<0) &&
					((strComment.indexOf("ms")>0) || (fExposureTime>3600)))
				{
					// Exposure time is most certainly in ms
					fExposureTime /= 1000.0;
				};
			};
			// 
			// Just to be awkward, some software (e.g. ZWO ASICAP) supplies the exposure
			// time in microseconds!
			//
			if (ReadKey("EXPOINUS", fExposureTime))
			{
				fExposureTime /= 1000000.0;
			}

			//
			// Number of frames in stack
			//
			bResult = ReadKey("NCOMBINE", m_nrframes);

			bResult = ReadKey("ISOSPEED", strISOSpeed);
			if (bResult)
			{

				if (strISOSpeed.startsWith("ISO"))
				{
					strISOSpeed.remove("ISO"); strISOSpeed = strISOSpeed.trimmed();
					lISOSpeed = strISOSpeed.toInt();
				};
			};

			bResult = ReadKey("GAIN", lGain);

			bResult = ReadKey("FILTER", filterName);

			bResult = ReadKey("NAXIS1", lWidth);
			bResult = ReadKey("NAXIS2", lHeight);
			if (lNrAxis>=3)
				bResult = ReadKey("NAXIS3", lNrChannels);
			else
				lNrChannels = 1;
			bResult = ReadKey("BITPIX", m_bitPix);

			//
			// One time action to create a mapping between the character name of the CFA
			// pattern and our internal CFA type 
			// 
			static std::map<QString, CFATYPE> bayerMap {
				{ "BGGR", CFATYPE_BGGR },
				{ "GRBG", CFATYPE_GRBG },
				{ "GBRG", CFATYPE_GBRG },
				{ "RGGB", CFATYPE_RGGB },

				{ "CGMY", CFATYPE_CGMY },
				{ "CGYM", CFATYPE_CGYM },
				{ "CMGY", CFATYPE_CMGY },
				{ "CMYG", CFATYPE_CMYG },
				{ "CYMG", CFATYPE_CYMG },
				{ "CYGM", CFATYPE_CYGM },

				{ "GCMY", CFATYPE_GCMY },
				{ "GCYM", CFATYPE_GCYM },
				{ "GMCY", CFATYPE_GMCY },
				{ "GMYC", CFATYPE_GMYC },
				{ "GYCM", CFATYPE_GYCM },
				{ "GYMC", CFATYPE_GYMC },

				{ "MCGY", CFATYPE_MCGY },
				{ "MCYG", CFATYPE_MCYG },
				{ "MGYC", CFATYPE_MGYC },
				{ "MGCY", CFATYPE_MGCY },
				{ "MYGC", CFATYPE_MYGC },
				{ "MYCG", CFATYPE_MYCG },

				{ "YCGM", CFATYPE_YCGM },
				{ "YCMG", CFATYPE_YCMG },
				{ "YGMC", CFATYPE_YGMC },
				{ "YGCM", CFATYPE_YGCM },
				{ "YMCG", CFATYPE_YMCG },
				{ "YMGC", CFATYPE_YMGC },

				{ "CYGMCYMG", CFATYPE_CYGMCYMG },
				{ "GMCYMGCY", CFATYPE_GMCYMGCY },
				{ "CYMGCYGM", CFATYPE_CYMGCYGM },
				{ "MGCYGMCY", CFATYPE_MGCYGMCY },
				{ "GMYCGMCY", CFATYPE_GMYCGMCY },
				{ "YCGMCYGM", CFATYPE_YCGMCYGM },
				{ "GMCYGMYC", CFATYPE_GMCYGMYC },
				{ "CYGMYCGM", CFATYPE_CYGMYCGM },
				{ "YCGMYCMG", CFATYPE_YCGMYCMG },
				{ "GMYCMGYC", CFATYPE_GMYCMGYC },
				{ "YCMGYCGM", CFATYPE_YCMGYCGM },
				{ "MGYCGMYC", CFATYPE_MGYCGMYC },
				{ "MGYCMGCY", CFATYPE_MGYCMGCY },
				{ "YCMGCYMG", CFATYPE_YCMGCYMG },
				{ "MGCYMGYC", CFATYPE_MGCYMGYC },
				{ "CYMGYCMG", CFATYPE_CYMGYCMG }
			};

			//
			// Attempt to determine the correct CFA (aka Bayer matrix) from keywords in the FITS header.
			// 
			// Some Meade DSI cameras used the keyword MOSAIC with a value of "CMYG", when in fact the actual
			// matrix used was CYGMCYMG. so that is special cased.
			// 
			if (ReadKey("MOSAIC", CFAPattern) && strMake.startsWith("DSI"))
			{
				ZTRACE_RUNTIME("CFA Pattern read from FITS keyword MOSAIC is %s", CFAPattern.toUtf8().constData());

				m_bDSI = true;
				// Special case of DSI FITS files
				CFAPattern = CFAPattern.trimmed();
				if (CFAPattern == "CMYG")
					m_CFAType = CFATYPE_CYGMCYMG;
			} 
			//
			// For everything else we attempt to use the BAYERPAT or COLORTYP keywords to establish
			// the correct CFA (Bayer) pattern.
			//
			else
			{
				//
				// If BAYERPAT keyword not found try to read COLORTYP, but only if BAYERPAT isn't found
				// as BAYERPAT has precedence.
				//
				if (ReadKey("BAYERPAT", CFAPattern) || ReadKey("COLORTYP", CFAPattern))
				{
					ZTRACE_RUNTIME("CFA Pattern read from FITS keyword BAYERPAT or COLORTYP is %s", CFAPattern.toUtf8().constData());
				}

				CFAPattern = CFAPattern.trimmed();

				if (!CFAPattern.isEmpty())
				{
					auto it = bayerMap.find(CFAPattern);
					if (bayerMap.end() != it) m_CFAType = it->second;
				}
			}

			//
			// Now extract the "Bayer" matrix offset values:
			// XBAYROFF (BAYOFFX) and 
			// YBAYROFF (BAYOFFY) values 
			//
			// The non-bracketed keywords have precedence if both are present
			// Both are zero unless set ...
			//
			if (ReadKey("XBAYROFF", xBayerOffset) || ReadKey("BAYOFFX", xBayerOffset))
			{
				ZTRACE_RUNTIME("CFA pattern X offset read from keyword XBAYROFF or BAYOFFX is %lf", xBayerOffset);
			}
			if (ReadKey("YBAYROFF", yBayerOffset) || ReadKey("BAYOFFY", yBayerOffset))
			{
				ZTRACE_RUNTIME("CFA pattern Y offset read from keyword YBAYROFF or BAYOFFY is %lf", yBayerOffset);
			}
			m_xBayerOffset = std::lround(xBayerOffset);
			m_yBayerOffset = std::lround(yBayerOffset);

			QString			strDateTime;

			if (ReadKey("DATE-OBS", strDateTime))
			{
				// Decode 2007-11-02T22:07:03.890
				//        01234567890123456789012
				QString				strTime;
				qsizetype length{ strDateTime.length() };

				if (19 == length)	// No trailing milli-seconds
				{
					m_DateTime = QDateTime::fromString(strDateTime, "yyyy-MM-ddThh:mm:ss");
				}
				else if (length > 19)	// Probably has trailing milli-seconds
				{
					m_DateTime = QDateTime::fromString(strDateTime, "yyyy-MM-ddThh:mm:ss.z");
				}
				else if ((strDateTime.length() == 8) && ReadKey("TIME-OBS", strTime))
				{
					// Decode dd/mm/yy
					//        01234567
					QDate date = QDate::fromString(strDateTime, "dd/MM/yy");
					if (date.year() < 70)
						date = date.addYears(2000);
					else
						date = date.addYears(1900);

					length = strTime.length();
					if (length >= 8)
					{
						QTime time;
						if (8 == length)
						{
							// Decode hh:mm:ss
							//        01234567
							time = QTime::fromString(strTime, "hh:mm:ss");
						}
						else
						{
							// Decode hh:mm:ss.xxx
							//        01234567
							time = QTime::fromString(strTime, "hh:mm:ss.z");
						}
						if (time.isValid())
						{
							m_DateTime = QDateTime(date, time);
						}
					};
				};
			};

			//
			// Before setting everything up for reading the data, there's a couple of special cases
			// where we need to treat a signed data file as if it were unsigned.
			//
			//double		fZero,	fScale;

			//if (ReadKey("BZERO", fZero) && ReadKey("BSCALE", fScale))
			//{
			//	if ((SHORT_IMG == lBitFormat) && (32768.0 == fZero) && (1.0 == fScale))
			//		lBitFormat = USHORT_IMG;
			//	if ((LONG_IMG == lBitFormat) && (2147483648.0 == fZero) && (1.0 == fScale))
			//		lBitFormat = ULONG_IMG;
			//}

			if (bResult)
			{
				m_lWidth	= lWidth;
				m_lHeight	= lHeight;
				m_lNrChannels = lNrChannels;
				m_fExposureTime = fExposureTime;
				m_strMake	= strMake;
				m_lISOSpeed = lISOSpeed;
				m_lGain     = lGain;
				m_bSigned = false;
				switch (m_bitPix)
				{
				case BYTE_IMG :
					m_lBitsPerPixel = 8;
					m_bFloat = false;
					break;
				case SHORT_IMG :
					m_bSigned = true;  // Fall through intentional
				case USHORT_IMG :
					m_lBitsPerPixel = 16;
					m_bFloat = false;
					break;
				case LONG_IMG :
					m_bSigned = true; // Fall through intentional
				case ULONG_IMG :
					m_lBitsPerPixel = 32;
					m_bFloat = false;
					break;
				case FLOAT_IMG :
					m_lBitsPerPixel = 32;
					m_bFloat = true;
					break;
				case DOUBLE_IMG :
					m_lBitsPerPixel = 64;
					m_bFloat = true;
					break;
				case LONGLONG_IMG :
					m_lBitsPerPixel = 64;
					m_bFloat = false;
					break;
				default :
					// Format not recognized
					bResult = false;
					break;
				};
				m_filterName = filterName;
			};
			
			//
			// If the user has explicitly set that this FITS file is a Bayer format RAW file,
			// then the user will also have explicitly set the Bayer pattern that's to be used.
			// In this case we use that and set the Bayer offsets (if any) to zero.
			// We do this only for 16 bits per pixel data.
			//
			if (m_lNrChannels == 1 && m_lBitsPerPixel == 16 && IsFITSRaw())
			{
				ZTRACE_RUNTIME("Using user supplied override for Bayer pattern.");
				m_CFAType = GetFITSCFATYPE();
				m_xBayerOffset = 0;
				m_yBayerOffset = 0;
				//
				// If user hasn't said it's a FITS format RAW, then use the CFA we've already
				// retrieved from the FITS header if set.
				//				
			}
		}
		else
			bResult = false;

		if (bResult)
			bResult = OnOpen();
		if (!bResult)
		{
			if (m_fits)
			{
				fits_close_file(m_fits, &status);
				m_fits = nullptr;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CFITSReader::Read()
{
	constexpr double scaleFactorInt16 = 1.0 + std::numeric_limits<std::uint8_t>::max();
	constexpr double scaleFactorInt32 = scaleFactorInt16 * (1.0 + std::numeric_limits<std::uint16_t>::max());

	ZFUNCTRACE_RUNTIME();
	bool result = true;
	char error_text[31] = "";			// Error text for FITS errors.
	
	const int colours = (m_lNrChannels >= 3) ? 3 : 1;		// 3 ==> RGB, 1 ==> Mono

	double fMin = 0.0, fMax = 0.0;		// minimum and maximum pixel values for floating point images

	if (m_lNrChannels > 3)
		ZTRACE_RUNTIME("Number of colour channels is %d, only 3 will be used.", m_lNrChannels);

	if (m_fits != nullptr)
	{
		double dNULL = 0;

		if (m_pProgress)
			m_pProgress->Start2(m_lHeight);

		LONGLONG fPixel[3] = { 1, 1, 1 };		// want to start reading at column 1, row 1, plane 1

		ZTRACE_RUNTIME("FITS colours=%d, bps=%d, w=%d, h=%d", colours, m_lBitsPerPixel, m_lWidth, m_lHeight);

		const LONGLONG nElements = static_cast<LONGLONG>(m_lWidth) * m_lHeight * colours;
		auto buff = std::make_unique<double[]>(nElements);
		double* const doubleBuff = buff.get();
		int status = 0;			// used for result of fits_read_pixll call

		//
		// Inhale the entire image (either single colour or RGB) as an array of doubles
		//
		fits_read_pixll(m_fits, TDOUBLE, fPixel, nElements, &dNULL, doubleBuff, nullptr, &status);
		if (0 != status)
		{
			fits_get_errstatus(status, error_text);
			const QString errMsg(QString("fits_read_pixll returned a status of %1, error text is \"%2\"").arg(status).arg(error_text));
			ZException exc(errMsg.toLatin1().constData(), status, ZException::unrecoverable);
			exc.addLocation(ZEXCEPTION_LOCATION());
			exc.logExceptionData();
			throw exc;
		}

		const int nrProcessors = CMultitask::GetNrProcessors(); // Returns 1, if the user de-selected multi-threading, # CPUs else.

		//
		// Step 1: If the image is in float format, need to extract the minimum and maximum pixel values.
		//
		if (m_bFloat)
		{
			double localMin = 0, localMax = 0;
#pragma omp parallel default(shared) shared(fMin, fMax, nElements, doubleBuff) firstprivate(localMin, localMax) if(nrProcessors > 1)
			{
#pragma omp for schedule(dynamic, 100'000)
				for (std::int64_t element = 0; element < nElements; ++element)
				{
					const double fValue = doubleBuff[element];	// int (8 byte) floating point
					if (!std::isnan(fValue))
					{
						localMin = std::min(localMin, fValue);
						localMax = std::max(localMax, fValue);
					};
				}
#pragma omp critical
				{
					fMin = std::min(localMin, fMin); // For non-OMP case this is equal to fMin = localMin
					fMax = std::max(localMax, fMax); // For non-OMP case this is equal to fMax = localMax
				}
			}
			double		fZero, fScale;

			if (ReadKey("BZERO", fZero) && ReadKey("BSCALE", fScale))
			{
				fMax = (fMax + fZero) / fScale;
				fMin = fZero / fScale;
			}
			else if (fMin >= 0 && fMin <= 1 && fMax >= 0 && fMax <= 1)
			{
				fMin = 0;
				fMax = 1;
			}
			if (m_bDSI && (fMax > 1))
			{
				fMin = min(0.0, fMin);
				fMax = max(fMax, 65535.0);
			};

		}
		else
		{
			if (!ReadKey("BZERO", fMin))
				fMin = 0;
		};

		//
		// Step 2: Process the image pixels
		//
		ptrdiff_t greenOffset = ptrdiff_t{ m_lWidth } * m_lHeight;		// index into buffer of the green image
		ptrdiff_t blueOffset = 2 * greenOffset;				// index into buffer of the blue image

		int	rowProgress = 0;

		const auto normalizeFloatValue = [fMin, fMax](const double value) -> double
		{
			constexpr double scaleFactor = static_cast<double>(std::numeric_limits<std::uint16_t>::max()) / 256.0;
			const double normalizationFactor = scaleFactor / (fMax - fMin);
			return (value - fMin) * normalizationFactor;
		};

		std::atomic_bool stop{ false };

#pragma omp parallel for default(shared) schedule(guided, 50) if(nrProcessors > 1)
		for (int row = 0; row < m_lHeight; ++row)
		{
			if (stop.load()) continue; // This is the only way we can "escape" from OPENMP loops. An early break is impossible.

			for (int col = 0; col < m_lWidth; ++col)
			{
				if (stop.load()) break;	// OK to break from inner loop

				double fRed = 0.0, fGreen = 0.0, fBlue = 0.0;
				const int index = col + (row * m_lWidth);	// index into the image for this plane

				if (1 == colours)
				{
					//
					// This is a monochrome image 
					//
					fRed = fGreen = fBlue = doubleBuff[index];
				}
				else
				{
					//
					// We assume this is a 3 colour image with each colour in a separate image plane
					//
					fRed = doubleBuff[index];
					fGreen = doubleBuff[greenOffset + index];
					fBlue = doubleBuff[blueOffset + index];
				}

				switch (m_bitPix)
				{
				case BYTE_IMG:
					break;
				case SHORT_IMG:
				case USHORT_IMG:
					fRed /= scaleFactorInt16;
					fGreen /= scaleFactorInt16;
					fBlue /= scaleFactorInt16;
					break;
				case LONG_IMG:
				case ULONG_IMG:
					fRed /= scaleFactorInt32;
					fGreen /= scaleFactorInt32;
					fBlue /= scaleFactorInt32;
					break;
				case LONGLONG_IMG:
					fRed /= scaleFactorInt32;
					fGreen /= scaleFactorInt32;
					fBlue /= scaleFactorInt32;
					break;
				case FLOAT_IMG:
				case DOUBLE_IMG:
					fRed = normalizeFloatValue(fRed);
					fGreen = normalizeFloatValue(fGreen);
					fBlue = normalizeFloatValue(fBlue);
					break;
				}

				if (!OnRead(col, row, AdjustColor(fRed), AdjustColor(fGreen), AdjustColor(fBlue)))
				{
					stop = true;
					result = false;
				}
			}

			if (m_pProgress != nullptr && 0 == omp_get_thread_num() && (rowProgress++ % 25) == 0)	// Are we on the master thread?
				m_pProgress->Progress2(row);
		}
		if (m_pProgress)
			m_pProgress->End2();
	}
	return result;
};

/* ------------------------------------------------------------------- */

bool CFITSReader::Close()
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		bResult = OnClose();
		if (bResult)
		{
			fits_close_file(m_fits, &nStatus);
			m_fits = nullptr;
		};
	};

	return bResult;
}


class CFITSReadInMemoryBitmap : public CFITSReader
{
private :
	std::shared_ptr<CMemoryBitmap>& m_outBitmap;
	std::shared_ptr<CMemoryBitmap> m_pBitmap;
	bool ignoreBrightness;

public :
	CFITSReadInMemoryBitmap(const fs::path& szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBr, ProgressBase* pProgress) :
		CFITSReader{ szFileName, pProgress },
		m_outBitmap{ rpBitmap },
		ignoreBrightness{ ignoreBr }
	{}

	virtual ~CFITSReadInMemoryBitmap() { Close(); };

	virtual bool Close() override { return OnClose(); };

	virtual bool OnOpen() override;
	virtual bool OnRead(int lX, int lY, double fRed, double fGreen, double fBlue) override;
	virtual bool OnClose() override;
};

/* ------------------------------------------------------------------- */

bool CFITSReadInMemoryBitmap::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (m_lNrChannels == 1)
	{
		if (m_lBitsPerPixel == 8)
		{
			m_pBitmap = std::make_shared<C8BitGrayBitmap>();
			ZTRACE_RUNTIME("Creating 8 Gray bit memory bitmap %p", m_pBitmap.get());
		}
		else if (m_lBitsPerPixel == 16)
		{
			m_pBitmap = std::make_shared<C16BitGrayBitmap>();
			ZTRACE_RUNTIME("Creating 16 Gray bit memory bitmap %p", m_pBitmap.get());
		}
		else if (m_lBitsPerPixel == 32 || m_lBitsPerPixel == 64)
		{
			if (m_bFloat)
			{
				m_pBitmap = std::make_shared<C32BitFloatGrayBitmap>();
				ZTRACE_RUNTIME("Creating 32 float Gray bit memory bitmap %p", m_pBitmap.get());
			}
			else
			{
				m_pBitmap = std::make_shared<C32BitGrayBitmap>();
				ZTRACE_RUNTIME("Creating 32 Gray bit memory bitmap %p", m_pBitmap.get());
			};
		};
	}
	else if (m_lNrChannels==3)
	{
		if (m_lBitsPerPixel == 8)
		{
			m_pBitmap = std::make_shared<C24BitColorBitmap>();
			ZTRACE_RUNTIME("Creating 8 RGB bit memory bitmap %p", m_pBitmap.get());
		}
		else if (m_lBitsPerPixel == 16)
		{
			m_pBitmap = std::make_shared<C48BitColorBitmap>();
			ZTRACE_RUNTIME("Creating 16 RGB bit memory bitmap %p", m_pBitmap.get());
		}
		else if (m_lBitsPerPixel == 32 || m_lBitsPerPixel == 64)
		{
			if (m_bFloat)
			{
				m_pBitmap = std::make_shared<C96BitFloatColorBitmap>();
				ZTRACE_RUNTIME("Creating 32 float RGB bit memory bitmap %p", m_pBitmap.get());
			}
			else
			{
				m_pBitmap = std::make_shared<C96BitColorBitmap>();
				ZTRACE_RUNTIME("Creating 32 RGB bit memory bitmap %p", m_pBitmap.get());
			};
		};
	};

	if (static_cast<bool>(m_pBitmap))
	{
		bResult = m_pBitmap->Init(m_lWidth, m_lHeight);

		if ((m_CFAType != CFATYPE_NONE) && (m_lNrChannels != 1))// || (m_lBitsPerPixel != 16)))
			m_CFAType = CFATYPE_NONE;

		//
		// If this file is an eight bit FITS, and purports to have a Bayer pattern (or the user has
		// explicitly specifed one), inform the the user that we aren't going to play
		//
		if ((1 == m_lNrChannels) &&
			(8 == m_lBitsPerPixel) &&
			(CFATYPE_NONE != m_CFAType))
		{
			// 
			// Set CFA type to none even if the FITS header specified a value
			//
			m_CFAType = CFATYPE_NONE;
			const QString errorMessage{ QCoreApplication::translate("FITSUtil",
									"DeepSkyStacker will not de-Bayer 8 bit images",
									"IDS_8BIT_FITS_NODEBAYER") };
			DSSBase::instance()->reportError(
				errorMessage,
				"Will not de-Bayer 8 bit images",
				DSSBase::Severity::Warning,
				DSSBase::Method::QErrorMessage);
		}

		if (m_CFAType != CFATYPE_NONE)
		{
			if (CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(m_pBitmap.get()))
			{
				m_pBitmap->SetCFA(true);
				pCFABitmapInfo->SetCFAType(m_CFAType);
				//
				// Set the CFA/Bayer offset information into the CFABitmapInfo
				//
				pCFABitmapInfo->setXoffset(m_xBayerOffset);
				pCFABitmapInfo->setYoffset(m_yBayerOffset);
				if (::IsCYMGType(m_CFAType))
					pCFABitmapInfo->UseBilinear(true);
				else if (IsFITSRawBayer())
					pCFABitmapInfo->UseRawBayer(true);
				else if (IsSuperPixels())			// Was IsFITSSuperPixels()
					pCFABitmapInfo->UseSuperPixels(true);
				else if (IsFITSBilinear())
					pCFABitmapInfo->UseBilinear(true);
				else if (IsFITSAHD())
					pCFABitmapInfo->UseAHD(true);

				// Retrieve ratios
				if (!this->ignoreBrightness)
					GetFITSRatio(m_fRedRatio, m_fGreenRatio, m_fBlueRatio);
			}
		}
		else
			m_fBrightnessRatio = this->ignoreBrightness ? 1.0 : GetFITSBrightnessRatio();

		m_pBitmap->SetMaster(false);
		if (m_fExposureTime)
			m_pBitmap->SetExposure(m_fExposureTime);
		if (m_lISOSpeed)
			m_pBitmap->SetISOSpeed(m_lISOSpeed);
		if (m_lGain >= 0)
			m_pBitmap->SetGain(m_lGain);
		m_pBitmap->setFilterName(m_filterName);
		m_pBitmap->m_DateTime = m_DateTime;

		QString strDescription;
		if (!m_strMake.isEmpty())
			strDescription = QString("FITS (%1)").arg(m_strMake);
		else
			strDescription = "FITS";

		m_pBitmap->SetDescription(strDescription);
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CFITSReadInMemoryBitmap::OnRead(int lX, int lY, double fRed, double fGreen, double fBlue)
{
	//
	// Define maximal scaled pixel value of 255 (will be multiplied up later)
	//
	constexpr double maxValue = 255.0;
	bool result = true;

	try
	{
		if (static_cast<bool>(m_pBitmap))
		{
			if (m_lNrChannels == 1)
			{
				if (m_CFAType != CFATYPE_NONE)
				{
					switch (::GetBayerColor(lX, lY, m_CFAType, m_xBayerOffset, m_yBayerOffset))
					{
					case BAYER_BLUE:
						fRed = std::min(maxValue, fRed * m_fBlueRatio);
						break;
					case BAYER_GREEN:
						fRed = std::min(maxValue, fRed * m_fGreenRatio);
						break;
					case BAYER_RED:
						fRed = std::min(maxValue, fRed * m_fRedRatio);
						break;
					}
				}
				else
				{
					fRed = std::min(maxValue, fRed * m_fBrightnessRatio);
					fGreen = std::min(maxValue, fGreen * m_fBrightnessRatio);
					fBlue = std::min(maxValue, fBlue * m_fBrightnessRatio);
				}
				m_pBitmap->SetPixel(lX, lY, fRed);
			}
			else
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
		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Critical);
		result = false;
	}
	return result;
};

/* ------------------------------------------------------------------- */

bool CFITSReadInMemoryBitmap::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	if (static_cast<bool>(m_pBitmap))
	{
		m_pBitmap->m_ExtraInfo = m_ExtraInfo;
		m_outBitmap = m_pBitmap;
		return true;
	}
	else
		return false;
}


bool ReadFITS(const fs::path& szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CFITSReadInMemoryBitmap	fitsReader{ szFileName, rpBitmap, ignoreBrightness, pProgress };
	return fitsReader.Open() && fitsReader.Read();
}


bool GetFITSInfo(const fs::path& path, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool					bResult = false;
	bool					bContinue = true;
	CFITSReader				fits(path, nullptr);

	// Require a fits file extension
	{
		//TCHAR szExt[1+_MAX_EXT];
		const QString strFilename(QString::fromStdU16String(path.generic_u16string().c_str()));
		const QString strExt = QFileInfo(strFilename).suffix();

		if (!(0 == strExt.compare("FIT", Qt::CaseInsensitive) ||
			  0 == strExt.compare("FITS", Qt::CaseInsensitive) ||
			  0 == strExt.compare("FTS", Qt::CaseInsensitive)))
		{
			bContinue = false;
		}
	}
	if (bContinue && fits.Open())
	{
		if (fits.m_strMake.length() != 0) 
			BitmapInfo.m_strFileType = QString("FITS (%1)").arg(fits.m_strMake);
		else 
			BitmapInfo.m_strFileType = "FITS";

		BitmapInfo.m_strFileName = path;
		BitmapInfo.m_lWidth			= fits.Width();
		BitmapInfo.m_lHeight		= fits.Height();
		BitmapInfo.m_lBitsPerChannel = fits.BitPerChannels();
		BitmapInfo.m_lNrChannels	= fits.NrChannels();
		BitmapInfo.m_bFloat			= fits.IsFloat();
		BitmapInfo.m_CFAType		= fits.GetCFAType();
		BitmapInfo.m_bMaster		= fits.IsMaster();
		BitmapInfo.m_lISOSpeed		= fits.GetISOSpeed();
		BitmapInfo.m_lGain			= fits.GetGain();
		BitmapInfo.m_bCanLoad		= true;
		BitmapInfo.m_fExposure		= fits.GetExposureTime();
		BitmapInfo.m_bFITS16bit	    = (fits.NrChannels() == 1) &&
									  ((fits.BitPerChannels() == 16) || (fits.BitPerChannels() == 32));
		BitmapInfo.m_DateTime		= fits.GetDateTime();
		BitmapInfo.m_ExtraInfo		= fits.m_ExtraInfo;
		BitmapInfo.m_xBayerOffset	= fits.getXOffset();
		BitmapInfo.m_yBayerOffset	= fits.getYOffset();
		BitmapInfo.m_filterName		= fits.m_filterName;
		bResult = true;
	}

	return bResult;
}


bool CFITSWriter::WriteKey(const char * szKey, double fValue, const char * szComment)
{
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_write_key(m_fits, TDOUBLE, szKey, &fValue, szComment, &nStatus);
		if (!nStatus)
			bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CFITSWriter::WriteKey(const char * szKey, int lValue, const char * szComment)
{
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_write_key(m_fits, TLONG, szKey, &lValue, szComment, &nStatus);
		if (!nStatus)
			bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CFITSWriter::WriteKey(const char * szKey, const QString& szValue, const char * szComment)
{
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_write_key(m_fits, TSTRING, szKey, (void*)szValue.toUtf8().constData(), szComment, &nStatus);
		if (!nStatus)
			bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CFITSWriter::WriteAllKeys()
{
	bool bFound = false;

	// Check if DATE-OBS is already in the list of Extra Info
	for (const ExtraInfo& extraInfo : m_ExtraInfo.m_vExtras)
	{
		if (extraInfo.m_strName.compare("DATE-OBS", Qt::CaseInsensitive) == 0)
		{
			bFound = true;
			break;
		}
	};

	if (!bFound && m_DateTime.isValid())
	{
		// Add DATE-OBS to the list
		QString	strDateTime{ m_DateTime.toString("yyyy-MM-ddThh:mm:ss") };


		m_ExtraInfo.AddInfo("DATE-OBS", strDateTime, "");
	};

	if (m_fits && m_ExtraInfo.m_vExtras.size())
	{
		int				nStatus = 0;


		for (int i = 0;i<m_ExtraInfo.m_vExtras.size();i++)
		{
			const ExtraInfo &ei = m_ExtraInfo.m_vExtras[i];
			char szValue[FLEN_VALUE];

			// check that the keyword is not already used
			fits_read_key(m_fits, TSTRING, ei.m_strName.toUtf8().constData(), szValue, nullptr, &nStatus);
			if (nStatus)
			{
				nStatus = 0;
				char		szCard[FLEN_CARD];
				int			nType;
				QString		strTemplate;

				if (!ei.m_strComment.isEmpty())
					strTemplate = QString("%1 = %2 / %3")
						.arg(ei.m_strName)
						.arg(ei.m_strValue)
						.arg(ei.m_strComment);
				else
					strTemplate = QString("%1 = %2")
					.arg(ei.m_strName)
					.arg(ei.m_strValue);

				fits_parse_template(strTemplate.toUtf8().data(), szCard, &nType, &nStatus);
				fits_write_record(m_fits, szCard, &nStatus);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CFITSWriter::SetFormat(int lWidth, int lHeight, FITSFORMAT FITSFormat, CFATYPE CFAType)
{
	ZFUNCTRACE_RUNTIME();
	m_CFAType	= CFAType;

	m_lWidth	= lWidth;
	m_lHeight	= lHeight;
	m_bFloat	= false;
	switch (FITSFormat)
	{
	case FF_8BITRGB	:
		m_lNrChannels = 3;
		m_lBitsPerPixel = 8;
		break;
	case FF_16BITRGB :
		m_lNrChannels = 3;
		m_lBitsPerPixel = 16;
		break;
	case FF_32BITRGB :
		m_lNrChannels = 3;
		m_lBitsPerPixel = 32;
		break;
	case FF_32BITRGBFLOAT :
		m_lNrChannels = 3;
		m_lBitsPerPixel = 32;
		m_bFloat = true;
		break;
	case FF_8BITGRAY :
		m_lNrChannels = 1;
		m_lBitsPerPixel = 8;
		break;
	case FF_16BITGRAY :
		m_lNrChannels = 1;
		m_lBitsPerPixel = 16;
		break;
	case FF_32BITGRAY :
		m_lNrChannels = 1;
		m_lBitsPerPixel = 32;
		break;
	case FF_32BITGRAYFLOAT :
		m_lNrChannels = 1;
		m_lBitsPerPixel = 32;
		m_bFloat = true;
		break;
	};
};

/* ------------------------------------------------------------------- */

bool CFITSWriter::Open()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

	// Create a new fits file
	int				nStatus = 0;

	fs::remove(file);
	fits_create_diskfile(&m_fits, reinterpret_cast<const char*>(file.generic_u8string().c_str()), &nStatus);
	if (m_fits && nStatus == 0)
	{
		bResult = OnOpen();
		if (bResult)
		{
			// Create the image
			long nAxes[3];
			int	 nAxis;
			int	 nBitPixels;

			nAxes[0] = m_lWidth;
			nAxes[1] = m_lHeight;
			nAxes[2] = 3;

			if (m_lNrChannels==1)
				nAxis = 2;
			else
				nAxis = 3;

			nBitPixels = m_lBitsPerPixel;
			if (nBitPixels == 16)
				nBitPixels = USHORT_IMG;
			else if (nBitPixels == 32)
			{
				if (m_bFloat)
					nBitPixels = -nBitPixels;	// Negative value when float values
				else
					nBitPixels = ULONG_IMG;
			};

			fits_create_img(m_fits, nBitPixels, nAxis, nAxes, &nStatus);
			if (nStatus == 0)
			{
				bResult = true;

				if (m_lISOSpeed)
					bResult = bResult && WriteKey("ISOSPEED", m_lISOSpeed);
				if (m_lGain >= 0)
					bResult = bResult && WriteKey("GAIN", m_lGain);
				if (m_filterName != "")
					bResult = bResult && WriteKey("FILTER", m_filterName);
				if (m_fExposureTime)
				{
					bResult = bResult && WriteKey("EXPTIME", m_fExposureTime, "Exposure time (in seconds)");
					bResult = bResult && WriteKey("EXPOSURE", m_fExposureTime, "Exposure time (in seconds)");
				};
				if (m_nrframes)
				{
					bResult = bResult && WriteKey("NCOMBINE", m_nrframes, "Number of stacked frames");
				}
				if ((m_lNrChannels == 1) && (m_CFAType != CFATYPE_NONE))
					bResult = bResult && WriteKey("DSSCFATYPE", (int)m_CFAType);

				WriteKey("SOFTWARE", QString("DeepSkyStacker %1").arg(VERSION_DEEPSKYSTACKER).toUtf8());
				WriteAllKeys();
			};

		};
		if (!bResult)
		{
			fits_close_file(m_fits, &nStatus);
			m_fits = nullptr;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CFITSWriter::Write()
{
	bool bResult = false;

	if (m_fits != nullptr)
	{
		int lScanLineSize;
		std::unique_ptr<std::uint8_t[]> redBuffer;
		std::unique_ptr<std::uint8_t[]> greenBuffer;
		std::unique_ptr<std::uint8_t[]> blueBuffer;

		lScanLineSize = m_lWidth * (m_lBitsPerPixel / 8);
		if (m_lNrChannels == 1)
		{
			redBuffer = std::make_unique<std::uint8_t[]>(lScanLineSize);
		}
		else
		{
			redBuffer = std::make_unique<std::uint8_t[]>(lScanLineSize);
			greenBuffer = std::make_unique<std::uint8_t[]>(lScanLineSize);
			blueBuffer = std::make_unique<std::uint8_t[]>(lScanLineSize);
		};
		// If we ran out of memory here, an exception was thrown, just let it crash.

		constexpr double scalingFactor16Bit = 1.0 + std::numeric_limits<std::uint8_t>::max();
		constexpr double scalingFactor32Bit = (1.0 + std::numeric_limits<std::uint8_t>::max()) * (1.0 + std::numeric_limits<std::uint16_t>::max()); // identical to (1 << 24).
		constexpr double scalingFactorFloat = static_cast<double>(std::numeric_limits<std::uint16_t>::max()) / 256.0;
		// This was 1.0 + std::numeric_limits<std::uint8_t>::max().
		// Change from 256.0 to 255.996, so that writing an image to a FITS file and then read back in again
		// results in identical values. The scaling in function CFITSReader::Read() is done with 255.996 to
		// avoid overflow issues which could result in values that were 0 instead of 65535.

		{
			bResult = true;
			int			datatype = 0;
			int			nStatus = 0;

			switch (m_lBitsPerPixel)
			{
			case 8 :
				datatype = TBYTE;
				break;
			case 16 :
				datatype = TUSHORT;
				break;
			case 32 :
				if (m_bFloat)
					datatype = TFLOAT;
				else
					datatype = TULONG;
				break;
			case 64 :
				datatype = TFLOAT;
				break;
			};

			if (m_pProgress)
				m_pProgress->Start2(m_lHeight);

			for (int j = 0; j < m_lHeight; j++)
			{
				long pfPixel[3];

				if (m_lNrChannels == 1)
				{
					std::uint8_t* pBYTELine = redBuffer.get();
					std::uint16_t* pWORDLine = reinterpret_cast<std::uint16_t*>(redBuffer.get());
					std::uint32_t* pDWORDLine = reinterpret_cast<std::uint32_t*>(redBuffer.get());
					float* pFLOATLine = reinterpret_cast<float*>(redBuffer.get());

					for (int i = 0; i < m_lWidth; i++)
					{
						double fRed, fGreen, fBlue;
						if (false == OnWrite(i, j, fRed, fGreen, fBlue))
							return false;

						double fGray;
						if (m_CFAType != CFATYPE_NONE)
						{
							fGray = std::max(fRed, std::max(fGreen, fBlue));
							// 2 out of 3 should be 0
						}
						else
						{
							constexpr double scalingFactor { 1.0 + std::numeric_limits<std::uint8_t>::max() };
							// Convert to gray scale
							double H, S, L;
							ToHSL(fRed, fGreen, fBlue, H, S, L);
							fGray = L * scalingFactor;
						};

						// fGray should be in the range [0.0, 256.0), 256 exclusive, so the maximum value is 255.996.

						if (m_lBitsPerPixel == 8)
						{
							*pBYTELine++ = static_cast<std::uint8_t>(fGray); // 0..255
						}
						else if (m_lBitsPerPixel == 16)
						{
							*pWORDLine++ = static_cast<std::uint16_t>(fGray * scalingFactor16Bit); // 0..65535
						}
						else if (m_lBitsPerPixel == 32)
						{
							if (m_bFloat)
							{
								*pFLOATLine++ = static_cast<float>(fGray / scalingFactorFloat); // [0.0, 1.0], 1.0 inclusive
							}
							else
							{
								*pDWORDLine++ = static_cast<std::uint32_t>(fGray * scalingFactor32Bit); // 0..255, shifted left by 24 bits
							}
						}
					}

					pfPixel[0] = 1;
					pfPixel[1] = j + 1;
					pfPixel[2] = 1;

					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, redBuffer.get(), &nStatus);
				}
				else
				{
					std::uint8_t* pBYTELineRed = redBuffer.get();
					std::uint16_t* pWORDLineRed = reinterpret_cast<std::uint16_t*>(redBuffer.get());
					std::uint32_t* pDWORDLineRed = reinterpret_cast<std::uint32_t*>(redBuffer.get());
					float* pFLOATLineRed = reinterpret_cast<float*>(redBuffer.get());

					std::uint8_t* pBYTELineGreen = greenBuffer.get();
					std::uint16_t* pWORDLineGreen = reinterpret_cast<std::uint16_t*>(greenBuffer.get());
					std::uint32_t* pDWORDLineGreen = reinterpret_cast<std::uint32_t*>(greenBuffer.get());
					float* pFLOATLineGreen = reinterpret_cast<float*>(greenBuffer.get());

					std::uint8_t* pBYTELineBlue = blueBuffer.get();
					std::uint16_t* pWORDLineBlue = reinterpret_cast<std::uint16_t*>(blueBuffer.get());
					std::uint32_t* pDWORDLineBlue = reinterpret_cast<std::uint32_t*>(blueBuffer.get());
					float* pFLOATLineBlue = reinterpret_cast<float*>(blueBuffer.get());

					for (int i = 0; i < m_lWidth; i++)
					{
						double fRed, fGreen, fBlue;

						if (false == OnWrite(i, j, fRed, fGreen, fBlue))
							return false;

						// fRed, fGreen, fBlue should be in the range [0.0, 256.0), 256 exclusive, so the maximum value is 255.996.

						if (m_lBitsPerPixel == 8)
						{
							*pBYTELineRed++ = static_cast<std::uint8_t>(fRed); // 0..255
							*pBYTELineGreen++ = static_cast<std::uint8_t>(fGreen);
							*pBYTELineBlue++ = static_cast<std::uint8_t>(fBlue);
						}
						else if (m_lBitsPerPixel == 16)
						{
							*pWORDLineRed++ = static_cast<std::uint16_t>(fRed * scalingFactor16Bit); // 0..65535
							*pWORDLineGreen++ = static_cast<std::uint16_t>(fGreen * scalingFactor16Bit);
							*pWORDLineBlue++ = static_cast<std::uint16_t>(fBlue * scalingFactor16Bit);
						}
						else if (m_lBitsPerPixel == 32)
						{
							if (m_bFloat)
							{
								*pFLOATLineRed++ = static_cast<float>(fRed / scalingFactorFloat); // [0.0, 1.0], 1.0 inclusive
								*pFLOATLineGreen++ = static_cast<float>(fGreen / scalingFactorFloat);
								*pFLOATLineBlue++ = static_cast<float>(fBlue / scalingFactorFloat);
							}
							else
							{
								*pDWORDLineRed++ = static_cast<std::uint32_t>(fRed * scalingFactor32Bit); // 0..255, shifted left by 24 bits
								*pDWORDLineGreen++ = static_cast<std::uint32_t>(fGreen * scalingFactor32Bit);
								*pDWORDLineBlue++ = static_cast<std::uint32_t>(fBlue * scalingFactor32Bit);
							}
						}
					}

					pfPixel[0] = 1;
					pfPixel[1] = j + 1;
					pfPixel[2] = 1;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, redBuffer.get(), &nStatus);
					pfPixel[2] = 2;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, greenBuffer.get(), &nStatus);
					pfPixel[2] = 3;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, blueBuffer.get(), &nStatus);
				}

				if (m_pProgress)
					m_pProgress->Progress2(j + 1);
			}

			if (m_pProgress)
				m_pProgress->End2();
		}
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CFITSWriter::Close()
{
	bool			bResult = true;
	int				nStatus = 0;

	if (m_fits)
	{
		bResult = OnClose();
		if (bResult)
		{
			fits_close_file(m_fits, &nStatus);
			m_fits = nullptr;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CFITSWriteFromMemoryBitmap : public CFITSWriter
{
private :
	CMemoryBitmap* m_pMemoryBitmap;

private :
	FITSFORMAT GetBestFITSFormat(const CMemoryBitmap* pBitmap);

public :
	CFITSWriteFromMemoryBitmap(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress) :
		CFITSWriter(szFileName, pProgress),
		m_pMemoryBitmap{ pBitmap }
	{}

	virtual bool Close()
	{
		return OnClose();
	}

	virtual ~CFITSWriteFromMemoryBitmap()
	{
		Close();
	};

	virtual bool OnOpen() override;
	virtual bool OnWrite(int lX, int lY, double & fRed, double & fGreen, double & fBlue) override;
	virtual bool OnClose() override;
};

/* ------------------------------------------------------------------- */

FITSFORMAT CFITSWriteFromMemoryBitmap::GetBestFITSFormat(const CMemoryBitmap* pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	if (dynamic_cast<const C24BitColorBitmap*>(pBitmap) != nullptr)
		return FF_8BITRGB;
	if (dynamic_cast<const C48BitColorBitmap*>(pBitmap) != nullptr)
		return FF_16BITRGB;
	if (dynamic_cast<const C96BitColorBitmap*>(pBitmap) != nullptr)
		return FF_32BITRGB;
	if (dynamic_cast<const C96BitFloatColorBitmap*>(pBitmap) != nullptr)
		return FF_32BITRGBFLOAT;
	if (dynamic_cast<const C8BitGrayBitmap*>(pBitmap) != nullptr)
		return FF_8BITGRAY;
	if (dynamic_cast<const C16BitGrayBitmap*>(pBitmap) != nullptr)
		return FF_16BITGRAY;
	if (dynamic_cast<const C32BitGrayBitmap*>(pBitmap) != nullptr)
		return FF_32BITGRAY;
	if (dynamic_cast<const C32BitFloatGrayBitmap*>(pBitmap) != nullptr)
		return FF_32BITGRAYFLOAT;

	return FF_UNKNOWN;
};

/* ------------------------------------------------------------------- */

bool CFITSWriteFromMemoryBitmap::OnOpen()
{
	ZFUNCTRACE_RUNTIME();

	const int lWidth  = m_pMemoryBitmap->Width();
	const int lHeight = m_pMemoryBitmap->Height();

	m_DateTime		= m_pMemoryBitmap->m_DateTime;
	m_lBitsPerPixel = m_pMemoryBitmap->BitPerSample();
	m_lNrChannels   = m_pMemoryBitmap->IsMonochrome() ? 1 : 3;
	m_bFloat		= m_pMemoryBitmap->IsFloat();
	m_CFAType = CFATYPE_NONE;
	if (m_pMemoryBitmap && m_pMemoryBitmap->IsCFA())
		m_CFAType = m_pMemoryBitmap->GetCFAType();

	if (m_Format == FF_UNKNOWN)
		m_Format = GetBestFITSFormat(m_pMemoryBitmap);

	if (m_Format != FF_UNKNOWN)
	{
		SetFormat(lWidth, lHeight, m_Format, m_CFAType);
		if (!m_lISOSpeed)
			m_lISOSpeed = m_pMemoryBitmap->GetISOSpeed();
		if (m_lGain < 0)
			m_lGain = m_pMemoryBitmap->GetGain();
		if ((m_pMemoryBitmap->filterName() != "") && (m_filterName == ""))
			m_filterName = m_pMemoryBitmap->filterName();
		if (!m_fExposureTime)
			m_fExposureTime = m_pMemoryBitmap->GetExposure();
		if (!m_nrframes)
			m_nrframes = m_pMemoryBitmap->GetNrFrames();
	};

	return true;
};

/* ------------------------------------------------------------------- */

bool CFITSWriteFromMemoryBitmap::OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue)
{
	bool result { true };
	try
	{
		if (m_pMemoryBitmap)
		{
			if (m_lNrChannels == 1)
			{
				m_pMemoryBitmap->GetPixel(lX, lY, fRed);
				fGreen = fRed;
				fBlue = fRed;
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
		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Critical);
		exit(1);
	}
	return result;
};

/* ------------------------------------------------------------------- */

bool CFITSWriteFromMemoryBitmap::OnClose()
{
	return true;
};

/* ------------------------------------------------------------------- */

bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat, const QString& szDescription, int lISOSpeed, int lGain, double fExposure)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap)
	{
		CFITSWriteFromMemoryBitmap fits(szFileName, pBitmap, pProgress);

		fits.m_ExtraInfo = pBitmap->m_ExtraInfo;
		fits.m_DateTime  = pBitmap->m_DateTime;
		fits.SetDescription(szDescription);
		if (lISOSpeed)
			fits.m_lISOSpeed = lISOSpeed;
		if (lGain >= 0)
			fits.m_lGain = lGain;
		if (fExposure)
			fits.m_fExposureTime = fExposure;
		fits.SetFormat(FITSFormat);
		if (fits.Open())
		{
			bResult = fits.Write();
			// fits.Close();
		};
	};

	return bResult;
}


/* ------------------------------------------------------------------- */

bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat)
{
	return WriteFITS(szFileName, pBitmap, pProgress, FITSFormat, /*szDescription*/"", /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0);
}

bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat, const QString& szDescription)
{
	return WriteFITS(szFileName, pBitmap, pProgress, FITSFormat, szDescription, /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0);
}

/* ------------------------------------------------------------------- */

bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, const QString& szDescription, int lISOSpeed, int lGain, double fExposure)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap)
	{
		CFITSWriteFromMemoryBitmap fits{ szFileName, pBitmap, pProgress };

		fits.m_ExtraInfo = pBitmap->m_ExtraInfo;
		fits.m_DateTime  = pBitmap->m_DateTime;
		fits.SetDescription(szDescription);
		if (lISOSpeed)
			fits.m_lISOSpeed = lISOSpeed;
		if (lGain >= 0)
			fits.m_lGain = lGain;
		if (fExposure)
			fits.m_fExposureTime = fExposure;
		if (fits.Open())
		{
			bResult = fits.Write();
			// fits.Close();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress)
{
	return WriteFITS(szFileName, pBitmap, pProgress, /*szDestription*/"", /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0);
}

bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, const QString& szDescription)
{
	return WriteFITS(szFileName, pBitmap, pProgress, szDescription, /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0);
}


bool IsFITSPicture(const fs::path& szFileName, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	return GetFITSInfo(szFileName, BitmapInfo);
};


int	LoadFITSPicture(const fs::path& szFileName, CBitmapInfo& BitmapInfo, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	int result = -1; // -1 means not a FITS file.

	if (GetFITSInfo(szFileName, BitmapInfo) && BitmapInfo.CanLoad())
	{
		if (ReadFITS(szFileName, rpBitmap, ignoreBrightness, pProgress))
		{
			if (BitmapInfo.IsCFA() && (IsSuperPixels() || IsRawBayer() || IsRawBilinear()))
			{
				if (C16BitGrayBitmap* pGrayBitmap = dynamic_cast<C16BitGrayBitmap*>(rpBitmap.get()))
				{
					if (IsSuperPixels())
						pGrayBitmap->UseSuperPixels(true);
					else if (IsRawBayer())
						pGrayBitmap->UseRawBayer(true);
					else if (IsRawBilinear())
						pGrayBitmap->UseBilinear(true);
				}
			}
			result = 0;
		}
		else
			result = 1; // Failed to read file
	}
	return result;
}

void GetFITSExtension(const QString& path, QString& strExtension)
{
	QFileInfo fileInfo(path);

	const QString strExt("." + fileInfo.suffix());
	if (strExt.compare(".FITS", Qt::CaseInsensitive) == 0)
		strExtension = strExt;
	else if (strExt.compare(".FIT", Qt::CaseInsensitive) == 0)
		strExtension = strExt;
	else
		strExtension = ".fts";
}

void GetFITSExtension(const fs::path& file, QString& strExtension)
{
	GetFITSExtension(QString::fromStdU16String(file.generic_u16string().c_str()), strExtension);
}

void GetFITSExtension(fs::path path, QString& strExtension)
{
	QDir qPath(path);
	GetFITSExtension(qPath.absolutePath(), strExtension);
}