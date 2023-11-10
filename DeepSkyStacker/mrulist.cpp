/****************************************************************************
**
** Copyright (C) 2020, 2022 David C. Partridge
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
#include "mrulist.h"

void	MRUList::readSettings()
{
	QSettings settings;
	uint32_t count = 0;

	m_vLists.clear();

	QString keyName(baseKeyName);
	keyName += "/NrMRU";

	count = settings.value(keyName, 0).toUInt();

	for (uint32_t i = 0; i < count; i++)
	{
		QString keyName1 = QString("%1/MRU%2")
			.arg(baseKeyName).arg(i);

		QString value = settings.value(keyName1).toString();

		m_vLists.emplace_back(value);
	};
};

/* ------------------------------------------------------------------- */

void	MRUList::saveSettings()
{
	QSettings	settings;

	QString keyName(baseKeyName);

	// Clear all the entries first
	settings.remove(keyName);

	keyName += "/NrMRU";

	settings.setValue(keyName, (uint)m_vLists.size());
	for (int i = 0; i < m_vLists.size(); i++)
	{
		QString keyName1 = QString("%1/MRU%2")
			.arg(baseKeyName).arg(i);
		QString value(m_vLists[i]);

		settings.setValue(keyName1, value);
	};
};


/* ------------------------------------------------------------------- */

void	MRUList::Add(const QString& list)
{
	bool				bFound = false;
	int index = -1;

	for (int i = 0; i < m_vLists.size() && !bFound; i++)
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
			it += index;
			m_vLists.erase(it);
			m_vLists.insert(m_vLists.begin(), list);
		};
	}
	else
		m_vLists.insert(m_vLists.begin(), list);

	if (m_vLists.size() > m_lMaxLists)
		m_vLists.resize(m_lMaxLists);
};
