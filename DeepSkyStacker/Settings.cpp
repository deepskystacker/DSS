#include <stdafx.h>
#include "StackingTasks.h"
#include "Registry.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "Settings.h"
#include <algorithm>

/* ------------------------------------------------------------------- */

bool CGlobalSettings::operator == (const CGlobalSettings & gs) const
{
	bool				bResult = false;
	LONG				i;

	if (m_vFiles.size() == gs.m_vFiles.size())
	{
		// Check that this is the same files
		bResult = true;
		for (i = 0;i<m_vFiles.size() && bResult;i++)
			bResult = !m_vFiles[i].CompareNoCase(gs.m_vFiles[i]);
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

BOOL	CGlobalSettings::ReadFromFile(LPCTSTR szFile)
{
	BOOL		bResult = FALSE;
	FILE *		hFile;

	m_sSettings.clear();
	m_vFiles.clear();
	hFile = _tfopen(szFile, _T("rt"));
	if (hFile)
	{
		// First read the settings
		CSetting		s;

		while (s.Read(hFile))
			m_sSettings.insert(s);

		// Then read the file list
		CHAR			szBuffer[2000];

		while (fgets(szBuffer, sizeof(szBuffer), hFile))
		{
			CString		strFileName = (LPCTSTR)CA2CTEX<sizeof(szBuffer)>(szBuffer);

			strFileName.TrimRight(_T("\n"));
			m_vFiles.push_back(strFileName);
		};

		std::sort(m_vFiles.begin(), m_vFiles.end());

		fclose(hFile);
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CGlobalSettings::WriteToFile(LPCTSTR szFile)
{
	FILE *				hFile;

	hFile = _tfopen(szFile, _T("wt"));
	if (hFile)
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
		for (LONG i = 0;i<m_vFiles.size();i++)
			fprintf(hFile, "%s\n", (LPCSTR)CT2CA(m_vFiles[i]));

		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

BOOL	CGlobalSettings::InitFromCurrent(CTaskInfo * pTask, LPCTSTR szFile)
{
	BOOL				bResult = FALSE;
	CBitmapInfo			bmpInfo;
	
	m_sSettings.clear();
	m_vFiles.clear();
	if (pTask && GetPictureInfo(szFile, bmpInfo))
	{
		bResult = TRUE;

		ReadFromRegistry();
		BOOL				bFITS = FALSE;
		BOOL				bRAW  = FALSE;
		LONG				lWidth = 0,
							lHeight = 0,
							lBitPerChannels = 0,
							lNrChannels = 0;
		LONG				i;


		for (i = 0;i<pTask->m_vBitmaps.size();i++)
		{
			CString			strFile;

			strFile.Format(_T("%s[%s]"), (LPCTSTR)(pTask->m_vBitmaps[i].m_strFileName), (LPCTSTR)pTask->m_vBitmaps[i].m_strDateTime);
			m_vFiles.push_back(strFile);

			if (!bFITS && (pTask->m_vBitmaps[i].m_strInfos.Left(4) == _T("FITS")))
				bFITS = TRUE;
			else if (!bRAW && (pTask->m_vBitmaps[i].m_strInfos.Left(3) == _T("RAW")))
				bRAW = TRUE;
			lWidth  = pTask->m_vBitmaps[i].m_lWidth;
			lHeight = pTask->m_vBitmaps[i].m_lHeight;
			lBitPerChannels = pTask->m_vBitmaps[i].m_lBitPerChannels;
			lNrChannels     = pTask->m_vBitmaps[i].m_lNrChannels;
		};

		// Check sizes
		if ((lWidth != bmpInfo.m_lWidth) || (lHeight!=bmpInfo.m_lHeight))
			bResult = FALSE;
		if ((lBitPerChannels != bmpInfo.m_lBitPerChannel) || (lNrChannels != bmpInfo.m_lNrChannels))
			bResult = FALSE;
		if (!bmpInfo.m_bMaster)
			bResult = FALSE;

		AddFileVariable(_T("Bitmap.FileName"), szFile);
		AddVariable(_T("Bitmap.Width"), lWidth);
		AddVariable(_T("Bitmap.Height"), lHeight);
		AddVariable(_T("Bitmap.BitPerChannels"), lBitPerChannels);
		AddVariable(_T("Bitmap.NrChannels"), lNrChannels);

		if (bFITS)
			AddFITSSettings();
		if (bRAW)
			AddRAWSettings();

		std::sort(m_vFiles.begin(), m_vFiles.end());
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
