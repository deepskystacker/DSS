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
	public:
		LiveSettings();
		~LiveSettings();

		//
		// Don't intend this to be copied or assigned.
		//
		LiveSettings(const LiveSettings&) = delete;
		LiveSettings& operator=(const LiveSettings&) = delete;
		LiveSettings(LiveSettings&& rhs) = delete;
		LiveSettings& operator=(LiveSettings&& rhs) = delete;

		void	load();
		void	save();

		bool	IsDontStack_Score() { return (m_dwStackingFlags & LSSF_SCORE) ? true : false; };
		bool	IsDontStack_Stars() { return (m_dwStackingFlags & LSSF_STARS) ? true : false; };
		bool	IsDontStack_FWHM() { return (m_dwStackingFlags & LSSF_FWHM) ? true : false; };
		bool	IsDontStack_Offset() { return (m_dwStackingFlags & LSSF_OFFSET) ? true : false; };
		bool	IsDontStack_Angle() { return (m_dwStackingFlags & LSSF_ANGLE) ? true : false; };
		bool	IsDontStack_SkyBackground() { return (m_dwStackingFlags & LSSF_SKY) ? true : false; };
		bool	IsDontStack_Until() { return (m_dwStackingFlags & LSSF_DELAYED) ? true : false; };
		bool	IsStack_Save() { return (m_dwStackingFlags & LSSF_SAVE) ? true : false; };
		bool	IsStack_Move() { return (m_dwStackingFlags & LSSF_MOVE) ? true : false; };

		bool	IsWarning_Score() { return (m_dwWarningFlags & LSWF_SCORE) ? true : false; };
		bool	IsWarning_Stars() { return (m_dwWarningFlags & LSWF_STARS) ? true : false; };
		bool	IsWarning_FWHM() { return (m_dwWarningFlags & LSWF_FWHM) ? true : false; };
		bool	IsWarning_Offset() { return (m_dwWarningFlags & LSWF_OFFSET) ? true : false; };
		bool	IsWarning_Angle() { return (m_dwWarningFlags & LSWF_ANGLE) ? true : false; };
		bool	IsWarning_SkyBackground() { return (m_dwWarningFlags & LSWF_SKY) ? true : false; };

		bool	IsWarning_Sound() { return (m_dwWarningActions & LSWA_SOUND) ? true : false; };
		bool	IsWarning_Flash() { return (m_dwWarningActions & LSWA_FLASH) ? true : false; };
		bool	IsWarning_Email() { return (m_dwWarningActions & LSWA_EMAIL) ? true : false; };
		bool	IsWarning_File() { return (m_dwWarningActions & LSWA_FILE) ? true : false; };
		bool	IsWarning_SendMultipleEmails() { return (m_dwWarningActions & LSWA_SENDMULTIPLEEMAILS) ? true : false; };

		bool	IsProcess_RAW() { return (m_dwProcessFlags & LSPF_RAW) ? true : false; };
		bool	IsProcess_FITS() { return (m_dwProcessFlags & LSPF_FITS) ? true : false; };
		bool	IsProcess_TIFF() { return (m_dwProcessFlags & LSPF_TIFF) ? true : false; };
		bool	IsProcess_Others() { return (m_dwProcessFlags & LSPF_OTHERS) ? true : false; };

		void	SetDontStack_Score(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_SCORE) : (m_dwStackingFlags &= ~LSSF_SCORE); };
		void	SetDontStack_Stars(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_STARS) : (m_dwStackingFlags &= ~LSSF_STARS); };
		void	SetDontStack_FWHM(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_FWHM) : (m_dwStackingFlags &= ~LSSF_FWHM); };
		void	SetDontStack_Offset(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_OFFSET) : (m_dwStackingFlags &= ~LSSF_OFFSET); };
		void	SetDontStack_Angle(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_ANGLE) : (m_dwStackingFlags &= ~LSSF_ANGLE); };
		void	SetDontStack_SkyBackground(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_SKY) : (m_dwStackingFlags &= ~LSSF_SKY); };
		void	SetDontStack_Until(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_DELAYED) : (m_dwStackingFlags &= ~LSSF_DELAYED); };
		void	SetStack_Save(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_SAVE) : (m_dwStackingFlags &= ~LSSF_SAVE); };
		void	SetStack_Move(bool bSet) { bSet ? (m_dwStackingFlags |= LSSF_MOVE) : (m_dwStackingFlags &= ~LSSF_MOVE); };

		void	SetWarning_Score(bool bSet) { bSet ? (m_dwWarningFlags |= LSWF_SCORE) : (m_dwWarningFlags &= ~LSWF_SCORE); };
		void	SetWarning_Stars(bool bSet) { bSet ? (m_dwWarningFlags |= LSWF_STARS) : (m_dwWarningFlags &= ~LSWF_STARS); };
		void	SetWarning_FWHM(bool bSet) { bSet ? (m_dwWarningFlags |= LSWF_FWHM) : (m_dwWarningFlags &= ~LSWF_FWHM); };
		void	SetWarning_Offset(bool bSet) { bSet ? (m_dwWarningFlags |= LSWF_OFFSET) : (m_dwWarningFlags &= ~LSWF_OFFSET); };
		void	SetWarning_Angle(bool bSet) { bSet ? (m_dwWarningFlags |= LSWF_ANGLE) : (m_dwWarningFlags &= ~LSWF_ANGLE); };
		void	SetWarning_SkyBackground(bool bSet) { bSet ? (m_dwWarningFlags |= LSWF_SKY) : (m_dwWarningFlags &= ~LSWF_SKY); };

		void	SetWarning_Sound(bool bSet) { bSet ? (m_dwWarningActions |= LSWA_SOUND) : (m_dwWarningActions &= ~LSWA_SOUND); };
		void	SetWarning_Flash(bool bSet) { bSet ? (m_dwWarningActions |= LSWA_FLASH) : (m_dwWarningActions &= ~LSWA_FLASH); };
		void	SetWarning_Email(bool bSet) { bSet ? (m_dwWarningActions |= LSWA_EMAIL) : (m_dwWarningActions &= ~LSWA_EMAIL); };
		void	SetWarning_File(bool bSet) { bSet ? (m_dwWarningActions |= LSWA_FILE) : (m_dwWarningActions &= ~LSWA_FILE); };
		void	SetWarning_SendMultipleEmails(bool bSet) { bSet ? (m_dwWarningActions |= LSWA_SENDMULTIPLEEMAILS) : (m_dwWarningActions &= ~LSWA_SENDMULTIPLEEMAILS); };

		QString	GetWarning_FileFolder() { return m_strWarnFileFolder; };
		void	SetWarning_FileFolder(const QString& folder) { m_strWarnFileFolder = folder; };

		QString	GetStackedOutputFolder() { return m_strStackedOutputFolder; };
		void	SetStackedOutputFolder(const QString& folder) { m_strStackedOutputFolder = folder; };

		void	SetProcess_RAW(bool bSet) { bSet ? (m_dwProcessFlags |= LSPF_RAW) : (m_dwProcessFlags &= ~LSPF_RAW); };
		void	SetProcess_FITS(bool bSet) { bSet ? (m_dwProcessFlags |= LSPF_FITS) : (m_dwProcessFlags &= ~LSPF_FITS); };
		void	SetProcess_TIFF(bool bSet) { bSet ? (m_dwProcessFlags |= LSPF_TIFF) : (m_dwProcessFlags &= ~LSPF_TIFF); };
		void	SetProcess_Others(bool bSet) { bSet ? (m_dwProcessFlags |= LSPF_OTHERS) : (m_dwProcessFlags &= ~LSPF_OTHERS); };

		void getEmailSettings(QString& email, QString& subject, QString& SMTP, int& port, uint& encryption,
			QString& account, QString& pw)
		{
			email = emailTo;
			subject = emailSubject;
			SMTP = smtpServer;
			port = smtpPort;
			encryption = smtpEncryption;
			account = emailAccount;
			pw = emailPassword;
		};

		QString	emailAddress()
		{
			return emailTo;
		}

		void setEmailSettings(const QString& email, const QString& subject, const QString& SMTP, int port, uint encryption,
			const QString& account, const QString& pw)
		{
			emailTo = email;
			emailSubject = subject;
			smtpServer = SMTP;
			smtpPort = port;
			smtpEncryption = encryption;
			emailAccount = account;
			emailPassword = pw;
		};

		double	GetScore()
		{
			return (double)m_dwScore / 10.0;
		};

		void	SetScore(double fScore)
		{
			m_dwScore = fScore * 10.0;
		};

		std::uint32_t	GetStars()
		{
			return (double)m_dwStars;
		};

		void	SetStars(std::uint32_t dwStars)
		{
			m_dwStars = dwStars;
		};

		std::uint32_t	GetMinImages()
		{
			return (double)m_dwMinImages;
		};

		void	SetMinImages(std::uint32_t dwMinImages)
		{
			m_dwMinImages = dwMinImages;
		};

		std::uint32_t GetSaveCount()
		{
			return m_dwSaveCount;
		};

		void SetSaveCount(std::uint32_t count)
		{
			m_dwSaveCount = count;
		};

		double	GetFWHM()
		{
			return (double)m_dwFWHM / 10.0;
		};

		void	SetFWHM(double fFWHM)
		{
			m_dwFWHM = fFWHM * 10.0;
		};

		std::uint32_t	GetSkyBackground()
		{
			return m_dwSkyBackground;
		};

		void	SetSkyBackground(std::uint32_t dwSkyBackground)
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

	private:
		std::uint32_t m_dwStackingFlags;
		std::uint32_t m_dwWarningFlags;
		std::uint32_t m_dwWarningActions;
		std::uint32_t m_dwMinImages;
		std::uint32_t m_dwScore;
		std::uint32_t m_dwStars;
		std::uint32_t m_dwSkyBackground;
		std::uint32_t m_dwFWHM;
		std::uint32_t m_dwOffset;
		std::uint32_t m_dwAngle;
		std::uint32_t m_dwSaveCount;
		std::uint32_t m_dwProcessFlags;
		QString				m_strFileFolder;
		QString				m_strWarnFileFolder;
		QString				m_strStackedOutputFolder;

		//
		// Email configuration stuff
		//
		QString	emailTo;
		QString	emailSubject;
		QString	smtpServer;
		int		smtpPort;
		uint	smtpEncryption;
		QString	emailAccount;
		QString	emailPassword;
	};
}
