#pragma once
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
class MRUList
{
public:
	QString					baseKeyName;
	std::vector<QString>	m_vLists;
	int					m_lMaxLists;

private:
	void	CopyFrom(const MRUList& ml)
	{
		m_vLists = ml.m_vLists;
		m_lMaxLists = ml.m_lMaxLists;
	};

public:
	MRUList(char* baseKey = nullptr)
	{
		if (baseKey)
			baseKeyName = baseKey;
		else
			baseKeyName = "FileLists";
		m_lMaxLists = 10;
	};

	void setBasePath(const QString& baseKey)
	{
		baseKeyName = baseKey;
	};

	MRUList(const MRUList& ml)
	{
		CopyFrom(ml);
	};

	~MRUList() {};

	MRUList& operator = (const MRUList& ml)
	{
		CopyFrom(ml);
		return (*this);
	};

	void	readSettings();
	void	saveSettings();

	void	Add(const QString& list);
	inline void	Add(LPCTSTR szList)
	{
		QString list = QString::fromWCharArray(szList);
	}
};
