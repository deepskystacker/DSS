#pragma once
/****************************************************************************
**
** Copyright (C) 2023, 2025 David C. Partridge
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
#include <QElapsedTimer>
#include <QString>
namespace DSS
{
	class OldProgressBase
	{
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

		static inline const QString m_strEmptyString {};
		static constexpr float m_minProgressStep{ 5.0f };

	public:
		OldProgressBase() :
			m_jointProgress{ false },
			m_total1{ 0 },
			m_total2{ 0 },
			m_lastTotal1{ 0 },
			m_lastTotal2{ 0 },
			m_firstProgress{ false },
			m_enableCancel{ false }
		{}

		virtual ~OldProgressBase() = default;

		//
		// These eight mfs define the interface implemented in subclasses
		//
		virtual void Start1(const QString& szTitle, int lTotal1, bool bEnableCancel = true) = 0 ;
		virtual void Progress1(const QString& szText, int lAchieved1) = 0;
		virtual void Start2(const QString& szText, int lTotal2) = 0;
		virtual void Progress2(const QString& szText, int lAchieved2) = 0;
		virtual void End2() = 0;
		virtual void Close() = 0;
		virtual bool IsCanceled() const = 0;
		virtual bool Warning(const QString& szText) = 0;

		void SetJointProgress(bool bJointProgress) { m_jointProgress = bJointProgress; };
		virtual const QString& GetStart1Text() const { return m_strLastOut[OT_TEXT1]; }
		virtual const QString& GetStart2Text() const { return m_strLastOut[OT_TEXT2]; }
		const QString& GetTitleText() const { return m_strLastOut[OT_TITLE]; }
		const QString& GetProgress1Text() const { return m_strLastOut[OT_PROGRESS1]; }
		const QString& GetProgress2Text() const { return m_strLastOut[OT_PROGRESS2]; }

		// Helper functions - when you just want to update the progress and not the text.
		void Start1(int lTotal1, bool bEnableCancel = true) { Start1(m_strEmptyString, lTotal1, bEnableCancel); }
		void Start2(int lTotal2) { Start2(m_strEmptyString, lTotal2); }
		void Progress1(int lAchieved1) { Progress1(m_strEmptyString, lAchieved1); }
		void Progress2(int lAchieved2) { Progress2(m_strEmptyString, lAchieved2); }

	protected:
		void UpdateProcessorsUsed();
		virtual void applyProcessorsUsed(int nCount) = 0;
	};


	//
	// Enum class for mode of Progress reporting (single or dual)
	//
	enum class ProgressMode
	{
		Single, // Single progress bar (default)
		Dual    // Dual progress bar (for two progress bars)
	};

	// 
	// Enum class to say which progress bar we are referring to.
	//
	enum class ProgressBar
	{
		Partial,  // First progress bar for process of indiviual actions
		Total     // Second progress bar for overall progress
	};

	//
	// This is the base class for all progress classes. It defines the interface
	// that all progress classes must implement. It also provides some common
	// functionality that is used by all progress classes.
	// The interface is defined in the pure virtual functions that are
	// implemented in the subclasses. The subclasses are responsible for
	// implementing the actual progress display. The base class provides
	// some common functionality that is used by all progress classes.
	// 

	class ProgressBase : public QObject
	{
		Q_OBJECT
	protected:
		bool firstProgress{ false };
		bool cancelEnabled{ false };
		bool canceled{ false };
		int partialMinimum{ 0 };
		int partialMaximum{ 100 };
		int partialValue{ 0 };
		int totalMininum{ 0 };
		int totalMaximum{ 100 };
		int totalValue{ 0 };	

		QElapsedTimer timer;
		// Properties
		QString topText{};
		QString bottomText{};

		static inline const QString m_strEmptyString{};
		static constexpr float m_minProgressStep{ 5.0f };

		void updateProcessorsUsed();
		virtual void applyProcessorsUsed(int nCount) = 0;

	public:
		ProgressBase() :
			firstProgress{ false },
			cancelEnabled{ false },
			canceled{ false },
			partialMinimum{ 0 },
			partialMaximum{ 100 },
			partialValue{ 0 },
			totalMininum{ 0 },
			totalMaximum{ 100 },
			totalValue{ 0 },
			topText{},
			bottomText{}
			{}

		// Deleted copy and move constructors and assignment operators
		ProgressBase(const ProgressBase&) = delete;
		ProgressBase(ProgressBase&&) = delete;
		ProgressBase& operator=(const ProgressBase&) = delete;
		ProgressBase& operator=(ProgressBase&&) = delete;
		virtual ~ProgressBase() = default;
		virtual bool wasCanceled() const = 0;

	public slots:
		//
		// These twelve mfs define the interface implemented in subclasses
		//
		virtual void setMode(ProgressMode mode) = 0;
		virtual void setTitleText(const QString& title) = 0;
		virtual void setTopText(QString& text) = 0;
		virtual void setPartialMinimum(int minimum) = 0;
		virtual void setPartialMaximum(int maximum) = 0;
		virtual void setPartialRange(int minimum, int maximum) = 0;
		virtual void setPartialValue(int value) = 0;
		virtual void setTotalMinimum(int minimum) = 0;
		virtual void setTotalMaximum(int maximum) = 0;
		virtual void setTotalRange(int minimum, int maximum) = 0;
		virtual void setTotalValue(int value) = 0;
		virtual void setBottomText(QString& text) = 0;
	};
}

