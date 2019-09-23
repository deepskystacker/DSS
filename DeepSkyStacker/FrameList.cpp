#include <stdafx.h>
#include "FrameList.h"
#include "RegisterEngine.h"
#include "Workspace.h"
#include <direct.h>

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	CMRUList::InitFromRegistry()
{
	CRegistry			reg;
	DWORD				dwNrValues = 0;

	m_vLists.clear();
	reg.LoadKey(m_strBasePath, _T("NrMRU"), dwNrValues);
	for (LONG i = 0;i<dwNrValues;i++)
	{
		CString			strKey;
		CString			strValue;

		strKey.Format(_T("MRU%ld"), i);
		reg.LoadKey(m_strBasePath, (LPCTSTR)strKey, strValue);

		FILE *			hFile;

		hFile = _tfopen((LPCTSTR)strValue, _T("rt"));
		if (hFile)
		{
			m_vLists.push_back(strValue);
			fclose(hFile);
		};
	};
};

/* ------------------------------------------------------------------- */

void	CMRUList::SaveToRegistry()
{
	CRegistry			reg;

	// Clear all the entries first
	reg.DeleteKey(m_strBasePath);

	reg.SaveKey(m_strBasePath, _T("NrMRU"), (DWORD)m_vLists.size());
	for (LONG i = 0;i<m_vLists.size();i++)
	{
		CString			strKey;

		strKey.Format(_T("MRU%ld"), i);
		reg.SaveKey(m_strBasePath, (LPCTSTR)strKey, (LPCTSTR)m_vLists[i]);
	};
};


/* ------------------------------------------------------------------- */

void	CMRUList::Add(LPCTSTR szList)
{
	BOOL				bFound = FALSE;
	LONG				lFoundIndice = -1;

	for (LONG i = 0;i<m_vLists.size() && !bFound;i++)
	{
		if (!m_vLists[i].CompareNoCase(szList))
		{
			bFound = TRUE;
			lFoundIndice = i;
		};
	};

	std::vector<CString>::iterator	it;

	if (bFound)
	{
		// remove from the position if it is not 0
		if (lFoundIndice)
		{
			it = m_vLists.begin();
			it+= lFoundIndice;
			m_vLists.erase(it);
			m_vLists.insert(m_vLists.begin(), CString(szList));
		};
	}
	else
		m_vLists.insert(m_vLists.begin(), CString(szList));

	if (m_vLists.size()>m_lMaxLists)
		m_vLists.resize(m_lMaxLists);
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CFrameList::SaveListToFile(LPCTSTR szFile)
{
	FILE *						hFile;

	hFile = _tfopen(szFile, _T("wt"));
	if (hFile)
	{
		CString		strBaseDirectory;
		TCHAR		szDir[1 + _MAX_DIR];
		TCHAR		szDrive[1 + _MAX_DRIVE];
		TCHAR		szRelPath[1 + MAX_PATH];

		_tsplitpath(szFile, szDrive, szDir, nullptr, nullptr);
		strBaseDirectory = szDrive;
		strBaseDirectory += szDir;

		fprintf(hFile, "DSS file list\n");
		fprintf(hFile, "CHECKED\tTYPE\tFILE\n");
		for (LONG j = 0;j<m_Jobs.m_vJobs.size();j++)
		{
			DWORD						dwGroupID = 0;

			if ((m_Jobs.m_vJobs[j].m_ID != MAINJOBID) ||
				(m_Jobs.m_vJobs[j].m_RefID != GUID_NULL))

			{
				CString			strGUID;
				CString			strRefGUID;
				CComBSTR		szGUID;
				CComBSTR		szRefGUID;

				StringFromCLSID(m_Jobs.m_vJobs[j].m_ID, &szGUID);
				strGUID = szGUID;
				StringFromCLSID(m_Jobs.m_vJobs[j].m_RefID, &szRefGUID);
				strRefGUID = szRefGUID;
				fprintf(hFile, "#JOBID#%s#%s#%s\n",
					(LPCSTR)CT2CA(strGUID,CP_UTF8),
					(LPCSTR)CT2CA(m_Jobs.m_vJobs[j].m_strName, CP_UTF8),
					(LPCSTR)CT2CA(strRefGUID, CP_UTF8));
			};
			for (LONG i = 0;i<m_vFiles.size();i++)
			{
				LONG		lItem = i;
				LONG		lChecked = 0;
				CString		strType;

				if (!m_vFiles[lItem].m_bRemoved &&
					(m_vFiles[lItem].m_JobID == m_Jobs.m_vJobs[j].m_ID))
				{
					if (dwGroupID != m_vFiles[lItem].m_dwGroupID)
					{
						dwGroupID = m_vFiles[lItem].m_dwGroupID;
						fprintf(hFile, "#GROUPID#%ld\n", dwGroupID);
					};
					lChecked = m_vFiles[lItem].m_bChecked;
					if (m_vFiles[lItem].IsLightFrame())
					{
						if (m_vFiles[lItem].m_bUseAsStarting)
							strType = "reflight";
						else
							strType = "light";
					}
					else if (m_vFiles[lItem].IsDarkFrame())
						strType = "dark";
					else if (m_vFiles[lItem].IsDarkFlatFrame())
						strType = "darkflat";
					else if (m_vFiles[lItem].IsOffsetFrame())
						strType = "offset";
					else if (m_vFiles[lItem].IsFlatFrame())
						strType = "flat";

					//
					// Check if this file is on the same drive as the file-list file
					// if not we can't use relative paths and will need to save the
					// absolute path the the file-list
					//
					TCHAR		szItemDrive[1 + _MAX_DRIVE];
					_tsplitpath(m_vFiles[lItem].m_strFileName, szItemDrive, nullptr, nullptr, nullptr);

					if (!_tcscmp(szDrive, szItemDrive))
					{
						//
						// Convert m_strFileName to a relative path
						//
						PathRelativePathTo(szRelPath,
							(LPCTSTR)strBaseDirectory,
							FILE_ATTRIBUTE_DIRECTORY,
							(LPCTSTR)(m_vFiles[lItem].m_strFileName),
							FILE_ATTRIBUTE_NORMAL);

						fprintf(hFile, "%ld\t%s\t%s\n", lChecked,
							(LPCSTR)CT2CA(strType, CP_UTF8),
							(LPCSTR)CT2CA(szRelPath, CP_UTF8));
					}
					else
					{
						fprintf(hFile, "%ld\t%s\t%s\n", lChecked,
							(LPCSTR)CT2CA(strType, CP_UTF8),
							(LPCSTR)CT2CA(m_vFiles[lItem].m_strFileName, CP_UTF8));
					}
				};
			};
		};

		CWorkspace				workspace;

		workspace.SaveToFile(hFile);
		workspace.ResetDirty();
		m_bDirty = FALSE;

		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

static BOOL ParseLine(LPCTSTR szLine, LONG & lChecked, CString & strType, CString & strFile)
{
	BOOL				bResult = FALSE;
	LPCTSTR				szPos = szLine;
	LONG				lPos = 0;
	LONG				lTab1 = -1,
						lTab2 = -1,
						lEnd  = -1;

	// Looking for 2 tabs '\t' and a new line '\n'
	while (*szPos && *szPos != '\n' && lTab1<0)
	{
		if (*szPos == '\t')
			lTab1 = lPos;
		szPos++;
		lPos++;
	};
	while (*szPos && *szPos != '\n' && lTab2<0)
	{
		if (*szPos == '\t')
			lTab2 = lPos;
		szPos++;
		lPos++;
	};
	while (*szPos && lEnd < 0)
	{
		if (*szPos == '\n')
			lEnd = lPos;
		szPos++;
		lPos++;
	};

	if (lTab1>0 && lTab2 > 0 && lEnd > 0)
	{
		CString			strLine = szLine;
		CString			strChecked;

		strChecked = strLine.Left(lTab1);

		lChecked = _ttol(strChecked);
		strType = strLine.Mid(lTab1+1, lTab2-lTab1-1);
		strFile = strLine.Mid(lTab2+1, lEnd-lTab2-1);

		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

static BOOL	IsChangeGroupLine(LPCTSTR szLine, DWORD & dwGroupID)
{
	BOOL				bResult = FALSE;
	CString				strLine = szLine;

	if (strLine.Left(9) == _T("#GROUPID#"))
	{
		CString			strGroup;
		LPCTSTR			szPos = szLine;

		szPos += 9;
		while (*szPos != '\n')
		{
			strGroup += *szPos;
			szPos++;
		};
		dwGroupID = _ttol(strGroup);
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CFrameList::LoadFilesFromList(LPCTSTR szFileList)
{
	FILE *				hFile;
	DWORD				dwGroupID = 0;
	GUID				dwJobID = MAINJOBID;

	SetCursor(::LoadCursor(nullptr, IDC_WAIT));
	hFile = _tfopen(szFileList, _T("rt"));
	if (hFile)
	{
		CHAR			szBuffer[2000];
		CString			strValue;
		BOOL			bContinue = FALSE;

		CString		strBaseDirectory;
		TCHAR		szDir[1 + _MAX_DIR];
		TCHAR		szDrive[1 + _MAX_DRIVE];
		LPTSTR		szOldCWD;

		//
		// Extract the directory where the file list is stored.
		_tsplitpath(szFileList, szDrive, szDir, nullptr, nullptr);
		strBaseDirectory = szDrive;
		strBaseDirectory += szDir;

		//
		// Remember current directory and switch to directory containing filelist
		//
		szOldCWD = _tgetcwd(nullptr, 0);
		SetCurrentDirectory(strBaseDirectory);

		// Read scan line
		if (fgets(szBuffer, sizeof(szBuffer), hFile))
		{
			strValue = (LPCTSTR)CA2CTEX<sizeof(szBuffer)>(szBuffer, CP_UTF8);
			if (!strValue.CompareNoCase(_T("DSS file list\n")))
				bContinue = TRUE;
		}

		if (bContinue)
		{
			bContinue = FALSE;
			if (fgets(szBuffer, sizeof(szBuffer), hFile))
			{
				strValue = (LPCTSTR)CA2CTEX<sizeof(szBuffer)>(szBuffer, CP_UTF8);
				if (!strValue.CompareNoCase(_T("CHECKED\tTYPE\tFILE\n")))
					bContinue = TRUE;
			}
		};

		if (bContinue)
		{
			// Read the file info
			CWorkspace			workspace;
			CHAR				szLine[10000];

			while (fgets(szLine, sizeof(szLine), hFile))
			{
				LONG			lChecked;
				CString			strType;
				CString			strFile;
				CString			strLine((LPCTSTR)CA2CTEX<sizeof(szLine)>(szLine, CP_UTF8));

				BOOL			bUseAsStarting = FALSE;

				if (workspace.ReadFromString(strLine))
				{
				}
				else if (IsChangeGroupLine(strLine, dwGroupID))
				{
				}
				else if (ParseLine(strLine, lChecked, strType, strFile))
				{
					PICTURETYPE		Type = PICTURETYPE_UNKNOWN;

					if (!strType.CompareNoCase(_T("light")))
						Type = PICTURETYPE_LIGHTFRAME;
					else if (!strType.CompareNoCase(_T("dark")))
						Type = PICTURETYPE_DARKFRAME;
					else if (!strType.CompareNoCase(_T("darkflat")))
						Type = PICTURETYPE_DARKFLATFRAME;
					else if (!strType.CompareNoCase(_T("flat")))
						Type = PICTURETYPE_FLATFRAME;
					else if (!strType.CompareNoCase(_T("offset")))
						Type = PICTURETYPE_OFFSETFRAME;
					else if (!strType.CompareNoCase(_T("reflight")))
					{
						Type = PICTURETYPE_REFLIGHTFRAME;
						bUseAsStarting = TRUE;
					};

					if (Type != PICTURETYPE_UNKNOWN)
					{
						long	length = 0;
						TCHAR*	pszAbsoluteFile = nullptr;

						//
						// Convert relative path to absolute path.
						//
						length = GetFullPathName(static_cast<LPCTSTR>(strFile), 0L, nullptr, nullptr);
						pszAbsoluteFile = new TCHAR[length];

						length = GetFullPathName(static_cast<LPCTSTR>(strFile), length, pszAbsoluteFile, nullptr);
						if (0 == length) ZTRACE_RUNTIME("GetFullPathName for %s failed",
							(LPCSTR)CT2CA(strFile, CP_UTF8));

						// Check that the file exists
						FILE *		hTemp;

						hTemp = _tfopen(pszAbsoluteFile, _T("rb"));
						if (hTemp)
						{
							fclose(hTemp);

							CListBitmap			lb;

							if (lb.InitFromFile(pszAbsoluteFile, Type))
							{
								lb.m_dwGroupID = dwGroupID;
								if (!AddFile(pszAbsoluteFile, dwGroupID, dwJobID, Type, lChecked))
								{
									// Add to the list
									lb.m_bChecked = lChecked;
									if (lb.m_PictureType == PICTURETYPE_LIGHTFRAME)
									{
										lb.m_bUseAsStarting = bUseAsStarting;
										CLightFrameInfo			bmpInfo;

										bmpInfo.SetBitmap(pszAbsoluteFile, FALSE);
										if (bmpInfo.m_bInfoOk)
										{
											lb.m_bRegistered = TRUE;
											lb.m_fOverallQuality = bmpInfo.m_fOverallQuality;
											lb.m_fFWHM			 = bmpInfo.m_fFWHM;
											lb.m_lNrStars		 = (DWORD)bmpInfo.m_vStars.size();
											lb.m_bComet			 = bmpInfo.m_bComet;
											lb.m_SkyBackground	 = bmpInfo.m_SkyBackground;
										}
									};
									m_vFiles.push_back(lb);
								};
							};
						};
						delete [] pszAbsoluteFile;
					};
				};
			};

			workspace.ResetDirty();
		};


		fclose(hFile);
		if (nullptr != szOldCWD)
		{
			//
			// Restore working directory to status quo ante
			//
			SetCurrentDirectory(szOldCWD);
			free(szOldCWD);
		}
	};
	m_bDirty = FALSE;
	SetCursor(::LoadCursor(nullptr, IDC_ARROW));
};

/* ------------------------------------------------------------------- */

void CFrameList::FillTasks(CAllStackingTasks & tasks, GUID const& dwJobID)
{
	LONG				lNrComets = 0;
	BOOL				bReferenceFrameHasComet = FALSE;
	BOOL				bReferenceFrameSet = FALSE;
	double				fMaxScore = -1.0;

	if (m_vFiles.size())
	{
		CJob &				Job = m_Jobs.GetJob(dwJobID);

		if (!Job.IsNullJob())
		{
			for (LONG i = 0;i<m_vFiles.size();i++)
			{
				if (!m_vFiles[i].m_bRemoved &&
					m_vFiles[i].m_bChecked &&
					(m_vFiles[i].m_JobID == dwJobID))
				{
					if (m_vFiles[i].m_bUseAsStarting)
					{
						bReferenceFrameSet = TRUE;
						bReferenceFrameHasComet = m_vFiles[i].m_bComet;
					}
					if (!bReferenceFrameSet && (m_vFiles[i].m_fOverallQuality > fMaxScore))
					{
						fMaxScore = m_vFiles[i].m_fOverallQuality;
						bReferenceFrameHasComet = m_vFiles[i].m_bComet;
					};
					tasks.AddFileToTask(m_vFiles[i], m_vFiles[i].m_dwGroupID);
					if (m_vFiles[i].m_bComet)
						lNrComets++;
				};
			};

			if (lNrComets>1 && bReferenceFrameHasComet)
				tasks.SetCometAvailable(TRUE);

			tasks.m_dwJobID = Job.m_ID;
			tasks.m_strJob  = Job.m_strName;
		};
	};
};

/* ------------------------------------------------------------------- */

BOOL CFrameList::GetReferenceFrame(CString & strReferenceFrame)
{
	// First search for a reference frame
	BOOL				bResult = FALSE;

	for (LONG i = 0;i<m_vFiles.size() && !bResult;i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].IsLightFrame())
		{
			if (m_vFiles[i].m_bUseAsStarting)
			{
				bResult = TRUE;
				strReferenceFrame = m_vFiles[i].m_strFileName;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

LONG CFrameList::GetNrUnregisteredCheckedLightFrames(LONG lGroupID)
{
	LONG				lResult = 0;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].IsLightFrame() &&
			m_vFiles[i].m_bChecked &&
			!m_vFiles[i].m_bRegistered)
		{
			if ((lGroupID < 0) || (lGroupID == m_vFiles[i].m_dwGroupID))
				lResult++;
		};
	};

	return lResult;
};

/* ------------------------------------------------------------------- */
