#include "pch.h"
#include "RunningStackingEngine.h"
#include "ztrace.h"
#include "GrayBitmap.h"
#include "ColorBitmap.h"
#include "BitmapIterator.h"
#include "StackingTasks.h"
#include "FrameInfo.h"
#include "PixelTransform.h"
#include "Stars.h"
#include "RegisterEngine.h"

using namespace DSS;


void CRunningStackingEngine::CreatePublicBitmap()
{
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


bool CRunningStackingEngine::AddImage(CLightFrameInfo& lfi, OldProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	// First load the input bitmap
	std::shared_ptr<CMemoryBitmap> pBitmap;
	if (::LoadFrame(lfi.filePath, PICTURETYPE_LIGHTFRAME, pProgress, pBitmap))
	{
		QString strText;
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

		const bool isFirst = m_lNrStacked == 0;
		if (isFirst) // This is the first light frame
		{
			this->backgroundCalibration = std::make_unique<BackgroundCalibrator>(makeBackgroundCalibrator(CAllStackingTasks::GetBackgroundCalibrationInterpolation(),
				CAllStackingTasks::GetBackgroundCalibrationMode(), CAllStackingTasks::GetRGBBackgroundCalibrationMethod(), 1.0));
		}
		backgroundCalibration->calculateModelParameters(*pBitmap, isFirst, nullptr);

		// Stack it (average)
		CPixelTransform PixTransform(lfi.m_BilinearParameters);
		PIXELDISPATCHVECTOR vPixels;

		vPixels.reserve(16);

		QString name{ QString::fromStdU16String(lfi.filePath.filename().generic_u16string()) };
		if (lfi.m_lNrChannels == 3)
			strText = QCoreApplication::translate("RunningStackingEngine", "Stacking %1 bit/ch %2 light frame\n%3", "IDS_STACKRGBLIGHT").arg(lfi.m_lBitsPerChannel).arg(lfi.m_strInfos).arg(name);
		else
			strText = QCoreApplication::translate("RunningStackingEngine", "Stacking %1 bits gray %2 light frame\n%3", "IDS_STACKGRAYLIGHT").arg(lfi.m_lBitsPerChannel).arg(lfi.m_strInfos).arg(name);
		
		if (pProgress != nullptr)
			pProgress->Start2(strText, lHeight);

		for (int j = 0; j < lHeight; j++)
		{
			for (int i = 0; i < lWidth; i++)
			{
				const QPointF ptOut = PixTransform.transform(QPointF(i, j));

				double fRed, fGreen, fBlue;
				pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

				const auto [red, green, blue] = backgroundCalibration->calibratePixel(fRed, fGreen, fBlue);

				if ((red != 0.0 || green != 0.0 || blue != 0.0) && DSSRect { 0, 0, lWidth, lHeight }.contains(ptOut))
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
							fPreviousRed += red * Pixel.m_fPercentage;
							fPreviousGreen += green * Pixel.m_fPercentage;
							fPreviousBlue += blue * Pixel.m_fPercentage;
							m_pStackedBitmap->SetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
						}
					}
				}
			}
			if (pProgress != nullptr)
				pProgress->Progress2(j+1);
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

		for (size_t i = 0; i < std::min(lfi.m_vStars.size(), MaxNumberOfConsideredStars); i++)
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

		for (int i = 0;i<min(vStarsDst.size(), MaxNumberOfConsideredStars);i++)
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
