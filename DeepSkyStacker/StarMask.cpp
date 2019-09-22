#include <stdafx.h>
#include "StarMask.h"
#include "RegisterEngine.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <GdiPlus.h>
using namespace Gdiplus;

/* ------------------------------------------------------------------- */

BOOL CStarMaskEngine::CreateStarMask2(CMemoryBitmap * pBitmap, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	BOOL				bResult = FALSE;
	CLightFrameInfo		LightFrame;
	STARVECTOR			vStars;

	LightFrame.SetDetectionThreshold(m_fMinLuminancy);
	LightFrame.SetHotPixelRemoval(m_bRemoveHotPixels);
	LightFrame.SetRoundnessTolerance(2.0);

	LightFrame.SetProgress(pProgress);
	LightFrame.RegisterPicture(pBitmap);

	LightFrame.GetStars(vStars);

	{
		// Draw the stars
		CSmartPtr<C16BitGrayBitmap>			pOutBitmap;
		CStarMaskFunction *					pStarMaskFunction;

		GetShapeFunction(&pStarMaskFunction);

		pOutBitmap.Attach(new C16BitGrayBitmap());
		if (pOutBitmap)
		{
			pOutBitmap->Init(pBitmap->Width(), pBitmap->Height());
			double				fWidth	= pBitmap->Width();
			double				fHeight = pBitmap->Height();

			if (pProgress)
			{
				CString			strText;

				strText.LoadString(IDS_CREATINGSTARMASK);
				pProgress->Start2(strText, (LONG)vStars.size());
			};

			for (LONG k = 0;k<vStars.size();k++)
			{
				double			fRadius = vStars[k].m_fMeanRadius*2.35/1.5;
				double &		fXCenter = vStars[k].m_fX;
				double &		fYCenter = vStars[k].m_fY;

				if (2*fRadius>=m_fMinSize && 2*fRadius<=m_fMaxSize)
				{
					double		fFactor1 = 1.0/exp(-0.5);
					double		fFactor2 = 2*fRadius*fRadius;

					fRadius *= m_fPercentIncrease;
					if (m_fPixelIncrease)
						fRadius += m_fPixelIncrease;

					pStarMaskFunction->SetRadius(fRadius);
					for (double i = max(0.0, fXCenter - 3*fRadius);i<=min(fXCenter + 3*fRadius, fWidth-1);i++)
					{
						for (double j = max(0.0, fYCenter - 3*fRadius);j<=min(fYCenter + 3*fRadius, fHeight-1);j++)
						{
							// Compute the distance to the center
							double		fDistance;
							double		fXDistance = fabs(i-fXCenter);
							double		fYDistance = fabs(j-fYCenter);
							double		fPixelValue;
							double		fOldPixelValue;

							fDistance = sqrt(fXDistance * fXDistance + fYDistance * fYDistance);

							fPixelValue = pStarMaskFunction->Compute(fDistance);

							pOutBitmap->GetPixel(i+0.5, j+0.5, fOldPixelValue);
							fPixelValue = max(fOldPixelValue, max(0.0, min(fPixelValue*255.0, 255.0)));
							pOutBitmap->SetPixel(i+0.5, j+0.5, fPixelValue);
						};
					};
				};

				if (pProgress)
					pProgress->Progress2(NULL, k+1);
			};

			if (pProgress)
				pProgress->End2();
			C16BitGrayBitmap *	p16Bitmap;

			pOutBitmap.CopyTo(&p16Bitmap);
			*ppBitmap = dynamic_cast<CMemoryBitmap *>(p16Bitmap);

			if (pStarMaskFunction)
				delete pStarMaskFunction;
			bResult = TRUE;
		};
	}

	return bResult;
};

/* ------------------------------------------------------------------- */

