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
#include "stdafx.h"
#include "LiveSettings.h"

/* ------------------------------------------------------------------- */
namespace DSS
{

	LiveSettings::LiveSettings() :
		m_dwStackingFlags { 0 },
		m_dwWarningActions{ 0 },
		m_dwMinImages{ 0 },
		m_dwScore{ 0 },
		m_dwStars{ 0 },
		m_dwSkyBackground{ 0 },
		m_dwFWHM{ 0 },
		m_dwOffset{ 0 },
		m_dwAngle{ 0 },
		m_dwSaveCount{ 0 },
		m_dwProcessFlags{ 0 }
	{
		ZFUNCTRACE_RUNTIME();
		load();
	}

	/* ------------------------------------------------------------------- */

	LiveSettings::~LiveSettings()
	{}

	/* ------------------------------------------------------------------- */

	void	LiveSettings::load()
	{
		QSettings settings;
		settings.beginGroup("DeepSkyStackerLive");

		m_dwStackingFlags = settings.value("StackingFlags", 0U).toUInt();
		m_dwWarningFlags = settings.value("WarningFlags", 0U).toUInt();
		m_dwWarningActions = settings.value("WarningActions", LSWA_SOUND).toUInt();
		m_dwProcessFlags = settings.value("ProcessFlags", LSPF_ALL).toUInt();
		m_dwMinImages = settings.value("MinImages", 5U).toUInt();
		m_dwScore = settings.value("Score", 1000U).toUInt();
		m_dwStars = settings.value("Stars", 30U).toUInt();
		m_dwFWHM = settings.value("FWHM", 50U).toUInt();
		m_dwOffset = settings.value("Offset", 100U).toUInt();
		m_dwAngle = settings.value("Angle", 20U).toUInt();
		m_dwSkyBackground = settings.value("SkyBackground", 20U).toUInt();
		m_dwSaveCount = settings.value("SaveCount", 10U).toUInt();

		m_strWarnFileFolder = settings.value("WarningFileFolder", "").toString();
		m_strStackedOutputFolder = settings.value("StackedOutputFolder", "").toString();

		emailTo = settings.value("EmailTo", "").toString();
		emailSubject = settings.value("EmailSubject", "").toString();
		smtpServer = settings.value("SMTPServer", "").toString();
		smtpPort = settings.value("SMTPPort", 587).toInt();
		smtpEncryption = settings.value("SMTPEncryption", 0U).toUInt();
		emailAccount = settings.value("EmailAccount", "").toString();
		emailPassword = settings.value("EmailPassword", "").toString();

#if defined (Q_OS_WIN)
		if (smtpServer.isEmpty() && emailAccount.isEmpty())
		{
			QSettings sysSettings("HKEY_CURRENT_USER\\Software\\Microsoft",
				QSettings::NativeFormat);
			sysSettings.beginGroup("Internet Account Manager/Accounts/00000001");
			smtpServer = sysSettings.value("SMTP Server", "").toString();
			emailAccount = sysSettings.value("SMTP Email Address", "").toString();

			sysSettings.endGroup();
		};
		if (smtpServer.isEmpty() && emailAccount.isEmpty())
		{
			QSettings sysSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\16.0\\Outlook\\Profiles\\Outlook",
				QSettings::NativeFormat);
			sysSettings.beginGroup("9375CFF0413111d3B88A00104B2A6676/00000002");
			smtpServer = sysSettings.value("SMTP Server", "").toString();
			emailAccount = sysSettings.value("POP3 User", "").toString();

			sysSettings.endGroup();
		}; 
#endif
	};

	/* ------------------------------------------------------------------- */

	void	LiveSettings::save()
	{
		QSettings settings;
		settings.beginGroup("DeepSkyStackerLive");


		settings.setValue("StackingFlags", m_dwStackingFlags);
		settings.setValue("WarningFlags", m_dwWarningFlags);
		settings.setValue("WarningActions", m_dwWarningActions);
		settings.setValue("ProcessFlags", m_dwProcessFlags);
		settings.setValue("MinImages", m_dwMinImages);
		settings.setValue("Score", m_dwScore);
		settings.setValue("Stars", m_dwStars);
		settings.setValue("FWHM", m_dwFWHM);
		settings.setValue("Offset", m_dwOffset);
		settings.setValue("Angle", m_dwAngle);
		settings.setValue("SkyBackground", m_dwSkyBackground);
		settings.setValue("SaveCount", m_dwSaveCount);

		settings.setValue("WarningFileFolder", m_strWarnFileFolder);
		settings.setValue("StackedOutputFolder", m_strStackedOutputFolder);

		settings.setValue("EmailTo", emailTo);
		settings.setValue("EmailSubject", emailSubject);
		settings.setValue("SMTPServer", smtpServer);
		settings.setValue("SMTPPort", smtpPort);
		settings.setValue("SMTPEncryption", smtpEncryption);
		settings.setValue("EmailAccount", emailAccount);
		settings.setValue("EmailPassword", emailPassword);

		settings.endGroup();

	};
}

