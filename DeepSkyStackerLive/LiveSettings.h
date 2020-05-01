#ifndef __LIVESETTINGS_H__
#define __LIVESETTINGS_H__

const DWORD					LSWF_SCORE	= 0x00000001L;	// Warning Flags
const DWORD					LSWF_STARS	= 0x00000002L;
const DWORD					LSWF_FWHM	= 0x00000004L;
const DWORD					LSWF_OFFSET = 0x00000008L;
const DWORD					LSWF_ANGLE	= 0x00000010L;
const DWORD					LSWF_SKY	= 0x00000020L;

const DWORD					LSSF_SCORE	= 0x00000001L;	// Stacking Flags
const DWORD					LSSF_STARS	= 0x00000002L;
const DWORD					LSSF_FWHM	= 0x00000004L;
const DWORD					LSSF_OFFSET = 0x00000008L;
const DWORD					LSSF_ANGLE	= 0x00000010L;
const DWORD					LSSF_SKY	= 0x00000020L;
const DWORD					LSSF_DELAYED= 0x00010000L;
const DWORD					LSSF_SAVE	= 0x00020000L;
const DWORD					LSSF_MOVE	= 0x00040000L;

const DWORD					LSWA_SOUND	= 0x00000001L;	// Warning Actions
const DWORD					LSWA_FLASH	= 0x00000002L;
const DWORD					LSWA_EMAIL	= 0x00000004L;
const DWORD					LSWA_FILE	= 0x00000008L;
const DWORD					LSWA_SENDMULTIPLEEMAILS = 0x00000010L;

const DWORD					LSPF_RAW	= 0x00000001L;  // Process Flags
const DWORD					LSPF_FITS	= 0x00000002L;  // Process Flags
const DWORD					LSPF_TIFF	= 0x00000004L;  // Process Flags
const DWORD					LSPF_OTHERS	= 0x00000008L;  // Process Flags
const DWORD					LSPF_ALL	= 0x0000000FL;  // Process Flags

class CLiveSettings
{
private :
	DWORD				m_dwStackingFlags;
	DWORD				m_dwWarningFlags;
	DWORD				m_dwWarningActions;
	DWORD				m_dwMinImages;
	DWORD				m_dwScore;
	DWORD				m_dwStars;
	DWORD				m_dwFWHM;
	DWORD				m_dwOffset;
	DWORD				m_dwAngle;
	DWORD				m_dwSaveCount;
	DWORD				m_dwProcessFlags;
	DWORD				m_dwSkyBackground;
	CString				m_strFileFolder;
	CString				m_strWarnFileFolder;
	CString				m_strStackedOutputFolder;

	CString				m_strEmail;
	CString				m_strSMTP;
	CString				m_strAccount;
	CString				m_strObject;
	
	bool				m_bDarkMode;

public :
	CLiveSettings()
	{
		m_dwStackingFlags = 0;
		m_dwWarningFlags  = 0;
		m_dwWarningActions = LSWA_SOUND;
		m_dwMinImages	  = 5;
		m_dwScore		  = 1000;
		m_dwStars		  = 30;
		m_dwFWHM		  = 50;
		m_dwOffset		  = 100;
		m_dwAngle		  = 20;
		m_dwSaveCount	  = 10;
		m_dwSkyBackground = 20;
		m_dwProcessFlags  = LSPF_ALL;
		m_bDarkMode = true;
	};

	~CLiveSettings()
	{
	};

	void	LoadFromRegistry();
	void	SaveToRegistry();

	BOOL	IsDontStack_Score()	{	return (m_dwStackingFlags & LSSF_SCORE) ? TRUE : FALSE; };
	BOOL	IsDontStack_Stars()	{	return (m_dwStackingFlags & LSSF_STARS) ? TRUE : FALSE; };
	BOOL	IsDontStack_FWHM()	{	return (m_dwStackingFlags & LSSF_FWHM) ? TRUE : FALSE; };
	BOOL	IsDontStack_Offset(){	return (m_dwStackingFlags & LSSF_OFFSET) ? TRUE : FALSE; };
	BOOL	IsDontStack_Angle()	{	return (m_dwStackingFlags & LSSF_ANGLE) ? TRUE : FALSE; };
	BOOL	IsDontStack_SkyBackground()	{	return (m_dwStackingFlags & LSSF_SKY) ? TRUE : FALSE; };
	BOOL	IsDontStack_Delayed()	{	return (m_dwStackingFlags & LSSF_DELAYED) ? TRUE : FALSE; };
	BOOL	IsStack_Save()	{	return (m_dwStackingFlags & LSSF_SAVE) ? TRUE : FALSE; };
	BOOL	IsStack_Move()	{	return (m_dwStackingFlags & LSSF_MOVE) ? TRUE : FALSE; };

	BOOL	IsWarning_Score()	{	return (m_dwWarningFlags & LSWF_SCORE) ? TRUE : FALSE; };
	BOOL	IsWarning_Stars()	{	return (m_dwWarningFlags & LSWF_STARS) ? TRUE : FALSE; };
	BOOL	IsWarning_FWHM()	{	return (m_dwWarningFlags & LSWF_FWHM) ? TRUE : FALSE; };
	BOOL	IsWarning_Offset()	{	return (m_dwWarningFlags & LSWF_OFFSET) ? TRUE : FALSE; };
	BOOL	IsWarning_Angle()	{	return (m_dwWarningFlags & LSWF_ANGLE) ? TRUE : FALSE; };
	BOOL	IsWarning_SkyBackground()	{	return (m_dwWarningFlags & LSWF_SKY) ? TRUE : FALSE; };

	BOOL	IsWarning_Sound()	{	return (m_dwWarningActions & LSWA_SOUND) ? TRUE : FALSE; };
	BOOL	IsWarning_Flash()	{	return (m_dwWarningActions & LSWA_FLASH) ? TRUE : FALSE; };
	BOOL	IsWarning_Email()	{	return (m_dwWarningActions & LSWA_EMAIL) ? TRUE : FALSE; };
	BOOL	IsWarning_File()	{	return (m_dwWarningActions & LSWA_FILE) ? TRUE : FALSE; };
	BOOL	IsWarning_SendMultipleEmails()	{	return (m_dwWarningActions & LSWA_SENDMULTIPLEEMAILS) ? TRUE : FALSE; };

	BOOL	IsProcess_RAW()		{	return (m_dwProcessFlags & LSPF_RAW) ? TRUE : FALSE; };
	BOOL	IsProcess_FITS()	{	return (m_dwProcessFlags & LSPF_FITS) ? TRUE : FALSE; };
	BOOL	IsProcess_TIFF()	{	return (m_dwProcessFlags & LSPF_TIFF) ? TRUE : FALSE; };
	BOOL	IsProcess_Others()	{	return (m_dwProcessFlags & LSPF_OTHERS) ? TRUE : FALSE; };

	bool	UseDarkTheme() const { return m_bDarkMode; }
	void	UseDarkTheme(bool bState) { m_bDarkMode=bState; }

	void	SetDontStack_Score(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_SCORE) : (m_dwStackingFlags &=~LSSF_SCORE);};
	void	SetDontStack_Stars(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_STARS) : (m_dwStackingFlags &=~LSSF_STARS);};
	void	SetDontStack_FWHM(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_FWHM) : (m_dwStackingFlags &=~LSSF_FWHM);};
	void	SetDontStack_Offset(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_OFFSET) : (m_dwStackingFlags &=~LSSF_OFFSET);};
	void	SetDontStack_Angle(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_ANGLE) : (m_dwStackingFlags &=~LSSF_ANGLE);};
	void	SetDontStack_SkyBackground(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_SKY) : (m_dwStackingFlags &=~LSSF_SKY);};
	void	SetDontStack_Delayed(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_DELAYED) : (m_dwStackingFlags &=~LSSF_DELAYED);};
	void	SetStack_Save(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_SAVE) : (m_dwStackingFlags &=~LSSF_SAVE);};
	void	SetStack_Move(BOOL bSet)	{ bSet ? (m_dwStackingFlags |= LSSF_MOVE) : (m_dwStackingFlags &=~LSSF_MOVE);};

	void	SetWarning_Score(BOOL bSet)		{ bSet ? (m_dwWarningFlags |= LSWF_SCORE) : (m_dwWarningFlags &=~LSWF_SCORE);};
	void	SetWarning_Stars(BOOL bSet)		{ bSet ? (m_dwWarningFlags |= LSWF_STARS) : (m_dwWarningFlags &=~LSWF_STARS);};
	void	SetWarning_FWHM(BOOL bSet)		{ bSet ? (m_dwWarningFlags |= LSWF_FWHM) : (m_dwWarningFlags &=~LSWF_FWHM);};
	void	SetWarning_Offset(BOOL bSet)	{ bSet ? (m_dwWarningFlags |= LSWF_OFFSET) : (m_dwWarningFlags &=~LSWF_OFFSET);};
	void	SetWarning_Angle(BOOL bSet)		{ bSet ? (m_dwWarningFlags |= LSWF_ANGLE) : (m_dwWarningFlags &=~LSWF_ANGLE);};
	void	SetWarning_SkyBackground(BOOL bSet)		{ bSet ? (m_dwWarningFlags |= LSWF_SKY) : (m_dwWarningFlags &=~LSWF_SKY);};

	void	SetWarning_Sound(BOOL bSet)		{ bSet ? (m_dwWarningActions |= LSWA_SOUND) : (m_dwWarningActions &=~LSWA_SOUND);};
	void	SetWarning_Flash(BOOL bSet)		{ bSet ? (m_dwWarningActions |= LSWA_FLASH) : (m_dwWarningActions &=~LSWA_FLASH);};
	void	SetWarning_Email(BOOL bSet)		{ bSet ? (m_dwWarningActions |= LSWA_EMAIL) : (m_dwWarningActions &=~LSWA_EMAIL);};
	void	SetWarning_File(BOOL bSet)		{ bSet ? (m_dwWarningActions |= LSWA_FILE) : (m_dwWarningActions &=~LSWA_FILE);};
	void	SetWarning_SendMultipleEmails(BOOL bSet)		{ bSet ? (m_dwWarningActions |= LSWA_SENDMULTIPLEEMAILS) : (m_dwWarningActions &=~LSWA_SENDMULTIPLEEMAILS);};

	void	GetWarning_FileFolder(CString & strFolder)		{ strFolder = m_strWarnFileFolder;};
	void	SetWarning_FileFolder(LPCTSTR szFolder)		{ m_strWarnFileFolder = szFolder;};

	void	GetStackedOutputFolder(CString & strFolder)		{ strFolder = m_strStackedOutputFolder;};
	void	SetStackedOutputFolder(LPCTSTR szFolder)		{ m_strStackedOutputFolder = szFolder;};

	void	SetProcess_RAW(BOOL bSet)		{ bSet ? (m_dwProcessFlags |= LSPF_RAW) : (m_dwProcessFlags &=~LSPF_RAW);};
	void	SetProcess_FITS(BOOL bSet)		{ bSet ? (m_dwProcessFlags |= LSPF_FITS) : (m_dwProcessFlags &=~LSPF_FITS);};
	void	SetProcess_TIFF(BOOL bSet)		{ bSet ? (m_dwProcessFlags |= LSPF_TIFF) : (m_dwProcessFlags &=~LSPF_TIFF);};
	void	SetProcess_Others(BOOL bSet)	{ bSet ? (m_dwProcessFlags |= LSPF_OTHERS) : (m_dwProcessFlags &=~LSPF_OTHERS);};

	void	GetEmailSettings(CString & strEmail, CString & strAccount, CString & strSMTP, CString & strObject)
	{
		strEmail	= m_strEmail;
		strSMTP		= m_strSMTP;
		strObject	= m_strObject;
		strAccount	= m_strAccount;
	};

	void	GetEmailSettings(CString & strEmail)
	{
		strEmail	= m_strEmail;
	};

	void	SetEmailSettings(LPCTSTR szEmail, LPCTSTR szAccount, LPCTSTR szSMTP, LPCTSTR szObject)
	{
		m_strEmail	= szEmail;
		m_strSMTP	= szSMTP;
		m_strObject	= szObject;
		m_strAccount= szAccount;
	};

	double	GetScore()
	{
		return (double)m_dwScore/10.0;
	};

	void	SetScore(double fScore)
	{
		m_dwScore = fScore*10.0;
	};

	DWORD	GetStars()
	{
		return (double)m_dwStars;
	};

	void	SetStars(DWORD dwStars)
	{
		m_dwStars = dwStars;
	};

	DWORD	GetMinImages()
	{
		return (double)m_dwMinImages;
	};

	void	SetMinImages(DWORD dwMinImages)
	{
		m_dwMinImages = dwMinImages;
	};

	DWORD	GetSaveCount()
	{
		return (double)m_dwSaveCount;
	};

	void	SetSaveCount(DWORD dwSaveCount)
	{
		m_dwSaveCount= dwSaveCount;
	};

	double	GetFWHM()
	{
		return (double)m_dwFWHM/10.0;
	};

	void	SetFWHM(double fFWHM)
	{
		m_dwFWHM = fFWHM*10.0;
	};

	DWORD	GetSkyBackground()
	{
		return m_dwSkyBackground;
	};

	void	SetSkyBackground(DWORD dwSkyBackground)
	{
		m_dwSkyBackground = dwSkyBackground;
	};

	double	GetOffset()
	{
		return (double)m_dwOffset/10.0;
	};

	void	SetOffset(double fOffset)
	{
		m_dwOffset = fOffset*10.0;
	};

	double	GetAngle()
	{
		return (double)m_dwAngle/10.0;
	};

	void	SetAngle(double fAngle)
	{
		m_dwAngle = fAngle*10.0;
	};

	void	GetFileFolder(CString & strFileFolder)
	{
		strFileFolder = m_strFileFolder;
	};

	void	SetFileFolder(LPCTSTR szFileFolder)
	{
		m_strFileFolder = szFileFolder;
	};
};

#endif