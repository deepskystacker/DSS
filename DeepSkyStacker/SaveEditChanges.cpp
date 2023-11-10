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
// SaveEditChanges.cpp : implementation file
//

#include "stdafx.h"
#include "SaveEditChanges.h"

namespace DSS
{
	SaveEditChanges::SaveEditChanges(QWidget* parent) :
		QDialog(parent)
	{
		setupUi(this);
		connect(buttonBox, &QDialogButtonBox::clicked, this, &SaveEditChanges::buttonClicked);

		switch (getSaveEditMode())
		{
		case EditSaveMode::SaveDontAsk:
			saveDontAsk->setChecked(true);
			break;
		case EditSaveMode::DiscardDontAsk:
			discardDontAsk->setChecked(true);
			break;
		case EditSaveMode::AskAlways:
			askAlways->setChecked(true);
			break;
		};

	}

	void SaveEditChanges::buttonClicked(QAbstractButton* button)
	{
		result = buttonBox->buttonRole(button);
		if (QDialogButtonBox::RejectRole == result)
			reject();
		else
		{
			saveSettings();
			accept();
		}
	}

	void SaveEditChanges::saveSettings()
	{
		EditSaveMode	Mode = EditSaveMode::AskAlways;

		if (saveDontAsk->isChecked())
			Mode = EditSaveMode::SaveDontAsk;
		else if (discardDontAsk->isChecked())
			Mode = EditSaveMode::DiscardDontAsk;

		setSaveEditMode(Mode);
	};

	EditSaveMode SaveEditChanges::getSaveEditMode()
	{
		QSettings settings;

		uint value = settings.value("EditStars/AutoSave", (uint)0).toUInt();

		return static_cast<EditSaveMode>(value);
	};



}

/* ------------------------------------------------------------------- */

void	setSaveEditMode(EditSaveMode mode)
{
	QSettings settings;
	uint	value = static_cast<uint>(mode);

	settings.setValue("EditStars/AutoSave", value);
};

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

QDialogButtonBox::ButtonRole askToSaveEditChangeMode()
{
	QSettings settings;

	int value = settings.value("EditStars/AutoSave", 0).toInt();

	if (value == static_cast<int>(EditSaveMode::SaveDontAsk))
		return QDialogButtonBox::AcceptRole;
	else if (value == static_cast<int>(EditSaveMode::DiscardDontAsk))
		return QDialogButtonBox::DestructiveRole;
	else
	{
		DSS::SaveEditChanges dlg;

		dlg.exec();

		return dlg.result;

	};
};

/* ------------------------------------------------------------------- */

