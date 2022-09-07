#include <stdafx.h>
#include "FrameList.h"
#include "ImageListModel.h"
#include "RegisterEngine.h"
#include "Workspace.h"
#include <direct.h>
#include <QSettings>
#include "ZExcept.h"

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	CMRUList::readSettings()
{
	QSettings settings;
	uint32_t count = 0;

	m_vLists.clear();

	QString keyName(baseKeyName);
	keyName += "/NrMRU";

	count = settings.value(keyName, 0).toUInt();

	for (int i = 0; i < count; i++)
	{
		QString keyName = QString("%1/MRU%2")
			.arg(baseKeyName).arg(i);

		QString value = settings.value(keyName).toString();

		m_vLists.emplace_back(value);
	};
};

/* ------------------------------------------------------------------- */

void	CMRUList::saveSettings()
{
	QSettings	settings;

	QString keyName(baseKeyName);

	// Clear all the entries first
	settings.remove(keyName);

	keyName += "/NrMRU";
	
	settings.setValue(keyName, (uint)m_vLists.size());
	for (int i = 0;i<m_vLists.size();i++)
	{
		QString keyName = QString("%1/MRU%2")
			.arg(baseKeyName).arg(i);
		QString value(m_vLists[i]);

		settings.setValue(keyName, value);
	};
};


/* ------------------------------------------------------------------- */

void	CMRUList::Add(const QString& list)
{
	bool				bFound = false;
	int index = -1;

	for (int i = 0; i<m_vLists.size() && !bFound; i++)
	{
		if (!m_vLists[i].compare(list))
		{
			bFound = true;
			index = i;
			break;
		};
	};

	std::vector<QString>::iterator	it;

	if (bFound)
	{
		// remove from the position if it is not 0
		if (index)
		{
			it = m_vLists.begin();
			it+= index;
			m_vLists.erase(it);
			m_vLists.insert(m_vLists.begin(), list);
		};
	}
	else
		m_vLists.insert(m_vLists.begin(), list);

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
		uint16_t groupId = 0;

		for (LONG i = 0;i<m_vFiles.size();i++)
		{
			LONG		lItem = i;
			LONG		lChecked = 0;
			CString		strType;

			if (!m_vFiles[lItem].m_bRemoved)
			{
				if (groupId != m_vFiles[lItem].m_groupId)
				{
					groupId = m_vFiles[lItem].m_groupId;
					fprintf(hFile, "#GROUPID#%hu\n", groupId);
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
				_tsplitpath(m_vFiles[lItem].filePath.c_str(), szItemDrive, nullptr, nullptr, nullptr);

				if (!_tcscmp(szDrive, szItemDrive))
				{
					//
					// Convert FileName to a relative path
					//
					PathRelativePathTo(szRelPath,
						(LPCTSTR)strBaseDirectory,
						FILE_ATTRIBUTE_DIRECTORY,
						(LPCTSTR)(m_vFiles[lItem].filePath.c_str()),
						FILE_ATTRIBUTE_NORMAL);

					fprintf(hFile, "%ld\t%s\t%s\n", lChecked,
                        (LPCSTR)CT2CA(strType, CP_UTF8),
                        (LPCSTR)CT2CA(szRelPath, CP_UTF8));
				}
				else
				{
					fprintf(hFile, "%ld\t%s\t%s\n", lChecked,
						(LPCSTR)CT2CA(strType, CP_UTF8),
						m_vFiles[lItem].filePath.u8string().c_str());
				}
			};
		};
		
		Workspace				workspace;

		workspace.SaveToFile(hFile);
		workspace.setDirty();
		m_bDirty = false;

		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

static bool ParseLine(LPCTSTR szLine, std::int32_t & lChecked, CString & strType, CString & strFile)
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

static bool parseLine(QString line, std::int32_t& lChecked, QString& strType, QString& strFile)
{
	bool result = false;
	
	//
	// trailing \n has been removed by .trimmed() before calling this mf
	//
	auto list = line.split(QLatin1Char('\t'));

	//
	// line should now be split into three sections each in its own entry in the QStringList
	// list = [ "checked", "Type", "File" ] 
	//
	if (list.size() == 3)
	{
		lChecked = list[0].toInt();
		strType = list[1];
		strFile = list[2];
		result = true;
	}
	return result;
};

/* ------------------------------------------------------------------- */

static bool	IsChangeGroupLine(LPCTSTR szLine, DWORD & groupId)
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
		groupId = _ttol(strGroup);
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

static bool	isChangeGroupLine(QString line, uint16_t& groupId, QString& groupName)
{
	bool				bResult = false;

	if (line.left(9) == "#GROUPID#")
	{
		line.remove(0, 9);	// Strip off the identifier

		auto list = line.split(QLatin1Char('\t'));
		//
		// line should now be split into one or two sections each in its own entry in the QStringList
		// list = [ "groupId", "groupName"] 
		//

		groupId = list[0].toUInt();
		if (2 == list.size())
			groupName = list[1];

		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CFrameList::LoadFilesFromList(LPCTSTR szFileList)
{
	FILE *				hFile;
	DWORD				groupId = 0;

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
			Workspace			workspace;
			CHAR				szLine[10000];

			while (fgets(szLine, sizeof(szLine), hFile))
			{
				std::int32_t checkState(Qt::Unchecked);
				CString			strType;
				CString			strFile;
				CString			strLine((LPCTSTR)CA2CTEX<sizeof(szLine)>(szLine, CP_UTF8));

				bool			bUseAsStarting = false;

				if (workspace.ReadFromString(strLine))
				{
				}
				else if (IsChangeGroupLine(strLine, groupId))
				{
				}
				else if (ParseLine(strLine, checkState, strType, strFile))
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

							ListBitMap			lb;

							if (lb.InitFromFile(pszAbsoluteFile, Type))
							{
								lb.m_groupId = groupId;
								if (!AddFile(pszAbsoluteFile, groupId, Type, (checkState == Qt::Checked)))
								{
									// Add to the list
									lb.m_bChecked = static_cast<Qt::CheckState>(checkState);
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
											lb.m_lNrStars		 = static_cast<decltype(ListBitMap::m_lNrStars)>(bmpInfo.m_vStars.size());
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

void CFrameList::FillTasks(CAllStackingTasks & tasks)
{
	int				lNrComets = 0;
	bool				bReferenceFrameHasComet = false;
	bool				bReferenceFrameSet = false;
	double				fMaxScore = -1.0;


	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].m_bChecked == Qt::Checked)
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
			tasks.AddFileToTask(m_vFiles[i], m_vFiles[i].m_groupId);
			if (m_vFiles[i].m_bComet)
				lNrComets++;
		};
	};

	if (lNrComets>1 && bReferenceFrameHasComet)
		tasks.SetCometAvailable(true);
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
				strReferenceFrame = m_vFiles[i].filePath.c_str();
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
			m_vFiles[i].m_bChecked == Qt::Checked &&
			!m_vFiles[i].m_bRegistered)
		{
			if ((lGroupID < 0) || (lGroupID == m_vFiles[i].m_groupId))
				lResult++;
		};
	};

	return lResult;
};

#include <QMessageBox>
namespace DSS
{
	size_t FrameList::checkedImageCount(const PICTURETYPE type, const int16_t id) const
	{
		size_t result = 0;

		// Iterate over all groups.
		for (auto i = 0; i != imageGroups.size(); ++i)
		{
			// If the group number passed in was -1 then want to count the number of
			// checked images of the relevant type in ALL groups.  Otherwise only
			// count checked images for the passed group number.
			if (-1 == id || id == i)
			{
				for (auto it = imageGroups[i].pictures.cbegin();
					it != imageGroups[i].pictures.cend(); ++it)
				{
					if (it->m_PictureType == type && it->m_bChecked == Qt::Checked) ++result;
				}
			}
		}

		return result;
	}

	size_t FrameList::unregisteredCheckedLightFrameCount(int id) const
	{
		size_t result = 0;

		// Iterate over all groups
		for (const auto& group : imageGroups)
		{
			// If the group number passed in was -1 then want to count the number of
			// checked images of the relevant type in ALL groups.  Otherwise only
			// count checked images for the passed group number.
			if (-1 == id || id == group.index())
			{
				for (auto it = group.pictures.cbegin();
					it != group.pictures.cend(); ++it)
				{
					if (!it->m_bRemoved &&
						it->IsLightFrame() &&
						it->m_bChecked == Qt::Checked &&
						!it->m_bRegistered)	++result;
				};
			}
		}
		return result;
	};



	void FrameList::clearOffsets()
	{
		for (auto& group : imageGroups)
		{
			int row = 0; int groupIndex = group.index();
			for (auto it = group.pictures.begin();
				it != group.pictures.end(); ++it)
			{
				if (it->IsLightFrame())
					it->m_bDeltaComputed = false;
				if (index == groupIndex)
				{
					//
					// Tell the table view which columns have been impacted
					//
					imageGroups[index].pictures.emitChanged(row, row,
						static_cast<int>(ImageListModel::Column::dX),
						static_cast<int>(ImageListModel::Column::Angle));
				}
				++row;
			}
		}
		//
		// Tell the table view which columns have been impacted
		//
		imageGroups[index].pictures.emitChanged(0, imageGroups[index].pictures.rowCount(),
			static_cast<int>(ImageListModel::Column::dX),
			static_cast<int>(ImageListModel::Column::Angle));
	};

	void FrameList::clearOffset(fs::path file)
	{
		int group = Group::whichGroupContains(file);

		ZASSERTSTATE(-1 != group);
		int row = 0;
		for (auto it = imageGroups[group].pictures.begin(); it != imageGroups[group].pictures.end(); ++it)
		{
			if (file == it->filePath)
			{
				it->m_bDeltaComputed = false;
				//
				// Ask the Table Model to tell the table view which rows/columns have changed
				//
				if (index == group)
					imageGroups[group].pictures.emitChanged(row, row,
						static_cast<int>(ImageListModel::Column::dX),
						static_cast<int>(ImageListModel::Column::Angle));
				return;
			}
			++row;
		}
	};

	void FrameList::updateOffset(fs::path file, double xOffset, double yOffset, double angle, const CBilinearParameters& transform, const VOTINGPAIRVECTOR vVotedPairs)
	{
		int group = Group::whichGroupContains(file);

		ZASSERTSTATE(-1 != group);
		int row = 0;
		for (auto it = imageGroups[group].pictures.begin(); it != imageGroups[group].pictures.end(); ++it)
		{
			if (file == it->filePath)
			{
				it->m_bDeltaComputed = true;
				imageGroups[group].pictures.setData(row, ImageListModel::Column::dX, xOffset);
				imageGroups[group].pictures.setData(row, ImageListModel::Column::dY, yOffset);
				imageGroups[group].pictures.setData(row, ImageListModel::Column::Angle, angle);
				it->m_Transformation = transform;
				it->m_vVotedPairs = vVotedPairs;

				return;
			}
			++row;
		}
	};

	QString FrameList::getReferenceFrame()
	{
		for (const auto& group : imageGroups)
		{
			for (auto it = group.pictures.cbegin(); it != group.pictures.cend(); ++it)
			{
				if (!it->m_bRemoved && it->IsLightFrame() && 
					it->m_bChecked == Qt::Checked &&
					it->m_bUseAsStarting
					)
				{
					return QString::fromStdU16String(it->filePath.generic_u16string());
				}
			}
		}
		return QString();
	}
	
	QString FrameList::getFirstCheckedLightFrame()
	{
		for (const auto& group : imageGroups)
		{
			for (auto it = group.pictures.cbegin(); it != group.pictures.cend(); ++it)
			{
				if (!it->m_bRemoved && it->IsLightFrame() && it->m_bChecked == Qt::Checked)
				{
					return QString::fromStdU16String(it->filePath.generic_u16string());
				};
			};
		}

		return QString();
	};




	void FrameList::fillTasks(CAllStackingTasks& tasks)
	{
		size_t				comets = 0;
		bool				bReferenceFrameHasComet = false;
		bool				bReferenceFrameSet = false;
		double				fMaxScore = -1.0;

		// Iterate over all groups.
		for (uint16_t group = 0; group != imageGroups.size(); ++group)
		{
			// and then over each image in the group
			for (auto it = imageGroups[group].pictures.cbegin();
				it != imageGroups[group].pictures.cend(); ++it)
			{
				if (it->m_bChecked == Qt::Checked)
				{
					if (it->m_bUseAsStarting)
					{
						bReferenceFrameSet = true;
						bReferenceFrameHasComet = it->m_bComet;
					}
					if (!bReferenceFrameSet && (it->m_fOverallQuality > fMaxScore))
					{
						fMaxScore = it->m_fOverallQuality;
						bReferenceFrameHasComet = it->m_bComet;
					}
					tasks.AddFileToTask(*it, group);
					if (it->m_bComet)
						comets++;
				}
			}
		}

		if (comets > 1 && bReferenceFrameHasComet)
			tasks.SetCometAvailable(true);
		tasks.ResolveTasks();
	};

	/* ------------------------------------------------------------------- */

	FrameList& FrameList::saveListToFile(fs::path file)
	{
		if (std::FILE* hFile =
#if defined(_WINDOWS)
			_wfopen(file.c_str(), L"wt")
#else
			std::fopen(file.c_ctr(), "wt")
#endif
			)
		{
			fs::path directory;
		
			if (file.has_parent_path())
				directory = file.parent_path();
			else
				directory = file.root_path();

			fprintf(hFile, "DSS file list\n");
			fprintf(hFile, "CHECKED\tTYPE\tFILE\n");

			uint16_t groupId = 0;

			for (auto &g : imageGroups)
			{
				// and then over each image in the group
				for (auto it = g.pictures.cbegin();
					it != g.pictures.cend(); ++it) 
				{

					long	checked{ 0 };
					QString type;

					if (!it->m_bRemoved)
					{
						if (groupId != it->m_groupId)
						{
							groupId = it->m_groupId;
							fprintf(hFile, "#GROUPID#%hu\t%s\n", groupId, g.name().toUtf8().constData());
						};
						checked = it->m_bChecked == Qt::Checked ? 1L : 0L;
						if (it->IsLightFrame())
						{
							if (it->m_bUseAsStarting)
								type = "reflight";
							else
								type = "light";
						}
						else if (it->IsDarkFrame())
							type = "dark";
						else if (it->IsDarkFlatFrame())
							type = "darkflat";
						else if (it->IsOffsetFrame())
							type = "offset";
						else if (it->IsFlatFrame())
							type = "flat";

						//
						// Convert the path to the file to one that is relative to
						// the directory containing the file list file of that's possible.
						// If not just leave it as the absolute path.
						//
						fs::path path{ it->filePath.lexically_proximate(directory) };
						fprintf(hFile, "%ld\t%s\t%s\n", checked,
							type.toUtf8().constData(),
							path.u8string().c_str());
					};
				}
				g.setDirty(false);
			};

			Workspace				workspace;

			workspace.SaveToFile(hFile);
			workspace.setDirty();

			fclose(hFile);
		}
		return *this;
	}

	/* ------------------------------------------------------------------- */

	FrameList& FrameList::loadFilesFromList(fs::path fileList)
	{
		uint16_t groupId = 0;
		std::error_code ec;

		//
		// Remember current directory and extract directory containing filelist
		//
		fs::path directory;

		if (fileList.has_parent_path())
			directory = fileList.parent_path();
		else
			directory = fileList.root_path();

		fs::path oldCWD{ fs::current_path(ec) };		// Save CWD

		if (ec)
		{
			ZTRACE_RUNTIME("fs::current_path() failed with error code %ld, %s",
				ec.value(), ec.message().c_str());
		}
		

		if (std::FILE* hFile =
#if defined(_WINDOWS)
			_wfopen(fileList.c_str(), L"rt");
#else
			std::fopen(fileList.c_ctr(), "rt");
#endif
			!ec
			)
		{

			CHAR			szBuffer[2000];
			QString			strValue;
			bool			bContinue = false;

			fs::current_path(directory, ec);				// Set CWD to fileList dir
			if (!ec)
				bContinue = true;
			else
			{
				ZTRACE_RUNTIME("fs::current_path(%s) failed with error code %ld, %s",
					directory.generic_string().c_str(), ec.value(), ec.message().c_str());
				bContinue = false;
			}

			// Read scan line
			if (fgets(szBuffer, sizeof(szBuffer), hFile))
			{
				strValue = QString::fromUtf8(szBuffer);
				if (!strValue.compare("DSS file list\n", Qt::CaseInsensitive))
					bContinue = true;
			}

			if (bContinue)
			{
				bContinue = false;
				if (fgets(szBuffer, sizeof(szBuffer), hFile))
				{
					strValue = QString::fromUtf8(szBuffer);
					if (!strValue.compare("CHECKED\tTYPE\tFILE\n", Qt::CaseInsensitive))
						bContinue = true;
				}
			};

			if (bContinue)
			{
				// Read the file info
				Workspace			workspace;
				CHAR				szLine[10000];

				while (fgets(szLine, sizeof(szLine), hFile))
				{
					std::int32_t checkState(Qt::Unchecked);
					QString			strType;
					QString			strFile;
					QString groupName;
					QString			strLine{ QString::fromUtf8(szLine).trimmed() };

					bool			bUseAsStarting = false;

					if (workspace.ReadFromString(strLine))
					{
					}
					else if (isChangeGroupLine(strLine, groupId, groupName))
					{
						setGroup(groupId);	// Select the group in question
						if (!groupName.isEmpty())
							imageGroups[groupId].setName(groupName);
					}
					else if (parseLine(strLine, checkState, strType, strFile))
					{
						PICTURETYPE		Type = PICTURETYPE_UNKNOWN;

						if ("light" == strType)
							Type = PICTURETYPE_LIGHTFRAME;
						else if ("dark" == strType)
							Type = PICTURETYPE_DARKFRAME;
						else if ("darkflat" == strType)
							Type = PICTURETYPE_DARKFLATFRAME;
						else if ("flat" == strType)
							Type = PICTURETYPE_FLATFRAME;
						else if ("offset" == strType)
							Type = PICTURETYPE_OFFSETFRAME;
						else if ("reflight" == strType)
						{
							Type = PICTURETYPE_REFLIGHTFRAME;
							bUseAsStarting = true;
						};

						if (Type != PICTURETYPE_UNKNOWN)
						{
							//
							// Convert relative path to absolute path. 
							//
							fs::path filePath{ strFile.toStdU16String() };

							// If it's not already an absolute path
							if (!filePath.is_absolute())
							{
								filePath = fs::absolute(filePath, ec);
							}

							if (ec)
							{
								ZTRACE_RUNTIME("fs::absolute(%s) failed with error code %ld, %s",
									filePath.generic_u8string().c_str(), ec.value(), ec.message().c_str());
							}

							// Check that the file exists
							if (is_regular_file(filePath))
							{
								ListBitMap			lb;

								if (lb.InitFromFile(filePath, Type))
								{
									lb.m_groupId = groupId;
									beginInsertRows(1);
									addFile(filePath, Type, (checkState == Qt::Checked));
									endInsertRows();
								};
							};
						}
								
					};
				};

				workspace.setDirty();
			};

			fclose(hFile);
		}

		fs::current_path(oldCWD);
		setDirty(false);
		return *this;
	}

	void FrameList::blankCheckedItemScores()
	{
		// Iterate over all groups.
		for (uint16_t group = 0; group != imageGroups.size(); ++group)
		{
			// and then over each image in the group
			for (auto it = imageGroups[group].pictures.begin();
				it != imageGroups[group].pictures.end(); ++it)
			{
				if (it->m_bChecked == Qt::Checked && it->IsLightFrame())
				{
					it->m_bRegistered = false;
				}
			}
		}

	};

	bool FrameList::areCheckedImagesCompatible()
	{
		bool				bResult = true;
		bool				bFirst = true;
		const ListBitMap* lb;
		LONG				lNrDarks = 0;
		LONG				lNrDarkFlats = 0;
		LONG				lNrFlats = 0;
		LONG				lNrOffsets = 0;
		bool				bMasterDark = false;
		bool				bMasterFlat = false;
		bool				bMasterDarkFlat = false;
		bool				bMasterOffset = false;

		// Iterate over all groups.
		for (uint16_t group = 0; group != imageGroups.size(); ++group)
		{
			// and then over each image in the group
			for (auto it = imageGroups[group].pictures.cbegin();
				it != imageGroups[group].pictures.cend(); ++it)
			{
				if (it->m_bChecked == Qt::Checked)
				{
					if (bFirst)
					{
						lb = &(*it);
						bFirst = false;
					}
					else
						bResult = lb->IsCompatible(*it);
				}
			}
		}

		if (bResult)
		{
			if (bMasterDark && lNrDarks > 1)
				bResult = false;
			if (bMasterDarkFlat && lNrDarkFlats > 1)
				bResult = false;
			if (bMasterFlat && lNrFlats > 1)
				bResult = false;
			if (bMasterOffset && lNrOffsets > 1)
				bResult = false;
		};

		return bResult;
	};

	/* ------------------------------------------------------------------- */


	void FrameList::updateCheckedItemScores()
	{
		int row = 0;

		// Iterate over all groups.
		for (uint16_t group = 0; group != imageGroups.size(); ++group)
		{
			// and then over each image in the group
			for (auto it = imageGroups[group].pictures.begin();
				it != imageGroups[group].pictures.end(); ++it)
			{
				if (it->m_bChecked == Qt::Checked &&
					it->IsLightFrame())
				{
					CLightFrameInfo		bmpInfo;

					bmpInfo.SetBitmap(it->filePath, false, false);

					//
					// Update list information, but beware that you must use setData() for any of the columns
					// that are defined in the DSS::ImageListModel::Column enumeration as they are used for the 
					// QTableView.   If this isn't done, the image list view won't get updated.
					//
					// The "Sky Background" (Column::BackgroundCol) is a special case it's a class, not a primitive, so the model 
					// class has a specific member function to set that.
					//
					// Other member of ListBitMap (e.g.) m_bRegistered and m_bComet can be updated directly.
					//
					if (bmpInfo.m_bInfoOk)
					{
						it->m_bRegistered = true;
						imageGroups[group].pictures.setData(row, ImageListModel::Column::Score, bmpInfo.m_fOverallQuality);
						imageGroups[group].pictures.setData(row, ImageListModel::Column::FWHM, bmpInfo.m_fFWHM);
						imageGroups[group].pictures.setData(row, ImageListModel::Column::Stars, (int)bmpInfo.m_vStars.size());
						it->m_bComet = bmpInfo.m_bComet;
						imageGroups[group].pictures.setData(row, ImageListModel::Column::Background, (uint32_t)bmpInfo.m_vStars.size());
						imageGroups[group].pictures.setSkyBackground(row, bmpInfo.m_SkyBackground);

					}
					else
					{
						it->m_bRegistered = false;
					};
				};
				++row;
			};
		}

	};

	void FrameList::checkAll(bool check)
	{
		for (int id = 0; id != imageGroups.size(); ++id)
		{
			auto& group = imageGroups[id];
			for (int32_t index = 0; index < group.pictures.mydata.size(); ++index)
			{
				auto& file = group.pictures.mydata[index];
				if (!file.m_bRemoved)
				{
					if (check) file.m_bChecked = Qt::Checked;
					else file.m_bChecked = Qt::Unchecked;
				}
			}
			QModelIndex start{ group.pictures.createIndex(0, 0) };
			QModelIndex end{ group.pictures.createIndex(group.pictures.rowCount(), 0) };
			QVector<int> role{ Qt::CheckStateRole };
			group.pictures.dataChanged(start, end, role);
			group.setDirty();
		}
	}

	void FrameList::checkAllDarks(bool check)
	{
		for (int id = 0; id != imageGroups.size(); ++id)
		{
			auto& group = imageGroups[id];
			for (int32_t index = 0; index < group.pictures.mydata.size(); ++index)
			{
				auto& file = group.pictures.mydata[index];
				if (!file.m_bRemoved && file.IsDarkFrame())
				{
					if (check) file.m_bChecked = Qt::Checked;
					else file.m_bChecked = Qt::Unchecked;
				}
				QModelIndex start{ group.pictures.createIndex(index, 0) };
				QModelIndex end{ group.pictures.createIndex(index, 0) };
				QVector<int> role{ Qt::CheckStateRole };
				group.pictures.dataChanged(start, end, role);
			}

			group.setDirty();
		}
	}
	/* ------------------------------------------------------------------- */

	void FrameList::checkAllFlats(bool check)
	{
		for (int id = 0; id != imageGroups.size(); ++id)
		{
			auto& group = imageGroups[id];
			for (int32_t index = 0; index < group.pictures.mydata.size(); ++index)
			{
				auto& file = group.pictures.mydata[index];
				if (!file.m_bRemoved && file.IsFlatFrame())
				{
					if (check) file.m_bChecked = Qt::Checked;
					else file.m_bChecked = Qt::Unchecked;
				}
				QModelIndex start{ group.pictures.createIndex(index, 0) };
				QModelIndex end{ group.pictures.createIndex(index, 0) };
				QVector<int> role{ Qt::CheckStateRole };
				group.pictures.dataChanged(start, end, role);
			}

			group.setDirty();
		}
	}

	/* ------------------------------------------------------------------- */

	void FrameList::checkAllOffsets(bool check)
	{
		for (int id = 0; id != imageGroups.size(); ++id)
		{
			auto& group = imageGroups[id];
			for (int32_t index = 0; index < group.pictures.mydata.size(); ++index)
			{
				auto& file = group.pictures.mydata[index];
				if (!file.m_bRemoved && file.IsOffsetFrame())
				{
					if (check) file.m_bChecked = Qt::Checked;
					else file.m_bChecked = Qt::Unchecked;
				}
				QModelIndex start{ group.pictures.createIndex(index, 0) };
				QModelIndex end{ group.pictures.createIndex(index, 0) };
				QVector<int> role{ Qt::CheckStateRole };
				group.pictures.dataChanged(start, end, role);
			}

			group.setDirty();
		}
	}

	/* ------------------------------------------------------------------- */

	void FrameList::checkAllLights(bool check)
	{
		for (int id = 0; id != imageGroups.size(); ++id)
		{
			auto& group = imageGroups[id];
			for (int32_t index = 0; index < group.pictures.mydata.size(); ++index)
			{
				auto& file = group.pictures.mydata[index];
				if (!file.m_bRemoved && file.IsLightFrame())
				{
					if (check) file.m_bChecked = Qt::Checked;
					else file.m_bChecked = Qt::Unchecked;
				}
				QModelIndex start{ group.pictures.createIndex(index, 0) };
				QModelIndex end{ group.pictures.createIndex(index, 0) };
				QVector<int> role{ Qt::CheckStateRole };
				group.pictures.dataChanged(start, end, role);
			}

			group.setDirty();
		}
	}

	/* ------------------------------------------------------------------- */


	void FrameList::checkAbove(double threshold)
	{
		for (int id = 0; id != imageGroups.size(); ++id)
		{
			auto& group = imageGroups[id];
			for (int index = 0; index != group.pictures.mydata.size(); ++index)
			{
				auto& file = group.pictures.mydata[index];
				if (!file.m_bRemoved
					&& file.IsLightFrame())
				{
					file.m_bChecked = 
						(file.m_fOverallQuality >= threshold) ? Qt::Checked : Qt::Unchecked;
					QModelIndex start{ group.pictures.createIndex(index, 0) };
					QModelIndex end{ group.pictures.createIndex(index, 0) };
					QVector<int> role{ Qt::CheckStateRole };
					group.pictures.dataChanged(start, end, role);
				}
			}
			group.setDirty();
		};

	};

	/* ------------------------------------------------------------------- */

	void FrameList::checkBest(double fPercent)
	{
		std::vector<ScoredLightFrame> lightFrames;

		for (auto & group : imageGroups)
		{
			for (size_t i = 0; i != group.pictures.mydata.size(); ++i)
			{
				const auto& file = group.pictures.mydata[i];
				if (!file.m_bRemoved && file.IsLightFrame())
					lightFrames.emplace_back(group.index(),
						static_cast<decltype(ScoredLightFrame::index)>(i),
						file.m_fOverallQuality);
			}
			group.setDirty();
		}

		const int last = static_cast<int>(fPercent * lightFrames.size() / 100.0);
		//
		// Sort in *descending* order (see operator < in class definition)
		std::sort(lightFrames.begin(), lightFrames.end());

		for (size_t i = 0; i < lightFrames.size(); i++)
		{
			auto id = lightFrames[i].group;
			auto index = lightFrames[i].index;

			imageGroups[id].pictures.mydata[index].m_bChecked =
				(i <= last) ? Qt::Checked : Qt::Unchecked;
			QModelIndex start	{ imageGroups[id].pictures.createIndex(index, 0) };
			QModelIndex end		{ imageGroups[id].pictures.createIndex(index, 0) };
			QVector<int> role{ Qt::CheckStateRole };
			imageGroups[id].pictures.dataChanged(start, end, role);
		}
	};

	/* ------------------------------------------------------------------- */


}


/* ------------------------------------------------------------------- */
