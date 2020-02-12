#include <stdafx.h>
#include "FITSUtil.h"
#include <float.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <map>
#include "Registry.h"
#include "Workspace.h"
#include "Utils.h"

#include <omp.h>

/* ------------------------------------------------------------------- */

static	CComAutoCriticalSection			g_FITSCritical;

CFITSHeader::CFITSHeader()
{
	m_bFloat = FALSE;
	m_fExposureTime = 0;
	m_lISOSpeed     = 0;
	m_lGain         = -1;
	m_CFAType		= CFATYPE_NONE;
	m_Format		= FF_UNKNOWN;
	m_bByteSwap		= FALSE;
	m_bSigned		= FALSE;
	m_DateTime.wYear= 0;
	g_FITSCritical.Lock();
    m_lWidth = 0;
    m_lHeight = 0;
    m_lBitsPerPixel = 0;
    m_lNrChannels = 0;
	m_xBayerOffset = 0;
	m_yBayerOffset = 0;
	
};

/* ------------------------------------------------------------------- */

CFITSHeader::~CFITSHeader()
{
	g_FITSCritical.Unlock();
};

/* ------------------------------------------------------------------- */

inline void ByteSwap(DWORD & InValue)
{
	WORD		wLow, wHigh;
	/*BYTE		bLowLowByte,
				bLowHighByte,
				bHighLowByte,
				bHighHighByte;*/


	wLow = LOWORD(InValue);
	/*bLowLowByte = LOBYTE(wLow);
	bLowHighByte = HIBYTE(wLow);
	wLow = MAKEWORD(bLowHighByte, bLowLowByte);*/

	wHigh = HIWORD(InValue);
	/*bHighLowByte = LOBYTE(wHigh);
	bHighHighByte = HIBYTE(wHigh);
	wHigh = MAKEWORD(bHighHighByte, bHighLowByte);*/

	InValue = MAKELONG(wHigh, wLow);
	InValue <<=5;

/*	DWORD		OutValue;

	OutValue = ((Value & 0xFF) << 24) |
			   ((Value & 0xFF00) << 8) |
			   ((Value & 0xFF0000) >> 8) |
			   ((Value & 0xFF000000) >> 24);

	Value = OutValue;*/
};

/* ------------------------------------------------------------------- */

inline double AdjustColor(double fColor)
{
	if (_finite(fColor) && !_isnan(fColor))
	{
		if (fColor < 0)
			return 0;
		else if (fColor > 255)
			return 255;
		else
			return fColor;
	}
	else
		return 0;
};

/* ------------------------------------------------------------------- */

BOOL	IsFITSRaw()
{
	CWorkspace			workspace;
	DWORD				dwFitsisRaw = 0;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("FITSisRAW"), dwFitsisRaw);

	return dwFitsisRaw;
};

/* ------------------------------------------------------------------- */

CFATYPE GetFITSCFATYPE()
{
	CWorkspace			workspace;
	DWORD				dwFitsisRaw = 0;
	DWORD				dwPattern = 0;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("FITSisRAW"), dwFitsisRaw);
	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("BayerPattern"), dwPattern);


	return dwFitsisRaw ? (CFATYPE)dwPattern : CFATYPE_NONE;
};

/* ------------------------------------------------------------------- */

BOOL	IsFITSRawBayer()
{
	CWorkspace			workspace;
	CString				strInterpolation;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), strInterpolation);

	return !strInterpolation.CompareNoCase(_T("RawBayer"));
};

/* ------------------------------------------------------------------- */

BOOL	IsFITSSuperPixels()
{
	CWorkspace			workspace;
	CString				strInterpolation;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), strInterpolation);

	return !strInterpolation.CompareNoCase(_T("SuperPixels"));
};

/* ------------------------------------------------------------------- */

BOOL	IsFITSBilinear()
{
	CWorkspace			workspace;
	CString				strInterpolation;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), strInterpolation);

	return !strInterpolation.CompareNoCase(_T("Bilinear"));
};

/* ------------------------------------------------------------------- */

BOOL	IsFITSAHD()
{
	CWorkspace			workspace;
	CString				strInterpolation;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), strInterpolation);

	return !strInterpolation.CompareNoCase(_T("AHD"));
};

/* ------------------------------------------------------------------- */

void	GetFITSBrightnessRatio(double & fBrightness)
{
	CWorkspace			workspace;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Brighness"), fBrightness);
	if (fBrightness<=0)
		fBrightness = 1.0;
};

/* ------------------------------------------------------------------- */

void	GetFITSRatio(double & fRed, double & fGreen, double & fBlue)
{
	CWorkspace			workspace;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Brighness"), fGreen);
	if (fGreen<=0)
		fGreen = 1.0;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("RedScale"), fRed);
	fRed *= fGreen;
	if (fRed <= 0)
		fRed = 1.0;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("BlueScale"), fBlue);
	fBlue *= fGreen;
	if (fBlue <= 0)
		fBlue = 1.0;
};

/* ------------------------------------------------------------------- */

bool	IsFITSForcedUnsigned()
{
	bool				bResult = false;
	CWorkspace			workspace;

	workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("ForceUnsigned"), bResult);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReader::ReadKey(LPSTR szKey, double & fValue, CString & strComment)
{
	BOOL				bResult = FALSE;
	int					nStatus = 0;

	if (m_fits)
	{
		CHAR			szComment[500];

		fits_read_key(m_fits, TDOUBLE, szKey, &fValue, szComment, &nStatus);
		if (!nStatus)
		{
			bResult = TRUE;
			strComment = szComment;
		};
	};

	return bResult;
};

BOOL CFITSReader::ReadKey(LPSTR szKey, double & fValue)
{
	BOOL				bResult = FALSE;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_read_key(m_fits, TDOUBLE, szKey, &fValue, nullptr, &nStatus);
		if (!nStatus)
			bResult = TRUE;
	};

	return bResult;
};

BOOL CFITSReader::ReadKey(LPSTR szKey, LONG & lValue)
{
	BOOL				bResult = FALSE;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_read_key(m_fits, TLONG, szKey, &lValue, nullptr, &nStatus);
		if (!nStatus)
			bResult = TRUE;
	};

	return bResult;
};

BOOL CFITSReader::ReadKey(LPSTR szKey, CString & strValue)
{
	BOOL				bResult = FALSE;
	CHAR				szValue[2000];
	int					nStatus = 0;

	if (m_fits)
	{
		fits_read_key(m_fits, TSTRING, szKey, szValue, nullptr, &nStatus);
		if (!nStatus)
		{
			strValue = szValue;
			bResult = TRUE;
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
		CRegistry			reg;
		CString				strPropagated;

		if (!reg.LoadKey(REGENTRY_BASEKEY_FITSSETTINGS, _T("Propagated"), strPropagated))
			strPropagated = "[CRVAL1][CRVAL2][CRTYPE1][CRTYPE2][DEC][RA][OBJCTDEC][OBJCTRA][OBJCTALT][OBJCTAZ][OBJCTHA][SITELAT][SITELONG][TELESCOP][INSTRUME][OBSERVER][RADECSYS]";


		fits_get_hdrspace(m_fits, &nKeywords, nullptr, &nStatus);
		for (LONG i = 1;i<=nKeywords;i++)
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
				strKeyName.Format(_T("[%s]"), (LPCTSTR)CharToCString(szKeyName));

				if (strPropagated.Find(strKeyName) != -1)
					bPropagate = true;
				m_ExtraInfo.AddInfo(
					(LPCTSTR)CharToCString(szKeyName),
					(LPCTSTR)CharToCString(szValue),
					(LPCTSTR)CharToCString(szComment), bPropagate);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

BOOL CFITSReader::Open()
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
	int					nStatus = 0;

	// Close();
	fits_open_diskfile(&m_fits, CStringToChar(m_strFileName), READONLY, &nStatus);
	if (!nStatus && m_fits)
	{
		// File ok - move to the first image HDU
		CString			strSimple;
		LONG			lNrAxis = 0;
		LONG			lWidth  = 0,
						lHeight = 0,
						lNrChannels = 0;
		LONG			lBitFormat = 0;
		double			fExposureTime = 0;
		CString			strMake;
		CString			strISOSpeed;
		CString			CFAPattern("");
		LONG			lISOSpeed = 0;
		LONG			lGain = -1;
		double			xBayerOffset = 0.0, yBayerOffset = 0.0;

		m_bDSI = FALSE;

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

			bResult = ReadKey("NAXIS1", lWidth);
			bResult = ReadKey("NAXIS2", lHeight);
			if (lNrAxis>=3)
				bResult = ReadKey("NAXIS3", lNrChannels);
			else
				lNrChannels = 1;
			bResult = ReadKey("BITPIX", lBitFormat);

			CString				strByteSwap;
			if (ReadKey("BYTESWAP", strByteSwap))
				m_bByteSwap = TRUE;

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
				ZTRACE_RUNTIME("CFA Pattern read from FITS keyword MOSAIC is %s", CStringToChar(CFAPattern));

				m_bDSI = TRUE;
				// Special case of DSI FITS files
				CFAPattern.Trim();
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
					ZTRACE_RUNTIME("CFA Pattern read from FITS keyword BAYERPAT or COLORTYP is %s", CStringToChar(CFAPattern));
				}

				CFAPattern.Trim();

				if (CFAPattern != "")
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

			if (bResult)
			{
				m_lWidth	= lWidth;
				m_lHeight	= lHeight;
				m_lNrChannels = lNrChannels;
				m_fExposureTime = fExposureTime;
				m_strMake	= strMake;
				m_lISOSpeed = lISOSpeed;
				m_lGain     = lGain;
				m_bSigned = FALSE;
				switch (lBitFormat)
				{
				case BYTE_IMG :
					m_lBitsPerPixel = 8;
					m_bFloat = FALSE;
					break;
				case SHORT_IMG :
					m_bSigned = TRUE;  // Fall through intentional
				case USHORT_IMG :
					m_lBitsPerPixel = 16;
					m_bFloat = FALSE;
					break;
				case LONG_IMG :
					m_bSigned = TRUE; // Fall through intentional
				case ULONG_IMG :
					m_lBitsPerPixel = 32;
					m_bFloat = FALSE;
					break;
				case FLOAT_IMG :
					m_lBitsPerPixel = 32;
					m_bFloat = TRUE;
					break;
				case DOUBLE_IMG :
					m_lBitsPerPixel = 64;
					m_bFloat = TRUE;
					break;
				case LONGLONG_IMG :
					m_lBitsPerPixel = 64;
					m_bFloat = FALSE;
					break;
				default :
					// Format not recognized
					bResult = FALSE;
					break;
				};
			};
		}
		else
			bResult = FALSE;

		if (bResult)
			bResult = OnOpen();
		if (!bResult)
		{
			if (m_fits)
			{
				fits_close_file(m_fits, &nStatus);
				m_fits = nullptr;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReader::Read()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = true;
	char error_text[31];			// Error text for FITS errors.
	
	int colours = (m_lNrChannels >= 3) ? 3 : 1;		// 3 ==> RGB, 1 ==> Mono
	int			datatype;			// format of image data 


	double		fMin = 0.0, fMax = 0.0;		// minimum and maximum pixel values for floating point images

	if (m_lNrChannels > 3)
		ZTRACE_RUNTIME("Number of colour channels is %d, only 3 will be used.", m_lNrChannels);

	if (m_fits) do
	{
		BYTE	cNULL = 0;
		WORD	wNULL = 0;
		DWORD	dwNULL = 0;
		LONGLONG llNULL = 0;
		float	fNULL = 0;
		double	dNULL = 0;
		void *	pNULL;

		if (m_lBitsPerPixel == 8)
			pNULL = &cNULL;
		else if (m_lBitsPerPixel == 16)
			pNULL = &wNULL;
		else if (m_lBitsPerPixel == 32)
		{
			if (m_bFloat)
				pNULL = &fNULL;
			else
				pNULL = &dwNULL;
		}
		else if (m_lBitsPerPixel == 64)
		{
			if (m_bFloat)
				pNULL = &dNULL;
			else
				pNULL = &llNULL;
		}

		if (m_pProgress)
			m_pProgress->Start2(nullptr, m_lHeight);

		switch (m_lBitsPerPixel)
		{
		case 8:
			datatype = TBYTE;
			break;
		case 16:
			datatype = TUSHORT;
			if (m_bSigned && IsFITSForcedUnsigned())
				datatype = TSHORT;
			break;
		case 32:
			if (m_bFloat)
				datatype = TFLOAT;
			else if (m_bByteSwap/* || m_bSigned*/)
				datatype = TLONG;
			else
				datatype = TULONG;
			break;
		case 64:
			if (m_bFloat)
				datatype = TDOUBLE;
			else
				datatype = TLONGLONG;
			break;
		};

		LONGLONG fPixel[3] = { 1, 1, 1 };		// want to start reading at column 1, row 1, plane 1

		LONGLONG nelements = m_lWidth * m_lHeight * colours;
		size_t bufferSize = nelements * m_lBitsPerPixel / 8;

		auto buff = std::make_unique<byte []>(bufferSize);

		byte *	byteBuff	= (byte *)buff.get();
		short * shortBuff	= (short *)buff.get();
		WORD *	wordBuff	= (WORD *)buff.get();
		LONG *	longBuff	= (LONG *)buff.get();
		DWORD *	dwordBuff	= (DWORD *)buff.get();
		LONGLONG * longlongBuff = (LONGLONG *)buff.get();
		float *	floatBuff	= (float *)buff.get();
		double * doubleBuff = (double *)buff.get();

		int status = 0;			// used for result of fits_read_pixll call

		//
		// Inhale the entire image (either single colour or RGB).
		//
		fits_read_pixll(m_fits, datatype, fPixel, nelements, pNULL, byteBuff, nullptr, &status);
		if (0 != status)
		{
			fits_get_errstatus(status, error_text);
				ZTRACE_RUNTIME("fits_read_pixll returned a status of %n, error message: %s", status, error_text);
			bResult = false;
			break;
		}

		//
		// Step 1: If the image is in float format, need to extract the minimum and maximum pixel values.
		//
		if (m_bFloat)
		{
			double localMin = 0, localMax = 0;
#if defined(_OPENMP)
#pragma omp parallel default(none) shared(fMin, fMax) firstprivate(localMin, localMax)
			{
#pragma omp for
#endif
				for (LONGLONG element = 0; element < nelements; ++element)
				{
					double	fValue = 0.0;
					
					if (datatype == TFLOAT)
						fValue = floatBuff[element];	// Short (4 byte) floating point
					else
						fValue = doubleBuff[element];	// Long (8 byte) floating point

					if (!_isnan(fValue))
					{
						localMin = min(localMin, fValue);
						localMax = max(localMax, fValue);
					};
				}
#if defined(_OPENMP)
#pragma omp critical
#endif
				{
					fMin = localMin < fMin ? localMin : fMin; // For non-OMP case this is equal to fMin = localMin
					fMax = localMax > fMax ? localMax : fMax; // For non-OMP case this is equal to fMax = localMax
				}
#if defined(_OPENMP)
			}
#endif
			double		fZero,
				fScale;

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
		double fRed = 0.0, fGreen = 0.0, fBlue = 0.0;
		unsigned long greenOffset = m_lWidth * m_lHeight;		// index into buffer of the green image
		unsigned long blueOffset = 2 * greenOffset;				// index into buffer of the blue image

		long	rowProgress = 0;

#if defined(_OPENMP)
#pragma omp parallel for default(none)
#endif
		for (long row = 0; row < m_lHeight; ++row)
		{
			for (long col = 0; col < m_lWidth; ++col)
			{
				long index = col + (row * m_lWidth);	// index into the image for this plane

				DWORD redValue, greenValue, blueValue;	// Use only for some cases

				if (1 == colours)
				{
					//
					// This is a monochrome image 
					//
					switch (datatype)
					{
					case TBYTE:
						fRed = byteBuff[index];
						break;
					case TUSHORT:
					case TSHORT:
						fRed = ((double)(wordBuff[index])) / 256.0;
						// Currently don't handle signed type.
						break;
					case TULONG:
					case TLONG:
						redValue = dwordBuff[index];
						if (m_bByteSwap)
							ByteSwap(redValue);
						fRed = ((double)(redValue)) / 256.0 / 65536.0;
						break;
					case TFLOAT:
						fRed = (double)(floatBuff[index]);
						fRed = (fRed - fMin) / (fMax - fMin) * 256.0;
						break;
					case TLONGLONG:
						fRed = (double)(longlongBuff[index]);
						fRed = fRed / 256.0 / 65536.0;
						break;
					case TDOUBLE:
						fRed = doubleBuff[index];
						fRed = (fRed - fMin) / (fMax - fMin) * 256.0;
						break;
					}
					//
					// Set green and blue to the same as red
					// 
					fGreen = fBlue = fRed;
				}
				else  
				{
					//
					// We assume this is a 3 colour image with each colour in a separate image plane
					//
					switch (datatype)
					{
					case TBYTE:
						fRed = byteBuff[index];
						fGreen = byteBuff[greenOffset + index];
						fBlue = byteBuff[blueOffset + index];
						break;
					case TUSHORT:
					case TSHORT:
						fRed = ((double)(wordBuff[index])) / 256.0;
						fGreen = ((double)(wordBuff[greenOffset + index])) / 256.0;
						fBlue = ((double)(wordBuff[blueOffset + index])) / 256.0;
						// Currently don't handle signed type.
						break;
					case TULONG:
					case TLONG:
						redValue = dwordBuff[index];
						greenValue = dwordBuff[greenOffset + index];
						blueValue = dwordBuff[blueOffset + index];
						if (m_bByteSwap)
						{
							ByteSwap(redValue);
							ByteSwap(greenValue);
							ByteSwap(blueValue);
						}
						fRed = ((double)(redValue)) / 256.0 / 65536.0;
						fGreen = ((double)(greenValue)) / 256.0 / 65536.0;
						fBlue = ((double)(blueValue)) / 256.0 / 65536.0;
						break;
					case TFLOAT:
						fRed = (double)(floatBuff[index]);
						fGreen = (double)(floatBuff[greenOffset + index]);
						fBlue = (double)(floatBuff[blueOffset + index]);
						fRed = (fRed - fMin) / (fMax - fMin) * 256.0;
						fGreen = (fGreen - fMin) / (fMax - fMin) * 256.0;
						fBlue = (fBlue - fMin) / (fMax - fMin) * 256.0;
						break;
					case TLONGLONG:
						fRed = (double)(longlongBuff[index]);
						fGreen = (double)(longlongBuff[greenOffset + index]);
						fBlue = (double)(longlongBuff[blueOffset + index]);
						fRed = fRed / 256.0 / 65536.0;
						fGreen = fGreen / 256.0 / 65536.0;
						fGreen = fGreen / 256.0 / 65536.0;
						break;
					case TDOUBLE:
						fRed = doubleBuff[index];
						fGreen = doubleBuff[greenOffset + index];
						fBlue = doubleBuff[blueOffset + index];
						fRed = (fRed - fMin) / (fMax - fMin) * 256.0;
						fGreen = (fGreen - fMin) / (fMax - fMin) * 256.0;
						fBlue = (fBlue - fMin) / (fMax - fMin) * 256.0;
						break;
					}

				}

				OnRead(col, row, AdjustColor(fRed), AdjustColor(fGreen), AdjustColor(fBlue));

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
		}

#if (0)
		lScanLineSize = m_lWidth * m_lBitsPerPixel/8;

		if (m_lNrChannels == 1)
		{
			pScanLine = (VOID *)malloc(lScanLineSize);
		}
		else
		{
			pScanLineRed   = (VOID *)malloc(lScanLineSize);
			pScanLineGreen = (VOID *)malloc(lScanLineSize);
			pScanLineBlue  = (VOID *)malloc(lScanLineSize);
		};
		if (pScanLine || (pScanLineRed && pScanLineGreen && pScanLineBlue))
		{
			bResult = TRUE;
			LONG		i, j, k;
			int			datatype;
			double		fMin, fMax;
			int			nStatus = 0;

			switch (m_lBitsPerPixel)
			{
			case 8 :
				datatype = TBYTE;
				break;
			case 16 :
				datatype = TUSHORT;
				if (m_bSigned && IsFITSForcedUnsigned())
					datatype = TSHORT;
				break;
			case 32 :
				if (m_bFloat)
					datatype = TFLOAT;
				else if (m_bByteSwap/* || m_bSigned*/)
					datatype = TLONG;
				else
					datatype = TULONG;
				break;
			case 64 :
				datatype = TFLOAT;
				break;
			};

			// Step 1 - read min and max values
			if (m_bFloat)
			{
				BOOL			bFirst = TRUE;
				float *			pBuffer;

				pBuffer = (float*)malloc(sizeof(float)*m_lWidth*m_lNrChannels);

				for (j = 0;j<m_lHeight;j++)
				{
					LONG	pfPixel[3];
					float *	pValue = pBuffer;

					pfPixel[0] = 1;
					pfPixel[1] = j+1;
					pfPixel[2] = 1;

					nStatus = 0;
					fits_read_pix(m_fits, TFLOAT, pfPixel, m_lWidth * m_lNrChannels, &fNULL, pBuffer, nullptr, &nStatus);

					if (!nStatus)
					{
						for (i = 0;i<m_lWidth;i++)
						{
							for (k = 0;k<m_lNrChannels;k++)
							{
								float	fValue = *pValue;

								if (!_isnan(fValue))
								{
									if (bFirst)
									{
										fMin = fMax = fValue;
										bFirst = FALSE;
									}
									else
									{
										fMin = min(fMin, static_cast<double>(fValue));
										fMax = max(fMax, static_cast<double>(fValue));
									};
								};
								pValue++;
							};
						};
					};
				};
				free(pBuffer);

				double		fZero,
							fScale;

				if (ReadKey("BZERO", fZero) && ReadKey("BSCALE", fScale))
				{
					fMax = (fMax + fZero) / fScale;
					fMin = fZero / fScale;
				}
				else if (fMin >=0 && fMin <= 1 && fMax>=0 && fMax <= 1)
				{
					fMin = 0;
					fMax = 1;
				}
				if (m_bDSI && (fMax>1))
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

			// Step 2 - read the values
			for (j = 0;j<m_lHeight && bResult;j++)
			{
				LONG	pfPixel[3];

				if (m_lNrChannels == 1)
				{
					BYTE *  pBYTELine				= (BYTE *)pScanLine;
					WORD *	pWORDLine				= (WORD *)pScanLine;
					signed short int * pSHORTLine	= (signed short int*)pScanLine;
					DWORD * pDWORDLine				= (DWORD *)pScanLine;
					LONG *	pLONGLine				= (LONG *)pScanLine;
					float *	pFLOATLine				= (float *)pScanLine;

					pfPixel[0] = 1;
					pfPixel[1] = j+1;
					pfPixel[2] = 1;

					nStatus = 0;
					fits_read_pix(m_fits, datatype, pfPixel, m_lWidth, pNULL, pScanLine, nullptr, &nStatus);

					for (i = 0;i<m_lWidth && bResult && !nStatus;i++)
					{
                        double fRed = 0;
                        double fGreen = 0;
                        double fBlue = 0;

						if (m_lBitsPerPixel == 8)
						{
							fRed	= *(pBYTELine);
							fGreen	= *(pBYTELine);
							fBlue	= *(pBYTELine);
							pBYTELine ++;
						}
						else if (m_lBitsPerPixel == 16)
						{
							if (true)//!m_bSigned)
							{
								fRed	= ((double)(*(pWORDLine)))/256.0;
								fGreen	= ((double)(*(pWORDLine)))/256.0;
								fBlue	= ((double)(*(pWORDLine)))/256.0;
								pWORDLine ++;
							}
							else
							{
								fRed	= ((double)(*(pSHORTLine)))/256.0;
								fGreen	= ((double)(*(pSHORTLine)))/256.0;
								fBlue	= ((double)(*(pSHORTLine)))/256.0;
								pSHORTLine ++;
							};
						}
						else if (m_lBitsPerPixel == 32)
						{
							if (m_bFloat)
							{
								fRed	= (double)(*(pFLOATLine));
								fGreen	= (double)(*(pFLOATLine));
								fBlue	= (double)(*(pFLOATLine));

								fRed   = (fRed - fMin)/(fMax-fMin)  * 256.0;
								fGreen = (fGreen - fMin)/(fMax-fMin)* 256.0;
								fBlue  = (fBlue - fMin)/(fMax-fMin) * 256.0;

								pFLOATLine ++;
								pWORDLine+=2;
							}
							else
							{
								if (m_bByteSwap)
								{
									LPLONG			lpValue = (LPLONG)pDWORDLine;
									LONG			lValue;

									ByteSwap(*(pDWORDLine));

									lValue = *lpValue;
									(*(pDWORDLine)) = lValue;
								};
								fRed = fGreen = fBlue = ((double)(*(pDWORDLine)))/256.0/65536.0;
								pDWORDLine ++;
							};
						};

						bResult = OnRead(i, j, AdjustColor(fRed), AdjustColor(fGreen), AdjustColor(fBlue));

					};
					if (m_pProgress)
						m_pProgress->Progress2(nullptr, j+1);
				}
				else
				{
					BYTE *  pBYTELineRed	= (BYTE *)pScanLineRed;
					WORD *	pWORDLineRed	= (WORD *)pScanLineRed;
					DWORD * pDWORDLineRed	= (DWORD *)pScanLineRed;
					float *	pFLOATLineRed	= (float *)pScanLineRed;
					BYTE *  pBYTELineGreen	= (BYTE *)pScanLineGreen;
					WORD *	pWORDLineGreen	= (WORD *)pScanLineGreen;
					DWORD * pDWORDLineGreen	= (DWORD *)pScanLineGreen;
					float *	pFLOATLineGreen	= (float *)pScanLineGreen;
					BYTE *  pBYTELineBlue	= (BYTE *)pScanLineBlue;
					WORD *	pWORDLineBlue	= (WORD *)pScanLineBlue;
					DWORD * pDWORDLineBlue	= (DWORD *)pScanLineBlue;
					float *	pFLOATLineBlue	= (float *)pScanLineBlue;

					pfPixel[0] = 1;
					pfPixel[1] = j+1;
					pfPixel[2] = 1;
					nStatus = 0;
					fits_read_pix(m_fits, datatype, pfPixel, m_lWidth, pNULL, pScanLineRed, nullptr, &nStatus);
					pfPixel[2] = 2;
					fits_read_pix(m_fits, datatype, pfPixel, m_lWidth, pNULL, pScanLineGreen, nullptr, &nStatus);
					pfPixel[2] = 3;
					fits_read_pix(m_fits, datatype, pfPixel, m_lWidth, pNULL, pScanLineBlue, nullptr, &nStatus);

					for (i = 0;i<m_lWidth && bResult && !nStatus;i++)
					{
                        double fRed = 0;
                        double fGreen = 0;
                        double fBlue = 0;

						if (m_lBitsPerPixel == 8)
						{
							fRed	= *(pBYTELineRed);
							fGreen	= *(pBYTELineGreen);
							fBlue	= *(pBYTELineBlue);
							pBYTELineRed ++;
							pBYTELineGreen ++;
							pBYTELineBlue ++;
						}
						else if (m_lBitsPerPixel == 16)
						{
							fRed	= ((double)(*(pWORDLineRed)))/256.0;
							fGreen	= ((double)(*(pWORDLineGreen)))/256.0;
							fBlue	= ((double)(*(pWORDLineBlue)))/256.0;
							pWORDLineRed ++;
							pWORDLineGreen ++;
							pWORDLineBlue ++;
						}
						else if (m_lBitsPerPixel == 32)
						{
							if (m_bFloat)
							{
								fRed	= (double)(*(pFLOATLineRed));
								fGreen	= (double)(*(pFLOATLineGreen));
								fBlue	= (double)(*(pFLOATLineBlue));

								fRed   = (fRed - fMin)/(fMax-fMin) * 256.0;
								fGreen = (fGreen - fMin)/(fMax-fMin) * 256.0;
								fBlue  = (fBlue - fMin)/(fMax-fMin) * 256.0;

								pFLOATLineRed ++;
								pFLOATLineGreen ++;
								pFLOATLineBlue ++;
							}
							else
							{
								fRed	= ((double)(*(pDWORDLineRed)))/256.0/65536.0;
								fGreen	= ((double)(*(pDWORDLineGreen)))/256.0/65536.0;
								fBlue	= ((double)(*(pDWORDLineBlue)))/256.0/65536.0;
								pDWORDLineRed ++;
								pDWORDLineGreen ++;
								pDWORDLineBlue ++;
							};
						};

						bResult = OnRead(i, j, AdjustColor(fRed), AdjustColor(fGreen), AdjustColor(fBlue));

					};
					if (m_pProgress)
						m_pProgress->Progress2(nullptr, j+1);
				};
			};

			if (pScanLine)
				free(pScanLine);
			if (pScanLineRed)
				free(pScanLineRed);
			if (pScanLineGreen)
				free(pScanLineGreen);
			if (pScanLineBlue)
				free(pScanLineBlue);
			if (m_pProgress)
				m_pProgress->End2();
		}
#endif
	} while (false);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReader::Close()
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
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
};


/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CFITSReadInMemoryBitmap : public CFITSReader
{
private :
	CMemoryBitmap **			m_ppBitmap;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;

public :
	CFITSReadInMemoryBitmap(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress *	pProgress)
		: CFITSReader(szFileName, pProgress)
	{
		m_ppBitmap = ppBitmap;
	};

	virtual ~CFITSReadInMemoryBitmap() { Close(); };

	virtual BOOL Close() { return OnClose(); };

	virtual BOOL	OnOpen();
	virtual BOOL	OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue);
	virtual BOOL	OnClose();
};

/* ------------------------------------------------------------------- */

BOOL CFITSReadInMemoryBitmap::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;

	if (m_lNrChannels == 1)
	{
		if (m_lBitsPerPixel == 8)
		{
			m_pBitmap.Attach(new C8BitGrayBitmap());
			ZTRACE_RUNTIME("Creating 8 Gray bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (m_lBitsPerPixel == 16)
		{
			m_pBitmap.Attach(new C16BitGrayBitmap());
			ZTRACE_RUNTIME("Creating 16 Gray bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (m_lBitsPerPixel == 32 || m_lBitsPerPixel == 64)
		{
			if (m_bFloat)
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
	else if (m_lNrChannels==3)
	{
		if (m_lBitsPerPixel == 8)
		{
			m_pBitmap.Attach(new C24BitColorBitmap());
			ZTRACE_RUNTIME("Creating 8 RGB bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (m_lBitsPerPixel == 16)
		{
			m_pBitmap.Attach(new C48BitColorBitmap());
			ZTRACE_RUNTIME("Creating 16 RGB bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (m_lBitsPerPixel == 32 || m_lBitsPerPixel == 64)
		{
			if (m_bFloat)
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
		bResult = m_pBitmap->Init(m_lWidth, m_lHeight);

		if ((m_CFAType != CFATYPE_NONE) && (m_lNrChannels != 1))// || (m_lBitsPerPixel != 16)))
			m_CFAType = CFATYPE_NONE;

		//
		// If the user has explicitly set that this FITS file is a Bayer format RAW file,
		// then the user will also have explicitly set the Bayer pattern that's to be used.
		// In this case we use that and set the Bayer offsets (if any) to zero
		//
		bool isRaw = IsFITSRaw();
		
		if ((m_lNrChannels == 1) &&
			((m_lBitsPerPixel == 16) || (m_lBitsPerPixel == 32)))
		{
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
		else
		{
			// 
			// Set CFA type to none even if the FITS header specified a value
			//
			m_CFAType = CFATYPE_NONE;

			static bool eightBitWarningIssued = false;
			if (!eightBitWarningIssued &&
				(m_lNrChannels == 1) &&
				(m_lBitsPerPixel == 8))
			{
				CString errorMessage;
				errorMessage.Format(IDS_8BIT_FITS_NODEBAYER);
#if defined(_CONSOLE)
				std::cerr << errorMessage;
#else
				AfxMessageBox(errorMessage, MB_OK | MB_ICONWARNING);
#endif
				// Remember we already said we won't do that!
				eightBitWarningIssued = true;
			}
		}

		if (m_CFAType != CFATYPE_NONE)
		{
			CCFABitmapInfo *		pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(m_pBitmap.m_p);
			//C16BitGrayBitmap *		pGray16Bitmap = dynamic_cast<C16BitGrayBitmap *>(m_pBitmap.m_p);

			if (pCFABitmapInfo)
			{
				m_pBitmap->SetCFA(TRUE);
				pCFABitmapInfo->SetCFAType(m_CFAType);
				//
				// Set the CFA/Bayer offset information into the CFABitmapInfo
				//
				pCFABitmapInfo->setXoffset(m_xBayerOffset);
				pCFABitmapInfo->setYoffset(m_yBayerOffset);
				if (::IsCYMGType(m_CFAType))
					pCFABitmapInfo->UseBilinear(TRUE);
				else if (IsFITSRawBayer())
					pCFABitmapInfo->UseRawBayer(TRUE);
				else if (IsFITSSuperPixels())
					pCFABitmapInfo->UseSuperPixels(TRUE);
				else if (IsFITSBilinear())
					pCFABitmapInfo->UseBilinear(TRUE);
				else if (IsFITSAHD())
					pCFABitmapInfo->UseAHD(TRUE);

				// Retrieve ratios
				GetFITSRatio(m_fRedRatio, m_fGreenRatio, m_fBlueRatio);
			};
		}
		else
			GetFITSBrightnessRatio(m_fBrightnessRatio);

		m_pBitmap->SetMaster(FALSE);
		if (m_fExposureTime)
			m_pBitmap->SetExposure(m_fExposureTime);
		if (m_lISOSpeed)
			m_pBitmap->SetISOSpeed(m_lISOSpeed);
		if (m_lGain >= 0)
			m_pBitmap->SetGain(m_lGain);
		m_pBitmap->m_DateTime = m_DateTime;

		CString			strDescription;

		if (m_strMake.GetLength())
			strDescription.Format(_T("FITS (%s)"), (LPCTSTR)m_strMake);
		else
			strDescription	= _T("FITS");
		m_pBitmap->SetDescription(strDescription);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReadInMemoryBitmap::OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue)
{
	//
	// Define maximal scaled pixel value of 255 (will be multiplied up later)
	//
	double maxValue = 255.;
	
	try
	{
		if (m_pBitmap)
		{
			if (m_lNrChannels == 1)
			{
				if (m_CFAType != CFATYPE_NONE)
				{
					switch (::GetBayerColor(lX, lY, m_CFAType, m_xBayerOffset, m_yBayerOffset))
					{
					case BAYER_BLUE:
						fRed = min(maxValue, fRed *= m_fBlueRatio);
						break;
					case BAYER_GREEN:
						fRed = min(maxValue, fRed *= m_fGreenRatio);
						break;
					case BAYER_RED:
						fRed = min(maxValue, fRed *= m_fRedRatio);
						break;
					};
				}
				else
				{
					fRed = min(maxValue, fRed *= m_fBrightnessRatio);
					fGreen = min(maxValue, fGreen *= m_fBrightnessRatio);
					fBlue = min(maxValue, fBlue *= m_fBrightnessRatio);
				};
				m_pBitmap->SetPixel(lX, lY, fRed);
			}
			else
				m_pBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);
		};
	}
	catch (ZException e)
	{
		CString errorMessage;
		CString name(CharToCString(e.name()));
		CString fileName(CharToCString(e.locationAtIndex(0)->fileName()));
		CString functionName(CharToCString(e.locationAtIndex(0)->functionName()));
		CString text(CharToCString(e.text(0)));

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
	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReadInMemoryBitmap::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;

	if (m_pBitmap)
	{
		bResult = TRUE;
		m_pBitmap.CopyTo(m_ppBitmap);
		m_pBitmap->m_ExtraInfo = m_ExtraInfo;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	ReadFITS(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress *	pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;
	CFITSReadInMemoryBitmap	fits(szFileName, ppBitmap, pProgress);

	if (ppBitmap)
	{
		bResult = fits.Open();
		if (bResult)
			bResult = fits.Read();
		// if (bResult) bResult = fits.Close();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	GetFITSInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;
	BOOL					bContinue = TRUE;
	CFITSReader				fits(szFileName, nullptr);

	// Exclude JPEG, PNG or TIFF format
	{
		TCHAR				szExt[1+_MAX_EXT];
		CString				strExt;

		_tsplitpath(szFileName, nullptr, nullptr, nullptr, szExt);
		strExt = szExt;

		if (!strExt.CompareNoCase(_T(".JPG")) ||
			!strExt.CompareNoCase(_T(".JPEG")) ||
			!strExt.CompareNoCase(_T(".PNG")) ||
			!strExt.CompareNoCase(_T(".TIF")) ||
			!strExt.CompareNoCase(_T(".TIFF")))
			bContinue = FALSE;
	}
	if (bContinue && fits.Open())
	{
		if (fits.m_strMake.GetLength()) BitmapInfo.m_strFileType.Format(_T("FITS (%s)"), (LPCTSTR)fits.m_strMake);
		
		BitmapInfo.m_strFileType	= _T("FITS");
		BitmapInfo.m_strFileName	= szFileName;
		BitmapInfo.m_lWidth			= fits.Width();
		BitmapInfo.m_lHeight		= fits.Height();
		BitmapInfo.m_lBitPerChannel = fits.BitPerChannels();
		BitmapInfo.m_lNrChannels	= fits.NrChannels();
		BitmapInfo.m_bFloat			= fits.IsFloat();
		BitmapInfo.m_CFAType		= fits.GetCFAType();
		BitmapInfo.m_bMaster		= fits.IsMaster();
		BitmapInfo.m_lISOSpeed		= fits.GetISOSpeed();
		BitmapInfo.m_lGain			= fits.GetGain();
		BitmapInfo.m_bCanLoad		= TRUE;
		BitmapInfo.m_fExposure		= fits.GetExposureTime();
		BitmapInfo.m_bFITS16bit	    = (fits.NrChannels() == 1) &&
									  ((fits.BitPerChannels() == 16) || (fits.BitPerChannels() == 32));
		BitmapInfo.m_DateTime		= fits.GetDateTime();
		BitmapInfo.m_ExtraInfo		= fits.m_ExtraInfo;
		BitmapInfo.m_xBayerOffset	= fits.getXOffset();
		BitmapInfo.m_yBayerOffset	= fits.getYOffset();
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL	CFITSWriter::WriteKey(LPSTR szKey, double fValue, LPSTR szComment)
{
	BOOL				bResult = FALSE;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_write_key(m_fits, TDOUBLE, szKey, &fValue, szComment, &nStatus);
		if (!nStatus)
			bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CFITSWriter::WriteKey(LPSTR szKey, LONG lValue, LPSTR szComment)
{
	BOOL				bResult = FALSE;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_write_key(m_fits, TLONG, szKey, &lValue, szComment, &nStatus);
		if (!nStatus)
			bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CFITSWriter::WriteKey(LPSTR szKey, LPCTSTR szValue, LPSTR szComment)
{
	BOOL				bResult = FALSE;
	int					nStatus = 0;

	if (m_fits)
	{
		fits_write_key(m_fits, TSTRING, szKey, (void*)szValue, szComment, &nStatus);
		if (!nStatus)
			bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CFITSWriter::WriteAllKeys()
{
	BOOL				bFound = FALSE;

	// Check if DATE-OBS is already in the list of Extra Info
	for (LONG i = 0;i<m_ExtraInfo.m_vExtras.size() && !bFound;i++)
	{
		CExtraInfo &ei = m_ExtraInfo.m_vExtras[i];

		if (ei.m_strName.CompareNoCase(_T("DATE-OBS")))
			bFound = TRUE;
	};

	if (!bFound && m_DateTime.wYear)
	{
		// Add DATE-OBS to the list
		CString			strDateTime;

		strDateTime.Format(_T("%04d-%02d-%02dT%02d:%02d:%02d"),
						   m_DateTime.wYear, m_DateTime.wMonth, m_DateTime.wDay,
						   m_DateTime.wHour, m_DateTime.wMinute, m_DateTime.wSecond);

		m_ExtraInfo.AddInfo(_T("DATE-OBS"), strDateTime);
	};

	if (m_fits && m_ExtraInfo.m_vExtras.size())
	{
		int				nStatus = 0;


		for (LONG i = 0;i<m_ExtraInfo.m_vExtras.size();i++)
		{
			CExtraInfo &ei = m_ExtraInfo.m_vExtras[i];
			CHAR			szValue[FLEN_VALUE];

			// check that the keyword is not already used
			fits_read_key(m_fits, TSTRING, (LPCSTR)CT2A(ei.m_strName, CP_UTF8), szValue, nullptr, &nStatus);
			if (nStatus)
			{
				nStatus = 0;
				CHAR		szCard[FLEN_CARD];
				int			nType;
				CString		strTemplate;

				if (ei.m_strComment.GetLength())
					strTemplate.Format(_T("%s = %s / %s"), ei.m_strName, ei.m_strValue, ei.m_strComment);
				else
					strTemplate.Format(_T("%s = %s"), ei.m_strName, ei.m_strValue);

				fits_parse_template((LPSTR)CT2A(strTemplate, CP_UTF8), szCard, &nType, &nStatus);
				fits_write_record(m_fits, szCard, &nStatus);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CFITSWriter::SetFormat(LONG lWidth, LONG lHeight, FITSFORMAT FITSFormat, CFATYPE CFAType)
{
	ZFUNCTRACE_RUNTIME();
	m_CFAType	= CFAType;

	m_lWidth	= lWidth;
	m_lHeight	= lHeight;
	m_bFloat	= FALSE;
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
		m_bFloat = TRUE;
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
		m_bFloat = TRUE;
		break;
	};
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriter::Open()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	CString			strFileName = m_strFileName;

	// Close();

	// Create a new fits file
	int				nStatus = 0;

	DeleteFile((LPCTSTR)strFileName);
	fits_create_diskfile(&m_fits, (LPCSTR)CT2A(strFileName, CP_UTF8), &nStatus);
	if (m_fits && !nStatus)
	{
		bResult = OnOpen();
		if (bResult)
		{
			// Create the image
			long		nAxes[3];
			long		nAxis;
			long		nBitPixels;

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
			if (!nStatus)
			{
				bResult = TRUE;

				if (m_lISOSpeed)
					bResult = bResult && WriteKey("ISOSPEED", m_lISOSpeed);
				if (m_lGain >= 0)
					bResult = bResult && WriteKey("GAIN", m_lGain);
				if (m_fExposureTime)
				{
					bResult = bResult && WriteKey("EXPTIME", m_fExposureTime, "Exposure time (in seconds)");
					bResult = bResult && WriteKey("EXPOSURE", m_fExposureTime, "Exposure time (in seconds)");
				};
				if ((m_lNrChannels == 1) && (m_CFAType != CFATYPE_NONE))
					bResult = bResult && WriteKey("DSSCFATYPE", (LONG)m_CFAType);

				CString			strSoftware = "DeepSkyStacker ";
				strSoftware += VERSION_DEEPSKYSTACKER;

				WriteKey("SOFTWARE", (LPCTSTR)strSoftware);
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

BOOL CFITSWriter::Write()
{
	BOOL			bResult = FALSE;


	if (m_fits)
	{
		LONG			lScanLineSize;
		VOID *			pScanLine = nullptr;
		VOID *			pScanLineRed = nullptr;
		VOID *			pScanLineGreen = nullptr;
		VOID *			pScanLineBlue = nullptr;

		lScanLineSize = m_lWidth * m_lBitsPerPixel/8;
		if (m_lNrChannels == 1)
		{
			pScanLine = (VOID *)malloc(lScanLineSize);
		}
		else
		{
			pScanLineRed   = (VOID *)malloc(lScanLineSize);
			pScanLineGreen = (VOID *)malloc(lScanLineSize);
			pScanLineBlue  = (VOID *)malloc(lScanLineSize);
		};
		if (pScanLine || (pScanLineRed && pScanLineGreen && pScanLineBlue))
		{
			bResult = TRUE;
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
				m_pProgress->Start2(nullptr, m_lHeight);

			for (LONG j = 0;j<m_lHeight;j++)
			{
				LONG		pfPixel[3];

				if (m_lNrChannels == 1)
				{
					BYTE *  pBYTELine	= (BYTE *)pScanLine;
					WORD *	pWORDLine	= (WORD *)pScanLine;
					DWORD * pDWORDLine	= (DWORD *)pScanLine;
					float *	pFLOATLine	= (float *)pScanLine;

					for (LONG i = 0;i<m_lWidth;i++)
					{
						double		fRed, fGreen, fBlue;

						OnWrite(i, j, fRed, fGreen, fBlue);

						double			fGray;

						if (m_CFAType != CFATYPE_NONE)
						{
							fGray = max(fRed, max(fGreen, fBlue));
							// 2 out of 3 should be 0
						}
						else
						{
							// Convert to gray scale
							double		H, S, L;
							ToHSL(fRed, fGreen, fBlue, H, S, L);
							fGray = L*255.0;
						};

						if (m_lBitsPerPixel == 8)
						{
							*(pBYTELine) = fGray;
							pBYTELine ++;
						}
						else if (m_lBitsPerPixel == 16)
						{
							(*(pWORDLine)) = fGray * 256.0;
							pWORDLine ++;
						}
						else if (m_lBitsPerPixel == 32)
						{
							if (m_bFloat)
							{
								(*(pFLOATLine)) = fGray / 256.0;
								pFLOATLine ++;
							}
							else
							{
								(*(pDWORDLine)) = fGray * 256.0 *65536.0;
								pDWORDLine ++;
							};
						};
					};

					pfPixel[0] = 1;
					pfPixel[1] = j+1;
					pfPixel[2] = 1;

					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, pScanLine, &nStatus);
				}
				else
				{
					BYTE *  pBYTELineRed	= (BYTE *)pScanLineRed;
					WORD *	pWORDLineRed	= (WORD *)pScanLineRed;
					DWORD * pDWORDLineRed	= (DWORD *)pScanLineRed;
					float *	pFLOATLineRed	= (float *)pScanLineRed;
					BYTE *  pBYTELineGreen	= (BYTE *)pScanLineGreen;
					WORD *	pWORDLineGreen	= (WORD *)pScanLineGreen;
					DWORD * pDWORDLineGreen	= (DWORD *)pScanLineGreen;
					float *	pFLOATLineGreen	= (float *)pScanLineGreen;
					BYTE *  pBYTELineBlue	= (BYTE *)pScanLineBlue;
					WORD *	pWORDLineBlue	= (WORD *)pScanLineBlue;
					DWORD * pDWORDLineBlue	= (DWORD *)pScanLineBlue;
					float *	pFLOATLineBlue	= (float *)pScanLineBlue;

					for (LONG i = 0;i<m_lWidth;i++)
					{
						double		fRed, fGreen, fBlue;

						OnWrite(i, j, fRed, fGreen, fBlue);

						if (m_lBitsPerPixel == 8)
						{
							*(pBYTELineRed)		= fRed;
							*(pBYTELineGreen)	= fGreen;
							*(pBYTELineBlue)	= fBlue;
							pBYTELineRed ++;
							pBYTELineGreen ++;
							pBYTELineBlue ++;
						}
						else if (m_lBitsPerPixel == 16)
						{
							(*(pWORDLineRed))	= fRed *256.0;
							(*(pWORDLineGreen)) = fGreen *256.0;
							(*(pWORDLineBlue))	= fBlue *256.0;
							pWORDLineRed++;
							pWORDLineGreen++;
							pWORDLineBlue++;
						}
						else if (m_lBitsPerPixel == 32)
						{
							if (m_bFloat)
							{
								(*(pFLOATLineRed))	= fRed / 256.0;
								(*(pFLOATLineGreen))= fGreen / 256.0;
								(*(pFLOATLineBlue)) = fBlue / 256.0;
								pFLOATLineRed++;
								pFLOATLineGreen++;
								pFLOATLineBlue++;
							}
							else
							{
								(*(pDWORDLineRed))  = fRed * 256.0 * 65536.0;
								(*(pDWORDLineGreen))= fGreen * 256.0 * 65536.0;
								(*(pDWORDLineBlue)) = fBlue * 256.0 *65536.0;
								pDWORDLineRed ++;
								pDWORDLineGreen ++;
								pDWORDLineBlue ++;
							};
						};
					};

					pfPixel[0] = 1;
					pfPixel[1] = j+1;
					pfPixel[2] = 1;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, pScanLineRed, &nStatus);
					pfPixel[2] = 2;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, pScanLineGreen, &nStatus);
					pfPixel[2] = 3;
					fits_write_pix(m_fits, datatype, pfPixel, m_lWidth, pScanLineBlue, &nStatus);
				};

				if (m_pProgress)
					m_pProgress->Progress2(nullptr, j+1);
			};
			if (pScanLine)
				free(pScanLine);
			if (pScanLineRed)
				free(pScanLineRed);
			if (pScanLineGreen)
				free(pScanLineGreen);
			if (pScanLineBlue)
				free(pScanLineBlue);

			if (m_pProgress)
				m_pProgress->End2();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriter::Close()
{
	BOOL			bResult = TRUE;
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
	CMemoryBitmap *			m_pMemoryBitmap;

private :
	FITSFORMAT	GetBestFITSFormat(CMemoryBitmap * pBitmap);

public :
	CFITSWriteFromMemoryBitmap(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress) :
	   CFITSWriter(szFileName, pProgress)
	{
		m_pMemoryBitmap = pBitmap;
	};

	virtual BOOL Close() { return OnClose(); }

	virtual ~CFITSWriteFromMemoryBitmap()
	{
		Close();
	};

	virtual BOOL	OnOpen();
	virtual BOOL	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue);
	virtual BOOL	OnClose();
};

/* ------------------------------------------------------------------- */

FITSFORMAT	CFITSWriteFromMemoryBitmap::GetBestFITSFormat(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	FITSFORMAT						Result = FF_UNKNOWN;
	C24BitColorBitmap *			p24Color = dynamic_cast<C24BitColorBitmap *>(pBitmap);
	C48BitColorBitmap *			p48Color = dynamic_cast<C48BitColorBitmap *>(pBitmap);
	C96BitColorBitmap *			p96Color = dynamic_cast<C96BitColorBitmap *>(pBitmap);
	C96BitFloatColorBitmap*		p96FloatColor = dynamic_cast<C96BitFloatColorBitmap*>(pBitmap);
	C8BitGrayBitmap*				p8Gray	= dynamic_cast<C8BitGrayBitmap*>(pBitmap);
	C16BitGrayBitmap *				p16Gray = dynamic_cast<C16BitGrayBitmap *>(pBitmap);
	C32BitGrayBitmap *				p32Gray = dynamic_cast<C32BitGrayBitmap *>(pBitmap);
	C32BitFloatGrayBitmap *			p32FloatGray = dynamic_cast<C32BitFloatGrayBitmap *>(pBitmap);

	if (p24Color)
		Result = FF_8BITRGB;
	else if (p48Color)
		Result = FF_16BITRGB;
	else if (p96Color)
		Result = FF_32BITRGB;
	else if (p96FloatColor)
		Result = FF_32BITRGBFLOAT;
	else if (p8Gray)
		Result = FF_8BITGRAY;
	else if (p16Gray)
		Result = FF_16BITGRAY;
	else if (p32Gray)
		Result = FF_32BITGRAY;
	else if (p32FloatGray)
		Result = FF_32BITGRAYFLOAT;

	return Result;
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriteFromMemoryBitmap::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;
	LONG			lWidth,
					lHeight;

	lWidth  = m_pMemoryBitmap->Width();
	lHeight = m_pMemoryBitmap->Height();
	m_DateTime		= m_pMemoryBitmap->m_DateTime;
	m_lBitsPerPixel = m_pMemoryBitmap->BitPerSample();
	m_lNrChannels   = m_pMemoryBitmap->IsMonochrome() ? 1 : 3;
	m_bFloat		= m_pMemoryBitmap->IsFloat();
	m_CFAType = CFATYPE_NONE;
	if (::IsCFA(m_pMemoryBitmap))
		m_CFAType = ::GetCFAType(m_pMemoryBitmap);

	if (m_Format == TF_UNKNOWN)
		m_Format = GetBestFITSFormat(m_pMemoryBitmap);

	if (m_Format != TF_UNKNOWN)
	{
		SetFormat(lWidth, lHeight, m_Format, m_CFAType);
		if (!m_lISOSpeed)
			m_lISOSpeed = m_pMemoryBitmap->GetISOSpeed();
		if (m_lGain < 0)
			m_lGain = m_pMemoryBitmap->GetGain();
		if (!m_fExposureTime)
			m_fExposureTime = m_pMemoryBitmap->GetExposure();
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriteFromMemoryBitmap::OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue)
{
	try
	{
		if (m_pMemoryBitmap)
		{
			if (m_lNrChannels == 1)
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
		CString name(CharToCString(e.name()));
		CString fileName(CharToCString(e.locationAtIndex(0)->fileName()));
		CString functionName(CharToCString(e.locationAtIndex(0)->functionName()));
		CString text(CharToCString(e.text(0)));

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


	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriteFromMemoryBitmap::OnClose()
{
	BOOL			bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;

	if (pBitmap)
	{
		CFITSWriteFromMemoryBitmap	fits(szFileName, pBitmap, pProgress);

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
};


/* ------------------------------------------------------------------- */

BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription)
{
	return WriteFITS(szFileName, pBitmap, pProgress, FITSFormat, szDescription,
			/*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0);
};

/* ------------------------------------------------------------------- */

BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;

	if (pBitmap)
	{
		CFITSWriteFromMemoryBitmap	fits(szFileName, pBitmap, pProgress);

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

BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription)
{
	return WriteFITS(szFileName, pBitmap, pProgress, szDescription,
			/*lISOSpeed*/ 0, /*lGain*/ -1, /*fExposure*/ 0.0);
};

/* ------------------------------------------------------------------- */

BOOL	IsFITSPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	return GetFITSInfo(szFileName, BitmapInfo);
};

/* ------------------------------------------------------------------- */

int	LoadFITSPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	int		result = -1;		// -1 means not a FITS file.

	if (GetFITSInfo(szFileName, BitmapInfo) && BitmapInfo.CanLoad())
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;

		if (ReadFITS(szFileName, &pBitmap, pProgress))
		{
/*			if (BitmapInfo.IsCFA() && (IsSuperPixels() || IsRawBayer() || IsRawBilinear()))
			{
				C16BitGrayBitmap *	pGrayBitmap;

				pGrayBitmap = dynamic_cast<C16BitGrayBitmap *>(pBitmap.m_p);
				if (IsSuperPixels())
					pGrayBitmap->UseSuperPixels(TRUE);
				else if (IsRawBayer())
					pGrayBitmap->UseRawBayer(TRUE);
				else if (IsRawBilinear())
					pGrayBitmap->UseBilinear(TRUE);
			};*/
			pBitmap.CopyTo(ppBitmap);
			result = 0;
		}
		else
		{
			result = 1;		// Failed to read file
		}
	};

	return result;
};

/* ------------------------------------------------------------------- */

void	GetFITSExtension(LPCTSTR szFileName, CString & strExtension)
{
	TCHAR			szExt[1+_MAX_EXT];
	CString			strExt;

	_tsplitpath(szFileName, nullptr, nullptr, nullptr, szExt);

	strExt = szExt;
	if (!strExt.CompareNoCase(_T(".FITS")))
		strExtension = strExt;
	else if (!strExt.CompareNoCase(_T(".FIT")))
		strExtension = strExt;
	else
		strExtension = ".fts";
};

/* ------------------------------------------------------------------- */
