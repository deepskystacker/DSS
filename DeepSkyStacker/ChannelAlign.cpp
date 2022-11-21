#include <stdafx.h>
#include "dssrect_utils.h"
#include "ChannelAlign.h"
#include "RegisterEngine.h"
#include "MatchingStars.h"
#include "BitmapIterator.h"

/* ------------------------------------------------------------------- */

void CChannelAlign::CopyBitmap(const CMemoryBitmap* pSrcBitmap, CMemoryBitmap* pTgtBitmap) const
{
	ZFUNCTRACE_RUNTIME();

	const int lHeight = pSrcBitmap->Height();
	const int lWidth = pSrcBitmap->Width();

	BitmapIteratorConst<const CMemoryBitmap*> itSrc{ pSrcBitmap };
	BitmapIterator<CMemoryBitmap*> itTgt{ pTgtBitmap };

	for (int j = 0; j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++, ++itSrc, ++itTgt)
		{
			const double fGray = std::min(itSrc.GetPixel(), 255.0);
			itTgt.SetPixel(fGray);
		}
	}
}


std::shared_ptr<CMemoryBitmap> CChannelAlign::AlignChannel(CMemoryBitmap* pBitmap, CPixelTransform& PixTransform, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();

	CString strText;
	const int lWidth = pBitmap->Width();
	const int lHeight = pBitmap->Height();

	std::shared_ptr<CMemoryBitmap> pOutBitmap{ pBitmap->Clone(true) };
	pOutBitmap->Init(lWidth, lHeight);

	PIXELDISPATCHVECTOR vPixels;
	vPixels.reserve(16);

	if (pProgress != nullptr)
	{
		const QString strText(QObject::tr("Aligning Channel", "IDS_ALIGNINGCHANNEL"));
		pProgress->Start2(strText, lHeight);
	}

	for (int j = 0; j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++)
		{
			double fGray;
			const QPointF ptOut = PixTransform.transform(QPointF(i, j));

			pBitmap->GetPixel(i, j, fGray);
			if (fGray != 0 && DSS::pointIsInRect(ptOut, 0, 0, lWidth - 1, lHeight - 1))
			{
				vPixels.resize(0);
				ComputePixelDispatch(ptOut, 1.0, vPixels);

				for (const CPixelDispatch& pixel : vPixels)
				{
					// For each plane adjust the values
					if (pixel.m_lX >= 0 && pixel.m_lX < lWidth && pixel.m_lY >= 0 && pixel.m_lY < lHeight)
					{
						double fPreviousGray;
						pOutBitmap->GetPixel(pixel.m_lX, pixel.m_lY, fPreviousGray);
						pOutBitmap->SetPixel(pixel.m_lX, pixel.m_lY, fPreviousGray + fGray * pixel.m_fPercentage);
					}
				}
			}
		}
		if (pProgress != nullptr)
			pProgress->Progress2(nullptr, j+1);
	}

	if (pProgress != nullptr)
		pProgress->End2();

	return pOutBitmap;
}


bool CChannelAlign::AlignChannels(CMemoryBitmap* pBitmap, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();

	if (pBitmap->IsMonochrome())
		return true;

	if (CColorBitmap* pColorBitmap = dynamic_cast<CColorBitmap*>(pBitmap))
	{
		CMemoryBitmap* const pRed	= pColorBitmap->GetRed();
		CMemoryBitmap* const pGreen	= pColorBitmap->GetGreen();
		CMemoryBitmap* const pBlue	= pColorBitmap->GetBlue();

		if (pProgress)
		{
			// We will advance the progress1 bar for each channel (3 of them)
			pProgress->Start(nullptr, 3, false);
			pProgress->Progress1(nullptr, 0);
		}

		// Register each channels
		CLightFrameInfo lfiRed;
		CLightFrameInfo lfiGreen;
		CLightFrameInfo lfiBlue;

		lfiRed.SetProgress(pProgress);
		lfiGreen.SetProgress(pProgress);
		lfiBlue.SetProgress(pProgress);

		lfiRed.RegisterPicture(pRed);
		if (pProgress)
			pProgress->Progress1(nullptr, 1);
		lfiGreen.RegisterPicture(pGreen);
		if (pProgress)
			pProgress->Progress1(nullptr, 2);
		lfiBlue.RegisterPicture(pBlue);
		if (pProgress)
			pProgress->Progress1(nullptr, 3);

		// Get the best one to align the others
		CLightFrameInfo* pReference;
		CLightFrameInfo* pSecond;
		CLightFrameInfo* pThird;

//		CMemoryBitmap* pReferenceBitmap;
		CMemoryBitmap* pSecondBitmap;
		CMemoryBitmap* pThirdBitmap;

		const double fMaxScore = std::max(lfiRed.m_fOverallQuality, std::max(lfiGreen.m_fOverallQuality, lfiBlue.m_fOverallQuality));
		if (fMaxScore == lfiRed.m_fOverallQuality)
		{
			pReference	= &lfiRed;
			pSecond		= &lfiGreen;
			pThird		= &lfiBlue;
//			pReferenceBitmap = pRed;
			pSecondBitmap	 = pGreen;
			pThirdBitmap	 = pBlue;
		}
		else if (fMaxScore == lfiGreen.m_fOverallQuality)
		{
			pReference	= &lfiGreen;
			pSecond		= &lfiRed;
			pThird		= &lfiBlue;
//			pReferenceBitmap = pGreen;
			pSecondBitmap	 = pRed;
			pThirdBitmap	 = pBlue;
		}
		else
		{
			pReference	= &lfiBlue;
			pSecond		= &lfiRed;
			pThird		= &lfiGreen;
//			pReferenceBitmap = pBlue;
			pSecondBitmap	 = pRed;
			pThirdBitmap	 = pGreen;
		};

		// Compute the transformations
		CMatchingStars MatchingStars;
		MatchingStars.SetSizes(pBitmap->Width(), pBitmap->Height());
		{
			STARVECTOR& vStarsOrg = pReference->m_vStars;

			std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);

			for (size_t i = 0; i < min(vStarsOrg.size(), static_cast<STARVECTOR::size_type>(100)); i++)
				MatchingStars.AddReferenceStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);
		}

		{
			STARVECTOR& vStarsOrg = pSecond->m_vStars;

			std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);

			for (size_t i = 0; i < min(vStarsOrg.size(), static_cast<STARVECTOR::size_type>(100)); i++)
				MatchingStars.AddTargetedStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);
		}

		bool bTransformationsOk = MatchingStars.ComputeCoordinateTransformation(pSecond->m_BilinearParameters);

		if (bTransformationsOk)
		{
			STARVECTOR& vStarsOrg = pThird->m_vStars;

			std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);

			MatchingStars.ClearTarget();

			for (size_t i = 0; i < min(vStarsOrg.size(), static_cast<STARVECTOR::size_type>(100)); i++)
				MatchingStars.AddTargetedStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);

			bTransformationsOk = MatchingStars.ComputeCoordinateTransformation(pThird->m_BilinearParameters);
		}

		// Align the channels
		if (bTransformationsOk)
		{
			if (pProgress != nullptr)
			{
				// Advance the progress1 bar for each alignment step (2 of them)
				pProgress->Start(nullptr, 2, false);
				pProgress->Progress1(nullptr, 0);
			}

			CPixelTransform pixTransform;
			pixTransform.m_BilinearParameters = pSecond->m_BilinearParameters;
			std::shared_ptr<CMemoryBitmap> pOutSecondBitmap = AlignChannel(pSecondBitmap, pixTransform, pProgress);

			if (pProgress != nullptr)
				pProgress->Progress1(nullptr, 1);

			pixTransform.m_BilinearParameters = pThird->m_BilinearParameters;
			std::shared_ptr<CMemoryBitmap> pOutThirdBitmap = AlignChannel(pThirdBitmap, pixTransform, pProgress);

			if (pProgress != nullptr)
				pProgress->Progress1(nullptr, 2);

			// Dump the resulting modified channels in the image
			CopyBitmap(pOutSecondBitmap.get(), pSecondBitmap);
			CopyBitmap(pOutThirdBitmap.get(), pThirdBitmap);

			return true;
		}
	}

	return false;
}
