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
#include "stdafx.h"
#include <QFileDialog>
#include "DeepSkyStackerLive.h"
#include "LiveSettings.h"
#include "settingstab.h"
#include "emailsettings.h"
#include "tracecontrol.h"

namespace
{
	//
	// Comvert a QLabel with "plain text" to a hyperlink
	//
	static void makeLink(QLabel* label, QString color, QString text)
	{
		label->setText(QString("<a href='.' style='text-decoration: none; color: %1'>%2</a>").arg(color, text));
	}

	static void makeLink(QLabel* label, QString color)
	{
		makeLink(label, color, label->text());
	}
}

extern DSS::TraceControl traceControl;
namespace DSS
{
	SettingsTab::SettingsTab(QWidget* parent)
		: QWidget { parent },
		dirty{ false },
		liveSettings {*(DeepSkyStacker::instance()->liveSettings)},
		minImagesValidator{ new QIntValidator(1, 50, this) },
		scoreValidator {new QDoubleValidator(0.0, 9999.9, 1, this)},
		starCountValidator {new QIntValidator(10, 50, this)},
		skyBGValidator { new QIntValidator(1, 50, this) },
		fwhmValidator { new QDoubleValidator(5.0, 50.0, 1, this) },
		offsetValidator { new QDoubleValidator(5.0, 200.0, 1, this) },
		angleValidator{ new QDoubleValidator(0.0, 360.0, 1, this) },
		imageCountValidator { new QIntValidator(5, 50, this) },
		linkColour{ palette().color(QPalette::ColorRole::Link).name() }

	{
		// Initially invisible - until received initialised signal from liveSettings
		setVisible(false);
		setupUi(this);
		resetEmailCount->setVisible(false);

		connectSignalsToSlots();

		setValidators();

		emailAddress->setToolTip(tr("Click here to set or change the email address", "IDC_EMAIL"));
		warnFileFolder->setToolTip(tr("Click here to select the warning file folder", "IDC_WARNINGFILEFOLDER"));
		stackedOutputFolder->setToolTip(tr("Click here to select the stacked image output folder", "IDC_STACKEDOUTPUTFOLDER"));

		//
		// Populate the controls
		//
		load();

		//
		// Convert the labels we want to be clickable to hyperlinks
		//
		makeLinks();

		//
		// Disable Apply and Cancel buttons (dirty == false)
		//
		updateButtons();

	}

	/* ------------------------------------------------------------------- */

	SettingsTab::~SettingsTab()
	{}



	/* ------------------------------------------------------------------- */

	void SettingsTab::connectSignalsToSlots()
	{
		connect(dontStackUntil, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(minImages, &QLineEdit::textEdited, this, &SettingsTab::settingChanged);

		connect(warnScore, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(dontStackScore, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(score, &QLineEdit::textEdited, this, &SettingsTab::settingChanged);

		connect(warnStars, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(dontStackStars, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(stars, &QLineEdit::textEdited, this, &SettingsTab::settingChanged);

		connect(warnSkyBackground, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(dontStackSkyBackground, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(skyBackground, &QLineEdit::textEdited, this, &SettingsTab::settingChanged);

		connect(warnFWHM, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(dontStackFWHM, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(FWHM, &QLineEdit::textEdited, this, &SettingsTab::settingChanged);

		connect(warnOffset, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(dontStackOffset, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(offset, &QLineEdit::textEdited, this, &SettingsTab::settingChanged);

		connect(warnAngle, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(dontStackAngle, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(angle, &QLineEdit::textEdited, this, &SettingsTab::settingChanged);

		connect(moveNonStackable, &QCheckBox::clicked, this, &SettingsTab::settingChanged);

		connect(warnSound, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(warnEmail, &QCheckBox::clicked, this, &SettingsTab::warnEmail_Clicked);
		connect(emailAddress, &QLabel::linkActivated,
			this, &SettingsTab::setEmailAddress);
		connect(resetEmailCount, &QPushButton::clicked,
			DSSLive::instance(), &DSSLive::resetEmailCount);

		connect(warnFlash, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(warnFile, &QCheckBox::clicked, this, &SettingsTab::warnFile_Clicked);
		connect(warnFileFolder, &QLabel::linkActivated,
			this, &SettingsTab::setWarnFileFolder);

		connect(saveStackedImage, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(imageCount, &QLineEdit::textEdited, this, &SettingsTab::settingChanged);
		connect(stackedOutputFolder, &QLabel::linkActivated,
			this, &SettingsTab::setStackedOutputFolder);
		connect(resetOutputFolder, &QPushButton::pressed, this, &SettingsTab::resetOutputFolder_Pressed);

		connect(retainTraceFile, &QCheckBox::clicked,
			this, &SettingsTab::retainTraceFile_Clicked);

		connect(processRaw, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(processTIFF, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(processFITS, &QCheckBox::clicked, this, &SettingsTab::settingChanged);
		connect(processOther, &QCheckBox::clicked, this, &SettingsTab::settingChanged);


		connect(Apply, &QPushButton::clicked,
			this, &SettingsTab::applyChanges);
		connect(Cancel, &QPushButton::clicked,
			this, &SettingsTab::cancelChanges);
	}

	/* ------------------------------------------------------------------- */

	void SettingsTab::setValidators()
	{
		//
		// Set validators
		//
		score->setValidator(scoreValidator);
		stars->setValidator(starCountValidator);
		skyBackground->setValidator(skyBGValidator);
		FWHM->setValidator(fwhmValidator);
		offset->setValidator(offsetValidator);
		angle->setValidator(angleValidator);
		minImages->setValidator(minImagesValidator);
		imageCount->setValidator(imageCountValidator);
	}

	/* ------------------------------------------------------------------- */

	void SettingsTab::makeLinks()
	{
		makeLink(warnFileFolder, linkColour);
		makeLink(stackedOutputFolder, linkColour);
		makeLink(emailAddress, linkColour);
	}

	/* ------------------------------------------------------------------- */

	void SettingsTab::load()
	{
		dontStackUntil->setChecked(liveSettings.IsDontStack_Until());
		minImages->setText(QString::number(liveSettings.GetMinImages()));

		warnScore->setChecked(liveSettings.IsWarning_Score());
		dontStackScore->setChecked(liveSettings.IsDontStack_Score());
		score->setText(QString::number(liveSettings.GetScore(), 'f', 1));

		warnStars->setChecked(liveSettings.IsWarning_Stars());
		dontStackStars->setChecked(liveSettings.IsDontStack_Stars());
		stars->setText(QString::number(liveSettings.GetStars()));

		warnSkyBackground->setChecked(liveSettings.IsWarning_SkyBackground());
		dontStackSkyBackground->setChecked(liveSettings.IsDontStack_SkyBackground());
		skyBackground->setText(QString::number(liveSettings.GetSkyBackground()));

		warnFWHM->setChecked(liveSettings.IsWarning_FWHM());
		dontStackFWHM->setChecked(liveSettings.IsDontStack_FWHM());
		FWHM->setText(QString::number(liveSettings.GetFWHM(), 'f', 1));

		warnOffset->setChecked(liveSettings.IsWarning_Offset());
		dontStackOffset->setChecked(liveSettings.IsDontStack_Offset());
		offset->setText(QString::number(liveSettings.GetOffset(), 'f', 1));

		warnAngle->setChecked(liveSettings.IsWarning_Angle());
		dontStackAngle->setChecked(liveSettings.IsDontStack_Angle());
		angle->setText(QString::number(liveSettings.GetAngle(), 'f', 1));

		moveNonStackable->setChecked(liveSettings.IsStack_Move());

		warnSound->setChecked(liveSettings.IsWarning_Sound());
		warnEmail->setChecked(liveSettings.IsWarning_Email());

		strEmailAddress = liveSettings.emailAddress();
		if (!strEmailAddress.isEmpty())
		{
			emailAddress->setText(strEmailAddress);
			warnEmail->setEnabled(true);
		}
		else
		{
			emailAddress->setText(tr("Click here to set or change the email address", "IDC_EMAIL"));
			warnEmail->setChecked(false);
			warnEmail->setEnabled(false);
		}

		warnFlash->setChecked(liveSettings.IsWarning_Flash());
		warnFile->setChecked(liveSettings.IsWarning_File());
		strWarnFileFolder = liveSettings.GetWarning_FileFolder();
		if (!strWarnFileFolder.isEmpty())
			warnFileFolder->setText(strWarnFileFolder);
		else
			warnFileFolder->setText(tr("Click here to select the warning file folder", "IDC_WARNINGFILEFOLDER"));

		saveStackedImage->setChecked(liveSettings.IsStack_Save());
		imageCount->setText(QString::number(liveSettings.GetSaveCount()));
		strStackedOutputFolder = liveSettings.GetStackedOutputFolder();
		if (!strStackedOutputFolder.isEmpty())
			stackedOutputFolder->setText(strStackedOutputFolder);
		else
			stackedOutputFolder->setText(tr("Click here to select the stacked image output folder", "IDC_STACKEDOUTPUTFOLDER"));

		QSettings settings;
		auto retainTrace{ settings.value("DeepSkyStackerLive/RetainTraceFile", false).toBool() };
		retainTraceFile->setChecked(retainTrace);

		processRaw->setChecked(liveSettings.IsProcess_RAW());
		processTIFF->setChecked(liveSettings.IsProcess_TIFF());
		processFITS->setChecked(liveSettings.IsProcess_FITS());
		processOther->setChecked(liveSettings.IsProcess_Others());

		dirty = false;
	}

	/* ------------------------------------------------------------------- */

	void SettingsTab::save()
	{
		liveSettings.SetDontStack_Until(Qt::Checked == dontStackUntil->checkState());
		liveSettings.SetMinImages(minImages->text().toInt());

		liveSettings.SetWarning_Score(Qt::Checked == warnScore->checkState());
		liveSettings.SetDontStack_Score(Qt::Checked == dontStackScore->checkState());
		liveSettings.SetScore(score->text().toDouble());

		liveSettings.SetWarning_Stars(Qt::Checked == warnStars->checkState());
		liveSettings.SetDontStack_Stars(Qt::Checked == dontStackStars->checkState());
		liveSettings.SetStars(stars->text().toInt());

		liveSettings.SetWarning_SkyBackground(Qt::Checked == warnSkyBackground->checkState());
		liveSettings.SetDontStack_SkyBackground(Qt::Checked == dontStackSkyBackground->checkState());
		liveSettings.SetSkyBackground(skyBackground->text().toInt());

		liveSettings.SetWarning_FWHM(Qt::Checked == warnFWHM->checkState());
		liveSettings.SetDontStack_FWHM(Qt::Checked == dontStackFWHM->checkState());
		liveSettings.SetFWHM(FWHM->text().toDouble());

		liveSettings.SetWarning_Offset(Qt::Checked == warnOffset->checkState());
		liveSettings.SetDontStack_Offset(Qt::Checked == dontStackOffset->checkState());
		liveSettings.SetOffset(offset->text().toDouble());

		liveSettings.SetWarning_Angle(Qt::Checked == warnAngle->checkState());
		liveSettings.SetDontStack_Angle(Qt::Checked == dontStackAngle->checkState());
		liveSettings.SetAngle(angle->text().toDouble());

		liveSettings.SetStack_Move(Qt::Checked == moveNonStackable->checkState());

		liveSettings.SetWarning_Sound(Qt::Checked == warnSound->checkState());
		liveSettings.SetWarning_Email(Qt::Checked == warnEmail->checkState());
				
		liveSettings.SetWarning_Flash(Qt::Checked == warnFlash->checkState());
		liveSettings.SetWarning_File(Qt::Checked == warnFile->checkState());
		liveSettings.SetWarning_FileFolder(strWarnFileFolder);


		liveSettings.SetStack_Save(Qt::Checked == saveStackedImage->checkState());
		liveSettings.SetSaveCount(imageCount->text().toInt());

		liveSettings.SetStackedOutputFolder(strStackedOutputFolder);

		liveSettings.SetProcess_RAW(Qt::Checked == processRaw->checkState());
		liveSettings.SetProcess_FITS(Qt::Checked == processFITS->checkState());
		liveSettings.SetProcess_TIFF(Qt::Checked == processTIFF->checkState());
		liveSettings.SetProcess_Others(Qt::Checked == processOther->checkState());

		liveSettings.save();
		emit settingsChanged();
		dirty = false;
	}

	void SettingsTab::updateButtons()
	{
		Apply->setEnabled(dirty);
		Cancel->setEnabled(dirty);
	}

	/* ------------------------------------------------------------------- */

	/* ------------------------------------------------------------------- */
	/* Slots                                                               */
	/* ------------------------------------------------------------------- */
	/* ------------------------------------------------------------------- */

	void SettingsTab::setWarnFileFolder([[maybe_unused]] const QString& link)
	{
		QString dir = QFileDialog::getExistingDirectory(this,
			tr("Select Warning File Folder", "IDS_SELECTWARNINGFOLDER"),
			QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first(),
			QFileDialog::ShowDirsOnly
			| QFileDialog::DontResolveSymlinks);
		if (!dir.isEmpty())
		{
			strWarnFileFolder = dir;
			warnFileFolder->setText(dir);

			makeLink(warnFileFolder, linkColour);
			settingChanged();
		}
	}

	/* ------------------------------------------------------------------- */

	void SettingsTab::setStackedOutputFolder([[maybe_unused]] const QString& link)
	{
		QString dir = QFileDialog::getExistingDirectory(this,
			tr("Select Stacked Image Output Folder", "IDS_SELECTSTACKEDFOLDER"),
			QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first(),
			QFileDialog::ShowDirsOnly
			| QFileDialog::DontResolveSymlinks);
		if (!dir.isEmpty())
		{
			strStackedOutputFolder = dir;
			stackedOutputFolder->setText(dir);

			makeLink(stackedOutputFolder, linkColour);
			settingChanged();
		}
	}

	/* ------------------------------------------------------------------- */

	void SettingsTab::setEmailAddress([[maybe_unused]] const QString& link)
	{
		EmailSettings dlg(this);

		if (QDialog::Accepted == dlg.exec())
		{
			strEmailAddress = liveSettings.emailAddress();
			if (!strEmailAddress.isEmpty())
			{
				emailAddress->setText(strEmailAddress);
				warnEmail->setEnabled(true);
			}
			else
			{
				emailAddress->setText(tr("Click here to set or change the email address", "IDC_EMAIL"));
				warnEmail->setChecked(false);
				warnEmail->setEnabled(false);
			}
			makeLink(emailAddress, linkColour);
		}	
	}

	/* ------------------------------------------------------------------- */

	void SettingsTab::settingChanged()
	{
		dirty = true;
		updateButtons();
	}

	void SettingsTab::applyChanges()
	{
		save();
		updateButtons();
	}
	void SettingsTab::cancelChanges()
	{
		load();
		makeLinks();
		updateButtons();
	}
	void SettingsTab::resetOutputFolder_Pressed()
	{
		strStackedOutputFolder = "";
		stackedOutputFolder->setText(tr("Click here to select the stacked image output folder", "IDC_STACKEDOUTPUTFOLDER"));
		makeLink(stackedOutputFolder, linkColour);

		settingChanged();
	}

	void SettingsTab::warnEmail_Clicked(bool checked)
	{
		//
		// If the user un-checks this - reset the warn Email address
		//
		if (!checked)
		{
			strEmailAddress = "";
			emailAddress->setText(tr("Click here to set or change the email address", "IDC_EMAIL"));
			makeLink(emailAddress, linkColour);
		}

		settingChanged();
	}

	void SettingsTab::warnFile_Clicked(bool checked)
	{
		//
		// If the user un-checks this - reset the warn file folder
		//
		if (!checked)
		{
			strWarnFileFolder = "";
			warnFileFolder->setText(tr("Click here to select the warning file folder", "IDC_WARNINGFILEFOLDER"));
			makeLink(warnFileFolder, linkColour);
		}

		settingChanged();
	}

	void SettingsTab::showResetEmailCount()
	{
		resetEmailCount->setVisible(true);
	}

	void SettingsTab::retainTraceFile_Clicked(bool checked)
	{
		QSettings settings;
		settings.beginGroup("DeepSkyStackerLive");
		settings.setValue("RetainTraceFile", checked);
		settings.endGroup();
		traceControl.setDeleteOnExit(!checked);
	}

}
