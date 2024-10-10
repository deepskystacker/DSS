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
// StarMaskDlg.cpp : implementation file
//
#include "stdafx.h"
#include "StarMaskDlg.h"

#include "DSSCommon.h"

#define dssApp DeepSkyStacker::instance()
namespace DSS
{
	StarMaskDlg::StarMaskDlg(QWidget* parent, const fs::path& file)
		: BaseDialog(BaseDialog::Behaviour::PersistGeometry, parent),
		imageFile{ file },
		isFits { false }
	{
		setupUi(this);
		QSettings settings;

		//
		// Initialise the combo-box for the output star shape and
		// select the preview image
		//
		QStringList starShapes
		{
			tr("Bell"),
			tr("Truncated Bell"),
			tr("Cone"),
			tr("Truncated Cone"),
			tr("Cubic"),
			tr("Quadric")
		};
		starShape->addItems(starShapes);
		const auto currentShape{ settings.value("StarMask/StarShape", 0).toInt() };
		starShape->setCurrentIndex(currentShape);

		//
		// Now display a preview of the selected star shape
		//
		setStarShapePreview(currentShape);

		//
		// Set the other controls
		//
		uint value = 0;
		value = settings.value("StarMask/DetectionThreshold", 10).toUInt();
		thresholdSlider->setValue(value);
		thresholdChanged(value);

		detectHotPixels->setChecked(settings.value("StarMask/DetectHotPixels", false).toBool());

		value = settings.value("StarMask/MinSize", 2).toUInt();
		minSizeSlider->setValue(value);
		minSizeChanged(value);

		value = settings.value("StarMask/MaxSize", 25).toUInt();
		maxSizeSlider->setValue(value);
		maxSizeChanged(value);

		value = settings.value("StarMask/PercentRadius", 100).toUInt();
		percentSlider->setValue(value);
		percentChanged(value);

		value = settings.value("StarMask/PixelIncrease", 0).toUInt();
		pixelsSlider->setValue(value);
		pixelsChanged(value);
		
		connectSignalsToSlots();
	}

	void StarMaskDlg::connectSignalsToSlots()
	{
		connect(buttonBox, &QDialogButtonBox::accepted, this, &StarMaskDlg::onOK);
		connect(buttonBox, &QDialogButtonBox::rejected, this, &StarMaskDlg::onCancel);
		connect(starShape, &QComboBox::currentIndexChanged, this, &StarMaskDlg::setStarShapePreview);
		connect(thresholdSlider, &QSlider::valueChanged, this, &StarMaskDlg::thresholdChanged);
		connect(minSizeSlider, &QSlider::valueChanged, this, &StarMaskDlg::minSizeChanged);
		connect(maxSizeSlider, &QSlider::valueChanged, this, &StarMaskDlg::maxSizeChanged);
		connect(percentSlider, &QSlider::valueChanged, this, &StarMaskDlg::percentChanged);
		connect(pixelsSlider, &QSlider::valueChanged, this, &StarMaskDlg::pixelsChanged);
	}

	//
	// Slots
	//

	void StarMaskDlg::setStarShapePreview(int index)
	{
		QPixmap pix;
		switch (index)
		{
		case 0:
			pix.load(":/starmask/StarShape_Bell.bmp");
			break;
		case 1:
			pix.load(":/starmask/StarShape_TruncatedBell.bmp");
			break;
		case 2:
			pix.load(":/starmask/StarShape_Cone.bmp");
			break;
		case 3:
			pix.load(":/starmask/StarShape_TruncatedCone.bmp");
			break;
		case 4:
			pix.load(":/starmask/StarShape_Cubic.bmp");
			break;
		case 5:
			pix.load(":/starmask/StarShape_Quadratic.bmp");
			break;
		}
		starShapePreview->setPixmap(pix);
	}

	void StarMaskDlg::thresholdChanged(int value)
	{
		threshold->setText(tr("%1%", "").arg(value));
	}

	void StarMaskDlg::minSizeChanged(int value)
	{
		minSize->setText(tr("%n pixel(s)", "", value));
	}

	void StarMaskDlg::maxSizeChanged(int value)
	{
		maxSize->setText(tr("%n pixel(s)", "", value));
	}

	void StarMaskDlg::percentChanged(int value)
	{
		percent->setText(tr("%1%", "").arg(value));
	}

	void StarMaskDlg::pixelsChanged(int value)
	{
		pixels->setText(tr("%n pixel(s)", "", value));
	}

	void StarMaskDlg::onOK()
	{
		QSettings settings;
		fs::path file;

		if (imageFile.has_parent_path())
			file = imageFile.parent_path();
		else
			file = imageFile.root_path();
		file /= "StarMask";

		// 
		// Set the filetype based on the saved settings value of StarMask/FileType
		// which is a 1 based value.  1 ==> tif, 2 ==> fits.   It is 1 based because
		// the Windows file dialog used a 1 based value for the file type filter.
		//
		auto selectedType{ settings.value("StarMask/FileType", 1).toUInt() };

		switch (selectedType)
		{
		case 1:
			file.replace_extension("tif");
			break;
		case 2:
			file.replace_extension("fts");
			break;
		}

		QString selectedFilter;		// Will be set by QFileDialog::getSaveFileName
		QStringList fileFilters{
			tr("TIFF Image(*.tif *.tiff)", "IDS_FILTER_MASK"),
			tr("FITS Image(*.fits *.fts *.fit)", "IDS_FILTER_MASK")
		};

		//
		// SavePicture is a sub-class of QFileDialog, we'll set the QFileDialog vars first
		//
		QFileDialog dlg{ this, tr("Save the StarMask as ...", "IDS_TITLE_MASK"), QString::fromStdU16String(file.generic_u16string()) };
		dlg.setNameFilters(fileFilters);
		auto filter{ fileFilters.at(selectedType-1) };
		dlg.selectNameFilter(filter);
		dlg.setAcceptMode(QFileDialog::AcceptSave);

		//
		// display the dialogue
		//
		if (QDialog::Accepted == dlg.exec())
		{
			outputFile_ = dlg.selectedFiles().at(0).toStdU16String();
			//
			// For compatability with the old code add one so this is a 1 based index
			// 
			auto index{ fileFilters.indexOf(dlg.selectedNameFilter()) + 1 };

			settings.setValue("StarMask/FileType", (uint)index);
			switch (index)
			{
			case 1:
				isFits = false;
				break;
			case 2: 
				isFits = true;
				break;
			}

			//
			// Save the selected options for use by the Star Mask Engine
			//
			settings.setValue("StarMask/StarShape", starShape->currentIndex());

			settings.setValue("StarMask/DetectionThreshold", thresholdSlider->value());
			
			settings.setValue("StarMask/DetectHotPixels", detectHotPixels->isChecked());

			settings.setValue("StarMask/MinSize", minSizeSlider->value());

			settings.setValue("StarMask/MaxSize", maxSizeSlider->value());

			settings.setValue("StarMask/PercentRadius", percentSlider->value());

			settings.setValue("StarMask/PixelIncrease", pixelsSlider->value());

		}
			

		accept();
	}

	void StarMaskDlg::onCancel()
	{
		reject();
	}
}