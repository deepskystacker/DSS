#include "stdafx.h"
#include "dssrect.h"
#include "ChannelAlign.h"
#include "RegisterEngine.h"
#include "MatchingStars.h"
#include "BitmapIterator.h"
#include "ztrace.h"
#include "ColorBitmap.h"
#include "Multitask.h"

/* ------------------------------------------------------------------- */

// static
void CChannelAlign::CopyBitmap(std::shared_ptr<const CMemoryBitmap> pSrcBitmap, CMemoryBitmap* pTgtBitmap)
{
	ZFUNCTRACE_RUNTIME();

	const int lHeight = pSrcBitmap->Height();
	const int lWidth = pSrcBitmap->Width();

	BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> itSrc{ pSrcBitmap };
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

// static
std::shared_ptr<CMemoryBitmap> CChannelAlign::AlignChannel(const CMemoryBitmap* pBitmap, const CPixelTransform& PixTransform, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();

	const int lWidth = pBitmap->Width();
	const int lHeight = pBitmap->Height();

	std::shared_ptr<CMemoryBitmap> pOutBitmap{ pBitmap->Clone(true) };
	pOutBitmap->Init(lWidth, lHeight);

	PIXELDISPATCHVECTOR vPixels(16, PIXELDISPATCHVECTOR::value_type{});

	if (pProgress != nullptr)
	{
		const QString text(QCoreApplication::translate("ChannelAlign", "Aligning Channel", "IDS_ALIGNINGCHANNEL"));
		pProgress->Start2(text, lHeight);
	}

#pragma omp parallel for default(shared) firstprivate(vPixels) if(CMultitask::GetNrProcessors(false) > 1)
	for (int j = 0; j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++)
		{
			double fGray;
			const QPointF ptOut = PixTransform.transform(QPointF(i, j));

			pBitmap->GetPixel(i, j, fGray);
			if (fGray != 0 && DSSRect{ 0, 0, lWidth, lHeight }.contains(ptOut))
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
		if (pProgress != nullptr && omp_get_thread_num() == 0)
			pProgress->Progress2(j+1);
	}

	if (pProgress != nullptr)
		pProgress->End2();

	return pOutBitmap;
}

// static
bool CChannelAlign::AlignChannels(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();

	if (pBitmap->IsMonochrome())
		return true;

	if (std::shared_ptr<CColorBitmap> pColorBitmap = std::dynamic_pointer_cast<CColorBitmap>(pBitmap))
	{
		CMemoryBitmap* const pRed	= pColorBitmap->GetRed();
		CMemoryBitmap* const pGreen	= pColorBitmap->GetGreen();
		CMemoryBitmap* const pBlue	= pColorBitmap->GetBlue();

		if (pProgress)
		{
			// We will advance the progress1 bar for each channel (3 of them)
			pProgress->Start1(3, false);
			pProgress->Progress1(0);
		}

		// Register each channels
		CLightFrameInfo lfiRed;
		CLightFrameInfo lfiGreen;
		CLightFrameInfo lfiBlue;

		lfiRed.SetProgress(pProgress);
		lfiGreen.SetProgress(pProgress);
		lfiBlue.SetProgress(pProgress);

		lfiRed.RegisterPicture(pRed, -1); // -1 means, we do NOT register a series of frames.
		if (pProgress)
			pProgress->Progress1(1);
		lfiGreen.RegisterPicture(pGreen, -1);
		if (pProgress)
			pProgress->Progress1(2);
		lfiBlue.RegisterPicture(pBlue, -1);
		if (pProgress)
			pProgress->Progress1(3);

		// Get the best one to align the others
		CLightFrameInfo* pReference;
		CLightFrameInfo* pSecond;
		CLightFrameInfo* pThird;

//		CMemoryBitmap* pReferenceBitmap;
		CMemoryBitmap* pSecondBitmap;
		CMemoryBitmap* pThirdBitmap;

		const double fMaxScore = std::max(lfiRed.quality, std::max(lfiGreen.quality, lfiBlue.quality));
		if (fMaxScore == lfiRed.quality)
		{
			pReference	= &lfiRed;
			pSecond		= &lfiGreen;
			pThird		= &lfiBlue;
//			pReferenceBitmap = pRed;
			pSecondBitmap	 = pGreen;
			pThirdBitmap	 = pBlue;
		}
		else if (fMaxScore == lfiGreen.quality)
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
		CMatchingStars MatchingStars{ pBitmap->Width(), pBitmap->Height() };
		constexpr int MaxNumberOfConsideredStars = 100;

		const auto addRefOrTargetStar = [&MatchingStars, MaxNumberOfConsideredStars]<bool Refstar>(std::span<CStar> stars)
		{
			std::ranges::sort(stars, CompareStarLuminancy);
			for (const auto& star : std::views::take(stars, MaxNumberOfConsideredStars)) // Is safe, even if 'stars' has less than 100 elements.
			{
				if constexpr (Refstar)
					MatchingStars.AddReferenceStar(star.m_fX, star.m_fY);
				else
					MatchingStars.AddTargetedStar(star.m_fX, star.m_fY);
			}
		};

		addRefOrTargetStar.operator()<true>(pReference->m_vStars);
		addRefOrTargetStar.operator()<false>(pSecond->m_vStars);

//		{
//			STARVECTOR& vStarsOrg = pReference->m_vStars;
//
//			std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);
//
//			for (size_t i = 0; i < min(vStarsOrg.size(), static_cast<STARVECTOR::size_type>(100)); i++)
//				MatchingStars.AddReferenceStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);
//		}
//
//		{
//			STARVECTOR& vStarsOrg = pSecond->m_vStars;
//
//			std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);
//
//			for (size_t i = 0; i < min(vStarsOrg.size(), static_cast<STARVECTOR::size_type>(100)); i++)
//				MatchingStars.AddTargetedStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);
//		}

		bool bTransformationsOk = MatchingStars.ComputeCoordinateTransformation(pSecond->m_BilinearParameters);

		if (bTransformationsOk)
		{
			MatchingStars.ClearTarget();
			addRefOrTargetStar.operator()<false>(pThird->m_vStars);
//			STARVECTOR& vStarsOrg = pThird->m_vStars;
//
//			std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);
//
//			MatchingStars.ClearTarget();
//
//			for (size_t i = 0; i < min(vStarsOrg.size(), static_cast<STARVECTOR::size_type>(100)); i++)
//				MatchingStars.AddTargetedStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);

			bTransformationsOk = MatchingStars.ComputeCoordinateTransformation(pThird->m_BilinearParameters);
		}

		// Align the channels
		if (bTransformationsOk)
		{
			if (pProgress != nullptr)
			{
				// Advance the progress1 bar for each alignment step (2 of them)
				pProgress->Start1(2, false);
				pProgress->Progress1(0);
			}

			CPixelTransform pixTransform;
			pixTransform.m_BilinearParameters = pSecond->m_BilinearParameters;
			std::shared_ptr<CMemoryBitmap> pOutSecondBitmap = AlignChannel(pSecondBitmap, pixTransform, pProgress);

			if (pProgress != nullptr)
				pProgress->Progress1(1);

			pixTransform.m_BilinearParameters = pThird->m_BilinearParameters;
			std::shared_ptr<CMemoryBitmap> pOutThirdBitmap = AlignChannel(pThirdBitmap, pixTransform, pProgress);

			if (pProgress != nullptr)
				pProgress->Progress1(2);

			// Dump the resulting modified channels in the image
			CopyBitmap(pOutSecondBitmap, pSecondBitmap);
			CopyBitmap(pOutThirdBitmap, pThirdBitmap);

			return true;
		}
	}

	return false;
}
