#include <stdafx.h>
#include "LiveSettings.h"
#include "Registry.h"

/* ------------------------------------------------------------------- */

void	CLiveSettings::LoadFromRegistry()
{
	CRegistry			reg;

	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "StackingFlags", m_dwStackingFlags);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "WarningFlags", m_dwWarningFlags);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "WarningActions", m_dwWarningActions);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "ProcessFlags", m_dwProcessFlags);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "MinImages", m_dwMinImages);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "Score", m_dwScore);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "Stars", m_dwStars);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "FWHM", m_dwFWHM);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "Offset", m_dwOffset);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "Angle", m_dwAngle);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "SkyBackground", m_dwSkyBackground);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "SaveCount", m_dwSaveCount);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "FileFolder", m_strFileFolder);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "Email", m_strEmail);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "WarningFileFolder", m_strWarnFileFolder);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "StackedOutputFolder", m_strStackedOutputFolder);

	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "Email", m_strEmail);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "SMTP", m_strSMTP);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "Account", m_strAccount);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, "Object", m_strObject);

	if (!m_strSMTP.GetLength() && !m_strAccount.GetLength())
	{
		reg.LoadKey("Software\\Microsoft\\Internet Account Manager\\Accounts\\00000001", "SMTP Server", m_strSMTP); 
		reg.LoadKey("Software\\Microsoft\\Internet Account Manager\\Accounts\\00000001", "SMTP Email Address", m_strAccount); 
	};
};

/* ------------------------------------------------------------------- */

void	CLiveSettings::SaveToRegistry()
{
	CRegistry			reg;

	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "StackingFlags", m_dwStackingFlags);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "WarningFlags", m_dwWarningFlags);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "WarningActions", m_dwWarningActions);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "ProcessFlags", m_dwProcessFlags);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "MinImages", m_dwMinImages);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "Score", m_dwScore);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "Stars", m_dwStars);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "FWHM", m_dwFWHM);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "Offset", m_dwOffset);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "Angle", m_dwAngle);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "SkyBackground", m_dwSkyBackground);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "SaveCount", m_dwSaveCount);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "FileFolder", m_strFileFolder);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "Email", m_strEmail);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "WarningFileFolder", m_strWarnFileFolder);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "StackedOutputFolder", m_strStackedOutputFolder);

	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "Email", m_strEmail);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "SMTP", m_strSMTP);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "Account", m_strAccount);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, "Object", m_strObject);
};

/* ------------------------------------------------------------------- */
