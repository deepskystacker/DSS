#pragma once
/****************************************************************************
**
** Copyright (C) 2025 David C. Partridge
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
// ProgressDlg.h : Defines the NEW DSS Progress Dialog class
//
#include "DSSProgress.h"

namespace DSS
{
	namespace Ui {
		class ProgressDlg;
	}

	class ProgressDlg final : public QDialog, public ProgressBase
	{
		Q_OBJECT

	using Inherited = ProgressBase;

	private:
		Ui::ProgressDlg* ui;
		bool m_cancelInProgress;
		static inline const QString m_emptyString{};

	public:
		ProgressDlg(
			QWidget* parent = nullptr,
			ProgressMode mode = ProgressMode::Dual,
			bool enableCancel = true,
			Qt::WindowFlags f = Qt::WindowFlags());
		~ProgressDlg();

		//
		// These eleven mfs implement the public interface defined in DSS::ProgressBase
		// They invoke the corresponding slots using QMetaObject::invokeMethod
		// so that they can be invoked from ANY thread in the application will run on
		// the GUI thread.
		//
		virtual void setTitleText(const QString& title) override;
		virtual void setTopText(QStringView text) override;
		virtual void setPartialMinimum(int minimum) override;
		virtual void setPartialMaximum(int maximum) override;
		virtual void setPartialValue(int value) override;
		virtual void setTotalMinimum(int minimum) override;
		virtual void setTotalMaximum(int maximum) override;
		virtual void setTotalValue(int value) override;
		virtual void setBottomText(QStringView text) override;

		virtual bool wasCanceled() const override;

		void setTimeRemaining(QStringView text);

		void enableCancelButton(bool value);



		void initialise();
		void closeProgress();

		// ProgressBase
		virtual void applyProcessorsUsed(int nCount) override;

	protected slots:
		void slotSetTitleText(QStringView title);
		void slotSetTopText(QStringView text);
		void slotSetPartialMinimum(int minimum);
		void slotSetPartialMaximum(int maximum);
		void slotSetPartialValue(int value);
		void slotSetTotalMinimum(int minimum);
		void slotSetTotalMaximum(int maximum);
		void slotSetTotalValue(int value);
		void slotSetBottomText(QStringView text);

		void slotClose();

	private slots:
		void cancelPressed();

	private:
		void closeEvent(QCloseEvent* bar);
		void retainHiddenWidgetSize(QWidget* widget);
	};
}