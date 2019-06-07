#include <stdafx.h>
#include "FITSUtil.h"
#include <float.h>
#include "Registry.h"
#include "Workspace.h"

/* ------------------------------------------------------------------- */

static	CComAutoCriticalSection			g_FITSCritical;

CFITSHeader::CFITSHeader() 
{
	m_bFloat = FALSE;
	m_fExposureTime = 0;
	m_lISOSpeed     = 0;
	m_CFAType		= CFATYPE_NONE;
	m_bByteSwap		= FALSE;
	m_bSigned		= FALSE;
	m_DateTime.wYear= 0;
	g_FITSCritical.Lock();
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

BOOL	IsFITSisRaw()
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
		fits_read_key(m_fits, TDOUBLE, szKey, &fValue, NULL, &nStatus);		
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
		fits_read_key(m_fits, TLONG, szKey, &lValue, NULL, &nStatus);		
		if (!nStatus)
			bResult = TRUE;
	};

	return bResult;
};

BOOL CFITSReader::ReadKey(LPSTR szKey, CString & strValue)
{
	BOOL				bResult = FALSE;
	TCHAR				szValue[2000];
	int					nStatus = 0;

	if (m_fits)
	{
		fits_read_key(m_fits, TSTRING, szKey, szValue, NULL, &nStatus);
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
		

		fits_get_hdrspace(m_fits, &nKeywords, NULL, &nStatus);
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
				strKeyName.Format(_T("[%s]"), (LPCTSTR)CA2CT(szKeyName));

				if (strPropagated.Find(strKeyName) != -1)
					bPropagate = true;
				m_ExtraInfo.AddInfo(
					(LPCTSTR)CA2CT(szKeyName),
					(LPCTSTR)CA2CT(szValue),
					(LPCTSTR)CA2CT(szComment), bPropagate);
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

	Close();
	fits_open_diskfile(&m_fits, (LPCSTR)CT2CA(m_strFileName, CP_UTF8), READONLY, &nStatus);
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
		LONG			lISOSpeed = 0;
		LONG			cfaType;
		m_bDSI = FALSE;

		bResult = ReadKey("SIMPLE", strSimple);
		bResult = ReadKey("NAXIS", lNrAxis);
		if ((strSimple == _T("T")) && (lNrAxis >= 2 && lNrAxis <= 3))
		{
			CString				strComment;
			ReadAllKeys();

			bResult = ReadKey("INSTRUME", strMake);
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

			CString			strMosaic;

			if (ReadKey("DSSCFATYPE", cfaType))
				m_CFAType = (CFATYPE)cfaType;
			else if (ReadKey("MOSAIC", strMosaic) && (strMake.Left(3) == _T("DSI")))
			{
				m_bDSI = TRUE;
				// Special case of DSI FITS files
				strMosaic.Trim();
				//if (strMosaic == "CMYG")
				//	m_CFAType = CFATYPE_CYGMCYMG;
			};

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
				m_fits = NULL;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReader::Read()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;

	if (m_fits)
	{
		LONG			lScanLineSize;
		VOID *			pScanLine = NULL;
		VOID *			pScanLineRed = NULL;
		VOID *			pScanLineGreen = NULL;
		VOID *			pScanLineBlue = NULL;

		BYTE	cNULL = 0;
		WORD	wNULL = 0;
		DWORD	dwNULL = 0;
		float	fNULL = 0;
		void *	pNULL;

		if (m_lBitsPerPixel == 8)
			pNULL = &cNULL;
		else if (m_lBitsPerPixel == 16)
			pNULL = &wNULL;
		else if ((m_lBitsPerPixel == 32) && !m_bFloat)
			pNULL = &dwNULL;
		else
			pNULL = &fNULL;

		if (m_pProgress)
			m_pProgress->Start2(NULL, m_lHeight);

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
					fits_read_pix(m_fits, TFLOAT, pfPixel, m_lWidth * m_lNrChannels, &fNULL, pBuffer, NULL, &nStatus);

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
										fMin = min(fMin, fValue);
										fMax = max(fMax, fValue);
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
					fMin = min(0, fMin);
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
					fits_read_pix(m_fits, datatype, pfPixel, m_lWidth, pNULL, pScanLine, NULL, &nStatus);

					for (i = 0;i<m_lWidth && bResult && !nStatus;i++)
					{
						double		fRed, fGreen, fBlue;

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
						m_pProgress->Progress2(NULL, j+1);
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
					fits_read_pix(m_fits, datatype, pfPixel, m_lWidth, pNULL, pScanLineRed, NULL, &nStatus);
					pfPixel[2] = 2;
					fits_read_pix(m_fits, datatype, pfPixel, m_lWidth, pNULL, pScanLineGreen, NULL, &nStatus);
					pfPixel[2] = 3;
					fits_read_pix(m_fits, datatype, pfPixel, m_lWidth, pNULL, pScanLineBlue, NULL, &nStatus);

					for (i = 0;i<m_lWidth && bResult && !nStatus;i++)
					{
						double		fRed, fGreen, fBlue;
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
						m_pProgress->Progress2(NULL, j+1);
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
	};

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
			m_fits = NULL;
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

	virtual ~CFITSReadInMemoryBitmap() {};

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
		else if (m_lBitsPerPixel == 32)
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
		else if (m_lBitsPerPixel == 32)
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

		if (IsFITSisRaw() && 
			(m_lNrChannels == 1) && 
			((m_lBitsPerPixel == 16) || (m_lBitsPerPixel == 32)))
			m_CFAType = GetFITSCFATYPE();

		if (m_CFAType != CFATYPE_NONE)
		{
			CCFABitmapInfo *		pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(m_pBitmap.m_p);
			//C16BitGrayBitmap *		pGray16Bitmap = dynamic_cast<C16BitGrayBitmap *>(m_pBitmap.m_p);

			if (pCFABitmapInfo)
			{
				m_pBitmap->SetCFA(TRUE);
				pCFABitmapInfo->SetCFAType(m_CFAType);
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
	BOOL			bResult = FALSE;

	if (m_pBitmap)
	{
		if (m_lNrChannels == 1)
		{
			if (m_CFAType != CFAT_NONE)
			{
				switch (::GetBayerColor(lX, lY, m_CFAType))
				{
				case BAYER_BLUE :
					fRed *= m_fBlueRatio;
					break;
				case BAYER_GREEN :
					fRed *= m_fGreenRatio;
					break;
				case BAYER_RED :
					fRed *= m_fRedRatio;
					break;
				};
			}
			else
			{
				fRed	*= m_fBrightnessRatio;
				fGreen	*= m_fBrightnessRatio;
				fBlue	*= m_fBrightnessRatio;
			};
			bResult = m_pBitmap->SetPixel(lX, lY, fRed);
		}
		else
			bResult = m_pBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReadInMemoryBitmap::OnClose()
{
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
	BOOL					bResult = FALSE;
	CFITSReadInMemoryBitmap	fits(szFileName, ppBitmap, pProgress);

	if (ppBitmap)
	{
		bResult = fits.Open();
		if (bResult)
			bResult = fits.Read();
		if (bResult)
			bResult = fits.Close();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	GetFITSInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	BOOL					bResult = FALSE;
	BOOL					bContinue = TRUE;
	CFITSReader				fits(szFileName, NULL);

	// Exclude JPEG, PNG or TIFF format
	{
		TCHAR				szExt[1+_MAX_EXT];
		CString				strExt;

		_tsplitpath(szFileName, NULL, NULL, NULL, szExt);
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
		if (fits.m_strMake.GetLength())
			BitmapInfo.m_strFileType.Format(_T("FITS (%s)"), (LPCTSTR)fits.m_strMake);
		else
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
		BitmapInfo.m_bCanLoad		= TRUE;
		BitmapInfo.m_fExposure		= fits.GetExposureTime();
		BitmapInfo.m_bFITS16bit	    = (fits.NrChannels() == 1) &&
									  ((fits.BitPerChannels() == 16) || (fits.BitPerChannels() == 32));
		BitmapInfo.m_DateTime		= fits.GetDateTime();
		BitmapInfo.m_ExtraInfo		= fits.m_ExtraInfo;
		bResult = fits.Close();
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
			TCHAR			szValue[FLEN_VALUE];

			// check that the keyword is not already used
			fits_read_key(m_fits, TSTRING, (LPCSTR)CT2A(ei.m_strName, CP_UTF8), szValue, NULL, &nStatus);
			if (nStatus)
			{
				nStatus = 0;
				CHAR		szCard[FLEN_CARD];
				int			nType;
				CString		strTemplate;

				if (ei.m_strComment.GetLength())
					strTemplate.Format(_T("%s = %s / %s"), ei.m_strName, ei.m_strValue, ei.m_strComment);
				else
					strTemplate.Format(_T("%s = %s"), ei.m_strName, ei.m_strValue, ei.m_strComment);

				fits_parse_template((LPSTR)CT2A(strTemplate, CP_UTF8), szCard, &nType, &nStatus); 
				fits_write_record(m_fits, szCard, &nStatus);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CFITSWriter::SetFormat(LONG lWidth, LONG lHeight, FITSFORMAT FITSFormat, CFATYPE CFAType)
{
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
	BOOL			bResult = FALSE;
	CString			strFileName = m_strFileName;

	Close();

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
			m_fits = NULL;
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
		VOID *			pScanLine = NULL;
		VOID *			pScanLineRed = NULL;
		VOID *			pScanLineGreen = NULL;
		VOID *			pScanLineBlue = NULL;

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
			int			datatype;
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
				m_pProgress->Start2(NULL, m_lHeight);

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
					m_pProgress->Progress2(NULL, j+1);
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
			m_fits = NULL;
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

	virtual ~CFITSWriteFromMemoryBitmap()
	{
	};

	virtual BOOL	OnOpen();
	virtual BOOL	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue);
	virtual BOOL	OnClose();
};

/* ------------------------------------------------------------------- */

FITSFORMAT	CFITSWriteFromMemoryBitmap::GetBestFITSFormat(CMemoryBitmap * pBitmap)
{
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
		if (!m_fExposureTime)
			m_fExposureTime = m_pMemoryBitmap->GetExposure();
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriteFromMemoryBitmap::OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue)
{
	BOOL			bResult = FALSE;

	if (m_pMemoryBitmap)
	{
		if (m_lNrChannels == 1)
		{
			bResult = m_pMemoryBitmap->GetPixel(lX, lY, fRed);
			fGreen = fBlue = fRed;
		}
		else
			bResult = m_pMemoryBitmap->GetPixel(lX, lY, fRed, fGreen, fBlue);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriteFromMemoryBitmap::OnClose()
{
	BOOL			bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription, LONG lISOSpeed, double fExposure)
{
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
		if (fExposure)
			fits.m_fExposureTime = fExposure;
		fits.SetFormat(FITSFormat);
		if (fits.Open())
		{
			bResult = fits.Write();
			fits.Close();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription, LONG lISOSpeed, double fExposure)
{
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
		if (fExposure)
			fits.m_fExposureTime = fExposure;
		if (fits.Open())
		{
			bResult = fits.Write();
			fits.Close();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	IsFITSPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	return GetFITSInfo(szFileName, BitmapInfo);
};

/* ------------------------------------------------------------------- */

BOOL	LoadFITSPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	BOOL				bResult = FALSE;
	CBitmapInfo			BitmapInfo;

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
			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	GetFITSExtension(LPCTSTR szFileName, CString & strExtension)
{
	TCHAR			szExt[1+_MAX_EXT];
	CString			strExt;

	_tsplitpath(szFileName, NULL, NULL, NULL, szExt);

	strExt = szExt;
	if (!strExt.CompareNoCase(_T(".FITS")))
		strExtension = strExt;
	else if (!strExt.CompareNoCase(_T(".FIT")))
		strExtension = strExt;
	else
		strExtension = ".fts";
};

/* ------------------------------------------------------------------- */
