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
// SavePicture.cpp : implementation file
//
#include "stdafx.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QVBoxLayout>

#include "SavePicture.h"

namespace DSS
{
	SavePicture::SavePicture(QWidget* parent, const QString& caption, const QString& directory, const QString& filter) :
		QFileDialog(parent, caption, directory, filter),
		compressionGroup{ new QGroupBox(this) },
		compressionLayout{ new QHBoxLayout(compressionGroup) },
		compressionNone{ new QRadioButton(compressionGroup) },
		compressionZIP{new QRadioButton(compressionGroup)},
		compressionLZW{new QRadioButton(compressionGroup)},
		optionsGroup{new QGroupBox(this)},
		optionsLayout{new QVBoxLayout(optionsGroup)},
		applyAdjustments{new QRadioButton(optionsGroup)},
		embedAdjustments{new QRadioButton(optionsGroup)},
		useRectangle{new QCheckBox(optionsGroup)},
		apply_{ false },
		useRect_{false}
	{
		//
		// Only used for saving files ...
		//
		setAcceptMode(QFileDialog::AcceptSave);

		compressionGroup->setObjectName("compressionGroup");
		compressionLayout->setObjectName("compressionLayout");
		compressionNone->setObjectName("compressionNone");
		compressionZIP->setObjectName("compressionZIP");
		compressionLZW->setObjectName("compressionLZW");

		optionsGroup->setObjectName("optionsGroup");
		optionsLayout->setObjectName("optionsLayout");
		applyAdjustments->setObjectName("applyAdjustments");
		embedAdjustments->setObjectName("embedAdjustments");
		useRectangle->setObjectName("useRectangle");

		compressionGroup->setLayout(compressionLayout);
		compressionLayout->addWidget(compressionNone);
		compressionLayout->addWidget(compressionZIP);
		compressionLayout->addWidget(compressionLZW);

		optionsGroup->setLayout(optionsLayout);
		optionsLayout->addWidget(applyAdjustments);
		optionsLayout->addWidget(embedAdjustments);
		optionsLayout->addWidget(useRectangle);

		useRectangle->setEnabled(false);

		retranslateUi(this);

		//
		// Setting DontUseNativeDialog forces Qt to use a Widget based dialogue.
		//
		// This uses a QGridLayout to position the controls, and we can add 
		// additional controls using QGridLayout::addWidget()
		//
		setOption(QFileDialog::DontUseNativeDialog);

		QGridLayout* layout{ dynamic_cast<QGridLayout*>(this->layout()) };
		layout->addWidget(compressionGroup, layout->rowCount(), 0, 1, 2);
		layout->addWidget(optionsGroup, layout->rowCount(), 0, 1, 2);

		connectSignalsToSlots();
	}

	void SavePicture::retranslateUi([[maybe_unused]]QWidget* wdgt)
	{
		compressionGroup->setTitle(tr("Compression", "IDD_SAVEPICTURE"));
		compressionNone->setText(tr("None", "IDC_COMPRESSION_NONE"));
		compressionZIP->setText(tr("ZIP (Deflate)", "IDC_COMPRESSION_ZIP"));
		compressionLZW->setText(tr("LZW (Deprecated)", "IDC_COMPRESSION_LZW"));
		optionsGroup->setTitle(tr("Options", "IDD_SAVEPICTURE"));
		applyAdjustments->setText(tr("Apply adjustments to the saved image", "IDC_APPLIED"));
		applyAdjustments->setToolTip(tr("This option saves the image as you see it\n"
			"Use it if you plan to do further processing in other software",
			"IDS_TT_APPLIED"));
		embedAdjustments->setText(tr("Embed adjustments in the saved image but do not apply them", "IDC_EMBEDDED"));
		embedAdjustments->setToolTip(tr("This option saves the unprocessed image and embeds the processing settings\n"
			"Use it if you plan to do all the processing in other software"
			" or if you plan to re-open the picture with DeepSkyStacker",
			"IDS_TT_EMBEDDED"));
		embedText = embedAdjustments->text();
		noAdjustments = tr("Do not apply adjustments to the saved image", "IDS_SAVENOADJUSTMENT");
		useRectangle->setText(tr("Create an image from the selected rectangle", "IDC_USERECT"));
	}

	void SavePicture::connectSignalsToSlots()
	{
		connect(compressionNone, &QRadioButton::clicked, this, &SavePicture::onCompressionNone);
		connect(compressionZIP, &QRadioButton::clicked, this, &SavePicture::onCompressionZIP);
		connect(compressionLZW, &QRadioButton::clicked, this, &SavePicture::onCompressionLZW);
		connect(applyAdjustments, &QRadioButton::clicked, this, &SavePicture::onApply);
		connect(embedAdjustments, &QRadioButton::clicked, this, &SavePicture::onEmbed);
		connect(useRectangle, &QCheckBox::clicked, this, &SavePicture::onRect);
		connect(this, &QFileDialog::filterSelected, this, &SavePicture::onFilter);
	}

	//
	// Slots
	//
	void SavePicture::onCompressionNone(bool checked)
	{
		if (checked) compression_ = TC_NONE;
	}

	void SavePicture::onCompressionZIP(bool checked)
	{
		if (checked) compression_ = TC_DEFLATE;
	}

	void SavePicture::onCompressionLZW(bool checked)
	{
		if (checked) compression_ = TC_LZW;
	}

	void SavePicture::onApply(bool checked)
	{
		if (checked) apply_ = true;
	}

	void SavePicture::onEmbed(bool checked)
	{
		if (checked) apply_ = false;
	}

	void SavePicture::onRect(bool checked)
	{
		useRect_ = checked;
	}


	void SavePicture::onFilter(const QString& filter)
	{
		auto index{ nameFilters().indexOf(filter) };
		if (index > 2)		// FITS files
		{
			compression_ = TC_NONE;
			compressionNone->setChecked(true);
			compressionZIP->setEnabled(false);
			compressionLZW->setEnabled(false);
			embedAdjustments->setText(noAdjustments);
		}
		else                // TIF files
		{
			compressionZIP->setEnabled(true);
			compressionLZW->setEnabled(true);
			embedAdjustments->setText(embedText);
		}
	}

}
