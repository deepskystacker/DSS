#pragma once
/****************************************************************************
**
** Copyright (C) 2024 David C. Partridge
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
#include <QDialog>
#include "BaseDialog.h"
#include "ProcessingSettings.h"
#include "ui_ProcessingSettingsDlg.h"


namespace DSS {
	class ProcessingSettingsDlg  : public BaseDialog, public Ui::ProcessingSettingsDlg
	{
		Q_OBJECT

	public:
		ProcessingSettingsDlg(QWidget *parent, ProcessingSettings settings);
		~ProcessingSettingsDlg();

		ProcessingSettingsDlg(const ProcessingSettingsDlg& rhs) = delete;

		ProcessingSettingsDlg& operator = (const ProcessingSettingsDlg& rhs) = delete;

		inline bool settingsChanged() const
		{
			return settingsLoaded;
		}

		ProcessingSettings settings()
		{
			return currentSettings;
		}

	private slots:
		void addPressed();
		void deletePressed();
		void loadPressed();
		void closePressed();

		void nameEdited(const QString& text);
		void listItemDoubleClicked(QListWidgetItem*);

	private:
		std::map<QString, ProcessingSettings> settingsMap;
		ProcessingSettings currentSettings;

		void connectSignalsToSlots();

		bool settingsLoaded;
		bool dirty;
	};
} // namespace DSS
