#include "stdafx.h"
#include "FlatFrame.h"
#include "Multitask.h"
#include "DSSProgress.h"
#include "MemoryBitmap.h"
#include "CFABitmapInfo.h"
#include "ztrace.h"
//#include "resource.h"

using namespace DSS;

bool CFlatFrame::IsOk() const
{
	return static_cast<bool>(m_pFlatFrame) && m_pFlatFrame->IsOk();
}

bool CFlatFrame::IsCFA() const
{
	if(m_pFlatFrame.get())
		return m_pFlatFrame->IsCFA();
	return false;
}

void CFlatFrame::Clear()
{
	m_bComputed = false;
	m_pFlatFrame.reset();
}

bool CFlatFrame::ApplyFlat(std::shared_ptr<CMemoryBitmap> pTarget, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;
	const QString strText(QCoreApplication::translate("FlatFrame", "Applying Flat Frame", "IDS_APPLYINGFLAT"));

	// Check and remove super pixel settings
	CFATRANSFORMATION CFATransform = CFAT_NONE;
	CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(pTarget.get());
	if (pCFABitmapInfo != nullptr)
	{
		CFATransform = pCFABitmapInfo->GetCFATransformation();
		if (CFATransform == CFAT_SUPERPIXEL)
			pCFABitmapInfo->UseBilinear(true);
	}

	// Check that it is the same sizes
	if (static_cast<bool>(pTarget) && static_cast<bool>(m_pFlatFrame))
	{
		const int height = pTarget->RealHeight();
		const int width = pTarget->RealWidth();
		const int nrProcessors = CMultitask::GetNrProcessors();

		if (width == m_pFlatFrame->RealWidth() && height == m_pFlatFrame->RealHeight())
		{
			ZTRACE_RUNTIME(strText);
			const bool bUseGray = m_FlatNormalization.UseGray();
			const bool bUseCFA = IsCFA();
			
			if (pProgress != nullptr)
				pProgress->Start2(height);
			bResult = true;

			int	rowProgress = 0;

#pragma omp parallel for schedule(static, 100) default(shared) if(nrProcessors > 1)
			for (int j = 0; j < height; j++)
			{
				for (int i = 0; i < width; i++)
				{
					if (bUseGray)
					{
						double fSrcGray = 0.0;
						double fTgtGray = 0.0;
						pTarget->GetPixel(i, j, fTgtGray);
						m_pFlatFrame->GetPixel(i, j, fSrcGray);

						if (bUseCFA)
							m_FlatNormalization.Normalize(fTgtGray, fSrcGray, m_pFlatFrame->GetBayerColor(i, j));
						else
							m_FlatNormalization.Normalize(fTgtGray, fSrcGray);

						pTarget->SetPixel(i, j, fTgtGray);
					}
					else
					{
						double fSrcRed, fSrcGreen, fSrcBlue;
						double fTgtRed, fTgtGreen, fTgtBlue;

						pTarget->GetPixel(i, j, fTgtRed, fTgtGreen, fTgtBlue);
						m_pFlatFrame->GetPixel(i, j, fSrcRed, fSrcGreen, fSrcBlue);
						m_FlatNormalization.Normalize(fTgtRed, fTgtGreen, fTgtBlue, fSrcRed, fSrcGreen, fSrcBlue);
						pTarget->SetPixel(i, j, fTgtRed, fTgtGreen, fTgtBlue);
					}
				}

				if (omp_get_thread_num() == 0 && pProgress != nullptr)
					pProgress->Progress2(rowProgress += nrProcessors);
			}

			if (pProgress != nullptr)
				pProgress->End2();
		}
		else
		{
			ZTRACE_RUNTIME("Target.RealWidth = %d, Source.RealWidth = %d", pTarget->RealWidth(), m_pFlatFrame->RealWidth());
			ZTRACE_RUNTIME("Target.RealHeight = %d, Source.RealHeight = %d", pTarget->RealHeight(), m_pFlatFrame->RealHeight());
			ZTRACE_RUNTIME("Did not perform %s", strText.toUtf8().constData());
		}
	}

	if (CFATransform == CFAT_SUPERPIXEL)
		pCFABitmapInfo->UseSuperPixels(true);

	return bResult;
}

/* ------------------------------------------------------------------- */

void CFlatFrame::ComputeFlatNormalization(ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	if (IsOk() && !m_bComputed)
	{
		ZTRACE_RUNTIME("Compute Flat normalization");
		QString	strStart2;

		if (pProgress)
		{
			strStart2 = pProgress->GetStart2Text();
			const QString strText(QCoreApplication::translate("FlatFrame", "Calibrating Master Flat", "IDS_NORMALIZINGMASTERFLAT"));
			pProgress->Start2(strText, m_pFlatFrame->RealHeight());
		};

		// Color compute flat is not monochrome and not CFA
		double			fMeanGray	= 0,
						fMeanRed	= 0,
						fMeanGreen	= 0,
						fMeanBlue	= 0,
						fMeanYellow = 0,
						fMeanCyan   = 0,
						fMeanMagenta= 0,
						fMeanGreen2 = 0;
		int			lNrGrays	= 0,
						lNrReds		= 0,
						lNrGreens	= 0,
						lNrBlues	= 0,
						lNrYellows	= 0,
						lNrMagentas	= 0,
						lNrCyans	= 0,
						lNrGreen2s	= 0;

		const bool bCFA = this->IsCFA();

		for (int j = 0; j < m_pFlatFrame->RealHeight(); j++)
		{
			for (int i = 0; i < m_pFlatFrame->RealWidth(); i++)
			{
				double fGray, fRed, fGreen, fBlue;

				if (m_pFlatFrame->IsMonochrome())
				{
					if (bCFA)
					{
						m_pFlatFrame->GetPixel(i, j, fGray);
						switch (m_pFlatFrame->GetBayerColor(i, j))
						{
						case BAYER_RED :
							fMeanRed   = (fMeanRed*lNrReds+fGray)/(lNrReds+1);
							lNrReds++;
							break;
						case BAYER_GREEN :
							fMeanGreen = (fMeanGreen*lNrGreens+fGray)/(lNrGreens+1);
							lNrGreens++;
							break;
						case BAYER_BLUE :
							fMeanBlue  = (fMeanBlue*lNrBlues+fGray)/(lNrBlues+1);
							lNrBlues++;
							break;
						case BAYER_CYAN :
							fMeanCyan = (fMeanCyan*lNrCyans+fGray)/(lNrCyans+1);
							lNrCyans++;
							break;
						case BAYER_YELLOW :
							fMeanYellow = (fMeanYellow*lNrYellows+fGray)/(lNrYellows+1);
							lNrYellows++;
							break;
						case BAYER_MAGENTA :
							fMeanMagenta = (fMeanMagenta*lNrMagentas+fGray)/(lNrMagentas+1);
							lNrMagentas++;
							break;
						case BAYER_GREEN2 :
							fMeanGreen2 = (fMeanGreen2*lNrGreen2s+fGray)/(lNrGreen2s+1);
							lNrGreen2s++;
							break;
						};
					}
					else
					{
						m_pFlatFrame->GetPixel(i, j, fGray);
						fMeanGray   = (fMeanGray*lNrGrays+fGray)/(lNrGrays+1);
						lNrGrays++;
					};
				}
				else
				{
					m_pFlatFrame->GetPixel(i, j, fRed, fGreen, fBlue);

					fMeanRed   = (fMeanRed*lNrReds+fRed)/(lNrReds+1);
					fMeanGreen = (fMeanGreen*lNrGreens+fGreen)/(lNrGreens+1);
					fMeanBlue  = (fMeanBlue*lNrBlues+fBlue)/(lNrBlues+1);
					lNrReds++;
					lNrGreens++;
					lNrBlues++;
				};
			};

			if (pProgress)
				pProgress->Progress2(j+1);
		};

		if (m_pFlatFrame->IsMonochrome() && !bCFA)
		{
			m_FlatNormalization.SetParameters(fMeanGray);
			ZTRACE_RUNTIME("Flat normalization: Mean = %.2f", fMeanGray/256.0);
		}
		else
		{
			if (lNrYellows)
			{
				m_FlatNormalization.SetParameters(fMeanCyan, fMeanMagenta, fMeanYellow, fMeanGreen2);
				ZTRACE_RUNTIME("Flat normalization: Mean Cyan = %.2f - Magenta = %.2f - Yellow = %.2f - Green = %.2f", fMeanCyan/256.0, fMeanMagenta/256.0, fMeanYellow/256.0, fMeanGreen2/256.0);
			}
			else
			{
				m_FlatNormalization.SetParameters(fMeanRed, fMeanGreen, fMeanBlue);
				ZTRACE_RUNTIME("Flat normalization: Mean Red = %.2f - Green = %.2f - Blue = %.2f", fMeanRed/256.0, fMeanGreen/256.0, fMeanBlue/256.0);
			};
			if (bCFA) // Call bCFA after to force UseGray
				m_FlatNormalization.SetParameters(1.0);
		};

		if (pProgress)
		{
			pProgress->End2();
			pProgress->Start2(strStart2, 0);
		};
		m_bComputed = true;
	};
};
