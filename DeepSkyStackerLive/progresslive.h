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
// ProgressLive.h : Defines the DSS Live Progress class
//
#include <QObject>
#include "DSSProgress.h"

namespace DSS
{

	class ProgressLive : public QObject, public ProgressBase
	{
		Q_OBJECT

	signals:
		void progress(const QString& str, int achieved, int total);
		void endProgress();

	public:
		ProgressLive(QObject* parent = nullptr);
		~ProgressLive();

		//
		// These eight mfs implement the public interface defined in DSS::ProgressBase
		// Unlike DeepSkyStacker and DeepSkyStackerCL they run on whatever thread invokes
		// them.
		// 
		// They or the functions they call emit Qt signals to update the progress
		// information in the DeepSkyStackerLive window.
		//
		virtual void Start1(const QString& szTitle, int lTotal1, bool bEnableCancel = true) override;
		virtual void Progress1(const QString& szText, int lAchieved1) override;
		virtual void Start2(const QString& szText, int lTotal2) override;
		virtual void Progress2(const QString& szText, int lAchieved2) override;
		virtual void End2() override;
		virtual void Close() override;
		virtual bool IsCanceled() const override { return false; }
		virtual bool Warning([[maybe_unused]]const QString& szText) override { return true; };

		virtual const QString& GetStart1Text() const override { return progress1Text; }
		virtual const QString& GetStart2Text() const override { return progress2Text; }
/*
		void setTimeRemaining(const QString& strText);
		void setProgress1Range(int nMin, int nMax);
		void setProgress2Range(int nMin, int nMax);
		void setItemVisibility(bool bSet1, bool bSet2);

		void EnableCancelButton(bool bState);
*/

	protected:
		// ProgressBase
		virtual void applyProcessorsUsed([[maybe_unused]] int nCount) override {};

	private:
		QString progress1Text;
		QString progress2Text;
		int total1;
		int total2;
		int achieved1;
		int achieved2;
		QRecursiveMutex mutex;
	};
}