#include <stdafx.h>
#include "StackingTasks.h"

#include "TIFFUtil.h"
#include <set>
#include "Settings.h"
#include <QSettings>
#include <QRectF>

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

bool LoadFrame(LPCTSTR szFile, PICTURETYPE PictureType, CDSSProgress * pProgress, std::shared_ptr<CMemoryBitmap>& rpBitmap)
{
	ZFUNCTRACE_RUNTIME();

	bool bResult = false;
	CBitmapInfo bmpInfo;

	if (GetPictureInfo(szFile, bmpInfo) && bmpInfo.CanLoad())
	{
		QString strText;
		CString strDescription;
		bool bOverrideRAW = true;

		bmpInfo.GetDescription(strDescription);
		const auto pDescription = static_cast<LPCTSTR>(strDescription);

		switch (PictureType)
		{
		case PICTURETYPE_DARKFRAME:
			if (bmpInfo.m_lNrChannels==3)
				strText = QObject::tr("Loading %1 bit/ch %2 dark flat frame\n%3", "IDS_LOADRGBDARK").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			else
				strText = QObject::tr("Loading %1 bits gray %2 dark frame\n%3", "IDS_LOADGRAYDARK").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			break;
		case PICTURETYPE_DARKFLATFRAME:
			if (bmpInfo.m_lNrChannels == 3)
				strText = QObject::tr("Loading %1 bit/ch %2 dark flat frame\n%3", "IDS_LOADRGBDARKFLAT").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			else
				strText = QObject::tr("Loading %1 bits gray %2 dark flat frame\n%3", "IDS_LOADGRAYDARKFLAT").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			break;
		case PICTURETYPE_OFFSETFRAME:
			if (bmpInfo.m_lNrChannels == 3)
				strText = QObject::tr("Loading %1 bit/ch %2 offset frame\n%3", "IDS_LOADRGBOFFSET").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			else
				strText = QObject::tr("Loading %1 bits gray %2 offset frame\n%3", "IDS_LOADGRAYOFFSET").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			break;
		case PICTURETYPE_FLATFRAME:
			if (bmpInfo.m_lNrChannels == 3)
				strText = QObject::tr("Loading %1 bit/ch %2 flat frame\n%3", "IDS_LOADRGBFLAT").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			else
				strText = QObject::tr("Loading %1 bits gray %2 flat frame\n%3", "IDS_LOADGRAYFLAT").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			break;
		case PICTURETYPE_LIGHTFRAME:
			if (bmpInfo.m_lNrChannels == 3)
				strText = QObject::tr("Loading %1 bit/ch %2 light frame\n%3", "IDS_LOADRGBLIGHT").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			else
				strText = QObject::tr("Loading %1 bits gray %2 light frame\n%3", "IDS_LOADGRAYLIGHT").arg(bmpInfo.m_lBitPerChannel).arg(pDescription).arg(szFile);
			bOverrideRAW = false;
			break;
		};

		if (pProgress)
			pProgress->Start2(strText, 0);

		if (bOverrideRAW)
			PushRAWSettings(false, true); // Allways use Raw Bayer for dark, offset, and flat frames

		bResult = ::FetchPicture(szFile, rpBitmap, pProgress);
//#			bResult = pBitmap.CopyTo(ppBitmap); // *ppBitmap = pBitmap.m_p; pBitmap.m_p->Addref();

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
	std::uint32_t				m_dwOffsetTaskID;
	std::shared_ptr<CMemoryBitmap> m_pOffsetBitmap;
	std::uint32_t				m_dwDarkTaskID;
	std::shared_ptr<CMemoryBitmap> m_pDarkBitmap;
	std::uint32_t				m_dwDarkFlatTaskID;
	std::shared_ptr<CMemoryBitmap> m_pDarkFlatBitmap;
	std::uint32_t				m_dwFlatTaskID;
	std::shared_ptr<CMemoryBitmap> m_pFlatBitmap;

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
		m_pOffsetBitmap.reset();
		m_pDarkBitmap.reset();
		m_pDarkFlatBitmap.reset();
		m_pFlatBitmap.reset();
	};

	bool GetTaskResult(const CTaskInfo* pTaskInfo, CDSSProgress* pProgress, std::shared_ptr<CMemoryBitmap>& rpBitmap)
	{
		ZFUNCTRACE_RUNTIME();
		bool bResult = false;

		const auto checkFrame = [&rpBitmap, pTaskInfo, pProgress](std::uint32_t& taskId, std::shared_ptr<CMemoryBitmap>& pSrcBitmap) -> bool
		{
			if (taskId == pTaskInfo->m_dwTaskID && static_cast<bool>(pSrcBitmap))
			{
				rpBitmap = pSrcBitmap;
				return true;
			}
			else
			{
				pSrcBitmap.reset();
				if (LoadFrame(pTaskInfo->m_strOutputFile, pTaskInfo->m_TaskType, pProgress, pSrcBitmap))
				{
					taskId = pTaskInfo->m_dwTaskID;
					rpBitmap = pSrcBitmap;
					return true;
				}
				else
				{
					taskId = 0;
					return false;
				}
			}
		};

//#		*ppBitmap = nullptr;
		rpBitmap.reset();
		if (pTaskInfo != nullptr && pTaskInfo->m_strOutputFile.GetLength() != 0)
		{
			switch (pTaskInfo->m_TaskType)
			{
			case PICTURETYPE_OFFSETFRAME :
				bResult = checkFrame(this->m_dwOffsetTaskID, this->m_pOffsetBitmap);
				break;
/*				if ((m_dwOffsetTaskID == pTaskInfo->m_dwTaskID) && m_pOffsetBitmap)
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
						m_dwDarkTaskID = 0; // ### BUG! m_dwOffsetTaskID ###
				};
				break;*/
			case PICTURETYPE_DARKFRAME :
				bResult = checkFrame(this->m_dwDarkTaskID, this->m_pDarkBitmap);
				break;
/*				if ((m_dwDarkTaskID == pTaskInfo->m_dwTaskID) && m_pDarkBitmap)
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
				break;*/
			case PICTURETYPE_DARKFLATFRAME :
				bResult = checkFrame(this->m_dwDarkFlatTaskID, this->m_pDarkFlatBitmap);
				break;
/*				if ((m_dwDarkFlatTaskID == pTaskInfo->m_dwTaskID) && m_pDarkFlatBitmap)
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
				break;*/
			case PICTURETYPE_FLATFRAME :
				bResult = checkFrame(this->m_dwFlatTaskID, this->m_pFlatBitmap);
				break;
/*				if ((m_dwFlatTaskID == pTaskInfo->m_dwTaskID) && m_pFlatBitmap)
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
				break;*/
			}
		}

		return bResult;
	}
};

static CTaskBitmapCache g_BitmapCache;

/* ------------------------------------------------------------------- */

bool GetTaskResult(const CTaskInfo* pTaskInfo, CDSSProgress* pProgress, std::shared_ptr<CMemoryBitmap>& rpBitmap)
{
	return ::g_BitmapCache.GetTaskResult(pTaskInfo, pProgress, rpBitmap);
}

void ClearTaskCache()
{
	g_BitmapCache.ClearCache();
}

/* ------------------------------------------------------------------- */

static void BuildMasterFileNames(CTaskInfo *pTaskInfo, TCHAR const *pszType, bool bExposure, TCHAR const *pszDrive, TCHAR const *pszDir,
	CString *pstrMasterFile, CString *pstrMasterInfoFile)
{
	TCHAR const *pszISOGain = pTaskInfo->HasISOSpeed() ? _T("ISO") : _T("Gain");
	int const lISOGain = pTaskInfo->HasISOSpeed() ? pTaskInfo->m_lISOSpeed : pTaskInfo->m_lGain;

	CString strFileName;
	if (bExposure)
		strFileName.Format(_T("%s%s%s_%s%ld_%lds"),
			pszDrive, pszDir, pszType, pszISOGain, lISOGain, (int)pTaskInfo->m_fExposure);
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

		_tsplitpath(m_pOffsetTask->m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

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

bool CStackingInfo::DoOffsetTask(CDSSProgress* const pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

	if (!m_pOffsetTask->m_bDone)
	{
		ZASSERT(m_pOffsetTask->m_TaskType == PICTURETYPE_OFFSETFRAME);
		if (m_pOffsetTask->m_vBitmaps.size() == 1)
		{
			m_pOffsetTask->m_strOutputFile = m_pOffsetTask->m_vBitmaps[0].filePath.c_str();
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
			QString strText(QObject::tr("Create Master Offset Frame", "IDS_CREATEMASTEROFFSET"));
			ZTRACE_RUNTIME(strText);

			if (pProgress)
				pProgress->Start(strText, (int)m_pOffsetTask->m_vBitmaps.size(), true);

			for (size_t i = 0; i < m_pOffsetTask->m_vBitmaps.size() && bResult; i++)
			{
				std::shared_ptr<CMemoryBitmap> pBitmap;

				strText = QObject::tr("Adding Offset frame %1 of %2", "IDS_ADDOFFSET").arg(static_cast<int>(i)).arg(m_pOffsetTask->m_vBitmaps.size());
				ZTRACE_RUNTIME(strText);

				if (pProgress)
					pProgress->Progress1(strText, static_cast<int>(i));

				if (::LoadFrame(m_pOffsetTask->m_vBitmaps[i].filePath.c_str(), PICTURETYPE_OFFSETFRAME, pProgress, pBitmap))
				{
					// Load the bitmap
					if (!m_pOffsetTask->m_pMaster)
						m_pOffsetTask->CreateEmptyMaster(pBitmap.get());

					m_pOffsetTask->AddToMaster(pBitmap.get(), pProgress);
				};

				if (pProgress)
					bResult = !pProgress->IsCanceled();
			}

			if (bResult)
			{
				// Save the resulting master offset
				CString strMasterOffset;
				CString strMasterOffsetInfo;
				QString strMethod;
				FormatFromMethod(strMethod, m_pOffsetTask->m_Method, m_pOffsetTask->m_fKappa, m_pOffsetTask->m_lNrIterations);
				strText = QObject::tr("Computing master offset (%1)", "IDS_COMPUTINGMEDIANOFFSET").arg(strMethod);
				ZTRACE_RUNTIME(strText);

				if (pProgress != nullptr)
				{
					pProgress->Start(strText, 1, false);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(true);
				}

				std::shared_ptr<CMemoryBitmap> pOffsetBitmap = m_pOffsetTask->GetMaster(pProgress);
				if (pProgress != nullptr)
					pProgress->SetJointProgress(false);
				if (static_cast<bool>(pOffsetBitmap))
				{
					TCHAR szDrive[1+_MAX_DRIVE];
					TCHAR szDir[1+_MAX_DIR];
					const QString strInfo(QObject::tr("Master Offset created from %ld pictures (%1)", "IDS_MEDIANOFFSETINFO").arg(m_pOffsetTask->m_vBitmaps.size()).arg(strMethod));
					_tsplitpath(m_pOffsetTask->m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

					BuildMasterFileNames(m_pOffsetTask, _T("MasterOffset"), /* bExposure */false, szDrive, szDir, &strMasterOffset, &strMasterOffsetInfo);

					strText = QObject::tr("Saving Master Offset", "IDS_SAVINGMASTEROFFSET");
					ZTRACE_RUNTIME(strText);

					if (pProgress != nullptr)
					{
						pProgress->Start(strText, 1, false);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(QString::fromWCharArray(strMasterOffset.GetString()), 0);
					}

					// TODO: Work out how to do this better.
					const CString strInfo2(strInfo.toStdWString().c_str());
					WriteMasterTIFF(strMasterOffset, pOffsetBitmap.get(), pProgress, strInfo2, m_pOffsetTask);

					m_pOffsetTask->m_strOutputFile = strMasterOffset;
					m_pOffsetTask->m_bDone = true;

					// Save the description
					COffsetSettings s;
					s.InitFromCurrent(m_pOffsetTask, strMasterOffset);
					s.WriteToFile(strMasterOffsetInfo);
				}
			}
		}
	}

	return bResult;
}

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
			int			lExposure = m_pDarkTask->m_fExposure;

			_tsplitpath(m_pDarkTask->m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

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

bool	CStackingInfo::DoDarkTask(CDSSProgress* const pProgress)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = true;

	if (!m_pDarkTask->m_bDone)
	{
		ZASSERT(m_pDarkTask->m_TaskType == PICTURETYPE_DARKFRAME);

		if (m_pDarkTask->m_vBitmaps.size() == 1)
		{
			m_pDarkTask->m_strOutputFile = m_pDarkTask->m_vBitmaps[0].filePath.c_str();
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
			QString strText;
			std::shared_ptr<CMemoryBitmap> pMasterOffset;

			strText = QObject::tr("Create Master Dark Frame", "IDS_CREATEMASTERDARK");
			ZTRACE_RUNTIME(strText);

			if (pProgress)
				pProgress->Start(strText, (int)m_pDarkTask->m_vBitmaps.size(), true);

			// First load the master offset if available
			if (m_pOffsetTask)
				g_BitmapCache.GetTaskResult(m_pOffsetTask, pProgress, pMasterOffset);

			// First Add Dark frame
			for (size_t i = 0; i < m_pDarkTask->m_vBitmaps.size() && bResult; i++)
			{
				std::shared_ptr<CMemoryBitmap> pBitmap;

				strText = QString(QObject::tr("Adding Dark frame %1 of %2", "IDS_ADDDARK")).arg(static_cast<int>(i)).arg(m_pDarkTask->m_vBitmaps.size());
				ZTRACE_RUNTIME(strText);

				if (pProgress)
					pProgress->Progress1(strText, static_cast<int>(i));

				if (::LoadFrame(m_pDarkTask->m_vBitmaps[i].filePath.c_str(), PICTURETYPE_DARKFRAME, pProgress, pBitmap))
				{
					if (!m_pDarkTask->m_pMaster)
						m_pDarkTask->CreateEmptyMaster(pBitmap.get());

					// Subtract the offset frame from the dark frame
					if (static_cast<bool>(pMasterOffset) && !pBitmap->IsMaster())
					{
						QString strStart2;
						if (pProgress != nullptr)
						{
							QString strText;
							strStart2 = pProgress->GetStart2Text();
							strText = QObject::tr("Subtracting Offset Frame", "IDS_SUBSTRACTINGOFFSET");
							ZTRACE_RUNTIME(strText);
							pProgress->Start2(strText, 0);
						}
						Subtract(pBitmap, pMasterOffset, pProgress);
						if (pProgress)
							pProgress->Start2(strStart2, 0);
					};

					// Add the dark frame
					m_pDarkTask->AddToMaster(pBitmap.get(), pProgress);
				};

				if (pProgress)
					bResult = !pProgress->IsCanceled();
			};

			if (bResult)
			{
				// Save Master Dark Frame
				CString strMasterDark;
				CString strMasterDarkInfo;
				QString strMethod;
				FormatFromMethod(strMethod, m_pDarkTask->m_Method, m_pDarkTask->m_fKappa, m_pDarkTask->m_lNrIterations);
				strText = QString(QObject::tr("Computing master dark (%1)", "IDS_COMPUTINGMEDIANDARK")).arg(strMethod);
				ZTRACE_RUNTIME(strText);

				if (pProgress != nullptr)
				{
					pProgress->Start(strText, 1, false);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(true);
					pProgress->Start2(strText, 0);
				}
				std::shared_ptr<CMemoryBitmap> pDarkBitmap = m_pDarkTask->GetMaster(pProgress);
				if (pProgress != nullptr)
					pProgress->SetJointProgress(false);
				if (static_cast<bool>(pDarkBitmap))
				{
					TCHAR szDrive[1+_MAX_DRIVE];
					TCHAR szDir[1+_MAX_DIR];
					QString strInfo = QString(QObject::tr("Master Dark created from %1 pictures (%2)", "IDS_MEDIANDARKINFO")).arg(m_pDarkTask->m_vBitmaps.size()).arg(strMethod);

					_tsplitpath(m_pDarkTask->m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

					BuildMasterFileNames(m_pDarkTask, _T("MasterDark"), /* bExposure */ true, szDrive, szDir,
						&strMasterDark, &strMasterDarkInfo);

					strText = QObject::tr("Saving Master Dark", "IDS_SAVINGMASTERDARK");
					ZTRACE_RUNTIME(strText);

					if (pProgress != nullptr)
					{
						pProgress->Start(strText, 1, false);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(QString::fromWCharArray(strMasterDark), 0);
					}

					CString strInfo2(strInfo.toStdWString().c_str());
					WriteMasterTIFF(strMasterDark, pDarkBitmap.get(), pProgress, strInfo2, m_pDarkTask);

					m_pDarkTask->m_strOutputFile = strMasterDark;
					m_pDarkTask->m_bDone = true;

					// Save the description
					CDarkSettings s;
					s.InitFromCurrent(m_pDarkTask, strMasterDark);
					s.SetMasterOffset(m_pOffsetTask);
					s.WriteToFile(strMasterDarkInfo);
				}
			}
		}
	}

	return bResult;
}

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
			int			lExposure = m_pDarkFlatTask->m_fExposure;

			_tsplitpath(m_pDarkFlatTask->m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

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

bool	CStackingInfo::DoDarkFlatTask(CDSSProgress* const pProgress)
{
	ZFUNCTRACE_RUNTIME();

	bool bResult = true;

	if (!m_pDarkFlatTask->m_bDone)
	{
		ZASSERT(m_pDarkFlatTask->m_TaskType == PICTURETYPE_DARKFLATFRAME);

		if (m_pDarkFlatTask->m_vBitmaps.size() == 1)
		{
			m_pDarkFlatTask->m_strOutputFile = m_pDarkFlatTask->m_vBitmaps[0].filePath.c_str();
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
			std::shared_ptr<CMemoryBitmap> pMasterOffset;
			QString strText(QObject::tr("Create Master Dark Flat Frame", "IDS_CREATEMASTERDARKFLAT"));

			ZTRACE_RUNTIME(strText);

			if (pProgress)
				pProgress->Start(strText, (int)m_pDarkFlatTask->m_vBitmaps.size(), true);

			// First load the master offset if available
			if (m_pOffsetTask)
				g_BitmapCache.GetTaskResult(m_pOffsetTask, pProgress, pMasterOffset);

			// First Add Dark flat frame
			for (size_t i = 0; i < m_pDarkFlatTask->m_vBitmaps.size() && bResult; i++)
			{
				std::shared_ptr<CMemoryBitmap> pBitmap;

				strText = QString(QObject::tr("Adding Dark Flat frame %1 of %2", "IDS_ADDDARKFLAT")).arg(static_cast<int>(i)).arg(m_pDarkFlatTask->m_vBitmaps.size());
				ZTRACE_RUNTIME(strText);

				if (pProgress)
					pProgress->Progress1(strText, static_cast<int>(i));

				if (::LoadFrame(m_pDarkFlatTask->m_vBitmaps[i].filePath.c_str(), PICTURETYPE_DARKFLATFRAME, pProgress, pBitmap))
				{
					if (!m_pDarkFlatTask->m_pMaster)
						m_pDarkFlatTask->CreateEmptyMaster(pBitmap.get());

					// Subtract the offset frame from the dark frame
					if (static_cast<bool>(pMasterOffset) && !pBitmap->IsMaster())
					{
						QString strStart2;
						if (pProgress != nullptr)
						{
							QString strText;
							strStart2 = pProgress->GetStart2Text();
							strText = QObject::tr("Subtracting Offset Frame", "IDS_SUBSTRACTINGOFFSET");
							ZTRACE_RUNTIME(strText);

							pProgress->Start2(strText, 0);
						}
						Subtract(pBitmap, pMasterOffset, pProgress);
						if (pProgress)
							pProgress->Start2(strStart2, 0);
					}

					// Add the dark frame
					m_pDarkFlatTask->AddToMaster(pBitmap.get(), pProgress);
				}

				if (pProgress)
					bResult = !pProgress->IsCanceled();
			}

			if (bResult)
			{
				// Save Master Dark Frame
				CString						strMasterDarkFlat;
				CString						strMasterDarkFlatInfo;
				QString						strMethod;

				FormatFromMethod(strMethod, m_pDarkFlatTask->m_Method, m_pDarkFlatTask->m_fKappa, m_pDarkFlatTask->m_lNrIterations);
				strText = QString(QObject::tr("Computing master dark flat (%1)", "IDS_COMPUTINGMEDIANDARKFLAT")).arg(strMethod);
				ZTRACE_RUNTIME(strText);

				if (pProgress)
				{
					pProgress->Start(strText, 1, false);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(true);
					pProgress->Start2(strText, 0);
				};
				std::shared_ptr<CMemoryBitmap> pDarkFlatBitmap = m_pDarkFlatTask->GetMaster(pProgress);
				if (pProgress)
					pProgress->SetJointProgress(false);
				if (static_cast<bool>(pDarkFlatBitmap))
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					QString			strInfo;

					strInfo = QString(QObject::tr("Master Dark Flat created from %1 pictures (%2)", "IDS_MEDIANDARKFLATINFO")).arg(m_pDarkFlatTask->m_vBitmaps.size()).arg(strMethod);

					_tsplitpath(m_pDarkFlatTask->m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

					BuildMasterFileNames(m_pDarkFlatTask, _T("MasterDarkFlat"), /* bExposure */ true, szDrive, szDir,
						&strMasterDarkFlat, &strMasterDarkFlatInfo);
					strText = QObject::tr("Saving Master Dark Flat", "IDS_SAVINGMASTERDARKFLAT");
					ZTRACE_RUNTIME(strText);

					if (pProgress)
					{
						pProgress->Start(strText, 1, false);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(QString::fromWCharArray(strMasterDarkFlat.GetString()), 0);
					};

					CString strInfo2(strInfo.toStdWString().c_str());
					WriteMasterTIFF(strMasterDarkFlat, pDarkFlatBitmap.get(), pProgress, strInfo2, m_pDarkFlatTask);

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
	int				m_lNrValues;

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

	void	ComputeParameters(CMemoryBitmap* pBitmap, CDSSProgress * pProgress);
	void	ApplyParameters(CMemoryBitmap * pBitmap, const CFlatCalibrationParameters & fcp, CDSSProgress * pProgress);
};

/* ------------------------------------------------------------------- */

void	CFlatCalibrationParameters::ComputeParameters(CMemoryBitmap* pBitmap, CDSSProgress * pProgress)
{
	QString			strStart2;

	if (pProgress)
	{
		QString			strText;

		strStart2 = pProgress->GetStart2Text();
		strText = QObject::tr("Computing Flat Calibration Parameters", "IDS_COMPUTINGFLATCALIBRATION");
		ZTRACE_RUNTIME(strText);

		pProgress->Start2(strText, 0);
		pProgress->Start2(nullptr, pBitmap->RealWidth());
	};

	for (int i = 0;i<pBitmap->RealWidth();i++)
	{
		for (int j = 0;j<pBitmap->RealHeight();j++)
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
	QString			strStart2;
	if (pProgress)
	{
		QString			strText;

		strStart2 = pProgress->GetStart2Text();
		strText = QObject::tr("Applying Flat Calibration Parameters", "IDS_APPLYINGFLATCALIBRATION");
		ZTRACE_RUNTIME(strText);

		pProgress->Start2(strText, 0);
		pProgress->Start2(nullptr, pBitmap->RealWidth());
	};

	for (int i = 0;i<pBitmap->RealWidth();i++)
	{
		for (int j = 0;j<pBitmap->RealHeight();j++)
		{
			if (pBitmap->IsMonochrome())
			{
				double			fGray;

				pBitmap->GetPixel(i, j, fGray);
				fGray = fcp.m_vStats[(int)pBitmap->GetBayerColor(i, j)].Normalize(fGray);
				fGray = m_vStats[(int)pBitmap->GetBayerColor(i, j)].NormalizeInvert(fGray);
				AdjustValue(fGray);
				pBitmap->SetPixel(i, j, fGray);
			}
			else
			{
				double			fRed, fGreen, fBlue;

				pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
				fRed	= fcp.m_vStats[(int)BAYER_RED].Normalize(fRed);
				fGreen	= fcp.m_vStats[(int)BAYER_GREEN].Normalize(fGreen);
				fBlue	= fcp.m_vStats[(int)BAYER_BLUE].Normalize(fBlue);
				fRed	= m_vStats[(int)BAYER_RED].NormalizeInvert(fRed);
				fGreen	= m_vStats[(int)BAYER_GREEN].NormalizeInvert(fGreen);
				fBlue	= m_vStats[(int)BAYER_BLUE].NormalizeInvert(fBlue);
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

			_tsplitpath(m_pFlatTask->m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

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

bool	CStackingInfo::DoFlatTask(CDSSProgress* const pProgress)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = true;

	if (!m_pFlatTask->m_bDone)
	{
		ZASSERT(m_pFlatTask->m_TaskType == PICTURETYPE_FLATFRAME);

		if (m_pFlatTask->m_vBitmaps.size() == 1)
		{
			m_pFlatTask->m_strOutputFile = m_pFlatTask->m_vBitmaps[0].filePath.c_str();
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
			QString strText;
			std::shared_ptr<CMemoryBitmap> pMasterOffset;
			std::shared_ptr<CMemoryBitmap> pMasterDarkFlat;
			CFlatCalibrationParameters fcpBase;

			strText = QObject::tr("Create Master Flat Frame", "IDS_CREATEMASTERFLAT");
			ZTRACE_RUNTIME(strText);

			if (pProgress != nullptr)
				pProgress->Start(strText, static_cast<int>(m_pFlatTask->m_vBitmaps.size()), true);

			// First load the master offset if available
			if (m_pOffsetTask)
				g_BitmapCache.GetTaskResult(m_pOffsetTask, pProgress, pMasterOffset);
			if (m_pDarkFlatTask)
				g_BitmapCache.GetTaskResult(m_pDarkFlatTask, pProgress, pMasterDarkFlat);

			for (size_t i = 0; i < m_pFlatTask->m_vBitmaps.size() && bResult; i++)
			{
				std::shared_ptr<CMemoryBitmap> pBitmap;

				strText = QString(QObject::tr("Adding Flat frame %1 of %2", "IDS_ADDFLAT")).arg(static_cast<int>(i)).arg(m_pFlatTask->m_vBitmaps.size());
				ZTRACE_RUNTIME(strText);

				if (pProgress)
					pProgress->Progress1(strText, static_cast<int>(i));

				if (::LoadFrame(m_pFlatTask->m_vBitmaps[i].filePath.c_str(), PICTURETYPE_FLATFRAME, pProgress, pBitmap))
				{
					CFlatCalibrationParameters fcpBitmap;
					if (!m_pFlatTask->m_pMaster)
						m_pFlatTask->CreateEmptyMaster(pBitmap.get());

					// Subtract the offset frame from the dark frame
					if (static_cast<bool>(pMasterOffset) && !pBitmap->IsMaster())
					{
						QString strText;
						QString strStart2;

						strText = QObject::tr("Subtracting Offset Frame", "IDS_SUBSTRACTINGOFFSET");
						ZTRACE_RUNTIME(strText);

						if (pProgress != nullptr)
						{
							strStart2 = pProgress->GetStart2Text();
							pProgress->Start2(strText, 0);
						}
						Subtract(pBitmap, pMasterOffset, pProgress);
						if (pProgress != nullptr)
							pProgress->Start2(strStart2, 0);
					}

					if (static_cast<bool>(pMasterDarkFlat) && !pBitmap->IsMaster())
					{
						QString strText;
						QString strStart2;

						strText = QObject::tr("Subtracting Dark Frame", "IDS_SUBSTRACTINGDARK");
						ZTRACE_RUNTIME(strText);

						if (pProgress != nullptr)
						{
							strStart2 = pProgress->GetStart2Text();
							pProgress->Start2(strText, 0);
						}
						Subtract(pBitmap, pMasterDarkFlat, pProgress);
						if (pProgress != nullptr)
							pProgress->Start2(strStart2, 0);
					}

					if (!fcpBase.IsInitialized())
					{
						// This is the first flat
						fcpBase.ComputeParameters(pBitmap.get(), pProgress);
					}
					else
					{
						fcpBitmap.ComputeParameters(pBitmap.get(), pProgress);
						fcpBase.ApplyParameters(pBitmap.get(), fcpBitmap, pProgress);
					}

					// Add the dark frame
					m_pFlatTask->AddToMaster(pBitmap.get(), pProgress);
				};

				if (pProgress)
					bResult = !pProgress->IsCanceled();
			};

			if (bResult)
			{
				// Save Master Flat Frame
				CString						strMasterFlat;
				CString						strMasterFlatInfo;
				QString						strMethod;

				FormatFromMethod(strMethod, m_pFlatTask->m_Method, m_pFlatTask->m_fKappa, m_pFlatTask->m_lNrIterations);
				strText = QString(QObject::tr("Computing master flat (%1)","IDS_COMPUTINGMEDIANFLAT")).arg(strMethod);
				ZTRACE_RUNTIME(strText);

				if (pProgress)
				{
					pProgress->Start(strText, 1, false);
					pProgress->Progress1(strText, 0);
					pProgress->SetJointProgress(true);
				};
				std::shared_ptr<CMemoryBitmap> pFlatBitmap = m_pFlatTask->GetMaster(pProgress);
				if (pProgress)
					pProgress->SetJointProgress(false);

				if (static_cast<bool>(pFlatBitmap))
				{
					TCHAR			szDrive[1+_MAX_DRIVE];
					TCHAR			szDir[1+_MAX_DIR];
					QString			strInfo;

					strInfo = QString(QObject::tr("Master Flat created from %1 pictures (%2)", "IDS_MEDIANFLATINFO")).arg(m_pFlatTask->m_vBitmaps.size()).arg(strMethod);

					_tsplitpath(m_pFlatTask->m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

					BuildMasterFileNames(m_pFlatTask, _T("MasterFlat"), /* bExposure */ false, szDrive, szDir,
						&strMasterFlat, &strMasterFlatInfo);
					strText = QObject::tr("Saving Master Flat", "IDS_SAVINGMASTERFLAT");
					ZTRACE_RUNTIME(strText);

					if (pProgress)
					{
						pProgress->Start(strText, 1, false);
						pProgress->Progress1(strText, 1);
						pProgress->Start2(QString::fromWCharArray(strMasterFlat.GetString()), 0);
					};

					CString strInfo2(strInfo.toStdWString().c_str());
					WriteMasterTIFF(strMasterFlat, pFlatBitmap.get(), pProgress, strInfo2, m_pFlatTask);

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
		if (pCurrentTask->m_groupID)
		{
			if (pNewTask->m_groupID == pCurrentTask->m_groupID)
				bResult = true;
		}
		else
		{
			if ((pNewTask->m_groupID == BaseTask.m_groupID) ||
				 !pNewTask->m_groupID)
				bResult = true;
		};
	}
	else if ((pNewTask->m_groupID == BaseTask.m_groupID) ||
		     !pNewTask->m_groupID)
		bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::AddFileToTask(const CFrameInfo & FrameInfo, uint16_t dwGroupID)
{
	ZFUNCTRACE_RUNTIME();

	bool			bFound = false;

	for (int i = 0;i<m_vTasks.size() && !bFound;i++)
	{
		if ((m_vTasks[i].m_TaskType == FrameInfo.m_PictureType) &&
			(m_vTasks[i].m_groupID == dwGroupID))
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

		ti.m_dwTaskID  = (int)m_vTasks.size()+1;
		ti.m_groupID = dwGroupID;
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
	int				j;
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
							if ((pResult->m_groupID == m_vTasks[j].m_groupID) &&
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
	for (auto& task : this->m_vTasks)
	{
		if (task.m_TaskType == PICTURETYPE_LIGHTFRAME)
		{
			// Create a new stacking info
			CStackingInfo		si;

			si.m_pLightTask = std::addressof(task);

			// Try to find the best offset task for this task
			// same ISO (gain) if possible
			// else the closest ISO (gain), else 0
			// (tie breaker is number of frames in the offset task)
			si.m_pOffsetTask = FindBestMatchingTask(task, PICTURETYPE_OFFSETFRAME);

			// Try to find the best dark task for this task
			// same ISO (gain) and exposure, else same ISO (gain) and closest exposure
			// else no ISO (gain) and closest exposure
			// (tie breaker is number of frames in the dark task)
			si.m_pDarkTask = FindBestMatchingTask(task, PICTURETYPE_DARKFRAME);

			// Try to find the best dark flat task for this task
			// same ISO (gain) and exposure, else same ISO (gain) and closest exposure
			// else no ISO (gain) and closest exposure
			// (tie breaker is number of frames in the dark task)
			si.m_pDarkFlatTask = FindBestMatchingTask(task, PICTURETYPE_DARKFLATFRAME);

			// Try to find the best flat task for this task
			// same ISO (gain) if possible, else the closest ISO (gain), else 0
			// (tie breaker is number of frames in the flat task)
			si.m_pFlatTask = FindBestMatchingTask(task, PICTURETYPE_FLATFRAME);

			m_vStacks.push_back(si);
		}
	}

	UpdateTasksMethods();
}

/* ------------------------------------------------------------------- */

void CAllStackingTasks::ResetTasksStatus()
{
	for (auto& task : m_vTasks)
		task.m_bDone = false;
}

/* ------------------------------------------------------------------- */

int CAllStackingTasks::FindStackID(LPCTSTR szLightFrame)
{
	int			lResult = 0;
	int			i, j;

	// Find in which stack this light frame is located
	for (i = 0;(i<m_vStacks.size()) && !lResult;i++)
	{
		if (m_vStacks[i].m_pLightTask)
		{
			for (j = 0;j<m_vStacks[i].m_pLightTask->m_vBitmaps.size() && !lResult;j++)
			{
				if (!m_vStacks[i].m_pLightTask->m_vBitmaps[j].filePath.compare(szLightFrame))
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

	Workspace					workspace;
	MULTIBITMAPPROCESSMETHOD	LightMethod = MBP_AVERAGE;
	double						fLightKappa = 2.0;
	unsigned int				lLightIteration = 5;
	MULTIBITMAPPROCESSMETHOD	DarkMethod	= MBP_MEDIAN;
	double						fDarkKappa	= 2.0;
	unsigned int				lDarkIteration = 5;
	MULTIBITMAPPROCESSMETHOD	FlatMethod	= MBP_MEDIAN;
	double						fFlatKappa	= 2.0;
	unsigned int				lFlatIteration = 5;
	MULTIBITMAPPROCESSMETHOD	OffsetMethod	= MBP_MEDIAN;
	double						fOffsetKappa	= 2.0;
	unsigned int				lOffsetIteration = 5;

	unsigned int				dwMethod;

	dwMethod = workspace.value("Stacking/Light_Method", (uint)MBP_AVERAGE).toUInt();
	if (dwMethod)
		LightMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	lLightIteration = workspace.value("Stacking/Light_Iteration", (uint)5).toUInt();
	fLightKappa = workspace.value("Stacking/Light_Kappa", 2.0).toDouble();


	dwMethod = workspace.value("Stacking/Dark_Method", (uint)MBP_MEDIAN).toUInt();
	if (dwMethod)
		DarkMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	lDarkIteration = workspace.value("Stacking/Dark_Iteration", (uint)5).toUInt();
	fDarkKappa = workspace.value("Stacking/Dark_Kappa", 2.0).toDouble();

	dwMethod = workspace.value("Stacking/Flat_Method", (uint)MBP_MEDIAN).toUInt();
	if (dwMethod)
		FlatMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	lFlatIteration = workspace.value("Stacking/Flat_Iteration", (uint)5).toUInt();
	fFlatKappa = workspace.value("Stacking/Flat_Kappa", 2.0).toDouble();

	dwMethod = workspace.value("Stacking/Offset_Method", (uint)MBP_MEDIAN).toUInt();
	if (dwMethod)
		OffsetMethod = (MULTIBITMAPPROCESSMETHOD)dwMethod;
	lOffsetIteration = workspace.value("Stacking/Offset_Iteration", (uint)5).toUInt();
	fOffsetKappa = workspace.value("Stacking/Offset_Kappa", 2.0).toDouble();

	for (auto& stack : this->m_vStacks)
	{
		if (stack.m_pLightTask)
			stack.m_pLightTask->SetMethod(LightMethod, fLightKappa, lLightIteration);
		if (stack.m_pDarkTask)
			stack.m_pDarkTask->SetMethod(DarkMethod, fDarkKappa, lDarkIteration);
		if (stack.m_pDarkFlatTask)
			stack.m_pDarkFlatTask->SetMethod(DarkMethod, fDarkKappa, lDarkIteration);
		if (stack.m_pOffsetTask)
			stack.m_pOffsetTask->SetMethod(OffsetMethod, fOffsetKappa, lOffsetIteration);
		if (stack.m_pFlatTask)
			stack.m_pFlatTask->SetMethod(FlatMethod, fFlatKappa, lFlatIteration);
	}
}

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::DoOffsetTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

	// 1. create all the offset masters
	for (int i = 0;i<m_vStacks.size() && bResult;i++)
	{
		if (m_vStacks[i].m_pOffsetTask)
		{
			if (!m_vStacks[i].m_pOffsetTask->m_bDone)
			{
				ZTRACE_RUNTIME("------------------------------\nCreate Master Offset");
				bResult = m_vStacks[i].DoOffsetTask(pProgress);
				ZTRACE_RUNTIME("--> Output File: %s", CT2CA(m_vStacks[i].m_pOffsetTask->m_strOutputFile, CP_ACP));
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
	for (int i = 0;i<m_vStacks.size() && bResult;i++)
	{
		if (m_vStacks[i].m_pDarkTask)
		{
			if (!m_vStacks[i].m_pDarkTask->m_bDone)
			{
				ZTRACE_RUNTIME("------------------------------\nCreate Master Dark");

				if (m_vStacks[i].m_pOffsetTask)
					// Load the master offset
					ZTRACE_RUNTIME("Load Master Offset: %s", CT2CA(m_vStacks[i].m_pOffsetTask->m_strOutputFile, CP_ACP));
				else
					ZTRACE_RUNTIME("No Master Offset");

				CTaskInfo *			pTaskInfo = m_vStacks[i].m_pDarkTask;

				bResult = m_vStacks[i].DoDarkTask(pProgress);
				ZTRACE_RUNTIME("--> Output File: %s", CT2CA(m_vStacks[i].m_pDarkTask->m_strOutputFile,CP_ACP));
				if (!bResult)
					ZTRACE_RUNTIME("Abort");
			};
		};
	};

	return bResult;
};

bool CAllStackingTasks::DoDarkFlatTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

	// 2. create all the dark masters (using the offset master if necessary)
	for (int i = 0;i<m_vStacks.size() && bResult;i++)
	{
		if (m_vStacks[i].m_pDarkFlatTask)
		{
			if (!m_vStacks[i].m_pDarkFlatTask->m_bDone)
			{
				ZTRACE_RUNTIME("------------------------------\nCreate Master Dark Flat");

				if (m_vStacks[i].m_pOffsetTask)
					// Load the master offset
					ZTRACE_RUNTIME("Load Master Offset: %s", CT2CA(m_vStacks[i].m_pOffsetTask->m_strOutputFile,CP_ACP));
				else
					ZTRACE_RUNTIME("No Master Offset");

				CTaskInfo *			pTaskInfo = m_vStacks[i].m_pDarkFlatTask;

				bResult = m_vStacks[i].DoDarkFlatTask(pProgress);
				ZTRACE_RUNTIME("--> Output File: %s", CT2CA(m_vStacks[i].m_pDarkFlatTask->m_strOutputFile, CP_ACP));
				if (!bResult)
					ZTRACE_RUNTIME("Abort");
			};
		};
	};

	return bResult;
};

bool CAllStackingTasks::DoAllPreTasks(CDSSProgress* pProgress)
{
	if (!DoOffsetTasks(pProgress)) return false;
	if (!DoDarkTasks(pProgress)) return false;
	if (!DoDarkFlatTasks(pProgress)) return false;
	if (!DoFlatTasks(pProgress)) return false;
	return true;
}

bool CAllStackingTasks::DoFlatTasks(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

	// 3. create all the flat masters (using the offset master if necessary)
	for (int i = 0;i<m_vStacks.size() && bResult;i++)
	{
		if (m_vStacks[i].m_pFlatTask)
		{
			if (!m_vStacks[i].m_pFlatTask->m_bDone)
			{
				ZTRACE_RUNTIME("------------------------------\nCreate Master Flat");

				if (m_vStacks[i].m_pOffsetTask)
					// Load the master offset
					ZTRACE_RUNTIME("Load Master Offset: %s", CT2CA(m_vStacks[i].m_pOffsetTask->m_strOutputFile, CP_ACP));
				else
					ZTRACE_RUNTIME("No Master Offset");

				CTaskInfo *			pTaskInfo = m_vStacks[i].m_pFlatTask;

				bResult = m_vStacks[i].DoFlatTask(pProgress);
				ZTRACE_RUNTIME("--> Output File: %s", CT2CA(m_vStacks[i].m_pFlatTask->m_strOutputFile, CP_ACP));
				if (!bResult)
					ZTRACE_RUNTIME("Abort");
			};
		};
	};
	return bResult;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::checkReadOnlyStatus(QStringList & folders)
{
	std::set<fs::path>			sFolders;

	for (int i = 0;i<m_vTasks.size();i++)
	{
		for (int j = 0;j<m_vTasks[i].m_vBitmaps.size();j++)
		{
			QString		strFileName;

			auto& file = m_vTasks[i].m_vBitmaps[j].filePath;
			if (!m_vTasks[i].m_vBitmaps[j].IsMasterFrame())
			{
				fs::path directory;

				if (file.has_parent_path())
					directory = file.parent_path();
				else
					directory = file.root_path();
				
				sFolders.insert(directory);
			};
		};
	};

	// Check that it is possible to write a file in all the folders
	for (auto it = sFolders.begin(); it != sFolders.end(); it++)
	{
		bool			bDirOk = true;
		fs::path dir{ *it };
		auto file = dir / "DSS260FTR.testfile.txt";

		if (std::FILE* hFile =
#if defined _WINDOWS
			_wfopen(file.generic_wstring().c_str(), L"wt")
#else
			std::fopen(file.generic_string().c_str(), "wt")
#endif
			)
		{
			auto result = fprintf(hFile, "DeepSkyStacker: This is a test file to check that it is possible to write in this folder");
			if (result<=0)
				bDirOk = false;
			fclose(hFile);
			fs::remove(file);
		}
		else
			bDirOk = false;

		if (!bDirOk)
			folders.append(QString::fromStdU16String(dir.generic_u16string()));
	};

	return folders.isEmpty();
};

/* ------------------------------------------------------------------- */

__int64	CAllStackingTasks::computeNecessaryDiskSpace(const DSSRect& rcOutput)
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

	for (int i = 0;i<m_vStacks.size();i++)
	{
		int lWidth;
		int lHeight;
		int lNrChannels;
		int lNrBytesPerChannel;
		std::int64_t ulSpace;
		std::int64_t ulLSpace;

		if (m_vStacks[i].m_pLightTask && m_vStacks[i].m_pLightTask->m_vBitmaps.size())
		{
			lWidth		= m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lWidth;
			lHeight		= m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lHeight;
			lNrChannels = m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lNrChannels;
			lNrBytesPerChannel = m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lBitPerChannels/8;

			ulSpace = static_cast<std::int64_t>(lWidth) * lHeight * lNrBytesPerChannel * lNrChannels;
			ulLSpace = static_cast<std::int64_t>(lWidth) * lHeight * 2 * 3;

			if (!rcOutput.isEmpty())
				ulLSpace = static_cast<std::int64_t>(rcOutput.width()) * rcOutput.height() * 2 * 3;

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
				ulOffsetSpace = std::max(ulOffsetSpace, static_cast<__int64>(ulSpace * m_vStacks[i].m_pOffsetTask->m_vBitmaps.size()));

			if (m_vStacks[i].m_pDarkTask)
				ulDarkSpace = std::max(ulDarkSpace, static_cast<__int64>(ulSpace * m_vStacks[i].m_pDarkTask->m_vBitmaps.size()));

			if (m_vStacks[i].m_pDarkFlatTask)
				ulDarkFlatSpace = std::max(ulDarkFlatSpace, static_cast<__int64>(ulSpace * m_vStacks[i].m_pDarkFlatTask->m_vBitmaps.size()));

			if (m_vStacks[i].m_pFlatTask)
				ulFlatSpace = std::max(ulFlatSpace, static_cast<__int64>(ulSpace * m_vStacks[i].m_pFlatTask->m_vBitmaps.size()));
		};
	};

	ulResult = std::max(ulLightSpace, std::max(ulFlatSpace, std::max(ulOffsetSpace, std::max(ulDarkSpace, ulDarkFlatSpace))));
	ulResult *= 11;
	ulResult /= 10;

	return ulResult;
}

/* ------------------------------------------------------------------- */

__int64	CAllStackingTasks::computeNecessaryDiskSpace()
{
	DSSRect rcOutput;

	if (m_bUseCustomRectangle)
		rcOutput = m_rcCustom;

	return computeNecessaryDiskSpace(rcOutput);
}

/* ------------------------------------------------------------------- */

__int64	CAllStackingTasks::AvailableDiskSpace(CString & strDrive)
{
	fs::path path{ GetTemporaryFilesFolder().toStdU16String() };
	
	auto [cap, _, avail] = std::filesystem::space(path);

	strDrive = CString((LPCTSTR)path.root_name().wstring().c_str());

	return avail;
}

/* ------------------------------------------------------------------- */

QString CAllStackingTasks::GetTemporaryFilesFolder()
{
	ZFUNCTRACE_RUNTIME();
	QSettings	settings;

	fs::path path{ settings.value("Stacking/TemporaryFilesFolder", QString("")).toString().toStdU16String() };

	//
	// If it is a directory, check we can write a file to it and return its name if so.
	//
	if (fs::file_type::directory == status(path).type())
	{
		// Check that we can write to it
		auto file = path / "Temp.txt";

		if (std::FILE* f = 
#if defined _WINDOWS
			_wfopen(file.c_str(), L"wb")
#else
			std::fopen(file.c_str(), "wb")
#endif
			)
		{
			fclose(f);
			fs::remove(file);
			return QString::fromStdU16String(path.u16string());
		}
	}
	//
	// If it's not a directory or we can't write to it return the default
	// system temp directory
	//
	return QString::fromStdU16String(fs::temp_directory_path().u16string());
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::SetTemporaryFilesFolder(QString strFolder)
{
	ZFUNCTRACE_RUNTIME();
	QSettings settings;
	fs::path path{ strFolder.toStdU16String() };
	ZASSERT(fs::file_type::directory == status(path).type());
	settings.setValue("Stacking/TemporaryFilesFolder", strFolder);
};

/* ------------------------------------------------------------------- */

BACKGROUNDCALIBRATIONMODE	CAllStackingTasks::GetBackgroundCalibrationMode()
{
	Workspace			workspace;

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
	Workspace			workspace;
	int				interpolation = (int)BCI_RATIONAL;

	interpolation = workspace.value("Stacking/BackgroundCalibrationInterpolation").toUInt();

	return (BACKGROUNDCALIBRATIONINTERPOLATION)interpolation;
};

/* ------------------------------------------------------------------- */

RGBBACKGROUNDCALIBRATIONMETHOD	CAllStackingTasks::GetRGBBackgroundCalibrationMethod()
{
	Workspace			workspace;

	int method = workspace.value("Stacking/RGBBackgroundCalibrationMethod", (int)RBCM_MAXIMUM).toUInt();

	return (RGBBACKGROUNDCALIBRATIONMETHOD)method;
};

/* ------------------------------------------------------------------- */

bool	CAllStackingTasks::GetDarkOptimization()
{
	Workspace			workspace;

	bool value = workspace.value("Stacking/DarkOptimization", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

double	CAllStackingTasks::GetDarkFactor()
{
	double				value = 1.0;
	Workspace			workspace;

	if (workspace.value("Stacking/UseDarkFactor", false).toBool())
	{
		value = workspace.value("Stacking/DarkFactor", 1.0).toDouble();
	};

	return value;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetHotPixelsDetection()
{
	Workspace			workspace;

	bool value = workspace.value("Stacking/HotPixelsDetection", true).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetBadLinesDetection()
{
	Workspace			workspace;

	bool value = workspace.value("Stacking/BadLinesDetection", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

STACKINGMODE	CAllStackingTasks::GetResultMode()
{
	STACKINGMODE		Result = SM_NORMAL;
	Workspace			workspace;

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
	Workspace			workspace;

	bool value = workspace.value("Stacking/CreateIntermediates", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetSaveCalibrated()
{
	Workspace			workspace;

	bool value = workspace.value("Stacking/SaveCalibrated", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

bool CAllStackingTasks::GetSaveCalibratedDebayered()
{
	Workspace			workspace;

	bool value = workspace.value("Stacking/SaveCalibratedDebayered", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

WORD	CAllStackingTasks::GetAlignmentMethod()
{
	Workspace			workspace;

	WORD value = workspace.value("Stacking/AlignmentTransformation", 0).toUInt();

	return value;
};

/* ------------------------------------------------------------------- */

int	CAllStackingTasks::GetPixelSizeMultiplier()
{
	Workspace			workspace;

	int value = workspace.value("Stacking/PixelSizeMultiplier", 1).toUInt();

	return value;
};

/* ------------------------------------------------------------------- */

bool	CAllStackingTasks::GetChannelAlign()
{
	Workspace			workspace;

	return workspace.value("Stacking/AlignChannels", false).toBool();
};

/* ------------------------------------------------------------------- */

bool	CAllStackingTasks::GetSaveIntermediateCometImages()
{
	Workspace			workspace;

	bool value = workspace.value("Stacking/SaveCometImages", false).toBool();

	return value;
};

/* ------------------------------------------------------------------- */

bool	CAllStackingTasks::GetApplyMedianFilterToCometImage()
{
	Workspace			workspace;

	bool value = workspace.value("Stacking/ApplyFilterToCometImages", true).toBool();

	return false; //value;
};

/* ------------------------------------------------------------------- */

INTERMEDIATEFILEFORMAT CAllStackingTasks::GetIntermediateFileFormat()
{
	Workspace			workspace;

	int value = workspace.value("Stacking/IntermediateFileFormat", 1).toUInt();

	if (value != IFF_TIFF && value != IFF_FITS)
		value = IFF_TIFF;

	return (INTERMEDIATEFILEFORMAT)value;
};

/* ------------------------------------------------------------------- */

COMETSTACKINGMODE CAllStackingTasks::GetCometStackingMode()
{
	Workspace			workspace;

	int value = workspace.value("Stacking/CometStackingMode", 0).toUInt();

	if (value != CSM_STANDARD && value != CSM_COMETONLY)
		value = CSM_COMETSTAR;

	return (COMETSTACKINGMODE)value;
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::GetPostCalibrationSettings(CPostCalibrationSettings & pcs)
{
	Workspace			workspace;
	
	pcs.m_bHot = workspace.value("Stacking/PCS_DetectCleanHot", false).toBool();

	pcs.m_lHotFilter = workspace.value("Stacking/PCS_HotFilter", 1).toUInt();

	pcs.m_fHotDetection = workspace.value("Stacking/PCS_HotDetection", 500.0).toDouble()/10.0;

	pcs.m_bCold = workspace.value("Stacking/PCS_DetectCleanCold", false).toBool();

	pcs.m_lColdFilter = workspace.value("Stacking/PCS_ColdFilter", 1U).toUInt();

	pcs.m_fColdDetection = workspace.value("Stacking/PCS_ColdDetection", 500.0).toDouble()/10.0;
	 
	pcs.m_bSaveDeltaImage = workspace.value("Stacking/PCS_SaveDeltaImage", false).toBool();

	pcs.m_Replace = static_cast<COSMETICREPLACE>(workspace.value("Stacking/PCS_ReplaceMethod", (int)CR_MEDIAN).toInt());
};

/* ------------------------------------------------------------------- */

void CAllStackingTasks::SetPostCalibrationSettings(const CPostCalibrationSettings & pcs)
{
	Workspace			workspace;

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
