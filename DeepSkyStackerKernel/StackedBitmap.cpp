#include "stdafx.h"
#include "StackedBitmap.h"
#include "TIFFUtil.h"
#include "ColorHelpers.h"
#include "ztrace.h"
#include "DSSProgress.h"
#include "GrayBitmap.h"
#include "ColorBitmap.h"
#include "omp.h"
#include "tiffio.h"
#include "FITSUtil.h"
#include "BitmapExt.h"
#include "Multitask.h"
#include "avx_histogram.h"

using namespace DSS;
/* ------------------------------------------------------------------- */

StackedBitmap::StackedBitmap()
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
}

namespace {
	thread_local constinit int lastY = -1;
	thread_local std::unique_ptr<AvxBezierAndSaturation> pAvxBezierAndSaturation{};
}

//
// Define some convenience "functions" to either turn Visual Leak Detector on and off
// or do nothing
//
#if defined(Q_OS_WIN) && !defined(NDEBUG)
#include <vld.h>
void turnOffVld() { VLDDisable(); }
void turnOnVld() { VLDEnable(); }
#else
void turnOffVld() {}
void turnOnVld() {}
#endif

//
// MT, 11-March-2024
// This function is used in
//     *) CTIFFWriterStacker::OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue);
//     *) CFITSWriterStacker::OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue);
// for saving displayed (stacked or loaded) picture to file.
//
void StackedBitmap::GetPixel(int X, int Y, double& fRed, double& fGreen, double& fBlue, bool bApplySettings)
{
	int				lOffset = m_lWidth * Y + X;

//	double		H, S, L;

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
		//
		// Visual Leak Detector (VLD) under Windows reports false positive leaks for thread_local 
		// allocations (using unique_ptr in this case).
		// 
		// So we turn the leak detection off here, and turn it on again after the allocation.
		//
		turnOffVld();

		const size_t bufferLen = this->m_lWidth;
		if (!static_cast<bool>(pAvxBezierAndSaturation))
			pAvxBezierAndSaturation = std::make_unique<AvxBezierAndSaturation>(bufferLen);

		if (lastY != Y) // New row (lastY is thread_local and initialised to -1, see above).
		{
			lastY = Y;

			const float* const pRed = m_vRedPlane.data() + lOffset;
			const float* const pGreen = m_bMonochrome ? pRed : m_vGreenPlane.data() + lOffset;
			const float* const pBlue = m_bMonochrome ? pRed : m_vBluePlane.data() + lOffset;
			pAvxBezierAndSaturation->copyData(pRed, pGreen, pBlue, bufferLen, m_bMonochrome);

			pAvxBezierAndSaturation->avxAdjustRGB(m_lNrBitmaps, m_HistoAdjust);
			pAvxBezierAndSaturation->avxToHsl(m_BezierAdjust.curvePoints);
			pAvxBezierAndSaturation->avxBezierAdjust(bufferLen);
			pAvxBezierAndSaturation->avxBezierSaturation(bufferLen, static_cast<float>(m_BezierAdjust.m_fSaturationShift));
			pAvxBezierAndSaturation->avxToRgb(QSettings{}.value("ShowBlackWhiteClipping", true).toBool());
		}

		turnOnVld();

		const auto [redBuffer, greenBuffer, blueBuffer] = pAvxBezierAndSaturation->getBufferPtr();
		fRed = redBuffer[X];
		fGreen = greenBuffer[X];
		fBlue = blueBuffer[X];
/*
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
*/
	}
	else
	{
		fRed	/= 256.0;
		fGreen	/= 256.0;
		fBlue	/= 256.0;
	}
}

/* ------------------------------------------------------------------- */
namespace
{
	void limitColorValues(double& red, double& green, double& blue)
	{
		constexpr double UpperLimit = 255.0;
		red = std::min(red, UpperLimit);
		green = std::min(green, UpperLimit);
		blue = std::min(blue, UpperLimit);
	}
}

//
// MT, 11-March-2024
// This function is only used in CStackedBitmap::GetBitmap() for creating star masks.
//
/*
COLORREF StackedBitmap::GetPixel(float fRed, float fGreen, float fBlue, bool bApplySettings)
{
	constexpr double ScalingFactor = 256.0;

	if (true)
	{
		double H, S, L;

		// Adjust beetween 0 and 65535.0
		double Red = fRed / m_lNrBitmaps * ScalingFactor;
		double Green = fGreen / m_lNrBitmaps * ScalingFactor;
		double Blue = fBlue / m_lNrBitmaps * ScalingFactor;

		m_HistoAdjust.Adjust(Red, Green, Blue);

		Red		/= ScalingFactor;
		Green	/= ScalingFactor;
		Blue	/= ScalingFactor;

		limitColorValues(Red, Green, Blue);
		ToHSL(Red, Green, Blue, H, S, L);

		// adjust luminance
		L = m_BezierAdjust.GetValue(L);

		// adjust saturation
		S = m_BezierAdjust.AdjustSaturation(S);

		ToRGB(H, S, L, Red, Green, Blue);

		return static_cast<COLORREF>(RGB(Red, Green, Blue));
	}
	else
	{
		return static_cast<COLORREF>(RGB(fRed / m_lNrBitmaps, fGreen / m_lNrBitmaps, fBlue / m_lNrBitmaps));
	}
}
*/
/* ------------------------------------------------------------------- */

//COLORREF CStackedBitmap::GetPixel(int X, int Y, bool bApplySettings)
//{
//	int				lOffset = m_lWidth * Y + X;
//
//	if (m_bMonochrome)
//		return GetPixel(m_vRedPlane[lOffset], m_vRedPlane[lOffset], m_vRedPlane[lOffset], bApplySettings);
//	else
//		return GetPixel(m_vRedPlane[lOffset], m_vGreenPlane[lOffset], m_vBluePlane[lOffset], bApplySettings);
//};

/* ------------------------------------------------------------------- */
/*
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
*/
/* ------------------------------------------------------------------- */
/*
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
*/

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

//
// Invoked from DeepStack::PartialProcess() to display the picture
//
void StackedBitmap::updateQImage(uchar* pImageData, qsizetype bytes_per_line, DSSRect* pRect)
{
	ZFUNCTRACE_RUNTIME();
	//
	// pImageData is a uchar* pointer to the pre-allocated buffer used by the QImage
	//

	int lXMin = 0;
	int lYMin = 0;
	int lXMax = m_lWidth;
	int lYMax = m_lHeight;

	if (pRect != nullptr)
	{
		lXMin = std::max(0, pRect->left);
		lYMin = std::max(0, pRect->top);
		lXMax = std::min(m_lWidth, pRect->right);
		lYMax = std::min(m_lHeight, pRect->bottom);
	}

	const float* const pBaseRedPixel = m_vRedPlane.data() + (m_lWidth * lYMin + lXMin);
	const float* const pBaseGreenPixel = m_bMonochrome ? nullptr : m_vGreenPlane.data() + (m_lWidth * lYMin + lXMin);
	const float* const pBaseBluePixel = m_bMonochrome ? nullptr : m_vBluePlane.data() + (m_lWidth * lYMin + lXMin);

	const size_t bufferLen = lXMax - lXMin;
	AvxBezierAndSaturation avxBezierAndSaturation{ bufferLen };

#pragma omp parallel for default(shared) shared(lYMin) firstprivate(avxBezierAndSaturation) if(CMultitask::GetNrProcessors() > 1)
	for (int j = lYMin; j < lYMax; j++)
	{
		QRgb* pOutPixel = reinterpret_cast<QRgb*>(pImageData + (bytes_per_line * j) + (lXMin * sizeof(QRgb)));
		//
		// pxxxPixel = pBasexxxPixel + 0, + m_lWidth, +m_lWidth * 2, etc..
		//
		const float* const pRedPixel = pBaseRedPixel + m_lWidth * (j - lYMin);
		const float* const pGreenPixel = m_bMonochrome ? nullptr : pBaseGreenPixel + m_lWidth * (j - lYMin);
		const float* const pBluePixel = m_bMonochrome ? nullptr : pBaseBluePixel + m_lWidth * (j - lYMin);

		avxBezierAndSaturation.copyData(pRedPixel, pGreenPixel, pBluePixel, bufferLen, m_bMonochrome);
		const auto [redBuffer, greenBuffer, blueBuffer] = avxBezierAndSaturation.getBufferPtr();

		avxBezierAndSaturation.avxAdjustRGB(m_lNrBitmaps, m_HistoAdjust);
		avxBezierAndSaturation.avxToHsl(m_BezierAdjust.curvePoints);
		avxBezierAndSaturation.avxBezierAdjust(bufferLen);
		avxBezierAndSaturation.avxBezierSaturation(bufferLen, static_cast<float>(m_BezierAdjust.m_fSaturationShift));
		avxBezierAndSaturation.avxToRgb(QSettings{}.value("ShowBlackWhiteClipping", true).toBool());

		for (size_t n = 0; n < bufferLen; ++n)
		{
			*pOutPixel++ = qRgb(
				std::clamp(redBuffer[n], 0.0F, 255.0F),
				std::clamp(greenBuffer[n], 0.0F, 255.0F),
				std::clamp(blueBuffer[n], 0.0F, 255.0F));
			/*
			*pOutPixel++ = qRgb(
				std::clamp(pRedPixel[n], 0.0F, 255.0F),
				std::clamp(pGreenPixel[n], 0.0F, 255.0F),
				std::clamp(pBluePixel[n], 0.0F, 255.0F));
				*/
		}
	}
}
//
// MT, 11-March-2024
// This function is only used for creating star masks.
//
std::shared_ptr<CMemoryBitmap> StackedBitmap::GetBitmap(ProgressBase* const pProgress)
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
		constexpr int lXMin = 0;
		constexpr int lYMin = 0;
		const int lXMax = m_lWidth;
		const int lYMax = m_lHeight;

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

		const size_t bufferLen = lXMax - lXMin;
		AvxBezierAndSaturation avxBezierAndSaturation{ bufferLen };

#pragma omp parallel for default(shared) shared(lYMin) firstprivate(avxBezierAndSaturation) if(CMultitask::GetNrProcessors() > 1)
		for (int j = lYMin; j < lYMax; ++j)
		{
			//
			// pxxxPixel = pBasexxxPixel + 0, + m_lWidth, +m_lWidth * 2, etc..
			//
			float* pRedPixel = pBaseRedPixel + (m_lWidth * (j - lYMin));
			float* pGreenPixel = m_bMonochrome ? pRedPixel : pBaseGreenPixel + (m_lWidth * (j - lYMin));
			float* pBluePixel = m_bMonochrome ? pRedPixel : pBaseBluePixel + (m_lWidth * (j - lYMin));

			avxBezierAndSaturation.copyData(pRedPixel, pGreenPixel, pBluePixel, bufferLen, m_bMonochrome);

			avxBezierAndSaturation.avxAdjustRGB(m_lNrBitmaps, m_HistoAdjust);
			avxBezierAndSaturation.avxToHsl(m_BezierAdjust.curvePoints);
			avxBezierAndSaturation.avxBezierAdjust(bufferLen);
			avxBezierAndSaturation.avxBezierSaturation(bufferLen, static_cast<float>(m_BezierAdjust.m_fSaturationShift));
			avxBezierAndSaturation.avxToRgb(QSettings{}.value("ShowBlackWhiteClipping", true).toBool());

			const auto [redBuffer, greenBuffer, blueBuffer] = avxBezierAndSaturation.getBufferPtr();

			for (size_t n = 0; n < bufferLen; ++n)
			{
				if (this->m_bMonochrome)
					pBitmap->SetPixel(n + lXMin, j, redBuffer[n]);
				else
					pBitmap->SetPixel(n + lXMin, j, redBuffer[n], greenBuffer[n], blueBuffer[n]);
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

bool StackedBitmap::Load(const fs::path& file, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();

	if (GetPictureInfo(file, bmpInfo) && bmpInfo.CanLoad())
	{
		QString strFileType = bmpInfo.m_strFileType.left(4);

		if (strFileType == "TIFF")
			return LoadTIFF(file, pProgress);
		else if (strFileType == "FITS")
			return LoadFITS(file, pProgress);
		else
			return false;
	}
	else
		return false;
};

/* ------------------------------------------------------------------- */

void StackedBitmap::ReadSpecificTags(CTIFFReader * tiffReader)
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
			m_BezierAdjust.reset(true);
			m_HistoAdjust.reset();
		}
		else
		{
			char* szBezierParameters = nullptr;
			char* szAdjustParameters = nullptr;

			m_BezierAdjust.reset();
			if (TIFFGetField(tiffReader->m_tiff, TIFFTAG_DSS_BEZIERSETTINGS, &szBezierParameters))
			{
				QString strBezierParameters(szBezierParameters);
				if (strBezierParameters.length())
					m_BezierAdjust.fromString(strBezierParameters);
			};

			m_HistoAdjust.reset();
			if (TIFFGetField(tiffReader->m_tiff, TIFFTAG_DSS_ADJUSTSETTINGS, &szAdjustParameters))
			{
				QString strAdjustParameters(szAdjustParameters);
				if (strAdjustParameters.length())
					m_HistoAdjust.FromText(strAdjustParameters);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void StackedBitmap::ReadSpecificTags(CFITSReader * fitsReader)
{
	if (fitsReader)
	{
		// Read specific fields (if present)
		m_lISOSpeed  = fitsReader->m_lISOSpeed;
		m_lGain      = fitsReader->m_lGain;
		m_lTotalTime = fitsReader->m_fExposureTime;

		//
		// If the FITS file keyword NCOMBINE was present use the value from that for
		// the number of images in the stack.  If zero report as 1.
		//
		m_lNrBitmaps = fitsReader->m_nrframes;
		if (0 == m_lNrBitmaps) m_lNrBitmaps = 1;


		m_BezierAdjust.reset(true);
		m_HistoAdjust.reset();
	};
};

/* ------------------------------------------------------------------- */

void StackedBitmap::WriteSpecificTags(CTIFFWriter * tiffWriter, bool bApplySettings)
{
	if (tiffWriter)
	{
		// Set specific tags
		TIFFSetField(tiffWriter->m_tiff, TIFFTAG_DSS_NRFRAMES, m_lNrBitmaps);
		TIFFSetField(tiffWriter->m_tiff, TIFFTAG_DSS_SETTINGSAPPLIED, bApplySettings);

		const QString strBezierParameters{ m_BezierAdjust.toString() };
		QString	strHistoParameters;

		TIFFSetField(tiffWriter->m_tiff, TIFFTAG_DSS_BEZIERSETTINGS, strBezierParameters.toUtf8().constData());
		m_HistoAdjust.ToText(strHistoParameters);
		TIFFSetField(tiffWriter->m_tiff, TIFFTAG_DSS_ADJUSTSETTINGS, strHistoParameters.toUtf8().constData());
	};
};

/* ------------------------------------------------------------------- */

void StackedBitmap::WriteSpecificTags(CFITSWriter* fitsWriter, bool)
{
	if (fitsWriter)
	{}
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CTIFFWriterStacker : public CTIFFWriter
{
private :
	DSSRect rect;
	StackedBitmap *	m_pStackedBitmap;
	bool				m_bApplySettings;
	TIFFFORMAT			m_TiffFormat;
	TIFFCOMPRESSION		m_TiffComp;
	int				m_lXStart,
						m_lYStart;

public :
	CTIFFWriterStacker(const fs::path& p, const DSSRect& rc, ProgressBase *	pProgress) :
	   CTIFFWriter(p, pProgress),
		rect { rc },
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

	void	SetStackedBitmap(StackedBitmap * pStackedBitmap)
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
	virtual bool OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue) override;
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
		if (rect.isEmpty())
		{
			m_lXStart = 0;
			m_lYStart = 0;
		}
		else
		{
			rect.left	= std::max(0, rect.left);
			rect.right = std::min(lWidth, rect.right);
			rect.top		= std::max(0, rect.top);
			rect.bottom = std::min(lHeight, rect.bottom);

			lWidth			= (rect.right-rect.left);
			lHeight			= (rect.bottom-rect.top);

			m_lXStart = rect.left;
			m_lYStart = rect.top;
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
		strText = QCoreApplication::translate("StackedBitmap", "Saving %1", "IDS_SAVINGPICTURE").arg(QString::fromStdU16String(file.generic_u16string()));
		m_pProgress->Progress1(strText, 0);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CTIFFWriterStacker::OnWrite(int lX, int lY, double & fRed, double & fGreen, double & fBlue)
{
	lX += m_lXStart;
	lY += m_lYStart;

	m_pStackedBitmap->GetPixel(lX, lY, fRed, fGreen, fBlue, m_bApplySettings);

	return true;
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

void StackedBitmap::SaveTIFF16Bitmap(const fs::path& file, const DSSRect& rect, ProgressBase * pProgress, bool bApplySettings, TIFFCOMPRESSION TiffComp)
{
	ZFUNCTRACE_RUNTIME();
	CTIFFWriterStacker		tiff(file, rect, pProgress);
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

	tiff.SetDescription(strText);
	if (tiff.Open())
	{
		tiff.Write();
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void StackedBitmap::SaveTIFF32Bitmap(const fs::path& file, const DSSRect& rect, ProgressBase * pProgress, bool bApplySettings, bool bFloat, TIFFCOMPRESSION TiffComp)
{
	ZFUNCTRACE_RUNTIME();
	CTIFFWriterStacker		tiff(file, rect, pProgress);
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

	tiff.SetDescription(strText);
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
	DSSRect rect;
	StackedBitmap *	m_pStackedBitmap;
	bool				m_bApplySettings;
	FITSFORMAT			m_FitsFormat;
	int				m_lXStart,
						m_lYStart;

public :
	CFITSWriterStacker(const fs::path& file, const DSSRect& rc, ProgressBase *	pProgress) :
		CFITSWriter(file, pProgress),
		rect{rc}
	{
		m_bApplySettings = false;
		m_FitsFormat	 = FF_16BITRGB;
		m_lXStart = 0;
		m_lYStart = 0;
        m_pStackedBitmap = NULL;
	};

	virtual ~CFITSWriterStacker()
	{
	};

	void	SetStackedBitmap(StackedBitmap * pStackedBitmap)
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
		if (rect.isEmpty())
		{
			m_lXStart = 0;
			m_lYStart = 0;
		}
		else
		{
			rect.left	= std::max(0, rect.left);
			rect.right = std::min( lWidth, rect.right);
			rect.top		= std::max(0, rect.top);
			rect.bottom = std::min( lHeight, rect.bottom);

			lWidth			= (rect.right-rect.left);
			lHeight			= (rect.bottom-rect.top);

			m_lXStart = rect.left;
			m_lYStart = rect.top;
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
		strText = QCoreApplication::translate("StackedBitmap", "Saving %1", "IDS_SAVINGPICTURE").arg(file.generic_u16string().c_str());
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

void StackedBitmap::SaveFITS16Bitmap(const fs::path& file, const DSSRect& rect, ProgressBase * pProgress, bool bApplySettings)
{
	ZFUNCTRACE_RUNTIME();
	CFITSWriterStacker		fits(file, rect, pProgress);
	QString					strText;

	fits.SetStackedBitmap(this);
	fits.SetApplySettings(bApplySettings);
	fits.m_ExtraInfo = bmpInfo.m_ExtraInfo;

	if (m_bMonochrome)
		fits.SetFITSFormat(FF_16BITGRAY);
	else
		fits.SetFITSFormat(FF_16BITRGB);

	if (bApplySettings)
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings applied.", "IDS_SAVEWITHSETTINGSAPPLIED");
	else
		strText = QCoreApplication::translate("StackedBitmap", "Picture saved with settings embedded.", "IDS_SAVEWITHSETTINGSEMBEDDED");

	fits.SetDescription(strText);
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

void StackedBitmap::SaveFITS32Bitmap(const fs::path& file, const DSSRect& rect, ProgressBase * pProgress, bool bApplySettings, bool bFloat)
{
	ZFUNCTRACE_RUNTIME();
	CFITSWriterStacker		fits(file, rect, pProgress);
	QString					strText;

	fits.SetStackedBitmap(this);
	fits.SetApplySettings(bApplySettings);
	fits.m_ExtraInfo = bmpInfo.m_ExtraInfo;

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

	fits.SetDescription(strText);
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
	StackedBitmap *		m_pStackedBitmap;

public :
	CTIFFReadStacker(const fs::path& file, ProgressBase *	pProgress)
		: CTIFFReader(file, pProgress)
	{
        m_pStackedBitmap = NULL;
	};

	virtual ~CTIFFReadStacker() {};

	void	SetStackedBitmap(StackedBitmap * pStackedBitmap)
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
		strText = QCoreApplication::translate("StackedBitmap", "Loading %1", "IDS_LOADPICTURE").arg(QString::fromStdU16String(file.generic_u16string()));
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

bool StackedBitmap::LoadTIFF(const fs::path& file, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
	CTIFFReadStacker	tiff(file, pProgress);

	tiff.SetStackedBitmap(this);
	if (tiff.Open())
	{
		tiff.Read();
		tiff.Close();
		bResult = true;
	};

	if (IsMonochrome())
	{
		qDebug() << "Final stacked image read back in"
			<< getValue(0, 0) << getValue(1, 0) << getValue(2, 0) << getValue(3, 0)
			<< getValue(4, 0) << getValue(5, 0) << getValue(6, 0) << getValue(7, 0)
			<< getValue(8, 0) << getValue(9, 0) << getValue(10, 0) << getValue(11, 0);
	}
	else
	{
		qDebug() << "Final stacked image read back in:";
		for (size_t i = 0; i < 4; i++)
		{
			auto [r, g, b] = getValues(i, 0);
			qDebug() << r << g << b;
		}
	}

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CFITSReadStacker : public CFITSReader
{
private :
	StackedBitmap *		m_pStackedBitmap;

public :
	CFITSReadStacker(const fs::path& file, ProgressBase *	pProgress)
		: CFITSReader(file, pProgress)
	{
        m_pStackedBitmap = NULL;
	};

	virtual ~CFITSReadStacker() {};

	void	SetStackedBitmap(StackedBitmap * pStackedBitmap)
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
		strText = QCoreApplication::translate("StackedBitmap", "Loading %1", "IDS_LOADPICTURE").arg(file.generic_u16string().c_str());
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

bool StackedBitmap::LoadFITS(const fs::path& file, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
	CFITSReadStacker	fits(file, pProgress);

	fits.SetStackedBitmap(this);
	if (fits.Open())
	{
		fits.Read();
		fits.Close();
		bResult = true;
	};
	if (IsMonochrome())
	{
		qDebug() << "Final stacked image read back in"
			<< getValue(0, 0) << getValue(1, 0) << getValue(2, 0) << getValue(3, 0)
			<< getValue(4, 0) << getValue(5, 0) << getValue(6, 0) << getValue(7, 0)
			<< getValue(8, 0) << getValue(9, 0) << getValue(10, 0) << getValue(11, 0);
	}
	else
	{

		qDebug() << "Final stacked image read back in:";
		for (size_t i = 0; i < 4; i++)
		{
			auto [r, g, b] = getValues(i, 0);
			qDebug() << r << g << b;
		}
	}


	return bResult;
};

/* ------------------------------------------------------------------- */

