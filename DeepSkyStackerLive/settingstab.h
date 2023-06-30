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

#include "ui_SettingsTab.h"

namespace DSS
{
	class LiveSettings;

	class SettingsTab : public QWidget, public Ui::SettingsTab
	{
		Q_OBJECT

	signals:
		void settingsChanged();

	public:
		SettingsTab(QWidget* parent = nullptr);
		~SettingsTab();

	public slots:
		void showResetEmailCount();

	private:
		bool dirty;
		LiveSettings& liveSettings;
		QIntValidator* minImagesValidator;
		QDoubleValidator* scoreValidator;
		QIntValidator* starCountValidator;
		QIntValidator* skyBGValidator;
		QDoubleValidator* fwhmValidator;
		QDoubleValidator* offsetValidator;
		QDoubleValidator* angleValidator;
		QIntValidator* imageCountValidator;
		QString linkColour;
		QString strEmailAddress;
		QString strWarnFileFolder;
		QString strStackedOutputFolder;

		void connectSignalsToSlots();
		void setValidators();
		void makeLinks();
		void updateButtons();

		void load();
		void save();

	private slots:
		void setEmailAddress(const QString& link);
		void warnEmail_Clicked(bool checked);
		void setWarnFileFolder(const QString& link);
		void warnFile_Clicked(bool checked);
		void setStackedOutputFolder(const QString& link);
		void resetOutputFolder_Pressed();
		void applyChanges();
		void cancelChanges();
		void settingChanged();
		void retainTraceFile_Clicked(bool checked);
	};
}