#include <stdafx.h>
#include "DSSTools.h"
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "CosmeticEngine.h"
#include "Multitask.h"
#include <omp.h>

/* ------------------------------------------------------------------- */

class CDetectCosmeticTask
{
private:
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CSmartPtr<CMemoryBitmap>	m_pMedian;
	CSmartPtr<CMemoryBitmap>	m_pDelta;
	CDSSProgress*				m_pProgress;
	CCosmeticStats				m_Stats;
	double						m_fThreshold;
	bool						m_bHot;
	bool						m_bSimulate;
	bool						m_bInitDelta;

private:
	double Normalize(const double fValue) const
	{
		return std::pow(fValue / 256.0, 2);
	}

	bool AdjustHotPixel(double& fValue, const double fMedian) const
	{
		bool bResult = false;
		const double normalizedValue = Normalize(fValue);
		const double fPercent = (normalizedValue - Normalize(fMedian)) / normalizedValue;
		if (fPercent > m_fThreshold)
		{
			fValue = fMedian;
			bResult = true;
		}
		return bResult;
	}

	bool AdjustColdPixel(double& fValue, const double fMedian) const
	{
		bool bResult = false;
		const double normalizedMedian = Normalize(fMedian);
		const double fPercent = (normalizedMedian - Normalize(fValue)) / normalizedMedian;
		if (fPercent > m_fThreshold)
		{
			fValue = fMedian;
			bResult = true;
		}
		return bResult;
	}

	bool AdjustPixel(double& fValue, const double fMedian) const
	{
		if (m_bHot)
		{
			if (fValue > fMedian)
				return AdjustHotPixel(fValue, fMedian);
		}
		else
		{
			if (fValue < fMedian)
				return AdjustColdPixel(fValue, fMedian);
		}
		return false;
	}

public:
	CDetectCosmeticTask() :
		m_bSimulate{ false },
		m_bInitDelta{ false },
		m_bHot{ false },
		m_fThreshold{ 0 },
		m_pProgress{ nullptr }
	{}

	~CDetectCosmeticTask() = default;

	void SetSimulate(bool bSimulate)
	{
		m_bSimulate = bSimulate;
	}

	void SetInitDelta(bool bInitDelta)
	{
		m_bInitDelta = bInitDelta;
	}

	void FillStats(CCosmeticStats& cs) const
	{
		if (m_bHot)
			cs.m_lNrDetectedHotPixels = m_Stats.m_lNrDetectedHotPixels;
		else
			cs.m_lNrDetectedColdPixels = m_Stats.m_lNrDetectedColdPixels;
	}

	void Init(CMemoryBitmap* pBitmap, CMemoryBitmap* pMedian, CMemoryBitmap* pDelta, bool bHot, double fThreshold, CDSSProgress* pProgress)
	{
		m_pBitmap		= pBitmap;
		m_pMedian		= pMedian;
		m_pDelta		= pDelta;
		m_bHot			= bHot;
		m_fThreshold	= fThreshold;
		m_pProgress		= pProgress;
	};

	void process()
	{
		if (m_pBitmap->IsMonochrome())
			doProcess<true>();
		else
			doProcess<false>();
	}
private:
	template <bool Monochrome>
	void doProcess();
};

template <bool Monochrome>
void CDetectCosmeticTask::doProcess()
{
	ZFUNCTRACE_RUNTIME();
	const int height = m_pBitmap->RealHeight();
	const int width = m_pBitmap->RealWidth();
	const int nrProcessors = CMultitask::GetNrProcessors();
	int nrHotPixels = 0;
	int nrColdPixels = 0;
	int progress = 0;

	if (m_pProgress != nullptr)
		m_pProgress->SetNrUsedProcessors(nrProcessors);

#pragma omp parallel for schedule(guided, 100) default(none) reduction(+: nrHotPixels, nrColdPixels) if(nrProcessors > 1)
	for (int row = 0; row < height; ++row)
	{
		if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
			m_pProgress->Progress2(nullptr, progress += height / nrProcessors);

		const auto countColdHotPixels = [&](const int col, const int row, const bool changed) -> void
		{
			if (changed)
			{
				if (m_bHot)
					++nrHotPixels;
				else
					++nrColdPixels;
			}
			if (m_pDelta && (changed || m_bInitDelta))
				m_pDelta->SetPixel(col, row, changed ? (m_bHot ? 255 : 50) : 128);
		};

		if constexpr (Monochrome)
		{
			for (int col = 0; col < width; ++col)
			{
				double fGray, fMedianGray;
				m_pBitmap->GetPixel(col, row, fGray);
				m_pMedian->GetPixel(col, row, fMedianGray);
				const bool changed = AdjustPixel(fGray, fMedianGray);
				countColdHotPixels(col, row, changed);
			}
		}
		else
		{
			for (int col = 0; col < width; ++col)
			{
				double fRed, fGreen, fBlue, fMedianRed, fMedianGreen, fMedianBlue;
				m_pBitmap->GetPixel(col, row, fRed, fGreen, fBlue);
				m_pMedian->GetPixel(col, row, fMedianRed, fMedianGreen, fMedianBlue);
				const bool b1 = AdjustPixel(fRed, fMedianRed);
				const bool b2 = AdjustPixel(fGreen, fMedianGreen);
				const bool b3 = AdjustPixel(fBlue, fMedianBlue);
				countColdHotPixels(col, row, b1 || b2 || b3);
			}
		}
	}

	m_Stats.m_lNrDetectedHotPixels += nrHotPixels;
	m_Stats.m_lNrDetectedColdPixels += nrColdPixels;

	if (m_pProgress != nullptr)
		m_pProgress->SetNrUsedProcessors();
}


class CCleanCosmeticTask
{
private:
	CSmartPtr<CMemoryBitmap>	m_pOutBitmap;
	CSmartPtr<CMemoryBitmap>	m_pOrgBitmap;
	CSmartPtr<CMemoryBitmap>	m_pDelta;
	CDSSProgress*				m_pProgress;
	CPostCalibrationSettings    m_pcs;
	int							m_lWidth;
	int							m_lHeight;
	int							m_lColdFilterSize;
	int							m_lHotFilterSize;
	CFATYPE						m_CFAType;
	bool						m_bMonochrome;
	bool						m_bCFA;

private:
	static bool IsOkValue(const double fDelta)
	{
		return (fDelta > 100) && (fDelta < 200);
	}

	void ComputeMedian(int x, int y, int lFilterSize, double& fGray);
	void ComputeMedian(int x, int y, int lFilterSize, double& fRed, double& fGreen, double& fBlue);
	void ComputeGaussian(int x, int y, int lFilterSize, double& fGray);
	void ComputeGaussian(int x, int y, int lFilterSize, double& fRed, double& fGreen, double& fBlue);

	void FixPixel(const int x, const int y, const int lFilterSize)
	{
		if (m_bMonochrome)
		{
			double fGray;
			if (m_pcs.m_Replace == CR_MEDIAN)
				ComputeMedian(x, y, lFilterSize, fGray);
			else
				ComputeGaussian(x, y, lFilterSize, fGray);
			m_pOutBitmap->SetPixel(x, y, fGray);
		}
		else
		{
			double fRed, fGreen, fBlue;
			if (m_pcs.m_Replace == CR_MEDIAN)
				ComputeMedian(x, y, lFilterSize, fRed, fGreen, fBlue);
			else
				ComputeGaussian(x, y, lFilterSize, fRed, fGreen, fBlue);
			m_pOutBitmap->SetPixel(x, y, fRed, fGreen, fBlue);
		}
	}

	void FixHotPixel(int x, int y)
	{
		FixPixel(x, y, m_lHotFilterSize);
	}

	void FixColdPixel(int x, int y)
	{
		FixPixel(x, y, m_lColdFilterSize);
	}

public:
    CCleanCosmeticTask() :
		m_pProgress{ nullptr },
		m_lWidth{ 0 },
		m_lHeight{ 0 },
		m_bMonochrome{ false },
		m_bCFA{ false },
		m_CFAType{ CFATYPE_NONE },
		m_lColdFilterSize{ 0 },
		m_lHotFilterSize{ 0 }
	{}

	~CCleanCosmeticTask() = default;

	void Init(CMemoryBitmap* pOutBitmap, CMemoryBitmap* pOrgBitmap, CMemoryBitmap* pDelta, const CPostCalibrationSettings& pcs, CDSSProgress* pProgress)
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
		}
	}

	void process();
};

void CCleanCosmeticTask::process()
{
	ZFUNCTRACE_RUNTIME();
	const int nrProcessors = CMultitask::GetNrProcessors();
	int progress = 0;

	if (m_pProgress != nullptr)
		m_pProgress->SetNrUsedProcessors(nrProcessors);

#pragma omp parallel for schedule(guided, 100) default(none) if(nrProcessors > 1)
	for (int row = 0; row < m_lHeight; ++row)
	{
		if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
			m_pProgress->Progress2(nullptr, progress += m_lHeight / nrProcessors);

		for (int col = 0; col < m_lWidth; ++col)
		{
			bool changed = false;
			double delta;
			m_pDelta->GetPixel(col, row, delta);
			if (delta > 200)
				FixHotPixel(col, row);
			else if (delta < 100)
				FixColdPixel(col, row);
		}
	}

	if (m_pProgress != nullptr)
		m_pProgress->SetNrUsedProcessors();
}

/* ------------------------------------------------------------------- */

void CCleanCosmeticTask::ComputeMedian(int x, int y, int lFilterSize, double& fGray)
{
	std::vector<double>			vGrays;
	std::vector<double>			vAllGrays;
	BAYERCOLOR					BayerColor = BAYER_UNKNOWN;

	if (m_CFAType != CFATYPE_NONE)
		BayerColor = GetBayerColor(x, y, m_CFAType);

	vGrays.reserve((lFilterSize+1)*2);
	vAllGrays.reserve((lFilterSize+1)*2);
	for (int i = std::max(0, x-lFilterSize); i <= std::min(m_lWidth-1, x+lFilterSize); i++)
	{
		for (int j = std::max(0, y-lFilterSize); j <= std::min(m_lHeight-1, y+lFilterSize); j++)
		{
			// Check that this is a normal pixel
			bool				bAdd = true;
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

void CCleanCosmeticTask::ComputeMedian(int x, int y, int lFilterSize, double& fRed, double& fGreen, double& fBlue)
{
	std::vector<double>			vReds;
	std::vector<double>			vAllReds;
	std::vector<double>			vGreens;
	std::vector<double>			vAllGreens;
	std::vector<double>			vBlues;
	std::vector<double>			vAllBlues;

	const size_t vectorSize = (static_cast<size_t>(lFilterSize) + 1) * 2;
	vReds.reserve(vectorSize);
	vAllReds.reserve(vectorSize);
	vGreens.reserve(vectorSize);
	vAllGreens.reserve(vectorSize);
	vBlues.reserve(vectorSize);
	vAllBlues.reserve(vectorSize);
	for (int i = std::max(0, x-lFilterSize); i <= std::min(m_lWidth-1, x+lFilterSize); i++)
	{
		for (int j = std::max(0, y-lFilterSize); j <= std::min(m_lHeight-1, y+lFilterSize); j++)
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

void CCleanCosmeticTask::ComputeGaussian(int x, int y, int lFilterSize, double& fGray)
{
	double						fSumGrays = 0;
	double						fSumAllGrays = 0;
	BAYERCOLOR					BayerColor = BAYER_UNKNOWN;
	double						fTotalWeight = 0;
	double						fAllTotalWeight = 0;
	int							lNrGrays = 0;
	int							lNrAllGrays = 0;

	if (m_CFAType != CFATYPE_NONE)
		BayerColor = GetBayerColor(x, y, m_CFAType);

	for (int i = std::max(0, x-lFilterSize); i <= std::min(m_lWidth-1, x+lFilterSize); i++)
	{
		for (int j = std::max(0, y-lFilterSize); j <= std::min(m_lHeight-1, y+lFilterSize); j++)
		{
			// Check that this is a normal pixel
			bool bAdd = true;
			if (m_CFAType != CFAT_NONE)
				bAdd = GetBayerColor(i, j, m_CFAType) == BayerColor;

			if (bAdd)
			{
				double fGray;
				double fDelta;
				double fDistance2 = std::pow((i-x) / static_cast<double>(lFilterSize), 2) + std::pow((j-y) / static_cast<double>(lFilterSize), 2);
				double fWeight = std::exp(-fDistance2/2);

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

void CCleanCosmeticTask::ComputeGaussian(int x, int y, int lFilterSize, double& fRed, double& fGreen, double& fBlue)
{
	double						fSumReds = 0;
	double						fSumAllReds = 0;
	double						fSumGreens = 0;
	double						fSumAllGreens = 0;
	double						fSumBlues = 0;
	double						fSumAllBlues = 0;
	double						fTotalWeight = 0,
								fAllTotalWeight = 0;
	int							lNrGrays = 0;
	int							lNrAllGrays = 0;

	for (int i = std::max(0, x-lFilterSize); i <= std::min(m_lWidth-1, x+lFilterSize); i++)
	{
		for (int j = std::max(0, y-lFilterSize); j <= std::min(m_lHeight-1, y+lFilterSize); j++)
		{
			// Check that this is a normal pixel
			double fRed, fGreen, fBlue;
			double fDelta;
			double fDistance2 = std::pow((i-x)/ static_cast<double>(lFilterSize), 2) + std::pow((j-y) / static_cast<double>(lFilterSize), 2);
			double fWeight = std::exp(-fDistance2/2);

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
			}
		}
	}

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
	}
}


bool ApplyCosmetic(CMemoryBitmap* pBitmap, CMemoryBitmap** ppDeltaBitmap, const CPostCalibrationSettings& pcs, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (ppDeltaBitmap != nullptr)
		*ppDeltaBitmap = nullptr;
	if (pBitmap != nullptr)
	{
		if (pcs.m_bHot || pcs.m_bCold)
		{
			CSmartPtr<CMemoryBitmap>		pMedian;
			CSmartPtr<CMemoryBitmap>		pDelta;
			int								lHeight = pBitmap->RealHeight();
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
				}

				GetFilteredImage(pBitmap, &pMedian, pcs.m_lHotFilter, pProgress);

				if (pProgress)
					pProgress->Start2(strCorrection, lHeight);

				CDetectCosmeticTask CosmeticTask;
				CosmeticTask.SetInitDelta(true);
				CosmeticTask.Init(pBitmap, pMedian, pDelta, true, pcs.m_fHotDetection/100.0, pProgress);
				CosmeticTask.process();

				CosmeticTask.FillStats(Stats);

				if (pProgress)
					pProgress->End2();

				if (pcs.m_lColdFilter != pcs.m_lHotFilter)
					pMedian.Release();
			}

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
				}

				if (pProgress)
					pProgress->Start2(strCorrection, lHeight);

				CDetectCosmeticTask CosmeticTask;
				CosmeticTask.SetInitDelta(!pcs.m_bHot);
				CosmeticTask.Init(pBitmap, pMedian, pDelta, false, pcs.m_fColdDetection/100.0, pProgress);
				CosmeticTask.process();

				CosmeticTask.FillStats(Stats);

				if (pProgress)
					pProgress->End2();
			}

			if (Stats.m_lNrDetectedColdPixels || Stats.m_lNrDetectedHotPixels)
			{
				// Now fix it - Use pDelta to retrieve the pixels that need to be fixed
				CSmartPtr<CMemoryBitmap> pCloneBitmap;
				pCloneBitmap.Attach(pBitmap->Clone());

				if (pProgress)
					pProgress->Start2(nullptr, lHeight);

				CCleanCosmeticTask CosmeticTask;
				CosmeticTask.Init(pBitmap, pCloneBitmap, pDelta, pcs, pProgress);
				CosmeticTask.process();

				if (pProgress)
					pProgress->End2();
			}

			if (pDelta && ppDeltaBitmap != nullptr && pcs.m_bSaveDeltaImage)
				pDelta.CopyTo(ppDeltaBitmap);
		}

		bResult = true;

	}
	return bResult;
}


bool SimulateCosmetic(CMemoryBitmap* pBitmap, const CPostCalibrationSettings& pcs, CCosmeticStats& cs, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pBitmap != nullptr)
	{
		cs.m_lNrTotalPixels = pBitmap->RealWidth() * pBitmap->RealHeight();
		if (pcs.m_bHot || pcs.m_bCold)
		{
			CSmartPtr<CMemoryBitmap>		pMedian;
			const int						lHeight = pBitmap->RealHeight();
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

				CDetectCosmeticTask CosmeticTask;
				CosmeticTask.SetSimulate(true);
				CosmeticTask.Init(pBitmap, pMedian, nullptr, true, pcs.m_fHotDetection/100.0, pProgress);
				CosmeticTask.process();

				if (pProgress)
					pProgress->End2();

				if (pcs.m_lColdFilter != pcs.m_lHotFilter)
					pMedian.Release();

				CosmeticTask.FillStats(cs);
			}

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

				CDetectCosmeticTask CosmeticTask;
				CosmeticTask.SetSimulate(true);
				CosmeticTask.Init(pBitmap, pMedian, nullptr, false, pcs.m_fColdDetection/100.0, pProgress);
				CosmeticTask.process();

				if (pProgress)
					pProgress->End2();

				CosmeticTask.FillStats(cs);
			}
		}

		bResult = true;
	}

	return bResult;
}
