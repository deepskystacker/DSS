#include <stdafx.h>
#include "resource.h"
#include "Workspace.h"
#include "FITSUtil.h"
#include "..\CFitsio\fitsio.h"
#include "Ztrace.h"
#include "DSSProgress.h"
#include "ZExcBase.h"
#include "Multitask.h"
#include "GrayBitmap.h"
#include "ColorBitmap.h"
#include "RAWUtils.h"
#include "BitmapInfo.h"
#include "ColorHelpers.h"

using namespace DSS;



/* ------------------------------------------------------------------- */

static	CComAutoCriticalSection			g_FITSCritical;

CFITSHeader::CFITSHeader()
{
	m_bFloat = false;
	m_fExposureTime = 0;
	m_lISOSpeed     = 0;
	m_lGain         = -1;
	m_CFAType		= CFATYPE_NONE;
	m_Format		= FF_UNKNOWN;
	m_bSigned		= false;
	m_DateTime.wYear= 0;
	g_FITSCritical.Lock();
    m_lWidth = 0;
    m_lHeight = 0;
	m_lBitsPerPixel = 0;
    m_lNrChannels = 0;
	m_xBayerOffset = 0;
	m_yBayerOffset = 0;
	m_bitPix = 0;

	
};

/* ------------------------------------------------------------------- */

CFITSHeader::~CFITSHeader()
{
	g_FITSCritical.Unlock();
};

/* ------------------------------------------------------------------- */

inline double AdjustColor(const double fColor)
{
	if (std::isfinite(fColor))
		return std::clamp(fColor, 0.0, 255.0);
	else
		return 0.0;
};

/* ------------------------------------------------------------------- */

bool	IsFITSRaw()
{
	Workspace			workspace;

	return workspace.value("FitsDDP/FITSisRAW", false).toBool();
};

/* ------------------------------------------------------------------- */

CFATYPE GetFITSCFATYPE()
{
	Workspace			workspace;

	bool isFitsRaw = workspace.value("FitsDDP/FITSisRAW", false).toBool();
	CFATYPE pattern = (CFATYPE)workspace.value("FitsDDP/BayerPattern", (uint)CFATYPE_NONE).toUInt();


	return isFitsRaw ? pattern : CFATYPE_NONE;
};

/* ------------------------------------------------------------------- */

bool	IsFITSRawBayer()
{
	Workspace			workspace;
	QString				interpolation;

	interpolation = workspace.value("FitsDDP/Interpolation").toString();

	return (0 == interpolation.compare("RawBayer", Qt::CaseInsensitive));
};

/* ------------------------------------------------------------------- */

bool	IsFITSSuperPixels()
{
	Workspace			workspace;
	QString				interpolation;

	workspace.value("FitsDDP/Interpolation").toString();

	return (0 == interpolation.compare("SuperPixels", Qt::CaseInsensitive));
};

/* ------------------------------------------------------------------- */

bool	IsFITSBilinear()
{
	Workspace			workspace;
	QString				interpolation;

	workspace.value("FitsDDP/Interpolation").toString();

	return (0 == interpolation.compare("Bilinear", Qt::CaseInsensitive));
};

/* ------------------------------------------------------------------- */

bool	IsFITSAHD()
{
	Workspace			workspace;
	QString				interpolation;

	workspace.value("FitsDDP/Interpolation").toString();

	return (0 == interpolation.compare("AHD", Qt::CaseInsensitive));
};

/* ------------------------------------------------------------------- */

double	GetFITSBrightnessRatio()
{
	Workspace			workspace;

	return workspace.value("FitsDDP/Brightness", 1.0).toDouble();
};

/* ------------------------------------------------------------------- */

void	GetFITSRatio(double & fRed, double & fGreen, double & fBlue)
{
	Workspace			workspace;

	fGreen = workspace.value("FitsDDP/Brightness", 1.0).toDouble();

	fRed = workspace.value("FitsDDP/RedScale", 1.0).toDouble();
	fRed *= fGreen;

	fBlue = workspace.value("FitsDDP/BlueScale", 1.0).toDouble();
	fBlue *= fGreen;
};

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

bool CFITSReader::ReadKey(LPCSTR szKey, double & fValue, CString & strComment)
{
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		CHAR			szComment[500];

		fits_read_key(m_fits, TDOUBLE, szKey, &fValue, szComment, &nStatus);
		if (!nStatus)
		{
			bResult = true;
			strComment = szComment;
		};
	};

	return bResult;
};

bool CFITSReader::ReadKey(LPCSTR szKey, double & fValue)
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

bool CFITSReader::ReadKey(LPCSTR szKey, int& lValue)
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

bool CFITSReader::ReadKey(LPCSTR szKey, CString & strValue)
{
	bool				bResult = false;
	CHAR				szValue[2000];
	int					nStatus = 0;

	if (m_fits)
	{
		fits_read_key(m_fits, TSTRING, szKey, szValue, nullptr, &nStatus);
		if (!nStatus)
		{
			strValue = szValue;
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
		int					nKeywords;
		int					nStatus = 0;
		QSettings			settings;
		CString				strPropagated;

		strPropagated = (LPCTSTR)settings.value("FitsDDP/Propagated", "").toString().utf16();
		if ("" == strPropagated)
			strPropagated = "[CRVAL1][CRVAL2][CRTYPE1][CRTYPE2][DEC][RA][OBJCTDEC][OBJCTRA][OBJCTALT][OBJCTAZ][OBJCTHA][SITELAT][SITELONG][TELESCOP][INSTRUME][OBSERVER][RADECSYS]";


		fits_get_hdrspace(m_fits, &nKeywords, nullptr, &nStatus);
		for (int i = 1;i<=nKeywords;i++)
		{
			CHAR			szKeyName[FLEN_CARD];
			CHAR			szValue[FLEN_VALUE];
			CHAR			szComment[FLEN_COMMENT];
			int				nKeyClass;

			fits_read_keyn(m_fits, i, szKeyName, szValue, szComment, &nStatus);
			nKeyClass = fits_get_keyclass(szKeyName);
			if ((nKeyClass == TYP_USER_KEY) || (nKeyClass == TYP_REFSYS_KEY))
			{
				bool		bPropagate = false;
				CString		strKeyName;
				strKeyName.Format(_T("[%s]"), (LPCTSTR)CA2CT(szKeyName));

				if (strPropagated.Find(strKeyName) != -1)
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

	fits_open_diskfile(&m_fits, (LPCSTR)CT2CA(m_strFileName, CP_ACP), READONLY, &status);
	if (0 != status)
	{
		fits_get_errstatus(status, error_text);
		CString errorMessage;
		errorMessage.Format(_T("fits_open_diskfile %s\nreturned a status of %d, error text is:\n\"%s\""),
			m_strFileName,
			status,
			CString(error_text));

		ZTRACE_RUNTIME((LPCSTR)CT2CA(errorMessage));

#if defined(_CONSOLE)
		std::wcerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONWARNING);
#endif


	}


	if (m_fits)
	{
		CStringA fileName(m_strFileName);
		ZTRACE_RUNTIME("Opened %s", fileName);

		// File ok - move to the first image HDU
		CString			strSimple;
		int			lNrAxis = 0;
		int			lWidth  = 0,
						lHeight = 0,
						lNrChannels = 0;
		double			fExposureTime = 0;
		CString			strMake;
		CString			strISOSpeed;
		CString			CFAPattern("");
		CString			filterName("");
		int			lISOSpeed = 0;
		int			lGain = -1;
		double			xBayerOffset = 0.0, yBayerOffset = 0.0;

		m_bDSI = false;

		bResult = ReadKey("SIMPLE", strSimple);
		bResult = ReadKey("NAXIS", lNrAxis);
		if ((strSimple == _T("T")) && (lNrAxis >= 2 && lNrAxis <= 3))
		{
			CString				strComment;
			ReadAllKeys();

			bResult = ReadKey("INSTRUME", strMake);

			//
			// Attempt to get the exposure time
			//
			bResult = ReadKey("EXPTIME", fExposureTime, strComment);
			if (!bResult)
				bResult = ReadKey("EXPOSURE", fExposureTime, strComment);

			if (bResult && strComment.GetLength())
			{
				if ((strComment.Find(_T("in seconds"))<0) &&
					((strComment.Find(_T("ms"))>0) || (fExposureTime>3600)))
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

			bResult = ReadKey("ISOSPEED", strISOSpeed);
			if (bResult)
			{
				if (strISOSpeed.Find(_T("ISO"), 0) == 0)
				{
					strISOSpeed = strISOSpeed.Right(strISOSpeed.GetLength()-3);
					strISOSpeed.Trim();
					lISOSpeed = _ttol(strISOSpeed);
				};
			};

			bResult = ReadKey("GAIN", lGain);

			ReadKey("FILTER", filterName);

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
			static std::map<CString, CFATYPE> bayerMap;
			if (bayerMap.empty())
			{
				bayerMap.emplace("BGGR", CFATYPE_BGGR);
				bayerMap.emplace("GRBG", CFATYPE_GRBG);
				bayerMap.emplace("GBRG", CFATYPE_GBRG);
				bayerMap.emplace("RGGB", CFATYPE_RGGB);

				bayerMap.emplace("CGMY", CFATYPE_CGMY);
				bayerMap.emplace("CGYM", CFATYPE_CGYM);
				bayerMap.emplace("CMGY", CFATYPE_CMGY);
				bayerMap.emplace("CMYG", CFATYPE_CMYG);
				bayerMap.emplace("CYMG", CFATYPE_CYMG);
				bayerMap.emplace("CYGM", CFATYPE_CYGM);

				bayerMap.emplace("GCMY", CFATYPE_GCMY);
				bayerMap.emplace("GCYM", CFATYPE_GCYM);
				bayerMap.emplace("GMCY", CFATYPE_GMCY);
				bayerMap.emplace("GMYC", CFATYPE_GMYC);
				bayerMap.emplace("GYCM", CFATYPE_GYCM);
				bayerMap.emplace("GYMC", CFATYPE_GYMC);

				bayerMap.emplace("MCGY", CFATYPE_MCGY);
				bayerMap.emplace("MCYG", CFATYPE_MCYG);
				bayerMap.emplace("MGYC", CFATYPE_MGYC);
				bayerMap.emplace("MGCY", CFATYPE_MGCY);
				bayerMap.emplace("MYGC", CFATYPE_MYGC);
				bayerMap.emplace("MYCG", CFATYPE_MYCG);

				bayerMap.emplace("YCGM", CFATYPE_YCGM);
				bayerMap.emplace("YCMG", CFATYPE_YCMG);
				bayerMap.emplace("YGMC", CFATYPE_YGMC);
				bayerMap.emplace("YGCM", CFATYPE_YGCM);
				bayerMap.emplace("YMCG", CFATYPE_YMCG);
				bayerMap.emplace("YMGC", CFATYPE_YMGC);

				bayerMap.emplace("CYGMCYMG", CFATYPE_CYGMCYMG);
				bayerMap.emplace("GMCYMGCY", CFATYPE_GMCYMGCY);
				bayerMap.emplace("CYMGCYGM", CFATYPE_CYMGCYGM);
				bayerMap.emplace("MGCYGMCY", CFATYPE_MGCYGMCY);
				bayerMap.emplace("GMYCGMCY", CFATYPE_GMYCGMCY);
				bayerMap.emplace("YCGMCYGM", CFATYPE_YCGMCYGM);
				bayerMap.emplace("GMCYGMYC", CFATYPE_GMCYGMYC);
				bayerMap.emplace("CYGMYCGM", CFATYPE_CYGMYCGM);
				bayerMap.emplace("YCGMYCMG", CFATYPE_YCGMYCMG);
				bayerMap.emplace("GMYCMGYC", CFATYPE_GMYCMGYC);
				bayerMap.emplace("YCMGYCGM", CFATYPE_YCMGYCGM);
				bayerMap.emplace("MGYCGMYC", CFATYPE_MGYCGMYC);
				bayerMap.emplace("MGYCMGCY", CFATYPE_MGYCMGCY);
				bayerMap.emplace("YCMGCYMG", CFATYPE_YCMGCYMG);
				bayerMap.emplace("MGCYMGYC", CFATYPE_MGCYMGYC);
				bayerMap.emplace("CYMGYCMG", CFATYPE_CYMGYCMG);
			}

			//
			// Attempt to determine the correct CFA (aka Bayer matrix) from keywords in the FITS header.
			// 
			// Some Meade DSI cameras used the keyword MOSAIC with a value of "CMYG", when in fact the actual
			// matrix used was CYGMCYMG. so that is special cased.
			// 
			if (ReadKey("MOSAIC", CFAPattern) && (strMake.Left(3) == _T("DSI")))
			{
				ZTRACE_RUNTIME("CFA Pattern read from FITS keyword MOSAIC is %s", (LPCSTR)CT2CA(CFAPattern, CP_UTF8));

				m_bDSI = true;
				// Special case of DSI FITS files
				CFAPattern.Trim();
				if (CFAPattern == _T("CMYG"))
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
					ZTRACE_RUNTIME("CFA Pattern read from FITS keyword BAYERPAT or COLORTYP is %s", (LPCSTR)CT2CA(CFAPattern,CP_UTF8));
				}

				CFAPattern.Trim();

				if (CFAPattern != _T(""))
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

			CString			strDateTime;

			memset(&m_DateTime, 0, sizeof(m_DateTime));
			if (ReadKey("DATE-OBS", strDateTime))
			{
				// Decode 2007-11-02T22:07:03.890
				//        01234567890123456789012
				CString				strTime;

				if (strDateTime.GetLength() >= 19)
				{
					m_DateTime.wYear  = _ttol(strDateTime.Left(4));
					m_DateTime.wMonth = _ttol(strDateTime.Mid(5, 2));
					m_DateTime.wDay   = _ttol(strDateTime.Mid(8, 2));
					m_DateTime.wHour  = _ttol(strDateTime.Mid(11, 2));
					m_DateTime.wMinute= _ttol(strDateTime.Mid(14, 2));
					m_DateTime.wSecond= _ttol(strDateTime.Mid(17, 2));
				}
				else if ((strDateTime.GetLength() == 8) && ReadKey("TIME-OBS", strTime))
				{
					// Decode dd/mm/yy
					//        01234567
					m_DateTime.wYear  = _ttol(strDateTime.Mid(6, 2));
					if (m_DateTime.wYear < 70)
						m_DateTime.wYear += 2000;
					else
						m_DateTime.wYear += 1900;
					m_DateTime.wMonth = _ttol(strDateTime.Mid(3, 2));
					m_DateTime.wDay   = _ttol(strDateTime.Mid(0, 2));

					if (strTime.GetLength() >= 8)
					{
						// Decode hh:mm:ss.xxx
						//        01234567
						m_DateTime.wHour   = _ttol(strTime.Mid(0, 2));
						m_DateTime.wMinute = _ttol(strTime.Mid(3, 2));
						m_DateTime.wSecond = _ttol(strTime.Mid(6, 2));
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
				m_filterName = QString::fromStdWString(filterName.GetString());
			};
			
			//
			// If the user has explicitly set that this FITS file is a Bayer format RAW file,
			// then the user will also have explicitly set the Bayer pattern that's to be used.
			// In this case we use that and set the Bayer offsets (if any) to zero
			//
			bool isRaw = IsFITSRaw();

			if ((m_lNrChannels == 1) &&
				((m_lBitsPerPixel == 16) || (m_lBitsPerPixel == 32)))
			{
				ZTRACE_RUNTIME("Using user supplied override for Bayer pattern.");
				if (isRaw)
				{
					m_CFAType = GetFITSCFATYPE();
					m_xBayerOffset = 0;
					m_yBayerOffset = 0;
				}
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
	constexpr double scaleFactorInt16 = 1.0 + std::numeric_limits<std::uint8_t>::max();;
	constexpr double scaleFactorInt32 = scaleFactorInt16 * (1.0 + std::numeric_limits<std::uint16_t>::max());

	ZFUNCTRACE_RUNTIME();
	bool bResult = true;
	char error_text[31] = "";			// Error text for FITS errors.
	
	const int colours = (m_lNrChannels >= 3) ? 3 : 1;		// 3 ==> RGB, 1 ==> Mono

	double fMin = 0.0, fMax = 0.0;		// minimum and maximum pixel values for floating point images

	if (m_lNrChannels > 3)
		ZTRACE_RUNTIME("Number of colour channels is %d, only 3 will be used.", m_lNrChannels);

	if (m_fits != nullptr) do
	{
		double dNULL = 0;

		if (m_pProgress)
			m_pProgress->Start2(m_lHeight);

		std::int64_t fPixel[3] = { 1, 1, 1 };		// want to start reading at column 1, row 1, plane 1

		ZTRACE_RUNTIME("FITS colours=%d, bps=%d, w=%d, h=%d", colours, m_lBitsPerPixel, m_lWidth, m_lHeight);

		const std::int64_t nElements = static_cast<std::int64_t>(m_lWidth) * m_lHeight * colours;
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
			CStringA errMsg;
			errMsg.Format(
				"fits_read_pixll returned a status of %d, error text is \"%s\"",
				status,
				error_text);

			ZException exc(errMsg, status, ZException::unrecoverable);
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
#pragma omp parallel default(none) shared(fMin, fMax) firstprivate(localMin, localMax) if(nrProcessors > 1)
			{
#pragma omp for schedule(dynamic, 10'000)
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
			constexpr double scaleFactor = double{ std::numeric_limits<std::uint16_t>::max() } / 256.0;
			const double normalizationFactor = scaleFactor / (fMax - fMin);
			return (value - fMin) * normalizationFactor;
		};

#pragma omp parallel for default(none) schedule(guided, 50) if(nrProcessors > 1)
		for (int row = 0; row < m_lHeight; ++row)
		{
			for (int col = 0; col < m_lWidth; ++col)
			{
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

				OnRead(col, row, AdjustColor(fRed), AdjustColor(fGreen), AdjustColor(fBlue));
			}

			if (m_pProgress != nullptr && 0 == omp_get_thread_num() && (rowProgress++ % 25) == 0)	// Are we on the master thread?
				m_pProgress->Progress2(row);
		}

	} while (false);

	return bResult;
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

public :
	CFITSReadInMemoryBitmap(LPCTSTR szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase*	pProgress):
		CFITSReader(szFileName, pProgress),
		m_outBitmap{ rpBitmap }
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
		if ((m_lNrChannels == 1) &&
			(m_lBitsPerPixel == 8) &&
			(m_CFAType != CFATYPE_NONE))
		{
			// 
			// Set CFA type to none even if the FITS header specified a value
			//
			m_CFAType = CFATYPE_NONE;

			static bool eightBitWarningIssued = false;
			if (!eightBitWarningIssued)
			{
				CString errorMessage;
				errorMessage.Format(IDS_8BIT_FITS_NODEBAYER);
#if defined(_CONSOLE)
				std::wcerr << errorMessage;
#else
				AfxMessageBox(errorMessage, MB_OK | MB_ICONWARNING);
#endif
				// Remember we already said we won't do that!
				eightBitWarningIssued = true;
			}
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
				GetFITSRatio(m_fRedRatio, m_fGreenRatio, m_fBlueRatio);
			}
		}
		else
			m_fBrightnessRatio = GetFITSBrightnessRatio();

		m_pBitmap->SetMaster(false);
		if (m_fExposureTime)
			m_pBitmap->SetExposure(m_fExposureTime);
		if (m_lISOSpeed)
			m_pBitmap->SetISOSpeed(m_lISOSpeed);
		if (m_lGain >= 0)
			m_pBitmap->SetGain(m_lGain);
		m_pBitmap->setFilterName(m_filterName);
		m_pBitmap->m_DateTime = m_DateTime;

		CString strDescription;
		if (m_strMake.GetLength())
			strDescription.Format(_T("FITS (%s)"), static_cast<LPCTSTR>(m_strMake));
		else
			strDescription	= _T("FITS");
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
						fRed = min(maxValue, fRed * m_fBlueRatio);
						break;
					case BAYER_GREEN:
						fRed = min(maxValue, fRed * m_fGreenRatio);
						break;
					case BAYER_RED:
						fRed = min(maxValue, fRed * m_fRedRatio);
						break;
					}
				}
				else
				{
					fRed = min(maxValue, fRed * m_fBrightnessRatio);
					fGreen = min(maxValue, fGreen * m_fBrightnessRatio);
					fBlue = min(maxValue, fBlue * m_fBrightnessRatio);
				}
				m_pBitmap->SetPixel(lX, lY, fRed);
			}
			else
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
		std::wcerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
		exit(1);

	}
	return true;
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


bool ReadFITS(LPCTSTR szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase *	pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CFITSReadInMemoryBitmap	fits(szFileName, rpBitmap, pProgress);
	return fits.Open() && fits.Read();
}


bool GetFITSInfo(LPCTSTR szFileName, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool					bResult = false;
	bool					bContinue = true;
	CFITSReader				fits(szFileName, nullptr);

	// Exclude JPEG, PNG or TIFF format
	{
		TCHAR szExt[1+_MAX_EXT];
		CString strExt;
		_tsplitpath(szFileName, nullptr, nullptr, nullptr, szExt);
		strExt = szExt;

		if (!strExt.CompareNoCase(_T(".JPG")) ||
			!strExt.CompareNoCase(_T(".JPEG")) ||
			!strExt.CompareNoCase(_T(".PNG")) ||
			!strExt.CompareNoCase(_T(".TIF")) ||
			!strExt.CompareNoCase(_T(".TIFF")))
		{
			bContinue = false;
		}
	}
	if (bContinue && fits.Open())
	{
		if (fits.m_strMake.GetLength() != 0) 
			BitmapInfo.m_strFileType = QString("FITS (%1)").arg(QString::fromWCharArray(fits.m_strMake.GetString()));
		else 
			BitmapInfo.m_strFileType = "FITS";

		BitmapInfo.m_strFileName	= QString::fromWCharArray(szFileName);
		BitmapInfo.m_lWidth			= fits.Width();
		BitmapInfo.m_lHeight		= fits.Height();
		BitmapInfo.m_lBitPerChannel = fits.BitPerChannels();
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


bool CFITSWriter::WriteKey(LPCSTR szKey, double fValue, LPCSTR szComment)
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

bool	CFITSWriter::WriteKey(LPCSTR szKey, int lValue, LPCSTR szComment)
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

bool	CFITSWriter::WriteKey(LPCSTR szKey, LPCTSTR szValue, LPCSTR szComment)
{
	bool				bResult = false;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_write_key(m_fits, TSTRING, szKey, (void*)szValue, szComment, &nStatus);
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
	for (const CExtraInfo& extraInfo : m_ExtraInfo.m_vExtras)
	{
		if (extraInfo.m_strName.compare("DATE-OBS", Qt::CaseInsensitive) == 0)
		{
			bFound = true;
			break;
		}
	};

	if (!bFound && m_DateTime.wYear)
	{
		// Add DATE-OBS to the list
		CString			strDateTime;

		strDateTime.Format(_T("%04d-%02d-%02dT%02d:%02d:%02d"),
						   m_DateTime.wYear, m_DateTime.wMonth, m_DateTime.wDay,
						   m_DateTime.wHour, m_DateTime.wMinute, m_DateTime.wSecond);

		m_ExtraInfo.AddInfo("DATE-OBS", QString::fromWCharArray(strDateTime.GetString()), "");
	};

	if (m_fits && m_ExtraInfo.m_vExtras.size())
	{
		int				nStatus = 0;


		for (int i = 0;i<m_ExtraInfo.m_vExtras.size();i++)
		{
			const CExtraInfo &ei = m_ExtraInfo.m_vExtras[i];
			CHAR szValue[FLEN_VALUE];

			// check that the keyword is not already used
			CString strName(ei.m_strName.toStdWString().c_str());
			fits_read_key(m_fits, TSTRING, (LPCSTR)CT2A(strName, CP_UTF8), szValue, nullptr, &nStatus);
			if (nStatus)
			{
				nStatus = 0;
				CHAR		szCard[FLEN_CARD];
				int			nType;
				CString		strTemplate;

				if (!ei.m_strComment.isEmpty())
					strTemplate.Format(_T("%s = %s / %s"), ei.m_strName.toStdWString().c_str(), ei.m_strValue.toStdWString().c_str(), ei.m_strComment.toStdWString().c_str());
				else
					strTemplate.Format(_T("%s = %s"), ei.m_strName.toStdWString().c_str(), ei.m_strValue.toStdWString().c_str());

				fits_parse_template((LPSTR)CT2A(strTemplate, CP_UTF8), szCard, &nType, &nStatus);
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
	CString			strFileName = m_strFileName;

	// Close();

	// Create a new fits file
	int				nStatus = 0;

	DeleteFile(strFileName.GetString());
	fits_create_diskfile(&m_fits, (LPCSTR)CT2A(strFileName, CP_ACP), &nStatus);
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
					bResult = bResult && WriteKey("FILTER", m_filterName.toStdWString().c_str());
				if (m_fExposureTime)
				{
					bResult = bResult && WriteKey("EXPTIME", m_fExposureTime, "Exposure time (in seconds)");
					bResult = bResult && WriteKey("EXPOSURE", m_fExposureTime, "Exposure time (in seconds)");
				};
				if ((m_lNrChannels == 1) && (m_CFAType != CFATYPE_NONE))
					bResult = bResult && WriteKey("DSSCFATYPE", (int)m_CFAType);

				CString			strSoftware = "DeepSkyStacker ";
				strSoftware += VERSION_DEEPSKYSTACKER;

				WriteKey("SOFTWARE", strSoftware.GetString());
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

	//
	// Multipliers of 256.0 and 65536.0 were not correct and resulted in a fully saturated
	// pixel being written with a value of zero because the value overflowed the data type 
	// which was being stored.   Change the code to use UCHAR_MAX and USHRT_MAX
	//

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
						OnWrite(i, j, fRed, fGreen, fBlue);

						double fGray;
						if (m_CFAType != CFATYPE_NONE)
						{
							fGray = std::max(fRed, std::max(fGreen, fBlue));
							// 2 out of 3 should be 0
						}
						else
						{
							constexpr double scalingFactor = 255.0;
							// Convert to gray scale
							double H, S, L;
							ToHSL(fRed, fGreen, fBlue, H, S, L);
							fGray = L * scalingFactor;
						};

						if (m_lBitsPerPixel == 8)
						{
							*pBYTELine++ = static_cast<std::uint8_t>(fGray);
						}
						else if (m_lBitsPerPixel == 16)
						{
							constexpr double scalingFactor = double{ UCHAR_MAX };
							*pWORDLine++ = static_cast<std::uint16_t>(fGray * scalingFactor);
						}
						else if (m_lBitsPerPixel == 32)
						{
							if (m_bFloat)
							{
								constexpr double scalingFactor = 1.0 + double{ UCHAR_MAX };
								*pFLOATLine++ = static_cast<float>(fGray / scalingFactor);
							}
							else
							{
								constexpr double scalingFactor = double{ UCHAR_MAX } *double{ USHRT_MAX };
								*pDWORDLine++ = static_cast<std::uint32_t>(fGray * scalingFactor);
							};
						};
					};

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

						OnWrite(i, j, fRed, fGreen, fBlue);

						if (m_lBitsPerPixel == 8)
						{
							*pBYTELineRed++ = static_cast<std::uint8_t>(fRed);
							*pBYTELineGreen++ = static_cast<std::uint8_t>(fGreen);
							*pBYTELineBlue++ = static_cast<std::uint8_t>(fBlue);
						}
						else if (m_lBitsPerPixel == 16)
						{
							constexpr double scalingFactor = double{ UCHAR_MAX };
							*pWORDLineRed++ = static_cast<std::uint16_t>(fRed * scalingFactor);
							*pWORDLineGreen++ = static_cast<std::uint16_t>(fGreen * scalingFactor);
							*pWORDLineBlue++ = static_cast<std::uint16_t>(fBlue * scalingFactor);
						}
						else if (m_lBitsPerPixel == 32)
						{
							if (m_bFloat)
							{
								constexpr double scalingFactor = 1.0 + double{ UCHAR_MAX };
								*pFLOATLineRed++ = static_cast<float>(fRed / scalingFactor);
								*pFLOATLineGreen++ = static_cast<float>(fGreen / scalingFactor);
								*pFLOATLineBlue++ = static_cast<float>(fBlue / scalingFactor);
							}
							else
							{
								constexpr double scalingFactor = double{ UCHAR_MAX } * double{ USHRT_MAX };
								*pDWORDLineRed++ = static_cast<std::uint32_t>(fRed * scalingFactor);
								*pDWORDLineGreen++ = static_cast<std::uint32_t>(fGreen * scalingFactor);
								*pDWORDLineBlue++ = static_cast<std::uint32_t>(fBlue * scalingFactor);
							};
						};
					};

					pfPixel[0] = 1;
					pfPixel[1] = j + 1;
					pfPixel[2] = 1;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, redBuffer.get(), &nStatus);
					pfPixel[2] = 2;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, greenBuffer.get(), &nStatus);
					pfPixel[2] = 3;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, blueBuffer.get(), &nStatus);
				};

				if (m_pProgress)
					m_pProgress->Progress2(j + 1);
			};

			if (m_pProgress)
				m_pProgress->End2();
		};
	};

	return bResult;
};

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
	CFITSWriteFromMemoryBitmap(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress) :
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

	if (m_Format == TF_UNKNOWN)
		m_Format = GetBestFITSFormat(m_pMemoryBitmap);

	if (m_Format != TF_UNKNOWN)
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
	};

	return true;
};

/* ------------------------------------------------------------------- */

bool CFITSWriteFromMemoryBitmap::OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue)
{
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
		std::wcerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
		exit(1);
	}

	return true;
};

/* ------------------------------------------------------------------- */

bool CFITSWriteFromMemoryBitmap::OnClose()
{
	return true;
};

/* ------------------------------------------------------------------- */

bool WriteFITS(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription, int lISOSpeed, int lGain, double fExposure)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap)
	{
		CFITSWriteFromMemoryBitmap fits(szFileName, pBitmap, pProgress);

		fits.m_ExtraInfo = pBitmap->m_ExtraInfo;
		fits.m_DateTime  = pBitmap->m_DateTime;
		if (szDescription)
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

bool WriteFITS(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription)
{
	return WriteFITS(szFileName, pBitmap, pProgress, FITSFormat, szDescription, /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0);
}

/* ------------------------------------------------------------------- */

bool WriteFITS(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, LPCTSTR szDescription, int lISOSpeed, int lGain, double fExposure)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap)
	{
		CFITSWriteFromMemoryBitmap fits{ szFileName, pBitmap, pProgress };

		fits.m_ExtraInfo = pBitmap->m_ExtraInfo;
		fits.m_DateTime  = pBitmap->m_DateTime;
		if (szDescription)
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

bool WriteFITS(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase * pProgress, LPCTSTR szDescription)
{
	return WriteFITS(szFileName, pBitmap, pProgress, szDescription, /*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0);
}


bool IsFITSPicture(LPCTSTR szFileName, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	return GetFITSInfo(szFileName, BitmapInfo);
};


int	LoadFITSPicture(LPCTSTR szFileName, CBitmapInfo& BitmapInfo, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	int result = -1; // -1 means not a FITS file.

	if (GetFITSInfo(szFileName, BitmapInfo) && BitmapInfo.CanLoad())
	{
		if (ReadFITS(szFileName, rpBitmap, pProgress))
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


void GetFITSExtension(LPCTSTR szFileName, CString& strExtension)
{
	TCHAR szExt[1 + _MAX_EXT];
	CString strExt;

	_tsplitpath(szFileName, nullptr, nullptr, nullptr, szExt);

	strExt = szExt;
	if (!strExt.CompareNoCase(_T(".FITS")))
		strExtension = strExt;
	else if (!strExt.CompareNoCase(_T(".FIT")))
		strExtension = strExt;
	else
		strExtension = ".fts";
}

void GetFITSExtension(fs::path path, CString& strExtension)
{
	fs::path ext = path.extension();

	if (!ext.compare(".fits"))
		strExtension = ext.c_str();
	else if (!ext.compare(".fit"))
		strExtension = ext.c_str();
	else
		strExtension = ".fts";
}