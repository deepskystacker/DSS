#include <stdafx.h>
#include "FlatFrame.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "Multitask.h"

#include <omp.h>

class CFlatDivideTask : public CMultitask
{
private :
	CSmartPtr<CMemoryBitmap>	m_pTarget;
	CDSSProgress *				m_pProgress;
	CFlatFrame *				m_pFlatFrame;
	BOOL						m_bUseGray;
	BOOL						m_bUseCFA;

public :
    CFlatDivideTask()
    {
        m_pProgress = nullptr;
        m_pFlatFrame = nullptr;
        m_bUseGray = false;
        m_bUseCFA = false;
    }

	virtual ~CFlatDivideTask()
	{
	};

	void	Init(CMemoryBitmap * pTarget, CDSSProgress * pProgress, CFlatFrame * pFlatFrame)
	{
		m_pTarget		= pTarget;
		m_pProgress		= pProgress;
		m_pFlatFrame	= pFlatFrame;

		m_bUseGray = m_pFlatFrame->m_FlatNormalization.UseGray();
		m_bUseCFA  = m_pFlatFrame->IsCFA();
		if (m_pProgress)
			m_pProgress->Start2(nullptr, m_pTarget->RealWidth());

	};

	void	End()
	{
		if (m_pProgress)
			m_pProgress->End2();
	};

	virtual BOOL	Process();
	virtual BOOL	DoTask(HANDLE hEvent);
};

/* ------------------------------------------------------------------- */

BOOL	CFlatDivideTask::DoTask(HANDLE hEvent)
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;

	LONG			i, j;
	BOOL			bEnd = FALSE;
	MSG				msg;
	LONG			lWidth = m_pTarget->RealWidth();

	// Create a message queue and signal the event
	PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
	SetEvent(hEvent);
	while (!bEnd && GetMessage(&msg, nullptr, 0, 0))
	{
		if (msg.message == WM_MT_PROCESS)
		{
			for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
			{
				for (i = 0;i<lWidth;i++)
				{
					if (m_bUseGray)
					{
						double			fSrcGray;
						double			fTgtGray;

						m_pTarget->GetPixel(i, j, fTgtGray);
						m_pFlatFrame->m_pFlatFrame->GetPixel(i, j, fSrcGray);
						if (m_bUseCFA)
							m_pFlatFrame->m_FlatNormalization.Normalize(fTgtGray, fSrcGray, m_pFlatFrame->m_pFlatFrame->GetBayerColor(i, j));
						else
							m_pFlatFrame->m_FlatNormalization.Normalize(fTgtGray, fSrcGray);
						m_pTarget->SetPixel(i, j, fTgtGray);
					}
					else
					{
						double			fSrcRed, fSrcGreen, fSrcBlue;
						double			fTgtRed, fTgtGreen, fTgtBlue;

						m_pTarget->GetPixel(i, j, fTgtRed, fTgtGreen, fTgtBlue);
						m_pFlatFrame->m_pFlatFrame->GetPixel(i, j, fSrcRed, fSrcGreen, fSrcBlue);
						m_pFlatFrame->m_FlatNormalization.Normalize(fTgtRed, fTgtGreen, fTgtBlue, fSrcRed, fSrcGreen, fSrcBlue);
						m_pTarget->SetPixel(i, j, fTgtRed, fTgtGreen, fTgtBlue);
					};
				};
			};
			SetEvent(hEvent);
		}
		else if (msg.message == WM_MT_STOP)
			bEnd = TRUE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL	CFlatDivideTask::Process()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;
	LONG			lHeight = m_pTarget->RealHeight();
	LONG			lStep;
	LONG			lRemaining;
	LONG			i = 0;

	if (m_pProgress)
	{
		m_pProgress->Start2(nullptr, lHeight);
		m_pProgress->SetNrUsedProcessors(GetNrThreads());
	};

	lStep		= max(1L, lHeight/50);
	lRemaining	= lHeight;

	bResult = TRUE;
	while (i<lHeight)
	{
		LONG			lAdd = min(lStep, lRemaining);
		DWORD			dwThreadId;

		dwThreadId = GetAvailableThreadId();
		PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

		i			+= lAdd;
		lRemaining	-= lAdd;

		if (m_pProgress)
			m_pProgress->Progress2(nullptr, i);
	};

	CloseAllThreads();
	if (m_pProgress)
	{
		m_pProgress->SetNrUsedProcessors();
		m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL CFlatFrame::ApplyFlat(CMemoryBitmap * pTarget, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	BOOL			bUseGray;
	BOOL			bUseCFA;
	CStringA strText;
	strText.LoadString(IDS_APPLYINGFLAT);

	// Check and remove super pixel settings
	CCFABitmapInfo *			pCFABitmapInfo;
	CFATRANSFORMATION			CFATransform = CFAT_NONE;

	pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pTarget);
	if (pCFABitmapInfo)
	{
		CFATransform = pCFABitmapInfo->GetCFATransformation();
		if (CFATransform == CFAT_SUPERPIXEL)
			pCFABitmapInfo->UseBilinear(TRUE);
	};

	// Check that it is the same sizes
	if (pTarget && m_pFlatFrame)
	{
		if ((pTarget->RealWidth() == m_pFlatFrame->RealWidth()) &&
			(pTarget->RealHeight() == m_pFlatFrame->RealHeight()))
		{
			ZTRACE_RUNTIME(strText);
			/*
			CFlatDivideTask			DivideTask;

			DivideTask.Init(pTarget, pProgress, this);
			DivideTask.StartThreads();
			DivideTask.Process();
			*/

			bUseGray = m_FlatNormalization.UseGray();
			bUseCFA     = IsCFA();
			
			if (pProgress)
				pProgress->Start2(nullptr, pTarget->RealWidth());
			bResult = TRUE;

			int	rowProgress = 0;

#if defined(_OPENMP)
#pragma omp parallel for default(none)
#endif
			for (LONG i = 0;i<pTarget->RealWidth();i++)
			{
				for (LONG j = 0;j<pTarget->RealHeight();j++)
				{
					if (bUseGray)
					{
						double			fSrcGray = 0.0;
						double			fTgtGray = 0.0;


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
						double			fSrcRed, fSrcGreen, fSrcBlue;
						double			fTgtRed, fTgtGreen, fTgtBlue;

						pTarget->GetPixel(i, j, fTgtRed, fTgtGreen, fTgtBlue);
						m_pFlatFrame->GetPixel(i, j, fSrcRed, fSrcGreen, fSrcBlue);
						m_FlatNormalization.Normalize(fTgtRed, fTgtGreen, fTgtBlue, fSrcRed, fSrcGreen, fSrcBlue);
						pTarget->SetPixel(i, j, fTgtRed, fTgtGreen, fTgtBlue);
					};
				};

#if defined (_OPENMP)
				if (pProgress && 0 == omp_get_thread_num())	// Are we on the master thread?
				{
					rowProgress += omp_get_num_threads();
					pProgress->Progress2(nullptr, rowProgress);
				}
#else
				if (pProgress)
					pProgress->Progress2(nullptr, ++rowProgress);
#endif
			};
			if (pProgress)
				pProgress->End2();
		}
		else
		{
			ZTRACE_RUNTIME("Target.RealWidth = %d, Source.RealWidth = %d", pTarget->RealWidth(), m_pFlatFrame->RealWidth());
			ZTRACE_RUNTIME("Target.RealHeight = %d, Source.RealHeight = %d", pTarget->RealHeight(), m_pFlatFrame->RealHeight());
			ZTRACE_RUNTIME("Did not perform %s", strText);
		}
	};

	if (CFATransform == CFAT_SUPERPIXEL)
		pCFABitmapInfo->UseSuperPixels(TRUE);

	return bResult;
};

/* ------------------------------------------------------------------- */

void CFlatFrame::ComputeFlatNormalization(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	if (IsOk() && !m_bComputed)
	{
		ZTRACE_RUNTIME("Compute Flat normalization");
		CString			strStart2;
		CString			strText;

		if (pProgress)
		{
			pProgress->GetStart2Text(strStart2);
			strText.LoadString(IDS_NORMALIZINGMASTERFLAT);
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
		LONG			lNrGrays	= 0,
						lNrReds		= 0,
						lNrGreens	= 0,
						lNrBlues	= 0,
						lNrYellows	= 0,
						lNrMagentas	= 0,
						lNrCyans	= 0,
						lNrGreen2s	= 0;
		BOOL			bCFA;

		bCFA = ::IsCFA(m_pFlatFrame);
		for (LONG j = 0;j<m_pFlatFrame->RealHeight();j++)
		{
			for (LONG i = 0;i<m_pFlatFrame->RealWidth();i++)
			{
				double			fGray,
								fRed,
								fGreen,
								fBlue;

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
				pProgress->Progress2(nullptr, j+1);
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
		m_bComputed = TRUE;
	};
};
