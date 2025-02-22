#pragma once
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
#include <QLocale>
#include <QString>
#include "BezierAdjust.h"
#include "histogram.h"


namespace DSS
{
	class ProcessingSettings
	{
	public:
		mutable QString		name_;
		BezierAdjust		bezierAdjust_;
		RGBHistogramAdjust	histoAdjust_;

	private:
		void	CopyFrom(const ProcessingSettings& rhs)
		{
			name_ = rhs.name_;
			bezierAdjust_ = rhs.bezierAdjust_;
			histoAdjust_ = rhs.histoAdjust_;
		};

	public:
		ProcessingSettings(const QString& name = "") :
			name_ {name}
		{
			if (!name_.isEmpty()) loadSettings();
		};

		virtual ~ProcessingSettings()
		{
		};

		ProcessingSettings(const ProcessingSettings& rhs)
		{
			CopyFrom(rhs);
		};

		ProcessingSettings& operator = (const ProcessingSettings& rhs)
		{
			CopyFrom(rhs);
			return (*this);
		};

		bool operator < (const ProcessingSettings& rhs) const
		{
			//
			// Case independent local aware comparison
			//
			QLocale locale;
			int			nCompare;
			QString myName{ locale.toLower(name_) }, rhsName{ locale.toLower(rhs.name_) };
			nCompare = myName.localeAwareCompare(rhsName);

			if (nCompare < 0)
				return true;
			else
				return false;
		};

		void loadSettings()
		{
			ZFUNCTRACE_RUNTIME();
			const QString groupName{ "ProcessingSettings/" + name_ };
			qDebug() << "Load settings for " << groupName;
			bezierAdjust_.loadSettings(groupName);
			histoAdjust_.loadSettings(groupName);
		};

		void saveSettings() const
		{
			ZFUNCTRACE_RUNTIME();
			const QString groupName{ "ProcessingSettings/" + name_ };
			qDebug() << "Save settings for " << groupName;
			bezierAdjust_.saveSettings(groupName);
			histoAdjust_.saveSettings(groupName);
		};
	};

}
