/****************************************************************************
**
** Copyright (C) 2023 David C. Partridge
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
// ProgressLive.cpp : Implements most of the DSSLive Progress logic
#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "progresslive.h"
#include <Ztrace.h>

namespace
{
	QString stripString(const QString& s)
	{
		QString result{ s };
		result.replace('\n', ' ');
		return result + '\n';
	}
}

namespace DSS
{
	ProgressLive::ProgressLive(QObject* parent)
		: QObject(parent),
		total1{ 0 },
		total2{ 0 },
		achieved1{ 0 },
		achieved2{ 0 }
	{}

	ProgressLive::~ProgressLive()
	{}

	// DSSProgress methods
	void	ProgressLive::Start1(const QString& title, int t1, [[maybe_unused]] bool bEnableCancel)
	{
		if (!title.isEmpty())
		{
			progress1Text = title;
			DeepSkyStackerLive::instance()->writeToLog(stripString(title));
		};
		if (0 != t1)
			total1 = t1;
		achieved1 = 0;
	};

	/* ------------------------------------------------------------------- */

	void	ProgressLive::Progress1(const QString& text, int lAchieved1)
	{
		if (!text.isEmpty())
			progress1Text = text;
		if ((static_cast<double>(lAchieved1) - static_cast<double>(achieved1)) / static_cast<double>(total1) > 0.10)
		{
			emit progress(progress1Text, lAchieved1, total1);
			achieved1 = lAchieved1;
		};
	};

	/* ------------------------------------------------------------------- */

	void	ProgressLive::Start2(const QString& text, int lTotal2)
	{
		ZFUNCTRACE_RUNTIME();
		mutex.lock();
		if (!text.isEmpty())
		{
			progress2Text = text;
			DeepSkyStackerLive::instance()->writeToLog(stripString(text));
		};
		total2 = lTotal2;
		achieved2 = 0;
		emit progress(progress2Text, achieved2, total2);
	};

	/* ------------------------------------------------------------------- */

	void	ProgressLive::Progress2(const QString& text, int lAchieved2)
	{
		if (!text.isEmpty())
			progress2Text = text;
		if ((static_cast<double>(lAchieved2) - static_cast<double>(achieved2)) / static_cast<double>(total2) > 0.10 ||
			(lAchieved2 == total2))
		{
			emit progress(progress2Text, lAchieved2, total2);
			achieved2 = lAchieved2;
		};
	};

	/* ------------------------------------------------------------------- */

	void	ProgressLive::End2()
	{
		ZFUNCTRACE_RUNTIME();
		emit endProgress();
		mutex.unlock();
	};

	/* ------------------------------------------------------------------- */

	void ProgressLive::Close()
	{
		emit endProgress();
	};

	/* ------------------------------------------------------------------- */
}