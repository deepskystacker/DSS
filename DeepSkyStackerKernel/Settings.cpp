#include "stdafx.h"

//#include "resource.h"
#include "StackingTasks.h"

#include "DSSTools.h"
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "Settings.h"


/* ------------------------------------------------------------------- */

bool CGlobalSettings::operator == (const CGlobalSettings & gs) const
{
	bool				bResult = false;

	if (m_vFiles.size() == gs.m_vFiles.size())
	{
		// Check that this is the same files
		bResult = true;
		for (size_t i = 0; i < m_vFiles.size() && bResult; i++)
			bResult = (m_vFiles[i].compare(gs.m_vFiles[i], Qt::CaseInsensitive) == 0);
	};

	if (bResult)
	{
		// Check that this is the same properties with the same values
		SETTINGCONSTITERATOR		it1, it2;

		it1 = m_sSettings.begin();
		it2 = gs.m_sSettings.begin();

		while ((it1 != m_sSettings.end()) && (it2 != gs.m_sSettings.end()) && bResult)
		{
			bResult = ((*it1) == (*it2));
			if (bResult)
			{
				it1++;
				it2++;
			};
		};

		bResult = bResult && (it1 == m_sSettings.end()) && (it2 == gs.m_sSettings.end());
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CGlobalSettings::ReadFromFile(const fs::path& file)
{
	bool		bResult = false;

	m_sSettings.clear();
	m_vFiles.clear();
	if (std::FILE* hFile =
#if defined(Q_OS_WIN)
		_wfopen(file.c_str(), L"wt")
#else
		std::fopen(file.c_str(), "wt")
#endif
		)
	{
		// First read the settings
		CSetting		s;

		while (s.Read(hFile))
			m_sSettings.insert(s);

		// Then read the file list
		char			szBuffer[2000];
		while (fgets(szBuffer, sizeof(szBuffer), hFile))
		{
			const QString strFileName(szBuffer);
			m_vFiles.push_back(strFileName.trimmed());
		};

		std::sort(m_vFiles.begin(), m_vFiles.end());

		fclose(hFile);
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CGlobalSettings::WriteToFile(const fs::path& file)
{
	if (std::FILE* hFile =
#if defined(Q_OS_WIN)
		_wfopen(file.c_str(), L"wt")
#else
		std::fopen(file.c_str(), "wt")
#endif
		)
	{
		// First write the settings
		SETTINGITERATOR		it;

		for (it = m_sSettings.begin(); it != m_sSettings.end(); it++)
		{
			CSetting &s = const_cast<CSetting&>(*it); //MATD
			s.Write(hFile);
		}

		// Then write the file list
		fprintf(hFile, "----FileList----\n");
		for (size_t i = 0; i < m_vFiles.size(); i++)
			fprintf(hFile, "%s\n", m_vFiles[i].toStdString().c_str());

		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

bool	CGlobalSettings::InitFromCurrent(CTaskInfo * pTask, const fs::path& file)
{
	bool				bResult = false;
	CBitmapInfo			bmpInfo;

	m_sSettings.clear();
	m_vFiles.clear();
	if (pTask && GetPictureInfo(file, bmpInfo))
	{
		bResult = true;

		ReadFromRegistry();
		bool				bFITS = false;
		bool				bRAW  = false;
		int				lWidth = 0,
							lHeight = 0,
							lBitPerChannels = 0,
							lNrChannels = 0;


		for (size_t i = 0; i < pTask->m_vBitmaps.size(); i++)
		{
			const QString strFile(QString("%1[%2]").arg(pTask->m_vBitmaps[i].filePath.u8string().c_str()).arg(pTask->m_vBitmaps[i].m_strDateTime));
			m_vFiles.push_back(strFile);

			if (!bFITS && (pTask->m_vBitmaps[i].m_strInfos.left(4) == "FITS"))
				bFITS = true;
			else if (!bRAW && (pTask->m_vBitmaps[i].m_strInfos.left(3) == "RAW"))
				bRAW = true;
			lWidth  = pTask->m_vBitmaps[i].m_lWidth;
			lHeight = pTask->m_vBitmaps[i].m_lHeight;
			lBitPerChannels = pTask->m_vBitmaps[i].m_lBitsPerChannel;
			lNrChannels     = pTask->m_vBitmaps[i].m_lNrChannels;
		};

		// Check sizes
		if ((lWidth != bmpInfo.m_lWidth) || (lHeight!=bmpInfo.m_lHeight))
			bResult = false;
		//if ((lBitPerChannels != bmpInfo.m_lBitsPerChannel) || (lNrChannels != bmpInfo.m_lNrChannels))
		//	bResult = false;
		if (lNrChannels != bmpInfo.m_lNrChannels)
			bResult = false;

		if (!bmpInfo.m_bMaster)
			bResult = false;

		AddFileVariable("Bitmap.FileName", file);
		AddVariable("Bitmap.Width", lWidth);
		AddVariable("Bitmap.Height", lHeight);
		AddVariable("Bitmap.BitPerChannels", bmpInfo.m_lBitsPerChannel);
		AddVariable("Bitmap.NrChannels", lNrChannels);

		if (bFITS)
			AddFITSSettings();
		if (bRAW)
			AddRAWSettings();

		std::sort(m_vFiles.begin(), m_vFiles.end());
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
