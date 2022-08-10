#pragma once

#include "FrameInfo.h"


class CTaskInfo
{
public:
	std::uint32_t				m_dwTaskID;
	std::uint32_t				m_groupID;
	PICTURETYPE					m_TaskType;
	int							m_lISOSpeed;
	int							m_lGain;
	double						m_fExposure;
	double						m_fAperture;
	bool						m_bUnmodified;
	bool						m_bDone;
	CString						m_strOutputFile;
	FRAMEINFOVECTOR				m_vBitmaps;
	MULTIBITMAPPROCESSMETHOD	m_Method;
	double						m_fKappa;
	int							m_lNrIterations;
	CSmartPtr<CMultiBitmap>		m_pMaster;

private:
	void	CopyFrom(const CTaskInfo& ti)
	{
		m_dwTaskID = ti.m_dwTaskID;
		m_groupID = ti.m_groupID;
		m_TaskType = ti.m_TaskType;
		m_lISOSpeed = ti.m_lISOSpeed;
		m_lGain = ti.m_lGain;
		m_fExposure = ti.m_fExposure;
		m_fAperture = ti.m_fAperture;
		m_vBitmaps = ti.m_vBitmaps;
		m_bDone = ti.m_bDone;
		m_bUnmodified = ti.m_bUnmodified;
		m_strOutputFile = ti.m_strOutputFile;
		m_Method = ti.m_Method;
		m_fKappa = ti.m_fKappa;
		m_lNrIterations = ti.m_lNrIterations;
		m_pMaster = ti.m_pMaster;
	};

public:
	CTaskInfo()
	{
		m_dwTaskID = 0;
		m_groupID = 0;
		m_lISOSpeed = 0;
		m_lGain = -1;
		m_fExposure = 0.0;
		m_fAperture = 0.0;
		m_bDone = false;
		m_Method = MBP_MEDIAN;
		m_fKappa = 2.0;
		m_lNrIterations = 5;
		m_bUnmodified = false;
		m_TaskType = PICTURETYPE(0);
	};

	CTaskInfo(const CTaskInfo& ti)
	{
		CopyFrom(ti);
	};

	const CTaskInfo& operator=(const CTaskInfo& ti)
	{
		CopyFrom(ti);
		return (*this);
	};

	virtual ~CTaskInfo() = default;

	void	SetMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations)
	{
		m_Method = Method;
		m_fKappa = fKappa;
		m_lNrIterations = lNrIterations;
	};

	void	CreateEmptyMaster(CMemoryBitmap* pBitmap)
	{
		m_pMaster.Attach(pBitmap->CreateEmptyMultiBitmap());
		m_pMaster->SetNrBitmaps((int)m_vBitmaps.size());
		m_pMaster->SetProcessingMethod(m_Method, m_fKappa, m_lNrIterations);
	};

	void	AddToMaster(CMemoryBitmap* pBitmap, CDSSProgress* pProgress)
	{
		if (m_pMaster)
			m_pMaster->AddBitmap(pBitmap, pProgress);
	};

	bool	GetMaster(CMemoryBitmap** ppBitmap, CDSSProgress* pProgress)
	{
		bool			bResult = false;

		if (ppBitmap)
		{
			CSmartPtr<CMemoryBitmap>	pBitmap;

			*ppBitmap = nullptr;
			if (m_pMaster && m_pMaster->GetNrBitmaps() > 1)
			{
				bResult = m_pMaster->GetResult(&pBitmap, pProgress);
				if (pBitmap)
					pBitmap->SetMaster(true);
			};
			m_pMaster.Release();;
			bResult = pBitmap.CopyTo(ppBitmap);
		};

		return bResult;
	};

	bool 	HasISOSpeed() const
	{
		// Has valid ISOSpeed value or no valid Gain value.
		return m_lISOSpeed != 0 || m_lGain < 0;
	};
};
