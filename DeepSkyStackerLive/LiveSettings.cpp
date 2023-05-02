#include <stdafx.h>
#include "LiveSettings.h"

/* ------------------------------------------------------------------- */

void	DSS::LiveSettings::LoadFromRegistry()
{
	QSettings settings;
	settings.beginGroup("DeepSkyStackerLive");


	m_dwStackingFlags = settings.value("StackingFlags", 0U).toUInt();
	m_dwWarningFlags = settings.value("WarningFlags", 0U).toUInt();
	m_dwWarningActions = settings.value("WarningActions", 0U).toUInt();
	m_dwProcessFlags = settings.value("ProcessFlags", 0U).toUInt();
	m_dwMinImages = settings.value("MinImages", 0U).toUInt();
	m_dwScore = settings.value("Score", 0U).toUInt();
	m_dwStars = settings.value("Stars", 0U).toUInt();
	m_dwFWHM = settings.value("FWHM", 0U).toUInt();
	m_dwOffset = settings.value("Offset", 0U).toUInt();
	m_dwAngle = settings.value("Angle", 0U).toUInt();
	m_dwSkyBackground = settings.value("SkyBackground", 0U).toUInt();
	m_dwSaveCount = settings.value("SaveCount", 0U).toUInt();

	m_bDarkMode = settings.value("DarkMode", false).toBool();

	m_strFileFolder = settings.value("FileFolder", "").toString();
	m_strEmail = settings.value("Email", "").toString();
	m_strWarnFileFolder = settings.value("WarningFileFolder", "").toString();
	m_strStackedOutputFolder = settings.value("StackedOutputFolder", "").toString();

	m_strSMTP = settings.value("SMTP", "").toString();
	m_strAccount = settings.value("Account", "").toString();
	m_strObject = settings.value("Object", "").toString();
	settings.endGroup();

#if defined (Q_OS_WIN)
	if (!m_strSMTP.isEmpty() && !m_strAccount.isEmpty())
	{
		QSettings sysSettings("HKEY_CURRENT_USER\\Software\\Microsoft",
			QSettings::NativeFormat);
		sysSettings.beginGroup("Internet Account Manager/Accounts/00000001");
		m_strSMTP = sysSettings.value("SMTP Server", "").toString();
		m_strAccount = sysSettings.value("SMTP Email Address", "").toString();

		sysSettings.endGroup();
	};
#endif
};

/* ------------------------------------------------------------------- */

void	DSS::LiveSettings::SaveToRegistry()
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

	settings.setValue("DarkMode", m_bDarkMode);

	settings.setValue("FileFolder", m_strFileFolder);
	settings.setValue("WarningFileFolder", m_strWarnFileFolder);
	settings.setValue("StackedOutputFolder", m_strStackedOutputFolder);

	settings.setValue("Email", m_strEmail);
	settings.setValue("SMTP", m_strSMTP);
	settings.setValue("Account", m_strAccount);
	settings.setValue("Object", m_strObject);

	settings.endGroup();

};

/* ------------------------------------------------------------------- */
