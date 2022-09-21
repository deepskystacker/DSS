/****************************************************************************
**
** Copyright (C) 2022 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
#include "stdafx.h"

#include "BezierAdjust.h"
#include "Histogram.h"
#include "dss_settings.h"
#pragma pack(push, HDSETTINGS, 2)

static BOOL	GetDefaultSettingsFileName(CString& strFile)
{
	CString			strBase;
	TCHAR			szFileName[1 + _MAX_PATH];
	TCHAR			szDrive[1 + _MAX_DRIVE];
	TCHAR			szDir[1 + _MAX_DIR];

	GetModuleFileName(nullptr, szFileName, sizeof(szFileName));
	strBase = szFileName;
	_tsplitpath(strBase, szDrive, szDir, nullptr, nullptr);

	strFile = szDrive;
	strFile += szDir;
	strFile += "DSSSettings.DSSSettings";

	return TRUE;
};

/* ------------------------------------------------------------------- */
const DWORD			HDSSETTINGS_MAGIC = 0x7ABC6F10L;

typedef struct tagHDSETTINGSHEADER
{
	DWORD			dwMagic;		// Magic number (always HDSSETTINGS_MAGIC)
	DWORD			dwHeaderSize;	// Always sizeof(HDSETTINGSHEADER);
	LONG			lNrSettings;	// Number of settings
	DWORD			dwFlags;		// Flags
	char			Reserved[32];	// Reserved (set to 0)
}HDSETTINGSHEADER;

#pragma pack(pop, HDSETTINGS)

/* ------------------------------------------------------------------- */

bool	CDSSSettings::Load(LPCTSTR szFile)
{
	bool			bResult = false;
	CString			strFile = szFile;
	FILE* hFile = nullptr;

	if (!strFile.GetLength())
		GetDefaultSettingsFileName(strFile);

	hFile = _tfopen(strFile, _T("rb"));
	if (hFile)
	{
		HDSETTINGSHEADER		Header;
		LONG					i;

		fread(&Header, sizeof(Header), 1, hFile);
		if ((Header.dwMagic == HDSSETTINGS_MAGIC) &&
			(Header.dwHeaderSize == sizeof(Header)))
		{
			m_lSettings.clear();
			for (i = 0; i < Header.lNrSettings; i++)
			{
				CDSSSetting		cds;

				cds.Load(hFile);
				m_lSettings.push_back(cds);
			};

			bResult = true;
			m_lSettings.sort();
		};

		fclose(hFile);
	};

	m_bLoaded = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CDSSSettings::Save(LPCTSTR szFile)
{
	bool bResult = false;
	CString			strFile = szFile;
	FILE* hFile = nullptr;

	if (!strFile.GetLength())
		GetDefaultSettingsFileName(strFile);

	hFile = _tfopen(strFile, _T("wb"));
	if (hFile)
	{
		m_lSettings.sort();

		HDSETTINGSHEADER		Header;
		DSSSETTINGITERATOR		it;

		memset(&Header, 0, sizeof(Header));

		Header.dwMagic = HDSSETTINGS_MAGIC;
		Header.dwHeaderSize = sizeof(Header);
		Header.lNrSettings = (LONG)m_lSettings.size();

		fwrite(&Header, sizeof(Header), 1, hFile);
		for (it = m_lSettings.begin(); it != m_lSettings.end(); it++)
			(*it).Save(hFile);

		fclose(hFile);
		bResult = true;
	};

	return bResult;
};
