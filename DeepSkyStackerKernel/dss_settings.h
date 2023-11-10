#pragma once
#include "BezierAdjust.h"
#include "Histogram.h"
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
class CDSSSetting
{
public:
	QString				m_strName;
	CBezierAdjust		m_BezierAdjust;
	CRGBHistogramAdjust	m_HistoAdjust;

private:
	void	CopyFrom(const CDSSSetting& cds)
	{
		m_strName = cds.m_strName;
		m_BezierAdjust = cds.m_BezierAdjust;
		m_HistoAdjust = cds.m_HistoAdjust;
	};

public:
	CDSSSetting() {};
	virtual ~CDSSSetting() {};

	CDSSSetting(const CDSSSetting& cds)
	{
		CopyFrom(cds);
	};

	CDSSSetting& operator = (const CDSSSetting& cds)
	{
		CopyFrom(cds);
		return (*this);
	};

	bool operator < (const CDSSSetting& cds) const
	{
		int			nCompare;
		nCompare = m_strName.compare(cds.m_strName, Qt::CaseInsensitive);

		if (nCompare < 0)
			return true;
		else
			return false;
	};

	bool	Load(FILE* hFile)
	{
		int		lNameSize;
		TCHAR		szName[2000] = { _T('\0') };

		fread(&lNameSize, sizeof(lNameSize), 1, hFile);
		fread(szName, sizeof(TCHAR), lNameSize, hFile);
		m_strName = QString::fromWCharArray(&szName[0]);
		return m_BezierAdjust.Load(hFile) && m_HistoAdjust.Load(hFile);
	};

	bool	Save(FILE* hFile)
	{
		int		lNameSize = m_strName.length() + 1;
		fwrite(&lNameSize, sizeof(lNameSize), 1, hFile);
		fwrite(m_strName.toStdWString().c_str(), sizeof(TCHAR), lNameSize, hFile);

		return m_BezierAdjust.Save(hFile) && m_HistoAdjust.Save(hFile);
	};
};

typedef std::list<CDSSSetting>			DSSSETTINGLIST;
typedef	DSSSETTINGLIST::iterator		DSSSETTINGITERATOR;

class CDSSSettings
{
private:
	std::list<CDSSSetting>	m_lSettings;
	bool					m_bLoaded;

public:
	CDSSSettings()
	{
		m_bLoaded = false;
	};
	virtual ~CDSSSettings() {};

	bool	IsLoaded()
	{
		return m_bLoaded;
	};
	bool Load();
	bool Save();

	int	Count()
	{
		return static_cast<int>(m_lSettings.size());
	};

	bool	GetItem(int lIndice, CDSSSetting& cds)
	{
		bool			bResult = false;

		if (lIndice < static_cast<int>(m_lSettings.size()))
		{
			DSSSETTINGITERATOR	it;

			it = m_lSettings.begin();
			while (lIndice)
			{
				it++;
				lIndice--;
			};

			cds = (*it);
			bResult = true;
		};

		return bResult;
	};

	bool	Add(const CDSSSetting& cds)
	{
		m_lSettings.push_back(cds);
		return true;
	};

	bool	Remove(int lIndice)
	{
		bool			bResult = false;

		if (lIndice < static_cast<int>(m_lSettings.size()))
		{
			DSSSETTINGITERATOR	it;

			it = m_lSettings.begin();
			while (lIndice)
			{
				it++;
				lIndice--;
			};

			m_lSettings.erase(it);
			bResult = true;
		};

		return bResult;
	};
};

