#include "stdafx.h"
#include "StackedBitmap.h"
#include "TIFFUtil.h"
#include "ColorHelpers.h"
#include "Ztrace.h"
#include "DSSProgress.h"
#include "GrayBitmap.h"
#include "ColorBitmap.h"
#include "omp.h"
#include "BitmapInfo.h"
#include "tiffio.h"
#include "FITSUtil.h"

using namespace DSS;
/* ------------------------------------------------------------------- */

CStackedBitmap::CStackedBitmap()
{
	m_lNrBitmaps	= 0;
	m_lWidth		= 0;
	m_lHeight		= 0;
	m_lOutputWidth	= 0;
	m_lOutputHeight = 0;
	m_lISOSpeed		= 0;
	m_lGain		= -1;
	m_lTotalTime	= 0;
	m_bMonochrome   = false;
	DSSTIFFInitialize();
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::GetPixel(int X, int Y, double& fRed, double& fGreen, double& fBlue, bool bApplySettings)
{
	int				lOffset = m_lWidth * Y + X;

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

	//
	// Ensure pixel values don't exceed USHRT_MAX
	// 
	constexpr double limit{ std::numeric_limits<std::uint16_t>::max() };
	fRed = std::min(limit, fRed);
	fGreen = std::min(limit, fGreen);
	fBlue = std::min(limit, fBlue);

	if (bApplySettings)
	{
		m_HistoAdjust.Adjust(fRed, fGreen, fBlue);

		fRed	/= 256.0;
		fGreen	/= 256.0;
		fBlue	/= 256.0;

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

COLORREF CStackedBitmap::GetPixel(float fRed, float fGreen, float fBlue, bool bApplySettings)
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

COLORREF CStackedBitmap::GetPixel(int X, int Y, bool bApplySettings)
{
	int				lOffset = m_lWidth * Y + X;

	if (m_bMonochrome)
		return GetPixel(m_vRedPlane[lOffset], m_vRedPlane[lOffset], m_vRedPlane[lOffset], bApplySettings);
	else
		return GetPixel(m_vRedPlane[lOffset], m_vGreenPlane[lOffset], m_vBluePlane[lOffset], bApplySettings);
};

/* ------------------------------------------------------------------- */

COLORREF16	CStackedBitmap::GetPixel16(int X, int Y, bool bApplySettings)
{
	COLORREF16			crResult;

	int				lOffset = m_lWidth * Y + X;
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

COLORREF32	CStackedBitmap::GetPixel32(int X, int Y, bool bApplySettings)
{
	COLORREF32			crResult;

	int				lOffset = m_lWidth * Y + X;
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

constexpr std::uint32_t HDSTACKEDBITMAP_MAGIC = 0x878A56E6U;

typedef struct tagHDSTACKEDBITMAPHEADER
{
	std::uint32_t dwMagic;		// Magic number (always HDSTACKEDBITMAP_MAGIC)
	std::uint32_t dwHeaderSize;	// Always sizeof(HDSTACKEDBITMAPHEADER);
	int			lWidth;			// Width
	int			lHeight;		// Height
	int			lNrBitmaps;		// Number of bitmaps
	std::uint32_t dwFlags;		// Flags
	int			lTotalTime;		// Total Time
	std::uint16_t lISOSpeed;		// ISO Speed of each frame
	int			lGain;		// Camera gain of each frame
	int			Reserved[22];	// Reserved (set to 0)
} HDSTACKEDBITMAPHEADER;

#pragma pack(pop, HDSTACKEDBITMAP)

/* ------------------------------------------------------------------- */

bool CStackedBitmap::LoadDSImage(LPCTSTR szStackedFile, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	FILE *			hFile;
	LPCSTR			strFile = CT2CA(szStackedFile, CP_UTF8); // Stacked fileid in ASCII

	QString strText(QCoreApplication::translate("StackedBitmap", "Loading DSImage", "IDS_LOADDSIMAGE"));
	if (pProgress)
		pProgress->Start1(strText, 0, false);

	hFile = _tfopen(szStackedFile, _T("rb"));
	if (hFile)
	{
		HDSTACKEDBITMAPHEADER	Header;
		int					lProgress = 0;

		if (pProgress)
		{
			strText = QCoreApplication::translate("StackedBitmap", "Loading %1", "IDS_LOADPICTURE").arg(szStackedFile);
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
			m_lGain     = Header.lGain;

			Allocate(Header.lWidth, Header.lHeight, false);

			if (pProgress)
				pProgress->Start1(m_lWidth * m_lHeight, false);

			for (int i = 0;i<m_vRedPlane.size();i++)
			{
				lProgress++;
				if (pProgress)
					pProgress->Progress1(lProgress);

				fread(&m_vRedPlane[i], sizeof(float), 1, hFile);
				fread(&m_vGreenPlane[i], sizeof(float), 1, hFile);
				fread(&m_vBluePlane[i], sizeof(float), 1, hFile);
			};

			bResult = true;
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

void CStackedBitmap::SaveDSImage(LPCTSTR szStackedFile, LPRECT pRect, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	FILE *			hFile;
	LPCSTR			strFile = CT2CA(szStackedFile, CP_ACP);  

	printf("Saving Stacked Bitmap in %s\n", strFile);
	ZTRACE_RUNTIME("Saving Stacked Bitmap in %s", strFile);
	hFile = _tfopen(szStackedFile, _T("wb"));
	if (hFile)
	{
		HDSTACKEDBITMAPHEADER	Header;

		int		lWidth,
					lHeight;
		int		lStartX,
					lStartY;
		int		i, j;

		int		lProgress = 0;

		if (pRect)
		{
			pRect->left		= std::max(0L, pRect->left);
			pRect->right = std::min(decltype(tagRECT::right){ m_lWidth }, pRect->right);
			pRect->top		= std::max(0L, pRect->top);
			pRect->bottom = std::min(decltype(tagRECT::bottom){ m_lHeight }, pRect->bottom);

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
			QString strText(QCoreApplication::translate("StackedBitmap", "Saving DSImage File", "IDS_SAVINGDSIMAGE"));
			pProgress->Start1(strText, lWidth * lHeight, false);
			strText = QCoreApplication::translate("StackedBitmap", "Saving stacked picture in %1 (DSImage)", "IDS_SAVEDSIMAGE").arg(szStackedFile);
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
		Header.lGain		= m_lGain;

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
				pProgress->Progress1(lProgress);
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
// 
// class CPixel
// {
// public :
// 	double			m_fRed,
// 					m_fGreen,
// 					m_fBlue;
// private :
// 	void	CopyFrom(const CPixel & px)
// 	{
// 		m_fRed		= px.m_fRed;
// 		m_fGreen	= px.m_fGreen;
// 		m_fBlue		= px.m_fBlue;
// 	};
// public :
// 	CPixel(double fRed = 0, double fGreen = 0, double fBlue = 0)
// 	{
// 		m_fRed		= fRed;
// 		m_fGreen	= fGreen;
// 		m_fBlue		= fBlue;
// 	};
// 	CPixel(const CPixel & px)
// 	{
// 		CopyFrom(px);
// 	};
// 
//     CPixel& operator=(CPixel const& other) = delete;
// 
// 	~CPixel() {};
// 
// 	bool operator < (const CPixel & px) const
// 	{
// 		if (m_fRed<px.m_fRed)
// 			return true;
// 		else if (m_fRed>px.m_fRed)
// 			return false;
// 		else if (m_fGreen<px.m_fGreen)
// 			return true;
// 		else if (m_fGreen>px.m_fGreen)
// 			return false;
// 		else
// 			return m_fBlue<px.m_fBlue;
// 	};
// };

HBITMAP CStackedBitmap::GetHBitmap(C32BitsBitmap & Bitmap, RECT * pRect)
{
	if (Bitmap.IsEmpty())
		Bitmap.Create(m_lWidth, m_lHeight);

	if (!Bitmap.IsEmpty())
	{
		int		lXMin = 0,
					lYMin = 0,
					lXMax = m_lWidth,
					lYMax = m_lHeight;

		if (pRect)
		{
			lXMin	= std::max(0L, pRect->left);
			lYMin	= std::max(0L, pRect->top);
			lXMax = std::min(decltype(tagRECT::right){ m_lWidth }, pRect->right);
			lYMax = std::min(decltype(tagRECT::bottom){ m_lHeight }, pRect->bottom);
		};

		/*PIXELSET		sPixels;
		PIXELITERATOR	it;*/

		float *				pBaseRedPixel;
		float *				pBaseGreenPixel = nullptr;
		float *				pBaseBluePixel  = nullptr;

		pBaseRedPixel	= &(m_vRedPlane[m_lWidth * lYMin + lXMin]);
		if (!m_bMonochrome)
		{
			pBaseGreenPixel = &(m_vGreenPlane[m_lWidth * lYMin + lXMin]);
			pBaseBluePixel	= &(m_vBluePlane[m_lWidth * lYMin + lXMin]);
		};

#pragma omp parallel for schedule(dynamic, 50) default(none) shared(lYMin)
		for (int j = lYMin;j<lYMax;j++)
		{
			std::uint8_t* lpOut = Bitmap.GetPixelBase(lXMin, j);
			LPRGBQUAD& lpOutPixel = reinterpret_cast<LPRGBQUAD&>(lpOut);
			float* pRedPixel = nullptr;;
			float* pGreenPixel = nullptr;
			float* pBluePixel = nullptr;

			//
			// pxxxPixel = pBasexxxPixel + 0, + m_lWidth, +m_lWidth * 2, etc..
			//
			pRedPixel	= pBaseRedPixel + (m_lWidth * (j - lYMin));
			if (!m_bMonochrome)
			{
				pGreenPixel = pBaseGreenPixel + (m_lWidth * (j - lYMin));
				pBluePixel  = pBaseBluePixel + (m_lWidth * (j - lYMin));
			};
			for (int i = lXMin;i<lXMax;i++)
			{
				COLORREF		crColor;

				if (!m_bMonochrome)
					crColor = GetPixel(*pRedPixel, *pGreenPixel, *pBluePixel, true);
				else
					crColor = GetPixel(*pRedPixel, *pRedPixel, *pRedPixel, true);

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
		};

		/*int				lNrPixels = sPixels.size();
		printf("%ld", lNrPixels);*/
	};

	return Bitmap.GetHBITMAP();
};

/* ------------------------------------------------------------------- */

std::shared_ptr<CMemoryBitmap> CStackedBitmap::GetBitmap(ProgressBase* const pProgress)
{
	ZFUNCTRACE_RUNTIME();

	std::shared_ptr<CMemoryBitmap> pBitmap;
	if (m_bMonochrome)
		pBitmap = std::make_shared<C32BitFloatGrayBitmap>();
	else
		pBitmap = std::make_shared<C96BitFloatColorBitmap>();
	pBitmap->Init(m_lWidth, m_lHeight);

	if (static_cast<bool>(pBitmap))
	{
		const int lXMin = 0, lYMin = 0, lXMax = m_lWidth, lYMax = m_lHeight;

		float* pBaseRedPixel;
		float* pBaseGreenPixel = nullptr;
		float* pBaseBluePixel = nullptr;
		int iProgress = 0;

		if (pProgress != nullptr)
		{
			QString			strText;
			strText = QCoreApplication::translate("StackedBitmap", "Processing Image...", "IDS_PROCESSINGIMAGE");
			pProgress->Start2(strText, lYMax - lYMin);
		}

		pBaseRedPixel = m_vRedPlane.data() + (m_lWidth * lYMin + lXMin);
		if (!m_bMonochrome)
		{
			pBaseGreenPixel = m_vGreenPlane.data() + (m_lWidth * lYMin + lXMin);
			pBaseBluePixel = m_vBluePlane.data() + (m_lWidth * lYMin + lXMin);
		}

#pragma omp parallel for default(none) schedule(static, 100)
		for (int j = lYMin; j < lYMax; j++)
		{
			//
			// pxxxPixel = pBasexxxPixel + 0, + m_lWidth, +m_lWidth * 2, etc..
			//
			float* pRedPixel = pBaseRedPixel + (m_lWidth * (j - lYMin));
			float* pGreenPixel = m_bMonochrome ? pRedPixel : pBaseGreenPixel + (m_lWidth * (j - lYMin));
			float* pBluePixel = m_bMonochrome ? pRedPixel : pBaseBluePixel + (m_lWidth * (j - lYMin));

			for (int i = lXMin; i < lXMax; i++)
			{
				COLORREF crColor;

				if (!m_bMonochrome)
				{
					crColor = GetPixel(*pRedPixel, *pGreenPixel, *pBluePixel, true);
					pBitmap->SetPixel(i, j, GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
				}
				else
				{
					crColor = GetPixel(*pRedPixel, *pRedPixel, *pRedPixel, true);
					pBitmap->SetPixel(i, j, GetRValue(crColor));
				}

				pRedPixel++;
				pGreenPixel++; // Incrementing the pointers is harmless, even if we don't use them due to monochrome image.
				pBluePixel++;
			}
			if (pProgress != nullptr && 0 == omp_get_thread_num())	// Are we on the master thread?
			{
				iProgress += omp_get_num_threads();
				pProgress->Progress2(iProgress);
			}
		}
		if (pProgress != nullptr)
			pProgress->End2();
	}
	return pBitmap;
}

/* ------------------------------------------------------------------- */

bool CStackedBitmap::Load(LPCTSTR szStackedFile, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CBitmapInfo		bmpInfo;

	if (GetPictureInfo(szStackedFile, bmpInfo) && bmpInfo.CanLoad())
	{
		QString strFileType = bmpInfo.m_strFileType.left(4);

		if (strFileType == "TIFF")
			return LoadTIFF(szStackedFile, pProgress);
		else if (strFileType == "FITS")
			return LoadFITS(szStackedFile, pProgress);
		else
			return false;
	}
	else
		return LoadDSImage(szStackedFile, pProgress);
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::ReadSpecificTags(CTIFFReader * tiffReader)
{
	uint32_t				nrbitmaps = 1,
						settingsapplied = 0;

	if (tiffReader)
	{
		// Read specific fields (if present)
		m_lISOSpeed = tiffReader->GetISOSpeed();
		m_lGain = tiffReader->GetGain();
		m_lTotalTime = tiffReader->GetExposureTime();

		if (TIFFGetField(tiffReader->m_tiff, TIFFTAG_DSS_NRFRAMES, &nrbitmaps))
			m_lNrBitmaps = nrbitmaps;
		else
			m_lNrBitmaps = 1;

		if (!TIFFGetField(tiffReader->m_tiff, TIFFTAG_DSS_SETTINGSAPPLIED, &settingsapplied))
			settingsapplied = 1;

		if (settingsapplied)
		{
			m_BezierAdjust.Reset(true);
			m_HistoAdjust.Reset();
		}
		else
		{
			char *				szBezierParameters = nullptr;
			char *				szAdjustParameters = nullptr;
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
		m_lGain      = fitsReader->m_lGain;
		m_lTotalTime = fitsReader->m_fExposureTime;
		m_lNrBitmaps = 1;

		m_BezierAdjust.Reset(true);
		m_HistoAdjust.Reset();
	};
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::WriteSpecificTags(CTIFFWriter * tiffWriter, bool bApplySettings)
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

void CStackedBitmap::WriteSpecificTags(CFITSWriter* fitsWriter, bool)
{
	if (fitsWriter)
	{}
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CTIFFWriterStacker : public CTIFFWriter
{
private :
	LPRECT				m_lprc;
	CStackedBitmap *	m_pStackedBitmap;
	bool				m_bApplySettings;
	TIFFFORMAT			m_TiffFormat;
	TIFFCOMPRESSION		m_TiffComp;
	int				m_lXStart,
						m_lYStart;

public :
	CTIFFWriterStacker(LPCTSTR szFileName, LPRECT lprc, ProgressBase *	pProgress) :
	   CTIFFWriter(szFileName, pProgress),
		m_lprc { lprc },
		m_pStackedBitmap{ nullptr },
		m_bApplySettings { false },
		m_TiffFormat{ TF_16BITRGB },
		m_TiffComp { TC_NONE },
		m_lXStart { 0 },
		m_lYStart { 0 }
	{
	};

	virtual ~CTIFFWriterStacker()
	{
		OnClose();
	};

	void	SetStackedBitmap(CStackedBitmap * pStackedBitmap)
	{
		m_pStackedBitmap = pStackedBitmap;
	};

	void	SetApplySettings(bool bApplySettings)
	{
		m_bApplySettings = bApplySettings;
	};

	void	SetTIFFFormat(TIFFFORMAT TiffFormat, TIFFCOMPRESSION TiffComp)
	{
		m_TiffFormat = TiffFormat;
		m_TiffComp   = TiffComp;
	};

	virtual bool OnOpen() override;
	void OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue) override;
	virtual bool OnClose() override;
};

/* ------------------------------------------------------------------- */

bool CTIFFWriterStacker::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	int			lWidth,
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
			m_lprc->left	= std::max(0L, m_lprc->left);
			m_lprc->right = std::min(decltype(tagRECT::right){ lWidth }, m_lprc->right);
			m_lprc->top		= std::max(0L, m_lprc->top);
			m_lprc->bottom = std::min(decltype(tagRECT::bottom){ lHeight }, m_lprc->bottom);

			lWidth			= (m_lprc->right-m_lprc->left);
			lHeight			= (m_lprc->bottom-m_lprc->top);

			m_lXStart = m_lprc->left;
			m_lYStart = m_lprc->top;
		};

		SetCompression(m_TiffComp);
		SetFormat(lWidth, lHeight, m_TiffFormat, CFATYPE_NONE, false);
		m_pStackedBitmap->WriteSpecificTags(this, m_bApplySettings);
		bResult = true;
	};

	if (m_pProgress)
	{
		QString		strText;

		m_pProgress->SetJointProgress(true);
		strText = QCoreApplication::translate("StackedBitmap", "Saving TIFF %1 bit", "IDS_SAVINGTIFF").arg(bps);

		m_pProgress->Start1(strText, 0, false);
		strText = QCoreApplication::translate("StackedBitmap", "Saving %1", "IDS_SAVINGPICTURE").arg(QString::fromWCharArray(m_strFileName.GetString()));
		m_pProgress->Progress1(strText, 0);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CTIFFWriterStacker::OnWrite(int lX, int lY, double & fRed, double & fGreen, double & fBlue)
{
	lX += m_lXStart;
	lY += m_lYStart;

	m_pStackedBitmap->GetPixel(lX, lY, fRed, fGreen, fBlue, m_bApplySettings);

	return;
};

/* ------------------------------------------------------------------- */

bool CTIFFWriterStacker::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveTIFF16Bitmap(LPCTSTR szBitmapFile, LPRECT pRect, ProgressBase * pProgress, bool bApplySettings, TIFFCOMPRESSION TiffComp)
{
	ZFUNCTRACE_RUNTIME();
	CTIFFWriterStacker		tiff(szBitmapFile, pRect, pProgress);
	QString					strText;

	tiff.SetStackedBitmap(this);
	tiff.SetApplySettings(bApplySettings);

	if (m_bMonochrome)
		tiff.SetTIFFFormat(TF_16BITGRAY, TiffComp);
	else
		tiff.SetTIFFFormat(TF_16BITRGB, TiffComp);

	if (bApplySettings)
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings applied.", "IDS_SAVEWITHSETTINGSAPPLIED");
	else
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings embedded.", "IDS_SAVEWITHSETTINGSEMBEDDED");

	tiff.SetDescription(strText.toStdWString().c_str());
	if (tiff.Open())
	{
		tiff.Write();
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveTIFF32Bitmap(LPCTSTR szBitmapFile, LPRECT pRect, ProgressBase * pProgress, bool bApplySettings, bool bFloat, TIFFCOMPRESSION TiffComp)
{
	ZFUNCTRACE_RUNTIME();
	CTIFFWriterStacker		tiff(szBitmapFile, pRect, pProgress);
	QString					strText;

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
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings applied.", "IDS_SAVEWITHSETTINGSAPPLIED");
	else
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings embedded.", "IDS_SAVEWITHSETTINGSEMBEDDED");

	tiff.SetDescription(strText.toStdWString().c_str());
	tiff.SetExposureTime(m_lTotalTime);
	tiff.SetISOSpeed(m_lISOSpeed);
	tiff.SetGain(m_lGain);

	if (tiff.Open())
	{
		tiff.Write();
	};
};

/* ------------------------------------------------------------------- */

class CFITSWriterStacker : public CFITSWriter
{
private :
	LPRECT				m_lprc;
	CStackedBitmap *	m_pStackedBitmap;
	bool				m_bApplySettings;
	FITSFORMAT			m_FitsFormat;
	int				m_lXStart,
						m_lYStart;

public :
	CFITSWriterStacker(LPCTSTR szFileName, LPRECT lprc, ProgressBase *	pProgress) :
	   CFITSWriter(szFileName, pProgress)
	{
		m_lprc = lprc;
		m_bApplySettings = false;
		m_FitsFormat	 = FF_16BITRGB;
		m_lXStart = 0;
		m_lYStart = 0;
        m_pStackedBitmap = NULL;
	};

	virtual ~CFITSWriterStacker()
	{
	};

	void	SetStackedBitmap(CStackedBitmap * pStackedBitmap)
	{
		m_pStackedBitmap = pStackedBitmap;
	};

	void	SetApplySettings(bool bApplySettings)
	{
		m_bApplySettings = bApplySettings;
	};

	void	SetFITSFormat(FITSFORMAT FitsFormat)
	{
		m_FitsFormat = FitsFormat;
	};

	virtual bool OnOpen() override;
	virtual bool OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue) override;
	virtual bool OnClose() override;
};

/* ------------------------------------------------------------------- */

bool CFITSWriterStacker::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	int			lWidth, lHeight;

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
			m_lprc->left	= std::max(0L, m_lprc->left);
			m_lprc->right = std::min(decltype(tagRECT::right){ lWidth }, m_lprc->right);
			m_lprc->top		= std::max(0L, m_lprc->top);
			m_lprc->bottom = std::min(decltype(tagRECT::bottom){ lHeight }, m_lprc->bottom);

			lWidth			= (m_lprc->right-m_lprc->left);
			lHeight			= (m_lprc->bottom-m_lprc->top);

			m_lXStart = m_lprc->left;
			m_lYStart = m_lprc->top;
		};

		SetFormat(lWidth, lHeight, m_FitsFormat, CFATYPE_NONE);
		m_pStackedBitmap->WriteSpecificTags(this, m_bApplySettings);
		bResult = true;
	};

	if (m_pProgress)
	{
		QString		strText;

		m_pProgress->SetJointProgress(true);
		strText = QCoreApplication::translate("StackedBitmap", "Saving FITS %1 bit", "IDS_SAVINGFITS").arg(m_lBitsPerPixel);

		m_pProgress->Start1(strText, 0, false);
		strText = QCoreApplication::translate("StackedBitmap", "Saving %1", "IDS_SAVINGPICTURE").arg(QString::fromWCharArray(m_strFileName.GetString()));
		m_pProgress->Progress1(strText, 0);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CFITSWriterStacker::OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue)
{
	bool			bResult = true;

	lX += m_lXStart;
	lY += m_lYStart;

	m_pStackedBitmap->GetPixel(lX, lY, fRed, fGreen, fBlue, m_bApplySettings);

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CFITSWriterStacker::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveFITS16Bitmap(LPCTSTR szBitmapFile, LPRECT pRect, ProgressBase * pProgress, bool bApplySettings)
{
	ZFUNCTRACE_RUNTIME();
	CFITSWriterStacker		fits(szBitmapFile, pRect, pProgress);
	QString					strText;

	fits.SetStackedBitmap(this);
	fits.SetApplySettings(bApplySettings);
	if (m_bMonochrome)
		fits.SetFITSFormat(FF_16BITGRAY);
	else
		fits.SetFITSFormat(FF_16BITRGB);

	if (bApplySettings)
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings applied.", "IDS_SAVEWITHSETTINGSAPPLIED");
	else
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings embedded.", "IDS_SAVEWITHSETTINGSEMBEDDED");

	fits.SetDescription(strText.toStdWString().c_str());
	fits.m_fExposureTime	= m_lTotalTime;
	fits.m_lISOSpeed		= m_lISOSpeed;
	fits.m_lGain		= m_lGain;
	if (fits.Open())
	{
		fits.Write();
		fits.Close();
	};
};

/* ------------------------------------------------------------------- */

void CStackedBitmap::SaveFITS32Bitmap(LPCTSTR szBitmapFile, LPRECT pRect, ProgressBase * pProgress, bool bApplySettings, bool bFloat)
{
	ZFUNCTRACE_RUNTIME();
	CFITSWriterStacker		fits(szBitmapFile, pRect, pProgress);
	QString					strText;

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
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings applied.", "IDS_SAVEWITHSETTINGSAPPLIED");
	else
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings embedded.", "IDS_SAVEWITHSETTINGSEMBEDDED");

	fits.SetDescription(strText.toStdWString().c_str());
	fits.m_fExposureTime	= m_lTotalTime;
	fits.m_lISOSpeed		= m_lISOSpeed;
	fits.m_lGain		= m_lGain;

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
	CTIFFReadStacker(LPCTSTR szFileName, ProgressBase *	pProgress)
		: CTIFFReader(szFileName, pProgress)
	{
        m_pStackedBitmap = NULL;
	};

	virtual ~CTIFFReadStacker() {};

	void	SetStackedBitmap(CStackedBitmap * pStackedBitmap)
	{
		m_pStackedBitmap = pStackedBitmap;
	};

	virtual bool	OnOpen() override;
	bool	OnRead(int lX, int lY, double fRed, double fGreen, double fBlue) override;
	virtual bool	OnClose() override;
};

/* ------------------------------------------------------------------- */

bool CTIFFReadStacker::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

	if (m_pProgress)
	{
		QString		strText;

		m_pProgress->SetJointProgress(true);
		if (spp == 1)
			strText = QCoreApplication::translate("StackedBitmap", "Loading TIFF %1 bit monochrome", "IDS_LOADGRAYTIFF").arg(bps);
		else
			strText = QCoreApplication::translate("StackedBitmap", "Loading TIFF %1 bit/ch", "IDS_LOADRGBTIFF").arg(bps);

		m_pProgress->Start1(strText, 0, false);
		strText = QCoreApplication::translate("StackedBitmap", "Loading %1", "IDS_LOADPICTURE").arg(QString::fromWCharArray(m_strFileName.GetString()));
		m_pProgress->Progress1(strText, 0);
	};

	// Read specific tags
	if (m_pStackedBitmap && m_tiff)
		m_pStackedBitmap->ReadSpecificTags(this);
	bResult = m_pStackedBitmap->Allocate(w, h, (spp==1));

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CTIFFReadStacker::OnRead(int lX, int lY, double fRed, double fGreen, double fBlue)
{
	if (m_pStackedBitmap)
		m_pStackedBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);

	return true;
};

/* ------------------------------------------------------------------- */

bool CTIFFReadStacker::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	return true;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

bool CStackedBitmap::LoadTIFF(LPCTSTR szStackedFile, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
	CTIFFReadStacker	tiff(szStackedFile, pProgress);

	tiff.SetStackedBitmap(this);
	if (tiff.Open())
	{
		tiff.Read();
		tiff.Close();
		bResult = true;
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
	CFITSReadStacker(LPCTSTR szFileName, ProgressBase *	pProgress)
		: CFITSReader(szFileName, pProgress)
	{
        m_pStackedBitmap = NULL;
	};

	virtual ~CFITSReadStacker() {};

	void	SetStackedBitmap(CStackedBitmap * pStackedBitmap)
	{
		m_pStackedBitmap = pStackedBitmap;
	};

	virtual bool	OnOpen() override;
	virtual bool	OnRead(int lX, int lY, double fRed, double fGreen, double fBlue) override;
	virtual bool	OnClose() override;
};

/* ------------------------------------------------------------------- */

bool CFITSReadStacker::OnOpen()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

	if (m_pProgress)
	{
		QString		strText;

		m_pProgress->SetJointProgress(true);
		if (m_lNrChannels == 1)
			strText = QCoreApplication::translate("StackedBitmap", "Loading FITS %1 bit monochrome", "IDS_LOADGRAYFITS").arg(m_lBitsPerPixel);
		else
			strText = QCoreApplication::translate("StackedBitmap", "Loading FITS %1 bit/ch", "IDS_LOADRGBFITS").arg(m_lBitsPerPixel);

		m_pProgress->Start1(strText, 0, false);
		strText = QCoreApplication::translate("StackedBitmap", "Loading %1", "IDS_LOADPICTURE").arg(QString::fromWCharArray(m_strFileName.GetString()));
		m_pProgress->Progress1(strText, 0);
	};

	// Read specific tags
	if (m_pStackedBitmap && m_fits)
		m_pStackedBitmap->ReadSpecificTags(this);
	bResult = m_pStackedBitmap->Allocate(m_lWidth, m_lHeight, (m_lNrChannels == 1));

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CFITSReadStacker::OnRead(int lX, int lY, double fRed, double fGreen, double fBlue)
{
	bool			bResult = true;

	if (m_pStackedBitmap)
		m_pStackedBitmap->SetPixel(lX, lY, fRed, fGreen, fBlue);

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CFITSReadStacker::OnClose()
{
	ZFUNCTRACE_RUNTIME();
	return true;
};

/* ------------------------------------------------------------------- */

bool CStackedBitmap::LoadFITS(LPCTSTR szStackedFile, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
	CFITSReadStacker	fits(szStackedFile, pProgress);

	fits.SetStackedBitmap(this);
	if (fits.Open())
	{
		fits.Read();
		fits.Close();
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

