#include <stdafx.h>
#include "FrameList.h"
#include "RegisterEngine.h"
#include "Workspace.h"
#include <direct.h>
#include <QSettings>

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	CMRUList::readSettings()
{
	QSettings settings;
	int dwNrValues = 0;

	m_vLists.clear();

	QString keyName(QString::fromWCharArray(m_strBasePath.GetString()));
	keyName += "/NrMRU";

	dwNrValues = static_cast<int>(settings.value(keyName, 0).toUInt());

	for (int i = 0; i < dwNrValues; i++)
	{
		QString keyName = QString("%1/MRU%2")
			.arg(QString::fromWCharArray(m_strBasePath.GetString())).arg(i);

		QString temp = settings.value(keyName).toString();
		CString strValue((LPCTSTR)temp.utf16());

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

void	CMRUList::saveSettings()
{
	QSettings	settings;

	QString keyName(QString::fromWCharArray(m_strBasePath.GetString()));

	// Clear all the entries first
	settings.remove(keyName);

	keyName += "/NrMRU";
	
	settings.setValue(keyName, (uint)m_vLists.size());
	for (int i = 0;i<m_vLists.size();i++)
	{
		QString keyName = QString("%1/MRU%2")
			.arg(QString::fromWCharArray(m_strBasePath.GetString())).arg(i);
		QString value(QString::fromWCharArray(m_vLists[i].GetString()));

		settings.setValue(keyName, value);
	};
};


/* ------------------------------------------------------------------- */

void	CMRUList::Add(LPCTSTR szList)
{
	bool				bFound = false;
	int				lFoundIndice = -1;

	for (int i = 0;i<m_vLists.size() && !bFound;i++)
	{
		if (!m_vLists[i].CompareNoCase(szList))
		{
			bFound = true;
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
		for (int j = 0;j<m_Jobs.m_vJobs.size();j++)
		{
			std::uint32_t dwGroupID = 0;

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
                    (LPCSTR)CT2CA(strGUID, CP_UTF8),
					(LPCSTR)CT2CA(m_Jobs.m_vJobs[j].m_strName, CP_UTF8),
					(LPCSTR)CT2CA(strRefGUID, CP_UTF8));
			};
			for (int i = 0;i<m_vFiles.size();i++)
			{
				int		lItem = i;
				int		lChecked = 0;
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
		workspace.setDirty();
		m_bDirty = false;

		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

static bool ParseLine(LPCTSTR szLine, int & lChecked, CString & strType, CString & strFile)
{
	bool				bResult = false;
	LPCTSTR				szPos = szLine;
	int				lPos = 0;
	int				lTab1 = -1,
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

		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

static bool	IsChangeGroupLine(LPCTSTR szLine, std::uint32_t& dwGroupID)
{
	bool				bResult = false;
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
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CFrameList::LoadFilesFromList(LPCTSTR szFileList)
{
	FILE *				hFile;
	std::uint32_t		dwGroupID = 0;
	GUID				dwJobID = MAINJOBID;

	SetCursor(::LoadCursor(nullptr, IDC_WAIT));
	hFile = _tfopen(szFileList, _T("rt"));
	if (hFile)
	{
		CHAR			szBuffer[2000];
		CString			strValue;
		bool			bContinue = false;

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
				bContinue = true;
		}

		if (bContinue)
		{
			bContinue = false;
			if (fgets(szBuffer, sizeof(szBuffer), hFile))
			{
				strValue = (LPCTSTR)CA2CTEX<sizeof(szBuffer)>(szBuffer, CP_UTF8);
				if (!strValue.CompareNoCase(_T("CHECKED\tTYPE\tFILE\n")))
					bContinue = true;
			}
		};

		if (bContinue)
		{
			// Read the file info
			CWorkspace			workspace;
			CHAR				szLine[10000];

			while (fgets(szLine, sizeof(szLine), hFile))
			{
				int			lChecked;
				CString			strType;
				CString			strFile;
				CString			strLine((LPCTSTR)CA2CTEX<sizeof(szLine)>(szLine, CP_UTF8));

				bool			bUseAsStarting = false;

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
						bUseAsStarting = true;
					};

					if (Type != PICTURETYPE_UNKNOWN)
					{
						int	length = 0;
						TCHAR*	pszAbsoluteFile = nullptr;

						//
						// Convert relative path to absolute path.
						//
						length = GetFullPathName(static_cast<LPCTSTR>(strFile), 0L, nullptr, nullptr);
						pszAbsoluteFile = new TCHAR[length];

						length = GetFullPathName(static_cast<LPCTSTR>(strFile), length, pszAbsoluteFile, nullptr);
						if (0 == length)
                            ZTRACE_RUNTIME("GetFullPathName for %s failed", (LPCSTR)CT2CA(strFile, CP_UTF8));

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

										bmpInfo.SetBitmap(pszAbsoluteFile, false);
										if (bmpInfo.m_bInfoOk)
										{
											lb.m_bRegistered = true;
											lb.m_fOverallQuality = bmpInfo.m_fOverallQuality;
											lb.m_fFWHM			 = bmpInfo.m_fFWHM;
											lb.m_lNrStars		 = static_cast<decltype(CListBitmap::m_lNrStars)>(bmpInfo.m_vStars.size());
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

			workspace.setDirty();
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
	m_bDirty = false;
	SetCursor(::LoadCursor(nullptr, IDC_ARROW));
};

/* ------------------------------------------------------------------- */

void CFrameList::FillTasks(CAllStackingTasks & tasks, GUID const& dwJobID)
{
	int				lNrComets = 0;
	bool				bReferenceFrameHasComet = false;
	bool				bReferenceFrameSet = false;
	double				fMaxScore = -1.0;

	if (m_vFiles.size())
	{
		CJob &				Job = m_Jobs.GetJob(dwJobID);

		if (!Job.IsNullJob())
		{
			for (int i = 0;i<m_vFiles.size();i++)
			{
				if (!m_vFiles[i].m_bRemoved &&
					m_vFiles[i].m_bChecked &&
					(m_vFiles[i].m_JobID == dwJobID))
				{
					if (m_vFiles[i].m_bUseAsStarting)
					{
						bReferenceFrameSet = true;
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
				tasks.SetCometAvailable(true);

			tasks.m_dwJobID = Job.m_ID;
			tasks.m_strJob  = Job.m_strName;
		};
	};
};

/* ------------------------------------------------------------------- */

bool CFrameList::GetReferenceFrame(CString & strReferenceFrame)
{
	// First search for a reference frame
	bool				bResult = false;

	for (int i = 0;i<m_vFiles.size() && !bResult;i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].IsLightFrame())
		{
			if (m_vFiles[i].m_bUseAsStarting)
			{
				bResult = true;
				strReferenceFrame = m_vFiles[i].m_strFileName;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

int CFrameList::GetNrUnregisteredCheckedLightFrames(int lGroupID)
{
	int				lResult = 0;

	for (int i = 0;i<m_vFiles.size();i++)
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
