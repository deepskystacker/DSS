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
// ProgressDlg.h : Defines the DSS Progress Dialog class
//
#include "DSSProgress.h"

namespace Ui {
	class ProgressDlg;
}

namespace DSS
{
	class ProgressDlg : public QDialog, public ProgressBase				
	{
		Q_OBJECT

	private:
		Ui::ProgressDlg* ui;
		bool m_cancelInProgress;
		static inline const QString m_emptyString{};

	public:
		ProgressDlg(QWidget* parent = nullptr);
		~ProgressDlg();

		//
		// These eight mfs implement the public interface defined in DSS::ProgressBase
		// They invoke the corresponding slots using QMetaObject::invokeMethod
		// so that they can be invoked from ANY thread in the application will run on
		// the GUI thread.
		//
		virtual void Start1(const QString& szTitle, int lTotal1, bool bEnableCancel = true) override;
		virtual void Progress1(const QString& szText, int lAchieved1) override;
		virtual void Start2(const QString& szText, int lTotal2) override;
		virtual void Progress2(const QString& szText, int lAchieved2) override;
		virtual void End2() override;
		virtual void Close() override;
		virtual bool IsCanceled() const override { return m_cancelInProgress; }
		virtual bool Warning(const QString& szText) override;


		void setTimeRemaining(const QString& strText);
		void setProgress1Range(int nMin, int nMax);
		void setProgress2Range(int nMin, int nMax);
		void setItemVisibility(bool bSet1, bool bSet2);

		void EnableCancelButton(bool bState);


	protected:
		void applyStart1Text(const QString& strText);
		void applyStart2Text(const QString& strText);
		void applyProgress1(int lAchieved);
		void applyProgress2(int lAchieved);
		void applyTitleText(const QString& strText);
		void initialise();
		void endProgress2();
		void closeProgress();
		bool doWarning(const QString& szText);

		// ProgressBase
		virtual void applyProcessorsUsed(int nCount) override;

	protected slots:
		virtual void slotStart1(const QString& szTitle, int lTotal1, bool bEnableCancel = true);
		virtual void slotProgress1(const QString& szText, int lAchieved1);
		virtual void slotStart2(const QString& szText, int lTotal2);
		virtual void slotProgress2(const QString& szText, int lAchieved2);
		virtual void slotEnd2();
		virtual void slotClose();

	private slots:
		void cancelPressed();

	private:
		void closeEvent(QCloseEvent* bar);
		void retainHiddenWidgetSize(QWidget& rWidget);
	};
}