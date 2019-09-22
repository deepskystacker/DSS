#include <stdafx.h>
#include "MasterFrames.h"
#include "DSSProgress.h"
#include "DeBloom.h"

/* ------------------------------------------------------------------- */

BOOL CMasterFrames::LoadMasters(CStackingInfo * pStackingInfo, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	if (pStackingInfo->m_pOffsetTask)
		bResult = GetTaskResult(pStackingInfo->m_pOffsetTask, pProgress, &m_pMasterOffset);
	if (pStackingInfo->m_pDarkTask)
	{
		CSmartPtr<CMemoryBitmap>	pMasterDark;

		bResult = bResult && GetTaskResult(pStackingInfo->m_pDarkTask, pProgress, &pMasterDark);

		if (bResult)
			m_MasterDark.SetMasterDark(pMasterDark);
	};
	if (pStackingInfo->m_pDarkFlatTask)
	{
		CSmartPtr<CMemoryBitmap>	pMasterDarkFlat;
		bResult = bResult && GetTaskResult(pStackingInfo->m_pDarkFlatTask, pProgress, &pMasterDarkFlat);
	};

	if (pStackingInfo->m_pFlatTask)
	{
		bResult = bResult && GetTaskResult(pStackingInfo->m_pFlatTask, pProgress, &m_MasterFlat.m_pFlatFrame);
		if (bResult)
			m_MasterFlat.ComputeFlatNormalization(pProgress);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CMasterFrames::ApplyMasterOffset(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CString				strText;

	if (m_pMasterOffset && m_pMasterOffset->IsOk())
	{
		if (pProgress)
		{
			strText.LoadString(IDS_SUBSTRACTINGOFFSET);
			pProgress->Start2(strText, 0);
		};
		Subtract(pBitmap, m_pMasterOffset, pProgress);
	};
};

/* ------------------------------------------------------------------- */

void	CMasterFrames::ApplyMasterDark(CMemoryBitmap * pBitmap, STARVECTOR * pStars, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CString				strText;

	if (m_MasterDark.IsOk())
		m_MasterDark.Subtract(pBitmap, pProgress);
};

/* ------------------------------------------------------------------- */

void	CMasterFrames::ApplyMasterFlat(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CString				strText;

	if (m_MasterFlat.IsOk())
	{
		m_MasterFlat.ComputeFlatNormalization(pProgress);
		if (pProgress)
		{
			strText.LoadString(IDS_APPLYINGFLAT);
			pProgress->Start2(strText, 0);
		};
		m_MasterFlat.ApplyFlat(pBitmap, pProgress);
	};
};

/* ------------------------------------------------------------------- */

void	CMasterFrames::ApplyHotPixelInterpolation(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	if (m_MasterDark.IsOk())
		m_MasterDark.InterpolateHotPixels(pBitmap, pProgress);
};

/* ------------------------------------------------------------------- */

void	CMasterFrames::ApplyAllMasters(CMemoryBitmap * pBitmap, STARVECTOR * pStars, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CDeBloom			debloom;
	bool				bDebloom = false;

	if (m_dwDebloom)
		bDebloom = debloom.CreateBloomMask(pBitmap, pProgress);

	ApplyMasterOffset(pBitmap, pProgress);
	ApplyMasterDark(pBitmap, pStars, pProgress);
	ApplyMasterFlat(pBitmap, pProgress);
	ApplyHotPixelInterpolation(pBitmap, pProgress);

	if (bDebloom)
		debloom.DeBloomImage(pBitmap, pProgress);
};

/* ------------------------------------------------------------------- */

