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
#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "ProcessingSettingsDlg.h"

#define dssApp DeepSkyStacker::instance()
namespace DSS {
	ProcessingSettingsDlg::ProcessingSettingsDlg(QWidget* parent, ProcessingSettings ps)
		: BaseDialog(BaseDialog::Behaviour::PersistGeometry, parent),
		currentSettings{ ps },
		dirty{ false }
	{
		ZFUNCTRACE_RUNTIME();
		setupUi(this);

		QStringList parameterSetNames;
		{
			QSettings settings;
			settings.beginGroup("ProcessingSettings");
			parameterSetNames = settings.childGroups();
		}

		//
		// Loop over the named parameter sets
		//
		QString name;		// Parameter set name
		for (qsizetype i = 0; i < parameterSetNames.size(); ++i)
		{
			name = parameterSetNames[i];
			settingsList->addItem(name);
			//
			// This apparently innocent ctor will actually populate the ProcessingSettings 
			// object from the QSettings group called "ProcessingSettings/"name
			//
			ProcessingSettings settings(name);

			settingsMap[name] = settings;
		}

		connectSignalsToSlots();
	}

	void ProcessingSettingsDlg::connectSignalsToSlots()
	{
		connect(addButton, &QPushButton::pressed, this, &ProcessingSettingsDlg::addPressed);
		connect(deleteButton, &QPushButton::pressed, this, &ProcessingSettingsDlg::deletePressed);
		connect(loadButton, &QPushButton::pressed, this, &ProcessingSettingsDlg::loadPressed);
		connect(closeButton, &QPushButton::pressed, this, &ProcessingSettingsDlg::closePressed);

		connect(settingsName, &QLineEdit::textEdited, this, &ProcessingSettingsDlg::nameEdited);
		connect(settingsList, &QListWidget::itemDoubleClicked, this, &ProcessingSettingsDlg::listItemDoubleClicked);
	}


	ProcessingSettingsDlg::~ProcessingSettingsDlg()
	{}

	//
	// Slots
	//
	void ProcessingSettingsDlg::addPressed()
	{
		QString name{ settingsName->text() };

		currentSettings.name_ = name;		// Set name of current settings
		settingsMap.emplace(name, currentSettings);	// Add current settings to the set
		settingsList->addItem(name);		// Add the name to the list of named settings
		dirty = true;
	}

	void ProcessingSettingsDlg::deletePressed()
	{
		QString name{ settingsName->text() };
		settingsMap.erase(name);
		dirty = true;
	}

	void ProcessingSettingsDlg::loadPressed()
	{
		ZFUNCTRACE_RUNTIME();

		QString name = settingsList->currentItem()->text();
		settingsName->setText(name);
		//
		// The next statement could throw a std::out_of_range exception if the item doesn't exist.
		// This would indicate a serious logic error in the code ...
		//
		currentSettings = settingsMap.at(name);
		settingsLoaded = true;
	}

	void ProcessingSettingsDlg::closePressed()
	{
		if (dirty)
		{
			//
			// Loop over all the settings
			// 
			for (auto it = settingsMap.cbegin(); it != settingsMap.cend(); ++it)
			{
				it->second.saveSettings();
			}
		}
		QDialog::accept();
	}

	void ProcessingSettingsDlg::nameEdited(const QString& text)
	{
		if (text.contains(QRegularExpression("[/\\\\]")))
		{
			QApplication::beep();
			QMessageBox::critical(const_cast<ProcessingSettingsDlg*>(this), "DeepSkyStacker",
				tr("The Processing Settings name may not contain either a / or \\.\n"
					"Please change the name."));
		}
	}

	void ProcessingSettingsDlg::listItemDoubleClicked(QListWidgetItem*)
	{
		loadPressed();
	}


} // namespace DSS
