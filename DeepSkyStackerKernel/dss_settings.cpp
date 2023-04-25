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
#include "dss_settings.h"
#pragma pack(push, HDSETTINGS, 2)

namespace {
	void GetDefaultSettingsFileName(QString& strFile)
	{
		strFile = QString("%1%2DSSSettings.DSSSettings")
					.arg(QCoreApplication::applicationDirPath())
					.arg(QDir::separator());
	}
}

/* ------------------------------------------------------------------- */
constexpr std::uint32_t HDSSETTINGS_MAGIC = 0x7ABC6F10;

using HDSETTINGSHEADER = struct
{
	std::uint32_t	dwMagic;		// Magic number (always HDSSETTINGS_MAGIC)
	std::uint32_t	dwHeaderSize;	// Always sizeof(HDSETTINGSHEADER);
	std::int32_t	lNrSettings;	// Number of settings
	std::uint32_t	dwFlags;		// Flags
	char			Reserved[32];	// Reserved (set to 0)
};

#pragma pack(pop, HDSETTINGS)

/* ------------------------------------------------------------------- */
bool CDSSSettings::Load()
{
	QString szFile;
	if (szFile.isEmpty())
		GetDefaultSettingsFileName(szFile);

	if (FILE* hFile = _tfopen(szFile.toStdWString().c_str(), _T("rb")))
	{
		HDSETTINGSHEADER Header;
		fread(&Header, sizeof(Header), 1, hFile);

		if ((Header.dwMagic == HDSSETTINGS_MAGIC) &&
			(Header.dwHeaderSize == sizeof(Header)))
		{
			m_lSettings.clear();
			for (std::int32_t i = 0; i < Header.lNrSettings; i++)
			{
				CDSSSetting cds;

				cds.Load(hFile);
				m_lSettings.push_back(std::move(cds));
			}
			m_lSettings.sort();
			fclose(hFile);
			return true;
		}
		fclose(hFile);
	}
	return false;
}

/* ------------------------------------------------------------------- */

bool	CDSSSettings::Save()
{
	QString outFile;
	if (outFile.isEmpty())
		GetDefaultSettingsFileName(outFile);

	if (FILE* hFile = _tfopen(outFile.toStdWString().c_str(), _T("wb")))
	{
		m_lSettings.sort();

		HDSETTINGSHEADER		Header;
		memset(&Header, 0, sizeof(Header));

		Header.dwMagic = HDSSETTINGS_MAGIC;
		Header.dwHeaderSize = sizeof(Header);
		Header.lNrSettings = static_cast<decltype(Header.lNrSettings)>(m_lSettings.size());

		fwrite(&Header, sizeof(Header), 1, hFile);
		for (DSSSETTINGITERATOR it = m_lSettings.begin(); it != m_lSettings.end(); ++it)
			it->Save(hFile);

		fclose(hFile);
		return true;
	}
	return false;
}
