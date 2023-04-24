#include "stdafx.h"
#include "TaskInfo.h"
#include "MemoryBitmap.h"
#include "MultiBitmap.h"

CTaskInfo::CTaskInfo() :
	m_dwTaskID{ 0 },
	m_groupID{ 0 },
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
{
}

void CTaskInfo::SetMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations)
{
	m_Method = Method;
	m_fKappa = fKappa;
	m_lNrIterations = lNrIterations;
}

void CTaskInfo::CreateEmptyMaster(const CMemoryBitmap* pBitmap)
{
	if (pBitmap == nullptr)
		return;
	m_pMaster = pBitmap->CreateEmptyMultiBitmap();
	m_pMaster->SetNrBitmaps(static_cast<int>(m_vBitmaps.size()));
	m_pMaster->SetProcessingMethod(m_Method, m_fKappa, m_lNrIterations);
}

void CTaskInfo::AddToMaster(CMemoryBitmap* pBitmap, ProgressBase* pProgress)
{
	if (static_cast<bool>(m_pMaster))
		m_pMaster->AddBitmap(pBitmap, pProgress);
}

std::shared_ptr<CMemoryBitmap> CTaskInfo::GetMaster(ProgressBase* const pProgress)
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

bool CTaskInfo::HasISOSpeed() const
{
	// Has valid ISOSpeed value or no valid Gain value.
	return m_lISOSpeed != 0 || m_lGain < 0;
}