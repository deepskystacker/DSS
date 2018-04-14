#include <stdafx.h>
#include "TIFFUtil.h"
#include "Registry.h"

#define NRCUSTOMTIFFTAGS		10

static const TIFFFieldInfo DSStiffFieldInfo[NRCUSTOMTIFFTAGS] = 
{
    { TIFFTAG_DSS_NRFRAMES,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSNumberOfFrames" },
    { TIFFTAG_DSS_TOTALEXPOSUREOLD, 1, 1, TIFF_LONG, FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSTotalExposureOld" },
    { TIFFTAG_DSS_TOTALEXPOSURE, 1, 1, TIFF_FLOAT, FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSTotalExposure" },
    { TIFFTAG_DSS_ISO,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSISO" },
    { TIFFTAG_DSS_SETTINGSAPPLIED,	1, 1, TIFF_LONG,	FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSSettingsApplied" },
    { TIFFTAG_DSS_BEZIERSETTINGS,	-1,-1, TIFF_ASCII, FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSBezierSettings" },
    { TIFFTAG_DSS_ADJUSTSETTINGS,	-1,-1, TIFF_ASCII, FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSAdjustSettings" },
    { TIFFTAG_DSS_CFA,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSCFA" },
    { TIFFTAG_DSS_MASTER,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSMaster" },
    { TIFFTAG_DSS_CFATYPE,	1, 1, TIFF_LONG, FIELD_CUSTOM,
      FALSE,	FALSE,	"DSSCFATYPE" }
};

static TIFFExtendProc	g_TIFFParentExtender = NULL;
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

BOOL CTIFFReader::Open()
{
	BOOL			bResult = FALSE;
	CRegistry		reg;
	DWORD			dwSkipExifInfo = 1;

	reg.LoadKey(REGENTRY_BASEKEY, _T("SkipTIFFExifInfo"), dwSkipExifInfo);

	Close();
	m_tiff = TIFFOpen((LPCSTR)CT2CA(m_strFileName), "r");
	if (m_tiff)
	{
		cfa = 0;
		master = 0;
		samplemin = 0;
		samplemax = 1.0;
		exposureTime = 0.0;
		isospeed = 0;
		cfatype = CFATYPE_NONE;

		TIFFGetField(m_tiff,TIFFTAG_IMAGEWIDTH, &w);
        TIFFGetField(m_tiff,TIFFTAG_IMAGELENGTH, &h);

		if (!TIFFGetField(m_tiff, TIFFTAG_COMPRESSION, &compression))
			compression = COMPRESSION_NONE;
		if (!TIFFGetField(m_tiff, TIFFTAG_BITSPERSAMPLE, &bpp))
			bpp = 1;
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

		char *				szMake = NULL;
		char *				szModel = NULL;

		if (TIFFGetField(m_tiff, TIFFTAG_MODEL, &szModel))
			strMakeModel = szModel;
		else if (TIFFGetField(m_tiff, TIFFTAG_MAKE, &szMake))
			strMakeModel = szMake;

		if (!TIFFGetField(m_tiff, TIFFTAG_DSS_ISO, &isospeed))
			isospeed = 0;
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
			if ((bpp == 8) || (bpp == 16))
			{
				bResult = (sampleformat == SAMPLEFORMAT_UINT) ||
						  (sampleformat == SAMPLEFORMAT_INT);
			}
			else if (bpp == 32)
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

		if (!dwSkipExifInfo)
		{
			// Try to read EXIF data
			uint32				ExifID;

			if (TIFFGetField(m_tiff, TIFFTAG_EXIFIFD, &ExifID))
			{
				if (TIFFReadEXIFDirectory(m_tiff, ExifID))
				{
					if (!TIFFGetField(m_tiff, EXIFTAG_EXPOSURETIME, &exposureTime)) 
						exposureTime = 0.0;
					if (!TIFFGetField(m_tiff, EXIFTAG_ISOSPEEDRATINGS, &isospeed))
						isospeed = 0;
				};
			};
		}
		else
		{
			CBitmapInfo			BitmapInfo;

			if (RetrieveEXIFInfo(m_strFileName, BitmapInfo))
			{
				exposureTime = BitmapInfo.m_fExposure;
				isospeed	 = BitmapInfo.m_lISOSpeed;
				m_DateTime	 = BitmapInfo.m_DateTime;
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
				m_DateTime.wYear  = _ttol(strDateTime.Left(4));
				m_DateTime.wMonth = _ttol(strDateTime.Mid(5, 2));
				m_DateTime.wDay   = _ttol(strDateTime.Mid(8, 2));
				m_DateTime.wHour  = _ttol(strDateTime.Mid(11, 2));
				m_DateTime.wMinute= _ttol(strDateTime.Mid(14, 2));
				m_DateTime.wSecond= _ttol(strDateTime.Mid(17, 2));
			};
		};

		if (bResult)
			bResult = OnOpen();
		if (!bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = NULL;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFReader::Read()
{
	BOOL			bResult = FALSE;

	if (m_tiff)
	{
		LONG			lScanLineSize;
		VOID *			pScanLine;

		if (m_pProgress)
			m_pProgress->Start2(NULL, h);

		lScanLineSize = TIFFScanlineSize(m_tiff);
		pScanLine = (VOID *)malloc(lScanLineSize);
		if (pScanLine)
		{
			bResult = TRUE;
			for (LONG j = 0;j<h && bResult;j++)
			{
				BYTE *  pBYTELine	= (BYTE *)pScanLine;
				WORD *	pWORDLine	= (WORD *)pScanLine;
				DWORD * pDWORDLine	= (DWORD *)pScanLine;
				float *	pFLOATLine	= (float *)pScanLine;

				int			nResult;

				nResult = TIFFReadScanline(m_tiff, pScanLine, j);
				for (LONG i = 0;i<w && bResult;i++)
				{
					double		fRed, fGreen, fBlue;

					if (spp == 1)
					{
						if (bpp == 8)
						{
							fRed	= *(pBYTELine);
							fGreen	= *(pBYTELine);
							fBlue	= *(pBYTELine);
							pBYTELine ++;
						}
						else if (bpp == 16)
						{
							fRed	= (double)(*(pWORDLine))/256.0;
							fGreen	= (double)(*(pWORDLine))/256.0;
							fBlue	= (double)(*(pWORDLine))/256.0;
							pWORDLine ++;
						}
						else if (bpp == 32)
						{
							if (sampleformat == SAMPLEFORMAT_IEEEFP)
							{
								fRed	= (double)(*(pFLOATLine));
								fGreen	= (double)(*(pFLOATLine));
								fBlue	= (double)(*(pFLOATLine));

								fRed   = (fRed - samplemin)/(samplemax-samplemin) * 256.0;
								fGreen = (fGreen - samplemin)/(samplemax-samplemin) * 256.0;
								fBlue  = (fBlue - samplemin)/(samplemax-samplemin) * 256.0;

								pFLOATLine ++;
							}
							else
							{
								fRed	= (double)(*(pDWORDLine))/256.0/65536.0;
								fGreen	= (double)(*(pDWORDLine))/256.0/65536.0;
								fBlue	= (double)(*(pDWORDLine))/256.0/65536.0;
								pDWORDLine ++;
							};
						};
					}
					else
					{
						if (bpp == 8)
						{
							fRed	= *(pBYTELine);
							fGreen	= *(pBYTELine+1);
							fBlue	= *(pBYTELine+2);
							pBYTELine += 3;
							if (spp == 4)
								pBYTELine++;
						}
						else if (bpp == 16)
						{
							fRed	= (double)(*(pWORDLine))/256.0;
							fGreen	= (double)(*(pWORDLine+1))/256.0;
							fBlue	= (double)(*(pWORDLine+2))/256.0;
							pWORDLine += 3;
							if (spp == 4)
								pWORDLine++;
						}
						else if (bpp == 32)
						{
							if (sampleformat == SAMPLEFORMAT_IEEEFP)
							{
								fRed	= (double)(*(pFLOATLine));
								fGreen	= (double)(*(pFLOATLine+1));
								fBlue	= (double)(*(pFLOATLine+2));

								fRed   = (fRed - samplemin)/(samplemax-samplemin) * 256.0;
								fGreen = (fGreen - samplemin)/(samplemax-samplemin) * 256.0;
								fBlue  = (fBlue - samplemin)/(samplemax-samplemin) * 256.0;

								pFLOATLine += 3;
								if (spp == 4)
									pFLOATLine++;
							}
							else
							{
								fRed	= (double)(*(pDWORDLine))/256.0/65536.0;
								fGreen	= (double)(*(pDWORDLine+1))/256.0/65536.0;
								fBlue	= (double)(*(pDWORDLine+2))/256.0/65536.0;
								pDWORDLine += 3;
								if (spp == 4)
									pDWORDLine++;
							};
						};
					};

					bResult = OnRead(i, j, fRed, fGreen, fBlue);

					if (m_pProgress)
						m_pProgress->Progress2(NULL, j+1);
				};
			};
			free(pScanLine);
			if (m_pProgress)
				m_pProgress->End2();
		}
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFReader::Close()
{
	BOOL			bResult = TRUE;
	if (m_tiff)
	{
		bResult = OnClose();
		if (bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = NULL;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CTIFFWriter::SetFormat(LONG lWidth, LONG lHeight, TIFFFORMAT TiffFormat, CFATYPE CFAType, BOOL bMaster)
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
		bpp = 8;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_16BITRGB :
		spp = 3;
		bpp = 16;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_32BITRGB :
		spp = 3;
		bpp = 32;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_32BITRGBFLOAT :
		spp = 3;
		bpp = 32;
		sampleformat = SAMPLEFORMAT_IEEEFP;
		samplemin    = 0;
		samplemax    = 1.0;
		break;
	case TF_8BITGRAY :
		spp = 1;
		bpp = 8;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_16BITGRAY :
		spp = 1;
		bpp = 16;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_32BITGRAY :
		spp = 1;
		bpp = 32;
		sampleformat = SAMPLEFORMAT_UINT;
		break;
	case TF_32BITGRAYFLOAT :
		spp = 1;
		bpp = 32;
		sampleformat = SAMPLEFORMAT_IEEEFP;
		samplemin    = 0;
		samplemax    = 1.0;
		break;
	};
};

/* ------------------------------------------------------------------- */

BOOL CTIFFWriter::Open()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;

	Close();
	m_tiff = TIFFOpen((LPCSTR)CT2CA(m_strFileName), "w");
	if (m_tiff)
	{
		photo = PHOTOMETRIC_RGB;

		bResult = OnOpen();
		if (bResult)
		{
			TIFFSetField(m_tiff, TIFFTAG_IMAGEWIDTH, w);
			TIFFSetField(m_tiff, TIFFTAG_IMAGELENGTH, h);
			TIFFSetField(m_tiff, TIFFTAG_COMPRESSION, compression);

			TIFFSetField(m_tiff, TIFFTAG_BITSPERSAMPLE, bpp);
			TIFFSetField(m_tiff, TIFFTAG_SAMPLESPERPIXEL, spp);
			TIFFSetField(m_tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			if (spp == 1)
				TIFFSetField(m_tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
			else
				TIFFSetField(m_tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			TIFFSetField(m_tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
			TIFFSetField(m_tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);

			if (samplemax-samplemin)
			{
				TIFFSetField(m_tiff, TIFFTAG_SMINSAMPLEVALUE, samplemin);
				TIFFSetField(m_tiff, TIFFTAG_SMAXSAMPLEVALUE, samplemax);
			};

			CStringA			strSoftware;

			strSoftware.LoadString(IDS_DEEPSKYSTACKER);
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
			if (exposureTime)
				TIFFSetField(m_tiff, TIFFTAG_DSS_TOTALEXPOSURE, exposureTime);
			if (nrframes)
				TIFFSetField(m_tiff, TIFFTAG_DSS_NRFRAMES, nrframes);
		}
		else
		{
			TIFFClose(m_tiff);
			m_tiff = NULL;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFWriter::Write()
{
	ZFUNCTRACE_RUNTIME();
	BOOL		bResult = FALSE;
	bool		bError = false;


	if (m_tiff)
	{
		tmsize_t		szScanLineSize;
		VOID *			pScanLine;

		szScanLineSize = TIFFScanlineSize(m_tiff);
		pScanLine = (VOID *)malloc(szScanLineSize);
		if (pScanLine)
		{
			if (m_pProgress)
				m_pProgress->Start2(NULL, h);

			for (LONG j = 0;(j<h) && !bError; j++)
			{
				BYTE *  pBYTELine	= (BYTE *)pScanLine;
				WORD *	pWORDLine	= (WORD *)pScanLine;
				DWORD * pDWORDLine	= (DWORD *)pScanLine;
				float *	pFLOATLine	= (float *)pScanLine;

				for (LONG i = 0;i<w;i++)
				{
					double		fRed, fGreen, fBlue;

					OnWrite(i, j, fRed, fGreen, fBlue);

					if (spp == 1)
					{
						double			fGray;

						if (cfa)
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

						if (bpp == 8)
						{
							*(pBYTELine) = fGray;
							pBYTELine ++;
						}
						else if (bpp == 16)
						{
							(*(pWORDLine)) = fGray * 256.0;
							pWORDLine ++;
						}
						else if (bpp == 32)
						{
							if (sampleformat == SAMPLEFORMAT_IEEEFP)
							{
								(*(pFLOATLine)) = fGray / 256.0 * (samplemax - samplemin) + samplemin;
								pFLOATLine ++;
							}
							else
							{
								(*(pDWORDLine)) = fGray * 256.0 *65536.0;
								pDWORDLine ++;
							};
						};
					}
					else
					{
						if (bpp == 8)
						{
							*(pBYTELine) = fRed;
							*(pBYTELine+1) = fGreen;
							*(pBYTELine+2) = fBlue;
							pBYTELine += 3;
						}
						else if (bpp == 16)
						{
							(*(pWORDLine))   = fRed *256.0;
							(*(pWORDLine+1)) = fGreen *256.0;
							(*(pWORDLine+2)) = fBlue *256.0;
							pWORDLine += 3;
						}
						else if (bpp == 32)
						{
							if (sampleformat == SAMPLEFORMAT_IEEEFP)
							{
								(*(pFLOATLine))	  = fRed / 256.0  * (samplemax - samplemin) + samplemin;
								(*(pFLOATLine+1)) = fGreen / 256.0  * (samplemax - samplemin) + samplemin;
								(*(pFLOATLine+2)) = fBlue / 256.0  * (samplemax - samplemin) + samplemin;
								pFLOATLine += 3;
							}
							else
							{
								(*(pDWORDLine))   = fRed * 256.0 * 65536.0;
								(*(pDWORDLine+1)) = fGreen * 256.0 * 65536.0;
								(*(pDWORDLine+2)) = fBlue * 256.0 *65536.0;
								pDWORDLine += 3;
							};
						};
					};

				};
				int			nResult;
				
				nResult = TIFFWriteScanline(m_tiff, pScanLine, j, 0);
				if (m_pProgress)
					m_pProgress->Progress2(NULL, j+1);
				if (-1 == nResult)
				{
					ZTRACE_RUNTIME("TIFFWriteScanLine failed");
					bError = true;
					break;
				}
			};
			free(pScanLine);
			if (m_pProgress)
				m_pProgress->End2();
		};
		bResult = (!bError) ? TRUE : FALSE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFWriter::Close()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;

	if (m_tiff)
	{
		bResult = OnClose();
		if (bResult)
		{
			TIFFClose(m_tiff);
			m_tiff = NULL;
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
	};

	virtual BOOL	OnOpen();
	virtual BOOL	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue);
	virtual BOOL	OnClose();
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

BOOL CTIFFWriteFromMemoryBitmap::OnOpen()
{
	BOOL			bResult = TRUE;
	LONG			lWidth,
					lHeight;
	CFATYPE			CFAType = CFATYPE_NONE;
	BOOL			bMaster;

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
		if (!exposureTime)
			exposureTime = m_pMemoryBitmap->GetExposure();
		if (!nrframes)
			nrframes = m_pMemoryBitmap->GetNrFrames();
		m_DateTime = m_pMemoryBitmap->m_DateTime;
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFWriteFromMemoryBitmap::OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue)
{
	BOOL			bResult = FALSE;

	if (m_pMemoryBitmap)
	{
		if (spp == 1)
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

BOOL CTIFFWriteFromMemoryBitmap::OnClose()
{
	BOOL			bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription, LONG lISOSpeed, double fExposure)
{
	BOOL				bResult = FALSE;

	if (pBitmap)
	{
		CTIFFWriteFromMemoryBitmap	tiff(szFileName, pBitmap, pProgress);

		if (szDescription)
			tiff.SetDescription(szDescription);
		if (lISOSpeed)
			tiff.SetISOSpeed(lISOSpeed);
		else
			tiff.SetISOSpeed(pBitmap->GetISOSpeed());
		if (fExposure)
			tiff.SetExposureTime(fExposure);
		else
			tiff.SetExposureTime(pBitmap->GetExposure());
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

BOOL	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, LPCTSTR szDescription, LONG lISOSpeed, double fExposure)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (pBitmap)
	{
		CTIFFWriteFromMemoryBitmap	tiff(szFileName, pBitmap, pProgress);

		if (szDescription)
			tiff.SetDescription(szDescription);
		if (lISOSpeed)
			tiff.SetISOSpeed(lISOSpeed);
		if (fExposure)
			tiff.SetExposureTime(fExposure);
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

	virtual ~CTIFFReadInMemoryBitmap() {};

	virtual BOOL	OnOpen();
	virtual BOOL	OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue);
	virtual BOOL	OnClose();
};

/* ------------------------------------------------------------------- */

BOOL CTIFFReadInMemoryBitmap::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;

	if (spp == 1)
	{
		if (bpp == 8)
		{
			m_pBitmap.Attach(new C8BitGrayBitmap());
			ZTRACE_RUNTIME("Creating 8 Gray bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (bpp == 16)
		{
			m_pBitmap.Attach(new C16BitGrayBitmap());
			ZTRACE_RUNTIME("Creating 16 Gray bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (bpp == 32)
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
		if (bpp == 8)
		{
			m_pBitmap.Attach(new C24BitColorBitmap());
			ZTRACE_RUNTIME("Creating 8 RGB bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (bpp == 16)
		{
			m_pBitmap.Attach(new C48BitColorBitmap());
			ZTRACE_RUNTIME("Creating 16 RGB bit memory bitmap %p", m_pBitmap.m_p);
		}
		else if (bpp == 32)
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
		m_pBitmap->SetExposure(exposureTime);
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

BOOL CTIFFReadInMemoryBitmap::OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue)
{
	BOOL			bResult = FALSE;

	if (m_pBitmap)
		bResult = m_pBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFReadInMemoryBitmap::OnClose()
{
	BOOL			bResult = FALSE;

	if (m_pBitmap)
	{
		bResult = TRUE;
		m_pBitmap.CopyTo(m_ppBitmap);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	ReadTIFF(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress *	pProgress)
{
	BOOL					bResult = FALSE;
	CTIFFReadInMemoryBitmap	tiff(szFileName, ppBitmap, pProgress);

	if (ppBitmap)
	{
		bResult = tiff.Open();
		if (bResult)
			bResult = tiff.Read();
		if (bResult)
			bResult = tiff.Close();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	GetTIFFInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	BOOL					bResult = FALSE;
	CTIFFReader				tiff(szFileName, NULL);

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
		BitmapInfo.m_bCanLoad		= TRUE;
		BitmapInfo.m_lISOSpeed		= tiff.GetISOSpeed();
		BitmapInfo.m_fExposure		= tiff.GetExposureTime();
		BitmapInfo.m_DateTime		= tiff.GetDateTime();
		bResult = tiff.Close();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	IsTIFFPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	return GetTIFFInfo(szFileName, BitmapInfo);
};

/* ------------------------------------------------------------------- */

BOOL	LoadTIFFPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	BOOL				bResult = FALSE;
	CBitmapInfo			BitmapInfo;

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
						pGrayBitmap->UseSuperPixels(TRUE);
					else if (IsRawBayer())
						pGrayBitmap->UseRawBayer(TRUE);
					else  if (IsRawBilinear())
						pGrayBitmap->UseBilinear(TRUE);
					else if (IsRawAHD())
						pGrayBitmap->UseAHD(TRUE);
				};
			};
			pBitmap.CopyTo(ppBitmap);
			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
