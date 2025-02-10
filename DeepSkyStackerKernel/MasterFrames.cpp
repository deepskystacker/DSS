#include "stdafx.h"
//#include "resource.h"
#include "MasterFrames.h"
#include "DSSProgress.h"
#include "DeBloom.h"
#include "Workspace.h"
#include "ztrace.h"
#include "StackingTasks.h"
#include "BitmapExt.h"

using namespace DSS;

/* ------------------------------------------------------------------- */
CMasterFrames::CMasterFrames() :
	currentStackinfo{ nullptr },
	m_fDebloom{ Workspace{}.value("Stacking/Debloom", false).toBool() }
{}

bool CMasterFrames::LoadMasters(const CStackingInfo& stackingInfo, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	// If LoadMasters has already been called with this stackingInfo object, we return here.
	if (this->currentStackinfo == std::addressof(stackingInfo))
		return true;
	this->currentStackinfo = std::addressof(stackingInfo);
	bool bResult = true;

	if (stackingInfo.m_pOffsetTask != nullptr)
		bResult = GetTaskResult(stackingInfo.m_pOffsetTask, pProgress, m_pMasterOffset);

	if (stackingInfo.m_pDarkTask != nullptr)
	{
		std::shared_ptr<CMemoryBitmap> pMasterDark;
		bResult = bResult && GetTaskResult(stackingInfo.m_pDarkTask, pProgress, pMasterDark);

		if (bResult)
			m_MasterDark.SetMasterDark(pMasterDark);
	}

	if (stackingInfo.m_pDarkFlatTask)
	{
		std::shared_ptr<CMemoryBitmap> pMasterDarkFlat;
		bResult = bResult && GetTaskResult(stackingInfo.m_pDarkFlatTask, pProgress, pMasterDarkFlat);
		// ### WTF??? Nothing is done with pMasterDarkFlat!
	}

	if (stackingInfo.m_pFlatTask)
	{
		bResult = bResult && GetTaskResult(stackingInfo.m_pFlatTask, pProgress, m_MasterFlat.m_pFlatFrame);
		if (bResult)
			m_MasterFlat.ComputeFlatNormalization(pProgress);
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

void	CMasterFrames::ApplyMasterOffset(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	ZTRACE_RUNTIME("Subtracting Offset Frame");

	if (m_pMasterOffset && m_pMasterOffset->IsOk())
	{
		if (pProgress)
		{
			const QString strText(QCoreApplication::translate("MasterFrame", "Subtracting Offset Frame", "IDS_SUBSTRACTINGOFFSET"));
			pProgress->Start2(strText, 0);
		};
		Subtract(pBitmap, m_pMasterOffset, pProgress);
	};
}

void CMasterFrames::ApplyMasterDark(std::shared_ptr<CMemoryBitmap> pBitmap, const STARVECTOR*, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	ZTRACE_RUNTIME("Subtracting Dark Frame");

	if (m_MasterDark.IsOk())
		m_MasterDark.Subtract(pBitmap, pProgress);
}

void	CMasterFrames::ApplyMasterFlat(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	if (m_MasterFlat.IsOk())
	{
		m_MasterFlat.ComputeFlatNormalization(pProgress);
		if (pProgress)
		{
			const QString strText(QCoreApplication::translate("MasterFrame", "Applying Flat Frame", "IDS_APPLYINGFLAT"));
			pProgress->Start2(strText, 0);
		};
		m_MasterFlat.ApplyFlat(pBitmap, pProgress);
	}
}

void	CMasterFrames::ApplyHotPixelInterpolation(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	if (m_MasterDark.IsOk())
		m_MasterDark.InterpolateHotPixels(pBitmap, pProgress);
}

void CMasterFrames::ApplyAllMasters(std::shared_ptr<CMemoryBitmap> pBitmap, const STARVECTOR*, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CDeBloom debloom;

	if (m_fDebloom)
		debloom.CreateBloomMask(pBitmap.get(), pProgress);

	ApplyMasterOffset(pBitmap, pProgress);
	ApplyMasterDark(pBitmap, nullptr, pProgress); // MT, Sept 2024: STARVECTOR* pStars not any more used for ApplyMasterDark().
	ApplyMasterFlat(pBitmap, pProgress);
	ApplyHotPixelInterpolation(pBitmap, pProgress);

	debloom.DeBloomImage(pBitmap.get(), pProgress);
}
