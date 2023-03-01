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
// DropFilesDlg.cpp : implementation file
//
#include "stdafx.h"
#include <QDropEvent>
#include <QMimeData>

//#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "DropFilesDlg.h"
#include "ui/ui_DropFilesDlg.h"

//#include "BitmapExt.h"

DropFilesDlg::DropFilesDlg(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DropFilesDlg),
	initialised(false),
	type{ PICTURETYPE_UNKNOWN }
{
	ui->setupUi(this);

}

DropFilesDlg::~DropFilesDlg()
{
	delete ui;
}

bool DropFilesDlg::isMasterFile(const fs::path& path)
{
	bool result = false;
	CBitmapInfo			BitmapInfo;

	if (GetPictureInfo(path.generic_wstring().c_str(), BitmapInfo))
		result = BitmapInfo.IsMaster();

	return result;
}

void DropFilesDlg::onInitDialog()
{
	QSettings settings;
	QString string;
	size_t fileCount{ 0 };
	bool checked = false;
	std::vector<fs::path> masters;

	//
	// Restore Window position etc..
	//
	QByteArray ba = settings.value("Dialogs/DropFilesDlg/geometry").toByteArray();
	if (!ba.isEmpty())
	{
		restoreGeometry(ba);
	}
	else
	{
		//
		// Get main Window rectangle
		//
		const QRect r{ DeepSkyStacker::instance()->rect() };
		QSize size = this->size();

		int top = ((r.top() + (r.height() / 2) - (size.height() / 2)));
		int left = ((r.left() + (r.width() / 2) - (size.width() / 2)));
		move(left, top);
	}

	QList<QUrl> urls = dropEvent->mimeData()->urls();
	for (int i = 0; i != urls.size(); ++i)
	{
		QString name{ urls[i].toLocalFile() };
		fs::path path{ name.toStdU16String() };
		switch (status(path).type())
		{
		case fs::file_type::regular:
			if (isMasterFile(path))
				masters.emplace_back(path);
			else
				files.emplace_back(path);
			break;
		case fs::file_type::directory:
			for (const auto& e : fs::directory_iterator{ path })
			{
				auto& entry{ e.path() };
				if (is_regular_file(entry))
				{
					if (isMasterFile(path))
						masters.emplace_back(path);
					else
						files.emplace_back(path);
				}
			}
			break;
		}
	}

	if (files.empty())
		files = std::move(masters);

	fileCount = files.size();

	ui->dropFiles->setTitle(tr("Add %n file(s) as", "IDC_DROPFILESTEXT", static_cast<int>(fileCount)));
	ui->lightFrames->setText(tr("Light Frames", "IDC_LIGHTFRAMES", static_cast<int>(fileCount)));
	ui->darkFrames->setText(tr("Dark Frames", "IDC_DARKFRAMES", static_cast<int>(fileCount)));
	ui->flatFrames->setText(tr("Flat Frames", "IDC_FLATFRAMES", static_cast<int>(fileCount)));
	ui->darkFlatFrames->setText(tr("Dark Flat Frames", "IDC_DARKFLATFRAMES", static_cast<int>(fileCount)));
	ui->biasFrames->setText(tr("Offset/Bias Frames", "IDC_BIASFRAMES", static_cast<int>(fileCount)));

	ui->lightFrames->setChecked(true);
}


void DropFilesDlg::accept()
{
	QSettings settings;

	settings.setValue("Dialogs/DropFilesDlg/geometry", saveGeometry());

	if (ui->lightFrames->isChecked())
		type = PICTURETYPE_LIGHTFRAME;
	else if (ui->darkFrames->isChecked())
		type = PICTURETYPE_DARKFRAME;
	else if (ui->darkFlatFrames->isChecked())
		type = PICTURETYPE_DARKFLATFRAME;
	else if (ui->flatFrames->isChecked())
		type = PICTURETYPE_FLATFRAME;
	else
		type = PICTURETYPE_OFFSETFRAME;

	Inherited::accept();
}

void DropFilesDlg::showEvent(QShowEvent* event)
{
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitDialog();
		}
	}
	// Invoke base class showEvent()
	return Inherited::showEvent(event);
}

