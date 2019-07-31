#include <stdafx.h>
#include "DSSTools.h"
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "CosmeticEngine.h"
#include "Multitask.h"

/* ------------------------------------------------------------------- */

class CDetectCosmeticTask : public CMultitask
{
private :
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CSmartPtr<CMemoryBitmap>	m_pMedian;
	CSmartPtr<CMemoryBitmap>	m_pDelta;
	BOOL						m_bHot;
	double						m_fThreshold;
	CDSSProgress *				m_pProgress;
	BOOL						m_bSimulate;
	CCosmeticStats				m_Stats;
	BOOL						m_bInitDelta;

private :
	double	Normalize(double fValue)
	{
		return pow(fValue/256.0, 2);
	};

	BOOL	AdjustHotPixel(double & fValue, double fMedian)
	{
		BOOL			bResult = FALSE;

		double	fPercent;// = (fValue-fMedian)/fValue/sqrt((fMedian+1)/256.0);

		fPercent = (Normalize(fValue)-Normalize(fMedian))/Normalize(fValue);
		if (fPercent>m_fThreshold)
		{
			fValue	 = fMedian;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	AdjustColdPixel(double & fValue, double fMedian)
	{
		BOOL			bResult = FALSE;

		double	fPercent;// = (fMedian-fValue)/fMedian/sqrt((fValue+1)/256.0);

		fPercent = (Normalize(fMedian)-Normalize(fValue))/Normalize(fMedian);
		if (fPercent > m_fThreshold)
		{
			fValue  = fMedian;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	AdjustPixel(double & fValue, double fMedian)
	{
		BOOL		bResult = FALSE;

		if (m_bHot)
		{
			if (fValue > fMedian)
				bResult = AdjustHotPixel(fValue, fMedian);
		}
		else
		{
			if (fValue < fMedian)
				bResult = AdjustColdPixel(fValue, fMedian);
		};

		return bResult;
	};

public :
	CDetectCosmeticTask()
	{
		m_bSimulate		= FALSE;
		m_bInitDelta	= FALSE;
	};

	virtual ~CDetectCosmeticTask()
	{
	};

	void	SetSimulate(BOOL bSimulate)
	{
		m_bSimulate = bSimulate;
	};

	void	SetInitDelta(BOOL bInitDelta)
	{
		m_bInitDelta = bInitDelta;
	};

	void	FillStats(CCosmeticStats & cs)
	{
		if (m_bHot)
			cs.m_lNrDetectedHotPixels = m_Stats.m_lNrDetectedHotPixels;
		else
			cs.m_lNrDetectedColdPixels = m_Stats.m_lNrDetectedColdPixels;
	};

	void	Init(CMemoryBitmap * pBitmap, CMemoryBitmap * pMedian, CMemoryBitmap * pDelta, BOOL bHot, double fThreshold, CDSSProgress * pProgress)
	{
		m_pBitmap		= pBitmap;
		m_pMedian		= pMedian;
		m_pDelta		= pDelta;
		m_bHot			= bHot;
		m_fThreshold	= fThreshold;
		m_pProgress		= pProgress;
	};

	virtual BOOL	DoTask(HANDLE hEvent)
	{
		ZFUNCTRACE_RUNTIME();
		BOOL					bResult = TRUE;
		LONG					i, j;
		BOOL					bEnd = FALSE;
		MSG						msg;
		LONG					lWidth  = m_pBitmap->RealWidth(),
								lHeight = m_pBitmap->RealHeight();
		BOOL					bMonochrome = m_pBitmap->IsMonochrome();
		LONG					lNrHotPixels = 0,
								lNrColdPixels = 0;

		// Create a message queue and signal the event
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		SetEvent(hEvent);
		while (!bEnd && GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_MT_PROCESS)
			{
				for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
				{
					for (i = 0;i<lWidth;i++)
					{
						BOOL				bChanged = FALSE;

						if (bMonochrome)
						{
							double			fGray,
											fMedianGray;

							m_pBitmap->GetPixel(i, j, fGray);
							m_pMedian->GetPixel(i, j, fMedianGray);

							bChanged = AdjustPixel(fGray, fMedianGray);
						}
						else
						{
							double			fRed, fGreen, fBlue,
											fMedianRed, fMedianGreen, fMedianBlue;

							m_pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
							m_pMedian->GetPixel(i, j, fMedianRed, fMedianGreen, fMedianBlue);

							bChanged = AdjustPixel(fRed, fMedianRed);
							bChanged = AdjustPixel(fGreen, fMedianGreen) || bChanged;
							bChanged = AdjustPixel(fBlue, fMedianBlue) || bChanged;
						};

						if (bChanged)
						{
							if (m_bHot)
								lNrHotPixels++;
							else
								lNrColdPixels++;
						};

						if (m_pDelta && (bChanged || m_bInitDelta))
							m_pDelta->SetPixel(i, j, bChanged ? (m_bHot ? 255 : 50) : 128);
					};
				};

				SetEvent(hEvent);
			}
			else if (msg.message == WM_MT_STOP)
				bEnd = TRUE;
		};

		m_CriticalSection.Lock();
		m_Stats.m_lNrDetectedHotPixels	+= lNrHotPixels;
		m_Stats.m_lNrDetectedColdPixels += lNrColdPixels;
		m_CriticalSection.Unlock();
		return TRUE;
	};

	virtual BOOL	Process()
	{
		ZFUNCTRACE_RUNTIME();
		BOOL				bResult = TRUE;
		LONG				lHeight = m_pBitmap->RealHeight();
		LONG				i = 0;
		LONG				lStep;
		LONG				lRemaining;

		if (m_pProgress)
			m_pProgress->SetNrUsedProcessors(GetNrThreads());
		lStep		= max(1L, lHeight/50);
		lRemaining	= lHeight;
		bResult = TRUE;
		while (i<lHeight)
		{
			LONG			lAdd = min(lStep, lRemaining);
			DWORD			dwThreadId;
			
			dwThreadId = GetAvailableThreadId();
			PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

			i			+=lAdd;
			lRemaining	-= lAdd;
			if (m_pProgress)
				m_pProgress->Progress2(NULL, i);
		};

		CloseAllThreads();

		if (m_pProgress)
			m_pProgress->SetNrUsedProcessors();

		return bResult;
	};
};

/* ------------------------------------------------------------------- */

class CCleanCosmeticTask : public CMultitask
{
private :
	CSmartPtr<CMemoryBitmap>	m_pOutBitmap;
	CSmartPtr<CMemoryBitmap>	m_pOrgBitmap;
	CSmartPtr<CMemoryBitmap>	m_pDelta;
	CDSSProgress *				m_pProgress;
	CPostCalibrationSettings    m_pcs;
	LONG						m_lWidth,
								m_lHeight;
	BOOL						m_bMonochrome;
	BOOL						m_bCFA;
	CFATYPE						m_CFAType;
	LONG						m_lColdFilterSize,
								m_lHotFilterSize;


private :
	BOOL	IsOkValue(double fDelta)
	{
		return (fDelta > 100) && (fDelta < 200);
	};

	void	ComputeMedian(LONG x, LONG y, LONG lFilterSize, double & fGray);
	void	ComputeMedian(LONG x, LONG y, LONG lFilterSize, double & fRed, double & fGreen, double & fBlue);
	void	ComputeGaussian(LONG x, LONG y, LONG lFilterSize, double & fGray);
	void	ComputeGaussian(LONG x, LONG y, LONG lFilterSize, double & fRed, double & fGreen, double & fBlue);

	void	FixPixel(LONG x, LONG y, LONG lFilterSize)
	{
		if (m_bMonochrome)
		{
			double			fGray;

			if (m_pcs.m_Replace == CR_MEDIAN)
				ComputeMedian(x, y, lFilterSize, fGray);
			else
				ComputeGaussian(x, y, lFilterSize, fGray);
			m_pOutBitmap->SetPixel(x, y, fGray);
		}
		else
		{
			double			fRed, fGreen, fBlue;

			if (m_pcs.m_Replace == CR_MEDIAN)
				ComputeMedian(x, y, lFilterSize, fRed, fGreen, fBlue);
			else
				ComputeGaussian(x, y, lFilterSize, fRed, fGreen, fBlue);
			m_pOutBitmap->SetPixel(x, y, fRed, fGreen, fBlue);
		};
	};
	void	FixHotPixel(LONG x, LONG y)
	{
		FixPixel(x, y, m_lHotFilterSize);
	};

	void	FixColdPixel(LONG x, LONG y)
	{
		FixPixel(x, y, m_lColdFilterSize);
	};

public :
	CCleanCosmeticTask()
	{
	};

	virtual ~CCleanCosmeticTask()
	{
	};

	void	Init(CMemoryBitmap * pOutBitmap, CMemoryBitmap * pOrgBitmap, CMemoryBitmap * pDelta, const CPostCalibrationSettings & pcs, CDSSProgress * pProgress)
	{
		m_pOutBitmap		= pOutBitmap;
		m_pOrgBitmap		= pOrgBitmap;
		m_pDelta			= pDelta;
		m_pProgress			= pProgress;
		m_pcs				= pcs;

		m_bMonochrome		= pOutBitmap->IsMonochrome();
		m_bCFA				= pOutBitmap->IsCFA();
		m_CFAType			= GetCFAType(pOutBitmap);
		m_lColdFilterSize	= m_pcs.m_lColdFilter;
		m_lHotFilterSize	= m_pcs.m_lHotFilter;
		m_lWidth			= pOutBitmap->RealWidth();
		m_lHeight			= pOutBitmap->RealHeight();

		if (m_bCFA)
		{
			m_lColdFilterSize *= 2;
			m_lHotFilterSize  *= 2;
		};
	};

	virtual BOOL	DoTask(HANDLE hEvent)
	{
		ZFUNCTRACE_RUNTIME();
		BOOL					bResult = TRUE;
		LONG					i, j;
		BOOL					bEnd = FALSE;
		MSG						msg;
		BOOL					bMonochrome = m_pOutBitmap->IsMonochrome();
		BOOL					bCFA = m_pOutBitmap->IsCFA();

		// Create a message queue and signal the event
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		SetEvent(hEvent);
		while (!bEnd && GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_MT_PROCESS)
			{
				for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
				{
					for (i = 0;i<m_lWidth;i++)
					{
						BOOL				bChanged = FALSE;
						double				fDelta;

						m_pDelta->GetPixel(i, j, fDelta);

						if (fDelta > 200)
						{
							// Hot pixel to fix
							FixHotPixel(i, j);
						}
						else if (fDelta < 100)
						{
							// Cold pixel to fix
							FixColdPixel(i, j);
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

	virtual BOOL	Process()
	{
		ZFUNCTRACE_RUNTIME();
		BOOL				bResult = TRUE;
		LONG				i = 0;
		LONG				lStep;
		LONG				lRemaining;

		if (m_pProgress)
			m_pProgress->SetNrUsedProcessors(GetNrThreads());
		lStep		= max(1L, m_lHeight/50);
		lRemaining	= m_lHeight;
		bResult = TRUE;
		while (i<m_lHeight)
		{
			LONG			lAdd = min(lStep, lRemaining);
			DWORD			dwThreadId;
			
			dwThreadId = GetAvailableThreadId();
			PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

			i			+=lAdd;
			lRemaining	-= lAdd;
			if (m_pProgress)
				m_pProgress->Progress2(NULL, i);
		};

		CloseAllThreads();

		if (m_pProgress)
			m_pProgress->SetNrUsedProcessors();

		return bResult;
	};
};

/* ------------------------------------------------------------------- */

void	CCleanCosmeticTask::ComputeMedian(LONG x, LONG y, LONG lFilterSize, double & fGray)
{
	ZFUNCTRACE_RUNTIME();
	std::vector<double>			vGrays;
	std::vector<double>			vAllGrays;
	BAYERCOLOR					BayerColor = BAYER_UNKNOWN;

	if (m_CFAType != CFATYPE_NONE)
		BayerColor = GetBayerColor(x, y, m_CFAType);

	vGrays.reserve((lFilterSize+1)*2);
	vAllGrays.reserve((lFilterSize+1)*2);
	for (LONG i = max(0L, x-lFilterSize);i<=min(m_lWidth-1, x+lFilterSize);i++)
	{
		for (LONG j = max(0L, y-lFilterSize);j<=min(m_lHeight-1, y+lFilterSize);j++)
		{
			// Check that this is a normal pixel
			BOOL				bAdd = TRUE;
			if (m_CFAType != CFAT_NONE)
				bAdd = (GetBayerColor(i, j, m_CFAType) == BayerColor);

			if (bAdd)
			{
				double					fGray;
				double					fDelta;

				m_pOrgBitmap->GetPixel(i, j, fGray);
				m_pDelta->GetPixel(i, j, fDelta);

				vAllGrays.push_back(fGray);
				if (IsOkValue(fDelta))
					vGrays.push_back(fGray);
			};
		};
	};

	if (vGrays.size() > vAllGrays.size()/3)
		fGray = Median(vGrays);
	else
		fGray = Median(vAllGrays);
};

/* ------------------------------------------------------------------- */

void	CCleanCosmeticTask::ComputeMedian(LONG x, LONG y, LONG lFilterSize, double & fRed, double & fGreen, double & fBlue)
{
	ZFUNCTRACE_RUNTIME();
	std::vector<double>			vReds;
	std::vector<double>			vAllReds;
	std::vector<double>			vGreens;
	std::vector<double>			vAllGreens;
	std::vector<double>			vBlues;
	std::vector<double>			vAllBlues;


	vReds.reserve((lFilterSize+1)*2);
	vAllReds.reserve((lFilterSize+1)*2);
	vGreens.reserve((lFilterSize+1)*2);
	vAllGreens.reserve((lFilterSize+1)*2);
	vBlues.reserve((lFilterSize+1)*2);
	vAllBlues.reserve((lFilterSize+1)*2);
	for (LONG i = max(0L, x-lFilterSize);i<=min(m_lWidth-1, x+lFilterSize);i++)
	{
		for (LONG j = max(0L, y-lFilterSize);j<=min(m_lHeight-1, y+lFilterSize);j++)
		{
			double					fRed, fGreen, fBlue;
			double					fDelta;

			m_pOrgBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
			m_pDelta->GetPixel(i, j, fDelta);

			vAllReds.push_back(fRed);
			vAllGreens.push_back(fGreen);
			vAllBlues.push_back(fBlue);
			if (IsOkValue(fDelta))
			{
				vReds.push_back(fRed);
				vGreens.push_back(fGreen);
				vBlues.push_back(fBlue);
			};
		};
	};

	if (vReds.size() > vAllReds.size()/3)
	{
		fRed	= Median(vReds);
		fGreen	= Median(vGreens);
		fBlue	= Median(vBlues);
	}
	else
	{
		fRed	= Median(vAllReds);
		fGreen	= Median(vAllGreens);
		fBlue	= Median(vAllBlues);
	};
};

/* ------------------------------------------------------------------- */

void	CCleanCosmeticTask::ComputeGaussian(LONG x, LONG y, LONG lFilterSize, double & fGray)
{
	ZFUNCTRACE_RUNTIME();
	double						fSumGrays = 0;
	double						fSumAllGrays = 0;
	BAYERCOLOR					BayerColor = BAYER_UNKNOWN;
	double						fTotalWeight = 0,
								fAllTotalWeight = 0;
	LONG						lNrGrays = 0,
								lNrAllGrays = 0;

	if (m_CFAType != CFATYPE_NONE)
		BayerColor = GetBayerColor(x, y, m_CFAType);

	for (LONG i = max(0L, x-lFilterSize);i<=min(m_lWidth-1, x+lFilterSize);i++)
	{
		for (LONG j = max(0L, y-lFilterSize);j<=min(m_lHeight-1, y+lFilterSize);j++)
		{
			// Check that this is a normal pixel
			BOOL				bAdd = TRUE;
			if (m_CFAType != CFAT_NONE)
				bAdd = (GetBayerColor(i, j, m_CFAType) == BayerColor);

			if (bAdd)
			{
				double					fGray;
				double					fDelta;
				double					fDistance2 = pow((double)(i-x)/lFilterSize, 2)+pow((double)(j-y)/lFilterSize, 2);
				double					fWeight = exp(-fDistance2/2);

				fWeight = 
				m_pOrgBitmap->GetPixel(i, j, fGray);
				m_pDelta->GetPixel(i, j, fDelta);

				fSumAllGrays += fGray*fWeight;
				fAllTotalWeight += fWeight;
				lNrAllGrays++;
				if (IsOkValue(fDelta))
				{
					fSumGrays    += fGray*fWeight;
					fTotalWeight += fWeight;
					lNrGrays++;
				};
			};
		};
	};

	if (lNrGrays > lNrAllGrays/3)
		fGray = fSumGrays/fTotalWeight;
	else
		fGray = fSumAllGrays/fAllTotalWeight;
};

/* ------------------------------------------------------------------- */

void	CCleanCosmeticTask::ComputeGaussian(LONG x, LONG y, LONG lFilterSize, double & fRed, double & fGreen, double & fBlue)
{
	ZFUNCTRACE_RUNTIME();
	double						fSumReds = 0;
	double						fSumAllReds = 0;
	double						fSumGreens = 0;
	double						fSumAllGreens = 0;
	double						fSumBlues = 0;
	double						fSumAllBlues = 0;
	double						fTotalWeight = 0,
								fAllTotalWeight = 0;
	LONG						lNrGrays = 0,
								lNrAllGrays = 0;

	for (LONG i = max(0L, x-lFilterSize);i<=min(m_lWidth-1, x+lFilterSize);i++)
	{
		for (LONG j = max(0L, y-lFilterSize);j<=min(m_lHeight-1, y+lFilterSize);j++)
		{
			// Check that this is a normal pixel
			double					fRed, fGreen, fBlue;
			double					fDelta;
			double					fDistance2 = pow((double)(i-x)/lFilterSize, 2)+pow((double)(j-y)/lFilterSize, 2);
			double					fWeight = exp(-fDistance2/2);

			fWeight = 
			m_pOrgBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
			m_pDelta->GetPixel(i, j, fDelta);

			fSumAllReds		+= fRed*fWeight;
			fSumAllGreens	+= fGreen*fWeight;
			fSumAllBlues	+= fBlue*fWeight;

			fAllTotalWeight += fWeight;
			lNrAllGrays++;
			if (IsOkValue(fDelta))
			{
				fSumReds	 += fRed*fWeight;
				fSumGreens   += fGreen*fWeight;
				fSumBlues    += fBlue*fWeight;
				fTotalWeight += fWeight;
				lNrGrays++;
			};
		};
	};

	if (lNrGrays > lNrAllGrays/3)
	{
		fRed	= fSumReds/fTotalWeight;
		fGreen	= fSumGreens/fTotalWeight;
		fBlue	= fSumBlues/fTotalWeight;
	}
	else
	{
		fRed	= fSumAllReds/fAllTotalWeight;
		fGreen	= fSumAllGreens/fAllTotalWeight;
		fBlue	= fSumAllBlues/fAllTotalWeight;
	};
};

/* ------------------------------------------------------------------- */

BOOL	ApplyCosmetic(CMemoryBitmap * pBitmap, CMemoryBitmap ** ppDeltaBitmap, const CPostCalibrationSettings & pcs, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (ppDeltaBitmap)
		*ppDeltaBitmap = NULL;
	if (pBitmap)
	{
		if (pcs.m_bHot || pcs.m_bCold)
		{
			CSmartPtr<CMemoryBitmap>		pMedian;
			CSmartPtr<CMemoryBitmap>		pDelta;
			LONG							lHeight = pBitmap->RealHeight();
			CString							strText;
			CString							strCorrection;
			CCosmeticStats 					Stats;

			pDelta.Attach(new C8BitGrayBitmap);
			pDelta->Init(pBitmap->RealWidth(), pBitmap->RealHeight());
			
			if (pcs.m_bHot)
			{
				strCorrection.LoadString(IDS_APPLYINGCOSMETIC_HOT);
				if (pProgress)
				{
					strText.LoadString(IDS_CREATINGMEDIANIMAGE);
					strText = strCorrection+" - "+strText;
					pProgress->Start2(strText, 0);
				};

				GetFilteredImage(pBitmap, &pMedian, pcs.m_lHotFilter, pProgress);

				if (pProgress)
					pProgress->Start2(strCorrection, lHeight);

				CDetectCosmeticTask		CosmeticTask;

				CosmeticTask.SetInitDelta(TRUE);
				CosmeticTask.Init(pBitmap, pMedian, pDelta, TRUE, pcs.m_fHotDetection/100.0, pProgress);
				CosmeticTask.StartThreads();
				CosmeticTask.Process();

				CosmeticTask.FillStats(Stats);

				if (pProgress)
					pProgress->End2();

				if (pcs.m_lColdFilter != pcs.m_lHotFilter)
					pMedian.Release();
			};

			if (pcs.m_bCold)
			{
				strCorrection.LoadString(IDS_APPLYINGCOSMETIC_COLD);
				if (!pMedian)
				{
					if (pProgress)
					{
						strText.LoadString(IDS_CREATINGMEDIANIMAGE);
						strText = strCorrection+" - "+strText;
						pProgress->Start2(strText, 0);
					};
					GetFilteredImage(pBitmap, &pMedian, pcs.m_lColdFilter, pProgress);
				};

				if (pProgress)
					pProgress->Start2(strCorrection, lHeight);

				CDetectCosmeticTask		CosmeticTask;

				CosmeticTask.SetInitDelta(!pcs.m_bHot);
				CosmeticTask.Init(pBitmap, pMedian, pDelta, FALSE, pcs.m_fColdDetection/100.0, pProgress);
				CosmeticTask.StartThreads();
				CosmeticTask.Process();

				CosmeticTask.FillStats(Stats);

				if (pProgress)
					pProgress->End2();
			};

			if (Stats.m_lNrDetectedColdPixels || Stats.m_lNrDetectedHotPixels)
			{
				// Now fix it - Use pDelta to retrieve the pixels that need to be fixed
				CCleanCosmeticTask			CosmeticTask;
				CSmartPtr<CMemoryBitmap>	pCloneBitmap;

				pCloneBitmap.Attach(pBitmap->Clone());

				if (pProgress)
					pProgress->Start2(NULL, lHeight);

				CosmeticTask.Init(pBitmap, pCloneBitmap, pDelta, pcs, pProgress);
				CosmeticTask.StartThreads();
				CosmeticTask.Process();

				if (pProgress)
					pProgress->End2();
			};

			if (pDelta && ppDeltaBitmap && pcs.m_bSaveDeltaImage)
				pDelta.CopyTo(ppDeltaBitmap);
		};

		bResult = TRUE;

	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	SimulateCosmetic(CMemoryBitmap * pBitmap, const CPostCalibrationSettings & pcs, CCosmeticStats & cs, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (pBitmap)
	{
		cs.m_lNrTotalPixels = pBitmap->RealWidth()*pBitmap->RealHeight();
		if (pcs.m_bHot || pcs.m_bCold)
		{
			CSmartPtr<CMemoryBitmap>		pMedian;
			LONG							lHeight = pBitmap->RealHeight();
			CString							strText;
			CString							strCorrection;

			if (pcs.m_bHot)
			{
				strCorrection.LoadString(IDS_APPLYINGCOSMETIC_HOT);
				if (pProgress)
				{
					strText.LoadString(IDS_CREATINGMEDIANIMAGE);
					strText = strCorrection+" - "+strText;
					pProgress->Start2(strText, 0);
				};

				GetFilteredImage(pBitmap, &pMedian, pcs.m_lHotFilter, pProgress);

				if (pProgress)
					pProgress->Start2(strCorrection, lHeight);

				CDetectCosmeticTask		CosmeticTask;

				CosmeticTask.SetSimulate(TRUE);
				CosmeticTask.Init(pBitmap, pMedian, NULL, TRUE, pcs.m_fHotDetection/100.0, pProgress);
				CosmeticTask.StartThreads();
				CosmeticTask.Process();

				if (pProgress)
					pProgress->End2();

				if (pcs.m_lColdFilter != pcs.m_lHotFilter)
					pMedian.Release();

				CosmeticTask.FillStats(cs);
			};

			if (pcs.m_bCold)
			{
				strCorrection.LoadString(IDS_APPLYINGCOSMETIC_COLD);
				if (!pMedian)
				{
					if (pProgress)
					{
						strText.LoadString(IDS_CREATINGMEDIANIMAGE);
						strText = strCorrection+" - "+strText;
						pProgress->Start2(strText, 0);
					};
					GetFilteredImage(pBitmap, &pMedian, pcs.m_lColdFilter, pProgress);
				};

				if (pProgress)
					pProgress->Start2(strCorrection, lHeight);

				CDetectCosmeticTask		CosmeticTask;

				CosmeticTask.SetSimulate(TRUE);
				CosmeticTask.Init(pBitmap, pMedian, NULL, FALSE, pcs.m_fColdDetection/100.0, pProgress);
				CosmeticTask.StartThreads();
				CosmeticTask.Process();

				if (pProgress)
					pProgress->End2();

				CosmeticTask.FillStats(cs);
			};
		};

		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
