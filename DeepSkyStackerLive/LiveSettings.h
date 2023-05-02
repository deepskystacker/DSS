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

const std::uint32_t 	LSWF_SCORE	= 0x00000001L;	// Warning Flags
const std::uint32_t 	LSWF_STARS	= 0x00000002L;
const std::uint32_t 	LSWF_FWHM	= 0x00000004L;
const std::uint32_t 	LSWF_OFFSET = 0x00000008L;
const std::uint32_t 	LSWF_ANGLE	= 0x00000010L;
const std::uint32_t 	LSWF_SKY	= 0x00000020L;

const std::uint32_t 	LSSF_SCORE	= 0x00000001L;	// Stacking Flags
const std::uint32_t 	LSSF_STARS	= 0x00000002L;
const std::uint32_t 	LSSF_FWHM	= 0x00000004L;
const std::uint32_t 	LSSF_OFFSET = 0x00000008L;
const std::uint32_t 	LSSF_ANGLE	= 0x00000010L;
const std::uint32_t 	LSSF_SKY	= 0x00000020L;
const std::uint32_t 	LSSF_DELAYED= 0x00010000L;
const std::uint32_t 	LSSF_SAVE	= 0x00020000L;
const std::uint32_t 	LSSF_MOVE	= 0x00040000L;

const std::uint32_t 	LSWA_SOUND	= 0x00000001L;	// Warning Actions
const std::uint32_t 	LSWA_FLASH	= 0x00000002L;
const std::uint32_t 	LSWA_EMAIL	= 0x00000004L;
const std::uint32_t 	LSWA_FILE	= 0x00000008L;
const std::uint32_t 	LSWA_SENDMULTIPLEEMAILS = 0x00000010L;

const std::uint32_t 	LSPF_RAW	= 0x00000001L;  // Process Flags
const std::uint32_t 	LSPF_FITS	= 0x00000002L;  // Process Flags
const std::uint32_t 	LSPF_TIFF	= 0x00000004L;  // Process Flags
const std::uint32_t 	LSPF_OTHERS	= 0x00000008L;  // Process Flags
const std::uint32_t 	LSPF_ALL	= 0x0000000FL;  // Process Flags

namespace DSS
{
	class LiveSettings
	{
	private:
		std::uint32_t m_dwStackingFlags;
		std::uint32_t m_dwWarningFlags;
		std::uint32_t m_dwWarningActions;
		std::uint32_t m_dwMinImages;
		std::uint32_t m_dwScore;
		std::uint32_t m_dwStars;
		std::uint32_t m_dwFWHM;
		std::uint32_t m_dwOffset;
		std::uint32_t m_dwAngle;
		std::uint32_t m_dwSaveCount;
		std::uint32_t m_dwProcessFlags;
		std::uint32_t m_dwSkyBackground;
		QString				m_strFileFolder;
		QString				m_strWarnFileFolder;
		QString				m_strStackedOutputFolder;

		QString				m_strEmail;
		QString				m_strSMTP;
		QString				m_strAccount;
		QString				m_strObject;

		bool				m_bDarkMode;

	public:
		LiveSettings()
		{
			m_dwStackingFlags = 0;
			m_dwWarningFlags = 0;
			m_dwWarningActions = LSWA_SOUND;
			m_dwMinImages = 5;
			m_dwScore = 1000;
			m_dwStars = 30;
			m_dwFWHM = 50;
			m_dwOffset = 100;
			m_dwAngle = 20;
			m_dwSaveCount = 10;
			m_dwSkyBackground = 20;
			m_dwProcessFlags = LSPF_ALL;
			m_bDarkMode = true;
		};

		~LiveSettings()
		{
		};

		void	LoadFromRegistry();
		void	SaveToRegistry();

		BOOL	IsDontStack_Score() { return (m_dwStackingFlags & LSSF_SCORE) ? TRUE : FALSE; };
		BOOL	IsDontStack_Stars() { return (m_dwStackingFlags & LSSF_STARS) ? TRUE : FALSE; };
		BOOL	IsDontStack_FWHM() { return (m_dwStackingFlags & LSSF_FWHM) ? TRUE : FALSE; };
		BOOL	IsDontStack_Offset() { return (m_dwStackingFlags & LSSF_OFFSET) ? TRUE : FALSE; };
		BOOL	IsDontStack_Angle() { return (m_dwStackingFlags & LSSF_ANGLE) ? TRUE : FALSE; };
		BOOL	IsDontStack_SkyBackground() { return (m_dwStackingFlags & LSSF_SKY) ? TRUE : FALSE; };
		BOOL	IsDontStack_Delayed() { return (m_dwStackingFlags & LSSF_DELAYED) ? TRUE : FALSE; };
		BOOL	IsStack_Save() { return (m_dwStackingFlags & LSSF_SAVE) ? TRUE : FALSE; };
		BOOL	IsStack_Move() { return (m_dwStackingFlags & LSSF_MOVE) ? TRUE : FALSE; };

		BOOL	IsWarning_Score() { return (m_dwWarningFlags & LSWF_SCORE) ? TRUE : FALSE; };
		BOOL	IsWarning_Stars() { return (m_dwWarningFlags & LSWF_STARS) ? TRUE : FALSE; };
		BOOL	IsWarning_FWHM() { return (m_dwWarningFlags & LSWF_FWHM) ? TRUE : FALSE; };
		BOOL	IsWarning_Offset() { return (m_dwWarningFlags & LSWF_OFFSET) ? TRUE : FALSE; };
		BOOL	IsWarning_Angle() { return (m_dwWarningFlags & LSWF_ANGLE) ? TRUE : FALSE; };
		BOOL	IsWarning_SkyBackground() { return (m_dwWarningFlags & LSWF_SKY) ? TRUE : FALSE; };

		BOOL	IsWarning_Sound() { return (m_dwWarningActions & LSWA_SOUND) ? TRUE : FALSE; };
		BOOL	IsWarning_Flash() { return (m_dwWarningActions & LSWA_FLASH) ? TRUE : FALSE; };
		BOOL	IsWarning_Email() { return (m_dwWarningActions & LSWA_EMAIL) ? TRUE : FALSE; };
		BOOL	IsWarning_File() { return (m_dwWarningActions & LSWA_FILE) ? TRUE : FALSE; };
		BOOL	IsWarning_SendMultipleEmails() { return (m_dwWarningActions & LSWA_SENDMULTIPLEEMAILS) ? TRUE : FALSE; };

		BOOL	IsProcess_RAW() { return (m_dwProcessFlags & LSPF_RAW) ? TRUE : FALSE; };
		BOOL	IsProcess_FITS() { return (m_dwProcessFlags & LSPF_FITS) ? TRUE : FALSE; };
		BOOL	IsProcess_TIFF() { return (m_dwProcessFlags & LSPF_TIFF) ? TRUE : FALSE; };
		BOOL	IsProcess_Others() { return (m_dwProcessFlags & LSPF_OTHERS) ? TRUE : FALSE; };

		bool	UseDarkTheme() const { return m_bDarkMode; }
		void	UseDarkTheme(bool bState) { m_bDarkMode = bState; }

		void	SetDontStack_Score(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_SCORE) : (m_dwStackingFlags &= ~LSSF_SCORE); };
		void	SetDontStack_Stars(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_STARS) : (m_dwStackingFlags &= ~LSSF_STARS); };
		void	SetDontStack_FWHM(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_FWHM) : (m_dwStackingFlags &= ~LSSF_FWHM); };
		void	SetDontStack_Offset(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_OFFSET) : (m_dwStackingFlags &= ~LSSF_OFFSET); };
		void	SetDontStack_Angle(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_ANGLE) : (m_dwStackingFlags &= ~LSSF_ANGLE); };
		void	SetDontStack_SkyBackground(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_SKY) : (m_dwStackingFlags &= ~LSSF_SKY); };
		void	SetDontStack_Delayed(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_DELAYED) : (m_dwStackingFlags &= ~LSSF_DELAYED); };
		void	SetStack_Save(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_SAVE) : (m_dwStackingFlags &= ~LSSF_SAVE); };
		void	SetStack_Move(BOOL bSet) { bSet ? (m_dwStackingFlags |= LSSF_MOVE) : (m_dwStackingFlags &= ~LSSF_MOVE); };

		void	SetWarning_Score(BOOL bSet) { bSet ? (m_dwWarningFlags |= LSWF_SCORE) : (m_dwWarningFlags &= ~LSWF_SCORE); };
		void	SetWarning_Stars(BOOL bSet) { bSet ? (m_dwWarningFlags |= LSWF_STARS) : (m_dwWarningFlags &= ~LSWF_STARS); };
		void	SetWarning_FWHM(BOOL bSet) { bSet ? (m_dwWarningFlags |= LSWF_FWHM) : (m_dwWarningFlags &= ~LSWF_FWHM); };
		void	SetWarning_Offset(BOOL bSet) { bSet ? (m_dwWarningFlags |= LSWF_OFFSET) : (m_dwWarningFlags &= ~LSWF_OFFSET); };
		void	SetWarning_Angle(BOOL bSet) { bSet ? (m_dwWarningFlags |= LSWF_ANGLE) : (m_dwWarningFlags &= ~LSWF_ANGLE); };
		void	SetWarning_SkyBackground(BOOL bSet) { bSet ? (m_dwWarningFlags |= LSWF_SKY) : (m_dwWarningFlags &= ~LSWF_SKY); };

		void	SetWarning_Sound(BOOL bSet) { bSet ? (m_dwWarningActions |= LSWA_SOUND) : (m_dwWarningActions &= ~LSWA_SOUND); };
		void	SetWarning_Flash(BOOL bSet) { bSet ? (m_dwWarningActions |= LSWA_FLASH) : (m_dwWarningActions &= ~LSWA_FLASH); };
		void	SetWarning_Email(BOOL bSet) { bSet ? (m_dwWarningActions |= LSWA_EMAIL) : (m_dwWarningActions &= ~LSWA_EMAIL); };
		void	SetWarning_File(BOOL bSet) { bSet ? (m_dwWarningActions |= LSWA_FILE) : (m_dwWarningActions &= ~LSWA_FILE); };
		void	SetWarning_SendMultipleEmails(BOOL bSet) { bSet ? (m_dwWarningActions |= LSWA_SENDMULTIPLEEMAILS) : (m_dwWarningActions &= ~LSWA_SENDMULTIPLEEMAILS); };

		void	GetWarning_FileFolder(QString& strFolder) { strFolder = m_strWarnFileFolder; };
		void	SetWarning_FileFolder(const QString& folder) { m_strWarnFileFolder = folder; };

		QString	getStackedOutputFolder() { return m_strStackedOutputFolder; };
		void	setStackedOutputFolder(const QString& folder) { m_strStackedOutputFolder = folder; };

		void	SetProcess_RAW(BOOL bSet) { bSet ? (m_dwProcessFlags |= LSPF_RAW) : (m_dwProcessFlags &= ~LSPF_RAW); };
		void	SetProcess_FITS(BOOL bSet) { bSet ? (m_dwProcessFlags |= LSPF_FITS) : (m_dwProcessFlags &= ~LSPF_FITS); };
		void	SetProcess_TIFF(BOOL bSet) { bSet ? (m_dwProcessFlags |= LSPF_TIFF) : (m_dwProcessFlags &= ~LSPF_TIFF); };
		void	SetProcess_Others(BOOL bSet) { bSet ? (m_dwProcessFlags |= LSPF_OTHERS) : (m_dwProcessFlags &= ~LSPF_OTHERS); };

		void	GetEmailSettings(QString& strEmail, QString& strAccount, QString& strSMTP, QString& strObject)
		{
			strEmail = m_strEmail;
			strSMTP = m_strSMTP;
			strObject = m_strObject;
			strAccount = m_strAccount;
		};

		void	GetEmailSettings(QString& strEmail)
		{
			strEmail = m_strEmail;
		};

		void	SetEmailSettings(const QString& email, const QString& account, const QString& SMTP, const QString& Object)
		{
			m_strEmail = email;
			m_strAccount = account;
			m_strSMTP = SMTP;
			m_strObject = Object;
		};

		double	GetScore()
		{
			return (double)m_dwScore / 10.0;
		};

		void	SetScore(double fScore)
		{
			m_dwScore = fScore * 10.0;
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
			m_dwSaveCount = dwSaveCount;
		};

		double	GetFWHM()
		{
			return (double)m_dwFWHM / 10.0;
		};

		void	SetFWHM(double fFWHM)
		{
			m_dwFWHM = fFWHM * 10.0;
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
			return (double)m_dwOffset / 10.0;
		};

		void	SetOffset(double fOffset)
		{
			m_dwOffset = fOffset * 10.0;
		};

		double	GetAngle()
		{
			return (double)m_dwAngle / 10.0;
		};

		void	SetAngle(double fAngle)
		{
			m_dwAngle = fAngle * 10.0;
		};

		void	GetFileFolder(QString& strFileFolder)
		{
			strFileFolder = m_strFileFolder;
		};

		void	SetFileFolder(const QString& fileFolder)
		{
			m_strFileFolder = fileFolder;
		};
	};
}
