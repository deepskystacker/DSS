#include <stdafx.h>
#include "LiveSettings.h"

/* ------------------------------------------------------------------- */

void	CLiveSettings::LoadFromRegistry()
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

	m_strFileFolder = settings.value("FileFolder", "").toString().toStdWString().c_str();
	m_strEmail = settings.value("Email", "").toString().toStdWString().c_str();
	m_strWarnFileFolder = settings.value("WarningFileFolder", "").toString().toStdWString().c_str();
	m_strStackedOutputFolder = settings.value("StackedOutputFolder", "").toString().toStdWString().c_str();

	m_strSMTP = settings.value("SMTP", "").toString().toStdWString().c_str();
	m_strAccount = settings.value("Account", "").toString().toStdWString().c_str();
	m_strObject = settings.value("Object", "").toString().toStdWString().c_str();
	settings.endGroup();

#if defined (Q_OS_WIN)
	if (!m_strSMTP.GetLength() && !m_strAccount.GetLength())
	{
		QSettings sysSettings("HKEY_CURRENT_USER\\Software\\Microsoft",
			QSettings::NativeFormat);
		sysSettings.beginGroup("Internet Account Manager/Accounts/00000001");
		m_strSMTP = sysSettings.value("SMTP Server", "").toString().toStdWString().c_str();
		m_strAccount = sysSettings.value("SMTP Email Address", "").toString().toStdWString().c_str();

		sysSettings.endGroup();
	};
#endif
};

/* ------------------------------------------------------------------- */

void	CLiveSettings::SaveToRegistry()
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

	settings.setValue("FileFolder", QString::fromStdWString(m_strFileFolder.GetString()));
	settings.setValue("WarningFileFolder", QString::fromStdWString(m_strWarnFileFolder.GetString()));
	settings.setValue("StackedOutputFolder", QString::fromStdWString(m_strStackedOutputFolder.GetString()));

	settings.setValue("Email", QString::fromStdWString(m_strEmail.GetString()));
	settings.setValue("SMTP", QString::fromStdWString(m_strSMTP.GetString()));
	settings.setValue("Account", QString::fromStdWString(m_strAccount.GetString()));
	settings.setValue("Object", QString::fromStdWString(m_strObject.GetString()));

	settings.endGroup();

};

/* ------------------------------------------------------------------- */
