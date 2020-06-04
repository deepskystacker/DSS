#include <stdafx.h>
#include "StackingTasks.h"
#include "Registry.h"
#include "TIFFUtil.h"
#include <set>
#include "Settings.h"
#include <QSettings>

/* ------------------------------------------------------------------- */

bool	AreExposureEquals(double fExposure1, double fExposure2)
{
	bool			bResult = false;

	if (fExposure1 == fExposure2)
		bResult = true;
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

bool	LoadFrame(LPCTSTR szFile, PICTURETYPE PictureType, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap)
{
	ZFUNCTRACE_RUNTIME();

	bool			bResult = false;

	CBitmapInfo			bmpInfo;

	if (GetPictureInfo(szFile, bmpInfo) && bmpInfo.CanLoad())
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;
		CString						strText;
		CString						strDescription;
		bool						bOverrideRAW = true;

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
			bOverrideRAW = false;
			break;
		};

		if (pProgress)
			pProgress->Start2(strText, 0);

		if (bOverrideRAW)
			PushRAWSettings(false, true); // Allways use Raw Bayer for dark, offset, and flat frames
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

	bool	GetTaskResult(CTaskInfo * pTaskInfo, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap)
	{
		ZFUNCTRACE_RUNTIME();
		bool					bResult = false;

		*ppBitmap = nullptr;
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

bool	GetTaskResult(CTaskInfo * pTaskInfo, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap)
{
	return g_BitmapCache.GetTaskResult(pTaskInfo, pProgress, ppBitmap);
};

void	ClearTaskCache()
{
	g_BitmapCache.ClearCache();
};

/* ------------------------------------------------------------------- */

static void BuildMasterFileNames(CTaskInfo *pTaskInfo, TCHAR const *pszType, bool bExposure, TCHAR const *pszDrive, TCHAR const *pszDir,
	CString *pstrMasterFile, CString *pstrMasterInfoFile)
{
	TCHAR const *pszISOGain = pTaskInfo->HasISOSpeed() ? _T("ISO") : _T("Gain");
	LONG const lISOGain = pTaskInfo->HasISOSpeed() ? pTaskInfo->m_lISOSpeed : pTaskInfo->m_lGain;

	CString strFileName;
	if (bExposure)
		strFileName.Format(_T("%s%s%s_%s%ld_%lds"),
			pszDrive, pszDir, pszType, pszISOGain, lISOGain, (LONG)pTaskInfo->m_fExposure);
	else
		strFileName.Format(_T("%s%s%s_%s%ld"),
			pszDrive, pszDir, pszType, pszISOGain, lISOGain);

	pstrMasterFile->Format(_T("%s.tif"), strFileName);
	pstrMasterInfoFile->Format(_T("%s.Description.txt"), strFileName);
};

/* ------------------------------------------------------------------- */

static void WriteMasterTIFF(LPCTSTR szMasterFileName, CMemoryBitmap * pMasterBitmap, CDSSProgress * pProgress,
			LPCTSTR szDescription, CTaskInfo *pTaskInfo)
{
    WriteTIFF(szMasterFileName, pMasterBitmap, pProgress, szDescription,
		pTaskInfo->m_lISOSpeed, pTaskInfo->m_lGain, pTaskInfo->m_fExposure, pTaskInfo->m_fAperture);
};

/* ------------------------------------------------------------------- */

bool	CStackingInfo::CheckForExistingOffset(CString & strMasterFile)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;

	if (m_pOffsetTask && m_pOffsetTask->m_vBitmaps.size())
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		CString			strMasterOffset;
		CString			strMasterOffsetInfo;

		_tsplitpath(m_pOffsetTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, nullptr, nullptr);

		BuildMasterFileNames(m_pOffsetTask, _T("MasterOffset"), /* bExposure */ false, szDrive, szDir,
			&strMasterOffset, &strMasterOffsetInfo);

		// Check that the Master Offset File is existing
		COffsetSettings		bmpSettings;
		COffsetSettings		newSettings;

		if (newSettings.InitFromCurrent(m_pOffsetTask, strMasterOffset) &&
			bmpSettings.ReadFromFile(strMasterOffsetInfo))
		{
			if (newSettings == bmpSettings)
			{
				strMasterFile = strMasterOffset;
				bResult = true;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CStackingInfo::DoOffsetTask(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

	if (!m_pOffsetTask->m_bDone)
	{
		ASSERT(m_pOffsetTask->m_TaskType == PICTURETYPE_OFFSETFRAME);
		if (m_pOffsetTask->m_vBitmaps.size() == 1)
		{
			m_pOffsetTask->m_strOutputFile = m_pOffsetTask->m_vBitmaps[0].m_strFileName;
			m_pOffsetTask->m_bDone = true;
			m_pOffsetTask->m_bUnmodified = true;
		}
		else if (CheckForExistingOffset(m_pOffsetTask->m_strOutputFile))
		{
			m_pOffsetTask->m_bDone		 = true;
			m_pOffsetTask->m_bUnmodified = true;
		}
		else
		{
			// Else create the master offset
			CString			strText;
			LONG			i = 0;
			LONG			lNrOffsets = 0;

			strText.LoadString(IDS_CREATEMASTEROFFSET);
			ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

			if (pProgress)
				pProgress->Start(strText, (LONG)m_pOffsetTask->m_vBitmaps.size(), true);

			for (i = 0;i<m_pOffsetTask->m_vBitmaps.size() && bResult;i++)
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;

				lNrOffsets++;
				strText.Format(IDS_ADDOFFSET, lNrOffsets, m_pOffsetTask->m_vBitmaps.size());
				ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

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
				ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

				if (pProgress)
				{
					pProgress->Start(strText, 1, false);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(true);
				};
				m_pOffsetTask->GetMaster(&pOffsetBitmap, pProgress);
				if (pProgress)
					pProgress->SetJointProgress(false);
				if (pOffsetBitmap)
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					CString			strInfo;

					strInfo.Format(IDS_MEDIANOFFSETINFO, m_pOffsetTask->m_vBitmaps.size(), (LPCTSTR)strMethod);

					_tsplitpath(m_pOffsetTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, nullptr, nullptr);

					BuildMasterFileNames(m_pOffsetTask, _T("MasterOffset"), /* bExposure */ false, szDrive, szDir,
						&strMasterOffset, &strMasterOffsetInfo);

					strText.LoadString(IDS_SAVINGMASTEROFFSET);
					ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

					if (pProgress)
					{
						pProgress->Start(strText, 1, false);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(strMasterOffset, 0);
					};
					WriteMasterTIFF(strMasterOffset, pOffsetBitmap, pProgress, strInfo, m_pOffsetTask);

					m_pOffsetTask->m_strOutputFile = strMasterOffset;
					m_pOffsetTask->m_bDone = true;

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

bool	CStackingInfo::CheckForExistingDark(CString & strMasterFile)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;

	if (m_pDarkTask && m_pDarkTask->m_vBitmaps.size())
	{
		if (!m_pOffsetTask || (m_pOffsetTask && m_pOffsetTask->m_bUnmodified))
		{
			TCHAR			szDrive[1+_MAX_DRIVE];
			TCHAR			szDir[1+_MAX_DIR];
			CString			strMasterDark;
			CString			strMasterDarkInfo;
			LONG			lExposure = m_pDarkTask->m_fExposure;

			_tsplitpath(m_pDarkTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, nullptr, nullptr);

			BuildMasterFileNames(m_pDarkTask, _T("MasterDark"), /* bExposure */ true, szDrive, szDir,
				&strMasterDark, &strMasterDarkInfo);

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
					bResult = true;
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CStackingInfo::DoDarkTask(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = true;

	if (!m_pDarkTask->m_bDone)
	{
		ASSERT(m_pDarkTask->m_TaskType == PICTURETYPE_DARKFRAME);

		if (m_pDarkTask->m_vBitmaps.size() == 1)
		{
			m_pDarkTask->m_strOutputFile = m_pDarkTask->m_vBitmaps[0].m_strFileName;
			m_pDarkTask->m_bDone = true;
		}
		else if (CheckForExistingDark(m_pDarkTask->m_strOutputFile))
		{
			m_pDarkTask->m_bDone	   = true;
			m_pDarkTask->m_bUnmodified = true;
		}
		else
		{
			// Else create the master dark
			CString						strText;
			LONG						i;
			LONG						lNrDarks = 0;
			CSmartPtr<CMemoryBitmap>	pMasterOffset;

			strText.LoadString(IDS_CREATEMASTERDARK);
			ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

			if (pProgress)
				pProgress->Start(strText, (LONG)m_pDarkTask->m_vBitmaps.size(), true);

			// First load the master offset if available
			if (m_pOffsetTask)
				g_BitmapCache.GetTaskResult(m_pOffsetTask, pProgress, &pMasterOffset);

			// First Add Dark frame
			for (i = 0;i<m_pDarkTask->m_vBitmaps.size() && bResult;i++)
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;

				lNrDarks++;
				strText.Format(IDS_ADDDARK, lNrDarks, m_pDarkTask->m_vBitmaps.size());
				ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

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
							ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));
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
				ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

				if (pProgress)
				{
					pProgress->Start(strText, 1, false);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(true);
					pProgress->Start2(strText, 0);
				};
				m_pDarkTask->GetMaster(&pDarkBitmap, pProgress);
				if (pProgress)
					pProgress->SetJointProgress(false);
				if (pDarkBitmap)
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					CString			strInfo;
					LONG			lExposure = m_pDarkTask->m_fExposure;

					strInfo.Format(IDS_MEDIANDARKINFO, m_pDarkTask->m_vBitmaps.size(), (LPCTSTR)strMethod);

					_tsplitpath(m_pDarkTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, nullptr, nullptr);

					BuildMasterFileNames(m_pDarkTask, _T("MasterDark"), /* bExposure */ true, szDrive, szDir,
						&strMasterDark, &strMasterDarkInfo);
					strText.LoadString(IDS_SAVINGMASTERDARK);
					ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

					if (pProgress)
					{
						pProgress->Start(strText, 1, false);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(strMasterDark, 0);
					};
					WriteMasterTIFF(strMasterDark, pDarkBitmap, pProgress, strInfo, m_pDarkTask);

					m_pDarkTask->m_strOutputFile = strMasterDark;
					m_pDarkTask->m_bDone = true;

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

bool	CStackingInfo::CheckForExistingDarkFlat(CString & strMasterFile)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;

	if (m_pDarkFlatTask && m_pDarkFlatTask->m_vBitmaps.size())
	{
		if (!m_pOffsetTask || (m_pOffsetTask && m_pOffsetTask->m_bUnmodified))
		{
			TCHAR			szDrive[1+_MAX_DRIVE];
			TCHAR			szDir[1+_MAX_DIR];
			CString			strMasterDarkFlat;
			CString			strMasterDarkFlatInfo;
			LONG			lExposure = m_pDarkFlatTask->m_fExposure;

			_tsplitpath(m_pDarkFlatTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, nullptr, nullptr);

			BuildMasterFileNames(m_pDarkFlatTask, _T("MasterDarkFlat"), /* bExposure */ true, szDrive, szDir,
				&strMasterDarkFlat, &strMasterDarkFlatInfo);

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
					bResult = true;
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CStackingInfo::DoDarkFlatTask(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = true;

	if (!m_pDarkFlatTask->m_bDone)
	{
		ASSERT(m_pDarkFlatTask->m_TaskType == PICTURETYPE_DARKFLATFRAME);

		if (m_pDarkFlatTask->m_vBitmaps.size() == 1)
		{
			m_pDarkFlatTask->m_strOutputFile = m_pDarkFlatTask->m_vBitmaps[0].m_strFileName;
			m_pDarkFlatTask->m_bDone = true;
		}
		else if (CheckForExistingDarkFlat(m_pDarkFlatTask->m_strOutputFile))
		{
			m_pDarkFlatTask->m_bDone	   = true;
			m_pDarkFlatTask->m_bUnmodified = true;
		}
		else
		{
			// Else create the master dark flat
			CString						strText;
			LONG						i;
			LONG						lNrDarks = 0;
			CSmartPtr<CMemoryBitmap>	pMasterOffset;

			strText.LoadString(IDS_CREATEMASTERDARKFLAT);
			ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

			if (pProgress)
				pProgress->Start(strText, (LONG)m_pDarkFlatTask->m_vBitmaps.size(), true);

			// First load the master offset if available
			if (m_pOffsetTask)
				g_BitmapCache.GetTaskResult(m_pOffsetTask, pProgress, &pMasterOffset);

			// First Add Dark flat frame
			for (i = 0;i<m_pDarkFlatTask->m_vBitmaps.size() && bResult;i++)
			{
				CSmartPtr<CMemoryBitmap>	pBitmap;

				lNrDarks++;
				strText.Format(IDS_ADDDARKFLAT, lNrDarks, m_pDarkFlatTask->m_vBitmaps.size());
				ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

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
							ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

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
				ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

				if (pProgress)
				{
					pProgress->Start(strText, 1, false);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(true);
					pProgress->Start2(strText, 0);
				};
				m_pDarkFlatTask->GetMaster(&pDarkFlatBitmap, pProgress);
				if (pProgress)
					pProgress->SetJointProgress(false);
				if (pDarkFlatBitmap)
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					CString			strInfo;
					LONG			lExposure = m_pDarkFlatTask->m_fExposure;

					strInfo.Format(IDS_MEDIANDARKFLATINFO, m_pDarkFlatTask->m_vBitmaps.size(), (LPCTSTR)strMethod);

					_tsplitpath(m_pDarkFlatTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, nullptr, nullptr);

					BuildMasterFileNames(m_pDarkFlatTask, _T("MasterDarkFlat"), /* bExposure */ true, szDrive, szDir,
						&strMasterDarkFlat, &strMasterDarkFlatInfo);
					strText.LoadString(IDS_SAVINGMASTERDARKFLAT);
					ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

					if (pProgress)
					{
						pProgress->Start(strText, 1, false);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(strMasterDarkFlat, 0);
					};
					WriteMasterTIFF(strMasterDarkFlat, pDarkFlatBitmap, pProgress, strInfo, m_pDarkFlatTask);

					m_pDarkFlatTask->m_strOutputFile = strMasterDarkFlat;
					m_pDarkFlatTask->m_bDone = true;

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
	bool								m_bInitialized;

private :
	void	AdjustValue(double & fValue)
	{
		fValue = min(max(0.0, fValue), 255.0);
	};

public :
	CFlatCalibrationParameters()
	{
		m_vStats.resize(BAYER_NRCOLORS);
		m_bInitialized = false;
	};

	bool	IsInitialized()
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
		ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

		pProgress->Start2(strText, 0);
		pProgress->Start2(nullptr, pBitmap->RealWidth());
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
			pProgress->Progress2(nullptr, i+1);
	};

	if (pProgress)
	{
		pProgress->End2();
		pProgress->Start2(strStart2, 0);
	};

	m_bInitialized = true;
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
		ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

		pProgress->Start2(strText, 0);
		pProgress->Start2(nullptr, pBitmap->RealWidth());
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
			pProgress->Progress2(nullptr, i+1);
	};

	if (pProgress)
	{
		pProgress->End2();
		pProgress->Start2(strStart2, 0);
	};
};

/* ------------------------------------------------------------------- */

bool	CStackingInfo::CheckForExistingFlat(CString & strMasterFile)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;

	if (m_pFlatTask && m_pFlatTask->m_vBitmaps.size())
	{
		if (!m_pOffsetTask || (m_pOffsetTask && m_pOffsetTask->m_bUnmodified))
		{
			TCHAR			szDrive[1+_MAX_DRIVE];
			TCHAR			szDir[1+_MAX_DIR];
			CString			strMasterFlat;
			CString			strMasterFlatInfo;

			_tsplitpath(m_pFlatTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, nullptr, nullptr);

			BuildMasterFileNames(m_pFlatTask, _T("MasterFlat"), /* bExposure */ false, szDrive, szDir,
				&strMasterFlat, &strMasterFlatInfo);

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
					bResult = true;
				};
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CStackingInfo::DoFlatTask(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = true;

	if (!m_pFlatTask->m_bDone)
	{
		ASSERT(m_pFlatTask->m_TaskType == PICTURETYPE_FLATFRAME);

		if (m_pFlatTask->m_vBitmaps.size() == 1)
		{
			m_pFlatTask->m_strOutputFile = m_pFlatTask->m_vBitmaps[0].m_strFileName;
			m_pFlatTask->m_bDone = true;
		}
		else if (CheckForExistingFlat(m_pFlatTask->m_strOutputFile))
		{
			m_pFlatTask->m_bDone	   = true;
			m_pFlatTask->m_bUnmodified = true;
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
			ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

			if (pProgress)
				pProgress->Start(strText, (LONG)m_pFlatTask->m_vBitmaps.size(), true);

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
				ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

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
						CString			strText;
						CString			strStart2;

						strText.LoadString(IDS_SUBSTRACTINGOFFSET);
						ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

						if (pProgress)
						{
							pProgress->GetStart2Text(strStart2);
							pProgress->Start2(strText, 0);
						};
						Subtract(pBitmap, pMasterOffset, pProgress);
						if (pProgress)
							pProgress->Start2(strStart2, 0);
					};

					if (pMasterDarkFlat && !pBitmap->IsMaster())
					{
						CString			strText;
						CString			strStart2;

						strText.LoadString(IDS_SUBSTRACTINGDARK);
						ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

						if (pProgress)
						{
							pProgress->GetStart2Text(strStart2);
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
				ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

				if (pProgress)
				{
					pProgress->Start(strText, 1, false);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(true);
				};
				m_pFlatTask->GetMaster(&pFlatBitmap, pProgress);
				if (pProgress)
					pProgress->SetJointProgress(false);

				if (pFlatBitmap)
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					CString			strInfo;

					strInfo.Format(IDS_MEDIANFLATINFO, m_pFlatTask->m_vBitmaps.size(), (LPCTSTR)strMethod);

					_tsplitpath(m_pFlatTask->m_vBitmaps[0].m_strFileName, szDrive, szDir, nullptr, nullptr);

					BuildMasterFileNames(m_pFlatTask, _T("MasterFlat"), /* bExposure */ false, szDrive, szDir,
						&strMasterFlat, &strMasterFlatInfo);
					strText.LoadString(IDS_SAVINGMASTERFLAT);
					ZTRACE_RUNTIME(CT2CA(strText, CP_UTF8));

					if (pProgress)
					{
						pProgress->Start(strText, 1, false);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(strMasterFlat, 0);
					};
					WriteMasterTIFF(strMasterFlat, pFlatBitmap, pProgress, strInfo, m_pFlatTask);

					m_pFlatTask->m_strOutputFile = strMasterFlat;
					m_pFlatTask->m_bDone = true;

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

inline bool	IsTaskGroupOk(const CTaskInfo & BaseTask, CTaskInfo * pCurrentTask, CTaskInfo * pNewTask)
{
	bool				bResult = false;

	if (pCurrentTask)
	{
		if (pCurrentTask->m_dwGroupID)
		{
			if (pNewTask->m_dwGroupID == pCurrentTask->m_dwGroupID)
				bResult = true;
		}
		else
		{
			if ((pNewTask->m_dwGroupID == BaseTask.m_dwGroupID) ||
				 !pNewTask->m_dwGroupID)
				bResult = true;
		};
	}
	else if ((pNewTask->m_dwGroupID == BaseTask.m_dwGroupID) ||
		     !pNewTask->m_dwGroupID)
		bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::AddFileToTask(const CFrameInfo & FrameInfo, DWORD dwGroupID)
{
	ZFUNCTRACE_RUNTIME();

	bool			bFound = false;

	for (LONG i = 0;i<m_vTasks.size() && !bFound;i++)
	{
		if ((m_vTasks[i].m_TaskType == FrameInfo.m_PictureType) &&
			(m_vTasks[i].m_dwGroupID == dwGroupID))
		{
			// Check ISO, gain and exposure time
			if ((m_vTasks[i].HasISOSpeed() ? (m_vTasks[i].m_lISOSpeed == FrameInfo.m_lISOSpeed) : (m_vTasks[i].m_lGain == FrameInfo.m_lGain)) &&
				AreExposureEquals(m_vTasks[i].m_fExposure,FrameInfo.m_fExposure))
			{
				bFound = true;
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
		ti.m_fAperture = FrameInfo.m_fAperture;
		ti.m_lISOSpeed = FrameInfo.m_lISOSpeed;
		ti.m_lGain     = FrameInfo.m_lGain;
		ti.m_TaskType  = FrameInfo.m_PictureType;
		ti.m_vBitmaps.push_back(FrameInfo);

		m_vTasks.push_back(ti);
	};

	if (!m_bUsingJPEG && (FrameInfo.m_strInfos.Left(4) == _T("JPEG")))
		m_bUsingJPEG = true;
	if (!m_bUsingFITS && (FrameInfo.m_strInfos.Left(4) == _T("FITS")))
		m_bUsingFITS = true;
	if (!m_bCalibrating && !FrameInfo.IsLightFrame())
		m_bCalibrating = true;
	if (!m_bUsingBayer && (FrameInfo.GetCFAType() != CFATYPE_NONE))
		m_bUsingBayer = true;
	if (!m_bUsingColorImages && (m_bUsingBayer || FrameInfo.m_lNrChannels>1))
		m_bUsingColorImages = true;

	if (FrameInfo.IsDarkFrame())
	{
		m_bDarkUsed = true;
		m_lNrDarkFrames++;
	}
	else if (FrameInfo.IsDarkFlatFrame())
	{
		m_bDarkUsed = true;
		m_lNrDarkFlatFrames++;
	}
	else if (FrameInfo.IsFlatFrame())
	{
		m_bFlatUsed = true;
		m_lNrFlatFrames++;
	}
	else if (FrameInfo.IsOffsetFrame())
	{
		m_bBiasUsed = true;
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

	CTaskInfo *			pResult = nullptr;
	LONG				j;
	bool				bExposureFirst = (TaskType == PICTURETYPE_DARKFRAME);

	if (bExposureFirst)
	{
		// Try to find in same group or in the common group if it's impossible
		// Try same ISO (gain) and same exposure
		for (j = 0;j<m_vTasks.size();j++)
		{
			if (m_vTasks[j].m_TaskType == TaskType)
			{
				if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
				{
					if ((BaseTask.HasISOSpeed() ? (BaseTask.m_lISOSpeed == m_vTasks[j].m_lISOSpeed) : (BaseTask.m_lGain == m_vTasks[j].m_lGain)) &&
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
			// Try same ISO (gain) and closest exposure
			for (j = 0;j<m_vTasks.size();j++)
			{
				if (m_vTasks[j].m_TaskType == TaskType)
				{
					if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
					{
						if (BaseTask.HasISOSpeed() ? (BaseTask.m_lISOSpeed == m_vTasks[j].m_lISOSpeed) : (BaseTask.m_lGain == m_vTasks[j].m_lGain))
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
		// Try same ISO (gain)
		for (j = 0;j<m_vTasks.size();j++)
		{
			if (m_vTasks[j].m_TaskType == TaskType)
			{
				if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
				{
					if (BaseTask.HasISOSpeed() ? (BaseTask.m_lISOSpeed == m_vTasks[j].m_lISOSpeed) : (BaseTask.m_lGain == m_vTasks[j].m_lGain))
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
			// Try closest ISO (gain)
			for (j = 0;j<m_vTasks.size();j++)
			{
				if (m_vTasks[j].m_TaskType == TaskType)
				{
					if (IsTaskGroupOk(BaseTask, pResult, &m_vTasks[j]))
					{
						if (BaseTask.HasISOSpeed())
						{
							if (m_vTasks[j].m_lISOSpeed)
							{
								if (pResult)
								{
									if (labs(pResult->m_lISOSpeed - BaseTask.m_lISOSpeed) > labs(m_vTasks[j].m_lISOSpeed - BaseTask.m_lISOSpeed))
										pResult = &m_vTasks[j];
									else if (labs(pResult->m_lISOSpeed - BaseTask.m_lISOSpeed) == labs(m_vTasks[j].m_lISOSpeed - BaseTask.m_lISOSpeed))
									{
										if (pResult->m_vBitmaps.size() < m_vTasks[j].m_vBitmaps.size())
											pResult = &m_vTasks[j];
									};
								}
								else
									pResult = &m_vTasks[j];
							};
						}
						else
						{
							if (m_vTasks[j].m_lGain >= 0)
							{
								if (pResult)
								{
									if (labs(pResult->m_lGain - BaseTask.m_lGain) > labs(m_vTasks[j].m_lGain - BaseTask.m_lGain))
										pResult = &m_vTasks[j];
									else if (labs(pResult->m_lGain - BaseTask.m_lGain) == labs(m_vTasks[j].m_lGain - BaseTask.m_lGain))
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
		};

		if (!pResult)
		{
			// Get any ISO (gain)
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
			// same ISO (gain) if possible
			// else the closest ISO (gain), else 0
			// (tie breaker is number of frames in the offset task)
			si.m_pOffsetTask = FindBestMatchingTask(m_vTasks[i], PICTURETYPE_OFFSETFRAME);

			// Try to find the best dark task for this task
			// same ISO (gain) and exposure, else same ISO (gain) and closest exposure
			// else no ISO (gain) and closest exposure
			// (tie breaker is number of frames in the dark task)
			si.m_pDarkTask = FindBestMatchingTask(m_vTasks[i], PICTURETYPE_DARKFRAME);

			// Try to find the best dark flat task for this task
			// same ISO (gain) and exposure, else same ISO (gain) and closest exposure
			// else no ISO (gain) and closest exposure
			// (tie breaker is number of frames in the dark task)
			si.m_pDarkFlatTask = FindBestMatchingTask(m_vTasks[i], PICTURETYPE_DARKFLATFRAME);

			// Try to find the best flat task for this task
			// same ISO (gain) if possible, else the closest ISO (gain), else 0
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
		m_vTasks[i].m_bDone = false;
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

	dwMethod = workspace.value("Stacking/Light_Method", (uint)MBP_AVERAGE).toUInt();
	if (dwMethod)
		LightMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	lLightIteration = workspace.value("Stacking/Light_Iteration", (uint)5).toUInt();
	fLightKappa = workspace.value("Stacking/Light_Kappa" "2.0").toDouble();


	dwMethod = workspace.value("Stacking/Dark_Method", (uint)MBP_MEDIAN).toUInt();
	if (dwMethod)
		DarkMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	lDarkIteration = workspace.value("Stacking/Dark_Iteration", (uint)5).toUInt();
	fDarkKappa = workspace.value("Stacking/Dark_Kappa", "2.0").toDouble();

	dwMethod = workspace.value("Stacking/Flat_Method", (uint)MBP_MEDIAN).toUInt();
	if (dwMethod)
		FlatMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	lFlatIteration = workspace.value("Stacking/Flat_Iteration", (uint)5).toUInt();
	fFlatKappa = workspace.value("Stacking/Flat_Kappa", "2.0").toDouble();

	dwMethod = workspace.value("Stacking/Offset_Method", (uint)MBP_MEDIAN).toUInt();
	if (dwMethod)
		OffsetMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	lOffsetIteration = workspace.value("Stacking/Offset_Iteration", (uint)5).toUInt();
	fOffsetKappa = workspace.value("Stacking/Offset_Kappa", "2.0").toDouble();

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

bool CAllStackingTasks::DoOffsetTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

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

bool CAllStackingTasks::DoDarkTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

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

bool CAllStackingTasks::DoDarkFlatTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

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

bool CAllStackingTasks::DoFlatTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

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

bool CAllStackingTasks::CheckReadOnlyStatus(std::vector<CString> & vFolders)
{
	bool						bResult = false;
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
				_tsplitpath(strFileName, szDrive, szDir, nullptr, nullptr);
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
		bool			bDirOk = true;
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
				bDirOk = false;
			fclose(hFile);
			DeleteFile(strFileName);
		}
		else
			bDirOk = false;

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
				ulOffsetSpace = max(ulOffsetSpace, static_cast<__int64>(ulSpace * m_vStacks[i].m_pOffsetTask->m_vBitmaps.size()));

			if (m_vStacks[i].m_pDarkTask)
				ulDarkSpace = max(ulDarkSpace, static_cast<__int64>(ulSpace * m_vStacks[i].m_pDarkTask->m_vBitmaps.size()));

			if (m_vStacks[i].m_pDarkFlatTask)
				ulDarkFlatSpace = max(ulDarkFlatSpace, static_cast<__int64>(ulSpace * m_vStacks[i].m_pDarkFlatTask->m_vBitmaps.size()));

			if (m_vStacks[i].m_pFlatTask)
				ulFlatSpace = max(ulFlatSpace, static_cast<__int64>(ulSpace * m_vStacks[i].m_pFlatTask->m_vBitmaps.size()));
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
	QString			strTempPath;

	GetTemporaryFilesFolder(strTempPath);

	ULARGE_INTEGER			ulFreeSpace;
	ULARGE_INTEGER			ulTotal;
	ULARGE_INTEGER			ulTotalFree;

	strDrive = CString((LPCTSTR)strTempPath.utf16());
	strDrive = strDrive.Left(2);

	GetDiskFreeSpaceEx(CString((LPCTSTR)strTempPath.utf16()), &ulFreeSpace, &ulTotal, &ulTotalFree);

	return ulFreeSpace.QuadPart;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::GetTemporaryFilesFolder(QString & strFolder)
{

	QSettings	settings;

	QString strTemp = settings.value("Stacking/TemporaryFilesFolder", QString("")).toString();
	if (strTemp.length())
	{
		// Check that the folder exists by creating an file in it
		FILE *			hFile;
		QString			strFile;

		strFile = strTemp;
		strFile += "Temp.txt";

		hFile = _tfopen((LPCTSTR)strFile.utf16(), _T("wb"));
		if (hFile)
		{
			fclose(hFile);
			DeleteFile((LPCTSTR)strFile.utf16());
		}
		else
			strTemp = "";
	};

	if (strTemp.isEmpty())
	{
		TCHAR			szTempPath[1+_MAX_PATH] = _T("");

		GetTempPath(sizeof(szTempPath)/sizeof(TCHAR), szTempPath);

		CString temp(szTempPath);
		strTemp = QString::fromWCharArray(temp.GetBuffer());
	};

	strFolder = strTemp;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::SetTemporaryFilesFolder(QString strFolder)
{
	ZFUNCTRACE_RUNTIME();

	QSettings settings;

	if ((strFolder.right(1) != "\\") && (strFolder.right(1) != "/"))
		strFolder += "\\";

	settings.setValue("Stacking/TemporaryFilesFolder", strFolder);
};

/* ------------------------------------------------------------------- */

BACKGROUNDCALIBRATIONMODE	CAllStackingTasks::GetBackgroundCalibrationMode()
{
	CWorkspace			workspace;

	bool backgroundCalibration = workspace.value("Stacking/BackgroundCalibration", true).toBool();
	bool perChannelCalibration = workspace.value("Stacking/PerChannelBackgroundCalibration", false).toBool();

	if (backgroundCalibration)
		return BCM_RGB;
	else if (perChannelCalibration)
		return BCM_PERCHANNEL;
	else
		return BCM_NONE;
};

/* ------------------------------------------------------------------- */

BACKGROUNDCALIBRATIONINTERPOLATION	CAllStackingTasks::GetBackgroundCalibrationInterpolation()
{
	CWorkspace			workspace;
	int				interpolation = (int)BCI_RATIONAL;

	interpolation = workspace.value("Stacking/BackgroundCalibrationInterpolation").toUInt();

	return (BACKGROUNDCALIBRATIONINTERPOLATION)interpolation;
};

/* ------------------------------------------------------------------- */

RGBBACKGROUNDCALIBRATIONMETHOD	CAllStackingTasks::GetRGBBackgroundCalibrationMethod()
{
	CWorkspace			workspace;

	int method = workspace.value("Stacking/RGBBackgroundCalibrationMethod", (int)RBCM_MAXIMUM).toUInt();

	return (RGBBACKGROUNDCALIBRATIONMETHOD)method;
};

/* ------------------------------------------------------------------- */

bool	CAllStackingTasks::GetDarkOptimization()
{
	CWorkspace			workspace;

	bool value = workspace.value("Stacking/DarkOptimization", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

double	CAllStackingTasks::GetDarkFactor()
{
	double				value = 1.0;
	CWorkspace			workspace;

	if (workspace.value("Stacking/UseDarkFactor", false).toBool())
	{
		value = workspace.value("Stacking/DarkFactor").toDouble();
	};

	return value;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetHotPixelsDetection()
{
	CWorkspace			workspace;

	bool value = workspace.value("Stacking/HotPixelsDetection", true).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetBadLinesDetection()
{
	CWorkspace			workspace;

	bool value = workspace.value("Stacking/BadLinesDetection", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

STACKINGMODE	CAllStackingTasks::GetResultMode()
{
	STACKINGMODE		Result = SM_NORMAL;
	CWorkspace			workspace;

	int value = workspace.value("Stacking/Mosaic", 0).toUInt();
	if (value==2)
		Result = SM_INTERSECTION;
	else if (value==1)
		Result = SM_MOSAIC;

	return Result;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetCreateIntermediates()
{
	CWorkspace			workspace;

	bool value = workspace.value("Stacking/CreateIntermediates", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetSaveCalibrated()
{
	CWorkspace			workspace;

	bool value = workspace.value("Stacking/SaveCalibrated", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetSaveCalibratedDebayered()
{
	CWorkspace			workspace;

	bool value = workspace.value("Stacking/SaveCalibratedDebayered", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

WORD	CAllStackingTasks::GetAlignmentMethod()
{
	CWorkspace			workspace;

	WORD value = workspace.value("Stacking/AlignmentTransformation", 0).toUInt();

	return value;
};

/* ------------------------------------------------------------------- */

LONG	CAllStackingTasks::GetPixelSizeMultiplier()
{
	CWorkspace			workspace;

	LONG value = workspace.value("Stacking/PixelSizeMultiplier", 1).toUInt();

	return value;
};

/* ------------------------------------------------------------------- */

bool	CAllStackingTasks::GetChannelAlign()
{
	CWorkspace			workspace;

	return workspace.value("Stacking/AlignChannels", false).toBool();
};

/* ------------------------------------------------------------------- */

bool	CAllStackingTasks::GetSaveIntermediateCometImages()
{
	CWorkspace			workspace;

	bool value = workspace.value("Stacking/SaveCometImages", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

bool	CAllStackingTasks::GetApplyMedianFilterToCometImage()
{
	CWorkspace			workspace;

	bool value = workspace.value("Stacking/ApplyFilterToCometImages", true).toBool();

	return false; //value;
};

/* ------------------------------------------------------------------- */

INTERMEDIATEFILEFORMAT CAllStackingTasks::GetIntermediateFileFormat()
{
	CWorkspace			workspace;

	int value = workspace.value("Stacking/IntermediateFileFormat", 1).toUInt();

	if (value != IFF_TIFF && value != IFF_FITS)
		value = IFF_TIFF;

	return (INTERMEDIATEFILEFORMAT)value;
};

/* ------------------------------------------------------------------- */

COMETSTACKINGMODE CAllStackingTasks::GetCometStackingMode()
{
	CWorkspace			workspace;

	int value = workspace.value("Stacking/CometStackingMode", 0).toUInt();

	if (value != CSM_STANDARD && value != CSM_COMETONLY)
		value = CSM_COMETSTAR;

	return (COMETSTACKINGMODE)value;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::GetPostCalibrationSettings(CPostCalibrationSettings & pcs)
{
	CWorkspace			workspace;
	
	pcs.m_bHot = workspace.value("Stacking/PCS_DetectCleanHot", false).toBool();

	pcs.m_lHotFilter = workspace.value("Stacking/PCS_HotFilter", 1).toUInt();

	pcs.m_fHotDetection = workspace.value("Stacking/PCS_HotDetection", 500.0).toDouble()/10.0;

	pcs.m_bCold = workspace.value("Stacking/PCS_DetectCleanCold", false).toBool();

	pcs.m_lColdFilter = workspace.value("Stacking/PCS_ColdFilter", 1L).toUInt();

	pcs.m_fColdDetection = workspace.value("Stacking/PCS_ColdDetection", 500.0).toDouble()/10.0;
	 
	pcs.m_bSaveDeltaImage = workspace.value("Stacking/PCS_SaveDeltaImage", false).toBool();

	pcs.m_Replace = static_cast<COSMETICREPLACE>(workspace.value("Stacking/PCS_ReplaceMethod", (int)CR_MEDIAN).toInt());
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::SetPostCalibrationSettings(const CPostCalibrationSettings & pcs)
{
	CWorkspace			workspace;

	workspace.setValue("Stacking/PCS_DetectCleanHot", pcs.m_bHot);

	workspace.setValue("Stacking/PCS_HotFilter", pcs.m_lHotFilter);

	workspace.setValue("Stacking/PCS_HotDetection", pcs.m_fHotDetection*10.0);

	workspace.setValue("Stacking/PCS_DetectCleanCold", pcs.m_bCold);

	workspace.setValue("Stacking/PCS_ColdFilter", pcs.m_lColdFilter);

	workspace.setValue("Stacking/PCS_ColdDetection", pcs.m_fColdDetection*10.0);

	workspace.setValue("Stacking/PCS_SaveDeltaImage", pcs.m_bSaveDeltaImage);

	workspace.setValue("Stacking/PCS_ReplaceMethod", (int)pcs.m_Replace);
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::GetOutputSettings(COutputSettings & os)
{
	QSettings settings;

	os.m_bOutput = settings.value("Output/Output", true).toBool();

	os.m_bOutputHTML = settings.value("Output/OutputHTML", false).toBool();

	bool temp = settings.value("Output/FileName", false).toBool();
	os.m_bAutosave = !temp;
	os.m_bFileList = temp;

	os.m_bAppend = settings.value("Output/AppendNumber", true).toBool();

	int tempInt = settings.value("Output/OutputFolder", 0).toUInt();
	os.m_bRefFrameFolder = (tempInt == 0);
	os.m_bFileListFolder = (tempInt == 1);
	os.m_bOtherFolder	 = (tempInt == 2);

	os.m_strFolder = settings.value("Output/OutputFolderName").toString();
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::SetOutputSettings(const COutputSettings & os)
{
	QSettings settings;

	settings.setValue("Output/Output", os.m_bOutput);

	settings.setValue("Output/OutputHTML", os.m_bOutputHTML);

	// 
	// Save value of false if m_bAutosave is true
	//
	settings.setValue("Output/FileName", (os.m_bAutosave ? false : true));

	settings.setValue("Output/AppendNumber",  os.m_bAppend);

	int	tempInt;
	if (os.m_bRefFrameFolder)
		tempInt = 0;
	else if (os.m_bFileListFolder)
		tempInt = 1;
	else
		tempInt = 2;
	settings.setValue("Output/OutputFolder", tempInt);

	settings.setValue("Output/OutputFolderName", os.m_strFolder);
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::ClearCache()
{
	g_BitmapCache.ClearCache();
};

/* ------------------------------------------------------------------- */
