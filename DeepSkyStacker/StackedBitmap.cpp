#include <stdafx.h>
#include "DeepStack.h"
#include <math.h>
#include <tiffio.h>
#include <algorithm>
#include <set>
#include "TIFFUtil.h"
#include "FITSUtil.h"

#define _USE_MATH_DEFINES
#include <cmath>

/* ------------------------------------------------------------------- */

CStackedBitmap::CStackedBitmap() 
{
	m_lNrBitmaps	= 0;
	m_lWidth		= 0;
	m_lHeight		= 0;
	m_lOutputWidth	= 0;
	m_lOutputHeight = 0;
	m_lISOSpeed		= 0;
	m_lTotalTime	= 0;
	m_bMonochrome   = FALSE;
	DSSTIFFInitialize();
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::GetPixel(LONG X, LONG Y, double & fRed, double & fGreen, double & fBlue, BOOL bApplySettings)
{
	LONG				lOffset = m_lWidth * Y + X;

	double		H, S, L;

	// Adjust beetween 0 and 65535.0
	fRed   = m_vRedPlane[lOffset]/m_lNrBitmaps*256.0; 
	if (!m_bMonochrome)
	{
		fGreen = m_vGreenPlane[lOffset]/m_lNrBitmaps*256.0;
		fBlue  = m_vBluePlane[lOffset]/m_lNrBitmaps*256.0;
	}
	else
		fGreen = fBlue = fRed;

	if (bApplySettings)
	{
		m_HistoAdjust.Adjust(fRed, fGreen, fBlue);

		fRed	/= 256.0;
		fGreen	/= 256.0;
		fBlue	/= 256.0;

		if (fRed > 255)
			fRed = 255;
		if (fGreen > 255)
			fGreen = 255;
		if (fBlue > 255)
			fBlue = 255;

		ToHSL(fRed, fGreen, fBlue, H, S, L);

		// adjust luminance
		L = m_BezierAdjust.GetValue(L);

		// adjust saturation
		S = m_BezierAdjust.AdjustSaturation(S);

		ToRGB(H, S, L, fRed, fGreen, fBlue);
	}
	else
	{
		fRed	/= 256.0;
		fGreen	/= 256.0;
		fBlue	/= 256.0;
	};
};

/* ------------------------------------------------------------------- */

COLORREF CStackedBitmap::GetPixel(float fRed, float fGreen, float fBlue, BOOL bApplySettings)
{
	COLORREF			crResult;
	double				Red, Green, Blue;

	double		H, S, L;

	// Adjust beetween 0 and 65535.0
	Red   = fRed/m_lNrBitmaps*255.0; 
	Green = fGreen/m_lNrBitmaps*255.0;
	Blue  = fBlue/m_lNrBitmaps*255.0;

	if (bApplySettings)
	{
		m_HistoAdjust.Adjust(Red, Green, Blue);

		Red		/= 255.0;
		Green	/= 255.0;
		Blue	/= 255.0;

		if (Red > 255)
			Red = 255;
		if (Green > 255)
			Green = 255;
		if (Blue > 255)
			Blue = 255;

		ToHSL(Red, Green, Blue, H, S, L);

		// adjust luminance
		L = m_BezierAdjust.GetValue(L);

		// adjust saturation
		S = m_BezierAdjust.AdjustSaturation(S);

		ToRGB(H, S, L, Red, Green, Blue);

		crResult = RGB(Red, Green, Blue);
	}
	else
	{
		crResult = RGB(Red/255.0, Green/255.0, Blue/255.0);
	};

	return crResult;
};

/* ------------------------------------------------------------------- */

COLORREF CStackedBitmap::GetPixel(LONG X, LONG Y, BOOL bApplySettings)
{
	LONG				lOffset = m_lWidth * Y + X;

	if (m_bMonochrome)
		return GetPixel(m_vRedPlane[lOffset], m_vRedPlane[lOffset], m_vRedPlane[lOffset], bApplySettings);
	else
		return GetPixel(m_vRedPlane[lOffset], m_vGreenPlane[lOffset], m_vBluePlane[lOffset], bApplySettings);
};

/* ------------------------------------------------------------------- */

COLORREF16	CStackedBitmap::GetPixel16(LONG X, LONG Y, BOOL bApplySettings)
{
	COLORREF16			crResult;

	LONG				lOffset = m_lWidth * Y + X;
	double				Red, Green, Blue;

	double		H, S, L;

	// Adjust beetween 0 and 65535.0
	Red   = m_vRedPlane[lOffset]/m_lNrBitmaps*256.0; 
	if (!m_bMonochrome)
	{
		Green = m_vGreenPlane[lOffset]/m_lNrBitmaps*256.0;
		Blue  = m_vBluePlane[lOffset]/m_lNrBitmaps*256.0;
	}
	else
		Green = Blue = Red;

	if (bApplySettings)
	{
		m_HistoAdjust.Adjust(Red, Green, Blue);

		Red		/= 256.0;
		Green	/= 256.0;
		Blue	/= 256.0;

		if (Red > 255)
			Red = 255;
		if (Green > 255)
			Green = 255;
		if (Blue > 255)
			Blue = 255;

		ToHSL(Red, Green, Blue, H, S, L);

		// adjust luminance
		L = m_BezierAdjust.GetValue(L);

		// adjust saturation
		S = m_BezierAdjust.AdjustSaturation(S);

		ToRGB(H, S, L, Red, Green, Blue);

		crResult.red = Red * 256.0;
		crResult.green = Green * 256.0;
		crResult.blue = Blue * 256.0;
	}
	else
	{
		crResult.red = Red;
		crResult.green = Green;
		crResult.blue = Blue;
	};

	return crResult;
};

/* ------------------------------------------------------------------- */

COLORREF32	CStackedBitmap::GetPixel32(LONG X, LONG Y, BOOL bApplySettings)
{
	COLORREF32			crResult;

	LONG				lOffset = m_lWidth * Y + X;
	double				Red, Green, Blue;

	double		H, S, L;

	// Adjust beetween 0 and 65535.0
	Red   = m_vRedPlane[lOffset]/m_lNrBitmaps*256.0; 
	if (!m_bMonochrome)
	{
		Green = m_vGreenPlane[lOffset]/m_lNrBitmaps*256.0;
		Blue  = m_vBluePlane[lOffset]/m_lNrBitmaps*256.0;
	}
	else
		Green = Blue = Red;

	if (bApplySettings)
	{
		m_HistoAdjust.Adjust(Red, Green, Blue);

		Red		/= 256.0;
		Green	/= 256.0;
		Blue	/= 256.0;

		if (Red > 255)
			Red = 255;
		if (Green > 255)
			Green = 255;
		if (Blue > 255)
			Blue = 255;

		ToHSL(Red, Green, Blue, H, S, L);

		// adjust luminance
		L = m_BezierAdjust.GetValue(L);

		// adjust saturation
		S = m_BezierAdjust.AdjustSaturation(S);

		ToRGB(H, S, L, Red, Green, Blue);

		crResult.red = Red * 256.0 * 65536.0;
		crResult.green = Green * 256.0 * 65536.0;
		crResult.blue = Blue * 256.0 * 65536.0;
	}
	else
	{
		crResult.red = Red * 65536.0;
		crResult.green = Green * 65536.0;
		crResult.blue = Blue * 65536.0;
	};

	return crResult;
};

/* ------------------------------------------------------------------- */

#pragma pack(push, HDSTACKEDBITMAP, 2)

const DWORD			HDSTACKEDBITMAP_MAGIC = 0x878A56E6L;

typedef struct tagHDSTACKEDBITMAPHEADER
{
	DWORD			dwMagic;		// Magic number (always HDSTACKEDBITMAP_MAGIC)
	DWORD			dwHeaderSize;	// Always sizeof(HDSTACKEDBITMAPHEADER);
	LONG			lWidth;			// Width
	LONG			lHeight;		// Height
	LONG			lNrBitmaps;		// Number of bitmaps
	DWORD			dwFlags;		// Flags
	LONG			lTotalTime;		// Total Time
	WORD			lISOSpeed;		// ISO Speed of each frame
	char			Reserved[26];	// Reserved (set to 0)
}HDSTACKEDBITMAPHEADER;

#pragma pack(pop, HDSTACKEDBITMAP)

/* ------------------------------------------------------------------- */

BOOL CStackedBitmap::LoadDSImage(LPCTSTR szStackedFile, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	FILE *			hFile;
	CString			strText;
	LPCSTR			strFile = CT2CA(szStackedFile, CP_UTF8); // Stacked fileid in ASCII

	strText.LoadString(IDS_LOADDSIMAGE);
	if (pProgress)
		pProgress->Start(strText, 0, FALSE);

	hFile = _tfopen(szStackedFile, _T("rb"));
	if (hFile)
	{
		HDSTACKEDBITMAPHEADER	Header;
		LONG					lProgress = 0;

		if (pProgress)
		{
			strText.Format(IDS_LOADPICTURE, szStackedFile);
			pProgress->Progress1(strText, 0);
		};

		fread(&Header, sizeof(Header), 1, hFile);
		if ((Header.dwMagic == HDSTACKEDBITMAP_MAGIC) && 
			(Header.dwHeaderSize == sizeof(Header)))
		{
			m_lWidth	= Header.lWidth;
			m_lHeight	= Header.lHeight;
			m_lNrBitmaps= Header.lNrBitmaps;
			m_lTotalTime= Header.lTotalTime;
			m_lISOSpeed = Header.lISOSpeed;

			Allocate(Header.lWidth, Header.lHeight, FALSE);

			if (pProgress)
				pProgress->Start(NULL, m_lWidth * m_lHeight, FALSE);

			for (LONG i = 0;i<m_vRedPlane.size();i++)
			{
				lProgress++;
				if (pProgress)
					pProgress->Progress1(NULL, lProgress);

				fread(&m_vRedPlane[i], sizeof(float), 1, hFile);
				fread(&m_vGreenPlane[i], sizeof(float), 1, hFile);
				fread(&m_vBluePlane[i], sizeof(float), 1, hFile);
			};

			bResult = TRUE;
			m_BezierAdjust.Reset();
			m_HistoAdjust.Reset();
		}
		else
		{

			printf("Wrong header in %s\n", strFile);
			ZTRACE_RUNTIME("Wrong header in %s", strFile);
		};
		fclose(hFile);
	}
	else
	{
		printf("Cannot open %s\n", strFile);
		ZTRACE_RUNTIME("Cannot open%s", strFile);
	}


	if (pProgress)
		pProgress->Close();

	return bResult;
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveDSImage(LPCTSTR szStackedFile, LPRECT pRect, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	FILE *			hFile;
	CString			strText;
	LPCSTR			strFile = CT2CA(szStackedFile, CP_UTF8);  // in UTF-8

	printf("Saving Stacked Bitmap in %s\n", strFile);
	ZTRACE_RUNTIME("Saving Stacked Bitmap in %s", strFile);
	hFile = _tfopen(szStackedFile, _T("wb"));
	if (hFile)
	{
		HDSTACKEDBITMAPHEADER	Header;

		LONG		lWidth,
					lHeight;
		LONG		lStartX,
					lStartY;
		LONG		i, j;

		LONG		lProgress = 0;

		if (pRect)
		{
			pRect->left		= max(0, pRect->left);
			pRect->right	= min(m_lWidth, pRect->right);
			pRect->top		= max(0, pRect->top);
			pRect->bottom	= min(m_lHeight, pRect->bottom);

			lWidth			= (pRect->right-pRect->left);
			lHeight			= (pRect->bottom-pRect->top);

			lStartX = pRect->left;
			lStartY = pRect->top;
		}
		else
		{
			lWidth			= m_lWidth;
			lHeight			= m_lHeight;

			lStartX = 0;
			lStartY = 0;
		};

		if (pProgress)
		{
			strText.LoadString(IDS_SAVINGDSIMAGE);
			pProgress->Start(strText, lWidth * lHeight, FALSE);
			strText.Format(IDS_SAVEDSIMAGE, szStackedFile);
			pProgress->Progress1(strText, 1);
		};

		memset(&Header, 0, sizeof(Header));
		Header.dwMagic = HDSTACKEDBITMAP_MAGIC;
		Header.dwHeaderSize = sizeof(HDSTACKEDBITMAPHEADER);
		Header.lWidth		= lWidth;
		Header.lHeight		= lHeight;
		Header.lNrBitmaps	= m_lNrBitmaps;
		Header.lTotalTime	= m_lTotalTime;
		Header.lISOSpeed	= m_lISOSpeed;

		fwrite(&Header, sizeof(Header), 1, hFile);

		for (j = lStartY;j<lStartY + lHeight;j++)
		{
			for (i = lStartX;i<lStartX + lWidth;i++)
			{
				lProgress++;

				fwrite(&m_vRedPlane[m_lWidth * j + i], sizeof(float), 1, hFile);
				if (!m_bMonochrome)
				{
					fwrite(&m_vGreenPlane[m_lWidth * j + i], sizeof(float), 1, hFile);
					fwrite(&m_vBluePlane[m_lWidth * j + i], sizeof(float), 1, hFile);
				}
				else
				{
					fwrite(&m_vRedPlane[m_lWidth * j + i], sizeof(float), 1, hFile);
					fwrite(&m_vRedPlane[m_lWidth * j + i], sizeof(float), 1, hFile);
				};
			};

			if (pProgress)
				pProgress->Progress1(NULL, lProgress);
		};

		fclose(hFile);

		if (pProgress)
			pProgress->Close();
	}
	else
	{
		printf("Error creating file %s!\n", strFile);
		ZTRACE_RUNTIME("Error creating file %s!", strFile);
	};
};

/* ------------------------------------------------------------------- */

#if !defined(PCL_PROJECT) && !defined(_CONSOLE)
class CPixel
{
public :
	double			m_fRed,
					m_fGreen,
					m_fBlue;
private :
	void	CopyFrom(const CPixel & px)
	{
		m_fRed		= px.m_fRed;
		m_fGreen	= px.m_fGreen;
		m_fBlue		= px.m_fBlue;
	};
public :
	CPixel(double fRed = 0, double fGreen = 0, double fBlue = 0) 
	{
		m_fRed		= fRed;
		m_fGreen	= fGreen;
		m_fBlue		= fBlue;
	};
	CPixel(const CPixel & px)
	{
		CopyFrom(px);
	};

	~CPixel() {};

	bool operator < (const CPixel & px) const
	{
		if (m_fRed<px.m_fRed)
			return true;
		else if (m_fRed>px.m_fRed)
			return false;
		else if (m_fGreen<px.m_fGreen)
			return true;
		else if (m_fGreen>px.m_fGreen)
			return false;
		else
			return m_fBlue<px.m_fBlue;
	};
};

typedef std::set<CPixel>			PIXELSET;
typedef PIXELSET::iterator			PIXELITERATOR;

HBITMAP CStackedBitmap::GetBitmap(C32BitsBitmap & Bitmap, RECT * pRect)
{
	if (Bitmap.IsEmpty())
		Bitmap.Create(m_lWidth, m_lHeight);

	if (!Bitmap.IsEmpty())
	{
		LONG		lXMin = 0, 
					lYMin = 0, 
					lXMax = m_lWidth, 
					lYMax = m_lHeight;

		if (pRect)
		{
			lXMin	= max(0, pRect->left);
			lYMin	= max(0, pRect->top);
			lXMax	= min(m_lWidth, pRect->right);
			lYMax	= min(m_lHeight, pRect->bottom);
		};

		/*PIXELSET		sPixels;
		PIXELITERATOR	it;*/

		float *				pBaseRedPixel;
		float *				pBaseGreenPixel = NULL;
		float *				pBaseBluePixel  = NULL;
		float *				pRedPixel;
		float *				pGreenPixel;
		float *				pBluePixel;

		pBaseRedPixel	= &(m_vRedPlane[m_lWidth * lYMin + lXMin]);
		if (!m_bMonochrome)
		{
			pBaseGreenPixel = &(m_vGreenPlane[m_lWidth * lYMin + lXMin]);
			pBaseBluePixel	= &(m_vBluePlane[m_lWidth * lYMin + lXMin]);
		};

		for (LONG j = lYMin;j<lYMax;j++)
		{
			LPBYTE			lpOut = Bitmap.GetPixelBase(lXMin, j);
			LPRGBQUAD &		lpOutPixel = (LPRGBQUAD &)lpOut;

			pRedPixel	= pBaseRedPixel;
			pGreenPixel = pBaseGreenPixel;
			pBluePixel	= pBaseBluePixel;
			for (LONG i = lXMin;i<lXMax;i++)
			{
				COLORREF		crColor;

				if (!m_bMonochrome)
					crColor = GetPixel(*pRedPixel, *pGreenPixel, *pBluePixel, TRUE);
				else
					crColor = GetPixel(*pRedPixel, *pRedPixel, *pRedPixel, TRUE);

				/*CPixel			px(*pRedPixel, *pGreenPixel, *pBluePixel);

				it = sPixels.find(px);
				if (it == sPixels.end())
					sPixels.insert(px);*/

				lpOutPixel->rgbRed		= GetRValue(crColor);
				lpOutPixel->rgbGreen	= GetGValue(crColor);
				lpOutPixel->rgbBlue		= GetBValue(crColor);
				lpOutPixel->rgbReserved	= 0;
				// Bitmap.SetPixel(i, j, crColor);

				pRedPixel++;
				if (!m_bMonochrome)
				{
					pGreenPixel++;
					pBluePixel++;
				};
				lpOut += 4;
			};
			pBaseRedPixel	+= m_lWidth;
			if (!m_bMonochrome)
			{
				pBaseGreenPixel += m_lWidth;
				pBaseBluePixel  += m_lWidth;
			};
		};

		/*LONG				lNrPixels = sPixels.size();
		printf("%ld", lNrPixels);*/
	};

	return Bitmap.GetHBITMAP();
};

#endif // !defined(PCL_PROJECT) && !defined(_CONSOLE)

/* ------------------------------------------------------------------- */

BOOL CStackedBitmap::GetBitmap(CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	*ppBitmap = NULL;

	CSmartPtr<CMemoryBitmap>	pBitmap;

	if (m_bMonochrome)
	{
		CSmartPtr<C32BitFloatGrayBitmap>	pTempBitmap;

		pTempBitmap.Attach(new C32BitFloatGrayBitmap);
		pTempBitmap->Init(m_lWidth, m_lHeight);
		pBitmap = pTempBitmap;
	}
	else
	{
		CSmartPtr<C96BitFloatColorBitmap>	pTempBitmap;

		pTempBitmap.Attach(new C96BitFloatColorBitmap);
		pTempBitmap->Init(m_lWidth, m_lHeight);
		pBitmap = pTempBitmap;
	};

	if (pBitmap)
	{
		LONG		lXMin = 0, 
					lYMin = 0, 
					lXMax = m_lWidth, 
					lYMax = m_lHeight;

		float *				pBaseRedPixel;
		float *				pBaseGreenPixel = NULL;
		float *				pBaseBluePixel  = NULL;
		float *				pRedPixel;
		float *				pGreenPixel;
		float *				pBluePixel;

		if (pProgress)
		{
			CString			strText;

			strText.LoadString(IDS_PROCESSINGIMAGE);
			pProgress->Start2(strText, lYMax-lYMin);
		};

		pBaseRedPixel	= &(m_vRedPlane[m_lWidth * lYMin + lXMin]);
		if (!m_bMonochrome)
		{
			pBaseGreenPixel = &(m_vGreenPlane[m_lWidth * lYMin + lXMin]);
			pBaseBluePixel	= &(m_vBluePlane[m_lWidth * lYMin + lXMin]);
		};

		for (LONG j = lYMin;j<lYMax;j++)
		{
			pRedPixel	= pBaseRedPixel;
			pGreenPixel = pBaseGreenPixel;
			pBluePixel	= pBaseBluePixel;
			for (LONG i = lXMin;i<lXMax;i++)
			{
				COLORREF		crColor;

				if (!m_bMonochrome)
				{
					crColor = GetPixel(*pRedPixel, *pGreenPixel, *pBluePixel, TRUE);
					pBitmap->SetPixel(i, j, GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
				}
				else
				{
					crColor = GetPixel(*pRedPixel, *pRedPixel, *pRedPixel, TRUE);
					pBitmap->SetPixel(i, j, GetRValue(crColor));
				};


				pRedPixel++;
				if (!m_bMonochrome)
				{
					pGreenPixel++;
					pBluePixel++;
				};
			};
			pBaseRedPixel	+= m_lWidth;
			if (!m_bMonochrome)
			{
				pBaseGreenPixel += m_lWidth;
				pBaseBluePixel  += m_lWidth;
			};

			if (pProgress)
				pProgress->Progress2(NULL, j-lYMin+1);
		};

		if (pProgress)
			pProgress->End2();
	};

	return pBitmap.CopyTo(ppBitmap);
};

/* ------------------------------------------------------------------- */

BOOL CStackedBitmap::Load(LPCTSTR szStackedFile, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CBitmapInfo		bmpInfo;

	if (GetPictureInfo(szStackedFile, bmpInfo) && bmpInfo.CanLoad())
	{
		CString		strFileType = bmpInfo.m_strFileType.Left(4);

		if (strFileType == "TIFF")
			return LoadTIFF(szStackedFile, pProgress);
		else if (strFileType == "FITS")
			return LoadFITS(szStackedFile, pProgress);
		else
			return FALSE;
	}
	else
		return LoadDSImage(szStackedFile, pProgress);
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::ReadSpecificTags(CTIFFReader * tiffReader)
{
	uint32				nrbitmaps = 1,
						settingsapplied = 0;

	if (tiffReader)
	{
		// Read specific fields (if present)
		m_lISOSpeed = tiffReader->GetISOSpeed();
		m_lTotalTime = tiffReader->GetExposureTime();

		if (TIFFGetField(tiffReader->m_tiff, TIFFTAG_DSS_NRFRAMES, &nrbitmaps))
			m_lNrBitmaps = nrbitmaps;
		else
			m_lNrBitmaps = 1;
		
		if (!TIFFGetField(tiffReader->m_tiff, TIFFTAG_DSS_SETTINGSAPPLIED, &settingsapplied))
			settingsapplied = 1;

		if (settingsapplied)
		{
			m_BezierAdjust.Reset(TRUE);
			m_HistoAdjust.Reset();
		}
		else
		{
			char *				szBezierParameters = NULL;
			char *				szAdjustParameters = NULL;
			CString				strBezierParameters;
			CString				strAdjustParameters;

			m_BezierAdjust.Reset();
			if (TIFFGetField(tiffReader->m_tiff, TIFFTAG_DSS_BEZIERSETTINGS, &szBezierParameters))
			{
				strBezierParameters = szBezierParameters;
				if (strBezierParameters.GetLength())
					m_BezierAdjust.FromText(strBezierParameters);
			};

			m_HistoAdjust.Reset();
			if (TIFFGetField(tiffReader->m_tiff, TIFFTAG_DSS_ADJUSTSETTINGS, &szAdjustParameters))
			{
				strAdjustParameters = szAdjustParameters;
				if (strAdjustParameters.GetLength())
					m_HistoAdjust.FromText(strAdjustParameters);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::ReadSpecificTags(CFITSReader * fitsReader)
{
	if (fitsReader)
	{
		// Read specific fields (if present)
		m_lISOSpeed  = fitsReader->m_lISOSpeed;
		m_lTotalTime = fitsReader->m_fExposureTime;
		m_lNrBitmaps = 1;
		
		m_BezierAdjust.Reset(TRUE);
		m_HistoAdjust.Reset();
	};
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::WriteSpecificTags(CTIFFWriter * tiffWriter, BOOL bApplySettings)	
{
	if (tiffWriter)
	{
		// Set specific tags
		TIFFSetField(tiffWriter->m_tiff, TIFFTAG_DSS_NRFRAMES, m_lNrBitmaps);
		TIFFSetField(tiffWriter->m_tiff, TIFFTAG_DSS_SETTINGSAPPLIED, bApplySettings);

		CString				strBezierParameters;
		CString				strHistoParameters;

		m_BezierAdjust.ToText(strBezierParameters);
		TIFFSetField(tiffWriter->m_tiff, TIFFTAG_DSS_BEZIERSETTINGS,
			(LPCSTR)CT2A(strBezierParameters));
		m_HistoAdjust.ToText(strHistoParameters);
		TIFFSetField(tiffWriter->m_tiff, TIFFTAG_DSS_ADJUSTSETTINGS,
			(LPCSTR)CT2A(strHistoParameters));
	};
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::WriteSpecificTags(CFITSWriter * fitsWriter, BOOL bApplySettings)	
{
	if (fitsWriter)
	{
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CTIFFWriterStacker : public CTIFFWriter
{
private :
	LPRECT				m_lprc;
	CStackedBitmap *	m_pStackedBitmap;
	BOOL				m_bApplySettings;
	TIFFFORMAT			m_TiffFormat;
	TIFFCOMPRESSION		m_TiffComp;
	LONG				m_lXStart,
						m_lYStart;

public :
	CTIFFWriterStacker(LPCTSTR szFileName, LPRECT lprc, CDSSProgress *	pProgress) :
	   CTIFFWriter(szFileName, pProgress)
	{
		m_lprc = lprc;
		m_bApplySettings = FALSE;
		m_TiffFormat	 = TF_16BITRGB;
		m_lXStart = 0;
		m_lYStart = 0;
	};

	virtual ~CTIFFWriterStacker()
	{
	};

	void	SetStackedBitmap(CStackedBitmap * pStackedBitmap)
	{
		m_pStackedBitmap = pStackedBitmap;
	};

	void	SetApplySettings(BOOL bApplySettings)
	{
		m_bApplySettings = bApplySettings;
	};

	void	SetTIFFFormat(TIFFFORMAT TiffFormat, TIFFCOMPRESSION TiffComp)
	{
		m_TiffFormat = TiffFormat;
		m_TiffComp   = TiffComp;
	};

	virtual BOOL	OnOpen();
	virtual BOOL	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue);
	virtual BOOL	OnClose();
};

/* ------------------------------------------------------------------- */

BOOL CTIFFWriterStacker::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	LONG			lWidth,
					lHeight;

	if (m_pStackedBitmap)
	{
		lWidth	= m_pStackedBitmap->GetWidth();
		lHeight = m_pStackedBitmap->GetHeight();
		if (!m_lprc)
		{
			m_lXStart = 0;
			m_lYStart = 0;
		}
		else
		{
			m_lprc->left	= max(0, m_lprc->left);
			m_lprc->right	= min(lWidth, m_lprc->right);
			m_lprc->top		= max(0, m_lprc->top);
			m_lprc->bottom	= min(lHeight, m_lprc->bottom);

			lWidth			= (m_lprc->right-m_lprc->left);
			lHeight			= (m_lprc->bottom-m_lprc->top);

			m_lXStart = m_lprc->left;
			m_lYStart = m_lprc->top;
		};

		SetCompression(m_TiffComp);
		SetFormat(lWidth, lHeight, m_TiffFormat, CFATYPE_NONE, FALSE);
		m_pStackedBitmap->WriteSpecificTags(this, m_bApplySettings);
		bResult = TRUE;
	};

	if (m_pProgress)
	{
		CString		strText;

		m_pProgress->SetJointProgress(TRUE);
		strText.Format(IDS_SAVINGTIFF, bpp);

		m_pProgress->Start(strText, 0, FALSE);
		strText.Format(IDS_SAVINGPICTURE, (LPCTSTR)m_strFileName);
		m_pProgress->Progress1(strText, 0);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFWriterStacker::OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue)
{
	BOOL			bResult = TRUE;

	lX += m_lXStart;
	lY += m_lYStart;

	m_pStackedBitmap->GetPixel(lX, lY, fRed, fGreen, fBlue, m_bApplySettings);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFWriterStacker::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveTIFF16Bitmap(LPCTSTR szBitmapFile, LPRECT pRect, CDSSProgress * pProgress, BOOL bApplySettings, TIFFCOMPRESSION TiffComp)
{
	ZFUNCTRACE_RUNTIME();
	CTIFFWriterStacker		tiff(szBitmapFile, pRect, pProgress);
	CString					strText;

	tiff.SetStackedBitmap(this);
	tiff.SetApplySettings(bApplySettings);

	if (m_bMonochrome)
		tiff.SetTIFFFormat(TF_16BITGRAY, TiffComp);
	else
		tiff.SetTIFFFormat(TF_16BITRGB, TiffComp);

	if (bApplySettings)
		strText.LoadString(IDS_SAVEWITHSETTINGSAPPLIED);
	else
		strText.LoadString(IDS_SAVEWITHSETTINGSEMBEDDED);

	tiff.SetDescription(strText);
	if (tiff.Open())
	{
		tiff.Write();
		tiff.Close();
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveTIFF32Bitmap(LPCTSTR szBitmapFile, LPRECT pRect, CDSSProgress * pProgress, BOOL bApplySettings, BOOL bFloat, TIFFCOMPRESSION TiffComp)
{
	ZFUNCTRACE_RUNTIME();
	CTIFFWriterStacker		tiff(szBitmapFile, pRect, pProgress);
	CString					strText;

	tiff.SetStackedBitmap(this);
	tiff.SetApplySettings(bApplySettings);

	if (m_bMonochrome)
	{
		if (bFloat)
			tiff.SetTIFFFormat(TF_32BITGRAYFLOAT, TiffComp);
		else
			tiff.SetTIFFFormat(TF_32BITGRAY, TiffComp);
	}
	else
	{
		if (bFloat)
			tiff.SetTIFFFormat(TF_32BITRGBFLOAT, TiffComp);
		else
			tiff.SetTIFFFormat(TF_32BITRGB, TiffComp);
	};

	if (bApplySettings)
		strText.LoadString(IDS_SAVEWITHSETTINGSAPPLIED);
	else
		strText.LoadString(IDS_SAVEWITHSETTINGSEMBEDDED);

	tiff.SetDescription(strText);
	tiff.SetExposureTime(m_lTotalTime);
	tiff.SetISOSpeed(m_lISOSpeed);

	if (tiff.Open())
	{
		tiff.Write();
		tiff.Close();
	};
};

/* ------------------------------------------------------------------- */

class CFITSWriterStacker : public CFITSWriter
{
private :
	LPRECT				m_lprc;
	CStackedBitmap *	m_pStackedBitmap;
	BOOL				m_bApplySettings;
	FITSFORMAT			m_FitsFormat;
	LONG				m_lXStart,
						m_lYStart;

public :
	CFITSWriterStacker(LPCTSTR szFileName, LPRECT lprc, CDSSProgress *	pProgress) :
	   CFITSWriter(szFileName, pProgress)
	{
		m_lprc = lprc;
		m_bApplySettings = FALSE;
		m_FitsFormat	 = FF_16BITRGB;
		m_lXStart = 0;
		m_lYStart = 0;
	};

	virtual ~CFITSWriterStacker()
	{
	};

	void	SetStackedBitmap(CStackedBitmap * pStackedBitmap)
	{
		m_pStackedBitmap = pStackedBitmap;
	};

	void	SetApplySettings(BOOL bApplySettings)
	{
		m_bApplySettings = bApplySettings;
	};

	void	SetFITSFormat(FITSFORMAT FitsFormat)
	{
		m_FitsFormat = FitsFormat;
	};

	virtual BOOL	OnOpen();
	virtual BOOL	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue);
	virtual BOOL	OnClose();
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriterStacker::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	LONG			lWidth,
					lHeight;

	if (m_pStackedBitmap)
	{
		lWidth	= m_pStackedBitmap->GetWidth();
		lHeight = m_pStackedBitmap->GetHeight();
		if (!m_lprc)
		{
			m_lXStart = 0;
			m_lYStart = 0;
		}
		else
		{
			m_lprc->left	= max(0, m_lprc->left);
			m_lprc->right	= min(lWidth, m_lprc->right);
			m_lprc->top		= max(0, m_lprc->top);
			m_lprc->bottom	= min(lHeight, m_lprc->bottom);

			lWidth			= (m_lprc->right-m_lprc->left);
			lHeight			= (m_lprc->bottom-m_lprc->top);

			m_lXStart = m_lprc->left;
			m_lYStart = m_lprc->top;
		};

		SetFormat(lWidth, lHeight, m_FitsFormat, CFATYPE_NONE);
		m_pStackedBitmap->WriteSpecificTags(this, m_bApplySettings);
		bResult = TRUE;
	};

	if (m_pProgress)
	{
		CString		strText;

		m_pProgress->SetJointProgress(TRUE);
		strText.Format(IDS_SAVINGFITS, m_lBitsPerPixel);

		m_pProgress->Start(strText, 0, FALSE);
		strText.Format(IDS_SAVINGPICTURE, (LPCTSTR)m_strFileName);
		m_pProgress->Progress1(strText, 0);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriterStacker::OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue)
{
	BOOL			bResult = TRUE;

	lX += m_lXStart;
	lY += m_lYStart;

	m_pStackedBitmap->GetPixel(lX, lY, fRed, fGreen, fBlue, m_bApplySettings);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSWriterStacker::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveFITS16Bitmap(LPCTSTR szBitmapFile, LPRECT pRect, CDSSProgress * pProgress, BOOL bApplySettings)
{
	ZFUNCTRACE_RUNTIME();
	CFITSWriterStacker		fits(szBitmapFile, pRect, pProgress);
	CString					strText;

	fits.SetStackedBitmap(this);
	fits.SetApplySettings(bApplySettings);
	if (m_bMonochrome)
		fits.SetFITSFormat(FF_16BITGRAY);
	else
		fits.SetFITSFormat(FF_16BITRGB);

	if (bApplySettings)
		strText.LoadString(IDS_SAVEWITHSETTINGSAPPLIED);
	else
		strText.LoadString(IDS_SAVEWITHSETTINGSEMBEDDED);

	fits.SetDescription(strText);
	fits.m_fExposureTime	= m_lTotalTime;
	fits.m_lISOSpeed		= m_lISOSpeed;
	if (fits.Open())
	{
		fits.Write();
		fits.Close();
	};
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveFITS32Bitmap(LPCTSTR szBitmapFile, LPRECT pRect, CDSSProgress * pProgress, BOOL bApplySettings, BOOL bFloat)
{
	ZFUNCTRACE_RUNTIME();
	CFITSWriterStacker		fits(szBitmapFile, pRect, pProgress);
	CString					strText;

	fits.SetStackedBitmap(this);
	fits.SetApplySettings(bApplySettings);

	if (m_bMonochrome)
	{
		if (bFloat)
			fits.SetFITSFormat(FF_32BITGRAYFLOAT);
		else
			fits.SetFITSFormat(FF_32BITGRAY);
	}
	else
	{
		if (bFloat)
			fits.SetFITSFormat(FF_32BITRGBFLOAT);
		else
			fits.SetFITSFormat(FF_32BITRGB);
	};

	if (bApplySettings)
		strText.LoadString(IDS_SAVEWITHSETTINGSAPPLIED);
	else
		strText.LoadString(IDS_SAVEWITHSETTINGSEMBEDDED);

	fits.SetDescription(strText);
	fits.m_fExposureTime	= m_lTotalTime;
	fits.m_lISOSpeed		= m_lISOSpeed;

	if (fits.Open())
	{
		fits.Write();
		fits.Close();
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CTIFFReadStacker : public CTIFFReader
{
private :
	CStackedBitmap *		m_pStackedBitmap;

public :
	CTIFFReadStacker(LPCTSTR szFileName, CDSSProgress *	pProgress) 
		: CTIFFReader(szFileName, pProgress)
	{
	};

	virtual ~CTIFFReadStacker() {};

	void	SetStackedBitmap(CStackedBitmap * pStackedBitmap)
	{
		m_pStackedBitmap = pStackedBitmap;
	};

	virtual BOOL	OnOpen();
	virtual BOOL	OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue);
	virtual BOOL	OnClose();
};

/* ------------------------------------------------------------------- */

BOOL CTIFFReadStacker::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;

	if (m_pProgress)
	{
		CString		strText;

		m_pProgress->SetJointProgress(TRUE);
		if (spp == 1)
			strText.Format(IDS_LOADGRAYTIFF, bpp);
		else
			strText.Format(IDS_LOADRGBTIFF, bpp);

		m_pProgress->Start(strText, 0, FALSE);
		strText.Format(IDS_LOADPICTURE, (LPCTSTR)m_strFileName);
		m_pProgress->Progress1(strText, 0);
	};

	// Read specific tags
	if (m_pStackedBitmap && m_tiff)
		m_pStackedBitmap->ReadSpecificTags(this);
	bResult = m_pStackedBitmap->Allocate(w, h, (spp==1));

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFReadStacker::OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue)
{
	BOOL			bResult = TRUE;

	if (m_pStackedBitmap)
		m_pStackedBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CTIFFReadStacker::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	return TRUE;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL CStackedBitmap::LoadTIFF(LPCTSTR szStackedFile, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
	CTIFFReadStacker	tiff(szStackedFile, pProgress);

	tiff.SetStackedBitmap(this);
	if (tiff.Open())
	{
		tiff.Read();
		tiff.Close();
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CFITSReadStacker : public CFITSReader
{
private :
	CStackedBitmap *		m_pStackedBitmap;

public :
	CFITSReadStacker(LPCTSTR szFileName, CDSSProgress *	pProgress) 
		: CFITSReader(szFileName, pProgress)
	{
	};

	virtual ~CFITSReadStacker() {};

	void	SetStackedBitmap(CStackedBitmap * pStackedBitmap)
	{
		m_pStackedBitmap = pStackedBitmap;
	};

	virtual BOOL	OnOpen();
	virtual BOOL	OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue);
	virtual BOOL	OnClose();
};

/* ------------------------------------------------------------------- */

BOOL CFITSReadStacker::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;

	if (m_pProgress)
	{
		CString		strText;

		m_pProgress->SetJointProgress(TRUE);
		if (m_lNrChannels == 1)
			strText.Format(IDS_LOADGRAYFITS, m_lBitsPerPixel);
		else
			strText.Format(IDS_LOADRGBFITS, m_lBitsPerPixel);

		m_pProgress->Start(strText, 0, FALSE);
		strText.Format(IDS_LOADPICTURE, (LPCTSTR)m_strFileName);
		m_pProgress->Progress1(strText, 0);
	};

	// Read specific tags
	if (m_pStackedBitmap && m_fits)
		m_pStackedBitmap->ReadSpecificTags(this);
	bResult = m_pStackedBitmap->Allocate(m_lWidth, m_lHeight, (m_lNrChannels == 1));

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReadStacker::OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue)
{
	BOOL			bResult = TRUE;

	if (m_pStackedBitmap)
		m_pStackedBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CFITSReadStacker::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL CStackedBitmap::LoadFITS(LPCTSTR szStackedFile, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
	CFITSReadStacker	fits(szStackedFile, pProgress);

	fits.SetStackedBitmap(this);
	if (fits.Open())
	{
		fits.Read();
		fits.Close();
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

