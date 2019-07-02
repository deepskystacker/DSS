#include <stdafx.h>
#include "ChannelAlign.h"
#include "RegisterEngine.h"
#include "MatchingStars.h"

/* ------------------------------------------------------------------- */

void	CChannelAlign::CopyBitmap(CMemoryBitmap * pSrcBitmap, CMemoryBitmap * pTgtBitmap)
{
	ZFUNCTRACE_RUNTIME();
	PixelIterator				itSrc;
	PixelIterator				itTgt;
	LONG						lHeight = pSrcBitmap->Height(),
								lWidth = pSrcBitmap->Width();

	pSrcBitmap->GetIterator(&itSrc);
	pTgtBitmap->GetIterator(&itTgt);

	for (int j = 0; j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++)
		{
			double				fGray;

			itSrc->GetPixel(fGray);
			fGray = min(fGray, 255.0);
			itTgt->SetPixel(fGray);

			(*itSrc)++;
			(*itTgt)++;
		};
	};
};

/* ------------------------------------------------------------------- */

BOOL	CChannelAlign::AlignChannel(CMemoryBitmap * pBitmap, CMemoryBitmap ** ppBitmap, CPixelTransform & PixTransform, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL						bResult = FALSE;
	CSmartPtr<CMemoryBitmap>	pOutBitmap;
	CString						strText;
	LONG						lWidth = pBitmap->Width(),
								lHeight = pBitmap->Height();
	PIXELDISPATCHVECTOR			vPixels;

	vPixels.reserve(16);

	pOutBitmap.Attach(pBitmap->Clone(TRUE));
	pOutBitmap->Init(lWidth, lHeight);

	if (pProgress)
	{
		strText.LoadString(IDS_ALIGNINGCHANNEL);
		pProgress->Start2(strText, lHeight);
	};

	for (LONG j = 0;j<lHeight;j++)
	{
		for (LONG i = 0;i<lWidth;i++)
		{
			double		fGray;
			CPointExt	pt(i, j);
			CPointExt	ptOut;

			ptOut = PixTransform.Transform(pt);
			pBitmap->GetPixel(i, j, fGray);

			if (fGray && ptOut.IsInRect(0, 0, lWidth-1, lHeight-1))
			{
				vPixels.resize(0);
				ComputePixelDispatch(ptOut, 1.0, vPixels);

				for (LONG k = 0;k<vPixels.size();k++)
				{
					CPixelDispatch &		Pixel = vPixels[k];

					// For each plane adjust the values
					if (Pixel.m_lX >= 0 && Pixel.m_lX < lWidth && 
						Pixel.m_lY >= 0 && Pixel.m_lY < lHeight)
					{
						double		fPreviousGray;

						pOutBitmap->GetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousGray);
						fPreviousGray   += (double)fGray * Pixel.m_fPercentage;
						pOutBitmap->SetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousGray);
					};
				};
			};
		};
		if (pProgress)
			pProgress->Progress2(NULL, j+1);
	};

	if (pProgress)
		pProgress->End2();

	pOutBitmap.CopyTo(ppBitmap);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CChannelAlign::AlignChannels(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (!pBitmap->IsMonochrome())
	{
		CColorBitmap *		pColorBitmap = dynamic_cast<CColorBitmap*>(pBitmap);

		if (pColorBitmap)
		{
			CMemoryBitmap *		pRed;
			CMemoryBitmap *		pGreen;
			CMemoryBitmap *		pBlue;

			pRed	= pColorBitmap->GetRed();
			pGreen	= pColorBitmap->GetGreen();
			pBlue	= pColorBitmap->GetBlue();

			if (pProgress)
			{
				// We will advance the progress1 bar for each channel (3 of them)
				pProgress->Start(NULL, 3, FALSE);
				pProgress->Progress1(NULL, 0);
			}

			// Register each channels
			CLightFrameInfo		lfiRed;
			CLightFrameInfo		lfiGreen;
			CLightFrameInfo		lfiBlue;

			lfiRed.SetProgress(pProgress);
			lfiGreen.SetProgress(pProgress);
			lfiBlue.SetProgress(pProgress);

			lfiRed.RegisterPicture(pRed);
			if (pProgress)
				pProgress->Progress1(NULL, 1);
			lfiGreen.RegisterPicture(pGreen);
			if (pProgress)
				pProgress->Progress1(NULL, 2);
			lfiBlue.RegisterPicture(pBlue);
			if (pProgress)
				pProgress->Progress1(NULL, 3);

			// Get the best one to align the others
			double				fMaxScore;
			CLightFrameInfo *	pReference;
			CLightFrameInfo *	pSecond;
			CLightFrameInfo *	pThird;

			CMemoryBitmap *		pReferenceBitmap;
			CMemoryBitmap *		pSecondBitmap;
			CMemoryBitmap *		pThirdBitmap;

			fMaxScore = max(lfiRed.m_fOverallQuality, max(lfiGreen.m_fOverallQuality, lfiBlue.m_fOverallQuality));
			if (fMaxScore == lfiRed.m_fOverallQuality)
			{
				pReference	= &lfiRed;
				pSecond		= &lfiGreen;
				pThird		= &lfiBlue;
				pReferenceBitmap = pRed;
				pSecondBitmap	 = pGreen;
				pThirdBitmap	 = pBlue;
			}
			else if (fMaxScore == lfiGreen.m_fOverallQuality)
			{
				pReference	= &lfiGreen;
				pSecond		= &lfiRed;
				pThird		= &lfiBlue;
				pReferenceBitmap = pGreen;
				pSecondBitmap	 = pRed;
				pThirdBitmap	 = pBlue;
			}
			else
			{
				pReference	= &lfiBlue;
				pSecond		= &lfiRed;
				pThird		= &lfiGreen;
				pReferenceBitmap = pBlue;
				pSecondBitmap	 = pRed;
				pThirdBitmap	 = pGreen;
			};

			// Compute the transformations
			CMatchingStars		MatchingStars;
			BOOL				bTransformationsOk;

			MatchingStars.SetSizes(pBitmap->Width(), pBitmap->Height());
			{
				STARVECTOR &		vStarsOrg = pReference->m_vStars;

				std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);

				for (LONG i = 0;i<min(vStarsOrg.size(), 100);i++)
					MatchingStars.AddReferenceStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);
			};

			{
				STARVECTOR &		vStarsOrg = pSecond->m_vStars;

				std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);

				for (LONG i = 0;i<min(vStarsOrg.size(), 100);i++)
					MatchingStars.AddTargetedStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);
			};

			bTransformationsOk = MatchingStars.ComputeCoordinateTransformation(pSecond->m_BilinearParameters);

			if (bTransformationsOk)
			{
				STARVECTOR &		vStarsOrg = pThird->m_vStars;

				std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);

				MatchingStars.ClearTarget();

				for (LONG i = 0;i<min(vStarsOrg.size(), 100);i++)
					MatchingStars.AddTargetedStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);

				bTransformationsOk = MatchingStars.ComputeCoordinateTransformation(pThird->m_BilinearParameters);
			};

			// Align the channels
			CSmartPtr<CMemoryBitmap>	pOutSecondBitmap;
			CSmartPtr<CMemoryBitmap>	pOutThirdBitmap;

			if (bTransformationsOk)
			{
				bResult = TRUE;
				CPixelTransform				PixTransform;

				if (pProgress)
				{
					// Advance the progress1 bar for each alignment step (2 of them)
					pProgress->Start(NULL, 2, FALSE);
					pProgress->Progress1(NULL, 0);
				}

				PixTransform.m_BilinearParameters = pSecond->m_BilinearParameters;
				AlignChannel(pSecondBitmap, &pOutSecondBitmap, PixTransform, pProgress);
				if (pProgress)
					pProgress->Progress1(NULL, 1);

				PixTransform.m_BilinearParameters = pThird->m_BilinearParameters;
				AlignChannel(pThirdBitmap, &pOutThirdBitmap, PixTransform, pProgress);
				if (pProgress)
					pProgress->Progress1(NULL, 2);
			};

			// Dump the resulting modified channels in the image
			if (bResult)
			{
				CopyBitmap(pOutSecondBitmap, pSecondBitmap);
				CopyBitmap(pOutThirdBitmap, pThirdBitmap);
			};
		};
	}
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */
