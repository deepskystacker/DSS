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
// DSSProgress.h : Header file
//
#include <QObject>
namespace DSS
{
	class ProgressBase : public QObject
	{
		Q_OBJECT

	protected:
		bool m_jointProgress;
		int m_total1;
		int m_total2;
		int m_lastTotal1;
		int m_lastTotal2;
		bool m_firstProgress;
		bool m_enableCancel;
		QElapsedTimer m_timer;

		enum eOutputType
		{
			OT_TITLE = 0,
			OT_TEXT1,
			OT_TEXT2,
			OT_PROGRESS1,
			OT_PROGRESS2,
			OT_MAX,
		};
		QString m_strLastOut[OT_MAX];

		static const QString m_strEmptyString;
		static constexpr float m_minProgressStep{ 5.0f };

	public:
		ProgressBase(QObject* parent = nullptr) :
			QObject(parent),
			m_jointProgress{ false },
			m_total1{ 0 },
			m_total2{ 0 },
			m_lastTotal1{ 0 },
			m_lastTotal2{ 0 },
			m_firstProgress{ false },
			m_enableCancel{ false }
		{}

		virtual ~ProgressBase() = default;

	public slots:
		virtual void Start1(const QString& szTitle, int lTotal1, bool bEnableCancel = true);
		virtual bool Progress1(const QString& szText, int lAchieved1);
		virtual void Start2(const QString& szText, int lTotal2);
		virtual bool Progress2(const QString& szText, int lAchieved2);
		virtual void End2();
		virtual bool IsCanceled();
		virtual void Close();
		virtual bool Warning(const QString& szText);

		virtual void SetJointProgress(bool bJointProgress) { m_jointProgress = bJointProgress; };
		virtual const QString& GetStart1Text() const { return m_strLastOut[OT_TEXT1]; }
		virtual const QString& GetStart2Text() const { return m_strLastOut[OT_TEXT2]; }
		virtual const QString& GetTitleText() const { return m_strLastOut[OT_TITLE]; }
		virtual const QString& GetProgress1Text() const { return m_strLastOut[OT_PROGRESS1]; }
		virtual const QString& GetProgress2Text() const { return m_strLastOut[OT_PROGRESS2]; }

		// Helper functions - when you just want to update the progress and not the text.
		virtual void Start1(int lTotal1, bool bEnableCancel = true) { Start1(m_strEmptyString, lTotal1, bEnableCancel); }
		virtual void Start2(int lTotal2) { Start2(m_strEmptyString, lTotal2); }
		virtual bool Progress1(int lAchieved1) { return Progress1(m_strEmptyString, lAchieved1); }
		virtual bool Progress2(int lAchieved2) { return Progress2(m_strEmptyString, lAchieved2); }

	protected:
		void UpdateProcessorsUsed();

	protected: // Pure virtual - implementation of the progress bar will fill these in.
		virtual void applyStart1Text(const QString& strText) = 0;
		virtual void applyStart2Text(const QString& strText) = 0;
		virtual void applyProgress1(int lAchieved) = 0;
		virtual void applyProgress2(int lAchieved) = 0;
		virtual void applyTitleText(const QString& strText) = 0;
		virtual void initialise() = 0;
		virtual void endProgress2() = 0;
		virtual bool hasBeenCanceled() = 0;
		virtual void closeProgress() = 0;
		virtual bool doWarning(const QString& szText) = 0;
		virtual void applyProcessorsUsed(int nCount) = 0;
	};
}
