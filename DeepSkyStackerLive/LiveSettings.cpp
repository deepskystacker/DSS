#include <stdafx.h>
#include "LiveSettings.h"
#include "Registry.h"

/* ------------------------------------------------------------------- */

void	CLiveSettings::LoadFromRegistry()
{
	CRegistry			reg;

	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("StackingFlags"), m_dwStackingFlags);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("WarningFlags"), m_dwWarningFlags);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("WarningActions"), m_dwWarningActions);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("ProcessFlags"), m_dwProcessFlags);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("MinImages"), m_dwMinImages);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Score"), m_dwScore);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Stars"), m_dwStars);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("FWHM"), m_dwFWHM);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Offset"), m_dwOffset);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Angle"), m_dwAngle);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("SkyBackground"), m_dwSkyBackground);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("SaveCount"), m_dwSaveCount);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("FileFolder"), m_strFileFolder);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Email"), m_strEmail);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("WarningFileFolder"), m_strWarnFileFolder);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("StackedOutputFolder"), m_strStackedOutputFolder);

	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Email"), m_strEmail);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("SMTP"), m_strSMTP);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Account"), m_strAccount);
	reg.LoadKey(REGENTRY_BASEKEY_LIVE, _T("Object"), m_strObject);

	if (!m_strSMTP.GetLength() && !m_strAccount.GetLength())
	{
		reg.LoadKey(_T("Software\\Microsoft\\Internet Account Manager\\Accounts\\00000001"), _T("SMTP Server"), m_strSMTP); 
		reg.LoadKey(_T("Software\\Microsoft\\Internet Account Manager\\Accounts\\00000001"), _T("SMTP Email Address"), m_strAccount); 
	};
};

/* ------------------------------------------------------------------- */

void	CLiveSettings::SaveToRegistry()
{
	CRegistry			reg;

	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("StackingFlags"), m_dwStackingFlags);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("WarningFlags"), m_dwWarningFlags);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("WarningActions"), m_dwWarningActions);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("ProcessFlags"), m_dwProcessFlags);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("MinImages"), m_dwMinImages);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("Score"), m_dwScore);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("Stars"), m_dwStars);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("FWHM"), m_dwFWHM);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("Offset"), m_dwOffset);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("Angle"), m_dwAngle);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("SkyBackground"), m_dwSkyBackground);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("SaveCount"), m_dwSaveCount);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("FileFolder"), m_strFileFolder);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("Email"), m_strEmail);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("WarningFileFolder"), m_strWarnFileFolder);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("StackedOutputFolder"), m_strStackedOutputFolder);

	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("Email"), m_strEmail);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("SMTP"), m_strSMTP);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("Account"), m_strAccount);
	reg.SaveKey(REGENTRY_BASEKEY_LIVE, _T("Object"), m_strObject);
};

/* ------------------------------------------------------------------- */
