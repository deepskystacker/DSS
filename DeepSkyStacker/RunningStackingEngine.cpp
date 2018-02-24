#include <stdafx.h>
#include "RunningStackingEngine.h"
#include "Registry.h"
#include "MatchingStars.h"
#include "PixelTransform.h"
#include <math.h>

#define _USE_MATH_DEFINES
#include <cmath>

/* ------------------------------------------------------------------- */

CRunningStackingEngine::CRunningStackingEngine()
{
	m_lNrStacked = 0;
	m_fTotalExposure = 0;
};

/* ------------------------------------------------------------------- */

CRunningStackingEngine::~CRunningStackingEngine()
{
};

/* ------------------------------------------------------------------- */

void	CRunningStackingEngine::CreatePublicBitmap()
{
	BOOL					bMonochrome;
	LONG					lWidth,
							lHeight;

	if (m_pStackedBitmap && m_lNrStacked)
	{
		bMonochrome = m_pStackedBitmap->IsMonochrome();
		lWidth		= m_pStackedBitmap->Width();
		lHeight		= m_pStackedBitmap->Height();

		if (!m_pPublicBitmap)
		{
			if (bMonochrome)
				m_pPublicBitmap.Attach(new C16BitGrayBitmap);
			else
				m_pPublicBitmap.Attach(new C48BitColorBitmap);

			m_pPublicBitmap->Init(lWidth, lHeight);
		};

		PixelIterator			itIn;
		PixelIterator			itOut;

		m_pPublicBitmap->GetIterator(&itOut);
		m_pStackedBitmap->GetIterator(&itIn);

		for (LONG j = 0;j<lHeight;j++)
			for (LONG i = 0;i<lWidth;i++)
			{
				if (bMonochrome)
				{
					double			fGray;

					itIn->GetPixel(fGray);
					fGray /= m_lNrStacked;
					itOut->SetPixel(min(fGray, 255.0));
				}
				else
				{
					double			fRed, fGreen, fBlue;

					itIn->GetPixel(fRed, fGreen, fBlue);
					fRed	/= m_lNrStacked;
					fGreen	/= m_lNrStacked;
					fBlue	/= m_lNrStacked;
					itOut->SetPixel(min(fRed, 255.0), min(fGreen, 255.0), min(fBlue, 255.0));
				};
				(*itIn)++;
				(*itOut)++;
			};
	};
};

/* ------------------------------------------------------------------- */

BOOL	CRunningStackingEngine::AddImage(CLightFrameInfo & lfi, CDSSProgress * pProgress)
{
	BOOL				bResult = FALSE;
	LONG				lWidth,
						lHeight;
	BOOL				bColor;

	// First load the input bitmap
	CSmartPtr<CMemoryBitmap>		pBitmap;
	if (::LoadFrame(lfi.m_strFileName,PICTURETYPE_LIGHTFRAME, pProgress, &pBitmap))
	{
		CString			strText;

		pBitmap->RemoveHotPixels(pProgress);
		lWidth = pBitmap->Width();
		lHeight = pBitmap->Height();
		bColor = !pBitmap->IsMonochrome() || pBitmap->IsCFA();

		// Create the output bitmap
		if (!m_lNrStacked)
		{
			if (bColor)
				m_pStackedBitmap.Attach(new C96BitFloatColorBitmap);
			else
				m_pStackedBitmap.Attach(new C32BitFloatGrayBitmap);
			m_pStackedBitmap->Init(lWidth, lHeight);
		};

		if (m_BackgroundCalibration.m_BackgroundCalibrationMode != BCM_NONE)
		{
			if (pProgress)
			{
				strText.LoadString(IDS_COMPUTINGBACKGROUNDCALIBRATION);
				pProgress->Start2(strText, 0);
			};
			m_BackgroundCalibration.ComputeBackgroundCalibration(pBitmap, !m_lNrStacked, pProgress);
		};

		// Stack it (average)
		CPixelTransform		PixTransform(lfi.m_BilinearParameters);
		CString				strDescription;
		PIXELDISPATCHVECTOR	vPixels;

		vPixels.reserve(16);

		strDescription = lfi.m_strInfos;
		if (lfi.m_lNrChannels==3)
			strText.Format(IDS_STACKRGBLIGHT, lfi.m_lBitPerChannels, (LPCTSTR)strDescription, (LPCTSTR)lfi.m_strFileName);
		else
			strText.Format(IDS_STACKGRAYLIGHT, lfi.m_lBitPerChannels, (LPCTSTR)strDescription, (LPCTSTR)lfi.m_strFileName);

		if (pProgress)
			pProgress->Start2(strText, lHeight);
		for (LONG j = 0;j<lHeight;j++)
		{
			for (LONG i = 0;i<lWidth;i++)
			{
				double		fRed, fGreen, fBlue;
				CPointExt	pt(i, j);
				CPointExt	ptOut;

				ptOut = PixTransform.Transform(pt);
				pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

				if (m_BackgroundCalibration.m_BackgroundCalibrationMode != BCM_NONE)
					m_BackgroundCalibration.ApplyCalibration(fRed, fGreen, fBlue);

				if ((fRed || fGreen || fBlue) && ptOut.IsInRect(0, 0, lWidth-1, lHeight-1))
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
							double		fPreviousRed, 
										fPreviousGreen, 
										fPreviousBlue;

							m_pStackedBitmap->GetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
							fPreviousRed   += (double)fRed * Pixel.m_fPercentage;
							fPreviousGreen += (double)fGreen * Pixel.m_fPercentage;
							fPreviousBlue  += (double)fBlue * Pixel.m_fPercentage;
							m_pStackedBitmap->SetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
						};
					};
				};
			};
			if (pProgress)
				pProgress->Progress2(NULL, j+1);
		};

		if (pProgress)
			pProgress->End2();
		m_lNrStacked++;
		m_fTotalExposure += lfi.m_fExposure;
		bResult = TRUE;
	};	

	if (bResult && !m_MatchingStars.IsReferenceSet())
	{
		STARVECTOR &		vStarsOrg = lfi.m_vStars;

		std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);

		for (LONG i = 0;i<min(vStarsOrg.size(), 100);i++)
			m_MatchingStars.AddReferenceStar(vStarsOrg[i].m_fX, vStarsOrg[i].m_fY);
	};

	if (bResult)
		CreatePublicBitmap();

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CRunningStackingEngine::ComputeOffset(CLightFrameInfo & lfi)
{
	BOOL				bResult = FALSE;

	if (m_lNrStacked)
	{
		m_MatchingStars.ClearTarget();

		STARVECTOR &		vStarsDst = lfi.m_vStars;

		std::sort(vStarsDst.begin(), vStarsDst.end(), CompareStarLuminancy);

		for (LONG i = 0;i<min(vStarsDst.size(), 100);i++)
			m_MatchingStars.AddTargetedStar(vStarsDst[i].m_fX, vStarsDst[i].m_fY);

		m_MatchingStars.SetSizes(lfi.RenderedWidth(), lfi.RenderedHeight());
		bResult = m_MatchingStars.ComputeCoordinateTransformation(lfi.m_BilinearParameters);
	}
	else
	{
		m_MatchingStars.ClearReference();
		lfi.m_BilinearParameters.Clear();
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
