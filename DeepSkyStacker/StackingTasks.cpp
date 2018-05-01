#include <stdafx.h>
#include "StackingTasks.h"
#include "Registry.h"
#include "TIFFUtil.h"
#include <set>
#include "Settings.h"

/* ------------------------------------------------------------------- */

BOOL	AreExposureEquals(double fExposure1, double fExposure2)
{
	BOOL			bResult = FALSE;

	if (fExposure1 == fExposure2)
		bResult = TRUE;
	else if (fExposure1 >= 1 && fExposure2 >= 1)
	{
		// Both more than 1 second - 5% difference allowed
		bResult = fabs(fExposure1 - fExposure2)/max(fExposure1, fExposure2) <= 0.05;
	}
	else if (fExposure1 < 1 && fExposure2 < 1 && fExposure1 > 0 && fExposure2 > 0)
	{
		// Both less than 1 second
		bResult = fabs(1.0/fExposure1 - 1.0/fExposure2) < 5;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	LoadFrame(LPCTSTR szFile, PICTURETYPE PictureType, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap)
{
	ZFUNCTRACE_RUNTIME();

	BOOL			bResult = FALSE;

	CBitmapInfo			bmpInfo;

	if (GetPictureInfo(szFile, bmpInfo) && bmpInfo.CanLoad())
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;
		CString						strText;
		CString						strDescription;
		BOOL						bOverrideRAW = TRUE;

		bmpInfo.GetDescription(strDescription);

		switch (PictureType)
		{
		case PICTURETYPE_DARKFRAME:
			if (bmpInfo.m_lNrChannels==3)
				strText.Format(IDS_LOADRGBDARK, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			else
				strText.Format(IDS_LOADGRAYDARK, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			break;
		case PICTURETYPE_DARKFLATFRAME:
			if (bmpInfo.m_lNrChannels==3)
				strText.Format(IDS_LOADRGBDARKFLAT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			else
				strText.Format(IDS_LOADGRAYDARKFLAT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			break;
		case PICTURETYPE_OFFSETFRAME:
			if (bmpInfo.m_lNrChannels==3)
				strText.Format(IDS_LOADRGBOFFSET, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			else
				strText.Format(IDS_LOADGRAYOFFSET, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			break;
		case PICTURETYPE_FLATFRAME:
			if (bmpInfo.m_lNrChannels==3)
				strText.Format(IDS_LOADRGBFLAT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			else
				strText.Format(IDS_LOADGRAYFLAT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			break;
		case PICTURETYPE_LIGHTFRAME:
			if (bmpInfo.m_lNrChannels==3)
				strText.Format(IDS_LOADRGBLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			else
				strText.Format(IDS_LOADGRAYLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFile);
			bOverrideRAW = FALSE;
			break;
		};

		if (pProgress)
			pProgress->Start2(strText, 0);

		if (bOverrideRAW)
			PushRAWSettings(FALSE, TRUE); // Allways use Raw Bayer for dark, offset, and flat frames
		if (::LoadPicture(szFile, &pBitmap, pProgress))
			bResult = pBitmap.CopyTo(ppBitmap);
		if (bOverrideRAW)
			PopRAWSettings();

		if (pProgress)
			pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

class CTaskBitmapCache
{
public :
	DWORD						m_dwOffsetTaskID;
	CSmartPtr<CMemoryBitmap>	m_pOffsetBitmap;
	DWORD						m_dwDarkTaskID;
	CSmartPtr<CMemoryBitmap>	m_pDarkBitmap;
	DWORD						m_dwDarkFlatTaskID;
	CSmartPtr<CMemoryBitmap>	m_pDarkFlatBitmap;
	DWORD						m_dwFlatTaskID;
	CSmartPtr<CMemoryBitmap>	m_pFlatBitmap;

public :
	CTaskBitmapCache() 
	{
		m_dwOffsetTaskID = 0;
		m_dwDarkTaskID   = 0;
		m_dwDarkFlatTaskID   = 0;
		m_dwFlatTaskID   = 0;
	};
	~CTaskBitmapCache() {};

	void	ClearCache()
	{
		m_dwOffsetTaskID = 0;
		m_dwDarkTaskID   = 0;
		m_dwDarkFlatTaskID   = 0;
		m_dwFlatTaskID   = 0;
		m_pOffsetBitmap.Release();
		m_pDarkBitmap.Release();
		m_pDarkFlatBitmap.Release();
		m_pFlatBitmap.Release();
	};

	BOOL	GetTaskResult(CTaskInfo * pTaskInfo, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap)
	{
		ZFUNCTRACE_RUNTIME();
		BOOL					bResult = FALSE;

		*ppBitmap = NULL;
		if (pTaskInfo && pTaskInfo->m_strOutputFile.GetLength())
		{
			switch (pTaskInfo->m_TaskType)
			{
			case PICTURETYPE_OFFSETFRAME :
				if ((m_dwOffsetTaskID == pTaskInfo->m_dwTaskID) && m_pOffsetBitmap)
					bResult = m_pOffsetBitmap.CopyTo(ppBitmap);
				else
				{
					m_pOffsetBitmap.Release();
					bResult = LoadFrame(pTaskInfo->m_strOutputFile, pTaskInfo->m_TaskType, pProgress, &m_pOffsetBitmap);
					if (bResult)
					{
						m_dwOffsetTaskID = pTaskInfo->m_dwTaskID;
						bResult = m_pOffsetBitmap.CopyTo(ppBitmap);
					}
					else
						m_dwDarkTaskID = 0;
				};
				break;
			case PICTURETYPE_DARKFRAME :
				if ((m_dwDarkTaskID == pTaskInfo->m_dwTaskID) && m_pDarkBitmap)
					bResult = m_pDarkBitmap.CopyTo(ppBitmap);
				else
				{
					m_pDarkBitmap.Release();
					bResult = LoadFrame(pTaskInfo->m_strOutputFile, pTaskInfo->m_TaskType, pProgress, &m_pDarkBitmap);
					if (bResult)
					{
						m_dwDarkTaskID = pTaskInfo->m_dwTaskID;
						bResult = m_pDarkBitmap.CopyTo(ppBitmap);
					}
					else
						m_dwDarkTaskID = 0;
				};
				break;
			case PICTURETYPE_DARKFLATFRAME :
				if ((m_dwDarkFlatTaskID == pTaskInfo->m_dwTaskID) && m_pDarkFlatBitmap)
					bResult = m_pDarkFlatBitmap.CopyTo(ppBitmap);
				else
				{
					m_pDarkFlatBitmap.Release();
					bResult = LoadFrame(pTaskInfo->m_strOutputFile, pTaskInfo->m_TaskType, pProgress, &m_pDarkFlatBitmap);
					if (bResult)
					{
						m_dwDarkFlatTaskID = pTaskInfo->m_dwTaskID;
						bResult = m_pDarkFlatBitmap.CopyTo(ppBitmap);
					}
					else
						m_dwDarkFlatTaskID = 0;
				};
				break;
			case PICTURETYPE_FLATFRAME :
				if ((m_dwFlatTaskID == pTaskInfo->m_dwTaskID) && m_pFlatBitmap)
					bResult = m_pFlatBitmap.CopyTo(ppBitmap);
				else
				{
					m_pFlatBitmap.Release();
					bResult = LoadFrame(pTaskInfo->m_strOutputFile, pTaskInfo->m_TaskType, pProgress, &m_pFlatBitmap);
					if (bResult)
					{
						m_dwFlatTaskID = pTaskInfo->m_dwTaskID;
						bResult = m_pFlatBitmap.CopyTo(ppBitmap);
					}
					else
						m_dwFlatTaskID = 0;
				};
				break;
			};
		};

		return bResult;
	};
};

static	CTaskBitmapCache		g_BitmapCache;

/* ------------------------------------------------------------------- */

BOOL	GetTaskResult(CTaskInfo * pTaskInfo, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap)
{
	return g_BitmapCache.GetTaskResult(pTaskInfo, pProgress, ppBitmap);
};

void	ClearTaskCache()
{
	g_BitmapCache.ClearCache();
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL	CStackingInfo::CheckForExistingOffset(CString & strMasterFile)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (m_pOffsetTask && m_pOffsetTask->m_vBitmaps.size())
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		CString			strMasterOffset;
		CString			strMasterOffsetInfo;

		_tsplitpath(m_pOffsetTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

		strMasterOffset.Format(_T("%s%sMasterOffset_ISO%ld.tif"), szDrive, szDir, m_pOffsetTask->m_lISOSpeed);
		strMasterOffsetInfo.Format(_T("%s%sMasterOffset_ISO%ld.Description.txt"), szDrive, szDir, m_pOffsetTask->m_lISOSpeed);

		// Check that the Master Offset File is existing
		COffsetSettings		bmpSettings;
		COffsetSettings		newSettings;

		if (newSettings.InitFromCurrent(m_pOffsetTask, strMasterOffset) &&
			bmpSettings.ReadFromFile(strMasterOffsetInfo))
		{
			if (newSettings == bmpSettings)
			{
				strMasterFile = strMasterOffset;
				bResult = TRUE;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingInfo::DoOffsetTask(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	if (!m_pOffsetTask->m_bDone)
	{
		ASSERT(m_pOffsetTask->m_TaskType == PICTURETYPE_OFFSETFRAME);
		if (m_pOffsetTask->m_vBitmaps.size() == 1)
		{
			m_pOffsetTask->m_strOutputFile = m_pOffsetTask->m_vBitmaps[0].m_strFileName;
			m_pOffsetTask->m_bDone = TRUE;
			m_pOffsetTask->m_bUnmodified = TRUE;
		}
		else if (CheckForExistingOffset(m_pOffsetTask->m_strOutputFile))
		{
			m_pOffsetTask->m_bDone		 = TRUE;
			m_pOffsetTask->m_bUnmodified = TRUE;
		}
		else
		{
			// Else create the master offset 
			CString			strText;
			LONG			i = 0;
			LONG			lNrOffsets = 0;

			strText.LoadString(IDS_CREATEMASTEROFFSET);

			if (pProgress)
				pProgress->Start(strText, (LONG)m_pOffsetTask->m_vBitmaps.size(), TRUE);

			for (i = 0;i<m_pOffsetTask->m_vBitmaps.size() && bResult;i++)
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;

				lNrOffsets++;
				strText.Format(IDS_ADDOFFSET, lNrOffsets, m_pOffsetTask->m_vBitmaps.size());

				if (pProgress)
					pProgress->Progress1(strText, lNrOffsets);

				if (::LoadFrame(m_pOffsetTask->m_vBitmaps[i].m_strFileName, PICTURETYPE_OFFSETFRAME, pProgress, &pBitmap))
				{
					// Load the bitmap
					if (!m_pOffsetTask->m_pMaster)
						m_pOffsetTask->CreateEmptyMaster(pBitmap);

					m_pOffsetTask->AddToMaster(pBitmap, pProgress);
				};

				if (pProgress)
					bResult = !pProgress->IsCanceled();
			};

			if (bResult)
			{
				// Save the resulting master offset
				CSmartPtr<CMemoryBitmap>	pOffsetBitmap;
				CString						strMasterOffset;
				CString						strMasterOffsetInfo;
				CString						strMethod;

				FormatFromMethod(strMethod, m_pOffsetTask->m_Method, m_pOffsetTask->m_fKappa, m_pOffsetTask->m_lNrIterations);
				strText.Format(IDS_COMPUTINGMEDIANOFFSET, (LPCTSTR)strMethod);

				if (pProgress)
				{
					pProgress->Start(strText, 1, FALSE);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(TRUE);
				};
				m_pOffsetTask->GetMaster(&pOffsetBitmap, pProgress);
				if (pProgress)
					pProgress->SetJointProgress(FALSE);
				if (pOffsetBitmap)
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					CString			strInfo;

					strInfo.Format(IDS_MEDIANOFFSETINFO, m_pOffsetTask->m_vBitmaps.size(), (LPCTSTR)strMethod);

					_tsplitpath(m_pOffsetTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

					strMasterOffset.Format(_T("%s%sMasterOffset_ISO%ld.tif"), szDrive, szDir, m_pOffsetTask->m_lISOSpeed);
					strMasterOffsetInfo.Format(_T("%s%sMasterOffset_ISO%ld.Description.txt"), szDrive, szDir, m_pOffsetTask->m_lISOSpeed);

					strText.LoadString(IDS_SAVINGMASTEROFFSET);

					if (pProgress)
					{
						pProgress->Start(strText, 1, FALSE);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(strMasterOffset, 0);
					};
					WriteTIFF(strMasterOffset, pOffsetBitmap, pProgress, strInfo, m_pOffsetTask->m_lISOSpeed, m_pOffsetTask->m_fExposure);

					m_pOffsetTask->m_strOutputFile = strMasterOffset;
					m_pOffsetTask->m_bDone = TRUE;

					// Save the description
					COffsetSettings		s;

					s.InitFromCurrent(m_pOffsetTask, strMasterOffset);
					s.WriteToFile(strMasterOffsetInfo);
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingInfo::CheckForExistingDark(CString & strMasterFile)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (m_pDarkTask && m_pDarkTask->m_vBitmaps.size())
	{
		if (!m_pOffsetTask || (m_pOffsetTask && m_pOffsetTask->m_bUnmodified))
		{
			TCHAR			szDrive[1+_MAX_DRIVE];
			TCHAR			szDir[1+_MAX_DIR];
			CString			strMasterDark;
			CString			strMasterDarkInfo;
			LONG			lExposure = m_pDarkTask->m_fExposure;

			_tsplitpath(m_pDarkTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

			strMasterDark.Format(_T("%s%sMasterDark_ISO%ld_%lds.tif"), szDrive, szDir, m_pDarkTask->m_lISOSpeed, lExposure);
			strMasterDarkInfo.Format(_T("%s%sMasterDark_ISO%ld_%lds.Description.txt"), szDrive, szDir, m_pDarkTask->m_lISOSpeed, lExposure);

			// Check that the Master Offset File is existing
			CDarkSettings		bmpSettings;
			CDarkSettings		newSettings;

			if (newSettings.InitFromCurrent(m_pDarkTask, strMasterDark) &&
				bmpSettings.ReadFromFile(strMasterDarkInfo))
			{
				newSettings.SetMasterOffset(m_pOffsetTask);
				if (newSettings == bmpSettings)
				{
					strMasterFile = strMasterDark;
					bResult = TRUE;
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingInfo::DoDarkTask(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = TRUE;

	if (!m_pDarkTask->m_bDone)
	{
		ASSERT(m_pDarkTask->m_TaskType == PICTURETYPE_DARKFRAME);

		if (m_pDarkTask->m_vBitmaps.size() == 1)
		{
			m_pDarkTask->m_strOutputFile = m_pDarkTask->m_vBitmaps[0].m_strFileName;
			m_pDarkTask->m_bDone = TRUE;
		}
		else if (CheckForExistingDark(m_pDarkTask->m_strOutputFile))
		{
			m_pDarkTask->m_bDone	   = TRUE;
			m_pDarkTask->m_bUnmodified = TRUE;
		}
		else
		{
			// Else create the master dark 
			CString						strText;
			LONG						i;
			LONG						lNrDarks = 0;
			CSmartPtr<CMemoryBitmap>	pMasterOffset;

			strText.LoadString(IDS_CREATEMASTERDARK);
			if (pProgress)
				pProgress->Start(strText, (LONG)m_pDarkTask->m_vBitmaps.size(), TRUE);

			// First load the master offset if available
			if (m_pOffsetTask)
				g_BitmapCache.GetTaskResult(m_pOffsetTask, pProgress, &pMasterOffset);

			// First Add Dark frame
			for (i = 0;i<m_pDarkTask->m_vBitmaps.size() && bResult;i++)
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;

				lNrDarks++;
				strText.Format(IDS_ADDDARK, lNrDarks, m_pDarkTask->m_vBitmaps.size());

				if (pProgress)
					pProgress->Progress1(strText, lNrDarks);

				if (::LoadFrame(m_pDarkTask->m_vBitmaps[i].m_strFileName, PICTURETYPE_DARKFRAME, pProgress, &pBitmap))
				{
					if (!m_pDarkTask->m_pMaster)
						m_pDarkTask->CreateEmptyMaster(pBitmap);

					// Subtract the offset frame from the dark frame
					if (pMasterOffset && !pBitmap->IsMaster())
					{
						CString			strStart2;
						if (pProgress)
						{
							CString			strText;

							pProgress->GetStart2Text(strStart2);
							strText.LoadString(IDS_SUBSTRACTINGOFFSET);
							pProgress->Start2(strText, 0);
						};
						Subtract(pBitmap, pMasterOffset, pProgress);
						if (pProgress)
							pProgress->Start2(strStart2, 0);
					};

					// Add the dark frame
					m_pDarkTask->AddToMaster(pBitmap, pProgress);
				};

				if (pProgress)
					bResult = !pProgress->IsCanceled();
			};

			if (bResult)
			{
				// Save Master Dark Frame
				CSmartPtr<CMemoryBitmap>	pDarkBitmap;
				CString						strMasterDark;
				CString						strMasterDarkInfo;
				CString						strMethod;

				FormatFromMethod(strMethod, m_pDarkTask->m_Method, m_pDarkTask->m_fKappa, m_pDarkTask->m_lNrIterations);
				strText.Format(IDS_COMPUTINGMEDIANDARK, (LPCTSTR)strMethod);

				if (pProgress)
				{
					pProgress->Start(strText, 1, FALSE);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(TRUE);
					pProgress->Start2(strText, 0);
				};
				m_pDarkTask->GetMaster(&pDarkBitmap, pProgress);
				if (pProgress)
					pProgress->SetJointProgress(FALSE);
				if (pDarkBitmap)
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					CString			strInfo;
					LONG			lExposure = m_pDarkTask->m_fExposure;

					strInfo.Format(IDS_MEDIANDARKINFO, m_pDarkTask->m_vBitmaps.size(), (LPCTSTR)strMethod);

					_tsplitpath(m_pDarkTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

					strMasterDark.Format(_T("%s%sMasterDark_ISO%ld_%lds.tif"), szDrive, szDir, m_pDarkTask->m_lISOSpeed, lExposure);
					strMasterDarkInfo.Format(_T("%s%sMasterDark_ISO%ld_%lds.Description.txt"), szDrive, szDir, m_pDarkTask->m_lISOSpeed, lExposure);
					strText.LoadString(IDS_SAVINGMASTERDARK);

					if (pProgress)
					{
						pProgress->Start(strText, 1, FALSE);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(strMasterDark, 0);
					};
					WriteTIFF(strMasterDark, pDarkBitmap, pProgress, strInfo, m_pDarkTask->m_lISOSpeed, m_pDarkTask->m_fExposure);

					m_pDarkTask->m_strOutputFile = strMasterDark;
					m_pDarkTask->m_bDone = TRUE;

					// Save the description
					CDarkSettings		s;

					s.InitFromCurrent(m_pDarkTask, strMasterDark);
					s.SetMasterOffset(m_pOffsetTask);
					s.WriteToFile(strMasterDarkInfo);
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingInfo::CheckForExistingDarkFlat(CString & strMasterFile)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	if (m_pDarkFlatTask && m_pDarkFlatTask->m_vBitmaps.size())
	{
		if (!m_pOffsetTask || (m_pOffsetTask && m_pOffsetTask->m_bUnmodified))
		{
			TCHAR			szDrive[1+_MAX_DRIVE];
			TCHAR			szDir[1+_MAX_DIR];
			CString			strMasterDarkFlat;
			CString			strMasterDarkFlatInfo;
			LONG			lExposure = m_pDarkFlatTask->m_fExposure;

			_tsplitpath(m_pDarkFlatTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

			strMasterDarkFlat.Format(_T("%s%sMasterDarkFlat_ISO%ld_%lds.tif"), szDrive, szDir, m_pDarkFlatTask->m_lISOSpeed, lExposure);
			strMasterDarkFlatInfo.Format(_T("%s%sMasterDarkFlat_ISO%ld_%lds.Description.txt"), szDrive, szDir, m_pDarkFlatTask->m_lISOSpeed, lExposure);

			// Check that the Master Offset File is existing
			CDarkSettings		bmpSettings;
			CDarkSettings		newSettings;

			if (newSettings.InitFromCurrent(m_pDarkFlatTask, strMasterDarkFlat) &&
				bmpSettings.ReadFromFile(strMasterDarkFlatInfo))
			{
				newSettings.SetMasterOffset(m_pOffsetTask);
				if (newSettings == bmpSettings)
				{
					strMasterFile = strMasterDarkFlat;
					bResult = TRUE;
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingInfo::DoDarkFlatTask(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = TRUE;

	if (!m_pDarkFlatTask->m_bDone)
	{
		ASSERT(m_pDarkFlatTask->m_TaskType == PICTURETYPE_DARKFLATFRAME);

		if (m_pDarkFlatTask->m_vBitmaps.size() == 1)
		{
			m_pDarkFlatTask->m_strOutputFile = m_pDarkFlatTask->m_vBitmaps[0].m_strFileName;
			m_pDarkFlatTask->m_bDone = TRUE;
		}
		else if (CheckForExistingDarkFlat(m_pDarkFlatTask->m_strOutputFile))
		{
			m_pDarkFlatTask->m_bDone	   = TRUE;
			m_pDarkFlatTask->m_bUnmodified = TRUE;
		}
		else
		{
			// Else create the master dark flat
			CString						strText;
			LONG						i;
			LONG						lNrDarks = 0;
			CSmartPtr<CMemoryBitmap>	pMasterOffset;

			strText.LoadString(IDS_CREATEMASTERDARKFLAT);
			if (pProgress)
				pProgress->Start(strText, (LONG)m_pDarkFlatTask->m_vBitmaps.size(), TRUE);

			// First load the master offset if available
			if (m_pOffsetTask)
				g_BitmapCache.GetTaskResult(m_pOffsetTask, pProgress, &pMasterOffset);

			// First Add Dark flat frame
			for (i = 0;i<m_pDarkFlatTask->m_vBitmaps.size() && bResult;i++)
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;

				lNrDarks++;
				strText.Format(IDS_ADDDARKFLAT, lNrDarks, m_pDarkFlatTask->m_vBitmaps.size());

				if (pProgress)
					pProgress->Progress1(strText, lNrDarks);

				if (::LoadFrame(m_pDarkFlatTask->m_vBitmaps[i].m_strFileName, PICTURETYPE_DARKFLATFRAME, pProgress, &pBitmap))
				{
					if (!m_pDarkFlatTask->m_pMaster)
						m_pDarkFlatTask->CreateEmptyMaster(pBitmap);

					// Subtract the offset frame from the dark frame
					if (pMasterOffset && !pBitmap->IsMaster())
					{
						CString			strStart2;
						if (pProgress)
						{
							CString			strText;

							pProgress->GetStart2Text(strStart2);
							strText.LoadString(IDS_SUBSTRACTINGOFFSET);
							pProgress->Start2(strText, 0);
						};
						Subtract(pBitmap, pMasterOffset, pProgress);
						if (pProgress)
							pProgress->Start2(strStart2, 0);
					};

					// Add the dark frame
					m_pDarkFlatTask->AddToMaster(pBitmap, pProgress);
				};

				if (pProgress)
					bResult = !pProgress->IsCanceled();
			};

			if (bResult)
			{
				// Save Master Dark Frame
				CSmartPtr<CMemoryBitmap>	pDarkFlatBitmap;
				CString						strMasterDarkFlat;
				CString						strMasterDarkFlatInfo;
				CString						strMethod;

				FormatFromMethod(strMethod, m_pDarkFlatTask->m_Method, m_pDarkFlatTask->m_fKappa, m_pDarkFlatTask->m_lNrIterations);
				strText.Format(IDS_COMPUTINGMEDIANDARKFLAT, (LPCTSTR)strMethod);

				if (pProgress)
				{
					pProgress->Start(strText, 1, FALSE);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(TRUE);
					pProgress->Start2(strText, 0);
				};
				m_pDarkFlatTask->GetMaster(&pDarkFlatBitmap, pProgress);
				if (pProgress)
					pProgress->SetJointProgress(FALSE);
				if (pDarkFlatBitmap)
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					CString			strInfo;
					LONG			lExposure = m_pDarkFlatTask->m_fExposure;

					strInfo.Format(IDS_MEDIANDARKFLATINFO, m_pDarkFlatTask->m_vBitmaps.size(), (LPCTSTR)strMethod);

					_tsplitpath(m_pDarkFlatTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

					strMasterDarkFlat.Format(_T("%s%sMasterDarkFlat_ISO%ld_%lds.tif"), szDrive, szDir, m_pDarkFlatTask->m_lISOSpeed, lExposure);
					strMasterDarkFlatInfo.Format(_T("%s%sMasterDarkFlat_ISO%ld_%lds.Description.txt"), szDrive, szDir, m_pDarkFlatTask->m_lISOSpeed, lExposure);
					strText.LoadString(IDS_SAVINGMASTERDARKFLAT);

					if (pProgress)
					{
						pProgress->Start(strText, 1, FALSE);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(strMasterDarkFlat, 0);
					};
					WriteTIFF(strMasterDarkFlat, pDarkFlatBitmap, pProgress, strInfo, m_pDarkFlatTask->m_lISOSpeed, m_pDarkFlatTask->m_fExposure);

					m_pDarkFlatTask->m_strOutputFile = strMasterDarkFlat;
					m_pDarkFlatTask->m_bDone = TRUE;

					// Save the description
					CDarkSettings		s;

					s.InitFromCurrent(m_pDarkFlatTask, strMasterDarkFlat);
					s.SetMasterOffset(m_pOffsetTask);
					s.WriteToFile(strMasterDarkFlatInfo);
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

class CRunningStatistics
{
public :
	double				m_fSum;
	double				m_fPowSum;
	LONG				m_lNrValues;

	mutable double		m_fMean;
	mutable double		m_fStdDev;

private :
	void	CopyFrom(const CRunningStatistics & s)
	{
		m_fSum			= s.m_fSum;
		m_fPowSum		= s.m_fPowSum;
		m_fMean			= s.m_fMean;
		m_fStdDev		= s.m_fStdDev;
		m_lNrValues		= s.m_lNrValues;
	};

	void	ComputeStatistics() const
	{
		if (m_lNrValues)
		{
			m_fMean		= m_fSum/m_lNrValues;
			m_fStdDev	= sqrt(m_fPowSum/m_lNrValues - pow(m_fSum/m_lNrValues, 2));
		}
		else
		{
			m_fMean		= 0;
			m_fStdDev	= 0;
		};
	};

public :
	CRunningStatistics()
	{
		m_fSum			= 0;
		m_fPowSum		= 0;
		m_fMean			= 0;
		m_fStdDev		= 0;
		m_lNrValues		= 0;
	};

	CRunningStatistics(const CRunningStatistics & rs)
	{
		CopyFrom(rs);
	};

	virtual ~CRunningStatistics()
	{
	};

	CRunningStatistics & operator = (const CRunningStatistics & rs)
	{
		CopyFrom(rs);
		return (*this);
	};

	void		AddValue(double fValue)
	{
		m_lNrValues++;
		m_fSum += fValue;
		m_fPowSum += fValue*fValue;
	};

	double		Normalize(double fValue) const
	{
		if (m_lNrValues && m_fStdDev == 0)
			ComputeStatistics();

		if (m_fStdDev)
		{
			fValue -= m_fMean;
			fValue /= m_fStdDev;
		};

		return fValue;
	};

	double		NormalizeInvert(double fValue) const
	{
		if (m_lNrValues && m_fStdDev == 0)
			ComputeStatistics();

		fValue *= m_fStdDev;
		fValue += m_fMean;

		return fValue;
	};
};

/* ------------------------------------------------------------------- */

class CFlatCalibrationParameters
{
public :
	std::vector<CRunningStatistics>		m_vStats;
	BOOL								m_bInitialized;

private :
	void	AdjustValue(double & fValue)
	{
		fValue = min(max(0, fValue), 255.0);
	};

public :
	CFlatCalibrationParameters()
	{
		m_vStats.resize(BAYER_NRCOLORS);
		m_bInitialized = FALSE;
	};

	BOOL	IsInitialized()
	{
		return m_bInitialized;
	};

	void	ComputeParameters(CMemoryBitmap * pBitmap, CDSSProgress * pProgress);
	void	ApplyParameters(CMemoryBitmap * pBitmap, const CFlatCalibrationParameters & fcp, CDSSProgress * pProgress);
};

/* ------------------------------------------------------------------- */

void	CFlatCalibrationParameters::ComputeParameters(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	CString			strStart2;

	if (pProgress)
	{
		CString			strText;

		pProgress->GetStart2Text(strStart2);
		strText.LoadString(IDS_COMPUTINGFLATCALIBRATION);
		pProgress->Start2(strText, 0);
		pProgress->Start2(NULL, pBitmap->RealWidth());
	};

	for (LONG i = 0;i<pBitmap->RealWidth();i++)
	{
		for (LONG j = 0;j<pBitmap->RealHeight();j++)
		{
			if (pBitmap->IsMonochrome())
			{
				double			fGray;

				pBitmap->GetPixel(i, j, fGray);
				m_vStats[pBitmap->GetBayerColor(i, j)].AddValue(fGray);
			}
			else 
			{
				double			fRed, fGreen, fBlue;

				pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
				m_vStats[BAYER_RED].AddValue(fRed);
				m_vStats[BAYER_GREEN].AddValue(fGreen);
				m_vStats[BAYER_BLUE].AddValue(fBlue);
			};
		};

		if (pProgress)
			pProgress->Progress2(NULL, i+1);
	};

	if (pProgress)
	{
		pProgress->End2();
		pProgress->Start2(strStart2, 0);
	};

	m_bInitialized = TRUE;
};

/* ------------------------------------------------------------------- */

void	CFlatCalibrationParameters::ApplyParameters(CMemoryBitmap * pBitmap, const CFlatCalibrationParameters & fcp, CDSSProgress * pProgress)
{
	CString			strStart2;
	if (pProgress)
	{
		CString			strText;

		pProgress->GetStart2Text(strStart2);
		strText.LoadString(IDS_APPLYINGFLATCALIBRATION);
		pProgress->Start2(strText, 0);
		pProgress->Start2(NULL, pBitmap->RealWidth());
	};

	for (LONG i = 0;i<pBitmap->RealWidth();i++)
	{
		for (LONG j = 0;j<pBitmap->RealHeight();j++)
		{
			if (pBitmap->IsMonochrome())
			{
				double			fGray;

				pBitmap->GetPixel(i, j, fGray);
				fGray = fcp.m_vStats[(LONG)pBitmap->GetBayerColor(i, j)].Normalize(fGray);
				fGray = m_vStats[(LONG)pBitmap->GetBayerColor(i, j)].NormalizeInvert(fGray);
				AdjustValue(fGray);
				pBitmap->SetPixel(i, j, fGray);
			}
			else 
			{
				double			fRed, fGreen, fBlue;

				pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
				fRed	= fcp.m_vStats[(LONG)BAYER_RED].Normalize(fRed);
				fGreen	= fcp.m_vStats[(LONG)BAYER_GREEN].Normalize(fGreen);
				fBlue	= fcp.m_vStats[(LONG)BAYER_BLUE].Normalize(fBlue);
				fRed	= m_vStats[(LONG)BAYER_RED].NormalizeInvert(fRed);
				fGreen	= m_vStats[(LONG)BAYER_GREEN].NormalizeInvert(fGreen);
				fBlue	= m_vStats[(LONG)BAYER_BLUE].NormalizeInvert(fBlue);
				AdjustValue(fRed);
				AdjustValue(fGreen);
				AdjustValue(fBlue);
				pBitmap->SetPixel(i, j, fRed, fGreen, fBlue);
			};
		};

		if (pProgress)
			pProgress->Progress2(NULL, i+1);
	};

	if (pProgress)
	{
		pProgress->End2();
		pProgress->Start2(strStart2, 0);
	};
};

/* ------------------------------------------------------------------- */

BOOL	CStackingInfo::CheckForExistingFlat(CString & strMasterFile)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	if (m_pFlatTask && m_pFlatTask->m_vBitmaps.size())
	{
		if (!m_pOffsetTask || (m_pOffsetTask && m_pOffsetTask->m_bUnmodified))
		{
			TCHAR			szDrive[1+_MAX_DRIVE];
			TCHAR			szDir[1+_MAX_DIR];
			CString			strMasterFlat;
			CString			strMasterFlatInfo;

			_tsplitpath(m_pFlatTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

			strMasterFlat.Format(_T("%s%sMasterFlat_ISO%ld.tif"), szDrive, szDir, m_pFlatTask->m_lISOSpeed);
			strMasterFlatInfo.Format(_T("%s%sMasterFlat_ISO%ld.Description.txt"), szDrive, szDir, m_pFlatTask->m_lISOSpeed);

			// Check that the Master Offset File is existing
			CFlatSettings		bmpSettings;
			CFlatSettings		newSettings;

			if (newSettings.InitFromCurrent(m_pFlatTask, strMasterFlat) &&
				bmpSettings.ReadFromFile(strMasterFlatInfo))
			{
				newSettings.SetMasterOffset(m_pOffsetTask);
				newSettings.SetMasterDarkFlat(m_pDarkFlatTask);
				if (newSettings == bmpSettings)
				{
					strMasterFile = strMasterFlat;
					bResult = TRUE;
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingInfo::DoFlatTask(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = TRUE;

	if (!m_pFlatTask->m_bDone)
	{
		ASSERT(m_pFlatTask->m_TaskType == PICTURETYPE_FLATFRAME);

		if (m_pFlatTask->m_vBitmaps.size() == 1)
		{
			m_pFlatTask->m_strOutputFile = m_pFlatTask->m_vBitmaps[0].m_strFileName;
			m_pFlatTask->m_bDone = TRUE;
		}
		else if (CheckForExistingFlat(m_pFlatTask->m_strOutputFile))
		{
			m_pFlatTask->m_bDone	   = TRUE;
			m_pFlatTask->m_bUnmodified = TRUE;
		}
		else
		{
			// Else create the master flat
			CString		strText;
			LONG		i;
			LONG		lNrFlats = 0;
			CSmartPtr<CMemoryBitmap>	pMasterOffset;
			CSmartPtr<CMemoryBitmap>	pMasterDarkFlat;
			CFlatCalibrationParameters	fcpBase;

			strText.LoadString(IDS_CREATEMASTERFLAT);

			if (pProgress)
				pProgress->Start(strText, (LONG)m_pFlatTask->m_vBitmaps.size(), TRUE);

			// First load the master offset if available
			if (m_pOffsetTask)
				g_BitmapCache.GetTaskResult(m_pOffsetTask, pProgress, &pMasterOffset);
			if (m_pDarkFlatTask)
				g_BitmapCache.GetTaskResult(m_pDarkFlatTask, pProgress, &pMasterDarkFlat);

			for (i = 0;i<m_pFlatTask->m_vBitmaps.size() && bResult;i++)
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;

				lNrFlats++;
				strText.Format(IDS_ADDFLAT, lNrFlats, m_pFlatTask->m_vBitmaps.size());

				if (pProgress)
					pProgress->Progress1(strText, lNrFlats);

				if (::LoadFrame(m_pFlatTask->m_vBitmaps[i].m_strFileName, PICTURETYPE_FLATFRAME, pProgress, &pBitmap))
				{
					CFlatCalibrationParameters		fcpBitmap;
					if (!m_pFlatTask->m_pMaster)
						m_pFlatTask->CreateEmptyMaster(pBitmap);

					// Subtract the offset frame from the dark frame
					if (pMasterOffset && !pBitmap->IsMaster())
					{
						CString			strStart2;
						if (pProgress)
						{
							CString			strText;

							pProgress->GetStart2Text(strStart2);
							strText.LoadString(IDS_SUBSTRACTINGOFFSET);
							pProgress->Start2(strText, 0);
						};
						Subtract(pBitmap, pMasterOffset, pProgress);
						if (pProgress)
							pProgress->Start2(strStart2, 0);
					};

					if (pMasterDarkFlat && !pBitmap->IsMaster())
					{
						CString			strStart2;
						if (pProgress)
						{
							CString			strText;

							pProgress->GetStart2Text(strStart2);
							strText.LoadString(IDS_SUBSTRACTINGDARK);
							pProgress->Start2(strText, 0);
						};
						Subtract(pBitmap, pMasterDarkFlat, pProgress);
						if (pProgress)
							pProgress->Start2(strStart2, 0);
					};

					if (!fcpBase.IsInitialized())
					{
						// This is the first flat
						fcpBase.ComputeParameters(pBitmap, pProgress);
					}
					else
					{
						fcpBitmap.ComputeParameters(pBitmap, pProgress);
						fcpBase.ApplyParameters(pBitmap, fcpBitmap, pProgress);
					};

					// Add the dark frame
					m_pFlatTask->AddToMaster(pBitmap, pProgress);
				};

				if (pProgress)
					bResult = !pProgress->IsCanceled();
			};

			if (bResult)
			{
				// Save Master Flat Frame
				CSmartPtr<CMemoryBitmap>	pFlatBitmap;
				CString						strMasterFlat;
				CString						strMasterFlatInfo;
				CString						strMethod;

				FormatFromMethod(strMethod, m_pFlatTask->m_Method, m_pFlatTask->m_fKappa, m_pFlatTask->m_lNrIterations);
				strText.Format(IDS_COMPUTINGMEDIANFLAT, (LPCTSTR)strMethod);

				if (pProgress)
				{
					pProgress->Start(strText, 1, FALSE);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(TRUE);
				};
				m_pFlatTask->GetMaster(&pFlatBitmap, pProgress);
				if (pProgress)
					pProgress->SetJointProgress(FALSE);

				if (pFlatBitmap)
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					CString			strInfo;

					strInfo.Format(IDS_MEDIANFLATINFO, m_pFlatTask->m_vBitmaps.size(), (LPCTSTR)strMethod);

					_tsplitpath(m_pFlatTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

					strMasterFlat.Format(_T("%s%sMasterFlat_ISO%ld.tif"), szDrive, szDir, m_pFlatTask->m_lISOSpeed);
					strMasterFlatInfo.Format(_T("%s%sMasterFlat_ISO%ld.Description.txt"), szDrive, szDir, m_pFlatTask->m_lISOSpeed);
					strText.LoadString(IDS_SAVINGMASTERFLAT);

					if (pProgress)
					{
						pProgress->Start(strText, 1, FALSE);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(strMasterFlat, 0);
					};
					WriteTIFF(strMasterFlat, pFlatBitmap, pProgress, strInfo, m_pFlatTask->m_lISOSpeed, m_pFlatTask->m_fExposure);

					m_pFlatTask->m_strOutputFile = strMasterFlat;
					m_pFlatTask->m_bDone = TRUE;

					// Save the description
					CFlatSettings		s;

					s.InitFromCurrent(m_pFlatTask, strMasterFlat);
					s.SetMasterOffset(m_pOffsetTask);
					s.SetMasterDarkFlat(m_pDarkFlatTask);
					s.WriteToFile(strMasterFlatInfo);
				};
			};			
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

inline BOOL	IsTaskGroupOk(const CTaskInfo & BaseTask, CTaskInfo * pCurrentTask, CTaskInfo * pNewTask)
{
	BOOL				bResult = FALSE;

	if (pCurrentTask)
	{
		if (pCurrentTask->m_dwGroupID)
		{
			if (pNewTask->m_dwGroupID == pCurrentTask->m_dwGroupID)
				bResult = TRUE;
		}
		else
		{
			if ((pNewTask->m_dwGroupID == BaseTask.m_dwGroupID) || 
				 !pNewTask->m_dwGroupID)
				bResult = TRUE;
		};
	}
	else if ((pNewTask->m_dwGroupID == BaseTask.m_dwGroupID) || 
		     !pNewTask->m_dwGroupID)
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::AddFileToTask(const CFrameInfo & FrameInfo, DWORD dwGroupID)
{
	ZFUNCTRACE_RUNTIME();

	BOOL			bFound = FALSE;

	for (LONG i = 0;i<m_vTasks.size() && !bFound;i++)
	{
		if ((m_vTasks[i].m_TaskType == FrameInfo.m_PictureType) &&
			(m_vTasks[i].m_dwGroupID == dwGroupID))
		{
			// Check ISO and exposure time
			if ((m_vTasks[i].m_lISOSpeed == FrameInfo.m_lISOSpeed) &&
				AreExposureEquals(m_vTasks[i].m_fExposure,FrameInfo.m_fExposure))
			{
				bFound = TRUE;
				m_vTasks[i].m_vBitmaps.push_back(FrameInfo);
			};
		};
	};

	if (!bFound)
	{
		// Create a new task for this file
		CTaskInfo			ti;

		ti.m_dwTaskID  = (LONG)m_vTasks.size()+1;
		ti.m_dwGroupID = dwGroupID;
		ti.m_fExposure = FrameInfo.m_fExposure;
		ti.m_lISOSpeed = FrameInfo.m_lISOSpeed;
		ti.m_TaskType  = FrameInfo.m_PictureType;
		ti.m_vBitmaps.push_back(FrameInfo);

		m_vTasks.push_back(ti);
	};

	if (!m_bUsingJPEG && (FrameInfo.m_strInfos.Left(4) == _T("JPEG")))
		m_bUsingJPEG = TRUE;
	if (!m_bUsingFITS && (FrameInfo.m_strInfos.Left(4) == _T("FITS")))
		m_bUsingFITS = TRUE;
	if (!m_bCalibrating && !FrameInfo.IsLightFrame())
		m_bCalibrating = TRUE;
	if (!m_bUsingBayer && (FrameInfo.GetCFAType() != CFATYPE_NONE))
		m_bUsingBayer = TRUE;
	if (!m_bUsingColorImages && (m_bUsingBayer || FrameInfo.m_lNrChannels>1))
		m_bUsingColorImages = TRUE;

	if (FrameInfo.IsDarkFrame())
	{
		m_bDarkUsed = TRUE;
		m_lNrDarkFrames++;
	}
	else if (FrameInfo.IsDarkFlatFrame())
	{
		m_bDarkUsed = TRUE;
		m_lNrDarkFlatFrames++;
	}
	else if (FrameInfo.IsFlatFrame())
	{
		m_bFlatUsed = TRUE;
		m_lNrFlatFrames++;
	}
	else if (FrameInfo.IsOffsetFrame())
	{
		m_bBiasUsed = TRUE;
		m_lNrBiasFrames++;
	}
	else
	{
		m_lNrLightFrames++;
		m_fMaxExposureTime = max(m_fMaxExposureTime, FrameInfo.m_fExposure);
	};
};

/* ------------------------------------------------------------------- */

CTaskInfo *	CAllStackingTasks::FindBestMatchingTask(const CTaskInfo & BaseTask, PICTURETYPE TaskType)
{
	ZFUNCTRACE_RUNTIME();

	CTaskInfo *			pResult = NULL;
	LONG				j;
	BOOL				bExposureFirst = (TaskType == PICTURETYPE_DARKFRAME);

	if (bExposureFirst)
	{
		// Try to find in same group or in the common group if it's impossible
		// Try same ISO and same exposure
		for (j = 0;j<m_vTasks.size();j++)
		{
			if (m_vTasks[j].m_TaskType == TaskType)
			{
				if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
				{
					if ((BaseTask.m_lISOSpeed == m_vTasks[j].m_lISOSpeed) &&
						AreExposureEquals(BaseTask.m_fExposure, m_vTasks[j].m_fExposure))
					{
						if (pResult)
						{
							if ((pResult->m_dwGroupID == m_vTasks[j].m_dwGroupID) && 
								     (pResult->m_vBitmaps.size() < m_vTasks[j].m_vBitmaps.size()))
								pResult = &m_vTasks[j];
						}
						else
							pResult = &m_vTasks[j];
					};
				};
			};
		};
		if (!pResult)
		{
			// Try to find in same group or in the common group if it's impossible
			// Try same ISO and closest exposure
			for (j = 0;j<m_vTasks.size();j++)
			{
				if (m_vTasks[j].m_TaskType == TaskType)
				{
					if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
					{
						if (BaseTask.m_lISOSpeed == m_vTasks[j].m_lISOSpeed)
						{
							if (pResult)
							{
								if (fabs(BaseTask.m_fExposure - m_vTasks[j].m_fExposure) < fabs(BaseTask.m_fExposure - pResult->m_fExposure))
									pResult = &m_vTasks[j];
								else if (fabs(BaseTask.m_fExposure - m_vTasks[j].m_fExposure) == fabs(BaseTask.m_fExposure - pResult->m_fExposure))
								{
									if (pResult->m_vBitmaps.size() < m_vTasks[j].m_vBitmaps.size())
										pResult = &m_vTasks[j];
								};
							}
							else
								pResult = &m_vTasks[j];
						};
					};
				};
			};
		};

		if (!pResult)
		{
			// Try closest exposure
			for (j = 0;j<m_vTasks.size();j++)
			{
				if (m_vTasks[j].m_TaskType == TaskType)
				{
					if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
					{
						if (pResult)
						{
							if (fabs(BaseTask.m_fExposure - m_vTasks[j].m_fExposure) < fabs(BaseTask.m_fExposure - pResult->m_fExposure))
								pResult = &m_vTasks[j];
							else if (fabs(BaseTask.m_fExposure - m_vTasks[j].m_fExposure) == fabs(BaseTask.m_fExposure - pResult->m_fExposure))
							{
								if (pResult->m_vBitmaps.size() < m_vTasks[j].m_vBitmaps.size())
									pResult = &m_vTasks[j];
							};
						}
						else
							pResult = &m_vTasks[j];
					};
				};
			};
		};
	}
	else
	{
		// Try same ISO
		for (j = 0;j<m_vTasks.size();j++)
		{
			if (m_vTasks[j].m_TaskType == TaskType)
			{
				if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
				{
					if (BaseTask.m_lISOSpeed == m_vTasks[j].m_lISOSpeed)
					{
						if (pResult)
						{
							if (pResult->m_vBitmaps.size() < m_vTasks[j].m_vBitmaps.size())
								pResult = &m_vTasks[j];
						}
						else
							pResult = &m_vTasks[j];
					};
				};
			};
		};

		if (!pResult)
		{
			// Try closest ISO
			for (j = 0;j<m_vTasks.size();j++)
			{
				if (m_vTasks[j].m_TaskType == TaskType)
				{
					if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
					{
						if (m_vTasks[j].m_lISOSpeed)
						{
							if (pResult)
							{
								if (labs(pResult->m_lISOSpeed-BaseTask.m_lISOSpeed) > labs(m_vTasks[j].m_lISOSpeed-BaseTask.m_lISOSpeed))
									pResult = &m_vTasks[j];
								else if (labs(pResult->m_lISOSpeed-BaseTask.m_lISOSpeed) == labs(m_vTasks[j].m_lISOSpeed-BaseTask.m_lISOSpeed))
								{
									if (pResult->m_vBitmaps.size() < m_vTasks[j].m_vBitmaps.size())
										pResult = &m_vTasks[j];
								};
							}
							else
								pResult = &m_vTasks[j];
						};
					};
				};
			};
		};

		if (!pResult)
		{
			// Get any ISO
			for (j = 0;j<m_vTasks.size();j++)
			{
				if (m_vTasks[j].m_TaskType == TaskType)
				{
					if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
					{
						if (pResult)
						{
							if (pResult->m_vBitmaps.size() < m_vTasks[j].m_vBitmaps.size())
								pResult = &m_vTasks[j];
						}
						else
							pResult = &m_vTasks[j];
					};
				};
			};
		};
	};

	return pResult;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::ResolveTasks()
{
	ZFUNCTRACE_RUNTIME();

	m_vStacks.clear();
	for (LONG i = 0;i<m_vTasks.size();i++)
	{
		if (m_vTasks[i].m_TaskType == PICTURETYPE_LIGHTFRAME)
		{
			// Create a new stacking info
			CStackingInfo		si;

			si.m_pLightTask = &(m_vTasks[i]);
			// Try to find the best offset task for this task 
			// same ISO if possible 
			// else the closest ISO, else 0
			// (tie breaker is number of frames in the offset task)
			si.m_pOffsetTask = FindBestMatchingTask(m_vTasks[i], PICTURETYPE_OFFSETFRAME);

			// Try to find the best dark task for this task
			// same ISO and exposure, else same ISO and closest exposure
			// else no ISO and closest exposure
			// (tie breaker is number of frames in the dark task)
			si.m_pDarkTask = FindBestMatchingTask(m_vTasks[i], PICTURETYPE_DARKFRAME);

			// Try to find the best dark flat task for this task
			// same ISO and exposure, else same ISO and closest exposure
			// else no ISO and closest exposure
			// (tie breaker is number of frames in the dark task)
			si.m_pDarkFlatTask = FindBestMatchingTask(m_vTasks[i], PICTURETYPE_DARKFLATFRAME);

			// Try to find the best flat task for this task
			// same ISO if possible, else the closest ISO, else 0
			// (tie breaker is number of frames in the flat task)
			si.m_pFlatTask = FindBestMatchingTask(m_vTasks[i], PICTURETYPE_FLATFRAME);

			m_vStacks.push_back(si);
		};
	};

	UpdateTasksMethods();
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::ResetTasksStatus()
{
	for (LONG i = 0;i<m_vTasks.size();i++)
		m_vTasks[i].m_bDone = FALSE;
};

/* ------------------------------------------------------------------- */

LONG CAllStackingTasks::FindStackID(LPCTSTR szLightFrame)
{
	LONG			lResult = 0;
	LONG			i, j;

	// Find in which stack this light frame is located
	for (i = 0;(i<m_vStacks.size()) && !lResult;i++)
	{
		if (m_vStacks[i].m_pLightTask)
		{
			for (j = 0;j<m_vStacks[i].m_pLightTask->m_vBitmaps.size() && !lResult;j++)
			{
				if (!m_vStacks[i].m_pLightTask->m_vBitmaps[j].m_strFileName.CompareNoCase(szLightFrame))
					lResult = m_vStacks[i].m_pLightTask->m_dwTaskID;
			};
		};
	};

	return lResult;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::UpdateTasksMethods()
{
	ZFUNCTRACE_RUNTIME();

	LONG						i;
	CWorkspace					workspace;
	MULTIBITMAPPROCESSMETHOD	LightMethod = MBP_AVERAGE;
	double						fLightKappa = 2.0;
	DWORD						lLightIteration = 5;
	MULTIBITMAPPROCESSMETHOD	DarkMethod	= MBP_MEDIAN;
	double						fDarkKappa	= 2.0;
	DWORD						lDarkIteration = 5;
	MULTIBITMAPPROCESSMETHOD	FlatMethod	= MBP_MEDIAN;
	double						fFlatKappa	= 2.0;
	DWORD						lFlatIteration = 5;
	MULTIBITMAPPROCESSMETHOD	OffsetMethod	= MBP_MEDIAN;
	double						fOffsetKappa	= 2.0;
	DWORD						lOffsetIteration = 5;

	DWORD						dwMethod;
	CString						strKappa;

	dwMethod = 0;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), dwMethod);
	if (dwMethod)
		LightMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Iteration"), lLightIteration);
	strKappa ="2.0";
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Kappa"), strKappa);
	fLightKappa = _ttof(strKappa);

	dwMethod = 0;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Method"), dwMethod);
	if (dwMethod)
		DarkMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Iteration"), lDarkIteration);
	strKappa ="2.0";
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Kappa"), strKappa);
	fDarkKappa = _ttof(strKappa);

	dwMethod = 0;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Method"), dwMethod);
	if (dwMethod)
		FlatMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Iteration"), lFlatIteration);
	strKappa ="2.0";
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Kappa"), strKappa);
	fFlatKappa = _ttof(strKappa);

	dwMethod = 0;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Method"), dwMethod);
	if (dwMethod)
		OffsetMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Iteration"), lOffsetIteration);
	strKappa ="2.0";
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Kappa"), strKappa);
	fOffsetKappa = _ttof(strKappa);

	for (i = 0;i<m_vStacks.size();i++)
	{
		if (m_vStacks[i].m_pLightTask)
			m_vStacks[i].m_pLightTask->SetMethod(LightMethod, fLightKappa, lLightIteration);
		if (m_vStacks[i].m_pDarkTask)
			m_vStacks[i].m_pDarkTask->SetMethod(DarkMethod, fDarkKappa, lDarkIteration);
		if (m_vStacks[i].m_pDarkFlatTask)
			m_vStacks[i].m_pDarkFlatTask->SetMethod(DarkMethod, fDarkKappa, lDarkIteration);
		if (m_vStacks[i].m_pOffsetTask)
			m_vStacks[i].m_pOffsetTask->SetMethod(OffsetMethod, fOffsetKappa, lOffsetIteration);
		if (m_vStacks[i].m_pFlatTask)
			m_vStacks[i].m_pFlatTask->SetMethod(FlatMethod, fFlatKappa, lFlatIteration);
	};
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::DoOffsetTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	// 1. create all the offset masters
	for (LONG i = 0;i<m_vStacks.size() && bResult;i++)
	{
		if (m_vStacks[i].m_pOffsetTask)
		{
			if (!m_vStacks[i].m_pOffsetTask->m_bDone)
			{
				ZTRACE_RUNTIME("------------------------------\nCreate Master Offset");
				bResult = m_vStacks[i].DoOffsetTask(pProgress);
				ZTRACE_RUNTIME("--> Output File: %s", (LPCTSTR)m_vStacks[i].m_pOffsetTask->m_strOutputFile);
				if (!bResult)
					ZTRACE_RUNTIME("Abort");
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::DoDarkTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	// 2. create all the dark masters (using the offset master if necessary)
	for (LONG i = 0;i<m_vStacks.size() && bResult;i++)
	{
		if (m_vStacks[i].m_pDarkTask)
		{
			if (!m_vStacks[i].m_pDarkTask->m_bDone)
			{
				ZTRACE_RUNTIME("------------------------------\nCreate Master Dark");

				if (m_vStacks[i].m_pOffsetTask)
					// Load the master offset
					ZTRACE_RUNTIME("Load Master Offset: %s", (LPCTSTR)m_vStacks[i].m_pOffsetTask->m_strOutputFile);
				else
					ZTRACE_RUNTIME("No Master Offset");

				CTaskInfo *			pTaskInfo = m_vStacks[i].m_pDarkTask;

				bResult = m_vStacks[i].DoDarkTask(pProgress);
				ZTRACE_RUNTIME("--> Output File: %s", (LPCTSTR)m_vStacks[i].m_pDarkTask->m_strOutputFile);
				if (!bResult)
					ZTRACE_RUNTIME("Abort");
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::DoDarkFlatTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	// 2. create all the dark masters (using the offset master if necessary)
	for (LONG i = 0;i<m_vStacks.size() && bResult;i++)
	{
		if (m_vStacks[i].m_pDarkFlatTask)
		{
			if (!m_vStacks[i].m_pDarkFlatTask->m_bDone)
			{
				ZTRACE_RUNTIME("------------------------------\nCreate Master Dark Flat");

				if (m_vStacks[i].m_pOffsetTask)
					// Load the master offset
					ZTRACE_RUNTIME("Load Master Offset: %s", (LPCTSTR)m_vStacks[i].m_pOffsetTask->m_strOutputFile);
				else
					ZTRACE_RUNTIME("No Master Offset");

				CTaskInfo *			pTaskInfo = m_vStacks[i].m_pDarkFlatTask;

				bResult = m_vStacks[i].DoDarkFlatTask(pProgress);
				ZTRACE_RUNTIME("--> Output File: %s", (LPCTSTR)m_vStacks[i].m_pDarkFlatTask->m_strOutputFile);
				if (!bResult)
					ZTRACE_RUNTIME("Abort");
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::DoFlatTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	// 3. create all the flat masters (using the offset master if necessary)
	for (LONG i = 0;i<m_vStacks.size() && bResult;i++)
	{
		if (m_vStacks[i].m_pFlatTask)
		{
			if (!m_vStacks[i].m_pFlatTask->m_bDone)
			{
				ZTRACE_RUNTIME("------------------------------\nCreate Master Flat");

				if (m_vStacks[i].m_pOffsetTask)
					// Load the master offset
					ZTRACE_RUNTIME("Load Master Offset: %s", (LPCTSTR)m_vStacks[i].m_pOffsetTask->m_strOutputFile);
				else
					ZTRACE_RUNTIME("No Master Offset");

				CTaskInfo *			pTaskInfo = m_vStacks[i].m_pFlatTask;

				bResult = m_vStacks[i].DoFlatTask(pProgress);
				ZTRACE_RUNTIME("--> Output File: %s", (LPCTSTR)m_vStacks[i].m_pFlatTask->m_strOutputFile);
				if (!bResult)
					ZTRACE_RUNTIME("Abort");
			};
		};
	};
	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::CheckReadOnlyStatus(std::vector<CString> & vFolders)
{
	BOOL						bResult = FALSE;
	std::set<CString>			sFolders;
	std::set<CString>::iterator	it;

	for (LONG i = 0;i<m_vTasks.size();i++)
	{
		for (LONG j = 0;j<m_vTasks[i].m_vBitmaps.size();j++)
		{
			CString		strFileName;

			if (!m_vTasks[i].m_vBitmaps[j].IsMasterFrame())
			{
				TCHAR			szDrive[1+_MAX_DRIVE];
				TCHAR			szDir[1+_MAX_DIR];
				CString			strPath;

				strFileName = m_vTasks[i].m_vBitmaps[j].m_strFileName;
				_tsplitpath(strFileName, szDrive, szDir, NULL, NULL);
				strPath = szDrive;
				strPath+= szDir;

				it = sFolders.find(strPath);
				if (it == sFolders.end())
					sFolders.insert(strPath);
			};
		};
	};

	// Check that it is possible to write a file in all the folders
	for (it = sFolders.begin(); it!= sFolders.end();it++)
	{
		BOOL			bDirOk = TRUE;
		CString			strFileName;
		FILE *			hFile;

		strFileName = (*it);
		strFileName += "DSS260FTR.testfile.txt";

		hFile = _tfopen(strFileName, _T("wt"));
		if (hFile)
		{
			int			nResult;

			nResult = fprintf(hFile, "DeepSkyStacker: This is a test file to check that it is possible to write in this folder");
			if (nResult<=0)
				bDirOk = FALSE;
			fclose(hFile);
			DeleteFile(strFileName);
		}
		else
			bDirOk = FALSE;

		if (!bDirOk)
			vFolders.push_back((*it));
	};

	return !vFolders.size();
};

/* ------------------------------------------------------------------- */

__int64	CAllStackingTasks::ComputeNecessaryDiskSpace(CRect & rcOutput)
{
	__int64				ulResult = 0;
	__int64				ulLightSpace = 0,
						ulFlatSpace = 0,
						ulDarkSpace = 0,
						ulDarkFlatSpace = 0,
						ulOffsetSpace = 0;
	__int64				ulNeededSpace = 0;
	__int64				ulPixelSize = 0;

	ulPixelSize = GetPixelSizeMultiplier();
	ulPixelSize *= ulPixelSize;

	for (LONG i = 0;i<m_vStacks.size();i++)
	{
		LONG			lWidth,
						lHeight,
						lNrChannels,
						lNrBytesPerChannel;
		__int64			ulSpace;
		__int64			ulLSpace;

		if (m_vStacks[i].m_pLightTask && m_vStacks[i].m_pLightTask->m_vBitmaps.size())
		{
			lWidth		= m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lWidth;
			lHeight		= m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lHeight;
			lNrChannels = m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lNrChannels;
			lNrBytesPerChannel = m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lBitPerChannels/8;
			
			ulSpace		= lWidth * lHeight * lNrBytesPerChannel * lNrChannels;
			ulLSpace	= lWidth * lHeight * 2 * 3;

			if (!rcOutput.IsRectEmpty())
				ulLSpace = rcOutput.Width() * rcOutput.Height() * 2 * 3;

			if ((m_vStacks[i].m_pLightTask->m_Method == MBP_AVERAGE)
				&& (!IsCometAvailable() || (GetCometStackingMode() != CSM_COMETSTAR)))
				m_vStacks[i].m_pLightTask->m_Method = MBP_FASTAVERAGE;

			if ((m_vStacks[i].m_pLightTask->m_Method != MBP_FASTAVERAGE) &&
				(m_vStacks[i].m_pLightTask->m_Method != MBP_MAXIMUM) &&
				(m_vStacks[i].m_pLightTask->m_Method != MBP_ENTROPYAVERAGE))
				ulLightSpace += ulLSpace * m_vStacks[i].m_pLightTask->m_vBitmaps.size() * ulPixelSize;

			if (m_vStacks[i].m_pLightTask->m_Method == MBP_FASTAVERAGE)
				m_vStacks[i].m_pLightTask->m_Method = MBP_AVERAGE;

			if (m_vStacks[i].m_pOffsetTask)
				ulOffsetSpace = max(ulOffsetSpace, ulSpace * m_vStacks[i].m_pOffsetTask->m_vBitmaps.size());

			if (m_vStacks[i].m_pDarkTask)
				ulDarkSpace = max(ulDarkSpace, ulSpace * m_vStacks[i].m_pDarkTask->m_vBitmaps.size());	

			if (m_vStacks[i].m_pDarkFlatTask)
				ulDarkFlatSpace = max(ulDarkFlatSpace, ulSpace * m_vStacks[i].m_pDarkFlatTask->m_vBitmaps.size());	

			if (m_vStacks[i].m_pFlatTask)
				ulFlatSpace = max(ulFlatSpace, ulSpace * m_vStacks[i].m_pFlatTask->m_vBitmaps.size());	
		};
	};

	ulResult = max(ulLightSpace, max(ulFlatSpace, max(ulOffsetSpace, max(ulDarkSpace, ulDarkFlatSpace))));
	ulResult *= 1.10;

	return ulResult;
};

/* ------------------------------------------------------------------- */

__int64	CAllStackingTasks::ComputeNecessaryDiskSpace()
{
	CRect				rcOutput;

	if (m_bUseCustomRectangle)
		rcOutput = m_rcCustom;
	else
		rcOutput.SetRectEmpty();

	return ComputeNecessaryDiskSpace(rcOutput);
};

/* ------------------------------------------------------------------- */

__int64	CAllStackingTasks::AvailableDiskSpace(CString & strDrive)
{
	CString			strTempPath;

	GetTemporaryFilesFolder(strTempPath);

	ULARGE_INTEGER			ulFreeSpace;
	ULARGE_INTEGER			ulTotal;
	ULARGE_INTEGER			ulTotalFree;

	strDrive = strTempPath;
	strDrive = strDrive.Left(2);

	GetDiskFreeSpaceEx(strTempPath, &ulFreeSpace, &ulTotal, &ulTotalFree);

	return ulFreeSpace.QuadPart;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::GetTemporaryFilesFolder(CString & strFolder)
{
	ZFUNCTRACE_RUNTIME();

	CRegistry			reg;
	CString				strTemp;

	reg.LoadKey(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("TemporaryFilesFolder"), strTemp);
	if (strTemp.GetLength())
	{
		// Check that the folder exists by creating an file in it
		FILE *			hFile;
		CString			strFile;

		strFile = strTemp;
		strFile += "Temp.txt";

		hFile = _tfopen(strFile, _T("wb"));
		if (hFile)
		{
			fclose(hFile);
			DeleteFile(strFile);
		}
		else
			strTemp.Empty();
	};

	if (!strTemp.GetLength())
	{
		TCHAR			szTempPath[1+_MAX_PATH] = _T("");

		GetTempPath(sizeof(szTempPath)/sizeof(TCHAR), szTempPath);

		strTemp = szTempPath;
	};

	strFolder = strTemp;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::SetTemporaryFilesFolder(LPCTSTR szFolder)
{
	ZFUNCTRACE_RUNTIME();

	CRegistry			reg;
	CString				strFolder = szFolder;

	if ((strFolder.Right(1) != _T("\\")) && (strFolder.Right(1) != _T("/")))
		strFolder+=_T("\\");

	reg.SaveKey(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("TemporaryFilesFolder"), strFolder);
};

/* ------------------------------------------------------------------- */

BACKGROUNDCALIBRATIONMODE	CAllStackingTasks::GetBackgroundCalibrationMode()
{
	CWorkspace			workspace;
	DWORD				dwBackgroundCalibration = 1;
	DWORD				dwPerChannelCalibration = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibration"), dwBackgroundCalibration);
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PerChannelBackgroundCalibration"), dwPerChannelCalibration);

	if (dwBackgroundCalibration)
		return BCM_RGB;
	else if (dwPerChannelCalibration)
		return BCM_PERCHANNEL;
	else
		return BCM_NONE;
};

/* ------------------------------------------------------------------- */

BACKGROUNDCALIBRATIONINTERPOLATION	CAllStackingTasks::GetBackgroundCalibrationInterpolation()
{
	CWorkspace			workspace;
	DWORD				dwInterpolation = (DWORD)BCI_RATIONAL;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibrationInterpolation"), dwInterpolation);

	return (BACKGROUNDCALIBRATIONINTERPOLATION)dwInterpolation;
};

/* ------------------------------------------------------------------- */

RGBBACKGROUNDCALIBRATIONMETHOD	CAllStackingTasks::GetRGBBackgroundCalibrationMethod()
{
	CWorkspace			workspace;
	DWORD				dwMethod = (DWORD)RBCM_MAXIMUM;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("RGBBackgroundCalibrationMethod"), dwMethod);

	return (RGBBACKGROUNDCALIBRATIONMETHOD)dwMethod;
};

/* ------------------------------------------------------------------- */

BOOL	CAllStackingTasks::GetDarkOptimization()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("DarkOptimization"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

double	CAllStackingTasks::GetDarkFactor()
{
	double				fResult = 1.0;
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("UseDarkFactor"), dwValue);
	if (dwValue)
	{
		CString			strFactor;
		
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("DarkFactor"), strFactor);
		fResult = _ttof(strFactor);
	};

	return fResult;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::GetHotPixelsDetection()
{
	CWorkspace			workspace;
	DWORD				dwValue = 1;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("HotPixelsDetection"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::GetBadLinesDetection()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BadLinesDetection"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

STACKINGMODE	CAllStackingTasks::GetResultMode()
{
	STACKINGMODE		Result = SM_NORMAL;
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Mosaic"), dwValue);
	if (dwValue==2)
		Result = SM_INTERSECTION;
	else if (dwValue==1)
		Result = SM_MOSAIC;

	return Result;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::GetCreateIntermediates()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CreateIntermediates"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::GetSaveCalibrated()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCalibrated"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

BOOL CAllStackingTasks::GetSaveCalibratedDebayered()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCalibratedDebayered"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

WORD	CAllStackingTasks::GetAlignmentMethod()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignmentTransformation"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

LONG	CAllStackingTasks::GetPixelSizeMultiplier()
{
	CWorkspace			workspace;
	DWORD				dwValue = 1;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PixelSizeMultiplier"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

BOOL	CAllStackingTasks::GetChannelAlign()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignChannels"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

BOOL	CAllStackingTasks::GetSaveIntermediateCometImages()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCometImages"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

BOOL	CAllStackingTasks::GetApplyMedianFilterToCometImage()
{
	CWorkspace			workspace;
	DWORD				dwValue = 1;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("ApplyFilterToCometImages"), dwValue);

	return 0; //dwValue;
};

/* ------------------------------------------------------------------- */

INTERMEDIATEFILEFORMAT CAllStackingTasks::GetIntermediateFileFormat()
{
	CWorkspace			workspace;
	DWORD				dwValue = 1;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("IntermediateFileFormat"), dwValue);

	if (dwValue != IFF_TIFF && dwValue != IFF_FITS)
		dwValue = IFF_TIFF;

	return (INTERMEDIATEFILEFORMAT)dwValue;
};

/* ------------------------------------------------------------------- */

COMETSTACKINGMODE CAllStackingTasks::GetCometStackingMode()
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CometStackingMode"), dwValue);

	if (dwValue != CSM_STANDARD && dwValue != CSM_COMETONLY)
		dwValue = CSM_COMETSTAR;

	return (COMETSTACKINGMODE)dwValue;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::GetPostCalibrationSettings(CPostCalibrationSettings & pcs)
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	dwValue = 0;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_DetectCleanHot"), dwValue);
	pcs.m_bHot = dwValue;

	dwValue = 1;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_HotFilter"), dwValue);
	pcs.m_lHotFilter = dwValue;

	dwValue = 500;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_HotDetection"), dwValue);
	pcs.m_fHotDetection = (double)dwValue/10.0;

	dwValue = 0;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_DetectCleanCold"), dwValue);
	pcs.m_bCold = dwValue;

	dwValue = 1;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ColdFilter"), dwValue);
	pcs.m_lColdFilter = dwValue;

	dwValue = 500;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ColdDetection"), dwValue);
	pcs.m_fColdDetection = (double)dwValue/10.0;

	dwValue = 0;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_SaveDeltaImage"), dwValue);
	pcs.m_bSaveDeltaImage = dwValue;

	dwValue = 1;
	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ReplaceMethod"), dwValue);
	pcs.m_Replace = (COSMETICREPLACE)dwValue;

};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::SetPostCalibrationSettings(const CPostCalibrationSettings & pcs)
{
	CWorkspace			workspace;
	DWORD				dwValue = 0;

	dwValue = pcs.m_bHot;
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_DetectCleanHot"), dwValue);

	dwValue = pcs.m_lHotFilter;
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_HotFilter"), dwValue);

	dwValue = pcs.m_fHotDetection*10.0;
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_HotDetection"), dwValue);

	dwValue = pcs.m_bCold;
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_DetectCleanCold"), dwValue);

	dwValue = pcs.m_lColdFilter;
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ColdFilter"), dwValue);

	dwValue = pcs.m_fColdDetection*10.0;
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ColdDetection"), dwValue);

	dwValue = pcs.m_bSaveDeltaImage;
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_SaveDeltaImage"), dwValue);

	dwValue = pcs.m_Replace;
	workspace.SetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ReplaceMethod"), dwValue);
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::GetOutputSettings(COutputSettings & os)
{
	CRegistry			registry;
	DWORD				dwValue = 0;

	dwValue = 1;
	registry.LoadKey(REGENTRY_BASEKEY_OUTPUT, _T("Output"), dwValue);
	os.m_bOutput = (dwValue == 1);

	dwValue = 0;
	registry.LoadKey(REGENTRY_BASEKEY_OUTPUT, _T("OutputHTML"), dwValue);
	os.m_bOutputHTML = (dwValue == 1);

	dwValue = 0;
	registry.LoadKey(REGENTRY_BASEKEY_OUTPUT, _T("FileName"), dwValue);
	os.m_bAutosave = (dwValue == 0);
	os.m_bFileList = (dwValue == 1);

	dwValue = 1;
	registry.LoadKey(REGENTRY_BASEKEY_OUTPUT, _T("AppendNumber"), dwValue);
	os.m_bAppend = (dwValue == 1);

	dwValue = 0;
	registry.LoadKey(REGENTRY_BASEKEY_OUTPUT, _T("OutputFolder"), dwValue);
	os.m_bRefFrameFolder = (dwValue == 0);
	os.m_bFileListFolder = (dwValue == 1);
	os.m_bOtherFolder	 = (dwValue == 2);

	registry.LoadKey(REGENTRY_BASEKEY_OUTPUT, _T("OutputFolderName"), os.m_strFolder);
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::SetOutputSettings(const COutputSettings & os)
{
	CRegistry			registry;
	DWORD				dwValue;

	dwValue = os.m_bOutput ? 1 : 0;
	registry.SaveKey(REGENTRY_BASEKEY_OUTPUT, _T("Output"), dwValue);

	dwValue = os.m_bOutputHTML ? 1 : 0;
	registry.SaveKey(REGENTRY_BASEKEY_OUTPUT, _T("OutputHTML"), dwValue);

	if (os.m_bAutosave)
		dwValue = 0;
	else
		dwValue = 1;
	registry.SaveKey(REGENTRY_BASEKEY_OUTPUT, _T("FileName"), dwValue);

	dwValue = os.m_bAppend ? 1 : 0;
	registry.SaveKey(REGENTRY_BASEKEY_OUTPUT, _T("AppendNumber"), dwValue);

	if (os.m_bRefFrameFolder)
		dwValue = 0;
	else if (os.m_bFileListFolder)
		dwValue = 1;
	else
		dwValue = 2;
	registry.SaveKey(REGENTRY_BASEKEY_OUTPUT, _T("OutputFolder"), dwValue);

	registry.SaveKey(REGENTRY_BASEKEY_OUTPUT, _T("OutputFolderName"), os.m_strFolder);
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::ClearCache()
{
	g_BitmapCache.ClearCache();
};

/* ------------------------------------------------------------------- */
