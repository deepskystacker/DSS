#include <stdafx.h>
#include "dssrect.h"
#include <QPointF>
#include "RunningStackingEngine.h"

#include "MatchingStars.h"
#include "PixelTransform.h"
#include "BitmapIterator.h"
#include <math.h>

#define _USE_MATH_DEFINES
#include <cmath>


CRunningStackingEngine::CRunningStackingEngine() :
	m_lNrStacked{ 0 },
	m_fTotalExposure{ 0 }
{}


void CRunningStackingEngine::CreatePublicBitmap()
{
	ZFUNCTRACE_RUNTIME();

	if (static_cast<bool>(m_pStackedBitmap) && m_lNrStacked != 0)
	{
		const bool bMonochrome = m_pStackedBitmap->IsMonochrome();
		const int lWidth = m_pStackedBitmap->Width();
		const int lHeight = m_pStackedBitmap->Height();

		if (!static_cast<bool>(m_pPublicBitmap))
		{
			if (bMonochrome)
				m_pPublicBitmap = std::make_shared<C16BitGrayBitmap>();
			else
				m_pPublicBitmap = std::make_shared<C48BitColorBitmap>();
			m_pPublicBitmap->Init(lWidth, lHeight);
		}

		BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> itIn{ m_pStackedBitmap };
		BitmapIterator<std::shared_ptr<CMemoryBitmap>> itOut{ m_pPublicBitmap };

		for (int j = 0; j < lHeight; j++)
		{
			for (int i = 0; i < lWidth; i++)
			{
				if (bMonochrome)
				{
					const double gray = itIn.GetPixel() / m_lNrStacked;
					itOut.SetPixel(std::min(gray, 255.0));
				}
				else
				{
					double fRed, fGreen, fBlue;
					itIn.GetPixel(fRed, fGreen, fBlue);
					itOut.SetPixel(std::min(fRed / m_lNrStacked, 255.0), std::min(fGreen / m_lNrStacked, 255.0), std::min(fBlue / m_lNrStacked, 255.0));
				}
				++itIn;
				++itOut;
			}
		}
	}
}


bool CRunningStackingEngine::AddImage(CLightFrameInfo& lfi, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	// First load the input bitmap
	std::shared_ptr<CMemoryBitmap> pBitmap;
	if (::LoadFrame(lfi.filePath.c_str(),PICTURETYPE_LIGHTFRAME, pProgress, pBitmap))
	{
		CString strText;
		pBitmap->RemoveHotPixels(pProgress);
		const int lWidth = pBitmap->Width();
		const int lHeight = pBitmap->Height();
		const bool bColor = !pBitmap->IsMonochrome() || pBitmap->IsCFA();

		// Create the output bitmap
		if (m_lNrStacked == 0)
		{
			if (bColor)
				m_pStackedBitmap = std::make_shared<C96BitFloatColorBitmap>();
			else
				m_pStackedBitmap = std::make_shared<C32BitFloatGrayBitmap>();
			m_pStackedBitmap->Init(lWidth, lHeight);
		}

		if (m_BackgroundCalibration.m_BackgroundCalibrationMode != BCM_NONE)
		{
			if (pProgress != nullptr)
			{
				strText.LoadString(IDS_COMPUTINGBACKGROUNDCALIBRATION);
				pProgress->Start2(strText, 0);
			};
			m_BackgroundCalibration.ComputeBackgroundCalibration(pBitmap.get(), !m_lNrStacked, pProgress);
		};

		// Stack it (average)
		CPixelTransform PixTransform(lfi.m_BilinearParameters);
		CString strDescription;
		PIXELDISPATCHVECTOR vPixels;

		vPixels.reserve(16);

		strDescription = lfi.m_strInfos;
		if (lfi.m_lNrChannels==3)
			strText.Format(IDS_STACKRGBLIGHT, lfi.m_lBitPerChannels, (LPCTSTR)strDescription, (LPCTSTR)lfi.filePath.c_str());
		else
			strText.Format(IDS_STACKGRAYLIGHT, lfi.m_lBitPerChannels, (LPCTSTR)strDescription, (LPCTSTR)lfi.filePath.c_str());

		if (pProgress != nullptr)
			pProgress->Start2(strText, lHeight);

		for (int j = 0; j < lHeight; j++)
		{
			for (int i = 0; i < lWidth; i++)
			{
				double fRed, fGreen, fBlue;
				QPointF pt(i, j);

				const QPointF ptOut = PixTransform.transform(pt);
				pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

				if (m_BackgroundCalibration.m_BackgroundCalibrationMode != BCM_NONE)
					m_BackgroundCalibration.ApplyCalibration(fRed, fGreen, fBlue);

				DSSRect rc{ 0, 0,
					lWidth, lHeight };
				if ((fRed != 0.0 || fGreen != 0.0 || fBlue != 0.0) && rc.contains(ptOut))
				{
					vPixels.resize(0);
					ComputePixelDispatch(ptOut, 1.0, vPixels);

					for (const CPixelDispatch& Pixel : vPixels)
					{
						// For each plane adjust the values
						if (Pixel.m_lX >= 0 && Pixel.m_lX < lWidth && Pixel.m_lY >= 0 && Pixel.m_lY < lHeight)
						{
							double fPreviousRed, fPreviousGreen, fPreviousBlue;
							m_pStackedBitmap->GetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
							fPreviousRed += fRed * Pixel.m_fPercentage;
							fPreviousGreen += fGreen * Pixel.m_fPercentage;
							fPreviousBlue += fBlue * Pixel.m_fPercentage;
							m_pStackedBitmap->SetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
						}
					}
				}
			}
			if (pProgress != nullptr)
				pProgress->Progress2(nullptr, j+1);
		}

		if (pProgress != nullptr)
			pProgress->End2();
		m_lNrStacked++;
		m_fTotalExposure += lfi.m_fExposure;
		bResult = true;
	}

	if (bResult && !m_MatchingStars.IsReferenceSet())
	{
		std::sort(lfi.m_vStars.begin(), lfi.m_vStars.end(), CompareStarLuminancy);

		for (size_t i = 0; i < std::min(lfi.m_vStars.size(), static_cast<STARVECTOR::size_type>(100)); i++)
			m_MatchingStars.AddReferenceStar(lfi.m_vStars[i].m_fX, lfi.m_vStars[i].m_fY);
	}

	if (bResult)
		CreatePublicBitmap();

	return bResult;
}

/* ------------------------------------------------------------------- */

bool	CRunningStackingEngine::ComputeOffset(CLightFrameInfo & lfi)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;

	if (m_lNrStacked)
	{
		m_MatchingStars.ClearTarget();

		STARVECTOR &		vStarsDst = lfi.m_vStars;

		std::sort(vStarsDst.begin(), vStarsDst.end(), CompareStarLuminancy);

		for (int i = 0;i<min(vStarsDst.size(), static_cast<STARVECTOR::size_type>(100));i++)
			m_MatchingStars.AddTargetedStar(vStarsDst[i].m_fX, vStarsDst[i].m_fY);

		m_MatchingStars.SetSizes(lfi.RenderedWidth(), lfi.RenderedHeight());
		bResult = m_MatchingStars.ComputeCoordinateTransformation(lfi.m_BilinearParameters);
	}
	else
	{
		m_MatchingStars.ClearReference();
		lfi.m_BilinearParameters.Clear();
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
