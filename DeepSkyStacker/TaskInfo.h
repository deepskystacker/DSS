#pragma once

#include "FrameInfo.h"


class CTaskInfo
{
public:
	std::uint32_t m_dwTaskID;
	std::uint32_t m_dwGroupID;
	PICTURETYPE m_TaskType;
	int m_lISOSpeed;
	int m_lGain;
	double m_fExposure;
	double m_fAperture;
	bool m_bUnmodified;
	bool m_bDone;
	CString m_strOutputFile;
	FRAMEINFOVECTOR m_vBitmaps;
	MULTIBITMAPPROCESSMETHOD m_Method;
	double m_fKappa;
	int m_lNrIterations;
	std::shared_ptr<CMultiBitmap> m_pMaster;

public:
	CTaskInfo() :
		m_dwTaskID{ 0 },
		m_dwGroupID{ 0 },
		m_TaskType{ PICTURETYPE(0) },
		m_lISOSpeed{ 0 },
		m_lGain{ -1 },
		m_fExposure{ 0.0 },
		m_fAperture{ 0.0 },
		m_bUnmodified{ false },
		m_bDone{ false },
		m_strOutputFile{},
		m_vBitmaps{},
		m_Method{ MBP_MEDIAN },
		m_fKappa{ 2.0 },
		m_lNrIterations{ 5 },
		m_pMaster{}
	{}

	CTaskInfo(const CTaskInfo&) = default;
	CTaskInfo& operator=(const CTaskInfo&) = default;
	virtual ~CTaskInfo() = default;

	void SetMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations)
	{
		m_Method = Method;
		m_fKappa = fKappa;
		m_lNrIterations = lNrIterations;
	}

	void CreateEmptyMaster(const CMemoryBitmap* pBitmap)
	{
		if (pBitmap == nullptr)
			return;
		m_pMaster = pBitmap->CreateEmptyMultiBitmap();
		m_pMaster->SetNrBitmaps(static_cast<int>(m_vBitmaps.size()));
		m_pMaster->SetProcessingMethod(m_Method, m_fKappa, m_lNrIterations);
	}

	void AddToMaster(const CMemoryBitmap* pBitmap, CDSSProgress* pProgress)
	{
		if (static_cast<bool>(m_pMaster))
			m_pMaster->AddBitmap(pBitmap, pProgress);
	}

	std::shared_ptr<CMemoryBitmap> GetMaster(CDSSProgress* const pProgress)
	{
		std::shared_ptr<CMemoryBitmap> pBitmap;

		if (static_cast<bool>(m_pMaster) && m_pMaster->GetNrBitmaps() > 1)
		{
			pBitmap = m_pMaster->GetResult(pProgress);
			if (static_cast<bool>(pBitmap))
				pBitmap->SetMaster(true);
		}
		m_pMaster.reset();
		return pBitmap;
	}

	bool HasISOSpeed() const
	{
		// Has valid ISOSpeed value or no valid Gain value.
		return m_lISOSpeed != 0 || m_lGain < 0;
	}
};
